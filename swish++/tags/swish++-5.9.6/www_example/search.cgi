#! /usr/local/bin/perl
###############################################################################
#
# NAME
#
#	search.cgi
#
# SYNOPSIS
#
#	<form action="search.cgi">
#	  <input type="text" name="Search" /><br />
#	  <input type="checkbox" name="Stem" /><b>Stem words</b>
#	  <input type="submit" value="Search" />
#	</form>
#
# DESCRIPTION
#
#	This is a toy example CGI script written in Perl 5 to show how to
#	interface SWISH++ to a web-based search form.  For errors, it simply
#	calls "die" and the message goes to the web server log file and the
#	user sees "Internal Server Error."  A real CGI script should report
#	errors better.  This is an exercise for the reader.
#
# SEE ALSO
#
#	search(1), CGI(3)
#
#	Larry Wall, et al.  "Programming Perl," 3rd ed., O'Reilly and
#	Associates, Inc., Sebastopol, CA, 2000.
#
###############################################################################

use lib qw( /home/www/swish++/lib/ );
#		Put the path to where the WWW library is above.
require WWW;

$SWISH_BIN =	'/usr/local/bin';
#		The full path to the bin directory where you installed the
#		SWISH++ executables.

$DOC_ROOT =	'/home/www/httpd/htdocs';
#		The top-level directory for your document tree presumeably
#		where the index was generated from.

$INDEX_FILE =	'/home/www/swish++.index';
#		The full path to the index file to be searched through.

#$SOCKET_FILE =	'/tmp/search.socket';
#		The full path to the socket file.  Uncomment this only if you
#		run 'search' as a daemon listening to a Unix domain socket.

#$SOCKET_ADDRESS = '*:1967';
#		The host:port of the TCP socket.  Uncomment this only if you
#		run 'search' as a daemon listening to a TCP socket.

##
# Get Search parameter.
##
use CGI;
my $q = new CGI;
my $search = $q->param( 'Search' ) || die "no Search parameter";

##
# Add in specified options.
##
my @options;
push( @options, '-s' ) if $q->param( 'Stem' );

##
# Call 'search' either as a client or as a command.  In a real CGI, as opposed
# to this toy example, you would have the code only for the case you are
# actually doing.
##
if ( $SOCKET_FILE || $SOCKET_ADDRESS ) {
	use Socket;
	if ( $SOCKET_FILE ) {
		##
		# Connect to the 'search' server via a Unix domain socket.
		##
		socket( SEARCH, PF_UNIX, SOCK_STREAM, 0 ) || die "socket: $!";
		connect( SEARCH, sockaddr_un( $SOCKET_FILE ) ) ||
			die "connect: $!";
	} else {
		##
		# Connect to the 'search' server via a TCP socket.
		##
		my( $host, $port ) = $SOCKET_ADDRESS =~ /(?:([^\s:]+):)?(\d+)/;
		$host = 'localhost' if $host eq '' || $host =~ /^\*?$/;
		my $iaddr = inet_aton( $host ) ||
			die "$me: \"$host\": bad or unknown host\n";
		socket( SEARCH, PF_INET, SOCK_STREAM, getprotobyname('tcp') ) ||
			die "socket: $!";
		connect( SEARCH, sockaddr_in( $port, $iaddr ) ) ||
			die "connect: $!";
	}

	##
	# We *MUST* set autoflush for the socket filehandle otherwise the
	# server thread will hang since I/O buffering will wait for the buffer
	# to fill that will never happen since queries are short.  See [Wall],
	# p. 781.
	##
	select( (select( SEARCH ), $| = 1)[0] );

	##
	# We also *MUST* print a trailing newline since the server reads an
	# entire line of input (so therefore it looks and waits for a newline).
	##
	print SEARCH "search @options $search\n";
} else {
	##
	# Zap dangerous characters before exposing to shell; escape rest.
	##
	$search =~ s/[^\s&'()*\-=\w]/ /g;
	$search =~ s/([&'()*])/\\$1/g;

	##
	# Open a pipe from the 'search' command.
	##
	open( SEARCH, "$SWISH_BIN/search -i $INDEX_FILE @options $search |" ) ||
		die "open: $!";
}

##
# Print header HTML
##
print <<END;
Content-Type: text/html

<html>
<head><title>Search Results</title></head>
<body>
<big><b>Search Results</b></big><hr>
<table border="0" cellpadding="0" cellspacing="0">
END

##
# Read the search results back.
##
while ( <SEARCH> ) {
	if ( /^# ignored: / ) {
		##
		# Get the ignored words so we can report them to the user.
		##
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
	$size = int( $size / 1024 );
	if ( $size ) {
		$size .= 'K';
	} else {
		$size = '&lt;1K';
	}

	print <<END;
	<tr valign="top"><td align="right">$rank%&nbsp;&nbsp;</td>
	<td><dl><dt><b><a href="$file">$title</a></b> ($size)<dd>$desc</dl></td>
END
}
close( SEARCH ) || die "close: $!";

print "</table>\n";
if ( $? ) {
	print "<b>malformed query</b>\n";
} else {
	print "<p>ignored: $ignored\n" if $ignored;
}

##
# Print footer HTML
##
print <<END;
</body>
</html>
END
