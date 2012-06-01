#! /usr/bin/env python
"""
This script builds the quicktime reader for matlab.  The target and several
options can be set by the commad line options.  The default version of 
Matlab is R2008b, but a different version may be used by supplying the path
to the application to the appropriate command line option.

Author: Ryan Cabeen, cabeen@gmail.com
Version: October 2, 2009
"""

import logging
from optparse import OptionParser
from tempfile import mkdtemp
from os.path import abspath
from os.path import join
from os.path import basename 
from os.path import dirname
from os.path import exists
from os import system
from os import makedirs
from sys import argv
from sys import stdout
from shutil import copy
from shutil import move 
from shutil import rmtree
from subprocess import Popen
from subprocess import STDOUT
from subprocess import PIPE 

BIN_DIR = 'bin'
SRC_DIR = 'src'
NAMES = ['read_qt', 'length_qt']
VERMEX = join(BIN_DIR, 'mexversion.o')

def execute(command, report=True):
  logger = logging.getLogger()
  p = Popen(command.split(), stdout=PIPE, stderr=STDOUT)
  out = p.stdout.readlines()
  if report: [logger.info(line.strip()) for line in out]
  if p.wait() != 0: raise Exception, "Error in command: " + command

def main():
  logger = logging.getLogger()
  h = logging.StreamHandler(stdout)
  h.setFormatter(logging.Formatter("%(asctime)s: %(message)s"))
  h.setLevel(logging.INFO)
  logger.addHandler(h)

  p = OptionParser(usage="%prog [options] in_image in_mesh out",
                   version="%prog 1.0", description=__doc__)
  p.add_option("-v", "--verbose", action="store_true", default=False,
               help="print status messages to standard output")
  p.add_option("-d", "--debug", action="store_true", default=False,
               help="print debugger messages")
  p.add_option("-g", "--gcc", default='gcc-4.0',
               help="specify a custom path to a gcc binary")
  p.add_option("-m", "--matlab", default='/Applications/MATLAB_R2008b.app',
               help="specify a custom path to a Matlab application")
  p.add_option("-t", "--target", type="choice", choices=["clean", "matlab"], \
               default="matlab", help="specify the build target.  "
               "This may be either 'clean' or 'matlab'.  The version of matlab"
               "can be set with other command line options.  Default: matlab")

  (options, args) = p.parse_args()

  if options.verbose: logger.setLevel(logging.INFO)
  if options.debug: logger.setLevel(logging.DEBUG)

  try:
    if len(args) != 0:
      raise Exception, "an incorrect number of arguments were found.  " \
                       "Expected %i but found %i." % (3, len(args))

    logger.info("Starting " + basename(argv[0]))

    mat = options.matlab
    base_dir = dirname(abspath(argv[0]))
    src_dir = join(base_dir, SRC_DIR)
    bin_dir = join(base_dir, BIN_DIR)
    ver = join(base_dir, VERMEX) 

    logger.info("Printing options:")
    for k, v in options.__dict__.items():
      logger.info("%s: %s" % (str(k), str(v)))
   
    if not exists(bin_dir):
      logger.info("Creating binary directory '%s'" % bin_dir)
      makedirs(bin_dir)
    
    if options.target == 'clean':
      logger.info("Cleaning the binary directory")
      rmtree(bin_dir)

    elif options.target == 'matlab':
      logger.info("Building matlab executable")

      command = "gcc-4.0 -c -I%s/extern/include -I%s/simulink/include -DMATLAB_MEX_FILE -fno-common -no-cpp-precomp -fexceptions -DMX_COMPAT_32 -O3 -DNDEBUG %s/extern/src/mexversion.c -o %s" % (mat, mat, mat, ver)
      execute(command) 

      for n in NAMES:
        src = join(src_dir, n + '.c')
        obj = join(bin_dir, n + '.o')
        mex = join(bin_dir, n + '.mexmaci')
        mfi = join(src_dir, n + '.m')
        mfo = join(bin_dir, n + '.m')
        
        logger.info("Building " + n)
        
        command = "gcc-4.0 -c -I%s/extern/include -I%s/simulink/include -DMATLAB_MEX_FILE -fno-common -no-cpp-precomp -fexceptions -DMX_COMPAT_32 -O3 -DNDEBUG %s -o %s" % (mat, mat, src, obj)
        execute(command) 

        command = "gcc-4.0 -O -Wl,-flat_namespace -undefined suppress -bundle -Wl,-exported_symbols_list,%s/extern/lib/maci/mexFunction.map -o %s %s %s -L%s/bin/maci -lmx -lmex -lmat -L%s/sys/os/maci -lstdc++ -framework Carbon -framework QuickTime" % (mat, mex, obj, ver, mat, mat)
        execute(command) 

        copy(mfi, mfo)

  except:
    import sys
    type, value, tb = sys.exc_info() 
    sys.stderr.write("Fatal error: %s\n" % value)
    if '-d' in argv or '--debug' in argv: 
      import traceback
      traceback.print_exc()
    logging.shutdown()
    sys.exit(1)

  logger.info("Finished")

if __name__ == "__main__":
  main()
