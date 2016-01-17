/*
**      PJL C++ Library
**      option_stream.h
**
**      Copyright (C) 1999-2015  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef option_stream_H
#define option_stream_H

// standard
#include <iostream>

#if 0
extern char const* dtoa( double );

// local
#include "itoa.h"
#include "util.h"
#endif

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * Given the traditional \c argc and \c argv for command-line arguments,
 * extract options from them following the stream model.
 *
 * Why not use the standard \c getopt() or GNU's \c getopt_long()?  Because
 * neither are thread-safe and I needed a thread-safe version.
 */
class option_stream {
public:
  enum {
    os_arg_none = 0,                    // option takes no argument
    os_arg_req  = 1,                    // option takes a required argument
    os_arg_opt  = 2                     // option takes an optional argument
  };

  /**
   * A %spec gives a specfication for an option: its long name, its argument
   * type, and it's short option equivalent.  A null-terminated array of specs
   * is given to the option_stream constructor.
   *
   * The \c arg_type takes on one of the above enum values, but is not declared
   * as an \c enum to allow the integer values to be used as a shorthand.
   *
   * Regardless of whether the user enters a long or a short option, it is the
   * short option character that is returned to the caller in order to be able
   * to use a simple switch statement to do different things depending on the
   * option.
   *
   * If, for a given long option there is to be no short option equivalent,
   * then the caller has to make up bogus ones that the user will never type.
   * Either the ASCII control or high characters can be user for this.  The
   * null character may not be used.
   */
  struct spec {
#if 0
    /**
     * Ignore the class in this section.  I'm not sure I want to implement
     * default values for options that can have optional arguments.  I left the
     * code here in case I ever decide I want to implement it.
     */
    class default_value {
    public:
      default_value( char v ) : val_( new char[2] ) {
        val_[0] = v;
        val_[1] = '\0';
      }

      default_value( char const *v = "" ) : val_( new_strdup( v ) ) { }
      default_value( long v ) : val_( new_strdup( ltoa( v ) ) ) { }
      default_value( double v ) : val_( new_strdup( dtoa( v ) ) ) { }
      default_value( default_value const &v ) : val_( new_strdup( v.val_ ) ) { }
      ~default_value() { delete[] val_; }

      operator char*() const { return val_; }

    private:
      char *const val_;

      default_value& operator=( default_value const& );
    };
#endif

    char const   *long_name;
    short         arg_type;
    unsigned char short_name;
#if 0
    default_value def_arg;
#endif
  };

  /**
   * An %option is what is extracted from an option_stream.
   * Its <code>operator char()</code> gives which option it is and \c arg()
   * gives its argument, if any.  For options that do not have an argument,
   * \c arg() returns the null pointer.
   *
   * An %option may be copied in which case it has a private copy of its
   * argument.
   */
  class option {
  public:
    option( char c = '\0', char *a = nullptr ) : short_name_( c ), arg_( a ) { }

    /**
     * Gets the option's argument, if any.
     *
     * @return Returns said argument or NULL if the option has no argument.
     */
    char* arg() const {
      return arg_;
    }

    /**
     * Converts to the short-name of the option.
     *
     * @return Returns said name.
     */
    operator char() const {
      return short_name_;
    }

    friend option_stream& operator>>( option_stream&, option& );

  private:
    char  short_name_;
    char *arg_;

    option( option const& ) = delete;
    option& operator=( option const& ) = delete;
  };

  /////////////////////////////////////////////////////////////////////////////

  /**
   * Constructs an %option_stream.
   *
   * @param argc The argument count.
   * @param argv The argument values.
   * @param specs The array of option specifications to use.
   * @param err The stream to emit error messages to, if any.
   */
  option_stream( int argc, char *argv[], spec const specs[],
                 std::ostream &err = std::cerr );

  /**
   * Gets the amount to shift \c argc and \c argv by (the number of options).
   *
   * @return Returns said amount.
   */
  int shift() const {
    return argi_;
  }

  /**
   * Gets whether we've reached the end of options.
   *
   * @return Returns \c true only if we've reached the end of options.
   */
  explicit operator bool() const {
    return !end_;
  }

  /**
   * Parses and extracts an option from an option stream.
   * Options begin with either a \c - for short options or a \c -- for long
   * options.  Either a \c - or \c -- by itself explicitly ends the options;
   * however, the difference is that \c - is returned as the first non-option
   * whereas \c -- is skipped entirely.
   *
   * When there are no more options, the option_stream converts to bool as
   * false.  The option_stream's \c shift() member is the number of options
   * parsed which the caller can use to adjust \c argc and \c argv.
   *
   * Short options can take an argument either as the remaining characters of
   * the same \c argv or in the next \c argv unless the next \c argv looks like
   * an option by beginning with a <code>-</code>.
   *
   * Long option names can be abbreviated so long as the abbreviation is
   * unambiguous.  Long options can take an argument either directly after a
   * \c = in the same \c argv or in the next \c argv (but without an
   * <code>=</code>) unless the next \c argv looks like an option by beginning
   * with a <code>-</code>).
   *
   * @param os The option_stream to extract an option from.
   * @param opt The extracted option.
   * @return Returns \a os.
   */
  friend option_stream& operator>>( option_stream &os, option &opt );

private:
  int           argc_;                  // argument count from main()
  char        **argv_;                  // argument vector from main()
  spec const   *specs_;                 // the option specifications
  std::ostream &err_;                   // ostream to write wrrors to

  int           argi_;                  // current index into argv_[]
  char         *next_c_;                // next char in group of short options
  bool          end_;                   // reached end of options?

  option_stream( option_stream const& ) = delete;
  option_stream& operator=( option_stream const& ) = delete;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* option_stream_H */
/* vim:set et sw=2 ts=2: */
