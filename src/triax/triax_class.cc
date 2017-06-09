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

// TO DO: fix #ifundef TRIAX and SPI

#include <octave/oct.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <errno.h>
#include <sys/mman.h>
#include <time.h>

using std::string;

#include "triax_class.h"
#include "serial_class.h"

DEFINE_OCTAVE_ALLOCATOR (octave_triax);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_triax, "octave_triax", "octave_triax");

// Setting to 0 will suppress (most) printf statements
static int debug = 1;

octave_triax::octave_triax()
{
    //    this->fd = -1;
}

octave_triax::~octave_triax()
{
    this->close();
}

int octave_triax::open(int dev, std::string flags)
{
    printf("triax_init\n");
    int ret_val = -1;
    std::string device;
//    if (dev > 0) {
//        device = "/dev/ttyS1";
//    } else {
//        device = "/dev/ttyS0";
//    }
    
    device = "/dev/ttyS0";
//    device = "/dev/serial0";
    // default configuration
    this->baud_rate = 9600;     // flag: 'r'
    this->timeout = 1;          // flag: 't'
    this->byte_size = 8;        // flag: 'b'
    this->parity = "N";         // flag: 'p'
    this->stop_bits = 1;        // flag: 's'
    // open (create) the serial connection
    octave_serial* sr = new octave_serial();
    if (sr->open(device) < 0) return ret_val;
    // parse parameter flags if specified
    if (!flags.empty()) {
        if (parse_string(flags,"-") < 0) {
            error("triax_init failed, flags incorrectly specified\n");
            this->close();
            return ret_val;
        }
    }
    // set params
    sr->set_baudrate(this->baud_rate);
    sr->set_timeout(this->timeout);
    sr->set_parity(this->parity);
    sr->set_bytesize(this->byte_size);
    sr->set_stopbits(this->stop_bits);
    // Flush input and output buffers
    sr->flush(2);
    // Check connection by selecting Monodrive
    sr->write("b/n");
    // Read the response
    int len = 12;
    uint8_t *buf = new uint8_t[len];
    ret_val = sr->read(buf, len);
    if (debug) printf("%s\n", buf);
    free(buf);
    // if we got a response, then we're good
    if (debug) printf("serial fd: %i\n", sr->get_fd());
    if (ret_val > 0) this->ser = sr;
    return ret_val;
}

static const char *t_turret = "b";
static const char *t_front = "bw";
static const char *t_side = "bww";
static const char *t_mirror = "bwww";
static int resp_len = 200;
static int comm_len = 20;
//static const char *s_turret = "+GO TO  MONO DRIVE!";
//static const char *s_front = "FR EN SLIT!";
//static const char *s_side = "SD EX SLIT!";
//static const char *s_mirror = "EXT MIRROR!+OPT";
//static const char *s_end1 = "!+EDIT";
//static const char *s_end2 = "!";
static const char *s_split = "+GO TMNDRIVE!FSLX+P";

// assemble a command
std::string octave_triax::assemble_command(const char* com, double* val)
{
    char comm[comm_len];
    if (val) {
        double value = *val;
        sprintf(comm,"%s%f", com, value);
        // TO DO: replace '.' with ' '
    } else {
        sprintf(comm,"%s", com);
    }
    strcat(comm, "\n");
    std::string command(comm, strlen(comm));
    if (debug) printf("Command: %s\n", command.c_str());
    return command;
}

// front slit
int octave_triax::set_front_width(int width)
{
    octave_serial* sr = this->ser;
    sr->flush(2);
    char *swidth;
    sprintf(swidth, "%i", width);
    char *command = strcat((char *)t_front, (const char *)swidth);
    command = strcat(command,"\n");
    sr->write(command);
    uint8_t *buf = new uint8_t[resp_len];
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_width = atoi((const char *)this->handle_triax_resp(buf, t_front));
    this->front_width = new_width;
    free(buf);
    return this->front_width;
}

int octave_triax::get_front_width()
{
    octave_serial* sr = this->ser;
    sr->flush(2);
    char *command = strcat((char *)t_front, "\n");
    sr->write(command);
    uint8_t *buf = new uint8_t[resp_len];
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_width = atoi((const char *)this->handle_triax_resp(buf, t_front));
    this->front_width = new_width;
    free(buf);
    return this->front_width;
}

// side slit
int octave_triax::set_side_width(int width)
{
    octave_serial* sr = this->ser;
    sr->flush(2);
    char *swidth;
    sprintf(swidth, "%i", width);
    char *command = strcat((char *)t_side, (const char *)swidth);
    command = strcat(command,"\n");
    sr->write(command);
    uint8_t *buf = new uint8_t[resp_len];
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_width = atoi((const char *)this->handle_triax_resp(buf, t_side));
    this->side_width = new_width;
    free(buf);
    return this->front_width;
}

int octave_triax::get_side_width()
{
    octave_serial* sr = this->ser;
    sr->flush(2);
    char *command = strcat((char *)t_side, "\n");
    sr->write(command);
    uint8_t *buf = new uint8_t[resp_len];
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_width = atoi((const char *)this->handle_triax_resp(buf, t_side));
    this->side_width = new_width;
    free(buf);
    return this->side_width;
}

// turret
int octave_triax::set_turret_pos(int pos)
{
    octave_serial* sr = this->ser;
    sr->flush(2);
    char *spos;
    sprintf(spos, "%i", pos);
    char *command = strcat((char *)t_turret, (const char *)spos);
    command = strcat(command,"\n");
    sr->write(command);
    uint8_t *buf = new uint8_t[resp_len];
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_pos = atoi((const char *)this->handle_triax_resp(buf, t_turret));
    this->turret = new_pos;
    free(buf);
    return this->turret;
}

int octave_triax::get_turret_pos()
{
    octave_serial* sr = this->ser;
    sr->flush(2);
    char *command = strcat((char *)t_turret, "\n");
    sr->write(command);
    uint8_t *buf = new uint8_t[resp_len];
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_pos = atoi((const char *)this->handle_triax_resp(buf, t_turret));
    this->turret = new_pos;
    free(buf);
    return this->turret;
}

// mirror
int octave_triax::set_mirror_pos(int pos)
{
    if (debug) printf("set mirror %i\n", pos);
    octave_serial* sr = this->ser;
    sr->flush(2);
    const char *spos;
    if (pos) {
        spos = "1\n";
    } else {
        spos = "0\n";
    }
    char *command = strcat((char *)t_mirror, spos);
    sr->write(command);
    uint8_t *buf = new uint8_t[resp_len];
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_pos = atoi((const char *)this->handle_triax_resp(buf, t_mirror));
    this->mirror = new_pos;
    free(buf);
    return this->mirror;
}

int octave_triax::get_mirror_pos()
{
    if (debug) printf("get mirror pos: \n");
    octave_serial* sr = this->ser;
    if (debug) printf("serial fd: %i\n", sr->get_fd());
    sr->flush(2);
    std::string command = assemble_command(t_mirror, NULL);
    if (debug) printf("serial fd: %i\n", sr->get_fd());
    sr->write(command);
//    usleep(1000);
    uint8_t *buf = new uint8_t[resp_len];
    if (debug) printf("serial fd: %i\n", sr->get_fd());
    int bytes = sr->read(buf, resp_len);
    if (debug) printf("read %i bytes\n", bytes);
    int new_pos = atoi((const char *)this->handle_triax_resp(buf, t_mirror));
    this->mirror = new_pos;
    if (debug) printf("%i\n", this->mirror);
    free(buf);
    return this->mirror;
}

int octave_triax::scan(unsigned int len)
{
    error("Scan feature not implemented");
}

char** octave_triax::ssplit(char* s, const char* delim)
{
    int i = 0;
    // go through and determine number of tokens first
    char temp[strlen(s)];
    strcpy(temp, s);
    char *tok = strtok(temp, delim);
    while (tok != NULL) {
        i++;
        tok = strtok(temp, delim);
    }
    if (debug) printf("Split  %s  x %i\n", s, i);
    char **el = new char*[i];
    i = 0;
    tok = strtok(s, delim);
    while (tok != NULL) {
        el[i] = tok;
        if (debug) printf("[%s] ", tok);
        tok = strtok(NULL, delim);
        i++;
    }
    if (debug) printf("\n");
    return el;
}

int octave_triax::parse_string(std::string flags, const char* delim)
{
    if (debug) printf("Parsing flags: %s\n", flags.c_str());
    char **params = ssplit((char *)flags.c_str(), delim);
    // Change initial parameter values if necessary
    for (int i = 0; i < (sizeof(params) / sizeof(params[0])); i++) {
        if (debug) printf("param: %s\n", params[i]);
        char **args = ssplit(params[i], " ");
        // if-else through all possible flags to set parameters
        int argVal = atoi(args[1]);
        if (argVal < 0) {
            // Invalid argument.
            error("invalid argument: %s\n", args[1]);
            return -1;
        }
        if (strcmp(args[0], "r") == 0) {        // baud rate (Hz)
            if (debug) printf("baud rate: %i\n", argVal);
            this->baud_rate = argVal;
        } else if (strcmp(args[0], "t") == 0) { // timeout (clock cycles/ms/us ??)
            if (debug) printf("timeout: %i\n", argVal);
            this->timeout = argVal;
        } else if (strcmp(args[0], "b") == 0) { // byte size (bits)
            if (debug) printf("byte size: %i\n", argVal);
            this->byte_size = argVal;
        } else if (strcmp(args[0], "p") == 0) { // parity [ 0 (none), 2 (even), 1 (odd) ]
            if (debug) printf("parity: %i\n", argVal);
            std::string par = "";
            switch (argVal) {
                case 0 :
                    par = "n";
                    break;
                case 1 :
                    par = "o";
                    break;
                case 2 :
                    par = "e";
                    break;
                default :
                    // Invalid parity
                    error("invalid parity: %i\nChoose 0 (none), 1 (odd), 2 (even)\n", argVal);
                    return -1;
            }
            this->parity = par;
        } else if (strcmp(args[0], "s") == 0) { // stop bits (bits)
            if (debug) printf("stop bits: %i\n", argVal);
            this->stop_bits = argVal;
        } else {                                // invalid param
            error("invalid flag specified: -%s\n", args[0]);
            return -1;
        }
        free(args);
    }
    free(params);
    return 1;
}

char* octave_triax::handle_triax_resp(uint8_t* iresp, const char* cmd)
{
    char* resp = (char *)iresp;
    if (debug) printf("handle response: %s\n", resp);
    char **spl = ssplit(resp, s_split);
    resp = spl[0];
    if (debug) printf("%s\n", resp);
    free(spl);
    return resp;
}

int octave_triax::close()
{
    printf("TRIAX end\n");
    int ret_val = 1;    // Success
    
    // TO DO: need to reset any pins ??
    // Close serial connection
    octave_serial *sr = this->ser;
    ret_val = sr->close();
    free(sr);
    return ret_val;
}
