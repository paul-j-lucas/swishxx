##
#	SWISH++
#	config/config.pl: Perl script configuration script
#
#	Copyright (C) 1998  Paul J. Lucas
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

$SCRIPT = $ARGV[0];
shift;

for ( @ARGV ) {
	( $k, $v ) = split( /=/, $_, 2 );
	$kv{ $k } = $v;
}

chmod( 0644, $SCRIPT );
open( SCRIPT_IN, "$SCRIPT.in" ) or die "$0: can't open $SCRIPT.in\n";
open( SCRIPT_OUT, ">$SCRIPT" ) or die "$0: can't open $SCRIPT\n";

while ( <SCRIPT_IN> ) {
	s/%%$k%%/$v/g while ( $k, $v ) = each %kv;
	print SCRIPT_OUT;
}

close SCRIPT_OUT;
close SCRIPT_IN;
chmod( 0444, $SCRIPT );
