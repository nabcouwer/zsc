# zsc

ZLIB DATA COMPRESSION LIBRARY - safety-critical version

zlib 1.2.11.f-abcouwer-safety-critical-v0 is a stripped-down version of zlib intended 
to be safe for flight software or other safety-critical applications.

See https://www.zlib.net/ for information about zlib.

This zlib version's repo is at https://github.jpl.nasa.gov/abcouwer/zlib-safe.
Owned/developed/maintained by Neil Abcouwer, 
neil.abcouwer [at] jpl.nasa.gov.

If you fork from this repo for more than just playing around, 
Neil would appreciate learning what it's being used for.

## Using the code

This code does not by itself compile into an executable of library. 
This code is intended to be compiled along with the code that uses it.

`include/zsc/zsc_pub.h` defines public functions for compression and decompression
that handle the aforementioned work buffers, and function that check buffer sizes.

`include/zsc/zsc_pub_types.h` defines macros for buffer sizing at compile time
and various public types.

zsc expects a configuration dependent `include/zsc/zsc_conf_global_types`
and `include/zsc/zsc_conf_private` that one must create for your 
specific configuration. `zsc_conf_global_types` defines sized types, and 
`zsc_conf_global_types` defines macros and memory functions. 
`test/zsc_test_global_types` and `test/zsc_test_private` are examples 
that will be copied over to `include/zsc` for unit testing.

## version info

This version of zlib is targeted toward safety-critical applications, 
such as spaceflight. Changes were made to follow the MISRA C 2012 guidelines: 

https://www.misra.org.uk/MISRAHome/MISRAC2012/tabid/196/Default.aspx

and the "Power of 10" rules:

http://spinroot.com/p10/

Changes have been noted in the Changelog and/or commented with the text 
"Abcouwer ZSC". Notable changes include:

No dynamic memory allocation - Dynamic allocation after initialization is not safe.
Dynamic memory functions, and ways to use dynamic memory, have been removed. 
Users must provide work buffers to the compression and decompression functions. 
Functions declared in zsc_pub.h will assist with this.

No conditional compilation - If code has N ifdefs, there are 2^N possible 
versions of the code, and testing becomes impossible. Several compilation 
options have been removed. If one of these options in necesary for your application,
consider a pull request that implements the option in a run-time manner.

Use of assertions - Assertion macros can be defined as whatever make sense 
for your application.

Use of types that define the size and sign - this has not yet been tested 
on 16 bit systems, be wary.

### Exceptions

In violation of required MISRA Directive R15.2, backward gotos have been left 
in inffast.c, rather than restructure the code.

In violation of advisory MISRA Directive R15.1, gotos have been left 
in the code, rather than restructure.



## Building

This code does not by itself compile into an executable or library. 
This code is intended to be compiled along with the code that uses it.

For testing, you'll need the following dependencies:

`build-essential cmake gcc valgrind lcov`

To run unit tests (including pulling google test framework and corpus data): 

`  ./build.bash test`
  
To build and run unit tests with coverage:

`  ./build.bash coverage`
  
Then open ./build/coverage/index.html to look at results.

To save unit test output to the test folder (so it can be committed 
for later delta comparison)

`  ./build.bash save`

To run unit tests with valgrind:

`./build.bash valgrind`

To clean (remove the build directory):

`./build.bash clean`


To use with your own framework, you will need to define your own versions of
`zsc_conf_global_types.h` and `zsc_conf_private.h`, 
to do appropriate declarations for your framework, and you may need to make 
build changes so they aren't overwritten.

## static analysis

This library has been analyzed using Cobra (http://spinroot.com/cobra/, 
https://github.com/nimble-code/Cobra). 

To use Cobra, follow the Cobra instructions to clone and configure. 
On linux, you may need the "yacc" program from the "bison" package.
You may want to add these lines in your bashrc (or equivalent) 
as discussed in the cobra readme:

`export COBRA=/path/to/your/clone/of/Cobra`
`export PATH=$PATH:$COBRA/bin_linux`
`export C_BASE=$COBRA/rules`

Then run commands of the form:

`cobra -f file -I/path/to/this/repo/include /path/to/this/repo/src/*.c`

Where file can be one of several rules files. This code, compiled with the 
unit test headers, was checked against the basic, misra2012, p10, and jpl rules. 
Running 

`./build.bash cobra`

runs all these checks. There will be some false positives and some items that 
have been left as-is, where changing the existing code would be higher-risk.

The main encoding/decoding loops loop over every pixel of an image or the 
entirety of the compressed bitstream. Thus the overhead of function calls 
becomes significant. Therefore these main loops exceed advised function sizes 
and use macros for the sake of performance.
  
## TODO 

- run more static analyzers, like semmle
- get close to 100% coverage in testing (currently at 91%)

## JPL Development Info  

This software was developed at the Jet Propulsion Laboratory, 
California Institute of Technology, under a contract with the 
National Aeronautics and Space Administration (80NM0018D0004).  

This work was funded by funded by the NASA Space Technology 
Mission Directorate (STMD) and the proposal, 
"CubeRover for Affordable, Modular, and Scalable Planetary Exploration" 
was selected as part of the NASA 2019 Tipping Point solicitation Topic 4: 
Other Capabilities Needed for Exploration. The project is managed by the 
STMD Utilizing Public-Private Partnerships to Advance Tipping Point 
Technologies Program.

This software was developed under JPL Task Plan No 15-106860.

This software is reported via JPL NTR 51704.

This software was approved for open source by JPL Software Release Authority 
Brian Morrison on 2020-10-21.

## Git

This version was created by pushing zlib 1.2.11, via

`git clone https://github.com/madler/zlib.git`

`cd zlib`

`git push --mirror github_url`

To pull any future changes from public zlib, one should be able to do

`git remote add public https://github.com/madler/zlib.git`

`git pull public master # Creates a merge commit`

`git push origin master`
  
But no guarantees on how well this work, given the significant changes.



  
## Original zlib README

1.2.11.f-abcouwer-safety-v0 specific info ends here.
The information below this line is all from zlib 1.2.11's README.
-----------------------------------------------------------------------------

ZLIB DATA COMPRESSION LIBRARY

zlib 1.2.11 is a general purpose data compression library.  All the code is
thread safe.  The data format used by the zlib library is described by RFCs
(Request for Comments) 1950 to 1952 in the files
http://tools.ietf.org/html/rfc1950 (zlib format), rfc1951 (deflate format) and
rfc1952 (gzip format).

All functions of the compression library are documented in the file zlib.h
(volunteer to write man pages welcome, contact zlib@gzip.org).  A usage example
of the library is given in the file test/example.c which also tests that
the library is working correctly.  Another example is given in the file
test/minigzip.c.  The compression library itself is composed of all source
files in the root directory.

To compile all files and run the test program, follow the instructions given at
the top of Makefile.in.  In short "./configure; make test", and if that goes
well, "make install" should work for most flavors of Unix.  For Windows, use
one of the special makefiles in win32/ or contrib/vstudio/ .  For VMS, use
make_vms.com.

Questions about zlib should be sent to <zlib@gzip.org>, or to Gilles Vollant
<info@winimage.com> for the Windows DLL version.  The zlib home page is
http://zlib.net/ .  Before reporting a problem, please check this site to
verify that you have the latest version of zlib; otherwise get the latest
version and check whether the problem still exists or not.

PLEASE read the zlib FAQ http://zlib.net/zlib_faq.html before asking for help.

Mark Nelson <markn@ieee.org> wrote an article about zlib for the Jan.  1997
issue of Dr.  Dobb's Journal; a copy of the article is available at
http://marknelson.us/1997/01/01/zlib-engine/ .

The changes made in version 1.2.11 are documented in the file ChangeLog.

Unsupported third party contributions are provided in directory contrib/ .

zlib is available in Java using the java.util.zip package, documented at
http://java.sun.com/developer/technicalArticles/Programming/compression/ .

A Perl interface to zlib written by Paul Marquess <pmqs@cpan.org> is available
at CPAN (Comprehensive Perl Archive Network) sites, including
http://search.cpan.org/~pmqs/IO-Compress-Zlib/ .

A Python interface to zlib written by A.M. Kuchling <amk@amk.ca> is
available in Python 1.5 and later versions, see
http://docs.python.org/library/zlib.html .

zlib is built into tcl: http://wiki.tcl.tk/4610 .

An experimental package to read and write files in .zip format, written on top
of zlib by Gilles Vollant <info@winimage.com>, is available in the
contrib/minizip directory of zlib.


Notes for some targets:

- For Windows DLL versions, please see win32/DLL_FAQ.txt

- For 64-bit Irix, deflate.c must be compiled without any optimization. With
  -O, one libpng test fails. The test works in 32 bit mode (with the -n32
  compiler flag). The compiler bug has been reported to SGI.

- zlib doesn't work with gcc 2.6.3 on a DEC 3000/300LX under OSF/1 2.1 it works
  when compiled with cc.

- On Digital Unix 4.0D (formely OSF/1) on AlphaServer, the cc option -std1 is
  necessary to get gzprintf working correctly. This is done by configure.

- zlib doesn't work on HP-UX 9.05 with some versions of /bin/cc. It works with
  other compilers. Use "make test" to check your compiler.

- gzdopen is not supported on RISCOS or BEOS.

- For PalmOs, see http://palmzlib.sourceforge.net/


Acknowledgments:

  The deflate format used by zlib was defined by Phil Katz.  The deflate and
  zlib specifications were written by L.  Peter Deutsch.  Thanks to all the
  people who reported problems and suggested various improvements in zlib; they
  are too numerous to cite here.

Copyright notice:

 (C) 1995-2017 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu

If you use the zlib library in a product, we would appreciate *not* receiving
lengthy legal documents to sign.  The sources are provided for free but without
warranty of any kind.  The library has been entirely written by Jean-loup
Gailly and Mark Adler; it does not include third-party code.

If you redistribute modified sources, we would appreciate that you include in
the file ChangeLog history information documenting your changes.  Please read
the FAQ for more information on the distribution of modified source versions.
