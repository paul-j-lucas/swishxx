#! /bin/sh
##
#       SWISH++
#       test/renew_expected.sh
#
#       Copyright (C) 2016  Paul J. Lucas
#
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the Licence, or
#       (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public License
#       along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

# Uncomment the following line for shell tracing.
#set -x

########## Functions ##########################################################

local_basename() {
  ##
  # Autoconf, 11.15:
  #
  # basename
  #   Not all hosts have a working basename. You can use expr instead.
  ##
  expr "//$1" : '.*/\(.*\)'
}

########## Initialize #########################################################

BUILD_SRC=../src
DATA_DIR=data
EXPECTED_DIR=expected
TESTS_DIR=tests

########## Run test ###########################################################

run_test_file() {
  TEST_FILE="$1"
  EXPECTED_FILE="$2"

  IFS='|' read COMMAND CONF OPTIONS ARGS EXPECTED_EXIT < $TEST_FILE

  if [ "$EXPECTED_EXIT" -eq 0 ]
  then
    # trim whitespace
    COMMAND=`echo $COMMAND`
    CONF=`echo $CONF`

    if [ "x$COMMAND" = "xindex" ]
    then CHDIR="-d $DATA_DIR"
    else unset CHDIR
    fi
    [ "x$CONF" = "x" ] || CONF="-c $DATA_DIR/$CONF"

    echo $COMMAND $CHDIR $CONF $OPTIONS $ARGS "> $EXPECTED_FILE"
    $COMMAND $CHDIR $CONF $OPTIONS $ARGS > $EXPECTED_FILE
  fi
}

##
# Must put BUILD_SRC first in PATH so we get the correct versions.
##
PATH=$BUILD_SRC:$PATH

SWISHXX_TEST=true; export SWISHXX_TEST

grep $TESTS_DIR/ Makefile.am | cut -f2 | sed 's! \\$!!' | while read TEST_FILE
do
  base=`local_basename $TEST_FILE`
  EXPECTED_FILE=`echo $EXPECTED_DIR/$base | sed 's/test$/txt/'`
  run_test_file $TEST_FILE $EXPECTED_FILE
done

# vim:set et sw=2 ts=2:
