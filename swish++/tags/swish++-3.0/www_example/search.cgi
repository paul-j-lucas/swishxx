#! /usr/local/bin/perl
###############################################################################
#
# NAME
#
#	search.cgi
#
# SYNOPSIS
#
#	<FORM ACTION="search.cgi">
#	<INPUT TYPE=text NAME=search>
#	<INPUT TYPE=submit VALUE="Search">
#	</FORM>
#
# DESCRIPTION
#
#	This is a toy example CGI script written in Perl 5 to show how to
#	interface SWISH++ to a web-based search form.  It uses the supplied
#	WWW package to parse form data.
#
# SEE ALSO
#
#	search(1), WWW(3)
#
###############################################################################

use lib ( '/home/www/swish++/lib' );
#		Put the path to where the WWW library is above.
require WWW;

$SWISH_BIN =	'/home/www/swish++/bin';
#		The full path to the bin directory where you installed the
#		SWISH++ executables.

$DOC_ROOT =	'/home/www/httpd/htdocs';
#		The top-level directory for your document tree presumeably
#		where the index was generated from.

$INDEX_FILE =	'/home/www/the.index';
#		The full path to the index file to be searched through.

print "Content-Type: text/html\n\n";

# Header HTML
print <<END;
<HTML>
<HEAD>
<TITLE>Search Results</TITLE>
</HEAD>
<BODY>
<BIG><B>Search Results</B></BIG>
<HR>
<TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0>
END

##
# Zap dangerous characters before exposing to shell; escape rest.
##
$FORM{ search } =~ s/[^\s&'()*\-=\w]/ /g;
$FORM{ search } =~ s/([&'()*])/\\$1/g;

open( SEARCH, "$SWISH_BIN/search -i $INDEX_FILE $FORM{ search } |" ) or die;
while ( <SEARCH> ) {
	if ( /^# ignored: (.+)$/ ) {
		$ignored = $1;
		next;
	}
	##
	# Future releases of SWISH++ may emit other comments: ignore ones we
	# don't know about.
	##
	next if /^#/;

	my( $rank, $file, $size, $title ) = split( / /, $_, 4 );

	my $desc = WWW::extract_description( "$DOC_ROOT/$file" );
	WWW::hyperlink( $desc );
	$size /= 1024;
	$size = '&lt;1K' unless $size;

	print <<END;
	<TR VALIGN=top><TD ALIGN=right>$rank%&nbsp;&nbsp;</TD>
	<TD><DL><DT><B><A HREF="$file">$title</A></B> ($size)<DD>$desc</DL></TD>
END
}
close SEARCH;

print "</TABLE>\n";
if ( $? ) {
	print "<B>malformed query</B>\n";
} else {
	print "<P>ignored: $ignored\n" if $ignored;
}

# Footer HTML
print <<END;
</BODY>
</HTML>
END
