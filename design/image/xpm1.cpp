//	Zinc Application Framework - XPM1.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include <stdio.h>
#include "image.hpp"
#include "xpm.hpp"

const int maxColors = 16;
const int MAX_LINE_LENGTH = 256;

class PARSE_SERVICE
{
public:
	PARSE_SERVICE(ZIL_FILE *file);
	int FindToken(char *tokenMatch);
	int MatchToken(char **tokenArray, int totalTokens, int count);
	char line[MAX_LINE_LENGTH];

private:
	ZIL_FILE *file;
	int offset;
	int readOffset;
	int readLength;
};

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ImportXPM(ZIL_FILE *file)
{
	int i, j;
	int totalColors, charsPerPixel;
	PARSE_SERVICE service(file);

	// Load the image information <Header>.
	service.FindToken("\"");
	::sscanf(service.line, "%d%d%d%d", &bitmapWidth, &bitmapHeight, &totalColors, &charsPerPixel);
	service.FindToken("\"");

	// Allocate and initialize the bitmap array.
	bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	memset(bitmapArray, 0, bitmapWidth * bitmapHeight);

	// Load the color table <Colors>.
	char *colorName = new char[40];
	char *defaultColor = "black";
	char *poundSign = "#";
	char **colorTable = new char *[maxColors];
	for (i = 0; i < maxColors; i++)
		colorTable[i] = ZIL_NULLP(char);
	for (i = 0; i < totalColors; i++)
	{
		service.FindToken("\""); // start of color.
		int rgbRed, rgbGreen, rgbBlue;
		int colorValue;
		char *pixel = new char[charsPerPixel+1];
		char *tChar = new char[2];
		char *tLine = service.line;
		::strncpy(pixel, tLine, charsPerPixel);
		pixel[charsPerPixel] = '\0';
		tLine += charsPerPixel;
		char *ptr = ::strchr(tLine, 'c');
		tLine = ptr;
		tLine++;
		tLine++;
		::sscanf(tLine, "%s", colorName);
		::strncpy(tChar, colorName, 1);
		tChar[1] = '\0';
		if (!::strcmp(tChar, poundSign))
		{
			colorName++;
			::sscanf(colorName, "%6x", &colorValue);
			colorName--;
			rgbRed = (colorValue & 0xFF0000) >> 16;
			rgbGreen = (colorValue & 0x00FF00) >> 8;
			rgbBlue = colorValue & 0x0000FF;
		}
#if defined(ZIL_XT)
		else
		{
			XColor xColor;
			if (strlen(colorName) <= 2)
				strcpy(colorName, defaultColor);
			XParseColor(display->xDisplay, DefaultColormap(display->xDisplay, display->xScreenNumber), colorName, &xColor);
			rgbRed = xColor.red >> 8;
			rgbGreen = xColor.green >> 8;
			rgbBlue = xColor.blue >> 8;
		}
#endif
		service.FindToken("\""); // end of color.
		delete tChar;

		// Find the zinc color match.
		//line below changed by jdh to conform to ISO "for" scoping rules
		//for (int j = 0; j < maxColors; j++)
		int j;
		for (j = 0; j < maxColors; j++)
			if (_zincRGBValue[j].rgbBlue == rgbBlue &&
				_zincRGBValue[j].rgbGreen == rgbGreen &&
				_zincRGBValue[j].rgbRed == rgbRed)
			{
				colorTable[j] = pixel;
				break;
			}
		if (j == maxColors) // no match was found.
		{
			for (j = 0; j < maxColors; j++)
				if (!colorTable[j])
				{
					colorTable[j] = pixel;
					break;
				}
		}
	}

	// Load the image <Pixels>.
	ZIL_UINT8 *fbitmap = bitmapArray;
	for (i = 0; i < bitmapHeight; i++)
	{
		service.FindToken("\""); // start of scan-line.
		for (j = 0; j < bitmapWidth; j++, fbitmap++)
			*fbitmap = (ZIL_UINT8)service.MatchToken(colorTable, maxColors, charsPerPixel);
		service.FindToken("\""); // end of scan-line.
	}

	// Clean up
	for (i = 0; i < totalColors; i++)
		delete colorTable[i];
	delete colorTable;
	if (colorName)
		delete colorName;

	// Return success.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

// ----- PARSE_SERVICE ------------------------------------------------------

PARSE_SERVICE::PARSE_SERVICE(ZIL_FILE *_file) :
	file(_file), offset(0), readOffset(0), readLength(MAX_LINE_LENGTH - 1)
{
}

int PARSE_SERVICE::FindToken(char *token)
{
	// Parse the file.
	offset = 0;
	while (readLength)
	{
		// Load the parse line.
		readLength = file->Read(&line[readOffset], MAX_LINE_LENGTH - readOffset - 1, sizeof(char));
		readLength += readOffset;
		line[readLength] = '\0';

		// Try to find a matching token.
		char *tline = strstr(line, token);
		if (tline)
		{
			tline += strlen(token);
			readOffset = readLength - (int)(tline - line);
			memcpy(line, tline, readOffset + 1);
			return (TRUE); // Return that match was found.
		}
		else
		{
			readOffset = readLength / 2;
			if (readOffset > readLength)
				readOffset = readLength;
			readLength -= readOffset;
			if (readLength)
				memcpy(line, line + readOffset, readLength);
		}
	}

	// No token match.
	return (FALSE);
}

int PARSE_SERVICE::MatchToken(char **tokenArray, int totalTokens, int count)
{
	for (int i = 0; i < totalTokens; i++)
		if (tokenArray[i]) // check for valid string token.
		{
			char *token = tokenArray[i];
			int j;
			for (j = 0; j < count; j++)
				if (token[j] != line[offset+j])
					break;
			if (j == count)
			{
				offset += count;
				return (i);
			}
		}
	offset += count;
	return (0xFF);
}
