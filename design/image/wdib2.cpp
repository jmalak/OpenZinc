//	Zinc Application Framework - WDIB2.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "wdib.hpp"

// ----- ZAF_IMAGE ----------------------------------------------------------

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ExportWDIB(ZIL_FILE *file)
{
	// Constant declarations.
	const int rgbQuadSize = 4;
	const int maxColors = 16;
	int twidth = bitmapWidth / WDIB_INFO_HEADER::_encodingRatio;
	int fwidth = (twidth + sizeof(ZIL_UINT32) - 1) / sizeof(ZIL_UINT32) * sizeof(ZIL_UINT32);

	// Store the file header.
	WDIB_FILE_HEADER bmiFileHeader;
	bmiFileHeader.bfType = BFT_BMAP;
	bmiFileHeader.bfSize =
		WDIB_FILE_HEADER::_size +
		WDIB_INFO_HEADER::_size +
		rgbQuadSize * maxColors +
		fwidth * bitmapHeight;
	bmiFileHeader.bfReserved1 = 0;
	bmiFileHeader.bfReserved2 = 0;
	bmiFileHeader.bfOffBits =
		WDIB_FILE_HEADER::_size +
		WDIB_INFO_HEADER::_size +
		rgbQuadSize * maxColors;
	bmiFileHeader.Store(file);

	// Store the info header.
   	WDIB_INFO_HEADER bmiInfoHeader;
	bmiInfoHeader.biSize = WDIB_INFO_HEADER::_size;
	bmiInfoHeader.biWidth = bitmapWidth;
	bmiInfoHeader.biHeight = bitmapHeight;
	bmiInfoHeader.biPlanes = 1;
	bmiInfoHeader.biBitCount = 4; // 16 colors.
	bmiInfoHeader.biCompression = 0; // no compression.
	bmiInfoHeader.biSizeImage = fwidth * bitmapHeight;
	bmiInfoHeader.biXPelsPerMeter = 0;
	bmiInfoHeader.biYPelsPerMeter = 0;
	bmiInfoHeader.biClrUsed = 0; // use default specified colors.
	bmiInfoHeader.biClrImportant = maxColors;
	bmiInfoHeader.Store(file);

	// Store the matching color table.
	int i;
	for (i = 0; i < maxColors; i++)
	{
		WDIB_RGB_QUAD rgbColor(WDIB_RGB_QUAD::_zincRGBValue[i]);
		rgbColor.Store(file);
	}

	// Store the bitmap image.
	ZIL_UINT8 *fbitmap = new ZIL_UINT8[fwidth];
	for (i = twidth; i < fwidth; i++) // pad the offset area with zeros.
		fbitmap[i] = 0;
	for (i = 0; i < bitmapHeight; i++)
	{
		int offset = (bitmapHeight - i - 1) * bitmapWidth;
		for (int j = 0; j < twidth; j++, offset += 2)
			fbitmap[j] = ((bitmapArray[offset] & 0x0F) << 4) | (bitmapArray[offset+1] & 0x0F);
		file->Write(fbitmap, fwidth, sizeof(ZIL_UINT8));
	}
	delete fbitmap;

	// Return the file status.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

// ----- WDIB_RGB_QUAD ------------------------------------------------------

ZIL_FILE::Z_ERROR WDIB_RGB_QUAD::Store(ZIL_FILE *file)
{
	file->Write(rgbBlue);
	file->Write(rgbGreen);
	file->Write(rgbRed);
	file->Write(rgbReserved);
	return (file->GetError());
}

// ----- WDIB_FILE_HEADER ---------------------------------------------------

ZIL_FILE::Z_ERROR WDIB_FILE_HEADER::Store(ZIL_FILE *file)
{
	file->Write(bfType);
	file->Write(bfSize);
	file->Write(bfReserved1);
	file->Write(bfReserved2);
	file->Write(bfOffBits);
	return (file->GetError());
}

// ----- WDIB_INFO_HEADER ---------------------------------------------------

ZIL_FILE::Z_ERROR WDIB_INFO_HEADER::Store(ZIL_FILE *file)
{
	file->Write(biSize);
	file->Write(biWidth);
	file->Write(biHeight);
	file->Write(biPlanes);
	file->Write(biBitCount);
	file->Write(biCompression);
	file->Write(biSizeImage);
	file->Write(biXPelsPerMeter);
	file->Write(biYPelsPerMeter);
	file->Write(biClrUsed);
	file->Write(biClrImportant);
	return (file->GetError());
}

