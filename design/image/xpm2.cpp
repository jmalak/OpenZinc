//	Zinc Application Framework - XPM2.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "image.hpp"
#include "xpm.hpp"

const int maxColors = 16;
const ZIL_UINT8 background = 0xFF;

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ExportXPM(ZIL_FILE *file)
{
	char name[32];
//	ZIL_ICHAR name[32];
	::strcpy(name, "zinc");

	// Determine the appropriate line length.
	int lineLength = bitmapWidth * 2 + 32; // for <Pixels> portion below.
	if (lineLength < 256)
		lineLength = 256;
//	ZIL_ICHAR *line = new ZIL_ICHAR[lineLength];
	char *line = new char[lineLength];

	// Store the image information <Values>.
	::sprintf(line, "/* XPM */\n");
	file->Write(line);

	::sprintf(line, "static char *%s[] = {\n", name);
	file->Write(line);

	::sprintf(line, "/* width height num_colors chars_per_pixel */\n");
	file->Write(line);

	::sprintf(line, "\" %d %d %d %d \",\n", bitmapWidth, bitmapHeight, maxColors, 1);
	file->Write(line);

	// Store the color table <Colors>.
	::sprintf(line, "/* colors */\n");
	file->Write(line);

	int i;
	for (i = 0; i < maxColors; i++)
	{
		::sprintf(line, "\"%s c #%02X%02X%02X\",\n",
			_zincRGBValue[i].pixel, _zincRGBValue[i].rgbRed,
			_zincRGBValue[i].rgbGreen, _zincRGBValue[i].rgbBlue);
			file->Write(line);
	}

	// Store the image <Pixels>.
	::sprintf(line, "/* pixels */\n");
	file->Write(line);

	ZIL_UINT8 *fbitmap = bitmapArray;
	for (i = 0; i < bitmapHeight; i++)
	{
		::strcpy(line, "\"");
		for (int j = 0; j < bitmapWidth; j++, fbitmap++)
		{
			ZIL_UINT8 index = ((ZIL_UINT8)*fbitmap != background) ? (ZIL_UINT8)*fbitmap : 0;
			::strcat(line, _zincRGBValue[index].pixel);
		}
		if (i == bitmapHeight - 1)
			::strcat(line, "\"\n");
		else
			::strcat(line, "\",\n");
		file->Write(line);
	}

	::sprintf(line, "};\n");
	file->Write(line);

	// Store the extensions <Extensions>.

	// Return success.
	delete line;
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

