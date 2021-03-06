/*
**      SWISH++
**      src/token.h
**
**      Copyright (C) 1998-2015  Paul J. Lucas
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

#ifndef token_H
#define token_H

// local
#include "swishxx-config.h"

// standard
#include <sstream>

class token_stream;

///////////////////////////////////////////////////////////////////////////////

/**
 * A %oktne contains a token parsed from a token_stream.
 */
class token {
public:
  enum type {
    tt_none,
    tt_and,
    tt_equal,
    tt_lparen,
#ifdef WITH_WORD_POS
    tt_near,
    //
    // The "tt_not_near" token isn't a real token in that it's not parsed as a
    // single word.  The parser synthesizes it by using look-ahead, i.e., a
    // "not" token followed by a "near" token.  Having a distinct enum value
    // for it here just makes life easier.
    //
    tt_not_near,
#endif /* WITH_WORD_POS */
    tt_not,
    tt_or,
    tt_rparen,
    tt_word_star,
    tt_word
  };

  token() : type_( tt_none )            { }
  explicit token( token_stream &in )    { in >> *this; }
  token( token const& ) = default;
  token& operator=( token const& ) = default;

  operator    type() const              { return type_; }
  int         length() const            { return len_; }
  char const* str() const               { return buf_; }
  char const* lower_str() const         { return lower_buf_; }

  friend token_stream& operator>>( token_stream&, token& );

private:
  type type_;
  char buf_[ Word_Hard_Max_Size + 1 ];
  char lower_buf_[ Word_Hard_Max_Size + 1 ];
  int  len_;
};

/**
 * A %token_stream is-an istringstream that has the additional ability to put
 * back previously parsed tokens used as look-ahead.  Note that it puts back
 * whole, already-parsed tokens rather than characters so they don't have to be
 * parsed again.
 */
class token_stream : public std::istringstream {
public:
  token_stream( char const *s ) : std::istringstream( s ), top_( -1 ) { }

  void put_back( token const &t ) {
    stack_[ ++top_ ] = t;
  }

private:
  // Our query parser needs at most 2 look-ahead tokens.
  token stack_[2];
  int   top_;

  token* put_back() {
    return top_ >= 0 ? stack_ + top_-- : nullptr;
  }

  friend token_stream& operator>>( token_stream&, token& );
};

/**
 * Extracts a token from the given input stream.
 *
 * @param ts The token stream to extract the token from.
 * @param t The newly extracted token.
 * @return Returns the first argument as is standard practice.
 */
token_stream& operator>>( token_stream &ts, token &t );

///////////////////////////////////////////////////////////////////////////////

#endif /* token_H */
/* vim:set et sw=2 ts=2: */
