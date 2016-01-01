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
  TEST_FILE=$1
  EXPECTED_FILE=$2

  IFS='|' read COMMAND CONF OPTIONS ARGS EXPECTED_EXIT < $TEST_FILE

  # trim whitespace
  COMMAND=`echo $COMMAND`
  CONF=`echo $CONF`

  if [ "$COMMAND" = "index" ]
  then CHDIR="-d $DATA_DIR"
  else unset CHDIR
  fi
  [ "$CONF" ] && CONF="-c $DATA_DIR/$CONF"

  echo $COMMAND $CHDIR $CONF $OPTIONS $ARGS "> $EXPECTED_FILE"
  $COMMAND $CHDIR $CONF $OPTIONS $ARGS > $EXPECTED_FILE
}

##
# Must put BUILD_SRC first in PATH so we get the correct versions.
##
PATH=$BUILD_SRC:$PATH

SWISHXX_TEST=true; export SWISHXX_TEST

for expected_file in $EXPECTED_DIR/*.txt
do
  base=`local_basename $expected_file`
  test_file=`echo $TESTS_DIR/$base | sed 's/txt$/test/'`
  run_test_file $test_file $expected_file
done

# vim:set et sw=2 ts=2:
