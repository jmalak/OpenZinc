//	Zinc Application Framework - ODIB2.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "odib.hpp"

// ----- ZAF_IMAGE ----------------------------------------------------------

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ExportODIB(ZIL_FILE *file)
{
	// Constant declarations.
	const int rgbTripleSize = 3;
	const int maxColors = 16;
	int twidth = bitmapWidth / ODIB_INFO_HEADER::_encodingRatio;
	int fwidth = (twidth + sizeof(ZIL_UINT32) - 1) / sizeof(ZIL_UINT32) * sizeof(ZIL_UINT32);

	// Store the file header.
	ODIB_FILE_HEADER bmiFileHeader;
	bmiFileHeader.usType = BFT_BMAP;
	if (FlagSet(imFlags, IMF_ICON_IMAGE))
		bmiFileHeader.usType = BFT_COLORICON;
	if (FlagSet(imFlags, IMF_MOUSE_IMAGE))
		bmiFileHeader.usType = BFT_COLORPOINTER;
	bmiFileHeader.cbSize =
		ODIB_FILE_HEADER::_size +
		ODIB_INFO_HEADER::_size1;
	bmiFileHeader.xHotspot = hotspotX;
	bmiFileHeader.yHotspot = hotspotY;
	bmiFileHeader.offBits =
		ODIB_FILE_HEADER::_size +
		ODIB_INFO_HEADER::_size1 +
		rgbTripleSize * maxColors;
	bmiFileHeader.Store(file);

	// Store the info header.
   	ODIB_INFO_HEADER bmiInfoHeader;
	bmiInfoHeader.cbFix = bmiInfoHeader._size1;
	bmiInfoHeader.c1x = bitmapWidth;
	bmiInfoHeader.c1y = bitmapHeight;
	bmiInfoHeader.cPlanes = 1;
	bmiInfoHeader.cBitCount = 4; // 16 colors.
	bmiInfoHeader.Store(file);

	// Store the matching color table.
	int i;
	for (i = 0; i < maxColors; i++)
	{
		ODIB_RGB_TRIPLE rgbColor(ODIB_RGB_TRIPLE::_zincRGBValue[i]);
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

	// Store the end-of-file.
	ZIL_UINT8 endOfFile = 0x0A;
	file->Write(endOfFile);

	// Return the file status.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

// ----- ODIB_RGB_TRIPLE ----------------------------------------------------

ZIL_FILE::Z_ERROR ODIB_RGB_TRIPLE::Store(ZIL_FILE *file)
{
	file->Write(rgbBlue);
	file->Write(rgbGreen);
	file->Write(rgbRed);
	return (file->GetError());
}

// ----- ODIB_FILE_HEADER ---------------------------------------------------

ZIL_FILE::Z_ERROR ODIB_FILE_HEADER::Store(ZIL_FILE *file)
{
	file->Write(usType);
	file->Write(cbSize);
	file->Write(xHotspot);
	file->Write(yHotspot);
	file->Write(offBits);
	return (file->GetError());
}

// ----- ODIB_INFO_HEADER ---------------------------------------------------

ZIL_FILE::Z_ERROR ODIB_INFO_HEADER::Store(ZIL_FILE *file)
{
	file->Write(cbFix);
	if (cbFix == _size1)
	{
		file->Write(c1x);
		file->Write(c1y);
		file->Write(cPlanes);
		file->Write(cBitCount);
	}
	else
	{
		file->Write(c2x);
		file->Write(c2y);
		file->Write(cPlanes);
		file->Write(cBitCount);
		file->Write(ulCompression);
		file->Write(cbImage);
		file->Write(cxResolution);
		file->Write(cyResolution);
		file->Write(cclrUsed);
		file->Write(cclrImportant);
		file->Write(usUnits);
		file->Write(usReserved);
		file->Write(usRecording);
		file->Write(usRendering);
		file->Write(cSize1);
		file->Write(cSize2);
		file->Write(ulColorEncoding);
		file->Write(ulIdentifier);
	}
	return (file->GetError());
}

// ----- ODIB_ARRAY_FILE_HEADER ---------------------------------------------

ZIL_FILE::Z_ERROR ODIB_ARRAY_FILE_HEADER::Store(ZIL_FILE *file)
{
	file->Write(usType);
	file->Write(cbSize);
	file->Write(offNext);
	file->Write(cxDisplay);
	file->Write(cyDisplay);
	return (file->GetError());
}

