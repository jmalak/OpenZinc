//	Zinc Application Framework - ODIB1.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "odib.hpp"

// ----- ZAF_IMAGE ----------------------------------------------------------

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ImportODIB(ZIL_FILE *file)
{
	int i, j, k;

	// Load the file header.
	ODIB_FILE_HEADER bmiFileHeader(file);
	if (file->GetError())
		return (SetError(ERROR_FILE_HEADER));
	imFlags &= ~(IMF_BITMAP_IMAGE | IMF_ICON_IMAGE | IMF_MOUSE_IMAGE);
	if (bmiFileHeader.usType == BFT_BMAP)
		imFlags |= IMF_BITMAP_IMAGE;
	else if (bmiFileHeader.usType == BFT_ICON || bmiFileHeader.usType == BFT_COLORICON)
		imFlags |= IMF_ICON_IMAGE;
	else
		imFlags |= IMF_MOUSE_IMAGE;

	// Load the info header.
   	ODIB_INFO_HEADER bmiInfoHeader(file);
	if (file->GetError())
		return (SetError(ERROR_INFO_HEADER));

	// Set-up the Zinc color table.
	int colorCount = bmiInfoHeader.cBitCount ? 1 : 2;
	for (i = 0; i < bmiInfoHeader.cBitCount; i++)
		colorCount <<= 1;
   	ZIL_UINT8 *zincColor = new ZIL_UINT8[colorCount];
	for (i = 0; i < colorCount; i++)
	{
   		ODIB_RGB_TRIPLE bmiColor(file);
   		zincColor[i] = bmiColor.GetZincColor();
	}

	// Initialize the image.
	if (bmiInfoHeader.c2x * bmiInfoHeader.c2y > 0x7FFF)
	{
		delete zincColor;
		return (SetError(ERROR_BITMAP_TOO_LARGE));
	}
	bitmapWidth = (ZIL_INT16)bmiInfoHeader.c2x;
	bitmapHeight = (ZIL_INT16)bmiInfoHeader.c2y;
	if (FlagSet(imFlags, IMF_ICON_IMAGE | IMF_MOUSE_IMAGE))
		bitmapHeight /= 2;
   	bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	memset(bitmapArray, 0, bitmapWidth * bitmapHeight);
	hotspotX = bmiFileHeader.xHotspot;
	hotspotY = bmiFileHeader.yHotspot;

	// Load the image information.
	int twidth = bitmapWidth / ODIB_INFO_HEADER::_encodingRatio;
	int fwidth = (twidth + sizeof(ZIL_UINT32) - 1) / sizeof(ZIL_UINT32) * sizeof(ZIL_UINT32);
	ZIL_UINT8 *fbitmap = new ZIL_UINT8[fwidth];
	// Check for XOR and AND pels
	file->Seek(bmiFileHeader.offBits, ZIL_FILE::SEEK_FROM_START);
	if (FlagSet(imFlags, IMF_ICON_IMAGE | IMF_MOUSE_IMAGE))
	{
		ZIL_UINT8 mask;
		int tsize = bitmapWidth / 8;
		for (i = 0; i < bitmapHeight; i++) // XOR
		{
			for (j = 0; j < tsize; j++)
				file->Read(&mask);
		}
		for (i = 0; i < bitmapHeight; i++) // AND
		{
			int offset = (bitmapHeight - i - 1) * bitmapWidth;
			for (j = 0; j < tsize; j++)
			{
				file->Read(&mask);
				for (k = 0; k < 8; k++, offset++)
					if (mask & (ZIL_UINT8)(0x80 >> k))
						bitmapArray[offset] = (ZIL_UINT8)BACKGROUND;
			}
		}
		delete zincColor;
		zincColor = ZIL_NULLP(ZIL_UINT8);
	}
	// Read the colors.
	ZIL_UINT8 color;
	for (i = 0; i < bitmapHeight; i++)
	{
		file->Read(fbitmap, fwidth, sizeof(ZIL_UINT8));
		int offset = (bitmapHeight - i - 1) * bitmapWidth;
		for (j = 0; j < twidth; j++, offset++)
		{
			// first colormask.
			color = ((fbitmap[j] & 0xF0) >> 4) & 0x0F;
			if (zincColor)
				color = zincColor[color];
			if (!bitmapArray[offset])
				bitmapArray[offset] = (color < 16) ? color : (ZIL_UINT8)BACKGROUND;
			offset++;
			// second colormask.
			color = fbitmap[j] & 0x0F;
			if (zincColor)
				color = zincColor[color];
			if (!bitmapArray[offset])
				bitmapArray[offset] = (color < 16) ? color : (ZIL_UINT8)BACKGROUND;
		}
	}
	delete fbitmap;
	if (zincColor)
		delete zincColor;

	// Return the file status.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

// ----- ODIB_RGB_TRIPLE ----------------------------------------------------

ODIB_RGB_TRIPLE::ODIB_RGB_TRIPLE(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR ODIB_RGB_TRIPLE::Load(ZIL_FILE *file)
{
	file->Read(&rgbBlue);
	file->Read(&rgbGreen);
	file->Read(&rgbRed);
	return (file->GetError());
}

// ----- ODIB_FILE_HEADER ---------------------------------------------------

ODIB_FILE_HEADER::ODIB_FILE_HEADER(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR ODIB_FILE_HEADER::Load(ZIL_FILE *file)
{
	file->Read(&usType);
	file->Read(&cbSize);
	file->Read(&xHotspot);
	file->Read(&yHotspot);
	file->Read(&offBits);
	return (file->GetError());
}

// ----- ODIB_INFO_HEADER ---------------------------------------------------

ODIB_INFO_HEADER::ODIB_INFO_HEADER(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR ODIB_INFO_HEADER::Load(ZIL_FILE *file)
{
	file->Read(&cbFix);
	if (cbFix == _size1)
	{
		cPlanes = cBitCount = usUnits = usReserved = usRecording =
			usRendering = 0; // ZIL_UINT16 variables.
    	c2x = c2y = ulCompression = cbImage = cxResolution = 
			cyResolution = cclrUsed = cclrImportant = cSize1 = cSize2 = 
			ulColorEncoding = ulIdentifier = 0; // ZIL_UINT32 variables.
		file->Read(&c1x);
		file->Read(&c1y);
		file->Read(&cPlanes);
		file->Read(&cBitCount);
	}
	else
	{
		file->Read(&c2x);
		file->Read(&c2y);
		file->Read(&cPlanes);
		file->Read(&cBitCount);
		file->Read(&ulCompression);
		file->Read(&cbImage);
		file->Read(&cxResolution);
		file->Read(&cyResolution);
		file->Read(&cclrUsed);
		file->Read(&cclrImportant);
		file->Read(&usUnits);
		file->Read(&usReserved);
		file->Read(&usRecording);
		file->Read(&usRendering);
		file->Read(&cSize1);
		file->Read(&cSize2);
		file->Read(&ulColorEncoding);
		file->Read(&ulIdentifier);
	}
	return (file->GetError());
}

// ----- ODIB_ARRAY_FILE_HEADER ---------------------------------------------

ODIB_ARRAY_FILE_HEADER::ODIB_ARRAY_FILE_HEADER(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR ODIB_ARRAY_FILE_HEADER::Load(ZIL_FILE *file)
{
	file->Read(&usType);
	file->Read(&cbSize);
	file->Read(&offNext);
	file->Read(&cxDisplay);
	file->Read(&cyDisplay);
	return (file->GetError());
}

