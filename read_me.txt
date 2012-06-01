Matlab QuickTime Reader Documentation 
-------------------------------------

Author: Ryan Cabeen, cabeen@gmail.com
Version: 1.0, October 2, 2009

Overview
--------

This package contains source code and binaries for a program to natively read
QuickTime-supported video in the Macintosh Matlab environment.


Requirements
------------

To use the distributed binaries, Macintosh Operating System version 10.6 and
MATLAB R2008b are required.

However, the source is very simple to build, and compilation only requires the
Apple Developer Tools and use of the command line.  Specifically, this uses
Python 2.5 for build automation and GCC 4.0 for compilation.  This has been
tested on Mac OS 10.5 and 10.6 using MATLAB R2008b.  There is a build option to
use a different MATLAB version, though this has not been tested.

Usage
-----

First, the functions and binaries must be added to the path.  If the project
directory is at '/a_path/project', the following command will add them to the
path:

>> addpath('/a_path/project/bin')

There are two programs in this package: 'read_qt' and 'length_qt'.  'length_qt'
computes the number of frames in a video, and 'read_qt' reads the entirety or
subset of a video.  For large videos, it may not be possible to load all frames
into memory.  This is often the case when loading long high resolution files.
In this case, the number of frames should be computed once and segments of the
video should be loaded and processed separately.

The programs have the following interfaces:

length = length_qt(name)
frames = read_qt(name)
frames = read_qt(name, last)
frames = read_qt(name, first, last)

The 'length_qt' function always has the movie filename as input and the number
of frames as output.  The 'read_qt' function has three forms, with different
amounts of specificity of frames to read, and it always returns a cell array of
three-dimensional arrays (width by height by color).  All frame indices are
one-based and all intervals are inclusive.  With one argument, all frames are
read; with two arguments all frames from the beginning to 'last' are read; with
three arguments all frames from 'first' to 'last' are read.

For example, given a movie named 'movie.mov', here are some typical commands to
read all or some of the frames:

>> mov = read_qt('movie.mov');
>> length = length_qt('movie.mov');
>> seg_size = 10;
>> first_seg = read_qt('movie.mov', seg_size);
>> second_seg = read_qt('movies.mov', seg_size, 2 * seg_size);
>> n = 20;
>> nth_seg = read_qt('movie.mov', n * seg_size, (n + 1) * seg_size);

Compilation
-----------

If the distributed binaries are not compatible with the current system, the
source is available for compilation.  To build the program, execute the script
'./build.py' in the project directory.  The binaries will be compiled to the
directory 'bin'.

If a different version of MATLAB than R2008b is installed, add the option
execute './build.py -m /Applications/MATLAB_XXXXXX.app', where the last
argument is the path to the MATLAB application.

In the most recent version of the Mac operating system, the QuickDraw library
has been deprecated, which may lead to some warnings during compilation.
Nonetheless, this has been tested to be functional as of Mac OS 10.6.

If the code needs to be build on OS versions before 10.5, the QuickTime header
should be changed from  '#include <QuickTime/QuickTime.h>' to 
'#include <Quicktime/Quicktime.h>'.

Info
----

This code was developed for the Caltech Vision Lab with Lihi Zelnik-Manor under
Pietro Perona for the importing of DV stored in a QuickTime container.  The
code is released under the license specified in the file 'license.txt'.
