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

	##### Miscellaneous ###################################################

	$description::chars = 2048;
	$description::words = 50;
}

1;
