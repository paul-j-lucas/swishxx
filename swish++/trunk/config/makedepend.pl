##
#	PJL Script Library
#	config/makedepend.pl
#
#	Copyright (C) 2004  Paul J. Lucas
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

$ME = basename( $0 );				# basename of executable

sub usage {
	die "usage: $ME [-I dir]... file\n";
}

##
# Go though all command-line arguments and capture -I options; ignore all
# others; stop when we encounter a non-option.
##
my $arg_is_next;
my @dirs = ( '.' );
while ( defined( $_ = shift( @ARGV ) ) ) {
	/(.+)/ && $arg_is_next && do {
		push( @dirs, $1 );
		$arg_is_next = 0;
		next;
	};
	/^[^-]/ && do {
		unshift( @ARGV, $_ );
		last;
	};
	/^-I(.*)/ && do {
		if ( $1 ) {
			push( @dirs, $1 );
		}  else {
			$arg_is_next = 1;
		}
		next;
	};
}

$#ARGV + 1 == 1 or usage();
( $SOURCE_FILE ) = @ARGV;
map { s!/$!!; } @dirs;				# get rid of trailing /'s
push( @files, $SOURCE_FILE );			# prime the pump

##
# Process all files.
##
while ( my $file = shift( @files ) ) {
	##
	# Look in all directories in @dirs for the current file.
	##
	my $dir;
	for ( @dirs ) {
		my $path = "$_/$file";
		next unless open( SOURCE, $path );
		$deps{ $path } = 1;
		$dir = $_;
		last;
	}
	die "$ME: error: can not open $file\n" unless $dir;

	##
	# Pluck files #include'd and add them to the list of files to process
	# only if we haven't seen them before.
	##
	while ( <SOURCE> ) {
		next unless /^#\s*include\s+"([^"]+)"/;
		push( @files, $1 ) unless exists $deps{ "$dir/$1" };
	}
	close( SOURCE );
}

##
# Print the file and its dependencies.
##
( $OBJECT_FILE = $SOURCE_FILE ) =~ s/\.\w+$/\.o/;
print "$OBJECT_FILE : ", join( ' ', keys %deps ), "\n";
