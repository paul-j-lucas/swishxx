/*
**	SWISH++
**	encoded_char.h
**
**	Copyright (C) 2000  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef encoded_char_H
#define encoded_char_H

// standard
#include <iterator>
#include <set>

// local
#include "fake_ansi.h"			/* for iterator, std */
#include "iso8859-1.h"
#include "util.h"			/* for to_lower() */

//*****************************************************************************
//
// SYNOPSIS
//
	class encoded_char_range
//
// DESCRIPTION
//
//	An encoded_char_range is an abstraction that contains a range of
//	characters in memory that are encoded according to some scheme, either
//	a Content-Transfer-Encoding (such as Quoted-Printable or Base64) or a
//	character set (such as UTF-7 or UTF-8).  A const_iterator is used to
//	iterate over the range and, when dereferenced, decodes a character.
//
//	However, doing this is a serious performance hit since it has to be
//	done for every single character examined.  Hence, the code is #ifdef'd
//	for MOD_id3 and MOD_mail: if neither is used, there's no need for any
//	special decoding.
//
//*****************************************************************************
{
public:
	typedef ptrdiff_t difference_type;
	typedef char value_type;
	typedef value_type const* pointer;
	typedef value_type (*charset_type )( pointer, pointer&, pointer );
	typedef value_type (*encoding_type)( pointer, pointer&, pointer );

	class const_iterator;
	friend class const_iterator;

	encoded_char_range(
		pointer begin, pointer end, charset_type = 0, encoding_type = 0
	);
	encoded_char_range( const_iterator const &pos );
	encoded_char_range(
		const_iterator const &begin, const_iterator const &end
	);

	// default copy constructor is fine
	// default assignment operator is fine

	const_iterator	begin() const;
	pointer		begin_pos() const		{ return begin_; }
	void		begin_pos( pointer p )		{ begin_ = p; }
	void		begin_pos( const_iterator const& );
	const_iterator	end() const;
	pointer		end_pos() const			{ return end_; }
	void		end_pos( pointer p )		{ end_ = p; }
	void		end_pos( const_iterator const& );

#ifdef	IMPLEMENT_DECODING
	class decoder;
#endif
protected:
	encoded_char_range() { }

	pointer		begin_;
	pointer		end_;
#ifdef	IMPLEMENT_DECODING
	charset_type	charset_;
	encoding_type	encoding_;
#endif
};

//*****************************************************************************
//
// SYNOPSIS
//
	class encoded_char_range::const_iterator :
		public encoded_char_range,
		public std::iterator<
			std::forward_iterator_tag,
			encoded_char_range::value_type const
		>
//
// DESCRIPTION
//
//	An encoded_char_range::const_iterator is (not surprisingly) an iterator
//	for an encoded_char_range.  It might seem a bit odd to have an iterator
//	derived from the container class it's an iterator for (that's because
//	it is odd), but the iterator needs access to all its data members and
//	going through an extra level of indirection by having a pointer to it
//	would be slower.
//
//*****************************************************************************
{
public:
	typedef encoded_char_range::difference_type difference_type;
	typedef encoded_char_range::value_type value_type;
	typedef encoded_char_range::pointer pointer;

	const_iterator() { }
	const_iterator(
		pointer begin, pointer end, charset_type = 0, encoding_type = 0
	);

	// default copy constructor is fine
	// default assignment operator is fine

	value_type	operator*() const;
	const_iterator&	operator++();
	const_iterator	operator++(int);

	bool		at_end() const			{ return pos_ == end_; }

	pointer		pos() const			{ return pos_; }
	pointer&	pos()				{ return pos_; }
	pointer		prev_pos() const		{ return prev_; }

	friend bool operator==( const_iterator const&, const_iterator const& );
	friend bool operator==( const_iterator const&, pointer );
private:
	mutable pointer	pos_;
	mutable pointer	prev_;
#ifdef	IMPLEMENT_DECODING
	mutable value_type	ch_;
	mutable bool		decoded_;
	mutable int		delta_;
#endif
	const_iterator( encoded_char_range const*, pointer start_pos );
	friend class	encoded_char_range;	// for access to c'tor above
#ifdef	IMPLEMENT_DECODING
	void		decode() const;
#endif
};

#ifdef	IMPLEMENT_DECODING
//*****************************************************************************
//
// SYNOPSIS
//
	class encoded_char_range::decoder
//
// DESCRIPTION
//
//	An encoded_char_range::decoder is used to keep decoders' state between
//	calls and reset state between files to their initial states just before
//	starting to index a file.
//
//*****************************************************************************
{
public:
	typedef encoded_char_range::value_type value_type;
	typedef encoded_char_range::pointer pointer;

	static void	reset_all();
protected:
	decoder()	{ set_.insert( this ); }

	virtual void	reset() = 0;
private:
	typedef std::set< decoder* > set_type;
	static set_type set_;
};
#endif	/* IMPLEMENT_DECODING */

////////// encoded_char_range inlines /////////////////////////////////////////

// I hate lots of typing.
#define ECR	encoded_char_range
#define	ECR_CI	ECR::const_iterator

inline ECR::ECR(
	pointer begin, pointer end, charset_type charset, encoding_type encoding
) :
	begin_( begin ), end_( end )
#ifdef	IMPLEMENT_DECODING
	, charset_( charset ), encoding_( encoding )
#endif
{
}

inline ECR::ECR( const_iterator const &i ) :
	begin_( i.pos_ ), end_( i.end_ )
#ifdef	IMPLEMENT_DECODING
	, charset_( i.charset_ ), encoding_( i.encoding_ )
#endif
{
}

inline ECR::ECR( const_iterator const &begin, const_iterator const &end ) :
	begin_( begin.pos_ ), end_( end.pos_ )
#ifdef	IMPLEMENT_DECODING
	, charset_( begin.charset_ ), encoding_( begin.encoding_ )
#endif
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

inline ECR_CI::const_iterator(
	pointer begin, pointer end,
	charset_type charset, encoding_type encoding
) :
	encoded_char_range( begin, end, charset, encoding ), pos_( begin )
#ifdef	IMPLEMENT_DECODING
	, decoded_( false )
#endif
{
}

inline ECR_CI::const_iterator( ECR const *ecr, pointer start_pos ) :
	encoded_char_range(
		start_pos, ecr->end_
#ifdef	IMPLEMENT_DECODING
		, ecr->charset_, ecr->encoding_
#endif
	),
	pos_( start_pos )
#ifdef	IMPLEMENT_DECODING
	, decoded_( false )
#endif
{
}

#ifdef	IMPLEMENT_DECODING
//*****************************************************************************
//
// SYNOPSIS
//
	inline void ECR_CI::decode() const
//
// DESCRIPTION
//
//	Decode the character at the iterator's current position according to
//	the character range's content-transfer-encoding.
//
// RETURN VALUE
//
//	Returns the decoded character.
//
//*****************************************************************************
{
	//
	// Remember the current position to allow the decoders to advance
	// through the encoded text.  This allows the delta to be computed so
	// the iterator can be incremented later.
	//
	pointer c = pos_;
	//
	// A mail message can have both an encoding and a non-ASCII or
	// non-ISO-8859-1 charset simultaneously, e.g., base64-encoded UTF-8.
	// (In practice, this particular case should never happen since UTF-7
	// should be used instead; but you get the idea.)
	//
	// However, handling both an encoding and such a charset simultaneously
	// is a real pain because both can use multiple characters to decode a
	// single character and keeping track of both positions is messy and I
	// didn't feel like thinking about this just now.
	//
	// Hence, a current caveat is that a mail message or attachment can
	// have EITHER an encoding OR a non-ASCII/ISO-8859-1 character set, but
	// not both.  If it does, the encoding takes precedence.
	//
	if ( encoding_ )
		ch_ = (*encoding_)( begin_, c, end_ );
	else if ( charset_ )
		ch_ = (*charset_)( begin_, c, end_ );
	else
		ch_ = iso8859_1_to_ascii( *c++ );
	delta_ = c - pos_;
}
#endif	/* IMPLEMENT_DECODING */

//*****************************************************************************
//
// SYNOPSIS
//
	inline ECR::value_type ECR_CI::operator*() const
//
// DESCRIPTION
//
//	Dereference an encoded_char_range::const_iterator at its current
//	position.
//
// RETURN VALUE
//
//	Returns the decoded character.
//
//*****************************************************************************
{
#ifdef	IMPLEMENT_DECODING
	if ( !decoded_ ) {
		decode();
		decoded_ = true;
	}
	return ch_;
#else
	return iso8859_1_to_ascii( *pos_ );
#endif
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline ECR_CI& ECR_CI::operator++()
//
// DESCRIPTION
//
//	Pre-increment the iterator's position by one.
//
// RETURN VALUE
//
//	Returns a reference to the given object.
//
//*****************************************************************************
{
#ifdef	IMPLEMENT_DECODING
	if ( decoded_ ) {
		//
		// The character at the current position has previously been
		// decoded so we know the delta.  However, since we're about to
		// increment the position to the next character, that character
		// will no longer have been decoded, so set decoded_ to false.
		//
		decoded_ = false;
	} else {
		//
		// The character at the current position has not previously
		// been decoded so we don't know the delta: call decode() to
		// calculate the delta only.  We can't set decoded_ to true
		// since we're about to increment the position to the next
		// character and that character hasn't been decoded.
		//
		decode();
	}
#endif
	prev_ = pos_;
#ifdef	IMPLEMENT_DECODING
	pos_ += delta_;
#else
	++pos_;
#endif
	return *this;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline ECR_CI ECR_CI::operator++(int)
//
// DESCRIPTION
//
//	Post-increment the iterator's position by one.
//
// RETURN VALUE
//
//	Returns a reference to the original (pre-incremented) object.
//
//*****************************************************************************
{
	ECR_CI const temp = *this;
	return ++*this, temp;
}

//*****************************************************************************
//
//	Equality operators.
//
//*****************************************************************************

inline bool operator==( ECR_CI const &e1, ECR_CI const &e2 ) {
	return e1.pos_ == e2.pos_;
}

inline bool operator==( ECR_CI const &e, ECR_CI::pointer p ) {
	return e.pos_ == p;
}

inline bool operator==( ECR_CI::pointer p, ECR_CI const &e ) {
	return e == p;
}

inline bool operator!=( ECR_CI const &e1, ECR_CI const &e2 ) {
	return !( e1 == e2 );
}

inline bool operator!=( ECR_CI const &e, ECR_CI::pointer p ) {
	return !( e == p );
}

inline bool operator!=( ECR_CI::pointer p, ECR_CI const &e ) {
	return e != p;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline char *to_lower( ECR const &range )
//
// DESCRIPTION
//
//	Return a pointer to a string converted to lower case taking the
//	encoding of the characters into account; the original string is
//	untouched.  The string returned is from an internal pool of string
//	buffers.  The time you get into trouble is if you hang on to more then
//	Num_Buffers strings.  This doesn't normally happen in practice,
//	however.
//
// PARAMETERS
//
//	c	The iterator to use.
//
// RETURN VALUE
//
//	A pointer to the lower-case string.
//
//*****************************************************************************
{
	extern char_buffer_pool<128,5> lower_buf;
	register char *p = lower_buf.next();
	for ( ECR_CI c = range.begin(); !c.at_end(); ++c )
		*p++ = to_lower( *c );
	*p = '\0';
	return lower_buf.current();
}

#undef	ECR_CI
#undef	ECR

#endif	/* encoded_char_H */
/* vim:set noet sw=8 ts=8: */
