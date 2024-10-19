//	Zinc Interface Library - M_PRINT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

/*       This file is a part of OpenZinc

          OpenZinc is free software; you can redistribute it and/or modify it under
          the terms of the GNU Lessor General Public License as published by
          the Free Software Foundation, either version 3 of the License, or (at
          your option) any later version

	OpenZinc is distributed in the hope that it will be useful, but WITHOUT
          ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
          or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
          General Public License for more details.

          You should have received a copy of the GNU Lessor General Public License
	 along with OpenZinc. If not, see <http://www.gnu.org/licenses/>                          */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ui_dsp.hpp"

static FILE *_outputFile;

UI_PRINTER::UI_PRINTER(UI_DISPLAY *_display) : UI_DISPLAY(TRUE),
	display(_display)
{
	isText = FALSE;

	// Set display members.
	if (display)
	{
		columns = display->columns;
		lines = display->lines;
		cellWidth = display->cellWidth;
		cellHeight = display->cellHeight;
	}
	else
	{
		columns = 612;
		lines = 792;
		cellWidth = 8;
		cellHeight = 23;
	}
#if defined(ZIL_POSIX)
	systemCommand = ZIL_NULLP(char);
#endif
    installed = TRUE;
}

UI_PRINTER::~UI_PRINTER(void)
{
}

void UI_PRINTER::Bitmap(ZIL_SCREENID, int x, int y,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *,	const UI_REGION *,
		ZIL_BITMAP_HANDLE *, ZIL_BITMAP_HANDLE *)
{
	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
	{
		int tWidth = bitmapWidth % 2 ? bitmapWidth + 1 : bitmapWidth;
		fprintf(_outputFile, "%d %d %d %d Bitmap\n", x, y, tWidth, bitmapHeight);
		//lines below chaged by jdh to conform to current scoping of "for"
		//for (int i = 0; i < bitmapHeight; i++)
		int i;
		for ( i = 0; i < bitmapHeight; i++)
		{
			for (int j = 0; j < bitmapWidth; j++)
			 	fprintf(_outputFile, "%x", bitmapArray[i*bitmapWidth+j] % 16);
			if (bitmapWidth % 2)
				fprintf(_outputFile, "f");
			fprintf(_outputFile, "\n");
		}
		if (bitmapHeight % 2)
		{
			for (i = 0; i < tWidth; i++)
				fprintf(_outputFile, "f");
			fprintf(_outputFile, "\n");
		}
	}
}

void UI_PRINTER::Ellipse(ZIL_SCREENID, int x, int y,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill, int,
		const UI_REGION *)
{
	int fColor = (int)palette->colorForeground % 16;
	int bColor = (int)palette->colorBackground % 16;

	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
	{
	 	fprintf(_outputFile, "%d %d %d %d %d %d %s %d %s 1 Ellipse\n",
			startAngle, endAngle, xRadius, yRadius, x, y,
			psColorTable[bColor], fill, psColorTable[fColor]);
	}
}

void UI_PRINTER::Line(ZIL_SCREENID, int x1, int y1, int x2,	int y2,
	const UI_PALETTE *palette, int width, int,	const UI_REGION *)
{
	int fColor = (int)palette->colorForeground % 16;

	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
	{
		fprintf(_outputFile, "%s %d %d %d %d %d Line\n",
			psColorTable[fColor], width, x2, y2, x1, y1);
	}
}


ZIL_COLOR UI_PRINTER::MapColor(const UI_PALETTE *palette, int foreground)
{
	return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_PRINTER::Polygon(ZIL_SCREENID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill, int, const UI_REGION *)
{
	int fColor = (int)palette->colorForeground % 16;
	int bColor = (int)palette->colorBackground % 16;

	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
	{
		int x, y;
		int isClosed = 0;
		int isFrame = fColor != bColor;

		if (numPoints > 2 && polygonPoints[0] == polygonPoints[numPoints * 2 - 2] &&
			polygonPoints[1] == polygonPoints[numPoints * 2 - 1])
			isClosed = 1;

	 	fprintf(_outputFile, "[");
		for (int i = 0; i < numPoints * 2; i+=2)
		{
			x = polygonPoints[i];
			y = polygonPoints[i+1];
		 	fprintf(_outputFile, "[%d %d]", x, y);
		}
	 	fprintf(_outputFile, "] %d %s %d %s %d %d Polygon\n", numPoints,
			psColorTable[bColor], fill, psColorTable[fColor], isFrame, isClosed);
	}
}

void UI_PRINTER::Rectangle(ZIL_SCREENID, int x1, int y1, int x2, int y2, const UI_PALETTE *palette, int width,
		int fill, int, const UI_REGION *)
{
	int fColor = (int)palette->colorForeground % 16;
	int bColor = (int)palette->colorBackground % 16;

	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
	{
	 	fprintf(_outputFile, "%d %d %d %d %d %s %d %s Rect\n",x2-x1, y2-y1,
			x1, y1, width, psColorTable[fColor], fill, psColorTable[bColor]);
	}
}

void UI_PRINTER::Text(ZIL_SCREENID, int x, int y, const ZIL_ICHAR *text,
	const UI_PALETTE *palette, int length, int, int, const UI_REGION *, ZIL_LOGICAL_FONT font)
{
	static ZIL_ICHAR fillLine[256];
	if (length < 0)
		length = strlen(text);
	else if (length > 255)
		length = 255;
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';

	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
	{
		int fColor = (int)palette->colorForeground % 16;
		char *tmpStr;
#if defined(ZIL_UNICODE)
		tmpStr = MapText(fillLine);
#else
		tmpStr = fillLine;
#endif
 		fprintf(_outputFile, "%s %d %d %s %d Text (", psColorTable[fColor],
			x, y,	psFontTable[font].typeFace, psFontTable[font].pointSize);
		for(char *ptr = tmpStr; *ptr; ptr++)
			if (*ptr == '(')
				fprintf(_outputFile, "\\050");
			else if (*ptr == ')')
				fprintf(_outputFile, "\\051");
			else
				fprintf(_outputFile, "%c", *ptr);
		fprintf(_outputFile,") show\n");
#if defined(ZIL_UNICODE)
		delete tmpStr;
#endif
	}
}

int UI_PRINTER::TextHeight(const ZIL_ICHAR *, ZIL_SCREENID, ZIL_LOGICAL_FONT font)
{
	return psFontTable[font].pointSize;
}

int UI_PRINTER::TextWidth(const ZIL_ICHAR *string,	ZIL_SCREENID, ZIL_LOGICAL_FONT)
{
	// Return an approximate width.
	if (string)
		return strlen(string) * 8;
	return 0;
}

int UI_PRINTER::VirtualGet(ZIL_SCREENID, int, int, int, int)
{
	return 0;
}

int UI_PRINTER::VirtualPut(ZIL_SCREENID)
{
	return 0;
}

int UI_PRINTER::BeginPrintJob(ZIL_PRINTER_MODE _printerMode, char *fileName)
{
	printerMode = _printerMode;

#if defined(ZIL_POSIX)
	if (printerMode == PRM_DEFAULT)
	{
		if (!fileName)
			fileName = "lpr";
		tmpName = ::tmpnam(ZIL_NULLP(char));
		_outputFile = fopen(tmpName, "w+");
		systemCommand = new char[::strlen(fileName) + 128];
		::sprintf(systemCommand, "cat %s | %s", tmpName, fileName);
	}
	else
#endif
		_outputFile = fopen(fileName, "w+");
	if (!_outputFile)
		return(FALSE);

	if (fileName)
	{
		printerMode = PRM_POSTSCRIPT;
		_outputFile = fopen(fileName, "w+");
		if (!_outputFile)
			return FALSE;

		fprintf(_outputFile, "%%!PS\n");
	
		if (!isText)
		{
			// Print PostScript routines;
			fprintf(_outputFile, "/Bitmap { %%args: x, y, width, height\n"
				"/height exch def /width exch def /m matrix currentmatrix def /temp width string def\n"
				"translate newpath width height neg scale width height 4 [ width 0 0 height neg 0 0 ]\n"
				"{ currentfile temp readhexstring pop } image m setmatrix } def\n");
			fprintf(_outputFile, "/Ellipse { %%args: sAng, eAng, xRad, yRad, x, y, bkgd, isBg, fgnd, isFg\n"
				"/isFrame exch def /fgColor exch def /isFill exch def /bgColor exch def\n"
				"/m matrix currentmatrix def translate 0 0 moveto newpath 1 index neg div 1 exch scale\n"
				"0 0 5 2 roll 3 1 roll arc closepath m setmatrix isFill 0 ne { gsave bgColor setgray\n"
				"fill grestore } if isFrame 0 ne { 1 setlinewidth fgColor setgray stroke } if } def\n");
			fprintf(_outputFile, "/Line { %%args: lineColor, lineWidth, x2, y2, x1, y1\n"
				"newpath moveto lineto setlinewidth setgray stroke } def\n");
			fprintf(_outputFile, "/Polygon { %%args: points, numPoints bkgd, isBg, fgnd, isFg, isClosed\n"
				"/isClosed exch def /isFrame exch def /fgColor exch def /isFill exch def /bgColor exch def\n"
				"/numPoints exch def /points exch def newpath points 0 get aload pop moveto 1 1 numPoints\n"
				"1 sub { points exch get aload pop lineto } for isClosed 0 ne { closepath } if isFill 0 ne\n"
				"{ gsave bgColor setgray fill grestore } if isFrame 0 ne { 1 setlinewidth fgColor setgray\n"
				"stroke } if } def\n");
			fprintf(_outputFile, "/Rect { %%args: width, height, x1, y1 lWidth fgColor, isFill bgColor\n"
				"/bgColor exch def /isFill exch def /fgColor exch def /lineWidth exch def newpath moveto\n"
				"1 index 0 rlineto 0 exch rlineto neg 0 rlineto closepath isFill 0 ne { gsave bgColor\n"
				"setgray fill grestore } if lineWidth 1 ge { lineWidth setlinewidth fgColor setgray\n"
				"stroke } if } def\n");
		}
		fprintf(_outputFile, "/Text { %%args: x, y, fontName, size, textColor\n"
			"/size exch def newpath findfont [size 0 0 size neg 0 size] makefont setfont moveto\n"
			"setgray } def\n\n");
	}
	return TRUE;
}

void UI_PRINTER::EndPrintJob(void)
{
	// finish print job, close file.
	if (printerMode == PRM_POSTSCRIPT)
	{
		fprintf(_outputFile, "%%%%Trailer");
		fclose(_outputFile);
	}
#if defined(ZIL_POSIX)
	else if (printerMode == PRM_DEFAULT)
	{
		fclose(_outputFile);
		system(systemCommand);
		::unlink(tmpName);
	}
#endif
	_outputFile = ZIL_NULLP(FILE);

}

void UI_PRINTER::BeginPage(void)
{
	if (printerMode == PRM_POSTSCRIPT)
	{
		// Set up environment (8.5 x 11 portrait mode)
		if (display)
		{
			float temp = 572.0 / display->columns;
			fprintf(_outputFile, "18 774 translate %f %f scale\n", temp, -temp);
		}
		else
			fprintf(_outputFile, "18 774 translate 1 -1 scale\n");
	}
}

void UI_PRINTER::BeginPage(int left, int top, int right, int bottom,
	int orientation, int)
{
	if (printerMode == PRM_POSTSCRIPT)
	{
		if (orientation == PRM_LANDSCAPE)
		{
			float xScale = (right - left) ? 792.0 / (right - left) : 1.0;
			float yScale = (bottom - top) ? 612.0 / (bottom - top) : 1.0;
			columns = (right - left) ? 792 : right-left;
			lines = (bottom - top) ? 612 : bottom-top;
			fprintf(_outputFile, "%d %d translate %f %f scale -90 rotate\n",
				left, top, xScale, -yScale);
		}
		else
		{
			float xScale = (right - left) ? 612.0 / (right - left) : 1.0;
			float yScale = (bottom - top) ? 792.0 / (bottom - top) : 1.0;
			columns = (right - left) ? 612 : right-left;
			lines = (bottom - top) ? 792 : bottom-top;
			fprintf(_outputFile, "%d %d translate %f %f scale\n",
				left, bottom, xScale, -yScale);
		}
 	}
	else
		BeginPage();
}

void UI_PRINTER::EndPage(void)
{
	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
		fprintf(_outputFile, "showpage\n");
	else
		;
}

void UI_PRINTER::ScreenDump(ZIL_SCREENID, UI_REGION &region,
	ZIL_PRINTER_MODE pMode, char *fileName)
{
	if (!display || display->isText || isText)
		return;

	static ZIL_UINT8 _bmpColorTable[] = {
   	0, 1,	2,	5,	3,	6,	4,	11, 7, 8, 9, 13, 10, 12, 14, 15
	};

	int x = region.left;
	int y = region.top;
	int width = region.Width();
	int height = region.Height();

	BeginPrintJob(pMode, fileName);
	BeginPage();
	
	if (printerMode == PRM_POSTSCRIPT || printerMode == PRM_DEFAULT)
	{
		int tWidth = width % 2 ? width + 1 : width;

		fprintf(_outputFile, "%d %d %d %d Bitmap\n", x, y, tWidth, height);
		int pixel;
		ZIL_UINT8 color;
		XImage *image = XGetImage(display->xDisplay,
			RootWindow(display->xDisplay, display->xScreenNumber),
			region.left, region.top, region.Width(), region.Height(),
			0xFFFFL, XYPixmap);
		XColor xcolor;
		xcolor.flags =  DoRed | DoGreen | DoBlue;
		if (DefaultDepthOfScreen(display->xScreen) <= 8)
		{
			// Take advantage of small colormaps for speed.
			unsigned maxPixels = 1 << DefaultDepthOfScreen(display->xScreen);
			unsigned long *pixels = new unsigned long[maxPixels];
			memset(pixels, -1, maxPixels * sizeof(unsigned long));
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					// Get pixel value from display.
					unsigned long xPixel = XGetPixel(image, j, i);
					if (pixels[xPixel] == -1)
					{
						xcolor.pixel = xPixel;
						XQueryColor(display->xDisplay,
							DefaultColormap(display->xDisplay,
							display->xScreenNumber), &xcolor);
	
						// Crude algorythm to determine intensity 0 <= i < 16.
						pixels[xPixel] = ((xcolor.red + xcolor.green +
							xcolor.blue) * 10) / 512;
					}
					color = _bmpColorTable[pixels[xPixel] % 16];
					fprintf(_outputFile, "%x", color);
				}
				if (width % 2)
					fprintf(_outputFile, "f");
				fprintf(_outputFile, "\n");
			}
			delete pixels;
		}
		else
		{
			// Brute force (slow, very, very slow).
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					// Get pixel value from display.
					unsigned long xPixel = XGetPixel(image, j, i);
					xcolor.pixel = xPixel;
					XQueryColor(display->xDisplay,
						DefaultColormap(display->xDisplay, display->xScreenNumber),
						&xcolor);
	
					// Crude algorythm to determine intensity 0 <= i < 16.
					pixel = ((xcolor.red + xcolor.green + xcolor.blue) * 10) / 512;
	
					color = _bmpColorTable[pixel % 16];
					fprintf(_outputFile, "%x", color);
				}
				if (width % 2)
					fprintf(_outputFile, "f");
				fprintf(_outputFile, "\n");
			}
		}
		XDestroyImage(image);
		if (height % 2)
		{
			for (int i = 0; i < tWidth; i++)
				fprintf(_outputFile, "f");
			fprintf(_outputFile, "\n");
		}
	}
	EndPage();
	EndPrintJob();
}

void UI_PRINTER::TextFormat(ZIL_SCREENID, int x, int y, ZIL_ICHAR *text,
	const UI_PALETTE *palette, int width, ZIL_LOGICAL_FONT font)
{
	const int BUFFER_LENGTH = 256;
 	int row = y, lineWidth = 0;
	int printLine = 0, printPage = 0;

	if (width < 0 || width >= columns)
		width = 70;

	ZIL_ICHAR lineBuffer[BUFFER_LENGTH];
	ZIL_ICHAR *ptr = text;

	while (ptr && *ptr)
	{
		if (*ptr == '\r' || *ptr == '\t')
			ptr++;
		else if (*ptr == '\n')
			ptr++, printLine = TRUE;
		else
		{
			lineBuffer[lineWidth++] = *ptr++;
			if (lineWidth == width)
			{
				// backup to a space character
				int index = lineWidth;
				//line below chaged by jdh to conform to current scoping of "for"
				//for (ZIL_ICHAR *space = ptr; *space != ' ' && *space != '-' &&
				//	index; space--, index--)
				ZIL_ICHAR *space;
				for (space = ptr; *space != ' ' && *space != '-' &&
					index; space--, index--)
					;
				if (index)
				{
					ptr = space;
					while (*ptr == ' ') ++ptr;
					if (*ptr == '-') ++index, ++ptr;
					lineWidth = index;
				}
				printLine = TRUE;
			}
		}
		if (*ptr == '\0')
			printLine = TRUE;
		if (printLine)
		{
			lineBuffer[lineWidth] = '\0';
			if (lineWidth)
				Text(0, x, row, lineBuffer, palette, -1, FALSE, FALSE,
					ZIL_NULLP(UI_REGION), font);
			row += TextHeight(lineBuffer) + TextHeight(lineBuffer) / 3;

			lineWidth = printLine = 0;
			if (row > lines - 108 && *ptr)
				printPage = TRUE;
		}
		if (printPage)
		{
			printPage = 0;
			row = 36;
			EndPage();
			BeginPage();
		}
	}
}
