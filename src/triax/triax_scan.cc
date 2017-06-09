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

// Put the triax in scan mode
#include <octave/oct.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <errno.h>
#include "triax_class.h"

static bool type_loaded = false;

DEFUN_DLD (triax_scan, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{ret} = } triax_scan (@var{triax}, @var{cnt})\n \
\n\
!UNIMPLEMENTED! Scan some number (cnt) of wavelengths with the triax.\n \
\n\
@var{triax} - instance of @var{octave_triax} class.@*\
@var{cnt} - number of wavelengths to sample.\n \
\n\
Upon successful completion, triax_scan() shall return the number of samples and a buffer containing the data @var{n}.\n \
@end deftypefn")
{
    if (!type_loaded) {
        octave_triax::register_type();
        type_loaded = true;
    }
    octave_value ret_val = octave_value(-1);
    int count = -1;
    if (args.length() != 2 || args(0).type_id() != octave_triax::static_type_id()) {
        print_usage();
        return ret_val;
    } else {
        count = 5;
    }
    octave_triax* triax = NULL;
    const octave_base_value& rep = args(0).get_rep();
    triax = &((octave_triax &)rep);
    ret_val = octave_value(triax->scan(count));

    return ret_val;
}
