/*

 MATLAB Mex code to compute the number of frames in a QuickTime movie.  See the
 documentation in 'read_me.txt' for information on compilation and use.

 Version: October 2, 2009 
 Author: Ryan Cabeen, cabeen@gmail.com

*/

#include <QuickTime/QuickTime.h>
#include <mex.h>

static PATH_BUFFER_SIZE = 512;

void mexFunction (int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {

  // Error check the arguments
  if (nlhs < 0) { mexErrMsgTxt("Too few output arguments."); return; }
  if (nlhs > 1) { mexErrMsgTxt("Too many output arguments."); return; }
  if (nrhs < 1) { mexErrMsgTxt("Too few input arguments."); return; }
  if (nrhs > 1) { mexErrMsgTxt("Too many input arguments."); return; }

  TimeValue duration;
  Rect bounds;
  OSErr result = 0;
  short resRefNum = -1;
  short actualResId = DoTheRightThing;
  FSSpec theFSSpec;
  GWorldPtr offWorld;
  Movie theMovie = nil;
  MovieController thePlayer = nil;

  long frame;
  long myStep = 1;
  char location[PATH_BUFFER_SIZE];
  mwSize dims[1];
  uint32_t* data;

  mxGetString(prhs[0], location, PATH_BUFFER_SIZE);

  EnterMovies ();            

  if (NativePathNameToFSSpec(location, &theFSSpec, 0) ||
      OpenMovieFile(&theFSSpec, &resRefNum, 0) ||
      NewMovieFromFile(&theMovie, resRefNum, &actualResId, 0, 0, 0)) {
    mexPrintf("Error: failed to open movie\n"); 
    return; 
  }

  if (resRefNum != -1) CloseMovieFile(resRefNum);

  GetMovieBox(theMovie, &bounds);
  QTNewGWorld(&offWorld, k32ARGBPixelFormat, &bounds, NULL, NULL, 0);
  LockPixels(GetGWorldPixMap(offWorld));
  SetGWorld(offWorld, NULL);

  thePlayer = NewMovieController(theMovie, &bounds, mcTopLeftMovie | mcNotVisible);
  SetMovieGWorld(theMovie, offWorld, NULL);
  SetMovieActive(theMovie, true);
  duration = GetMovieDuration(theMovie);
  
  long frame_count = 0;
  while (GetMovieTime(theMovie, NULL) < duration) { 
    frame_count++;
    MCDoAction(thePlayer, mcActionStep, (Ptr) myStep);
  } 

  dims[0] = 1;
  plhs[0] = mxCreateNumericArray(1, dims, mxUINT32_CLASS, mxREAL);
  data = (uint32_t*) mxGetData(plhs[0]);
  data[0] = frame_count;

  UnlockPixels(GetGWorldPixMap(offWorld));
  DisposeGWorld(offWorld);
  DisposeMovieController(thePlayer);
  DisposeMovie(theMovie);
  ExitMovies();

  return;
}
