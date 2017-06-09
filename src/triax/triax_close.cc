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

#include "triax_class.h"

static bool type_loaded = false;

DEFUN_DLD (triax_close, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} triax_close (@var{triax})\n \
\n\
Close the interface and release a file descriptor.\n \
\n\
@var{triax} - instance of @var{octave_triax} class.@*\
@end deftypefn")
{
    if (!type_loaded) {
        octave_triax::register_type();
        type_loaded = true;
    }
    if (args.length() != 1 || args(0).type_id() != octave_triax::static_type_id()) {
        print_usage();
        return octave_value(-1);
    }
    octave_triax* triax = NULL;
    const octave_base_value& rep = args(0).get_rep();
    triax = &((octave_triax &)rep);
    triax->close();
    delete triax;
    return octave_value(1);
}
