//	Zinc Application Framework - ODIB.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "odib.hpp"

// ----- ODIB_RGB_TRIPLE ----------------------------------------------------

// Map for matching RGB values to an index into _colorMap[16].
OS2_RGBTRIPLE ODIB_RGB_TRIPLE::_zincRGBValue[] =
{
	{ 0x00, 0x00, 0x00 },   // 0 - BLACK		
	{ 0x80, 0x00, 0x00 },	// 1 - BLUE		
	{ 0x00, 0x80, 0x00 },	// 2 - GREEN		
	{ 0x80, 0x80, 0x00 },	// 3 - CYAN		
	{ 0x00, 0x00, 0x80 },	// 4 - RED			
	{ 0x80, 0x00, 0x80 },	// 5 - MAGENTA		
	{ 0x00, 0x80, 0x80 },	// 6 - BROWN		
	{ 0xC0, 0xC0, 0xC0 },	// 7 - LIGHTGRAY	
	{ 0x80, 0x80, 0x80 },	// 8 - DARKGRAY	
	{ 0xFF, 0x00, 0x00 },	// 9 - LIGHTBLUE	
	{ 0x00, 0xFF, 0x00 },	// 10 - LIGHTGREEN	
	{ 0xFF, 0xFF, 0x00 },	// 11 - LIGHTCYAN	
	{ 0x00, 0x00, 0xFF },	// 12 - LIGHTRED	
	{ 0xFF, 0x00, 0xFF },	// 13 - LIGHTMAGENTA
	{ 0x00, 0xFF, 0xFF },	// 14 - YELLOW		
	{ 0xFF, 0xFF, 0xFF }	// 15 - WHITE		
};

ODIB_RGB_TRIPLE::ODIB_RGB_TRIPLE(void)
{
	rgbBlue = rgbGreen = rgbRed = 0; // uint8 variables.
}

ODIB_RGB_TRIPLE::ODIB_RGB_TRIPLE(OS2_RGBTRIPLE &rgbTriple)
{
	rgbBlue = rgbTriple.rgbBlue;
	rgbGreen = rgbTriple.rgbGreen;
	rgbRed = rgbTriple.rgbRed;
}

ZIL_UINT8 ODIB_RGB_TRIPLE::GetZincColor(void)
{
    for (ZIL_UINT8 color = 0; color < 16; color++)
		if (_zincRGBValue[color].rgbRed == rgbRed &&
			_zincRGBValue[color].rgbGreen == rgbGreen &&
			_zincRGBValue[color].rgbBlue == rgbBlue)
			return (color);
    return (0xFF);
}

// ----- ODIB_FILE_HEADER ---------------------------------------------------

ZIL_UINT32 ODIB_FILE_HEADER::_size = 14;

ODIB_FILE_HEADER::ODIB_FILE_HEADER(void)
{
    usType = xHotspot = yHotspot = 0; // ZIL_UINT16 variables.
    cbSize = offBits = 0; // ZIL_UINT32 variables.
}

// ----- ODIB_INFO_HEADER ---------------------------------------------------

ZIL_UINT32 ODIB_INFO_HEADER::_size1 = 12;
ZIL_UINT32 ODIB_INFO_HEADER::_size2 = 64;
int ODIB_INFO_HEADER::_encodingRatio = 2;

ODIB_INFO_HEADER::ODIB_INFO_HEADER(void)
{
	cPlanes = cBitCount = usUnits = usReserved = usRecording =
		usRendering = 0; // ZIL_UINT16 variables.
    cbFix = c2x = c2y = ulCompression = cbImage = cxResolution = 
		cyResolution = cclrUsed = cclrImportant = cSize1 = cSize2 = 
		ulColorEncoding = ulIdentifier = 0; // ZIL_UINT32 variables.
}

// ----- ODIB_ARRAY_FILE_HEADER ---------------------------------------------

ZIL_UINT32 ODIB_ARRAY_FILE_HEADER::_size = 14;

ODIB_ARRAY_FILE_HEADER::ODIB_ARRAY_FILE_HEADER(void)
{
	usType = cxDisplay = cyDisplay = 0; // ZIL_UINT16 variables.
	cbSize = offNext = 0; // ZIL_UINT32 variables.;
}

