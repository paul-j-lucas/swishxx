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

// local
#include "util.h"			/* for to_lower() */
#include "word_util.h"

enum content_transfer_encoding {
	Seven_Bit,
	Eight_Bit,
	Binary,
#ifdef MOD_MAIL
	Quoted_Printable,
	Base64,
#endif
};

//*****************************************************************************
//
// SYNOPSIS
//
	class encoded_char_range
//
// DESCRIPTION
//
//	An encoded_char_range is an abstraction that contains a range of
//	characters in memory that are encoded according to some scheme (a
//	Content-Transfer-Encoding) such as Quoted-Printable or Base64.  A
//	const_iterator is used to iterate over the range and, when
//	dereferenced, decodes a character.
//
//	However, doing this is a serious performance hit since it has to be
//	done for every single character examined.  Hence, the code is #ifdef'd
//	for MOD_MAIL: if MOD_MAIL isn't used, there's no need for any special
//	decoding.
//
//*****************************************************************************
{
public:
	typedef ptrdiff_t difference_type;
	typedef char value_type;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
	typedef value_type& reference;
	typedef value_type const& const_reference;

	class const_iterator;
	friend class const_iterator;

	encoded_char_range(
		const_pointer begin, const_pointer end,
		content_transfer_encoding = Eight_Bit
	);
	encoded_char_range( const_iterator const &pos );
	encoded_char_range(
		const_iterator const &begin, const_iterator const &end
	);

	// default copy constructor is fine
	// default assignment operator is fine

	const_iterator	begin() const;
	const_pointer	begin_pos() const		{ return begin_; }
	void		begin_pos( const_pointer p )	{ begin_ = p; }
	void		begin_pos( const_iterator const& );
	const_iterator	end() const;
	const_pointer	end_pos() const			{ return end_; }
	void		end_pos( const_pointer p )	{ end_ = p; }
	void		end_pos( const_iterator const& );

	content_transfer_encoding	encoding() const { return encoding_; }
protected:
	encoded_char_range() { }

	const_pointer			begin_;
	const_pointer			end_;
	content_transfer_encoding	encoding_;
};

//*****************************************************************************
//
// SYNOPSIS
//
	class encoded_char_range::const_iterator :
		public encoded_char_range,
		public forward_iterator< value_type, difference_type >
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
	typedef encoded_char_range::const_pointer const_pointer;
	typedef encoded_char_range::reference reference;
	typedef encoded_char_range::const_reference const_reference;

	const_iterator() { }
	const_iterator(
		const_pointer begin, const_pointer end,
		content_transfer_encoding = Eight_Bit
	);

	// default copy constructor is fine
	// default assignment operator is fine

	value_type	operator*() const;
	const_iterator&	operator++();
	const_iterator	operator++(int);

	bool		at_end() const			{ return pos_ == end_; }

	const_pointer	pos() const			{ return pos_; }
	const_pointer&	pos()				{ return pos_; }
	const_pointer	prev_pos() const		{ return prev_; }

	friend bool operator==( const_iterator const&, const_iterator const& );
	friend bool operator==( const_iterator const&, const_pointer );
private:
	mutable const_pointer	pos_;
	mutable const_pointer	prev_;
	mutable value_type	ch_;
#ifdef MOD_MAIL
	mutable bool		decoded_;
	mutable int		delta_;
#endif
	const_iterator( encoded_char_range const*, const_pointer start_pos );
	friend class	encoded_char_range;	// for access to c'tor above

	void		decode() const;
#ifdef MOD_MAIL
	static value_type decode_base64(
		const_pointer begin, const_pointer &pos, const_pointer end
	);
	static value_type decode_quoted_printable(
		const_pointer &pos, const_pointer end
	);
	static value_type decode_quoted_printable_aux(
		const_pointer &pos, const_pointer end
	);
#endif
};

// I hate lots of typing.
#define	ECR_CI encoded_char_range::const_iterator

////////// encoded_char_range inlines /////////////////////////////////////////

inline encoded_char_range::encoded_char_range(
	const_pointer begin, const_pointer end,
	content_transfer_encoding encoding
) :
	begin_( begin ), end_( end ), encoding_( encoding )
{
}

inline encoded_char_range::encoded_char_range( const_iterator const &i ) :
	begin_( i.pos_ ), end_( i.end_ ), encoding_( i.encoding_ )
{
}

inline encoded_char_range::encoded_char_range(
	const_iterator const &begin, const_iterator const &end
) :
	begin_( begin.pos_ ), end_( end.pos_ ), encoding_( begin.encoding_ )
{
}

inline ECR_CI encoded_char_range::begin() const {
	return const_iterator( this, begin_ );
}

inline ECR_CI encoded_char_range::end() const {
	return const_iterator( this, end_ );
}

inline void encoded_char_range::begin_pos( const_iterator const &i ) {
	begin_ = i.pos_;
}

inline void encoded_char_range::end_pos( const_iterator const &i ) {
	end_ = i.pos_;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline ECR_CI::const_iterator(
		const_pointer begin, const_pointer end,
		content_transfer_encoding encoding = Eight_Bit
	)
//
// DESCRIPTION
//
//	Construct a const_iterator.
//
// PARAMETERS
//
//	begin		An pointer marking the beginning of the range.
//
//	end		An pointer marking the end of the range.
//
//	encoding	The Content-Transfer-Encoding of the text.
//
//*****************************************************************************
	: encoded_char_range( begin, end, encoding ), pos_( begin )
#ifdef MOD_MAIL
	  , decoded_( false )
#endif
{
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline ECR_CI::const_iterator(
		encoded_char_range const *ecr, const_pointer start_pos
	)
//
// DESCRIPTION
//
//	Construct a const_iterator.
//
// PARAMETERS
//
//	ecr		The encoded_char_rage to iterate over.
//
//	start_pos	A pointer marking the position where the this pointer
//			is to start.
//
//*****************************************************************************
	: encoded_char_range( start_pos, ecr->end_, ecr->encoding_ ),
	  pos_( start_pos )
#ifdef MOD_MAIL
	  , decoded_( false )
#endif
{
}

#ifdef MOD_MAIL
//*****************************************************************************
//
// SYNOPSIS
//
	inline encoded_char_range::value_type ECR_CI::decode_quoted_printable(
		const_pointer &c, const_pointer end
	)
//
// DESCRIPTION
//
//	Check to see if the character at the current position is an '=': if
//	not, the character is an ordinary character; if so, the character is a
//	quoted-printable encoded character and needs to be decoded.
//
//	The reason for this function is to serve an inlined front-end so as to
//	call the more expensive out-of-lined only if the character really needs
//	to be decoded.
//
// PARAMETERS
//
//	c	An pointer marking the position of the character to be decoded.
//
//	end	An pointer marking the end of the text.
//
// RETURN VALUE
//
//	Returns the decoded character.
//
//*****************************************************************************
{
	if ( *c != '=' )
		return *c++;
	return ++c != end ? decode_quoted_printable_aux( c, end ) : ' ';
}
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	inline void ECR_CI::decode() const
//
// DESCRIPTION
//
//	Decode the character at the iterator's currend position according to
//	the character range's content-transfer-encoding.
//
// RETURN VALUE
//
//	Returns the decoded character.
//
//*****************************************************************************
{
#ifdef MOD_MAIL
	//
	// Remember the current position to allow the decoders to advance
	// through the encoded text.  This allows the delta to be computed so
	// the iterator can be incremented later.
	//
	const_pointer c = pos_;
	switch ( encoding_ ) {
		case Base64:
			ch_ = decode_base64( begin_, c, end_ );
			break;
		case Quoted_Printable:
			ch_ = decode_quoted_printable( c, end_ );
			break;
		default:
			ch_ = *c++;
	}
	delta_ = c - pos_;
#else
	ch_ = *pos_;
#endif
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline encoded_char_range::value_type ECR_CI::operator*() const
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
#ifdef MOD_MAIL
	if ( !decoded_ ) {
		decode();
		decoded_ = true;
	}
#endif
	return ch_;
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
#ifdef MOD_MAIL
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
#ifdef MOD_MAIL
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
	operator++();
	return temp;
}

//*****************************************************************************
//
//	Equality operators.
//
//*****************************************************************************

inline bool operator==( ECR_CI const &e1, ECR_CI const &e2 ) {
	return e1.pos_ == e2.pos_;
}

inline bool operator==( ECR_CI const &e, ECR_CI::const_pointer p ) {
	return e.pos_ == p;
}

inline bool operator==( ECR_CI::const_pointer p, ECR_CI const &e ) {
	return e == p;
}

inline bool operator!=( ECR_CI const &e1, ECR_CI const &e2 ) {
	return !( e1 == e2 );
}

inline bool operator!=( ECR_CI const &e, ECR_CI::const_pointer p ) {
	return !( e == p );
}

inline bool operator!=( ECR_CI::const_pointer p, ECR_CI const &e ) {
	return e != p;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline char *to_lower( encoded_char_range const &range )
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
	for ( encoded_char_range::const_iterator
		c = range.begin(); !c.at_end(); ++c
	)
		*p++ = to_lower( *c );
	*p = '\0';
	return lower_buf.current();
}

#undef	ECR_CI

#endif	/* encoded_char_H */
