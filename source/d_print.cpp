// D_PRINT.CPP - Printer display class.
// COPYRIGHT (C) 1990-1995.  All Rights Reserved.
// Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <stdlib.h>

#include "ui_dsp.hpp"

#if defined (ZIL_MSDOS)
#	include <dos.h>
#	if defined (GFX)
#		include <gfx.h>
#		include <gfx_pro.h>
#	elif defined (BGI)
#		include <graphics.h>
#	elif defined(MSC)
#		if defined (DOS386)
#			include <graph32.h>
#		else
#			include <graph.h>
#		endif
#	elif defined(WCC)
#		include <graph.h>
#	elif defined(META)
#		include <metawndo.h>
#	endif
#elif defined(ZIL_CURSES)
#   include <unistd.h>
#endif

// ----- DOS ---------------------------------------------------------
#if defined (ZIL_MSDOS)
struct PAGE_IMAGE
{
	long totalSize;
	int blockSize;
	int blockLines;
	char *blocks[4];
};

static FILE *_outputFile;
static FILE *_prnFile;
static PAGE_IMAGE _page;

UI_PRINTER::PRINTERFONT UI_PRINTER::fontTable[ZIL_MAXFONTS] =
{
	{ 8, 1, 10,  9, 0, 0, 4148 },	// FNT_SMALL_FONT
	{ 8, 1, 10, 11, 0, 0, 4148 },	// FNT_DIALOG_FONT
	{ 8, 1, 10, 13, 0, 0, 4148 },	// FNT_SYSTEM_FONT
	{ 8, 0, 12, 10, 0, 0, 4099 },
	{ 8, 0, 12, 10, 0, 0, 4099 },
	{ 8, 0, 12, 10, 0, 0, 4099 },
	{ 8, 0, 12, 10, 0, 0, 4099 },
	{ 8, 0, 12, 10, 0, 0, 4099 },
	{ 8, 0, 12, 10, 0, 0, 4099 },
	{ 8, 0, 12, 10, 0, 0, 4099 },
};

// ------ PCL font structure members ---------------------------------------
//
// symbolset	(Only values with U extension supported)
//		1F - ISO-69 French       0N - ISO-8859-1 Latin
//		0U - IDS 6:Ascii         1U - Legal
//		8U - Roman 8             10U- PC-8
//		0Y - Barcode             19U- Windows 3.1 Latin
//
// spacing
//		0 - Fixed                1 - Proportional
//
// pitch (Range: > 0)
//
// height (Range: 0 - 999)
//
// style
//		0   - Solid upright      1   - Italic
//		4   - Condensed          5   - Condensed Italic
//		8   - Extra condensed    24  - Expanded
//		32  - Outline            64  - Inline
//		128 - Shadowed           160 - Outline shadowed
//
// weight (Range: -7 - 7)
//		-7 - Ultra thin          -6 - Extra thin
//		-5 - Thin                -4 - Extra light
//		-3 - Light               -2 - Demi light
//		-1 - Semi light
//		 0 - Medium, Book, or Text
//		 1 - Semi Bold            2 - Demi Bold
//		 3 - Bold                 4 - Extra Bold
//		 5 - Black                6 - Extra Black
//		 7 - Ultra Black
//	
// family
//		0    - Line Printer       16602 - Arial
//		4168 - Antique Olive      4127  - ITC Avant Garde
//		4119 - CG Century         4101  - CG Times
//    4148 - Universal

UI_PRINTER::UI_PRINTER(UI_DISPLAY *_display) : UI_DISPLAY(TRUE),
	display(_display), printerMode(PRM_DEFAULT), lPort(0)
{
	port[0] = 0;
	tmpFileName = 0;

	char *zilPrinter = ::getenv("ZINC_PRINTER");
	char tokenString[32];
	if (zilPrinter)
	{
		::strcpy(tokenString, zilPrinter);
		char *token = ::strtok(tokenString, ",");
		if (!::strcmp(token, "PS"))
			printerMode = PRM_POSTSCRIPT;
		else if (!::strcmp(token, "PCL"))
			printerMode = PRM_LASER;
		else if (!::strcmp(token, "DM9"))
			printerMode = PRM_DOTMATRIX9;
		else if (!::strcmp(token, "DM24"))
			printerMode = PRM_DOTMATRIX24;
		token = ::strtok(0, "\0");
		if (token)
			::strcpy(port, token);
	}
	else
	{
		printerMode = PRM_LASER;
		::strcpy(port, "LPT1");
	}

	// Set display members.
	if (display && display->isText)
	{
		// Set up printer for VGA printing.
		columns = 640;
		lines = 480;
		cellWidth = 7;
		cellHeight = 23;
	}
	else if (display)
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
	dotMatrixLinesPerPage = 66;
	dotMatrixColumnsPerPage = 80;
	// Top of page is what user physically sets it to.
	dotMatrixBottomMargin = 8;
	isText = FALSE;
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
	if (printerMode == PRM_POSTSCRIPT)
	{
		int tWidth = bitmapWidth % 2 ? bitmapWidth + 1 : bitmapWidth;
		fprintf(_outputFile, "%d %d %d %d Bitmap\n", x, y, tWidth, bitmapHeight);

		int i;
		for (i = 0; i < bitmapHeight; i++)
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
	else
	{
		// Bitmap routine for Laser printers.
		for (int i = 0; i < bitmapHeight; i++)
			for (int j = 0; j < bitmapWidth; j++, bitmapArray++)
				SetPoint(x + j, y + i, MonoColor(*bitmapArray));
	}
}

void UI_PRINTER::Ellipse(ZIL_SCREENID, int x, int y,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill, int,
		const UI_REGION *)
{
	int fColor = (int)palette->colorForeground % 16;
	int bColor = (int)palette->colorBackground % 16;

	if (printerMode == PRM_POSTSCRIPT)
	{
	 	fprintf(_outputFile, "%d %d %d %d %d %d %s %d %s %d Ellipse\n",
			startAngle, endAngle, xRadius, yRadius, x, y,
			psColorTable[bColor], fill, psColorTable[fColor],
			(AbsValue(endAngle - startAngle) == 360));
	}
	else
	{
		int x1 = x - xRadius;
		int y1 = y - yRadius;
		int x2 = x + xRadius;
		int y2 = y + yRadius;

		long w = (long)x2 - x1 + 1;
		long h = (long)y2 - y1 + 1;

		for (int x = 1; x <= w / 2; x++)
		{
			int y = 1;
			while (y < h && 4*h*h*x*x + 4*w*w*y*y > 4*w*h*h*x + 4*w*w*h*y - w*w*h*h)
			++y;
			SetPoint(x1 + x, y1 + y - 1, MonoColor(fColor));
			SetPoint(x1 + x, y2 - y + 1, MonoColor(fColor));
			SetPoint(x2 - x, y1 + y - 1, MonoColor(fColor));
			SetPoint(x2 - x, y2 - y + 1, MonoColor(fColor));
		}
		for (int y = 1; y <= h / 2; y++)
		{
			int x = 1;
			while (x < w && 4*h*h*x*x + 4*w*w*y*y > 4*w*h*h*x + 4*w*w*h*y - w*w*h*h)
			++x;
			SetPoint(x1 + x - 1, y1 + y, MonoColor(fColor));
			SetPoint(x2 - x + 1, y1 + y, MonoColor(fColor));
			SetPoint(x1 + x - 1, y2 - y, MonoColor(fColor));
			SetPoint(x2 - x + 1, y2 - y, MonoColor(fColor));
		}
		if (fill)
		{
 			for (int y = 0; y <= h / 2; y++)
				for (int x = 0; x <= w / 2; x++)
 					if (4*h*h*x*x + 4*w*w*y*y <= 4*w*h*h*x + 4*w*w*h*y - w*w*h*h)
					{
						SetPoint(x1 + x, y1 + y, MonoColor(bColor));
						SetPoint(x1 + x, y2 - y, MonoColor(bColor));
						SetPoint(x2 - x, y1 + y, MonoColor(bColor));
						SetPoint(x2 - x, y2 - y, MonoColor(bColor));
					}
		}
	}
}

void UI_PRINTER::Line(ZIL_SCREENID, int x1, int y1, int x2,	int y2,
	const UI_PALETTE *palette, int width, int,	const UI_REGION *)
{
	int fColor = (int)palette->colorForeground % 16;

	if (printerMode == PRM_POSTSCRIPT)
	{
		fprintf(_outputFile, "%s %d %d %d %d %d Line\n",
			psColorTable[fColor], width, x2, y2, x1, y1);
	}
	else
		PrintLine(x1, y1, x2, y2, MonoColor(fColor));
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

	if (printerMode == PRM_POSTSCRIPT)
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
	else
	{
		int x1, y1, x2, y2;
		for (int i = 0, j = 0; j < numPoints - 1; i+=2, j++)
		{
			x1 = polygonPoints[i];
			y1 = polygonPoints[i+1];
			x2 = polygonPoints[i+2];
			y2 = polygonPoints[i+3];
			PrintLine(x1, y1, x2, y2, MonoColor(fColor));
		}
	}
}

void UI_PRINTER::Rectangle(ZIL_SCREENID, int x1, int y1, int x2, int y2, const UI_PALETTE *palette, int width,
		int fill, int, const UI_REGION *)
{
	int fColor = (int)palette->colorForeground % 16;
	int bColor = (int)palette->colorBackground % 16;

	if (printerMode == PRM_POSTSCRIPT)
	{
	 	fprintf(_outputFile, "%d %d %d %d %d %s %d %s Rect\n",x2-x1, y2-y1,
			x1, y1, width, psColorTable[fColor], fill, psColorTable[bColor]);
	}
	else
	{
		if (fill)
			for (int line = y1; line <= y2; line++)
				PrintLine(x1, line, x2, line, MonoColor(bColor));
		if (width)
			for (int i = 0; i < width; i++)
			{
				PrintLine(x1, y1, x2, y1, MonoColor(fColor));
				PrintLine(x1, y2, x2, y2, MonoColor(fColor));
				PrintLine(x1, y1, x1, y2, MonoColor(fColor));
				PrintLine(x2, y1, x2, y2, MonoColor(fColor));
				x1++, y1++, x2--, y2--;
			}
	}
}

void UI_PRINTER::Text(ZIL_SCREENID, int x, int y, const ZIL_ICHAR *text,
	const UI_PALETTE *palette, int length, int, int, const UI_REGION *, ZIL_LOGICAL_FONT font)
{
	static ZIL_ICHAR ZIL_FARDATA fillLine[256];
	if (length < 0)
		length = strlen(text);
	else if (length > 255)
		length = 255;
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';

	if (printerMode == PRM_POSTSCRIPT)
	{
		int fColor = (int)palette->colorForeground % 16;
#if defined(ZIL_UNICODE)
		char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ fillLine
#endif
 		fprintf(_outputFile, "%s %d %d %s %d Text (", psColorTable[fColor],
			x, y,	psFontTable[font].typeFace, psFontTable[font].pointSize);
		for(char *ptr = TEXT_; *ptr; ptr++)
			if (*ptr == '(')
				fprintf(_outputFile, "\\050");
			else if (*ptr == ')')
				fprintf(_outputFile, "\\051");
			else
				fprintf(_outputFile, "%c", *ptr);
		fprintf(_outputFile,") show\n");
	}
	else if (printerMode == PRM_LASER)
	{
		fprintf(_prnFile, "\033*p%dx%dY", x * 4, y * 4);
		fprintf(_prnFile, "\033(%dU\033(s%dp%dh%dv%ds%db%dT",
			fontTable[font].symbolset, fontTable[font].spacing,
			fontTable[font].pitch,	fontTable[font].height,
			fontTable[font].style, fontTable[font].weight,
			fontTable[font].family);
#if defined(ZIL_UNICODE)
		char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ fillLine
#endif
		fprintf(_prnFile, "%s", TEXT_);
	}
	// Text print calls must be done from left to right, top to bottom
	else if (printerMode == PRM_DOTMATRIX9 || printerMode == PRM_DOTMATRIX24)
	{
#if defined(ZIL_UNICODE)
		char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
#else
#		if defined TEXT_
#			undef TEXT_
#		endif
#		define TEXT_ text
#endif
		int dx,dy, i;
		char tString[256];

		// init tString
		for (i = 0;i < 80; i++)
			tString[i] = '\0';

		// convert pixel x, y to dot matrix line column
		if (!(display && display->isText))
		{
			dx = x/cellWidth;
			dy = y/cellHeight;
		}

		// correct for attempt to write off page margin
		if (dy > currentPrintheadPositionY)
			currentPrintheadPositionX = 0;

		// pad with ' ' - this positions printer in x direction
		for (i = 0; currentPrintheadPositionX < dx; currentPrintheadPositionX++)
			tString[i++] = ' ';

		// Wrap if off edge of page
		if (currentPrintheadPositionX > dotMatrixColumnsPerPage)
		{
			currentPrintheadPositionX %= dotMatrixColumnsPerPage;
			currentPrintheadPositionY += currentPrintheadPositionX / dotMatrixColumnsPerPage;
		}

		// This positions printer in y direction (down only)
		for (; currentPrintheadPositionY < dy; currentPrintheadPositionY++)
			PrintDotMatrixString("\r\n");// carraige return

		::strcat(tString,TEXT_);
		PrintDotMatrixString(tString);
	}
}

int UI_PRINTER::TextHeight(const ZIL_ICHAR *, ZIL_SCREENID, ZIL_LOGICAL_FONT font)
{
	if (printerMode == PRM_POSTSCRIPT)
		return psFontTable[font].pointSize;
	return fontTable[font].height;
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

int UI_PRINTER::BeginPrintJob(ZIL_PRINTER_MODE printMode, char *fileName)
{
	if (printMode != PRM_DEFAULT)
		printerMode = printMode;

	if (!fileName && printerMode == PRM_POSTSCRIPT && *port)
		fileName = port;

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
			fprintf(_outputFile, "/Ellipse { %%args: sAng, eAng, xRad, yRad, x, y, bkgd, isBg, fgnd, is360\n"
				"/is360 exch def /fgColor exch def /isFill exch def /bgColor exch def\n"
				"/m matrix currentmatrix def translate is360 0 ne { 0 0 moveto newpath } if is360 0 eq\n"
				"{ newpath 0 0 moveto } if 1 index neg div 1 exch scale\n"
				"0 0 5 2 roll 3 1 roll arc closepath m setmatrix isFill 0 ne { gsave bgColor setgray\n"
				"fill grestore } if 1 setlinewidth fgColor setgray stroke } def\n");
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
	else if (printerMode == PRM_LASER || printerMode == PRM_DEFAULT)
	{
		if (*port)
			_prnFile = fopen(port, "w+b");

		if (!_prnFile)
			return FALSE;

		if (printerMode == PRM_LASER)
			fprintf(_prnFile, "\033E");
	}
	// Check for valid printerMode
	else if (!(printerMode == PRM_DOTMATRIX9 ||
			printerMode == PRM_DOTMATRIX24))
		return FALSE;

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
	else
	{
		if (printerMode == PRM_LASER)
			fprintf(_prnFile, "\033%%0A\033E");

		if (_prnFile)
			fclose(_prnFile);

	}
	_outputFile = ZIL_NULLP(FILE);
	_prnFile = ZIL_NULLP(FILE);
}

void UI_PRINTER::BeginPage(void)
{
	if (printerMode == PRM_POSTSCRIPT)
	{
		// Set up environment (8.5 x 11 portrait mode)
		if (isText)
			fprintf(_outputFile, "18 774 translate 1 -1 scale\n");
		else
		{
			float temp = 576.0 / columns;
			fprintf(_outputFile, "18 774 translate %f %f scale\n", temp, -temp);
		}
	}
	// Initialize print head position
	else if (printerMode == PRM_DOTMATRIX9 || printerMode == PRM_DOTMATRIX24)
	{
		currentPrintheadPositionX = 0;
		currentPrintheadPositionY = 0;
	}
	else
	{
		pageWidth = columns / 8;
		pageHeight = lines + (lines % 8);

		if (pageWidth % 8)
			pageWidth++;

		if (!isText)
		{
			// Set up page - 4 blocks, totalSize dividable by 8.
			_page.totalSize = (long)pageWidth * pageHeight;
			_page.blockSize = (int)(_page.totalSize / 4);
			_page.blockLines = pageHeight / 4;
		
			for (int i = 0; i < 4; i++)
			{
				_page.blocks[i] = new char[_page.blockSize];
				memset(_page.blocks[i], 0, _page.blockSize);
			}
		}
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
	// Initialize print head position
	else if (printerMode == PRM_DOTMATRIX9 || printerMode == PRM_DOTMATRIX24)
	{
		currentPrintheadPositionX = 0;
		currentPrintheadPositionY = 0;
	}
	else
		BeginPage();
}

void UI_PRINTER::EndPage(void)
{
	if (printerMode == PRM_POSTSCRIPT)
		fprintf(_outputFile, "showpage\n");
	else if (printerMode == PRM_LASER)
	{
		if (isText)
			fprintf(_prnFile, "\033%%0A\f");
		else
		{
			// Set resolution.
			if (columns > 640)
				fprintf(_prnFile, "\033*t150R");
			else
				fprintf(_prnFile, "\033*t100R");

			// Start graphics.
			fprintf(_prnFile, "\033*r0A");
			fprintf(_prnFile, "\033*p0Y");
 			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < _page.blockLines; j++)
				{
					fprintf(_prnFile, "\033*b%dW", pageWidth);
					for (int k = 0; k < pageWidth; k++)
						putc(_page.blocks[i][j*pageWidth+k], _prnFile);
				}
				delete _page.blocks[i];
			}
			// End Graphics.
			fprintf(_prnFile, "\033*rB");
		}
	}
	else if (printerMode == PRM_DOTMATRIX9 || printerMode == PRM_DOTMATRIX24)
	{
		// Form Feed to next page
		for (int i = currentPrintheadPositionY; i < dotMatrixLinesPerPage; i++)
			PrintDotMatrixString("\r\n");
		PrintDotMatrixString("\r");
		currentPrintheadPositionX = 0;
		currentPrintheadPositionY = 0;
	}
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

	if (BeginPrintJob(pMode, fileName))
	{
		int tWidth = width % 2 ? width + 1 : width;

		BeginPage();
		if (printerMode == PRM_POSTSCRIPT)
			fprintf(_outputFile, "%d %d %d %d Bitmap\n", x, y, tWidth, height);
		else
			height = MinValue(height, lines), width = MinValue(width, columns);

		int pixel = 0;
		ZIL_UINT8 color;
		int i;
		for (i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
#if defined(ZIL_MSDOS)
#	if defined(GFX)
				pixel = Point(x+j, y+i);
#	elif defined(BGI)
				pixel = getpixel(x+j, y+i);
#	elif defined(MSC) || defined(WCC)
				pixel = _getpixel(x+j, y+i);
#	elif defined(META)
				pixel = GetPixel(x+j, y+i);
#	endif
#endif
				if (printerMode == PRM_POSTSCRIPT)
				{
					color = _bmpColorTable[pixel % 16];
					fprintf(_outputFile, "%x", color);
				}
				else
				{
					pixel = MonoColor(pixel % 16);
					if (pixel)
						SetPoint(x + j, y + i, pixel);
				}
			}
			if (printerMode == PRM_POSTSCRIPT)
			{
				if (width % 2)
					fprintf(_outputFile, "f");
				fprintf(_outputFile, "\n");
			}
		}
		if (printerMode == PRM_POSTSCRIPT && height % 2)
		{
			for (i = 0; i < tWidth; i++)
				fprintf(_outputFile, "f");
			fprintf(_outputFile, "\n");
		}
		EndPage();
		EndPrintJob();
	}
}

void UI_PRINTER::TextFormat(ZIL_SCREENID, int x, int y, ZIL_ICHAR *text,
	const UI_PALETTE *palette, int width, ZIL_LOGICAL_FONT font)
{
	const int BUFFER_LENGTH = 256;
 	int row = y, lineWidth = 0;
	int printLine = 0, printPage = 0;

	if (width < 0 || width >= columns)
        if (printerMode == PRM_DOTMATRIX9 || printerMode == PRM_DOTMATRIX24)
            width = dotMatrixColumnsPerPage;
        else
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
				ZIL_ICHAR *space = NULL;
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

			if (printerMode == PRM_POSTSCRIPT)
				row += TextHeight(lineBuffer) + TextHeight(lineBuffer) / 3;
			else if (printerMode == PRM_LASER)
				row += TextHeight(lineBuffer) + 2;
			else
				row += cellHeight;

			lineWidth = printLine = 0;
			if (printerMode == PRM_DOTMATRIX9 || printerMode == PRM_DOTMATRIX24)
			{
				if (currentPrintheadPositionY > dotMatrixLinesPerPage - dotMatrixBottomMargin && *ptr)//
					printPage = TRUE;
			}
			else if (row > lines - 108 && *ptr)
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

int UI_PRINTER::GetDMByte(char *byte, int bytePos, int width)
{
	int b, newByte=0;
	byte += (bytePos / 8);
	for (int i = 7; i>>=0; --i)
	{
		b = *byte;
		if (b & (1<<(7-bytePos%8)))
			newByte |= (1 << i);
		byte += (width/8);
	}
	return newByte;
}

int UI_PRINTER::MonoColor(ZIL_COLOR color)
{
	if ((color < LIGHTGREEN && color != LIGHTGRAY) || color == LIGHTRED)
		return 1;
	return 0;
}


void UI_PRINTER::PrintLine(int x1, int y1, int x2, int y2, int color)
{
	// One pixel.
	if (x1 == x2 && y1 == y2)
	{
		SetPoint(x1, y1, color);
		return;
	}

	// Figure line points.
	int left = MinValue(x1, x2);
	int top = MinValue(y1, y2);
	int right = MaxValue(x1, x2);
	int bottom = MaxValue(y1, y2);
	int deltaX = right - left;
	int deltaY = bottom - top;
	int sign = (x2 > x1) ? (y2 > y1) : (y1 > y2);

	if (deltaX >= deltaY)
	{
		int adjust = deltaY ? deltaX / deltaY / 2 : 0;
		for (int x = 0; x <= deltaX; x++)
		{
			int y = (int)(deltaY * (long)(x + adjust) / deltaX);
			if (sign)
				SetPoint(left + x, top + y, color);
			else
				SetPoint(left + x, bottom - y, color);
		}
	}
	else
	{
		int adjust = deltaX ? deltaY / deltaX / 2 : 0;
		for (int y = 0; y <= deltaY; y++)
		{
			int x = (int)(deltaX * (long)(y + adjust) / deltaY);
			if (sign)
				SetPoint(left + x, top + y, color);
			else
				SetPoint(right - x, top + y, color);
		}
	}
}

void UI_PRINTER::SetBit(char &byteValue, int bitPos, int color)
{
	static int maskBits[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	if (color)
		byteValue |= maskBits[bitPos];
	else
		byteValue &= ~maskBits[bitPos];
}

void UI_PRINTER::SetPoint(int column, int line, int color)
{
	if (column < 0 || line < 0 || column >= columns || line >= lines)
		return;

	long bytePosition = (long)(column / 8 + line * (long)pageWidth);
	int index = (int)(bytePosition / _page.blockSize);
	int byteOffset = (int)(bytePosition % _page.blockSize);
	int bitPosition = column % 8;

	char byte = _page.blocks[index][byteOffset];
	SetBit(byte, bitPosition, color);
	_page.blocks[index][byteOffset] = byte;
}

void UI_PRINTER::PrintDotMatrixString(char *string)
{
 while (*string) I_PrintDotMatrixChar(*string++);
}

// ----- CURSES ------------------------------------------------------
#elif defined(ZIL_CURSES)
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

		for (int i = 0; i < bitmapHeight; i++)
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
				for (ZIL_ICHAR *space = ptr; *space != ' ' && *space != '-' &&
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
#endif
