//	Zinc Application Framework - WICO2.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "wdib.hpp"

// ----- ZAF_IMAGE ----------------------------------------------------------

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ExportWICO(ZIL_FILE *file)
{
	int i, j;

	// Constant declarations.
	const int rgbQuadSize = 4;
	const int maxColors = FlagSet(imFlags, IMF_ICON_IMAGE) ? 16 : 0;
	int twidth = bitmapWidth / WDIB_INFO_HEADER::_encodingRatio;
	int fwidth = (twidth + sizeof(ZIL_UINT32) - 1) / sizeof(ZIL_UINT32) * sizeof(ZIL_UINT32);

	// Store the file header.
	WICO_FILE_HEADER icoFileHeader;
	icoFileHeader.iconType = FlagSet(imFlags, IMF_ICON_IMAGE) ? 1 : 2;
	icoFileHeader.iconCount = 1;
	icoFileHeader.Store(file);

	// Store the info header.
   	WICO_DIRECTORY icoInfoHeader;
   	icoInfoHeader.width = bitmapWidth;
   	icoInfoHeader.height = bitmapHeight;
	icoInfoHeader.colorCount = maxColors;
	icoInfoHeader.reserved1 = 0;
	icoInfoHeader.hotspotX = hotspotX;
	icoInfoHeader.hotspotY = hotspotY;
	if (FlagSet(imFlags, IMF_ICON_IMAGE))
		icoInfoHeader.icoDIBSize = WDIB_INFO_HEADER::_size + // bitmap info
			rgbQuadSize * maxColors + // colors
			fwidth * bitmapHeight + // color bit-map
			bitmapWidth / 8 * 32; // mono mapping
	else
		icoInfoHeader.icoDIBSize = WDIB_INFO_HEADER::_size + // bitmap info
			rgbQuadSize * 2 + // colors
			bitmapWidth / 8 * 32 + // color bit-map
			bitmapWidth / 8 * 32; // mono mapping
	icoInfoHeader.icoDIBOffset =
		WICO_FILE_HEADER::_size +
		WICO_DIRECTORY::_size;
	icoInfoHeader.Store(file);

	// Store the info header.
	WDIB_INFO_HEADER bmiInfoHeader;
	bmiInfoHeader.biSize = WDIB_INFO_HEADER::_size;
	bmiInfoHeader.biWidth = bitmapWidth;
	bmiInfoHeader.biHeight = bitmapHeight * 2; // color & mono
	bmiInfoHeader.biPlanes = 1;
	bmiInfoHeader.biCompression = 0; // no compression.
	if (FlagSet(imFlags, IMF_ICON_IMAGE))
	{
		bmiInfoHeader.biBitCount = 4; // 16 colors.
		bmiInfoHeader.biSizeImage = fwidth * bitmapHeight + // color
			bitmapWidth / 8 * bitmapHeight; // mono
	}
	else
	{
		bmiInfoHeader.biBitCount = 1; // mono.
		bmiInfoHeader.biSizeImage = bitmapWidth / 8 * bitmapHeight + // color
			bitmapWidth / 8 * bitmapHeight; // color
	}
	bmiInfoHeader.biSizeImage += 
	bmiInfoHeader.biXPelsPerMeter = 0;
	bmiInfoHeader.biYPelsPerMeter = 0;
	bmiInfoHeader.biClrUsed = 0; 			// use default specified colors.
	bmiInfoHeader.biClrImportant = maxColors;
	bmiInfoHeader.Store(file);

	// Store the color table.
	if (FlagSet(imFlags, IMF_ICON_IMAGE))
	{
		for (i = 0; i < maxColors; i++)
		{
			WDIB_RGB_QUAD rgbColor(WDIB_RGB_QUAD::_zincRGBValue[i]);
			rgbColor.Store(file);
		}
	}
	else
	{
		WDIB_RGB_QUAD black(WDIB_RGB_QUAD::_zincRGBValue[0x00]);
		black.Store(file);
		WDIB_RGB_QUAD white(WDIB_RGB_QUAD::_zincRGBValue[0x0F]);
		white.Store(file);
	}

	// Store the image.
	ZIL_UINT8 *fbitmap = new ZIL_UINT8[fwidth];
	for (i = twidth; i < fwidth; i++) // pad the offset area with zeros.
		fbitmap[i] = 0;
	if (FlagSet(imFlags, IMF_ICON_IMAGE))
	{
		for (i = 0; i < bitmapHeight; i++)
		{
			int offset = (bitmapHeight - i - 1) * bitmapWidth;
			for (j = 0; j < twidth; j++, offset += 2)
				fbitmap[j] = ((bitmapArray[offset] & 0x0F) << 4) | (bitmapArray[offset+1] & 0x0F);
			file->Write(fbitmap, fwidth, sizeof(ZIL_UINT8));
		}
	}

	// Store the mono image information.
	twidth = fwidth = bitmapWidth / 8; // mono sets each bit.
	if (!FlagSet(imFlags, IMF_ICON_IMAGE))
	{
		for (i = 0; i < bitmapHeight; i++)
		{
			int offset = (bitmapHeight - i - 1) * bitmapWidth;
			for (j = 0; j < twidth; j++)
			{
				ZIL_UINT8 value = 0;
				for (int k = 0; k < 8; k++, offset++)
				{
					value <<= 1;
					if (bitmapArray[offset])
						value |= 0x01;
				}
				fbitmap[j] = value;
			}
			file->Write(fbitmap, fwidth, sizeof(ZIL_UINT8));
		}
	}
	for (i = 0; i < bitmapHeight; i++)
	{
		int offset = (bitmapHeight - i - 1) * bitmapWidth;
		for (j = 0; j < twidth; j++)
		{
			ZIL_UINT8 value = 0;
			for (int k = 0; k < 8; k++, offset++)
			{
				value <<= 1;
				if (bitmapArray[offset] == BACKGROUND)
					 value |= 0x01;
			}
			fbitmap[j] = value;
		}
		file->Write(fbitmap, fwidth, sizeof(ZIL_UINT8));
	}
	delete fbitmap;

	// Return the file status.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

// ----- WICO_FILE_HEADER ---------------------------------------------------

ZIL_FILE::Z_ERROR WICO_FILE_HEADER::Store(ZIL_FILE *file)
{
	file->Write(reserved);
	file->Write(iconType);
	file->Write(iconCount);
	return (file->GetError());
}

// ----- WICO_DIRECTORY -----------------------------------------------------

ZIL_FILE::Z_ERROR WICO_DIRECTORY::Store(ZIL_FILE *file)
{
	file->Write(width);
	file->Write(height);
	file->Write(colorCount);
	file->Write(reserved1);
	file->Write(hotspotX);
	file->Write(hotspotY);
	file->Write(icoDIBSize);
	file->Write(icoDIBOffset);
	return (file->GetError());
}

