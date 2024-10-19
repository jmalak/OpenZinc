//	Zinc Application Framework - WICO1.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "wdib.hpp"

// ----- ZAF_IMAGE ----------------------------------------------------------

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ImportWICO(ZIL_FILE *file)
{
	int i, j;

	// Load the file header.
	WICO_FILE_HEADER icoFileHeader(file);
	if (file->GetError())
		return (SetError(ERROR_FILE_HEADER));
	imFlags &= ~(IMF_BITMAP_IMAGE | IMF_ICON_IMAGE | IMF_MOUSE_IMAGE);
	if (icoFileHeader.iconType == 1)
		imFlags |= IMF_ICON_IMAGE;
	else if (icoFileHeader.iconType == 2)
		imFlags |= IMF_MOUSE_IMAGE;
	else 
		return (SetError(ERROR_FILE_HEADER));

	// Get the image information.
   	WICO_DIRECTORY icoInfoHeader(file);
	if (file->GetError())
		return (SetError(ERROR_FILE_HEADER));

	// Bitmap information - for later use.
   	WDIB_INFO_HEADER bmiInfoHeader(file);

	// Set-up the Zinc color table.
	file->Seek(icoInfoHeader.icoDIBOffset + WDIB_INFO_HEADER::_size, ZIL_FILE::SEEK_FROM_START);
	int colorCount = icoInfoHeader.colorCount ? icoInfoHeader.colorCount : 2;
   	ZIL_UINT8 *zincColor = new ZIL_UINT8[colorCount];
	for (i = 0; i < colorCount; i++)
	{
   		WDIB_RGB_QUAD bmiColor(file);
   		zincColor[i] = bmiColor.GetZincColor();
	}

	// Initialize the image.
	bitmapWidth = (ZIL_INT16)icoInfoHeader.width;
	bitmapHeight = (ZIL_INT16)icoInfoHeader.height;
	bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	memset(bitmapArray, 0, bitmapWidth * bitmapHeight);
	hotspotX = icoInfoHeader.hotspotX;
	hotspotY = icoInfoHeader.hotspotY;

	// Load the color image information.
	int twidth = bitmapWidth / WDIB_INFO_HEADER::_encodingRatio;
	int fwidth = (twidth + sizeof(ZIL_UINT32) - 1) / sizeof(ZIL_UINT32) * sizeof(ZIL_UINT32);
	ZIL_UINT8 *fbitmap = new ZIL_UINT8[fwidth];
	ZIL_UINT8 color;
	if (FlagSet(imFlags, IMF_ICON_IMAGE))
	{
		for (i = 0; i < bitmapHeight; i++)
		{
			file->Read(fbitmap, fwidth, sizeof(ZIL_UINT8));
			int offset = (bitmapHeight - i - 1) * bitmapWidth;
			for (j = 0; j < twidth; j++, offset++)
			{
				// first colormask.
				color = ((fbitmap[j] & 0xF0) >> 4) & 0x0F;
				bitmapArray[offset] = zincColor[color];
				offset++;
				// second colormask.
				color = fbitmap[j] & 0x0F;
				bitmapArray[offset] = zincColor[color];
			}
		}
	}

	// Load the mono image information.
	twidth = fwidth = bitmapWidth / 8; // mono sets each bit.
	if (!FlagSet(imFlags, IMF_ICON_IMAGE))
	{
		for (i = 0; i < bitmapHeight; i++)
		{
			int offset = (bitmapHeight - i - 1) * bitmapWidth;
			file->Read(fbitmap, fwidth, sizeof(ZIL_UINT8));
			for (j = 0; j < twidth; j++)
			{
				ZIL_UINT8 value = fbitmap[j];
				for (int k = 0; k < 8; k++, offset++, value <<= 1)
					bitmapArray[offset] = (value & 0x80) ? (ZIL_UINT8)WHITE : (ZIL_UINT8)BLACK;
			}
		}
	}
	for (i = 0; i < bitmapHeight; i++)
	{
		int offset = (bitmapHeight - i - 1) * bitmapWidth;
		file->Read(fbitmap, fwidth, sizeof(ZIL_UINT8));
		for (j = 0; j < twidth; j++)
		{
			ZIL_UINT8 value = fbitmap[j];
			for (int k = 0; k < 8; k++, offset++, value <<= 1)
				if (value & 0x80)
					bitmapArray[offset] = (ZIL_UINT8)BACKGROUND;
		}
	}
	delete fbitmap;
	delete zincColor;

	// Return success.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

// ----- WICO_FILE_HEADER ---------------------------------------------------

WICO_FILE_HEADER::WICO_FILE_HEADER(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR WICO_FILE_HEADER::Load(ZIL_FILE *file)
{
	file->Read(&reserved);
	file->Read(&iconType);
	file->Read(&iconCount);
	return (file->GetError());
}

// ----- WICO_DIRECTORY -----------------------------------------------------

WICO_DIRECTORY::WICO_DIRECTORY(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR WICO_DIRECTORY::Load(ZIL_FILE *file)
{
	file->Read(&width);
	file->Read(&height);
	file->Read(&colorCount);
	file->Read(&reserved1);
	file->Read(&hotspotX);
	file->Read(&hotspotY);
	file->Read(&icoDIBSize);
	file->Read(&icoDIBOffset);
	return (file->GetError());
}

