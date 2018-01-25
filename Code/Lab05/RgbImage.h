/*
 *
 * RayTrace Software Package, release 1.0.4,  February 2004.
 *
 * Author: Samuel R. Buss
 *
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

#ifndef RGBIMAGE_H
#define RGBIMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Include the next line to turn off the routines that use OpenGL
#define RGBIMAGE_DONT_USE_OPENGL

#define bool int
#define true 1
#define false 0

struct RgbImage {
	unsigned char* ImagePtr;	// array of pixel values (integers range 0 to 255)
	long NumRows;				// number of rows in image
	long NumCols;				// number of columns in image
	int ErrorCode;				// error code
};
typedef struct RgbImage RgbImage;

void RgbImageInit(RgbImage* image);
bool RgbImageInitFile(RgbImage* image, const char* filename );
void RgbImageInitSize(RgbImage* image, int numRows, int numCols );	// Initialize a blank bitmap of this size.
void RgbImageDelete(RgbImage* image);

bool LoadBmpFile(RgbImage* image, const char *filename );		// Loads the bitmap from the specified file
bool WriteBmpFile(RgbImage* image, const char* filename );		// Write the bitmap to the specified file
#ifndef RGBIMAGE_DONT_USE_OPENGL
bool LoadFromOpenglBuffer(RgbImage* image);					// Load the bitmap from the current OpenGL buffer
#endif

long GetNumRows(RgbImage* image);
long GetNumCols(RgbImage* image);

// Rows are word aligned
long GetNumBytesPerRow(RgbImage* image);
const void* ImageData(RgbImage* image);

unsigned char* GetRgbPixel(RgbImage* image, long row, long col );

/*
const unsigned char* GetRgbPixel( long row, long col );
void GetRgbPixel( long row, long col, float* red, float* green, float* blue );
void GetRgbPixel( long row, long col, double* red, double* green, double* blue );
*/

void SetRgbPixelf( RgbImage* image, long row, long col, double red, double green, double blue );
void SetRgbPixelc( RgbImage* image, long row, long col, 
				   unsigned char red, unsigned char green, unsigned char blue );

// Error reporting. (errors also print message to stderr)
int GetErrorCode(RgbImage* image);
enum {
	NoError = 0,
	OpenError = 1,			// Unable to open file for reading
	FileFormatError = 2,	// Not recognized as a 24 bit BMP file
	MemoryError = 3,		// Unable to allocate memory for image data
	ReadError = 4,			// End of file reached prematurely
	WriteError = 5			// Unable to write out data (or no date to write out)
};
bool ImageLoaded(RgbImage* image);

void Reset(RgbImage* image);			// Frees image data memory

static short readShort( FILE* infile );
static long readLong( FILE* infile );
static void skipChars( FILE* infile, int numChars );
static void writeLong( long data, FILE* outfile );
static void writeShort( short data, FILE* outfile );

static unsigned char doubleToUnsignedChar( double x );

#endif // RGBIMAGE_H
