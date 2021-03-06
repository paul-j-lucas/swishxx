/*
**      SWISH++
**      src/encoded_char.h
**
**      Copyright (C) 2000-2016  Paul J. Lucas
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

#ifndef encoded_char_H
#define encoded_char_H

// local
#include "config.h"
#include "iso8859-1.h"
#include "pjl/char_buffer_pool.h"
#include "util.h"                       /* for to_lower() */

// standard
#include <iterator>
#include <limits>
#include <unordered_set>

///////////////////////////////////////////////////////////////////////////////

/**
 * An %encoded_char_range is an abstraction that contains a range of characters
 * in memory that are encoded according to some scheme, either a
 * \c Content-Transfer-Encoding (such as \c Quoted-Printable or \c Base64) or a
 * character set (such as UTF-7 or UTF-8).  A const_iterator is used to iterate
 * over the range and, when dereferenced, decodes a character.
 *
 * However, doing this is a serious performance hit since it has to be done for
 * every single character examined.  Hence, the code is #ifdef'd for \c
 * WITH_DECODING: if it's not defined, there's no need for any special
 * decoding.
 */
class encoded_char_range {
public:
  typedef ptrdiff_t difference_type;
  typedef char value_type;
  typedef value_type const* const_pointer;
  class const_iterator;
  typedef value_type (*charset_type)
    (const_pointer, const_pointer&, const_pointer);
  typedef value_type (*encoding_type)
    (const_pointer, const_pointer&, const_pointer);

  /**
   * Constructs an %encoded_char_range.
   *
   * @param begin A pointer to the beginning of the range.
   * @param end A pointer to one past the end of the range.
   * @param charset A pointer to the character set transcoder function, if any.
   * @param encoding A pointer to the character encoding decoder function, if
   * any.
   */
  encoded_char_range( const_pointer begin, const_pointer end,
                      charset_type charset = nullptr,
                      encoding_type encoding = nullptr );

  /**
   * Constructs an %encoded_char_range.
   *
   * @param pos The const_iterator whose range to use.
   */
  encoded_char_range( const_iterator const &pos );

  /**
   * Constructs an %encoded_char_range.
   *
   * @param begin The const_iterator positioned at the beginning of the range.
   * @param end The const_iterator positioned at one past the end of the range.
   */
  encoded_char_range( const_iterator const &begin, const_iterator const &end );

  encoded_char_range( encoded_char_range const& ) = default;
  encoded_char_range& operator=( encoded_char_range const& ) = default;

  const_iterator  begin() const;
  const_pointer   begin_pos() const               { return begin_; }
  void            begin_pos( const_pointer p )    { begin_ = p; }
  void            begin_pos( const_iterator const& );
  const_iterator  end() const;
  const_pointer   end_pos() const                 { return end_; }
  void            end_pos( const_pointer p )      { end_ = p; }
  void            end_pos( const_iterator const& );

  /**
   * Gets whether this iterator has been associated with an actual range.
   *
   * @return Returns \c true only if it is.
   */
  explicit operator bool() const {
    return begin_ != nullptr;
  }

#ifdef WITH_DECODING
  class decoder;
#endif /* WITH_DECODING */

protected:
  encoded_char_range();

  const_pointer begin_;
  const_pointer end_;
#ifdef WITH_DECODING
  charset_type  charset_;
  encoding_type encoding_;
#endif /* WITH_DECODING */
  friend class const_iterator;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * An %encoded_char_range::const_iterator is (not surprisingly) an iterator for
 * an encoded_char_range.  It might seem a bit odd to have an iterator derived
 * from the container class it's an iterator for (that's because it is odd),
 * but the iterator needs access to all its data members and going through an
 * extra level of indirection by having a pointer to it would be slower.
 */
class encoded_char_range::const_iterator :
  public encoded_char_range,
  public std::iterator<std::forward_iterator_tag,
                       encoded_char_range::value_type const> {
public:
  typedef encoded_char_range::difference_type difference_type;
  typedef encoded_char_range::value_type value_type;
  typedef encoded_char_range::const_pointer const_pointer;

  const_iterator();
  const_iterator( const_pointer begin, const_pointer end,
                  charset_type = nullptr, encoding_type = nullptr );

  const_iterator( const_iterator const& ) = default;
  const_iterator& operator=( const_iterator const& ) = default;

  value_type      operator*() const;
  const_iterator& operator++();
  const_iterator  operator++(int);

  bool            at_end() const      { return pos_ == end_; }

  /**
   * Performs a dereference but only if it's not at the end.
   *
   * @return Returns the decoded character or \c NUL if at the end.
   */
  value_type safe_deref() const;

  const_pointer   pos() const         { return pos_; }
  const_pointer&  pos()               { return pos_; }
  const_pointer   prev_pos() const    { return prev_; }

  friend bool operator==( const_iterator const&, const_iterator const& );
  friend bool operator< ( const_iterator const&, const_iterator const& );
  friend bool operator<=( const_iterator const&, const_iterator const& );

  friend bool operator==( const_iterator const&, const_pointer );
  friend bool operator==( const_pointer, const_iterator const& );
  friend bool operator< ( const_iterator const&, const_pointer );
  friend bool operator< ( const_pointer, const_iterator const& );
  friend bool operator<=( const_iterator const&, const_pointer );
  friend bool operator<=( const_pointer, const_iterator const& );

private:
  mutable const_pointer pos_;
  mutable const_pointer prev_;
#ifdef WITH_DECODING
  mutable value_type    ch_;
  mutable bool          decoded_;
  mutable ptrdiff_t     delta_;
#endif /* WITH_DECODING */

  const_iterator( encoded_char_range const*, const_pointer start_pos );
  friend class encoded_char_range;      // for access to c'tor above

#ifdef WITH_DECODING
  void decode() const;
#endif /* WITH_DECODING */
};

///////////////////////////////////////////////////////////////////////////////

#ifdef WITH_DECODING
/**
 * An %encoded_char_range::decoder is used to keep decoders' state between
 * calls and reset state between files to their initial states just before
 * starting to index a file.
 */
class encoded_char_range::decoder {
public:
  typedef encoded_char_range::value_type value_type;
  typedef encoded_char_range::const_pointer const_pointer;

  virtual ~decoder();

  static void reset_all();

protected:
  decoder() {
    decoders_.insert( this );
  }

  virtual void reset() = 0;

private:
  typedef std::unordered_set<decoder*> set_type;
  static set_type decoders_;
};
#endif /* WITH_DECODING */

////////// encoded_char_range inlines /////////////////////////////////////////

// I hate lots of typing.
#define ECR       encoded_char_range
#define ECR_CI    ECR::const_iterator
#define ECR_CI_CP ECR::const_iterator::const_pointer

inline ECR::ECR() {
  begin_ = end_ = nullptr;
#if WITH_DECODING
  charset_ = nullptr;
  encoding_ = nullptr;
#endif /* WITH_DECODING */
}

inline ECR::ECR( const_pointer begin, const_pointer end, charset_type charset,
                 encoding_type encoding ) :
  begin_( begin ), end_( end )
#ifdef WITH_DECODING
  , charset_( charset ), encoding_( encoding )
#endif /* WITH_DECODING */
{
}

inline ECR::ECR( const_iterator const &i ) :
  begin_( i.pos_ ), end_( i.end_ )
#ifdef WITH_DECODING
  , charset_( i.charset_ ), encoding_( i.encoding_ )
#endif /* WITH_DECODING */
{
}

inline ECR::ECR( const_iterator const &begin, const_iterator const &end ) :
  begin_( begin.pos_ ), end_( end.pos_ )
#ifdef WITH_DECODING
  , charset_( begin.charset_ ), encoding_( begin.encoding_ )
#endif /* WITH_DECODING */
{
}

inline ECR_CI ECR::begin() const {
  return const_iterator( this, begin_ );
}

inline ECR_CI ECR::end() const {
  return const_iterator( this, end_ );
}

inline void ECR::begin_pos( const_iterator const &i ) {
  begin_ = i.pos_;
}

inline void ECR::end_pos( const_iterator const &i ) {
  end_ = i.pos_;
}

////////// encoded_char_range::const_iterator inlines /////////////////////////

inline ECR_CI::const_iterator() : pos_( nullptr ) {
}

inline ECR_CI::const_iterator( const_pointer begin, const_pointer end,
                               charset_type charset, encoding_type encoding ) :
  encoded_char_range( begin, end, charset, encoding ), pos_( begin )
#ifdef WITH_DECODING
  , decoded_( false )
#endif /* WITH_DECODING */
{
}

inline ECR_CI::const_iterator( ECR const *ecr, const_pointer start_pos ) :
  encoded_char_range(
    start_pos, ecr->end_
#ifdef WITH_DECODING
    , ecr->charset_, ecr->encoding_
#endif /* WITH_DECODING */
  ),
  pos_( start_pos )
#ifdef WITH_DECODING
  , decoded_( false )
#endif /* WITH_DECODING */
{
}

#ifdef WITH_DECODING
/**
 * Decodes the character at the iterator's current position according to the
 * character range's encoding or character set.
 */
inline void ECR_CI::decode() const {
  //
  // Remember the current position to allow the decoders to advance through the
  // encoded text.  This allows the delta to be computed so the iterator can be
  // incremented later.
  //
  const_pointer c = pos_;
  //
  // A mail message can have both an encoding and a non-ASCII or non-ISO-8859-1
  // charset simultaneously, e.g., base64-encoded UTF-8.  (In practice, this
  // particular case should never happen since UTF-7 should be used instead;
  // but you get the idea.)
  //
  // However, handling both an encoding and such a charset simultaneously is a
  // real pain because both can use multiple characters to decode a single
  // character and keeping track of both positions is messy and I didn't feel
  // like thinking about this just now.
  //
  // Hence, a current caveat is that a mail message or attachment can have
  // EITHER an encoding OR a non-ASCII/ISO-8859-1 character set, but not both.
  // If it does, the encoding takes precedence.
  //
  if ( encoding_ )
    ch_ = (*encoding_)( begin_, c, end_ );
  else if ( charset_ )
    ch_ = (*charset_)( begin_, c, end_ );
  else
    ch_ = iso8859_1_to_ascii( *c++ );
  delta_ = c - pos_;
}
#endif /* WITH_DECODING */

/**
 * Dereferences an encoded_char_range::const_iterator at its current position.
 *
 * @return Returns the decoded character.
 */
inline ECR::value_type ECR_CI::operator*() const {
#ifdef WITH_DECODING
  if ( !decoded_ ) {
    decode();
    decoded_ = true;
  }
  return ch_;
#else
  return iso8859_1_to_ascii( *pos_ );
#endif /* WITH_DECODING */
}

inline ECR_CI& ECR_CI::operator++() {
#ifdef WITH_DECODING
  if ( decoded_ ) {
    //
    // The character at the current position has previously been decoded so we
    // know the delta.  However, since we're about to increment the position to
    // the next character, that character will no longer have been decoded, so
    // set decoded_ to false.
    //
    decoded_ = false;
  } else {
    //
    // The character at the current position has not previously been decoded so
    // we don't know the delta: call decode() to calculate the delta only.  We
    // can't set decoded_ to true since we're about to increment the position
    // to the next character and that character hasn't been decoded.
    //
    decode();
  }
  prev_ = pos_;
  pos_ += delta_;
#else
  prev_ = pos_++;
#endif /* WITH_DECODING */
  return *this;
}

inline ECR_CI ECR_CI::operator++(int) {
  ECR_CI const temp = *this;
  return ++*this, temp;
}

inline ECR::value_type ECR_CI::safe_deref() const {
  return at_end() ? '\0' : operator*();
}

////////// relational operators ///////////////////////////////////////////////

inline bool operator==( ECR_CI const &e1, ECR_CI const &e2 ) {
  return e1.pos_ == e2.pos_;
}

inline bool operator!=( ECR_CI const &e1, ECR_CI const &e2 ) {
  return !(e1 == e2);
}

inline bool operator<( ECR_CI const &e1, ECR_CI const &e2 ) {
  return e1.pos_ < e2.pos_;
}

inline bool operator<=( ECR_CI const &e1, ECR_CI const &e2 ) {
  return e1.pos_ <= e2.pos_;
}

inline bool operator>( ECR_CI const &e1, ECR_CI const &e2 ) {
  return !(e2 <= e1);
}

inline bool operator>=( ECR_CI const &e1, ECR_CI const &e2 ) {
  return !(e2 < e1);
}

inline bool operator==( ECR_CI const &e, ECR_CI_CP p ) {
  return e.pos_ == p;
}

inline bool operator==( ECR_CI_CP p, ECR_CI const &e ) {
  return e == p;
}

inline bool operator!=( ECR_CI const &e, ECR_CI_CP p ) {
  return !(e == p);
}

inline bool operator!=( ECR_CI_CP p, ECR_CI const &e ) {
  return e != p;
}

inline bool operator<( ECR_CI const &e, ECR_CI_CP p ) {
  return e.pos_ < p;
}

inline bool operator<( ECR_CI_CP p, ECR_CI const &e ) {
  return p < e.pos_;
}

inline bool operator<=( ECR_CI const &e, ECR_CI_CP p ) {
  return e.pos_ <= p;
}

inline bool operator<=( ECR_CI_CP p, ECR_CI const &e ) {
  return p <= e.pos_;
}

inline bool operator>( ECR_CI const &e, ECR_CI_CP p ) {
  return !(p <= e);
}

inline bool operator>( ECR_CI_CP p, ECR_CI const &e ) {
  return !(e <= p);
}

inline bool operator>=( ECR_CI const &e, ECR_CI_CP p ) {
  return !(p < e);
}

inline bool operator>=( ECR_CI_CP p, ECR_CI const &e ) {
  return !(e < p);
}

////////// utility functions //////////////////////////////////////////////////

/**
 * Returns a pointer to a string converted to lower case taking the encoding of
 * the characters into account; the original string is untouched.  The string
 * returned is from an internal pool of string buffers.  The time you get into
 * trouble is if you hang on to more than \c Num_Buffers strings.  This doesn't
 * normally happen in practice, however.
 *
 * @param c The iterator to use.
 * @return Returns a pointer to the lower-case string.
 */
char *to_lower( ECR const &range );

/**
 * Trims leading whitespace from the encoded_char_range.
 *
 * @param end The end of the range.
 */
void trim_left( ECR_CI *begin, ECR_CI const &end );

/**
 * Trims trailing whitespace from the encoded_char_range.
 *
 * @param begin The beginning of the range.
 * @param end A pointer to the iterator to alter so that trailing whitespace
 * is truncated.
 */
void trim_right( ECR_CI const &begin, ECR_CI *end );

/**
 * Trims both leading and trailing whitespace from the encoded_char_range.
 *
 * @param begin A pointer to the iterator to alter so that leading whitespace
 * is skipped over.
 * @param end A pointer to the iterator to alter so that trailing whitespace
 * is truncated.
 */
inline void trim( ECR_CI *begin, ECR_CI *end ) {
  trim_left( begin, *end );
  trim_right( *begin, end );
}

/**
 * Skips all characters up to and including \a c.
 *
 * @param i A pointer to the iterator to start at. If \c is found, \a i is
 * repositioned to just after \a c.
 * @param c The character to skip after.
 * @param limit The maximum number of character to scan looking for \a c.
 * @return Returns \c true only if the character was found.
 */
bool skip_char( ECR_CI *i, char c,
                size_t limit = std::numeric_limits<size_t>::max() );

///////////////////////////////////////////////////////////////////////////////

#undef ECR
#undef ECR_CI
#undef ECR_CI_CP

#endif /* encoded_char_H */
/* vim:set et sw=2 ts=2: */
