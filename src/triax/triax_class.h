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

#ifndef TRIAX_CLASS_H
#define TRIAX_CLASS_H

#include <octave/oct.h>
#include <string>
#include <stdio.h>
#include <stdint.h>
// Serial
#include "serial_class.h"

using std::string;

class octave_triax : public octave_base_value
{
public:
    octave_triax();
    ~octave_triax();
    // open /close serial connection
    int open(int /* interface number */, string /* parameter flags */);
    int close();    
    // check triax stats
    int set_front_width(int width);     // front slit
    int get_front_width();
    int set_side_width(int width);      // side slit
    int get_side_width();
    int set_turret_pos(int pos);        // turret
    int get_turret_pos();
    int set_mirror_pos(int pos);        // mirror
    int get_mirror_pos();
    // scan (unimplemented)
    int scan(unsigned int /* counts */);
    // parse triax response
    char* handle_triax_resp(uint8_t* /* response */, const char* /* command type */);

private:
    // triax stats
    int front_width;
    int side_width;
    int turret;
    bool mirror;
    // the serial connection
    octave_serial* ser;
    // serial stats
    unsigned int baud_rate;
    short timeout;
    unsigned short byte_size;
    std::string parity;
    unsigned short stop_bits;
    // parse flags and stuff
    char** ssplit(char* /* char string */, const char* /* delimiter */);
    int parse_string(std::string /* parameter flags */, const char* /* delimiter */);
    std::string assemble_command(const char* /* command */, double* /* value */);

    DECLARE_OCTAVE_ALLOCATOR
    DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA
};

#endif
