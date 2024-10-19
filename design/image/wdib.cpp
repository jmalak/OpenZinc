//	Zinc Application Framework - WDIB.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "wdib.hpp"

// ----- WDIB_RGB_QUAD ------------------------------------------------------

// Map for matching RGB values to an index into _colorMap[16].
WIN_RGBQUAD WDIB_RGB_QUAD::_zincRGBValue[] =
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

WDIB_RGB_QUAD::WDIB_RGB_QUAD(void)
{
	rgbBlue = rgbGreen = rgbRed = rgbReserved = 0; // ZIL_UINT8 variables.
}

WDIB_RGB_QUAD::WDIB_RGB_QUAD(WIN_RGBQUAD &rgbQuad)
{
	rgbBlue = rgbQuad.rgbBlue;
	rgbGreen = rgbQuad.rgbGreen;
	rgbRed = rgbQuad.rgbRed;
	rgbReserved = 0;
}

ZIL_UINT8 WDIB_RGB_QUAD::GetZincColor(void)
{
    for (ZIL_UINT8 color = 0; color < 16; color++)
		if (_zincRGBValue[color].rgbRed == rgbRed &&
			_zincRGBValue[color].rgbGreen == rgbGreen &&
			_zincRGBValue[color].rgbBlue == rgbBlue)
			return (color);
    return (0xFF);
}

// ----- WDIB_FILE_HEADER ---------------------------------------------------

ZIL_UINT32 WDIB_FILE_HEADER::_size = 14;

WDIB_FILE_HEADER::WDIB_FILE_HEADER(void)
{
    bfType = bfReserved1 = bfReserved2 = 0; // ZIL_UINT16 variables.
    bfSize = bfOffBits = 0; // ZIL_UINT32 variables.
}

// ----- WDIB_INFO_HEADER ---------------------------------------------------

ZIL_UINT32 WDIB_INFO_HEADER::_size = 40;
int WDIB_INFO_HEADER::_encodingRatio = 2;

WDIB_INFO_HEADER::WDIB_INFO_HEADER(void)
{
	biPlanes = biBitCount = 0; // ZIL_UINT16 variables.
    biWidth = biHeight = biXPelsPerMeter = biYPelsPerMeter = 0; // ZIL_INT32 variables.
    biSize = biCompression = biSizeImage = biClrUsed = biClrImportant = 0; // ZIL_UINT32 variables.
}

