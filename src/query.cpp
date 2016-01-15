/*
**      SWISH++
**      src/query.cpp
**
**      Copyright (C) 1998-2016  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#include "config.h"
#include "IndexFile.h"
#include "StemWords.h"
#include "exit_codes.h"
#include "file_list.h"
#include "pjl/vlq.h"
#include "query.h"
#include "query_node.h"
#include "stem_word.h"
#include "util.h"
#include "word_util.h"

// standard
#include <cassert>
#include <algorithm>                    /* for binary_search(), etc */
#include <cstdlib>                      /* for exit(3) */
#ifdef DEBUG_parse_query
#include <iostream>
#endif /* DEBUG_parse_query */
#include <vector>

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

typedef and_node::child_node_list and_node_list_type;
typedef query_node::pool_type node_pool_type;

namespace {

/**
 * Bundles all the 'q'uery-wide arguments to the various \c parse_*()
 * functions.
 */
struct parse_q_args {
  node_pool_type& node_pool;
  token_stream&   query;
  stop_word_set&  stop_words_found;
#ifdef WITH_WORD_POS
  bool            got_near;
#endif /* WITH_WORD_POS */

  parse_q_args( node_pool_type &p, token_stream &q, stop_word_set &s ) :
    node_pool( p ), query( q ), stop_words_found( s )
#ifdef WITH_WORD_POS
    , got_near( false )
#endif /* WITH_WORD_POS */
  {
  }

private:
  parse_q_args( parse_q_args const& );            // forbid copy
  parse_q_args& operator=( parse_q_args const& ); // forbid assignment
};

/**
 * Bundles all the 'r'eference arguments to the various \c parse_*() functions.
 */
struct parse_r_args {
  and_node_list_type  and_nodes;
  bool                ignore;
  query_node*         node;

  parse_r_args() { }

private:
  parse_r_args( parse_r_args const& );            // forbid copy
  parse_r_args& operator=( parse_r_args const& ); // forbid assignment
};

/**
 * Bundles all the 'v'alue arguments to the various \c parse_*() functions.
 */
struct parse_v_args {
  meta_id_type meta_id;

  parse_v_args() : meta_id( Meta_ID_None ) { }
  // default copy constructor is OK

private:
  parse_v_args& operator=( parse_v_args const& ); // forbid assignment
};

} // namespace

extern index_segment files, meta_names, stop_words, words;

// local functions
static void assert_index_has_word_pos_data();
static bool parse_meta   ( parse_q_args&, parse_r_args&, parse_v_args );
static bool parse_primary( parse_q_args&, parse_r_args&, parse_v_args );
static bool parse_query2 ( parse_q_args&, parse_r_args&, parse_v_args );
static bool parse_relop  ( token_stream&, token::type& );

////////// extern functions ///////////////////////////////////////////////////

/**
 * Parses a query.  This is merely a front-end for \c parse_query2(), but has a
 * less ugly API.
 *
 * @param query The token_stream whence the query string is extracted.
 * @param results The query results go here.
 * @param stop_words_found The set of stop-words in the query, if any.
 * @return Returns \c true only if a query was successfully parsed.
 */
bool parse_query( token_stream &query, search_results &results,
                  stop_word_set &stop_words_found ) {
  node_pool_type node_pool;

  parse_q_args q_args( node_pool, query, stop_words_found );
  parse_r_args r_args;
  parse_v_args v_args;
  if ( !parse_query2( q_args, r_args, v_args ) )
    return false;

#ifdef WITH_WORD_POS
  if ( q_args.got_near ) {
    assert_index_has_word_pos_data();
    //
    // We got a "near" somewhere in the query: walk the tree and distirbute
    // the terms of all the near nodes.
    //
    near_node::distributor const d( 0 );
    r_args.node = r_args.node->visit( d );
#   ifdef DEBUG_eval_query
    r_args.node->print( cerr );
    cerr << endl;
#   endif
  }
#endif /* WITH_WORD_POS */
  r_args.node->eval( results );
  return true;
}

////////// local functions ////////////////////////////////////////////////////

/**
 * For a given meta name, return its numeric ID that words in the index refer
 * to.
 * @param i An iterator positioned at a meta name.
 * @return Returns the numeric ID of the meta name in the range [0,N).
 */
static meta_id_type get_meta_id( index_segment::const_iterator i ) {
  auto p = reinterpret_cast<unsigned char const*>( *i );
  while ( *p++ ) ;                      // skip past word
  return static_cast<meta_id_type>( vlq::decode( p ) );
}

#ifdef WITH_WORD_POS
/**
 * The current query has a "near" in it: check that the current index has
 * word-position data stored in order to evaluate the "near".  If it doesn't,
 * complain.
 */
static void assert_index_has_word_pos_data() {
  //
  // A simple way to check that the current index has word-position data stored
  // is to get the file_list for the first word in the index then look to see
  // if pos_delta_ is empty: if it is, no word-position data was stored.
  //
  file_list const list( words.begin() );
  auto const file( list.begin() );
  if ( file->pos_deltas_.empty() ) {
    extern IndexFile index_file_name;
    error() << '"' << index_file_name
            << "\" does not contain word position data"
            << endl;
    ::exit( Exit_No_Word_Pos_Data );
  }
}
#endif /* WITH_WORD_POS */

/**
 * Parses a query via predictive top-down recursive decent.  The grammar for a
 * query is:
 * \code
 *    query:      query relop meta
 *            |   meta
 *
 *    meta:       meta_name = primary
 *            |   primary
 *
 *    meta_name:  word
 *
 *    primary:    '(' query ')'
 *            |   'not' meta
 *            |   word
 *            |   word*
 *
 *    relop:      'and'
 *            |   'near'
 *            |   'not' 'near'
 *            |   'or'
 *            |   (empty)
 * \endcode
 * However, the \c query production is left-recursive which doesn't work for
 * top-down parsers.  It must therefore be rewritten to be right-recursive as:
 * \code
 *    query:      meta rest
 *
 *    rest:       relop meta rest
 *            |   (empty)
 * \endcode
 * See also:
 *    Alfred V. Aho, Ravi Sethi, Jeffrey D. Ullman.  "Compilers: Principles,
 *    Techniques, and Tools," Addison-Wesley, Reading, MA, 1986, pp. 44-48.
 *
 * @param q_args The query-wide arguments.
 * @param r_args The query reference arguments.
 * @param v_args The query value arguments.
 * @return Returns \c true only if a query was successfully parsed.
 */
static bool parse_query2( parse_q_args &q_args, parse_r_args& r_args,
                          parse_v_args v_args ) {
  if ( !parse_meta( q_args, r_args, v_args ) )
    return false;

  //
  // This is parse_rest placed inside parse_query since every "primary" is
  // followed by a "rest" in the grammar.
  //
  token::type relop;
  while ( parse_relop( q_args.query, relop ) ) {
    parse_r_args r_args_rhs;
    parse_v_args v_args_rhs( v_args );
    if ( !parse_meta( q_args, r_args_rhs, v_args_rhs ) )
      return false;
    if ( r_args.ignore ) {
      if ( !r_args_rhs.ignore ) {       // results are simply the RHS
        r_args.node = r_args_rhs.node;
        r_args.ignore = false;
      }
      continue;
    }
    if ( r_args_rhs.ignore )            // results are simply the LHS
      continue;

    switch ( relop ) {

      case token::tt_and:
        //
        // Defer performing the "and" until later so that all the "and"s at the
        // same level can be performed together.
        //
        r_args.and_nodes.push_back( r_args_rhs.node );
        break;

#ifdef WITH_WORD_POS
      case token::tt_near:
      case token::tt_not_near: {
        //
        // Ensure that neither child node of the near_node we want to create is
        // a not_node since it's nonsensical.
        //
        if ( dynamic_cast<not_node*>( r_args_rhs.node ) )
          return false;
        query_node *const lhs_node = r_args.and_nodes.empty() ?
          r_args.node :
          new and_node( q_args.node_pool, r_args.and_nodes );
        if ( dynamic_cast<not_node*>( lhs_node ) )
          return false;
        //
        // If both child nodes have meta IDs specified, then they must be
        // equal; if not, the results must be empty.
        //
        if ( v_args.    meta_id != Meta_ID_None &&
             v_args_rhs.meta_id != Meta_ID_None &&
             v_args.    meta_id != v_args_rhs.meta_id ) {
          r_args.node = new empty_node;
          break;
        }
        q_args.got_near = true;
        r_args.node = relop == token::tt_not_near ?
          new not_near_node( q_args.node_pool, lhs_node, r_args_rhs.node ) :
          new near_node( q_args.node_pool, lhs_node, r_args_rhs.node );
        break;
      }
#endif /* WITH_WORD_POS */

      case token::tt_or:
        r_args.node = new or_node(
          q_args.node_pool,
          r_args.and_nodes.empty() ?
            r_args.node :
            new and_node( q_args.node_pool, r_args.and_nodes ),
          r_args_rhs.node
        );
        break;

      default:
        //
        // We should never get anything other than a tt_and, tt_near, or a
        // tt_or.  If we get here, the programmer goofed.
        //
        internal_error << "parse_query2(): unexpected token" << report_error;
    } // switch
  } // while

  if ( !r_args.and_nodes.empty() ) {
    r_args.and_nodes.push_back( r_args.node );
    r_args.node = new and_node( q_args.node_pool, r_args.and_nodes );
  }
  return true;
}

/**
 * Parses a meta name from the query using look-ahead.
 *
 * @param q_args The query-wide arguments.
 * @param r_args The query reference arguments.
 * @param v_args The query value arguments.
 * @return Returns \c true only if a query was successfully parsed.
 */
static bool parse_meta( parse_q_args& q_args, parse_r_args& r_args,
                        parse_v_args v_args ) {
  token const t( q_args.query );
  if ( t == token::tt_word ) {          // meta name ...
    token const t2( q_args.query );
    if ( t2 == token::tt_equal ) {      // ... followed by '='
      less<char const*> const comparator;
      word_range const range = ::equal_range(
        meta_names.begin(), meta_names.end(), t.lower_str(), comparator
      );
      v_args.meta_id =
        range.first != meta_names.end() &&
        !comparator( t.lower_str(), *range.first )
      ?
        get_meta_id( range.first ) : Meta_ID_Not_Found;
      goto parsed_meta_id;
    }
    q_args.query.put_back( t2 );
  }
  q_args.query.put_back( t );

parsed_meta_id:
  return parse_primary( q_args, r_args, v_args );
}

/**
 * Parses an optional relational operator of "and", "near", or "or" from the
 * given token_stream.  In the absense of a relational operator, "and" is
 * implied.
 *
 * @param query The token_stream whence the relational operator string is
 * extracted (if present).
 * @param relop Where the type of the relational operator is deposited.
 * @return Returns \c true unless no token at all could be parsed.
 */
static bool parse_relop( token_stream &query, token::type &relop ) {
  token const t( query );
  token::type t_type = t;
  switch ( t_type ) {

    case token::tt_none:
      return false;

#ifdef WITH_WORD_POS
    case token::tt_not: {
      token const t2( query );
      if ( t2 != token::tt_near ) {
        query.put_back( t2 );
        break;
      }
      t_type = token::tt_not_near;
    }
    case token::tt_near:
#endif /* WITH_WORD_POS */
    case token::tt_and:
    case token::tt_or:
#     ifdef DEBUG_parse_query
      cerr << "---> relop \"";
      switch ( t_type ) {
        case token::tt_and:
          cerr << "and";
          break;
#ifdef WITH_WORD_POS
        case token::tt_not_near:
          cerr << "not ";
          // no break;
        case token::tt_near:
          cerr << "near";
          break;
#endif /* WITH_WORD_POS */
        case token::tt_or:
          cerr << "or";
          break;
      } // switch
      cerr << "\"\n";
#     endif /* DEBUG_parse_query */
      relop = t_type;
      return true;

    default:
      /* do nothing */;
  } // switch
  query.put_back( t );
  if ( t == token::tt_rparen )
    return false;
# ifdef DEBUG_parse_query
  cerr << "---> relop \"and\" (implicit)\n";
# endif /* DEBUG_parse_query */
  relop = token::tt_and;
  return true;
}

/**
 * Parses a "primary" from the given token_stream.
 *
 * @param q_args The query-wide arguments.
 * @param r_args The query reference arguments.
 * @param v_args The query value arguments.
 * @return Returns \c true only if a primary was successfully parsed.
 */
static bool parse_primary( parse_q_args& q_args, parse_r_args& r_args,
                           parse_v_args v_args ) {
  r_args.ignore = false;
  r_args.node = new empty_node;
  word_range range;
  token t( q_args.query );

  switch ( t ) {

    case token::tt_word: {
      less_stem const comparator( stem_words );
      //
      // First check to see if the word wasn't indexed either because it's not
      // an "OK" word according to the heuristics employed or because it's a
      // stop-word.
      //
      if ( !is_ok_word( t.str() ) ||
        ::binary_search(
          stop_words.begin(), stop_words.end(), t.lower_str(), comparator
        )
      ) {
        q_args.stop_words_found.insert( t.str() );
#       ifdef DEBUG_parse_query
        cerr << "---> word \"" << t.str() << "\" (ignored: not OK)\n";
#       endif /* DEBUG_parse_query */
        //
        // The following "return true" indicates that a word was parsed
        // successfully, not that we found the word.
        //
        return r_args.ignore = true;
      }
      //
      // Look up the word.
      //
      range =
        ::equal_range( words.begin(), words.end(), t.lower_str(), comparator );
      if ( range.first == words.end() ||
           comparator( t.lower_str(), *range.first ) ) {
        //
        // The following "return true" indicates that a word was parsed
        // successfully, not that we found the word.
        //
        return true;
      }
      break;
    }

    case token::tt_word_star: {
      less_n<char const*> const comparator( t.length() );
      //
      // Look up all matching words.
      //
      range =
        ::equal_range( words.begin(), words.end(), t.lower_str(), comparator );
      if ( range.first == words.end() ||
           comparator( t.lower_str(), *range.first ) ) {
        //
        // The following "return true" indicates that a word was parsed
        // successfully, not that we found the word.
        //
        return true;
      }
      break;
    }

    case token::tt_lparen:
#     ifdef DEBUG_parse_query
      cerr << "---> '('\n";
#     endif /* DEBUG_parse_query */
      if ( !parse_query2( q_args, r_args, v_args ) )
        return false;
      q_args.query >> t;
#     ifdef DEBUG_parse_query
      if ( t == token::tt_rparen )
        cerr << "---> ')'\n";
#     endif /* DEBUG_parse_query */
      return t == token::tt_rparen;

    case token::tt_not: {
#     ifdef DEBUG_parse_query
      cerr << "---> begin not\n";
#     endif /* DEBUG_parse_query */
      parse_r_args r_temp;
      if ( !parse_meta( q_args, r_temp, v_args ) )
        return false;
#     ifdef DEBUG_parse_query
      cerr << "---> end not\n";
#     endif /* DEBUG_parse_query */
      if ( r_temp.node )
        r_args.node = new not_node( q_args.node_pool, r_temp.node );
      return true;
    }

    default:
      return false;
  } // switch

# ifdef DEBUG_parse_query
  cerr << "---> word \"" << t.str() << "\", meta-ID=" << v_args.meta_id << "\n";
# endif /* DEBUG_parse_query */
  //
  // Found a word or set of words matching a wildcard: iterate over all files
  // the word(s) is/are in and add their ranks together, but only if the
  // meta-names match (if any).
  //
  // Also start off assuming that this (sub)query should be ignored until we
  // get at least one word that isn't too frequent.
  //
  r_args.ignore = true;
  FOR_EACH_IN_PAIR( range, i ) {
    file_list const list( i );
    if ( is_too_frequent( list.size() ) ) {
      q_args.stop_words_found.insert( t.lower_str() );
#     ifdef DEBUG_parse_query
      cerr << "---> word \"" << t.str() << "\" (ignored: too frequent)\n";
#     endif /* DEBUG_parse_query */
    } else
      r_args.ignore = false;
  } // for

  if ( !r_args.ignore ) {
    r_args.node =
      new word_node( q_args.node_pool, t.str(), range, v_args.meta_id );
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
