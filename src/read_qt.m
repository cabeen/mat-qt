% READ_QT Quicktime Movie Reader
%
%       MOVIE=READ_QT(NAME)
%       MOVIE=READ_QT(NAME, END)
%       MOVIE=READ_QT(NAME, START, END)
%       imports the frames of a movie with a filename NAME.  If two arguments
%       are used, the second will specify the ending (one-based) frame.  If
%       three arguments are used, the second will specify the starting frame and
%       the third will specify the ending frame.  When only a filename is 
%       specified, the entire movie is read.  The length of the movie can be 
%       computed using the function 'length_qt'.
%
%       The object MOVIE is a cell array in which each entry is a frame of
%       the movie.  Each element of the movie array is a three dimensional
%       matrix, where the first dimension is the height, the second 
%       dimension is the width, and the third dimension is the color
%       channel.
%
%       Here is an example of the function usage:
%
%       movie = read_qt('movie.mov', 50);
%
%       This will create a cell array Movie that contains the first 50 frames
%       of the movie.  The second frame could be accessed as Movie{2}.
%
%       Author: Ryan Cabeen, cabeen@gmail.com 
