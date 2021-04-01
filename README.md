drvrem: cfitsio library with emscripten driver to access FITS files directly
----------------------------------------------------------------------------

The experimental cfitsio library in this repository contains drivers
to allow direct access to FITS files without downloading the entire
file first. To access a FITS file directly, prepend "@" to the URL:

@https://js9.si.edu/js9/data/fits/m13.fits

The cfitsio library has been compiled to byte-code using emscripten,
and can be used only within the emsripten environment. The byte-code
files have .a extensions and are stored in the lib subdirectory.
Include files have been copied to the include subdirectory.

Current emcc compiler: 2.0.16

An emscripten-enabled project such as JS9 can copy the contents of the
lib and include sub-directories into their own work space and then
link against the byte-code libraries. For general emscripten build
instructions, see the documentation at:

https://emscripten.org/docs/compiling/Building-Projects.html

For an example of the use of these particular libraries, see the
[Makefile](https://github.com/ericmandel/js9/blob/master/astroem/Makefile)
in the astroem sub-directory of https://github.com/ericmandel/js9.

The libraries are compiled using the -O3 level of optimization (see
the EMFLAGS variable in the top-level Makefile).  Emscripten wants you
to use the same optimization flags throughout the project, so if you
use a different level of optimization, you probably want to change the
EMFLAGS variable and rebuild the libraries by executing *make all*.

astroem is distributed under the terms of The MIT License.

Eric Mandel, Center for Astrophysics | Harvard & Smithsonian 