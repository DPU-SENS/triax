// Copyright (C) 2017   Abe Burleigh  <ktrout.ab@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.


// This octave pkg allows for control of a triax spectrometer in Octave for RPI

#include <octave/oct.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <fcntl.h>
#include "triax_class.h"

using std::string;

static bool type_loaded = false;

DEFUN_DLD (triax, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{triax} = } triax ([@var{device}], [@var{flags}])\n \
\n\
Open serial interface and start up triax.\n \
\n\
@var{device} - Desired serial interface. If omitted defaults to 0. @*\
@var{flags} - Flags for setting triax parameters. TO DO: usage details\n \
\n\
The triax() shall return instance of @var{octave_triax} class as the result @var{triax}.\n \
@end deftypefn")
{
    if (!type_loaded) {
        octave_triax::register_type();
        type_loaded = true;
    }
    octave_value ret_val = octave_value(-1);
    // Do not open interface if return value is not assigned
    if (nargout != 1) {
        print_usage();
        return ret_val;
    }
    // Default value: interface 0
    int dev = 0;
    string flags;
    // Parse the function arguments (device, flags)
    // is_float_type() or'ed to allow ("", 123), without using ("", int32(123)), we only take "int_value"
    if (args.length() > 0 && (args(0).is_integer_type() || args(0).is_float_type())) dev = args(0).int_value();
    // flags
    if (args.length() > 1) {
        if (args(1).is_string()) {
            flags = args(1).string_value();
        } else {
            print_usage();
            return ret_val;
        }
    }
    octave_triax* tr = new octave_triax();
    // Open the interface
    if (tr->open(dev, flags) < 0) return ret_val;
    ret_val = octave_value(1);
    return octave_value(ret_val);
}
