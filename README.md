# triax
control the triax spectrometer in Octave on RPi

1. Installation

Download git and clone the repository:
sudo apt-get install git
git clone https://github.com/DPU-SENS/triax

Make tarball from source:
tar -cvzf triax.tar.gz /path/to/triax/*

Install package in octave:
pkg install triax.tar.gz


2. Usage

triax(int device, string flags) - 
currently defaults to serial0 regardless of device value.
flags (entered as a string) can be used to change the serial parameters from default values:
-r set baud rate (Hz) default: 10kHz
-t set timeout (s*10) default: -1 (none)
-b set byte size (bits) default: 8
-p set parity [ 0 (none), 2 (even), 1 (odd) ] default: 0
-s set stop bits (bits) default: 1
returns instance of @var{octave_triax} class as the result


triax_front(triax tr, double width) - 
Check or set the front slit width.
tr - instance of @var{octave_triax} class.
width - Desired slit width (microns) - leave empty to check width.
returns the current front slit width in microns.


triax_mirror(triax tr, int pos) - 
Check or set the mirror position [0 (inactive) or 1 (active)].
tr - instance of @var{octave_triax} class.
pos - Desired mirror position - leave empty to check position.
returns the current mirror position [0 (inactive) or 1 (active)].


triax_side(triax tr, double width) - 
Check or set the side slit width.
tr - instance of @var{octave_triax} class.
width - Desired slit width (microns) - leave empty to check width.
returns the current side slit width in microns.


triax_turret(triax tr, double pos) - 
Check or set the turret position (nm).
tr - instance of @var{octave_triax} class.
pos - Desired turret position (nm) - leave empty to check position.
returns the current turret position in nm.


triax_close(triax tr) - 
Close the interface and release a file descriptor. 
triax is an instance of @var{octave_triax} class.



3. Examples

open triax device on ttys0 with default parameters (works just fine):
pkg load triax
tr=triax()
OR
pkg load triax
tr=triax(1) // only using serial0 right now
OR 
pkg load triax
tr=triax(0,”-r 10000 -b 8 -p 0 -s 1”)

open triax device on ttys0 with 30kHz clock and even parity
tr=triax(0,”-r 30000 -p 2”)

check front slit width and then set it to 1mm
wid=triax_front(tr)
wid=triax_front(tr,1000)

check mirror position and then set it to active, and then inactive
pos=triax_mirror(tr)
pos=triax_mirror(tr,1)
pos=triax_mirror(tr,0)

check side slit width and then set it to 0.5mm
wid=triax_side(tr)
wid=triax_side(tr,500)

check turret position and then set it to 1000nm
pos=triax_turret(tr)
pos=triax_turret(tr,1000)

close previously opened triax
triax_close(tr)



4. Notes

for use with spi-control:
Read MCP3008 CH0 command: 0b000000011000000000000000 = 0x018000
Read MCP3008 CH1 command: 0b000000011001000000000000 = 0x019000
Read MCP3008 CH2 command: 0b000000011010000000000000 = 0x01A000
Read MCP3008 CH3 command: 0b000000011011000000000000 = 0x01B000
Read MCP3008 CH4 command: 0b000000011100000000000000 = 0x01C000

The serial commands are executed using the code from the serial portion of the Octave Instrument-Control package. 

Instructions for updating this incoming.

TO DO: this 

