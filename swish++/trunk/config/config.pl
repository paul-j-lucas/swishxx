##
#	SWISH++
#	config/config.pl: Perl script configuration script
#
#	Copyright (C) 2000  Paul J. Lucas
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

########## You shouldn't have to change anything below this line. #############

use File::Basename;
use File::Find;

$ME = basename( $0 );				# basename of executable

##
# Check command-line arguments.
##
$#ARGV + 1 == 1 or die "usage: $ME file.in\n";
( $IN_FILE ) = @ARGV;

##
# Populate a key/value variable substitution hash from standard input.
##
while ( <STDIN> ) {
	next if /^\s*#/;			# skip comments
	chop;
	next unless /^\s*(\w+)\s*:?=\s*([^#]+)\s*/;# skip non-assignment lines
	my( $k, $v ) = ( $1, $2 );		# got an assignment
	$v =~ s/\s+$//;				# remove trailing whitespace
	##
	# Perform variable expansion on the RHS of the assignment allowing
	# either $VAR or $(VAR), the latter for 'make' variables.  Do NOT
	# expand \$other, i.e., a literal $.  (See also "Programming Perl," p.
	# 69.)
	#
	# In oder not to expand \$other, change all \$ to <DOLLAR>, i.e., some
	# character sequence not containing a $ and most likely not otherwise
	# appearing in the string.  When done, change them back.
	##
	$v =~ s/\\\$/<DOLLAR>/g;
	$v = "$`$kv{ $1 }$'" while $v =~ /\$\(?(\w+)\)?/;
	$v =~ s/<DOLLAR>/\$/g;

	$kv{ $k } = $v;
}

##
# Perform substitutions in file or files.
##
sub substitute {
	return unless /\.in$/ && -T $_;
	my $file_out = $_;
	$file_out =~ s/\.in$//;
	unless ( open( FILE_IN, $_ ) ) {
		warn "$ME: can not read $_\n";
		return;
	}
	unless ( open( FILE_OUT, ">$file_out" ) ) {
		warn "$ME: can not write $file_out\n";
		close( FILE_IN );
		return;
	}
	my( $mode, $uid, $gid ) = (stat( _ ))[2,4,5];
	while ( <FILE_IN> ) {
		s/%%$k%%/$v/g while ( $k, $v ) = each %kv;
		print FILE_OUT;
	}
	close( FILE_OUT );
	close( FILE_IN );
	chmod( "0$mode", $file_out );
	chown( $uid, $gid, $file_out );
}
find( \&substitute, $IN_FILE );
