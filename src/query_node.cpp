/*
**      SWISH++
**      src/query_node.cpp
**
**      Copyright (C) 2004-2015  Paul J. Lucas
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
#include "file_list.h"
#include "index_segment.h"
#include "query_node.h"
#include "WordsNear.h"
#include "util.h"

// standard
#include <iostream>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

empty_node empty_node::singleton_;

void empty_node::eval( search_results& ) {
  // Out-of-line because it's virtual.
}

////////// constructors ///////////////////////////////////////////////////////

and_node::and_node( pool_type &p, child_node_list &nodes ) : query_node( p ) {
  child_nodes_.swap( nodes );
}

#ifdef WITH_WORD_POS
near_node::near_node( pool_type &p, query_node *left, query_node *right ) :
  query_node( p ), left_child_ ( left  ), right_child_( right )
{
  // do nothing else
}
#endif /* WITH_WORD_POS */

word_node::word_node( pool_type &p, char const *word, word_range const &range,
                      meta_id_type meta_id ) :
  query_node( p ), word_( new_strdup( word ) ), range_( range ),
  meta_id_( meta_id )
{
  // do nothing else
}

////////// destructors ////////////////////////////////////////////////////////

and_node::~and_node() {
  //
  // We don't delete our child nodes because all nodes are deleted by the pool.
  //
}

#ifdef WITH_WORD_POS
near_node     ::~near_node    () { /* See comment in ~and_node(). */ }
not_near_node ::~not_near_node() { /* See comment in ~and_node(). */ }
#endif /* WITH_WORD_POS */
not_node      ::~not_node     () { /* See comment in ~and_node(). */ }
or_node       ::~or_node      () { /* See comment in ~and_node(). */ }
query_node    ::~query_node   () { /* Out-of-line because it's virtual. */ }
word_node     ::~word_node    () { delete[] word_; }

query_node::visitor::~visitor() {
  // Out-of-line because it's virtual.
}

////////// visitors ///////////////////////////////////////////////////////////

query_node* and_node::visit( visitor const &v ) {
  query_node *const result = v( this );
  if ( result == this )
    for ( auto const &child : child_nodes_ )
      child->visit( v );
  return result;
}

#ifdef WITH_WORD_POS
query_node* near_node::visit( visitor const &v ) {
  query_node *const result = v( this );
  if ( result == this ) {
    left() ->visit( v );
    right()->visit( v );
  }
  return result;
}
#endif /* WITH_WORD_POS */

query_node* not_node::visit( visitor const &v ) {
  query_node *const result = v( this );
  if ( result == this )
    child_->visit( v );
  return result;
}

query_node* or_node::visit( visitor const &v ) {
  query_node *const result = v( this );
  if ( result == this ) {
    left() ->visit( v );
    right()->visit( v );
  }
  return result;
}

query_node* query_node::visit( visitor const &v ) {
  return v( this );
}

void and_node::eval( search_results &results ) {
  //
  // Evaluate the search results for "and" by evaluating all of its child nodes
  // at the same time.  This is done to solve the weighting problem with more
  // than two "and" terms.  For example, the query:
  //
  //    mouse and computer and keyboard
  //
  // is parsed and treated as:
  //
  //    (mouse and computer) and keyboard
  //      25%        25%           50%
  //
  // The problem is that the last term always gets 50% of the weighting.
  //
  // In order to weight all the terms equally, the "and" results for each term
  // are saved in a list and then and'ed together at the end.
  //
  search_results const empty_place_holder;
  typedef vector<search_results> child_results_type;
  child_results_type child_results;
  child_results.reserve( child_nodes_.size() );

  for ( auto const &child_node : child_nodes_ ) {
    search_results results;
    child_node->eval( results );
    if ( results.empty() ) {
      //
      // Since we're evaluating an "and", we can stop immediately if any of the
      // child nodes return no results.
      //
      return;
    }
    //
    // Temporarily push an empty "place-holder" search_results onto the back of
    // the vector to increase its size without copying the actual potentially
    // large search_results.  Then simply swap it with the actual results in
    // O(1) time.
    //
    // Yes, a vector of pointers to search_results could have been used
    // instead, but then we'd have to make sure to delete all the
    // search_results when done in two places: above at the "return" and below
    // when this function "falls out the bottom."
    //
    child_results.push_back( empty_place_holder );
    child_results.back().swap( results );
  } // for

  //
  // Pluck out one of the child results and make it *the* result so far.  (It's
  // easiest to use the last one.  Since it's "and", which one we use doesn't
  // matter.)
  //
  results.swap( child_results.back() );
  child_results.pop_back();

  //
  // For each search result, see if it's in each child_result: if it is, sum
  // the ranks; if it isn't, delete the result.
  //
  for ( auto result = results.begin(); result != results.end(); ) {
    bool increment_result_iterator = true;
    for ( auto const &child_result : child_results ) {
      auto const found = child_result.find( result->first );
      if ( found != child_result.end() ) {
        result->second += found->second;
        continue;
      }
      //
      // Erasing an element at a given iterator invalidates the iterator: copy
      // the iterator to a temporary and increment the real iterator off of the
      // element to be erased so the real iterator won't be invalidated.
      //
      auto const erase_me = result++;
      results.erase( erase_me );
      increment_result_iterator = false;
      break;
    } // for
    if ( increment_result_iterator )
      ++result;
  } // for

  //
  // Now that the and-results have been summed, divide each by the number of
  // and-results, i.e., average them.  (It's +1 below because you have to
  // include the "results" variable itself.)
  //
  int const num_ands = child_nodes_.size() + 1;
  for ( auto &result : results )
    result.second /= num_ands;
}

#ifdef WITH_WORD_POS
void near_node::eval( search_results &results ) {
  word_node const *const node[] = {
    dynamic_cast<word_node*>( left()  ),
    dynamic_cast<word_node*>( right() )
  };
  if ( !node[0] || !node[1] )
    return;

  if ( node[0]->meta_id() != Meta_ID_None &&
       node[1]->meta_id() != Meta_ID_None &&
       node[0]->meta_id() != node[1]->meta_id() )
    return;

  FOR_EACH_IN_PAIR( node[0]->range(), word0 ) {
    file_list const list0( word0  );
    if ( is_too_frequent( list0.size() ) )
      continue;
    FOR_EACH_IN_PAIR( node[1]->range(), word1 ) {
      file_list const list1( word1 );
      if ( is_too_frequent( list1.size() ) )
        continue;

      file_list::const_iterator file[] = { list0.begin(), list1.begin() };
      while ( file[0] != list0.end() && file[1] != list1.end() ) {

        ////////// Words in same file with right meta ID? /////////////////////

        if ( file[0]->index_ < file[1]->index_ ||
            !file[0]->has_meta_id( node[0]->meta_id() ) ) {
          ++file[0];
          continue;
        }
        if ( file[0]->index_ > file[1]->index_ ||
            !file[1]->has_meta_id( node[1]->meta_id() ) ) {
          ++file[1];
          continue;
        }

        ////////// Are words near each other? /////////////////////////////////

        unsigned pdi[2];                // pos_deltas_[i] index
        unsigned pos[2];                // absolute position for file[i]
        for ( int i = 0; i < 2; ++i ) {
          pdi[i] = 0;
          pos[i] = file[i]->pos_deltas_[0];
        } // for

        while ( true ) {
          //
          // Two words are near each other only if their absolute positions
          // differ by at most words_near.
          //
          int const delta = pos[1] - pos[0];
          if ( pjl_abs( delta ) <= words_near ) {
            results[ file[0]->index_ ] = (file[0]->rank_ + file[1]->rank_) / 2;
            break;
          }
          //
          // Increment the ith file's pos_deltas_ index and add the next delta
          // to the accumulated absolute position.
          //
          int const i = delta < 1;
          if ( ++pdi[i] >= file[i]->pos_deltas_.size() )
            break;
          pos[i] += file[i]->pos_deltas_[ pdi[i] ];
        } // while

        ++file[0], ++file[1];
      } // while
    } // for
  } // for
}

query_node* near_node::distribute() {
  bool const this_is_not_near = dynamic_cast<not_near_node*>( this );
  bool const left_is_word     = dynamic_cast<word_node*>( left()  );
  bool const right_is_word    = dynamic_cast<word_node*>( right() );

  if ( !right_is_word ) {
    distributor const dl( left(), this_is_not_near );
    query_node *const temp = right()->visit( dl );
    if ( left_is_word )
      return temp;
    near_node::distributor const d( 0, this_is_not_near );
    return temp->visit( d );
  }
  if ( !left_is_word ) {
    distributor const dr( right(), this_is_not_near );
    return left()->visit( dr );
  }
  return this;
}

query_node* near_node::distributor::operator()( query_node *node ) const {
  if ( near_node *const n = dynamic_cast<near_node*>( node ) )
    return n->distribute();
  if ( !other_ )
    return node;

  distributor const d( 0, not_near_ );
  if ( and_node *const a = dynamic_cast<and_node*>( node ) ) {
    and_node::child_node_list new_child_nodes;
    for ( auto const &child : *a ) {
      near_node *const new_child = make_node(
        *node->pool(), other_, child->visit( d )
      );
      new_child_nodes.push_back( new_child->distribute() );
    } // for
    return new and_node( *node->pool(), new_child_nodes );
  }
  if ( or_node *const o = dynamic_cast<or_node*>( node ) ) {
    near_node *const new_left = make_node(
      *node->pool(), other_, o->left()->visit( d )
    );
    near_node *const new_right = make_node(
      *node->pool(), other_, o->right()->visit( d )
    );
    return new or_node(
      *node->pool(), new_left->distribute(), new_right->distribute()
    );
  }
  if ( dynamic_cast<not_node*>( node ) )
    internal_error
        << "near_node::distributor::operator()(): "
            "encountered not_node" << report_error;
  return node;
}

void not_near_node::eval( search_results &results ) {
  //
  // Evaluates the search results for "not near".  This code is very similar to
  // that for near_node::eval().  The difference is that the right-hand word
  // can either be "not near" the left-hand word -OR- not present in the same
  // file at all.
  //
  word_node const *const node[] = {
    dynamic_cast<word_node*>( left()  ),
    dynamic_cast<word_node*>( right() )
  };
  if ( !node[0] )
    return;

  FOR_EACH_IN_PAIR( node[0]->range(), word0 ) {
    file_list const list0( word0 );
    if ( is_too_frequent( list0.size() ) )
      continue;
    if ( !node[1] ) {
      //
      // If the right-hand side node isn't a word_node (i.e., it's an
      // empty_node), then this case degenerates into doing the same thing that
      // word_node::eval() does.
      //
      for ( auto const &file : list0 )
        if ( file.has_meta_id( node[0]->meta_id() ) )
          results[ file.index_ ] += file.rank_;
      continue;
    }

    ////////// Must check for "near"-ness of right-hand side word /////////////

    FOR_EACH_IN_PAIR( node[1]->range(), word1 ) {
      file_list const list1( word1 );
      file_list::const_iterator file[] = { list0.begin(), list1.begin() };
      while ( file[0] != list0.end() ) {
        if ( file[0]->has_meta_id( node[0]->meta_id() ) ) {
          //
          // Make file[1]'s index "catch up" to file[0]'s.
          //
          while ( file[1] != list1.end() && file[1]->index_ < file[0]->index_ )
            ++file[1];

          ////////// Are words in the same file? //////////////////////////////

          if ( file[1] != list1.end() &&
               file[0]->index_ == file[1]->index_ &&
               file[1]->has_meta_id( node[1]->meta_id() ) ) {

            ////////// Are words near each other? /////////////////////////////

            unsigned pdi[2];            // pos_deltas_[i] index
            unsigned pos[2];            // absolute position for file[i]
            for ( int i = 0; i < 2; ++i ) {
              pdi[i] = 0;
              pos[i] = file[i]->pos_deltas_[0];
            } // for

            while ( true ) {
              //
              // Two words are near each other only if their absolute positions
              // differ by at most words_near.
              //
              int const delta = pos[1] - pos[0];
              if ( pjl_abs( delta ) <= words_near )
                goto found_near;
              //
              // Increment the ith file's pos_deltas_ index and add the next
              // delta to the accumulated absolute position.
              //
              int const i = delta < 1;
              if ( ++pdi[i] >= file[i]->pos_deltas_.size() )
                break;
              pos[i] += file[i]->pos_deltas_[ pdi[i] ];
            } // while
          }
          results[ file[0]->index_ ] += file[0]->rank_;
        }
found_near:
        ++file[0];
        if ( file[1] != list1.end() )
          ++file[1];
      } // while
    } // for
  } // for
}
#endif /* WITH_WORD_POS */

void not_node::eval( search_results &results ) {
  extern index_segment files;
  search_results child_results;
  child_->eval( child_results );

  for ( size_t i = 0; i < files.size(); ++i )
    if ( child_results.find( i ) == child_results.end() )
      results[i] = 100;
}

void or_node::eval( search_results &left_results ) {
  search_results right_results;

  left() ->eval( left_results  );
  right()->eval( right_results );

  for ( auto const &result : right_results )
    left_results[ result.first ] += result.second;
}

void word_node::eval( search_results &results ) {
  FOR_EACH_IN_PAIR( range_, i ) {
    file_list const list( i );
    if ( is_too_frequent( list.size() ) )
      continue;
    for ( auto const &file : list )
      if ( file.has_meta_id( meta_id_ ) )
        results[ file.index_ ] += file.rank_;
  } // for
}

#ifdef DEBUG_eval_query
////////// print //////////////////////////////////////////////////////////////

ostream& and_node::print( ostream &o ) const {
  bool flag = false;
  for ( auto const &child : child_nodes_ ) {
    if ( flag )
      o << "and";
    else
      flag = true;
    o << " (" << child->print( o ) << ") ";
  }
  return o;
}

ostream& empty_node::print( ostream &o ) const {
  return o << "<empty>";
}

ostream& near_node::print( ostream &o ) const {
  o << " (" << left() ->print( o ) << ") near ("
    <<         right()->print( o ) << ") ";
  return o;
}

ostream& not_near_node::print( ostream &o ) const {
  o << " (" << left() ->print( o ) << ") not near ("
    <<         right()->print( o ) << ") ";
  return o;
}

ostream& not_node::print( ostream &o ) const {
  return o << "not (" << child_->print( o ) << ") ";
}

ostream& or_node::print( ostream &o ) const {
  o << " (" << left() ->print( o ) << ") or ("
    <<         right()->print( o ) << ") ";
  return o;
}

ostream& word_node::print( ostream &o ) const {
  return o << '"' << word_ << '"';
}
#endif /* DEBUG_eval_query */

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
