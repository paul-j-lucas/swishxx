/*
**      SWISH++
**      src/query_node.h
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

#ifndef query_node_H
#define query_node_H

// standard
#ifdef  DEBUG_eval_query
#include <iostream>
#endif
#include <new>
#include <vector>

// local
#include "pjl/auto_delete_pool.h"
#include "query.h"

//*****************************************************************************
//
// SYNOPSYS
//
        class query_node : public PJL::auto_delete_obj<query_node>
//
// DESCRIPTION
//
//      A query_node is the abstract base class for all nodes in a query tree.
//
//*****************************************************************************
{
public:
    class visitor {
        //
        // A visitor is an abstract base class for an object that "visits"
        // nodes in the tree during traversal.
        //
    public:
        virtual ~visitor();
        virtual query_node* operator()( query_node* ) const = 0;
    };

    virtual ~query_node();

    virtual void            eval( search_results& ) = 0;
    virtual query_node*     visit( visitor const& );
#   ifdef DEBUG_eval_query
    virtual std::ostream&   print( std::ostream& ) const = 0;
#   endif
protected:
    query_node() { }
    query_node( pool_type &p ) : pool_object_type( p ) { }
};

//*****************************************************************************
//
// SYNOPSYS
//
        class and_node : public query_node
//
// DESCRIPTION
//
//      An and_node is-a query_node that implements "and" in queries.
//
//*****************************************************************************
{
public:
    typedef std::vector<query_node*> child_node_list;

    typedef child_node_list::iterator iterator;
    typedef child_node_list::const_iterator const_iterator;
    typedef child_node_list::reverse_iterator reverse_iterator;
    typedef child_node_list::const_reverse_iterator const_reverse_iterator;

    and_node( pool_type&, child_node_list& );
    ~and_node();

    iterator        begin()             { return child_nodes_.begin(); }
    const_iterator  begin() const       { return child_nodes_.begin(); }
    iterator        end()               { return child_nodes_.end(); }
    const_iterator  end() const         { return child_nodes_.end(); }
    void            eval( search_results& );
    query_node*     visit( visitor const& );
#   ifdef DEBUG_eval_query
    std::ostream&   print( std::ostream& ) const;
#   endif
protected:
    child_node_list child_nodes_;
};

//*****************************************************************************
//
// SYNOPSYS
//
        class empty_node : public query_node
//
// DESCRIPTION
//
//      An empty_node is-a query_node that is used to represent a leaf node in
//      a query tree that has no search results.
//
//*****************************************************************************
{
public:
    //
    // Since we only ever want a single empty_node object to exist, we override
    // operators new and delete.
    //
    void*   operator new( size_t )              { return &singleton_; }
    void    operator delete( void*, size_t )    { /* do nothing */ }

    void            eval( search_results& );
#   ifdef DEBUG_eval_query
    std::ostream&   print( std::ostream& ) const;
#   endif
private:
    static empty_node singleton_;
};

#ifdef  FEATURE_word_pos
//*****************************************************************************
//
// SYNOPSYS
//
        class near_node : public query_node
//
// DESCRIPTION
//
//      A near_node is-a query_node that implements "near" in queries.  Unlike
//      all other nodes, the child nodes of a near_node MUST be word_nodes
//      because their word_ranges are used to compute search results.
//
//*****************************************************************************
{
public:
    class distributor : public visitor {
        //
        // A distributor is-a visitor that is used to "distribute" the child
        // nodes of a near_node that are not word_nodes since the child nodes
        // of a near_node MUST be word_nodes.  For example:
        //
        //      cat near (mouse or bird)
        //
        // becomes:
        //
        //      (cat near mouse) or (cat near bird)
        //
    public:
        distributor( query_node *other, bool not_near = false ) :
            other_( other ), not_near_( not_near ) { }

        query_node* operator()( query_node* ) const;
    private:
        near_node*  make_node( pool_type&, query_node*, query_node* ) const;

        query_node *const other_;
        bool const not_near_;
    };

    near_node( pool_type&, query_node*, query_node* );
    ~near_node();

    query_node* distribute();
    void        eval( search_results& );
    query_node* left () const           { return left_child_ ; }
    query_node* right() const           { return right_child_; }
    query_node* visit( visitor const& );

#   ifdef DEBUG_eval_query
    std::ostream&   print( std::ostream& ) const;
#   endif
private:
    query_node *const left_child_, *const right_child_;
};

//*****************************************************************************
//
// SYNOPSYS
//
        class not_near_node : public near_node
//
// DESCRIPTION
//
//      A not_near_node is-a near_node that implements "not near" in queries.
//
//*****************************************************************************
{
public:
    not_near_node( pool_type &pool, query_node *left, query_node *right ) :
        near_node( pool, left, right ) { }
    ~not_near_node();

    void            eval( search_results& );

#   ifdef DEBUG_eval_query
    std::ostream&   print( std::ostream& ) const;
#   endif
};
#endif  /* FEATURE_word_pos */

//*****************************************************************************
//
// SYNOPSYS
//
        class not_node : public query_node
//
// DESCRIPTION
//
//      A not_node is-a query_node that implements "not" in queries.
//
//*****************************************************************************
{
public:
    not_node( pool_type &p, query_node *child ) :
        query_node( p ), child_( child ) { }
    ~not_node();

    query_node*     child() const { return child_; }
    void            eval( search_results& );
    query_node*     visit( visitor const& );
#   ifdef DEBUG_eval_query
    std::ostream&   print( std::ostream& ) const;
#   endif
private:
    query_node *const child_;
};

//*****************************************************************************
//
// SYNOPSYS
//
        class or_node : public query_node
//
// DESCRIPTION
//
//      An or_node is-a query_node that implements "or" in queries.
//
//*****************************************************************************
{
public:
    or_node( pool_type &p, query_node *left, query_node *right ) :
        query_node( p ), left_child_( left ), right_child_( right ) { }
    ~or_node();

    void            eval( search_results& );
    query_node*     left () const { return left_child_ ; }
    query_node*     right() const { return right_child_; }
    query_node*     visit( visitor const& );
#   ifdef DEBUG_eval_query
    std::ostream&   print( std::ostream& ) const;
#   endif
private:
    query_node *const left_child_, *const right_child_;
};

//*****************************************************************************
//
// SYNOPSYS
//
        class word_node : public query_node
//
// DESCRIPTION
//
//      A word_node is-a query_node that implements either "word" or "word*" in
//      queries.
//
//*****************************************************************************
{
public:
    word_node( pool_type&, char const*, word_range const&, int );
    ~word_node();

    void                eval( search_results& );
    int                 meta_id() const { return meta_id_; }
    word_range const&   range() const   { return range_; }
#   ifdef DEBUG_eval_query
    std::ostream&       print( std::ostream& ) const;
#   endif
private:
    char *const word_;
    word_range const range_;
    int const meta_id_;
};

////////// inlines ////////////////////////////////////////////////////////////

#ifdef  FEATURE_word_pos
inline near_node* near_node::distributor::make_node(
    pool_type &pool, query_node *left, query_node *right
) const {
    return not_near_ ?
        new not_near_node( pool, left, right ) :
        new     near_node( pool, left, right ) ;
}
#endif

#endif  /* query_node_H */
/* vim:set et sw=4 ts=4: */
