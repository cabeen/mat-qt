/*
 documentation in 'read_me.txt' for information on compilation and use.

 Version: October 2, 2009 
 Author: Ryan Cabeen, cabeen@gmail.com

*/

#include <QuickTime/QuickTime.h>
#include <mex.h>

static pascal OSErr DrawCompleteProc(Movie theMovie, long refCon);
static PATH_BUFFER_SIZE = 512;

mxArray* framedata;         
long frame_num;
long frame_start;

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {

  if (nlhs < 0) { mexErrMsgTxt("Too few output arguments."); return; }
  if (nlhs > 1) { mexErrMsgTxt("Too many output arguments."); return; }
  if (nrhs < 1) { mexErrMsgTxt("Too few input arguments."); return; }
  if (nrhs > 3) { mexErrMsgTxt("Too many input arguments."); return; }

  TimeValue duration;
  TimeRecord myTimeRecord;
  Rect bounds;
  OSErr result = 0;
  short resRefNum = -1;
  short actualResId = DoTheRightThing;
  FSSpec theFSSpec;
  GWorldPtr offWorld;
  Movie theMovie = nil;
  MovieController thePlayer = nil;
  MovieDrawingCompleteUPP myDrawCompleteProc;
  long frame_end;
  long myStep = 1;
  char location[PATH_BUFFER_SIZE];
  long frame_count;
  mwSize cdims[2];

  mxGetString(prhs[0], location, PATH_BUFFER_SIZE);

  if (nrhs > 2) {
    frame_start = rint(mxGetScalar(prhs[1]));
    frame_end = rint(mxGetScalar(prhs[2]));
  } else if (nrhs > 1) {
    frame_start = 1;
    frame_end = rint(mxGetScalar(prhs[1]));
  } else {
    frame_start = 1;
    frame_end = 0;
  }

  if (frame_start < 1) {
    mexErrMsgTxt("Error: the starting frame must be positive\n"); 
    return; 
  }

  if (frame_end < 0) {
    mexErrMsgTxt("Error: the ending frame must be positive\n"); 
    return; 
  }

  if (frame_end != 0 && frame_end < frame_start) {
    mexErrMsgTxt("Error: the ending frame must not be less than the starting frame\n"); 
    return; 
  }

  myDrawCompleteProc = NewMovieDrawingCompleteUPP(DrawCompleteProc);

  EnterMovies();            

  if (NativePathNameToFSSpec(location, &theFSSpec, 0) ||
      OpenMovieFile(&theFSSpec, &resRefNum, 0) ||
      NewMovieFromFile(&theMovie, resRefNum, &actualResId, 0, 0, 0)) {
    mexErrMsgTxt("Error: failed to open movie\n"); 
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
  SetMovieDrawingCompleteProc(theMovie, movieDrawingCallWhenChanged, myDrawCompleteProc, (long) offWorld);
  GetMovieTime(theMovie, &myTimeRecord);
  duration = GetMovieDuration(theMovie);

  // Compute the number of frames for allocation of output structure
  frame_count = 0;
  while ((frame_end == 0 || frame_count < frame_end) && GetMovieTime(theMovie, NULL) < duration) {
    frame_count++;
    MCDoAction(thePlayer, mcActionStep, (Ptr) myStep);
  }
  SetMovieTime(theMovie, &myTimeRecord);
  
  // Ignore frames greater than those in the file
  if (frame_end == 0 || frame_count < frame_end) frame_end = frame_count;
  
  cdims[0] = frame_end - frame_start + 1; // Indices are one-based
  cdims[1] = 1; 

  plhs[0] = mxCreateCellArray(2, cdims);

  // Step through the movie and save the frame when in the chosen interval
  // Note: the step size seems to be handled as a short internally. 
  //       Using anything greater than 32758 will seek to an incorrect frame
  frame_num = 1;
  while (frame_num <= frame_end) {
    MCDoAction(thePlayer, mcActionStep, (Ptr) myStep);
    if (frame_num >= frame_start) {
      MCIdle(thePlayer);
      mxSetCell(plhs[0], frame_num - frame_start, framedata);
    }
    frame_num++;
  }

  UnlockPixels(GetGWorldPixMap (offWorld));
  DisposeGWorld(offWorld);
  DisposeMovieController (thePlayer);
  DisposeMovie(theMovie);
  DisposeMovieDrawingCompleteUPP(myDrawCompleteProc);
  ExitMovies();

  return;
}

static pascal OSErr DrawCompleteProc (Movie theMovie, long refCon) {

  long int h;
  long int w;
  int y, x;
  GWorldPtr offWorld = (GWorldPtr) refCon;
  Rect bounds;
  Ptr baseAddr;
  long rowBytes;
  uint8_t* imbuf;
  mwSize dims[3];

  GetPixBounds(GetGWorldPixMap(offWorld), &bounds);
  baseAddr = GetPixBaseAddr(GetGWorldPixMap(offWorld));
  rowBytes = GetPixRowBytes(GetGWorldPixMap(offWorld));

  h = rint(bounds.bottom - bounds.top);
  w = rint(bounds.right - bounds.left);

  dims[0] = h;
  dims[1] = w; 
  dims[2] = 3;

  framedata = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
  imbuf = (uint8_t*) mxGetData(framedata);

  // Retrieve the pixel data, unpack the RGB values and copy 
  for (y = 0; y < h; ++y) {
    long *p;
    p = (long *) (baseAddr + rowBytes * (long) y);
    for (x = 0; x < w; ++x) {
      UInt32 color = *(long *)((long) p + 4 * (long) x);;
      long B = (color & 0xFF000000) >> 24;
      long G = (color & 0x00FF0000) >> 16;
      long R = (color & 0x0000FF00) >> 8;

      imbuf[y + x * h + 0 * (h * w)] = R;
      imbuf[y + x * h + 1 * (h * w)] = G;
      imbuf[y + x * h + 2 * (h * w)] = B;
    }
  }

  return noErr;
}
