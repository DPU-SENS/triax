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

// Check or set the front slit width
#include <octave/oct.h>
#include <octave/uint8NDArray.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <errno.h>
#include "triax_class.h"

static bool type_loaded = false;

DEFUN_DLD (triax_front, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{ret} = } triax_front (@var{triax}, @var{width})\n \
\n\
Check or set the front slit width.\n \
\n\
@var{triax} - instance of @var{octave_triax} class.@*\
@var{width} - Desired slit width (microns) - leave empty to check width.\n \
\n\
Shall return the current front slit width in microns.\n \
@end deftypefn")
{
    if (!type_loaded) {
        octave_triax::register_type();
        type_loaded = true;
    }
    octave_value ret_val = octave_value(-1);
    if (args.length() > 2 || args.length() < 1) {
        print_usage();
        return ret_val;
    }
    int slit_width = -1;
    if (args.length() > 1) {
        if (!(args(1).is_integer_type() || args(1).is_float_type())) {
            print_usage();
            return ret_val;
        }
        slit_width = args(1).int_value();
    }
    octave_triax* triax = NULL;
    const octave_base_value& rep = args(0).get_rep();
    triax = &((octave_triax &)rep);
    if (slit_width >= 0) {
        ret_val = octave_value(triax->set_front_width(slit_width));
    } else {
        ret_val = octave_value(triax->get_front_width());
    }
    return ret_val;
}
