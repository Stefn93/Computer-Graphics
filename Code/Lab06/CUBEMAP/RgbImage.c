/*
 *
 * RayTrace Software Package, release 1.0.4,  February 2004.
 *
 * Author: Samuel R. Buss
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

#include "RgbImage.h"

#ifndef RGBIMAGE_DONT_USE_OPENGL
#include <windows.h>
#include "GL/gl.h"
#endif

void RgbImageSize(RgbImage* image, int numRows, int numCols )
{
	unsigned char* c;
	int rowLen, i, j;
	image->NumRows = numRows;
	image->NumCols = numCols;
	image->ImagePtr = malloc( sizeof(unsigned char) * image->NumRows*GetNumBytesPerRow(image) );
	if ( !image->ImagePtr ) {
		fprintf(stderr, "Unable to allocate memory for %ld x %ld bitmap.\n", 
				image->NumRows, image->NumCols);
		Reset(image);
		image->ErrorCode = MemoryError;
	}
	// Zero out the image
	c = image->ImagePtr;
	rowLen = GetNumBytesPerRow(image);
	for ( i=0; i<image->NumRows; i++ ) {
		for ( j=0; j<rowLen; j++ ) {
			*(c++) = 0;
		}
	}
}

/* ********************************************************************
 *  LoadBmpFile
 *  Read into memory an RGB image from an uncompressed BMP file.
 *  Return true for success, false for failure.  Error code is available
 *     with a separate call.
 *  Author: Sam Buss December 2001.
 **********************************************************************/

bool LoadBmpFile(RgbImage* image, const char* filename ) 
{  
	FILE* infile = fopen( filename, "rb" );		// Open for reading binary data
	bool fileFormatOK = false;
	int bChar, mChar, bitsPerPixel, i, j, k;
	unsigned char* cPtr;

	Reset(image);
	if ( !infile ) {
		fprintf(stderr, "Unable to open file: %s\n", filename);
		image->ErrorCode = OpenError;
		return false;
	}

	bChar = fgetc( infile );
	mChar = fgetc( infile );
	if ( bChar=='B' && mChar=='M' ) {			// If starts with "BM" for "BitMap"
		skipChars( infile, 4+2+2+4+4 );			// Skip 4 fields we don't care about
		image->NumCols = readLong( infile );
		image->NumRows = readLong( infile );
		skipChars( infile, 2 );					// Skip one field
		bitsPerPixel = readShort( infile );
		skipChars( infile, 4+4+4+4+4+4 );		// Skip 6 more fields

		if ( image->NumCols>0 && image->NumCols<=100000 && image->NumRows>0 && image->NumRows<=100000  
			&& bitsPerPixel==24 && !feof(infile) ) {
			fileFormatOK = true;
		}
	}
	if ( !fileFormatOK ) {
		Reset(image);
		image->ErrorCode = FileFormatError;
		fprintf(stderr, "Not a valid 24-bit bitmap file: %s.\n", filename);
		fclose ( infile );
		return false;
	}

	// Allocate memory
	image->ImagePtr = malloc( sizeof(unsigned char) * image->NumRows*GetNumBytesPerRow(image) );
	if ( !image->ImagePtr ) {
		fprintf(stderr, "Unable to allocate memory for %ld x %ld bitmap: %s.\n", 
				image->NumRows, image->NumCols, filename);
		Reset(image);
		image->ErrorCode = MemoryError;
		fclose ( infile );
		return false;
	}

	cPtr = image->ImagePtr;
	for ( i=0; i<image->NumRows; i++ ) {
		for ( j=0; j<image->NumCols; j++ ) {
			*(cPtr+2) = fgetc( infile );	// Blue color value
			*(cPtr+1) = fgetc( infile );	// Green color value
			*cPtr = fgetc( infile );		// Red color value
			cPtr += 3;
		}
		k=3*image->NumCols;					// Num bytes already read
		for ( ; k<GetNumBytesPerRow(image); k++ ) {
			fgetc( infile );				// Read and ignore padding;
			*(cPtr++) = 0;
		}
	}
	if ( feof( infile ) ) {
		fprintf( stderr, "Premature end of file: %s.\n", filename );
		Reset(image);
		image->ErrorCode = ReadError;
		fclose ( infile );
		return false;
	}
	fclose( infile );	// Close the file
	return true;
}

short readShort( FILE* infile )
{
	// read a 16 bit integer
	unsigned char lowByte, hiByte;
	short ret;
	lowByte = fgetc(infile);			// Read the low order byte (little endian form)
	hiByte = fgetc(infile);			// Read the high order byte

	// Pack together
	ret = hiByte;
	ret <<= 8;
	ret |= lowByte;
	return ret;
}

long readLong( FILE* infile )
{  
	// Read in 32 bit integer
	unsigned char byte0, byte1, byte2, byte3;
	long ret;
	byte0 = fgetc(infile);			// Read bytes, low order to high order
	byte1 = fgetc(infile);
	byte2 = fgetc(infile);
	byte3 = fgetc(infile);

	// Pack together
	ret = byte3;
	ret <<= 8;
	ret |= byte2;
	ret <<= 8;
	ret |= byte1;
	ret <<= 8;
	ret |= byte0;
	return ret;
}

void skipChars( FILE* infile, int numChars )
{
	int i;
	for ( i=0; i<numChars; i++ ) {
		fgetc( infile );
	}
}

/* ********************************************************************
 *  WriteBmpFile
 *  Write an RGB image to an uncompressed BMP file.
 *  Return true for success, false for failure.  Error code is available
 *     with a separate call.
 *  Author: Sam Buss, January 2003.
 **********************************************************************/

bool WriteBmpFile( RgbImage* image, const char* filename )
{
	int rowLen, i, j, k;
	unsigned char* cPtr;
	FILE* outfile = fopen( filename, "wb" );		// Open for reading binary data
	if ( !outfile ) {
		fprintf(stderr, "Unable to open file: %s\n", filename);
		image->ErrorCode = OpenError;
		return false;
	}

	fputc('B',outfile);
	fputc('M',outfile);
	rowLen = GetNumBytesPerRow(image);
	writeLong( 40+14+image->NumRows*rowLen, outfile );	// Length of file
	writeShort( 0, outfile );					// Reserved for future use
	writeShort( 0, outfile );
	writeLong( 40+14, outfile );				// Offset to pixel data
	writeLong( 40, outfile );					// header length
	writeLong( image->NumCols, outfile );				// width in pixels
	writeLong( image->NumRows, outfile );				// height in pixels (pos for bottom up)
	writeShort( 1, outfile );		// number of planes
	writeShort( 24, outfile );		// bits per pixel
	writeLong( 0, outfile );		// no compression
	writeLong( 0, outfile );		// not used if no compression
	writeLong( 0, outfile );		// Pixels per meter
	writeLong( 0, outfile );		// Pixels per meter
	writeLong( 0, outfile );		// unused for 24 bits/pixel
	writeLong( 0, outfile );		// unused for 24 bits/pixel

	// Now write out the pixel data:
	cPtr = image->ImagePtr;
	i;
	for ( i=0; i<image->NumRows; i++ ) {
		// Write out i-th row's data
		for ( j=0; j<image->NumCols; j++ ) {
			fputc( *(cPtr+2), outfile);		// Blue color value
			fputc( *(cPtr+1), outfile);		// Blue color value
			fputc( *(cPtr+0), outfile);		// Blue color value
			cPtr+=3;
		}
		// Pad row to word boundary
		k=3*image->NumCols;					// Num bytes already read
		for ( ; k<GetNumBytesPerRow(image); k++ ) {
			fputc( 0, outfile );				// Read and ignore padding;
			cPtr++;
		}
	}

	fclose( outfile );	// Close the file
	return true;
}

void writeLong( long data, FILE* outfile )
{  
	// Read in 32 bit integer
	unsigned char byte0, byte1, byte2, byte3;
	byte0 = (unsigned char)(data&0x000000ff);		// Write bytes, low order to high order
	byte1 = (unsigned char)((data>>8)&0x000000ff);
	byte2 = (unsigned char)((data>>16)&0x000000ff);
	byte3 = (unsigned char)((data>>24)&0x000000ff);

	fputc( byte0, outfile );
	fputc( byte1, outfile );
	fputc( byte2, outfile );
	fputc( byte3, outfile );
}

void writeShort( short data, FILE* outfile )
{  
	// Read in 32 bit integer
	unsigned char byte0, byte1;
	byte0 = data&0x000000ff;		// Write bytes, low order to high order
	byte1 = (data>>8)&0x000000ff;

	fputc( byte0, outfile );
	fputc( byte1, outfile );
}


/*********************************************************************
 * SetRgbPixel routines allow changing the contents of the RgbImage. *
 *********************************************************************/

void SetRgbPixelf(RgbImage* image,  long row, long col, double red, double green, double blue )
{
	SetRgbPixelc( image, row, col, doubleToUnsignedChar(red), 
							doubleToUnsignedChar(green),
							doubleToUnsignedChar(blue) );
}

void SetRgbPixelc( RgbImage* image, long row, long col,
				   unsigned char red, unsigned char green, unsigned char blue )
{
	unsigned char* thePixel;
	assert ( row<image->NumRows && col<image->NumCols );
	thePixel = GetRgbPixel( image, row, col );
	*(thePixel++) = red;
	*(thePixel++) = green;
	*(thePixel) = blue;
}


unsigned char doubleToUnsignedChar( double x )
{
	if ( x>=1.0 ) {
		return (unsigned char)255;
	}
	else if ( x<=0.0 ) {
		return (unsigned char)0;
	}
	else {
		return (unsigned char)(x*255.0);		// Rounds down
	}
}
// Bitmap file format  (24 bit/pixel form)		BITMAPFILEHEADER
// Header (14 bytes)
//	 2 bytes: "BM"
//   4 bytes: long int, file size
//   4 bytes: reserved (actually 2 bytes twice)
//   4 bytes: long int, offset to raster data
// Info header (40 bytes)						BITMAPINFOHEADER
//   4 bytes: long int, size of info header (=40)
//	 4 bytes: long int, bitmap width in pixels
//   4 bytes: long int, bitmap height in pixels
//   2 bytes: short int, number of planes (=1)
//   2 bytes: short int, bits per pixel
//   4 bytes: long int, type of compression (not applicable to 24 bits/pixel)
//   4 bytes: long int, image size (not used unless compression is used)
//   4 bytes: long int, x pixels per meter
//   4 bytes: long int, y pixels per meter
//   4 bytes: colors used (not applicable to 24 bit color)
//   4 bytes: colors important (not applicable to 24 bit color)
// "long int" really means "unsigned long int"
// Pixel data: 3 bytes per pixel: RGB values (in reverse order).
//	Rows padded to multiples of four.


#ifndef RGBIMAGE_DONT_USE_OPENGL

bool LoadFromOpenglBuffer()					// Load the bitmap from the current OpenGL buffer
{
	int viewportData[4];
	glGetIntegerv( GL_VIEWPORT, viewportData );
	int& vWidth = viewportData[2];
	int& vHeight = viewportData[3];
	
	if ( ImagePtr==0 ) { // If no memory allocated
		NumRows = vHeight;
		NumCols = vWidth;
		ImagePtr = malloc( sizeof(unsigned char) * NumRows*GetNumBytesPerRow() );
		if ( !ImagePtr ) {
			fprintf(stderr, "Unable to allocate memory for %ld x %ld buffer.\n", 
					NumRows, NumCols);
			Reset(image);
			ErrorCode = MemoryError;
			return false;
		}
	}
	assert ( vWidth>=NumCols && vHeight>=NumRows );
	int oldGlRowLen;
	if ( vWidth>=NumCols ) {
		glGetIntegerv( GL_UNPACK_ROW_LENGTH, &oldGlRowLen );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, NumCols );
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Get the frame buffer data.
	glReadPixels( 0, 0, NumCols, NumRows, GL_RGB, GL_UNSIGNED_BYTE, ImagePtr);

	// Restore the row length in glPixelStorei  (really ought to restore alignment too).
	if ( vWidth>=NumCols ) {
		glPixelStorei( GL_UNPACK_ROW_LENGTH, oldGlRowLen );
	}	
	return true;
}
#endif

long GetNumRows(RgbImage* image) { return image->NumRows; }
long GetNumCols(RgbImage* image) { return image->NumCols; }

long GetNumBytesPerRow(RgbImage* image) { return ((3*image->NumCols+3)>>2)<<2; }	
const void* ImageData(RgbImage* image) { return (void*)image->ImagePtr; }


void RgbImageInit(RgbImage* image)
{ 
	image->NumRows = 0;
	image->NumCols = 0;
	image->ImagePtr = 0;
	image->ErrorCode = 0;
}

bool RgbImageInitFile(RgbImage* image, const char* filename )
{
	image->NumRows = 0;
	image->NumCols = 0;
	image->ImagePtr = 0;
	image->ErrorCode = 0;
	return LoadBmpFile( image, filename );
}

void RgbImageDelete(RgbImage* image)
{ 
	free(image->ImagePtr);
}

/*
// Returned value points to three "unsigned char" values for R,G,B
const unsigned char* GetRgbPixel( long row, long col )
{
	assert ( row<NumRows && col<NumCols );
	const unsigned char* ret = ImagePtr;
	long i = row*GetNumBytesPerRow() + 3*col;
	ret += i;
	return ret;
}

void GetRgbPixel( long row, long col, float* red, float* green, float* blue ) const
{
	assert ( row<NumRows && col<NumCols );
	const unsigned char* thePixel = GetRgbPixel( row, col );
	const float f = 1.0f/255.0f;
	*red = f*(float)(*(thePixel++));
	*green = f*(float)(*(thePixel++));
	*blue = f*(float)(*thePixel);
}

void GetRgbPixel( long row, long col, double* red, double* green, double* blue ) const
{
	assert ( row<NumRows && col<NumCols );
	const unsigned char* thePixel = GetRgbPixel( row, col );
	const double f = 1.0/255.0;
	*red = f*(double)(*(thePixel++));
	*green = f*(double)(*(thePixel++));
	*blue = f*(double)(*thePixel);
}

*/

unsigned char* GetRgbPixel(RgbImage* image, long row, long col ) 
{
	unsigned char* ret = image->ImagePtr;
	long i = row*GetNumBytesPerRow(image) + 3*col;
	assert ( row<image->NumRows && col<image->NumCols );
	ret += i;
	return ret;
}

void Reset(RgbImage* image)
{
	image->NumRows = 0;
	image->NumCols = 0;
	if( image->ImagePtr != 0 )
		free(image->ImagePtr);
	image->ImagePtr = 0;
	image->ErrorCode = 0;
}

int GetErrorCode(RgbImage* image) { return image->ErrorCode; }
bool ImageLoaded(RgbImage* image) { return (image->ImagePtr!=0); }  // Is an image loaded?

