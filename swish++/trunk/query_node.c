/*
**      SWISH++
**      query_node.c
**
**      Copyright (C) 2004  Paul J. Lucas
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

// standard
#include <iostream>
#include <vector>

// local
#include "file_list.h"
#include "index_segment.h"
#include "query_node.h"
#include "WordsNear.h"
#include "util.h"

#ifndef PJL_NO_NAMESPACES
using namespace std;
#endif

empty_node empty_node::singleton_;

void empty_node::eval( search_results& ) {
    // Out-of-line because it's virtual.
}

////////// constructors ///////////////////////////////////////////////////////

and_node::and_node( pool_type &p, child_node_list &nodes ) : query_node( p ) {
    child_nodes_.swap( nodes );
}

#ifdef  FEATURE_word_pos
near_node::near_node( pool_type &p, query_node *left, query_node *right ) :
    query_node( p ), left_child_ ( left  ), right_child_( right )
{
    // do nothing else

}
#endif

word_node::word_node(
    pool_type &p, char const *word, word_range const &range, int meta_id
) :
    query_node( p ), word_( new_strdup( word ) ), range_( range ),
    meta_id_( meta_id )
{
    // do nothing else
}

////////// destructors ////////////////////////////////////////////////////////

and_node::~and_node() {
    //
    // We don't delete our child nodes because all nodes are deleted by the
    // pool.
    //
}

#ifdef  FEATURE_word_pos
near_node     ::~near_node    () { /* See comment in ~and_node(). */ }
not_near_node ::~not_near_node() { /* See comment in ~and_node(). */ }
#endif
not_node      ::~not_node     () { /* See comment in ~and_node(). */ }
or_node       ::~or_node      () { /* See comment in ~and_node(). */ }
query_node    ::~query_node   () { /* Out-of-line because it's virtual. */ }
word_node     ::~word_node    () { delete[] word_; }
query_node    ::visitor::~visitor() { /* Out-of-line because it's virtual. */ }

////////// visitors ///////////////////////////////////////////////////////////

query_node* and_node::visit( visitor const &v ) {
    query_node *const result = v( this );
    if ( result == this )
        FOR_EACH( child_node_list, child_nodes_, child )
            (*child)->visit( v );
    return result;
}

#ifdef  FEATURE_word_pos
query_node* near_node::visit( visitor const &v ) {
    query_node *const result = v( this );
    if ( result == this ) {
        left() ->visit( v );
        right()->visit( v );
    }
    return result;
}
#endif

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

//*****************************************************************************
//
// SYNOPSYS
//
        void and_node::eval( search_results &results )
//
// DESCRIPTION
//
//      Evaluate the search results for "and" by evaluating all of its child
//      nodes at the same time.  This is done to solve the weighting problem
//      with more than two "and" terms.  For example, the query:
//
//          mouse and computer and keyboard
//
//      is parsed and treated as:
//
//          (mouse and computer) and keyboard
//            25%        25%           50%
//
//      The problem is that the last term always gets 50% of the weighting.
//
//      In order to weight all the terms equally, the "and" results for each
//      term are saved in a list and then and'ed together at the end.
//
// PARAMETERS
//
//      results     The results get put here.
//
//*****************************************************************************
{
    //
    // Evaluate all the child nodes.
    //
    search_results const empty_place_holder;
    typedef vector< search_results > child_results_type;
    child_results_type child_results;
    child_results.reserve( child_nodes_.size() );
    FOR_EACH( child_node_list, child_nodes_, child_node ) {
        search_results results;
        (*child_node)->eval( results );
        if ( results.empty() ) {
            //
            // Since we're evaluating an "and", we can stop immediately if any
            // of the child nodes return no results.
            //
            return;
        }
        //
        // Temporarily push an empty "place-holder" search_results onto the
        // back of the vector to increase its size without copying the actual
        // potentially large search_results.  Then simply swap it with the
        // actual results in O(1) time.
        //
        // Yes, a vector of pointers to search_results could have been used
        // instead, but then we'd have to make sure to delete all the
        // search_results when done in two places: above at the "return" and
        // below when this function "falls out the bottom."
        //
        child_results.push_back( empty_place_holder );
        child_results.back().swap( results );
    }

    //
    // Pluck out one of the child results and make it *the* result so far.
    // (It's easiest to use the last one.  Since it's "and", which one we use
    // doesn't matter.)
    //
    results.swap( child_results.back() );
    child_results.pop_back();

    //
    // For each search result, see if it's in each child_result: if it is, sum
    // the ranks; if it isn't, delete the result.
    //
    for ( search_results::iterator
        result = results.begin(); result != results.end();
    ) {
        bool increment_result_iterator = true;
        FOR_EACH( child_results_type, child_results, child_result ) {
            search_results::const_iterator const
                found = child_result->find( result->first );
            if ( found != child_result->end() ) {
                result->second += found->second;
                continue;
            }
            //
            // Erasing an element at a given iterator invalidates the iterator:
            // copy the iterator to a temporary and increment the real iterator
            // off of the element to be erased so the real iterator won't be
            // invalidated.
            //
            search_results::iterator const erase_me = result++;
            results.erase( erase_me );
            increment_result_iterator = false;
            break;
        }
        if ( increment_result_iterator )
            ++result;
    }

    //
    // Now that the and-results have been summed, divide each by the number of
    // and-results, i.e., average them.  (It's +1 below because you have to
    // include the "results" variable itself.)
    //
    int const num_ands = child_nodes_.size() + 1;
    TRANSFORM_EACH( search_results, results, result )
        result->second /= num_ands;
}

#ifdef  FEATURE_word_pos
//*****************************************************************************
//
// SYNOPSYS
//
        void near_node::eval( search_results &results )
//
// DESCRIPTION
//
//      Evaluate the search results for "near".
//
// PARAMETERS
//
//      results     The results get put here.
//
//*****************************************************************************
{
    word_node const *const node[] = {
        dynamic_cast<word_node*>( left()  ),
        dynamic_cast<word_node*>( right() )
    };
    if ( !node[0] || !node[1] )
        return;

    if ( node[0]->meta_id() != No_Meta_ID &&
         node[1]->meta_id() != No_Meta_ID &&
         node[0]->meta_id() != node[1]->meta_id()
    )
        return;

    FOR_EACH_IN_PAIR( index_segment, node[0]->range(), word0 ) {
        file_list const list0( word0  );
        if ( is_too_frequent( list0.size() ) )
            continue;
        FOR_EACH_IN_PAIR( index_segment, node[1]->range(), word1 ) {
            file_list const list1( word1 );
            if ( is_too_frequent( list1.size() ) )
                continue;

            file_list::const_iterator file[] = { list0.begin(), list1.begin() };
            while ( file[0] != list0.end() && file[1] != list1.end() ) {

                ////////// Words in same file with right meta ID? /////////////

                if ( file[0]->index_ < file[1]->index_ ||
                    !file[0]->has_meta_id( node[0]->meta_id() )
                ) {
                    ++file[0];
                    continue;
                }
                if ( file[0]->index_ > file[1]->index_ ||
                    !file[1]->has_meta_id( node[1]->meta_id() )
                ) {
                    ++file[1];
                    continue;
                }

                ////////// Are words near each other? /////////////////////////

                int pdi[2];                 // pos_deltas_[i] index
                int pos[2];                 // absolute position for file[i]
                for ( int i = 0; i < 2; ++i ) {
                    pdi[i] = 0;
                    pos[i] = file[i]->pos_deltas_[0];
                }

                while ( true ) {
                    //
                    // Two words are near each other only if their absolute
                    // positions differ by at most words_near.
                    //
                    int const delta = pos[1] - pos[0];
                    if ( pjl_abs( delta ) <= words_near ) {
                        results[ file[0]->index_ ] = (
                            file[0]->rank_ + file[1]->rank_
                        ) / 2;
                        break;
                    }
                    //
                    // Increment the ith file's pos_deltas_ index and add the
                    // next delta to the accumulated absolute position.
                    //
                    int const i = delta < 1;
                    if ( ++pdi[i] >= file[i]->pos_deltas_.size() )
                        break;
                    pos[i] += file[i]->pos_deltas_[ pdi[i] ];
                }

                ++file[0], ++file[1];
            }
        }
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        query_node* near_node::distribute()
//
// DESCRIPTION
//
//      "Distribute" the child nodes that are not word_nodes since the child
//      nodes of a near_node MUST be word_nodes.  For example:
//
//          cat near (mouse or bird)
//
//      becomes:
//
//          (cat near mouse) or (cat near bird)
//
// RETURN VALUE
//
//      If distribution was performed (either or both child nodes were not
//      word nodes), returns a new, distributed subtree; otherwise, simply
//      returns this.
//
//*****************************************************************************
{
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

//*****************************************************************************
//
// SYNOPSYS
//
        query_node* near_node::distributor::operator()( query_node *node ) const
//
// DESCRIPTION
//
//      Visit a node in a query tree.  If it's a near_node, distribute it.
//
// PARAMETERS
//
//      node    The query_node to visit.
//
// RETURN VALUE
//
//      Returns a possibly new subtree after possibly having done distribution.
//
//*****************************************************************************
{
    if ( near_node *const n = dynamic_cast<near_node*>( node ) )
        return n->distribute();
    if ( !other_ )
        return node;

    distributor const d( 0, not_near_ );
    if ( and_node *const a = dynamic_cast<and_node*>( node ) ) {
        and_node::child_node_list new_child_nodes;
        FOR_EACH( and_node, *a, child ) {
            near_node *const new_child = make_node(
                node->pool(), other_, (*child)->visit( d )
            );
            new_child_nodes.push_back( new_child->distribute() );
        }
        return new and_node( node->pool(), new_child_nodes );
    }
    if ( or_node *const o = dynamic_cast<or_node*>( node ) ) {
        near_node *const new_left = make_node(
            node->pool(), other_, o->left()->visit( d )
        );
        near_node *const new_right = make_node(
            node->pool(), other_, o->right()->visit( d )
        );
        return new or_node(
            node->pool(), new_left->distribute(), new_right->distribute()
        );
    }
    if ( dynamic_cast<not_node*>( node ) )
        internal_error
            << "near_node::distributor::operator()(): "
               "encountered not_node" << report_error;
    return node;
}

//*****************************************************************************
//
// SYNOPSIS
//
        void not_near_node::eval( search_results &results )
//
// DESCRIPTION
//
//      Evaluate the search results for "not near".  This code is very similar
//      to that for near_node::eval().  The difference is that the right-hand
//      word can either be "not near" the left-hand word -OR- not present in
//      the same file at all.
//
// PARAMETERS
//
//      results     The results get put here.
//
//*****************************************************************************
{
    word_node const *const node[] = {
        dynamic_cast<word_node*>( left()  ),
        dynamic_cast<word_node*>( right() )
    };
    if ( !node[0] )
        return;

    FOR_EACH_IN_PAIR( index_segment, node[0]->range(), word0 ) {
        file_list const list0( word0 );
        if ( is_too_frequent( list0.size() ) )
            continue;
        if ( !node[1] ) {
            //
            // If the right-hand side node isn't a word_node (i.e., it's an
            // empty_node), then this case degenerates into doing the same
            // thing that word_node::eval() does.
            //
            FOR_EACH( file_list, list0, file )
                if ( file->has_meta_id( node[0]->meta_id() ) )
                    results[ file->index_ ] += file->rank_;
            continue;
        }

        ////////// Must check for "near"-ness of right-hand side word /////////

        FOR_EACH_IN_PAIR( index_segment, node[1]->range(), word1 ) {
            file_list const list1( word1 );
            file_list::const_iterator file[] = { list0.begin(), list1.begin() };
            while ( file[0] != list0.end() ) {
                if ( file[0]->has_meta_id( node[0]->meta_id() ) ) {
                    //
                    // Make file[1]'s index "catch up" to file[0]'s.
                    //
                    while ( file[1] != list1.end() &&
                            file[1]->index_ < file[0]->index_ )
                        ++file[1];

                    ////////// Are words in the same file? ////////////////////

                    if ( file[1] != list1.end() &&
                        file[0]->index_ == file[1]->index_ &&
                        file[1]->has_meta_id( node[1]->meta_id() )
                    ) {
                        ////////// Are words near each other? /////////////////

                        int pdi[2];         // pos_deltas_[i] index
                        int pos[2];         // absolute position for file[i]
                        for ( int i = 0; i < 2; ++i ) {
                            pdi[i] = 0;
                            pos[i] = file[i]->pos_deltas_[0];
                        }

                        while ( true ) {
                            //
                            // Two words are near each other only if their
                            // absolute positions differ by at most words_near.
                            //
                            int const delta = pos[1] - pos[0];
                            if ( pjl_abs( delta ) <= words_near )
                                goto found_near;
                            //
                            // Increment the ith file's pos_deltas_ index and
                            // add the next delta to the accumulated absolute
                            // position.
                            //
                            int const i = delta < 1;
                            if ( ++pdi[i] >= file[i]->pos_deltas_.size() )
                                break;
                            pos[i] += file[i]->pos_deltas_[ pdi[i] ];
                        }
                    }
                    results[ file[0]->index_ ] += file[0]->rank_;
                }
found_near:     ++file[0];
                if ( file[1] != list1.end() )
                    ++file[1];
            }
        }
    }
}
#endif  /* FEATURE_word_pos */

//*****************************************************************************
//
// SYNOPSYS
//
        void not_node::eval( search_results &results )
//
// DESCRIPTION
//
//      Evaluate the search results for "not".
//
// PARAMETERS
//
//      results     The results get put here.
//
//*****************************************************************************
{
    extern index_segment files;

    search_results child_results;
    child_->eval( child_results );
    if ( child_results.empty() )
        return;
    for ( int i = 0; i < files.size(); ++i )
        if ( child_results.find( i ) == child_results.end() )
            results[ i ] = 100;
}

//*****************************************************************************
//
// SYNOPSYS
//
        void or_node::eval( search_results &left_results )
//
// DESCRIPTION
//
//      Evaluate the search results for "or".
//
// PARAMETERS
//
//      left_results    The results get put here.
//
//*****************************************************************************
{
    search_results right_results;

    left() ->eval( left_results  );
    right()->eval( right_results );

    FOR_EACH( search_results, right_results, result )
        left_results[ result->first ] += result->second;
}

//*****************************************************************************
//
// SYNOPSYS
//
        void word_node::eval( search_results &results )
//
// DESCRIPTION
//
//      Evaluate the search results for a single word.
//
// PARAMETERS
//
//      results     The results get put here.
//
//*****************************************************************************
{
    FOR_EACH_IN_PAIR( index_segment, range_, i ) {
        file_list const list( i );
        if ( is_too_frequent( list.size() ) )
            continue;
        FOR_EACH( file_list, list, file )
            if ( file->has_meta_id( meta_id_ ) )
                results[ file->index_ ] += file->rank_;
    }
}

#ifdef  DEBUG_eval_query
////////// print //////////////////////////////////////////////////////////////

ostream& and_node::print( ostream &o ) const {
    bool flag = false;
    FOR_EACH( child_node_list, child_nodes_, child_ ) {
        if ( flag )
            o << "and";
        else
            flag = true;
        o << " (" << (*child_)->print( o ) << ") ";
    }
    return o;
}

ostream& empty_node::print( ostream &o ) const {
    return o << "<empty>";
}

ostream& near_node::print( ostream &o ) const {
    o   << " (" << left() ->print( o ) << ") near ("
        <<         right()->print( o ) << ") ";
    return o;
}

ostream& not_near_node::print( ostream &o ) const {
    o   << " (" << left() ->print( o ) << ") not near ("
        <<         right()->print( o ) << ") ";
    return o;
}

ostream& not_node::print( ostream &o ) const {
    return o << "not (" << child_->print( o ) << ") ";
}

ostream& or_node::print( ostream &o ) const {
    o   << " (" << left() ->print( o ) << ") or ("
        <<         right()->print( o ) << ") ";
    return o;
}

ostream& word_node::print( ostream &o ) const {
    return o << '"' << word_ << '"';
}
#endif  /* DEBUG_eval_query */
/* vim:set et sw=4 ts=4: */
