//	W_PRINT.CPP - Printer display class.
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
#include "ui_dsp.hpp"

static FILE *_outputFile;
static WORD patternTable[16][8] =
{
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	// BLACK
	{ 0x0000, 0x0001, 0x0000, 0x0010, 0x0000, 0x0001, 0x0000, 0x0010 },	// BLUE
	{ 0x00C0, 0x0000, 0x000C, 0x0000, 0x00C0, 0x0000, 0x000C, 0x0000 },	// GREEN
	{ 0x0000, 0x00C0, 0x0000, 0x000C, 0x0000, 0x00C0, 0x0000, 0x000C },	// CYAN
	{ 0x00AA, 0x0000, 0x0055, 0x0000, 0x00AA, 0x0000, 0x0055, 0x0000 },	// RED
	{ 0x000F, 0x00FF, 0x00F0, 0x00FF, 0x000F, 0x00FF, 0x00F0, 0x00FF },	// BROWN
	{ 0x00E7, 0x00FF, 0x007E, 0x00FF, 0x00E7, 0x00FF, 0x007E, 0x00FF },	// LIGHTCYAN
	{ 0x00DD, 0x0077, 0x00DD, 0x0077, 0x00DD, 0x0077, 0x00DD, 0x0077 },	// LIGHTGREEN
	{ 0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055 },	// GREEN
	{ 0x00EF, 0x0000, 0x00FE, 0x0000, 0x00EF, 0x0000, 0x00FE, 0x0000 },	// LIGHTBLUE
	{ 0x00DD, 0x0077, 0x00DD, 0x0077, 0x00DD, 0x0077, 0x00DD, 0x0077 },	// LIGHTGREEN
	{ 0x00E7, 0x00FF, 0x007E, 0x00FF, 0x00E7, 0x00FF, 0x007E, 0x00FF },	// LIGHTCYAN
	{ 0x00FF, 0x00FF, 0x00FE, 0x00FF, 0x00FF, 0x00EF, 0x00FF, 0x00FF },	// LIGHTRED
	{ 0x00E7, 0x00FF, 0x007E, 0x00FF, 0x00E7, 0x00FF, 0x007E, 0x00FF },	// LIGHTMAGENTA
	{ 0x00FF, 0x00FF, 0x00FE, 0x00FF, 0x00FF, 0x00EF, 0x00FF, 0x00FF },	// YELLOW
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// WHITE

};


struct WIN_TO_MAP
{
	int value;
	ZIL_ICHAR name[20];
};

UI_PRINTER::UI_PRINTER(UI_DISPLAY *_display) : UI_DISPLAY(TRUE),
	display(_display)
{
	ZIL_ICHAR printerString[80];
	ZIL_ICHAR *device, *driver, *port;
	isText = installed = FALSE;

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
	ZIL_ICHAR windowsString[] = { 'w','i','n','d','o','w','s', 0 };
	ZIL_ICHAR deviceString[] = { 'd','e','v','i','c','e', 0 };
	ZIL_ICHAR commasString[] = { ',',',',',', 0 };

	GetProfileString(windowsString, deviceString, commasString, printerString, 80);

	ZIL_ICHAR commaString[] = { ',', 0 };
	device = strtok(printerString, commaString);
	driver = strtok(0, commaString);
	port = strtok(0, commaString);
	if (device && driver && port)
	{
		hdc = CreateDC(driver, device, port, 0);
		if (hdc)
		{
			installed = TRUE;
			int xResolution = GetDeviceCaps(hdc, HORZRES);
			SetMapMode(hdc, MM_ISOTROPIC);
			SetWindowExtEx(hdc, columns, columns, ZIL_NULLP(SIZE));
			SetViewportExtEx(hdc, xResolution, xResolution, ZIL_NULLP(SIZE));
		}
	}

	HGDIOBJ systemFont = GetStockObject(SYSTEM_FONT);
	HGDIOBJ stockFont = GetStockObject(DEVICE_DEFAULT_FONT);
	LOGFONT systemInfo;
	GetObject(systemFont, sizeof(LOGFONT), (LPSTR)&systemInfo);
	LOGFONT stockInfo;
	GetObject(stockFont, sizeof(LOGFONT), (LPSTR)&stockInfo);
	if (hdc)
	{
		TEXTMETRIC tm;
		HANDLE tFont = SelectObject(hdc, stockFont);
		GetTextMetrics(hdc, &tm);
		SelectObject(hdc, tFont);
		stockInfo.lfHeight = tm.tmHeight;
		stockInfo.lfWidth = tm.tmAveCharWidth;
		stockInfo.lfPitchAndFamily = tm.tmPitchAndFamily;
	}

	ZIL_ICHAR *fStyle = stockInfo.lfFaceName;
	if (systemInfo.lfCharSet >= SHIFTJIS_CHARSET)
		fStyle = systemInfo.lfFaceName;
#if defined(ZIL_UNICODE) 
	ZIL_ICHAR _codeSet[] = { 'U','N','I','C','O','D','E', 0 };
	strcpy(codeSet, _codeSet);
#else
	static WIN_TO_MAP winToMap[] =
	{
		{ ANSI_CHARSET,        { 'I','S','O','8','8','5','9','-','1',0 } },
		{ 3,                   { 'I','B','M','_','1','2','5','2',0 } },
		{ SHIFTJIS_CHARSET,    { 'I','B','M','_','9','3','2',0 } },
		{ HANGEUL_CHARSET,     { 'I','B','M','_','9','4','9',0 } },
		// Simplified Chinese
		{ 134,                 { 'I','B','M','_','1','3','8','1',0 } },
		{ CHINESEBIG5_CHARSET, { 'I','B','M','_','9','5','0',0 } },
		// Hebrew
		{ 177,                 { 'I','B','M','_','1','2','5','5',0 } },
		// Arabic
		{ 178,                 { 'I','B','M','_','1','2','5','6',0 } },
		// Traditional Arabic
		{ 179,                 { 'I','B','M','_','1','2','5','6',0 } },
		// Andulas??
		{ 180,                 { 'I','B','M','_','1','2','5','4',0 } },
		// Cyrillic
		{ 204,                 { 'I','B','M','_','1','2','5','1',0 } },
		// Central Europe
		{ 238,                 { 'I','B','M','_','1','2','5','0',0 } },
		{ -1,                  { 'A','S','C','I','I', 0 } }
	};
	int k;
	for (k=0; winToMap[k].value != -1; k++)
		if (systemInfo.lfCharSet == winToMap[k].value)
			break;
	strcpy(codeSet, winToMap[k].name);
#endif

	BYTE pitchAndFamily = (stockInfo.lfPitchAndFamily & 0xF0) |
		(systemInfo.lfPitchAndFamily & 0x0F);

	if (!fontTable[FNT_SYSTEM_FONT])
	{
		HFONT systemFont = CreateFont(stockInfo.lfHeight,
			0, stockInfo.lfEscapement, stockInfo.lfOrientation,
			FW_HEAVY, stockInfo.lfItalic, stockInfo.lfUnderline, stockInfo.lfStrikeOut,
			systemInfo.lfCharSet, stockInfo.lfOutPrecision, stockInfo.lfClipPrecision,
			stockInfo.lfQuality, pitchAndFamily, fStyle);
		fontTable[FNT_SYSTEM_FONT] = systemFont;
	}
	if (!fontTable[FNT_DIALOG_FONT])
	{
		HFONT dialogFont = CreateFont(stockInfo.lfHeight,
			0, stockInfo.lfEscapement, stockInfo.lfOrientation,
			FW_NORMAL, stockInfo.lfItalic, stockInfo.lfUnderline, stockInfo.lfStrikeOut,
			systemInfo.lfCharSet, stockInfo.lfOutPrecision, stockInfo.lfClipPrecision,
			stockInfo.lfQuality, pitchAndFamily, fStyle);
		fontTable[FNT_DIALOG_FONT] = dialogFont;
	}
	if (!fontTable[FNT_SMALL_FONT])
	{
		HFONT smallFont = CreateFont(stockInfo.lfHeight * 9 / 10,
			0, stockInfo.lfEscapement, stockInfo.lfOrientation,
			FW_NORMAL, stockInfo.lfItalic, stockInfo.lfUnderline, stockInfo.lfStrikeOut,
			systemInfo.lfCharSet, stockInfo.lfOutPrecision, stockInfo.lfClipPrecision,
			stockInfo.lfQuality, pitchAndFamily, fStyle);
		fontTable[FNT_SMALL_FONT] = smallFont;
	}

	for (int i = 0; i < ZIL_MAXFONTS; i++)
		if (!fontTable[i])
			fontTable[i] = (HFONT)GetStockObject(DEVICE_DEFAULT_FONT);
}

UI_PRINTER::~UI_PRINTER(void)
{
	if (installed)
		DeleteDC(hdc);
	DeleteObject(fontTable[FNT_SYSTEM_FONT]);
	fontTable[FNT_SYSTEM_FONT] = 0;
	DeleteObject(fontTable[FNT_DIALOG_FONT]);
	fontTable[FNT_DIALOG_FONT] = 0;
	DeleteObject(fontTable[FNT_SMALL_FONT]);
	fontTable[FNT_SMALL_FONT] = 0;
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
		SetROP2(hdc, R2_COPYPEN);
		UI_PALETTE tPalette;
		ZIL_UINT8 pixel;

		ZIL_BITMAP_HANDLE hBitmap =
			CreateBitmap(bitmapWidth, bitmapHeight, 1, 1, ZIL_NULLP(void));
		HDC hMemDC = CreateCompatibleDC(hdc);
		SelectObject(hMemDC, hBitmap);
		for (int i = 0; i < bitmapHeight; i++)
			for (int j = 0; j < bitmapWidth; j++)
			{
				pixel = bitmapArray[i*bitmapWidth+j];
				tPalette.colorForeground = pixel;
				SetPixel(hMemDC, j, i, MapColor(&tPalette, TRUE));
			}
		BitBlt(hdc, x, y, bitmapWidth, bitmapHeight, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
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
	 	fprintf(_outputFile, "%d %d %d %d %d %d %s %d %s 1 Ellipse\n",
			startAngle, endAngle, xRadius, yRadius, x, y,
			psColorTable[bColor], fill, psColorTable[fColor]);
	}
	else
	{
		const int pSin[] = { 0, 871, 1736, 2588, 3420, 4226, 5000, 5736, 6428,
			7071, 7660, 8191, 8660, 9063, 9397, 9659, 9848, 9962, 10000 };

		while (startAngle < 0)
			startAngle += 360, endAngle += 360;
		if (startAngle > 360)
			startAngle %= 360;
		while (endAngle < 0)
			endAngle += 360;
		if (endAngle > 360)
			endAngle %= 360;

		// Compute points to start and stop.
		int startX = 0;
		int startY = 0;
		int endX = 0;
		int endY = 0;
		if (startAngle <= 90)
		{
			startX = x + (int)(1L * pSin[(90 - startAngle) / 5] * xRadius / 10000L);
			startY = y - (int)(1L * pSin[startAngle / 5] * yRadius / 10000L);
		}
		else if (startAngle <= 180)
		{
			startX = x - (int)(1L * pSin[(startAngle - 90) / 5] * xRadius / 10000L);
			startY = y - (int)(1L * pSin[(180 - startAngle) / 5] * yRadius / 10000L);
		}
		else if (startAngle <= 270)
		{
			startX = x - (int)(1L * pSin[(270 - startAngle) / 5] * xRadius / 10000L);
			startY = y + (int)(1L * pSin[(startAngle - 180) / 5] * yRadius / 10000L);
		}
		else if (startAngle <= 360)
		{
			startX = x + (int)(1L * pSin[(startAngle - 270) / 5] * xRadius / 10000L);
			startY = y + (int)(1L * pSin[(360 - startAngle) / 5] * yRadius / 10000L);
		}
	
		if (endAngle <= 90)
		{
			endX = x + (int)(1L * pSin[(90 - endAngle) / 5] * xRadius / 10000L);
			endY = y - (int)(1L * pSin[endAngle / 5] * yRadius / 10000L);
		}
		else if (endAngle <= 180)
		{
			endX = x - (int)(1L * pSin[(endAngle - 90) / 5] * xRadius / 10000L);
			endY = y - (int)(1L * pSin[(180 - endAngle) / 5] * yRadius / 10000L);
		}
		else if (endAngle <= 270)
		{
			endX = x - (int)(1L * pSin[(270 - endAngle) / 5] * xRadius / 10000L);
			endY = y + (int)(1L * pSin[(endAngle - 180) / 5] * yRadius / 10000L);
		}
		else if (endAngle <= 360)
		{
			endX = x + (int)(1L * pSin[(endAngle - 270) / 5] * xRadius / 10000L);
			endY = y + (int)(1L * pSin[(360 - endAngle) / 5] * yRadius / 10000L);
		}

		HPEN foreground = CreatePen(PS_SOLID, 1, MapColor(palette, TRUE));
		HGDIOBJ oldPen = SelectObject(hdc, foreground);
		SelectObject(hdc, foreground);

		if (fill)
		{
			WORD *pattern = patternTable[bColor];
			ZIL_BITMAP_HANDLE hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR)pattern);
			HBRUSH fillBrush = CreatePatternBrush(hBitmap);
			HGDIOBJ oldBrush = SelectObject(hdc, fillBrush);

			if (startAngle == 0 && endAngle == 360)
				::Ellipse(hdc, x - xRadius, y - yRadius, x + xRadius, y + yRadius);
			else
				Pie(hdc, x - xRadius, y - yRadius, x + xRadius, y + yRadius,
				startX - 1, startY - 1, endX - 1, endY - 1);
			SelectObject(hdc, oldBrush);
			DeleteObject(fillBrush);
			DeleteObject(hBitmap);
		}
		else
			// Draw a line ellipse.
			Arc(hdc, x - xRadius, y - yRadius, x + xRadius, y + yRadius,
			startX, startY, endX, endY);

		SelectObject(hdc, oldPen);
		DeleteObject(foreground);
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
	{
		HPEN foreground = CreatePen(PS_SOLID, width, MapColor(palette, TRUE));
		HGDIOBJ oldPen = SelectObject(hdc, foreground);
		MoveToEx(hdc, x1, y1, ZIL_NULLP(POINT));
		LineTo(hdc, x2, y2);
		SelectObject(hdc, oldPen);
		DeleteObject(foreground);
	}
}


ZIL_COLOR UI_PRINTER::MapColor(const UI_PALETTE *palette, int foreground)
{
	ZIL_COLOR color = foreground ? palette->colorForeground : palette->colorBackground;
	switch(color)
	{

	case WHITE:
	case YELLOW:
	case LIGHTGRAY:
	case LIGHTCYAN:
	case LIGHTRED:
	case LIGHTMAGENTA:
	case LIGHTBLUE:
	case LIGHTGREEN:
	case BACKGROUND:
		color = RGB_WHITE;
		break;

	default:
		color = RGB_BLACK;
	}
	return color;
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

		POINT *winPolygon = new POINT[numPoints];

		// Set up the polygon points.
		for (int i = 0; i < numPoints; i++)
		{
			winPolygon[i].x = polygonPoints[i*2];
			winPolygon[i].y = polygonPoints[i*2+1];
		}

		HPEN foreground = CreatePen(PS_SOLID, 1, MapColor(palette, TRUE));
		HGDIOBJ oldPen = SelectObject(hdc, foreground);
		SetPolyFillMode(hdc, WINDING);

		if (fill)
		{

			WORD *pattern = patternTable[bColor];
			ZIL_BITMAP_HANDLE hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR)pattern);

			HBRUSH fillBrush = CreatePatternBrush(hBitmap);
			HGDIOBJ oldBrush = SelectObject(hdc, fillBrush);
			::Polygon(hdc, winPolygon, numPoints);

			SelectObject(hdc, oldBrush);
			DeleteObject(hBitmap);
			DeleteObject(fillBrush);
		}
		else
		{
			Polyline(hdc, winPolygon, numPoints);
		}

		SelectObject(hdc, oldPen);
		DeleteObject(foreground);

		delete winPolygon;
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
		RECT rect;
		SetRect(&rect, x1, y1, x2, y2);
		if (fill)
		{
			WORD *pattern = patternTable[bColor];
			HBRUSH fillBrush;
			ZIL_BITMAP_HANDLE hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR)pattern);
			fillBrush = CreatePatternBrush(hBitmap);
			DeleteObject(hBitmap);
			FillRect(hdc, &rect, fillBrush);
			DeleteObject(fillBrush);
		}

		if (width)
		{
			HBRUSH frameBrush = CreateSolidBrush(MapColor(palette, TRUE));
			for (int i = 0; i < width; i++)
			{
				FrameRect(hdc, &rect, frameBrush);
				rect.left++;
				rect.top++;
				rect.right--;
				rect.bottom--;
			}
			DeleteObject(frameBrush);
		}
	}
}

void UI_PRINTER::Text(ZIL_SCREENID, int x, int y, const ZIL_ICHAR *text,
	const UI_PALETTE *palette, int length, int, int, const UI_REGION *, ZIL_LOGICAL_FONT font)
{
	static ZIL_ICHAR ZIL_FARDATA fillLine[256];
	if (length < 0)
		length = strlen(text);
	if (length > 255)
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
	else
	{
		SetTextColor(hdc, MapColor(palette, TRUE));
		SetBkColor(hdc, MapColor(palette, FALSE));
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, fontTable[font]);

#		define TEXT1_ text
		if (length < 0)
			length = strlen(text);
		TextOut(hdc, x, y, TEXT1_, length);
	}
}

int UI_PRINTER::TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID, ZIL_LOGICAL_FONT font)
{
	int height;

	if (printerMode == PRM_POSTSCRIPT)
		height = psFontTable[font].pointSize;
	else
	{
		SelectObject(hdc, fontTable[font]);
		SIZE size;
		GetTextExtentPoint(hdc, (ZIL_ICHAR *)string, strlen(string), &size);
		height = size.cy;
	}
	return (height);
}

int UI_PRINTER::TextWidth(const ZIL_ICHAR *string,	ZIL_SCREENID, ZIL_LOGICAL_FONT font)
{
	int width;

	if (printerMode == PRM_POSTSCRIPT)
		width = strlen(string) * 8;
	else
	{
		ZIL_ICHAR hotMarkStr[] = { ZIL_HOTMARK, 0 };
		SelectObject(hdc, fontTable[font]);
		SIZE size;
		GetTextExtentPoint(hdc, (ZIL_ICHAR *)string, strlen(string), &size);
		width = size.cx;
		GetTextExtentPoint(hdc, (ZIL_ICHAR *)hotMarkStr, 1, &size);
		int markLen = size.cx;

		for (int i = strlen(string); i >= 0; i--)
			if (string[i] == ZIL_HOTMARK)
		{
			i--;
			width -= markLen - 1;
		}
	}
	return (width);
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
	printerMode = printMode;
	char tempFile[12];

	if (!fileName && printMode == PRM_POSTSCRIPT)
	{
		::strcpy(tempFile, "print.ps");
		fileName = tempFile;
	}

	if (fileName)
	{
		if (printerMode == PRM_POSTSCRIPT)
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
	else
	{
		if (!installed)
			return FALSE;

		static char message[] = "Printing..";
		Escape(hdc, STARTDOC, sizeof(message) - 1, message, 0);
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
	else
		Escape(hdc, ENDDOC, 0, 0, 0);
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
	else
	{
		int xResolution = GetDeviceCaps(hdc, HORZRES);
		SetMapMode(hdc, MM_ISOTROPIC);
		SetWindowExtEx(hdc, columns, columns, ZIL_NULLP(SIZE));
		SetViewportExtEx(hdc, xResolution, xResolution, ZIL_NULLP(SIZE));
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
	if (printerMode == PRM_POSTSCRIPT)
		fprintf(_outputFile, "showpage\n");
	else
		Escape(hdc, NEWFRAME, 0, 0, 0);
}

void UI_PRINTER::ScreenDump(ZIL_SCREENID, UI_REGION &region,
	ZIL_PRINTER_MODE pMode, char *fileName)
	
{
	if (!display || isText)
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

	DWORD pixel;
	ZIL_ICHAR displayString[] = { 'D','I','S','P','L','A','Y', 0 };
	HDC screenDC = CreateDC(displayString, 0, 0, 0);

	if (printerMode == PRM_POSTSCRIPT)
	{
		int tWidth = width % 2 ? width + 1 : width;
		ZIL_COLOR color;

		fprintf(_outputFile, "%d %d %d %d Bitmap\n", x, y, tWidth, height);
		int i;
		for (i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				// Need to get pixel value.
				pixel = GetPixel(screenDC, x+j, y+i);
				switch(pixel)
				{
					case RGB_BLACK:
						color = BLACK; break;
					case RGB_BLUE:
						color = BLUE; break;
					case RGB_GREEN:
						color = GREEN; break;
					case RGB_CYAN:
						color = CYAN; break;
					case RGB_RED:
						color = RED; break;
					case RGB_MAGENTA:
						color = MAGENTA; break;
					case RGB_BROWN:
						color = BROWN; break;
					case RGB_LIGHTGRAY:
						color = LIGHTGRAY; break;
					case RGB_DARKGRAY:
						color = DARKGRAY; break;
					case RGB_LIGHTBLUE:
						color = LIGHTBLUE; break;
					case RGB_LIGHTGREEN:
						color = LIGHTGREEN; break;
					case RGB_LIGHTCYAN:
						color = LIGHTCYAN; break;
					case RGB_LIGHTRED:
						color = LIGHTRED; break;
					case RGB_LIGHTMAGENTA:
						color = LIGHTMAGENTA; break;
					case RGB_YELLOW:
						color = YELLOW; break;
					case RGB_WHITE:
						color = WHITE; break;
					default:
						color = WHITE; break;
				}
				fprintf(_outputFile, "%x", _bmpColorTable[(int)color]);
			}
			if (width % 2)
				fprintf(_outputFile, "f");
			fprintf(_outputFile, "\n");
		}
		if (height % 2)
		{
			for (i = 0; i < tWidth; i++)
				fprintf(_outputFile, "f");
			fprintf(_outputFile, "\n");
		}
	}
	else
	{
		SetROP2(hdc, R2_COPYPEN);

		ZIL_BITMAP_HANDLE hBitmap =
			CreateBitmap(width, height, 1, 1, ZIL_NULLP(void));
		HDC hMemDC = CreateCompatibleDC(hdc);
		SelectObject(hMemDC, hBitmap);

		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
			{
				// Need to get pixel value.
				pixel = GetPixel(screenDC, x+j, y+i);
				SetPixel(hMemDC, j, i, pixel);
			}
		BitBlt(hdc, x, y, width, height, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
  		DeleteDC(screenDC);
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

			if (printerMode == PRM_POSTSCRIPT)
				row += TextHeight(lineBuffer) + TextHeight(lineBuffer) / 3;
			else
				row += TextHeight(lineBuffer) + 2;

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

