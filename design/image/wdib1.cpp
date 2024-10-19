//	Zinc Application Framework - WDIB1.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "wdib.hpp"

// ----- ZAF_IMAGE ----------------------------------------------------------

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ImportWDIB(ZIL_FILE *file)
{
	int i, j;

	// Load the file header.
	WDIB_FILE_HEADER bmiFileHeader(file);
	if (file->GetError())
		return (SetError(ERROR_FILE_HEADER));

	// Load the info header.
   	WDIB_INFO_HEADER bmiInfoHeader(file);
	if (file->GetError())
		return (SetError(ERROR_INFO_HEADER));

	// Set-up the Zinc color table.
	int colorCount = bmiInfoHeader.biBitCount ? 1 : 2;
	for (i = 0; i < bmiInfoHeader.biBitCount; i++)
		colorCount <<= 1;
   	ZIL_UINT8 *zincColor = new ZIL_UINT8[colorCount];
	for (i = 0; i < colorCount; i++)
	{
   		WDIB_RGB_QUAD bmiColor(file);
   		zincColor[i] = bmiColor.GetZincColor();
	}

	// Initialize the image.
	if (bmiInfoHeader.biWidth * bmiInfoHeader.biHeight > 0x7FFF)
	{
		delete zincColor;
		return (SetError(ERROR_BITMAP_TOO_LARGE));
	}
	bitmapWidth = (ZIL_INT16)bmiInfoHeader.biWidth;
	bitmapHeight = (ZIL_INT16)bmiInfoHeader.biHeight;
	bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];

	// Load the image information.
	int twidth = bitmapWidth / WDIB_INFO_HEADER::_encodingRatio;
	int fwidth = (twidth + sizeof(ZIL_UINT32) - 1) / sizeof(ZIL_UINT32) * sizeof(ZIL_UINT32);
	ZIL_UINT8 *fbitmap = new ZIL_UINT8[fwidth];
	ZIL_UINT8 color;

	// 16-color bitmap information.
	if (colorCount == 16)
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

	// 2-color bitmap information.
	else if (colorCount == 2)
	{
		fwidth = (bitmapWidth + 7) / 8; // ensure proper slack in the read.
		for (i = 0; i < bitmapHeight; i++)
		{
			ZIL_UINT8 mask;
			int offset = (bitmapHeight - i - 1) * bitmapWidth;
			for (j = 0; j < fwidth; j++)
			{
				file->Read(&mask);
				twidth = (j == fwidth && (bitmapWidth % 8)) ? bitmapWidth % 8 : 8;
				for (int k = 0; k < twidth; k++, offset++)
					bitmapArray[offset] = (mask & (ZIL_UINT8)(0x80 >> k)) ? WHITE : BLACK;
			}
			while (j++ % 4)
				file->Read(&mask); // read the slack.
		}
	}

	delete fbitmap;
	delete zincColor;

	// Return success.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

// ----- WDIB_RGB_QUAD ------------------------------------------------------

WDIB_RGB_QUAD::WDIB_RGB_QUAD(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR WDIB_RGB_QUAD::Load(ZIL_FILE *file)
{
	file->Read(&rgbBlue);
	file->Read(&rgbGreen);
	file->Read(&rgbRed);
	file->Read(&rgbReserved);
	return (file->GetError());
}

// ----- WDIB_FILE_HEADER ---------------------------------------------------

WDIB_FILE_HEADER::WDIB_FILE_HEADER(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR WDIB_FILE_HEADER::Load(ZIL_FILE *file)
{
	file->Read(&bfType);
	if (bfType != BFT_BMAP)
		return (file->SetError(ZIL_FILE::ERROR_ACCESS));
	file->Read(&bfSize);
	if (bfSize == 14 + 12 || bfSize == 14 + 64 ||
		bfSize == 14 + 14 + 12 || bfSize == 14 + 14 + 64) // Test for OS/2.
		return (file->SetError(ZIL_FILE::ERROR_ACCESS));
	file->Read(&bfReserved1);
	file->Read(&bfReserved2);
	if (bfReserved1 != 0 || bfReserved2 != 0)
		return (file->SetError(ZIL_FILE::ERROR_ACCESS));
	file->Read(&bfOffBits);
	return (file->GetError());
}

// ----- WDIB_INFO_HEADER ---------------------------------------------------

WDIB_INFO_HEADER::WDIB_INFO_HEADER(ZIL_FILE *file)
{
	Load(file);
}

ZIL_FILE::Z_ERROR WDIB_INFO_HEADER::Load(ZIL_FILE *file)
{
	file->Read(&biSize);
	file->Read(&biWidth);
	file->Read(&biHeight);
	file->Read(&biPlanes);
	file->Read(&biBitCount);
	file->Read(&biCompression);
	file->Read(&biSizeImage);
	file->Read(&biXPelsPerMeter);
	file->Read(&biYPelsPerMeter);
	file->Read(&biClrUsed);
	file->Read(&biClrImportant);
	return (file->GetError());
}

