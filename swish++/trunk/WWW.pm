##
#	WWW.pm: World Wide Web package
#
#	Copyright (C) 1996  Paul J. Lucas
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
# 
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
# 
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, write to the Free Software
#	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

package		WWW;
use		Exporter();
@ISA =		qw( Exporter );
@EXPORT =	qw(
			$description::chars
			$description::words
			extract_description
			extract_meta
			hyperlink
			trim_whitespace
			url_decode url_encode
		);

sub private_extract {
	my $path_name = shift;
	if ( $path_name ne $extract::path_name ) {
		open( FILE, $path_name ) or die "can not open $path_name\n";
		read( FILE, $extract::text, $description::chars );
		close FILE;
		$extract::text =~ s/\s+/ /g;	# turn into one big long string
		$extract::path_name = $path_name;
	}
}

###############################################################################
#
# SYNOPSIS
#
	sub extract_description
#
# DESCRIPTION
#
#	Extract a description from a file by looking at an initial chunk of
#	text.  If the file is HTML and it has a META description (or Dublin
#	Code DC.description) tag, use that description.  Also extract text
#	from ALT attributes.  Strip all HTML tags and return the number of
#	words requested.
#
# SEE ALSO
#
#	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "Notes on helping
#	search engines index your Web site," HTML 4.0 Specification, Appendix
#	B: "Performance, Implementation, and Design Notes," World Wide Web
#	Consortium, April 1998.
#		http://www.w3.org/TR/REC-html40/appendix/notes.html#recs
#
#	---. "Objects, Images, and Applets: How to specify alternate text,"
#	HTML 4.0 Specification, section 13.8, World Wide Web Consortium,
#	April 1998.
#		http://www.w3.org/TR/REC-html40/struct/objects.html#h-13.8
#
#	Dublin Core Directorate.  "The Dublin Core: A Simple Content
#	Description Model for Electronic Resources."
#		http://purl.oclc.org/dc/
#
###############################################################################
{
	my $path_name = shift;
	private_extract( $path_name );
	my $s = $extract::text;

	for ( $extract::path_name ) {

	/\.[a-z]?html?$/i && do {		# file is HTML

		my $m = extract_meta( $extract::path_name, 'description' );
		return $m if $m;

		$m = extract_meta( $extract::path_name, 'DC.description' );
		return $m if $m;

		# Zap HTML comments and a partial, trailing comment, if any.
		$s =~ s/<!--.*?-->//g;
		$s =~ s/<!--.*\s*$//;

		# Zap text between SCRIPT, STYLE, and TITLE tags and partial,
		# trailing tag, if any.
		$s =~ s!<(SCRIPT|STYLE|TITLE).*?>.*?</\1>!!gi;
		$s =~ s/<(?:SCRIPT|STYLE|TITLE).*$//i;

		# Extract text from ALT attributes of tags and partial,
		# trailing tag, if any.
		$s =~ s/<[^>]+?ALT\s*=\s*(['"])([^>]*?)\1[^>]*?>/$2/gi;
		$s =~ s/<[^>]+?ALT\s*=\s*(['"])([^'"]*)\1?\s*$/$2/i;

		# Zap all remaining HTML tags and partial, trailing tag, if any.
		$s =~ s/<.*?>//g;
		$s =~ s/<.*$//;

		# no last;
	};

	/\.(?:[a-z]?html?|txt)$/i && do {	# file is plain text or HTML

		# Zap leading whitespace.
		$s =~ s/^\s+//;

		# Return at most $description::words words.
		$s =~ s/^((?:\S+\s+){$description::words}).*/$1/;

		last;
	};

	/./ && do {				# file is neither text nor HTML
		$s = '';
	};

	}
	return $s;
}

###############################################################################
#
# SYNOPSIS
#
	sub extract_meta
#
# DESCRIPTION
#
#	For an HTML file, extract the value of the CONTENT attribute from a
#	META element having the given NAME attribute by looking at an initial
#	chunk of text for META elements in general.  If no META element is
#	found having the given NAME element, return null.
#
#	The chunk of text is cached between calls for a given filename.
#
###############################################################################
{
	my( $path_name, $meta_name ) = @_;
	private_extract( $path_name );

	if ( $extract::path_name =~ /\.[a-z]?html?$/i ) {	# file is HTML

		# If a <META NAME=$meta_name CONTENT="...">, return content.
		return $3 if $extract::text =~ /<META\s[^>]*?NAME\s*=\s*(['"])?$meta_name\1?\s[^>]*?CONTENT\s*=\s*(['"])([^>]+)\2[^>]*?>/i;

		# If a <META CONTENT="..." NAME=$meta_name>, return content.
		return $2 if $extract::text =~ /<META\s[^>]*?CONTENT\s*=\s*(['"])([^>]+)\1\s[^>]*?NAME\s*=\s*(?:['"])?${meta_name}[^>]*?>/i;
	}
	return '';
}

###############################################################################
#
# SYNOPSIS
#
	sub hyperlink
#
# DESCRIPTION
#
#	Make valid URLs embedded in the text selectable by wrapping hyperlinks
#	around them.
#
###############################################################################
{
	for ( @_ ) {
		s/</\&lt;/g;			# translate problematic HTML...
		s/>/\&gt;/g;			# ...characters first

		s!$url_syntax{ ftp    }!<A HREF="$&">$&</A>!g;
		s!$url_syntax{ gopher }!<A HREF="$&">$&</A>!g;
		s!$url_syntax{ http   }!<A HREF="$&">$&</A>!g;
		s!$url_syntax{ e_mail }!<A HREF="mailto:$&">$&</A>!g;
		s!$url_syntax{ news   }!<A HREF="$&">$&</A>!g;
		s!$url_syntax{ telnet }!<A HREF="$&">$&</A>!g;
		s!$url_syntax{ wais   }!<A HREF="$&">$&</A>!g;
	}
}

###############################################################################
#
# SYNOPSIS
#
	sub trim_whitespace
#
# DESCRIPTION
#
#	Remove both leading and trailing whitespace from strings.
#
# SEE ALSO
#
#	Larry Wall, et al.  "Programming Perl," 2nd ed., O'Reilly and
#	Associates, Inc., Sebestapol, CA, 1996, p. 73.
#
###############################################################################
{
	map { s/^\s*(.*?)\s*$/$1/ } @_;
}

###############################################################################
#
# SYNOPSIS
#
	sub url_decode
#
# DESCRIPTION
#
#	Decode all URL-encoded characters in strings.
#
# SEE ALSO
#
#	url_encode()
#
###############################################################################
{
	for ( @_ ) {
		tr/+/ /;
		s/%([a-fA-F0-9]{2})/pack( 'C', hex($1) )/eg;
	}
}
 
###############################################################################
#
# SYNOPSIS
#
	sub url_encode
#
# DESCRIPTION
#
#	URL-encode (%xx) all "unsafe" characters according to RFC 1738.
#
# SEE ALSO
#
#	Larry Masinter and Mark McCahill.  "RFC 1738: Uniform Resource
#	Locators," Network Working Group of the Internet Engineering Task
#	Force, December 1994.
#
###############################################################################
{
	for ( @_ ) {
		s/[][\0-\x1F"#%&+\/:;<=>?@\\^`{|}~\x7F]/sprintf( "%%%02X", ord( $& ) )/eg;
		tr/ /+/;
	}
}

###############################################################################
#
# SYNOPSIS
#
	sub parse_urlencoded
#
# DESCRIPTION
#
#	Parse application/x-www-form-urlencoded form data.  Values are placed
#	into the global hash %FORM.  Leading and trailing whitespace
#	characters are removed from values.  Multiple values for the same key
#	are separated by a null character.
#
# SEE ALSO
#
#	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "Form content types,"
#	HTML 4.0 Specification, W3C Recommendation, section 17.13.4, April
#	1998.  http://www.w3.org/TR/REC-html40/
#
###############################################################################
{
	for ( split( /&/, $_[0] ) ) {
		# All other Perl web packages I've encountered have a bug by
		# not specifying the '2' below.
		my( $key, $value ) = split( /=/, $_, 2 );

		# add value to hash only if it's not null
		url_decode( $value );
		trim_whitespace( $value );
		next if $value eq '';

		# if the key already has a value, append the new value
		url_decode( $key );
		if ( $::FORM{ $key } ) {
			$::FORM{ $key } .= "\0$value";
		} else {
			$::FORM{ $key } = $value;
		}
	}
}

###############################################################################
#
# SYNOPSIS
#
	sub parse_multipart
#
# DESCRIPTION
#
#	Parse multipart/form-data used by form-based file upload.  Non-file
#	form data is handled in exactly the same way as it is in the
#	parse_urlencoded function.
#
#	Files uploaded are placed into the /tmp directory; filenames have non-
#	Unix-friendly characters stripped and spaces are converted to
#	underscores.  After upload, if a file appears to be a text file,
#	carriage returns or carriage-return/newline sequences are replaced by
#	(just) carriage returns.  Exception: PostScript and PDF files are not
#	touched.
#
# SEE ALSO
#
#	parse_urlencoded()
#
#	Larry Masinter and Ernesto Nebel.  "RFC 1867: Form-based File Upload
#	in HTML," Network Extensions Working Group of the Internet Engineering
#	Task Force, November 1995.
#
###############################################################################
{
	my( $boundary ) = @_;
	while ( <STDIN> ) {
		next unless /Content-Disposition: [^\r\n]+\r?\n?/i;
		my( $key ) = /name="([^"]+)"/i;
		my( $file_name ) = /filename="([^"]*)"/i;
		( $file_name ) = $file_name =~ m!([^\\/]+)$!;
		trim_whitespace( $file_name );

		##
		# Skip any other headers.
		##
		while ( <STDIN> ) {
			last if /^\s*$/;
		}

		if ( !$file_name ) {
			##
			# Simple field
			##
			my $value;
			while ( <STDIN> ) {
				s/\r$//;
				if ( /$boundary/o ) {
					trim_whitespace( $value );
					# if the key already has a value,
					# append the new value
					if ( $::FORM{ $key } ) {
						$::FORM{ $key } .= "\0$value";
					} else {
						$::FORM{ $key } = $value;
					}
					last;
				}
				$value .= $_;
			}
		} else {
			##
			# File upload
			##
			$file_name =~ s![]["#\$&'()*/;<>?\\`|~]!!g;
			$file_name =~ tr/ /_/s;
			$::FORM{ $key } = $file_name;
			my( $dest_file ) = "/tmp/$file_name";
			open( FILE, ">$dest_file" ) or die;
			my $p;
			while ( <STDIN> ) {
				if ( /$boundary/o ) {
					$p =~ s/\r?\n$//;
					print FILE $p;
					last;
				}
				print FILE $p;
				$p = $_;
			}
			close FILE;
			if ( -T $dest_file ) {
				open( FILE, $dest_file ) or die;
				$_ = join( '', <FILE> );
				close FILE;
				next if /^%(?:!|PDF)/; # don't touch PS/PDF
				s/\r\n?/\n/gm;
				unlink( $dest_file );
				open( FILE, ">$dest_file" ) or die;
				print FILE;
				close FILE;
			}
		}
	}
}

BEGIN {
	##### URL syntax ######################################################

	# Tim Berners-Lee.  "RFC 1630: Universal Resource Identifiers in WWW,"
	# Network Working Group of the Internet Engineering Task Force, June
	# 1994.

	# common sub-parts
	my $url_user_password = '(?:[a-zA-Z][-\w.]+(?::\w+)?@)?';
	my $url_host = '\w[-\w.]*\w';
	my $url_port = '(?::\d+)?';
	my $url_path = '(?:[-%/\w.~]*[-%/\w~])?';
	my $url_query = '(?:\?[-!$%&\'()*+,./\w=?~]+)?';

	$url_syntax{ ftp } =
		"ftp://$url_user_password$url_host$url_port$url_path";

	$url_syntax{ gopher } =
		"gopher://$url_host$url_port$url_path";

	$url_syntax{ http } =
		"https?://$url_host$url_port$url_path$url_query";

	$url_syntax{ e_mail } = '[a-zA-Z][-\w.]+@(?:[-\w]+\.)+[a-zA-Z]{2,3}';
	$url_syntax{ mailto } = "mailto:$url_syntax{ e_mail }";
	$url_syntax{ news   } = 'news:[a-z][-\w.]*\w';

	$url_syntax{ telnet } =
		"telnet://$url_user_password$url_host$url_port";

	$url_syntax{ wais } =
		"wais://$url_host$url_port$url_path$url_query";

	##### Parse form data #################################################

	if ( $::ENV{ CONTENT_TYPE } eq 'application/x-www-form-urlencoded' ) {
		read( STDIN, $_, $::ENV{ CONTENT_LENGTH } );
		parse_urlencoded( $_ );
	} elsif ( $::ENV{ CONTENT_TYPE } =~ m!^multipart/form-data!i ) {
		my( $boundary ) = $::ENV{ CONTENT_TYPE } =~ /boundary=(\S+)$/;
		parse_multipart( $boundary );
	}
	parse_urlencoded( $::ENV{ QUERY_STRING } );

	##### Miscellaneous ###################################################

	$description::chars = 2048;
	$description::words = 50;
}

1;
