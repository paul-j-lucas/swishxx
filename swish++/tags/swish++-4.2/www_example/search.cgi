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
#	<INPUT TYPE=text NAME=Search><BR>
#	<INPUT TYPE=checkbox NAME=Stem><B>Stem words</B>
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
#	Larry Wall, et al.  "Programming Perl," 2nd ed., O'Reilly and
#	Associates, Inc., Sebestapol, CA, 1996, p. 73.
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

#$SOCKET_FILE =	'/tmp/swish++.socket';
#		The full path to the socket file.  Uncomment this only if you
#		run 'search' as a daemon.

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
$FORM{ Search } =~ s/[^\s&'()*\-=\w]/ /g;
$FORM{ Search } =~ s/([&'()*])/\\$1/g;

##
# Add in specified options.
##
push( @options, '-s' ) if $FORM{ Stem };

##
# Call 'search' either as a server or as a command.  In a real CGI, as opposed
# to this toy example, you would have the code only for the case you are
# actually doing.
##
if ( $SOCKET_FILE ) {
	##
	# Connect to the 'search' server via a Unix domain socket.  See [Wall],
	# p. 353.
	##
	use Socket;
	socket( SEARCH, PF_UNIX, SOCK_STREAM, 0 ) or die "socket: $!";
	connect( SEARCH, sockaddr_un( $SOCKET_FILE ) ) or die "connect: $!";

	##
	# We *MUST* set autoflush for the socket filehandle otherwise the
	# server thread will hang since I/O buffering will wait for the buffer
	# to fill that will never happen since queries are short.  See [Wall],
	# p. 130, 211.
	##
	my $old_fh = select( SEARCH ); $| = 1; select( $old_fh );

	##
	# We also *MUST* print a trailing newline since the server reads an
	# entire line of input (so therefore it looks and waits for a newline).
	##
	print SEARCH "search @options $FORM{ Search }\n";
} else {
	##
	# Open a pipe from the 'search' command.
	##
	open( SEARCH, "$SWISH_BIN/search -i $INDEX_FILE @options $FORM{ Search } |" ) or
		die;
}

while ( <SEARCH> ) {
	if ( /^# ignored: / ) {
		$ignored = $';
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
