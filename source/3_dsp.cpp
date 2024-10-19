//	Zinc Interface Library - W_DSP.CPP
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


#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#	include <mem.h>
#endif
#include "ui_dsp.hpp"
#if defined(ZIL_MSWINDOWS_CTL3D)
#	include <ctl3d.h>
#endif

static int _virtualCount = 0;
static UI_REGION _virtualRegion;

	static ZIL_ICHAR _operatingSystem[] = { 'W','i','n','N','T', 0 };
	static ZIL_ICHAR _windowingSystem[] = { 'W','i','n','d','o','w','s', 0 };

// ----- Static member variables --------------------------------------------

HDC UI_MSWINDOWS_DISPLAY::hDC = 0;
PAINTSTRUCT UI_MSWINDOWS_DISPLAY::paintStruct;

HFONT UI_MSWINDOWS_DISPLAY::fontTable[ZIL_MAXFONTS] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
WORD UI_MSWINDOWS_DISPLAY::patternTable[ZIL_MAXPATTERNS][8] =
{
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	// EMPTY_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// SOLID_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// LINE_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// LTSLASH_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// SLASH_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// BKSLASH_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// LTBKSLASH_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// HATCH_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// XHATCH_FILL
	{ 0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055 },	// INTERLEAVE_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// WIDE_DOT_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },	// CLOSE_DOT_FILL

	{ 0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055 },	// PTN_BACKGROUND_FILL
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF },
	{ 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF }
};

// ----- UI_MSWINDOWS_DISPLAY -----------------------------------------------

struct WIN_TO_MAP
{
	int value;
	ZIL_ICHAR name[20];
};

UI_MSWINDOWS_DISPLAY::UI_MSWINDOWS_DISPLAY(HINSTANCE _hInstance,
	HINSTANCE _hPrevInstance, int _nCmdShow) :
	UI_DISPLAY(FALSE, _operatingSystem, _windowingSystem), maxColors(16)
{
	hInstance = _hInstance;
	hPrevInstance = _hPrevInstance;
	nCmdShow = _nCmdShow;

#if defined(ZIL_MSWINDOWS_CTL3D)
	// Init CTL3DV2.DLL
	Ctl3dRegister(hInstance);
	Ctl3dAutoSubclass(hInstance);
#endif

	// Set up the initial logical fonts.
	HFONT systemFont = (HFONT)GetStockObject(SYSTEM_FONT);
	HFONT stockFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	LOGFONT systemInfo;
	GetObject(systemFont, sizeof(LOGFONT), (LPSTR)&systemInfo);
	LOGFONT stockInfo;
	GetObject(stockFont, sizeof(LOGFONT), (LPSTR)&stockInfo);

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

	if (!fontTable[FNT_DIALOG_FONT])
	{
		HFONT dialogFont = CreateFont(stockInfo.lfHeight,
			stockInfo.lfWidth, stockInfo.lfEscapement, stockInfo.lfOrientation,
			systemInfo.lfWeight, stockInfo.lfItalic, stockInfo.lfUnderline, stockInfo.lfStrikeOut,
			systemInfo.lfCharSet, stockInfo.lfOutPrecision, stockInfo.lfClipPrecision,
			stockInfo.lfQuality, systemInfo.lfPitchAndFamily, fStyle);
		fontTable[FNT_DIALOG_FONT] = dialogFont;
	}
	if (!fontTable[FNT_SMALL_FONT])
	{
#if (ZIL_MSWINDOWS >= 0x30a)
		LOGFONT iconInfo;
		SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &iconInfo, 0);
		HFONT smallFont = CreateFont(iconInfo.lfHeight,
			iconInfo.lfWidth, iconInfo.lfEscapement, iconInfo.lfOrientation,
			iconInfo.lfWeight, iconInfo.lfItalic, iconInfo.lfUnderline, iconInfo.lfStrikeOut,
			iconInfo.lfCharSet, iconInfo.lfOutPrecision, iconInfo.lfClipPrecision,
			iconInfo.lfQuality, iconInfo.lfPitchAndFamily, iconInfo.lfFaceName);
#else
		HFONT smallFont = CreateFont(stockInfo.lfHeight,
			stockInfo.lfWidth, stockInfo.lfEscapement, stockInfo.lfOrientation,
			FW_NORMAL, stockInfo.lfItalic, stockInfo.lfUnderline, stockInfo.lfStrikeOut,
			systemInfo.lfCharSet, stockInfo.lfOutPrecision, stockInfo.lfClipPrecision,
			stockInfo.lfQuality, systemInfo.lfPitchAndFamily, fStyle);
#endif
		fontTable[FNT_SMALL_FONT] = smallFont;
	}
	for (int i = FNT_SYSTEM_FONT; i < ZIL_MAXFONTS; i++)
		if (!fontTable[i])
			fontTable[i] = systemFont;

	// Compute the default display values.
	columns = GetSystemMetrics(SM_CXSCREEN);
	lines = GetSystemMetrics(SM_CYSCREEN);

	HWND desktop = GetDesktopWindow();
	HDC desktopDC = GetDC(desktop);
	HFONT oldFont = (HFONT)SelectObject(desktopDC, fontTable[FNT_DIALOG_FONT]);
	TEXTMETRIC textMetrics;
	GetTextMetrics(desktopDC, &textMetrics);
	SelectObject(desktopDC, oldFont);
	ReleaseDC(desktop, desktopDC);

	cellWidth = textMetrics.tmAveCharWidth + 1;
#if defined(ZIL_UNICODE)
	cellHeight =
		textMetrics.tmHeight +				// letter height
		-textMetrics.tmInternalLeading +	// blank space above and below letter
		textMetrics.tmExternalLeading +
		8 +									// border space
		preSpace +							// preSpace (before the border)
		postSpace;							// postSpace (after the border)
#else
	cellHeight =
		textMetrics.tmHeight +					// letter height
        (textMetrics.tmHeight / 2) +			
        ((textMetrics.tmHeight % 2) ? 1 : 0) +	
		preSpace +								// preSpace (before the border)
		postSpace;								// postSpace (after the border)
#endif

	if (!ZIL_INTERNATIONAL::defaultLocale)
		ZIL_INTERNATIONAL::DefaultI18nInitialize();	// Assume the path is set
	installed = TRUE;
}

UI_MSWINDOWS_DISPLAY::~UI_MSWINDOWS_DISPLAY(void)
{
	DeleteObject(fontTable[FNT_SMALL_FONT]);
	DeleteObject(fontTable[FNT_DIALOG_FONT]);

#if defined(ZIL_MSWINDOWS_CTL3D)
	Ctl3dUnregister(hInstance);
#endif
}

void UI_MSWINDOWS_DISPLAY::Bitmap(ZIL_SCREENID screenID, int left, int top,
	int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
	const UI_PALETTE *palette, const UI_REGION *clipRegion,
	ZIL_BITMAP_HANDLE *_colorBitmap, ZIL_BITMAP_HANDLE *_monoBitmap)
{
	// Make sure there is a valid bitmap.
	ZIL_BITMAP_HANDLE colorBitmap = 0, monoBitmap = 0;
	if (_colorBitmap)
		colorBitmap = *_colorBitmap;
	if (_monoBitmap)
		monoBitmap = *_monoBitmap;
	if (!bitmapArray && !colorBitmap)
		return;

	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, left, top, left + bitmapWidth - 1, top + bitmapHeight - 1);
	left -= _virtualRegion.left;
	top -= _virtualRegion.top;

	// Set the clipRegion. (Relative to hDC)
	HRGN cRegion = ZIL_NULLH(HRGN);
	if (clipRegion)
	{
		cRegion = CreateRectRgn(MaxValue(clipRegion->left - _virtualRegion.left, paintStruct.rcPaint.left),
			MaxValue(clipRegion->top - _virtualRegion.top, paintStruct.rcPaint.top),
			MinValue(clipRegion->right - _virtualRegion.left + 1, paintStruct.rcPaint.right),
			MinValue(clipRegion->bottom - _virtualRegion.top + 1, paintStruct.rcPaint.bottom));
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
		cRegion = CreateRectRgn(paintStruct.rcPaint.left, paintStruct.rcPaint.top,
			paintStruct.rcPaint.right, paintStruct.rcPaint.bottom);
	}

	// Convert the bitmap array then draw the bitmap.
	int oldROP2 = SetROP2(hDC, R2_COPYPEN);
	if (!colorBitmap)
		BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight, bitmapArray,
			palette, &colorBitmap, &monoBitmap);
	if (monoBitmap)
	{
		HDC hMemDC = CreateCompatibleDC(hDC);
		ZIL_COLOR oldBkColor = SetBkColor(hDC, 0x00FFFFFFL);
		ZIL_COLOR oldFgColor = SetTextColor(hDC, 0x00000000L);
		SelectObject(hMemDC, monoBitmap);
		BitBlt(hDC, left, top, bitmapWidth, bitmapHeight, hMemDC, 0, 0, SRCAND);
		SelectObject(hMemDC, colorBitmap);
		BitBlt(hDC, left, top, bitmapWidth, bitmapHeight, hMemDC, 0, 0, SRCINVERT);
		SetBkColor(hDC, oldBkColor);
		SetTextColor(hDC, oldFgColor);
		DeleteDC(hMemDC);
	}
	else if (colorBitmap)
	{
		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, colorBitmap);
		BitBlt(hDC, left, top, bitmapWidth, bitmapHeight, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
	}
	if (monoBitmap)
	{
		if (!_monoBitmap)
			DeleteObject(monoBitmap);
		else
			*_monoBitmap = monoBitmap;
	}
	if (colorBitmap)
	{
		if (!_colorBitmap)
			DeleteObject(colorBitmap);
		else
			*_colorBitmap = colorBitmap;
	}
	SetROP2(hDC, oldROP2);

	// Un-virtualize the display.
	if (cRegion)
	{
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
	}
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::BitmapArrayToHandle(ZIL_SCREENID ,
	int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
	const UI_PALETTE *palette, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
	HBITMAP hBitmap;
	HBITMAP hOldBitmap;
	HDC hDC = GetDC(0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	const ZIL_UINT8 *pixel;
	int column, row;

	if (colorBitmap)
	{
		// Create a bitmap compatable with the display.
		hBitmap = CreateCompatibleBitmap(hDC, bitmapWidth, bitmapHeight);

		// Select the bitmap into the memory DC.
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		if (bitmapArray)
		{
			// Draw the Zinc bitmap onto the memory DC.
			pixel = bitmapArray;
			for (row = 0; row < bitmapHeight; row++)
			{
				for (column = 0; column < bitmapWidth; column++)
				{
					if (palette)
						SetPixel(hMemDC, column, row, MapColor(&palette[*pixel], TRUE));
					else if (*pixel <= (ZIL_UINT8)MAX_COLORMAP_INDEX)
						SetPixel(hMemDC, column, row, MapColor(&colorMap[*pixel], TRUE));
					else
						SetPixel(hMemDC, column, row, RGB_BLACK);
					pixel++;
				}
			}
		}
		else
			// Create a white icon if no bitmap array was passed.
			BitBlt(hMemDC, 0, 0, bitmapWidth, bitmapHeight, 0, 0, 0, WHITENESS);

		// Get the bitmap.
		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
		*colorBitmap = hBitmap;
	}

	if (monoBitmap)
	{
		hBitmap = CreateBitmap(bitmapWidth, bitmapHeight, 1, 1, ZIL_NULLP(void));
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		BitBlt(hMemDC, 0, 0, bitmapWidth, bitmapHeight, 0, 0, 0, BLACKNESS);

		if (bitmapArray)
		{
			// Draw the Zinc bitmap onto the memory DC.
			pixel = bitmapArray;
			for (row = 0; row < bitmapHeight; row++)
			{
				for (column = 0; column < bitmapWidth; column++)
				{
					if (*pixel == BACKGROUND)
						SetPixel(hMemDC, column, row, RGB_WHITE);
					pixel++;
				}
			}
		}
		else
			// Create a white icon if no bitmap array was passed.
			BitBlt(hMemDC, 0, 0, bitmapWidth, bitmapHeight, 0, 0, 0, WHITENESS);

		// Get the bitmap.
		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
		*monoBitmap = hBitmap;
		
		BITMAP bitmap;
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	}
	
	// Clean up.
	DeleteDC(hMemDC);
	ReleaseDC(0, hDC);
}

void UI_MSWINDOWS_DISPLAY::BitmapHandleToArray(ZIL_SCREENID , ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray)
{
	BITMAP bitmap;
	ZIL_BITMAP_HANDLE hBitmap;
	ZIL_BITMAP_HANDLE hOldBitmap;
	HDC hDC = GetDC(0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	ZIL_UINT8 *pixel;
	int column, row;

	if (colorBitmap)
	{
		hBitmap = colorBitmap;
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);
		*bitmapWidth = bitmap.bmWidth;
		*bitmapHeight = bitmap.bmHeight;
		if (!(*bitmapArray))
			*bitmapArray = new ZIL_UINT8[bitmap.bmWidth * bitmap.bmHeight];

		pixel = *bitmapArray;
		for (row = 0; row < *bitmapHeight; row++)
			for (column = 0; column < *bitmapWidth; column++)
			{
				ZIL_COLOR color = GetPixel(hMemDC, column, row);
				*pixel = 0;

				for (ZIL_UINT8 i = 0; i <= MAX_COLORMAP_INDEX; i++)
					if (color == MapColor(&colorMap[i], TRUE))
					{
						*pixel = i;
						break;
					}
				pixel++;
			}

		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	}

	if (monoBitmap)
	{
		hBitmap = monoBitmap;
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		if (!(*bitmapArray))
			*bitmapArray = new ZIL_UINT8[bitmap.bmWidth * bitmap.bmHeight];

		pixel = *bitmapArray;
		for (row = 0; row < *bitmapHeight; row++)
			for (column = 0; column < *bitmapWidth; column++)
			{
				if (GetPixel(hMemDC, column, row) != RGB_BLACK)
					*pixel = (ZIL_UINT8)BACKGROUND;
				pixel++;
			}

		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	}
	
	// Clean up.
	ReleaseDC(0, hDC);
	DeleteDC(hMemDC);
}

void UI_MSWINDOWS_DISPLAY::DestroyBitmapHandle(ZIL_SCREENID, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
	if (colorBitmap && *colorBitmap)
	{
		DeleteObject(*colorBitmap);
		*colorBitmap = ZIL_NULLH(HBITMAP);
	}
	if (monoBitmap && *monoBitmap)
	{
		DeleteObject(*monoBitmap);
		*monoBitmap = ZIL_NULLH(HBITMAP);
	}
}

void UI_MSWINDOWS_DISPLAY::Ellipse(ZIL_SCREENID screenID, int x, int y,
	int startAngle, int endAngle, int xRadius, int yRadius,
	const UI_PALETTE *palette, int fill, int xorInt, const UI_REGION *clipRegion)
{
	const int pSin[] = { 0, 871, 1736, 2588, 3420, 4226, 5000, 5736, 6428,
		7071, 7660, 8191, 8660, 9063, 9397, 9659, 9848, 9962, 10000 };

	// Virtualize the display. (This call sets up hDC.)
	int left;
	int top;
	int right;
	int bottom;
	if (!_virtualCount)
	{
		// Find the bounds only if it will be used by VirtualGet.
		left = x - xRadius + 1;
		top = y - yRadius + 1;
		right = x + xRadius - 1;
		bottom = y + yRadius - 1;
	}
	VirtualGet(screenID, left, top, right, bottom);
	x -= _virtualRegion.left;
	y -= _virtualRegion.top;

	// Set the clipRegion. (Relative to hDC)
	HRGN cRegion = ZIL_NULLH(HRGN);
	if (clipRegion)
	{
		cRegion = CreateRectRgn(MaxValue(clipRegion->left - _virtualRegion.left, paintStruct.rcPaint.left),
			MaxValue(clipRegion->top - _virtualRegion.top, paintStruct.rcPaint.top),
			MinValue(clipRegion->right - _virtualRegion.left + 1, paintStruct.rcPaint.right),
			MinValue(clipRegion->bottom - _virtualRegion.top + 1, paintStruct.rcPaint.bottom));
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
		cRegion = CreateRectRgn(paintStruct.rcPaint.left, paintStruct.rcPaint.top,
			paintStruct.rcPaint.right, paintStruct.rcPaint.bottom);
	}

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

	int oldROP2 = SetROP2(hDC, (xorInt == TRUE) ? R2_XORPEN : R2_COPYPEN);
	HANDLE oldPen = SelectObject(hDC, CreatePen(PS_SOLID, 1, MapColor(palette, TRUE)));

	if (fill)
	{
		HBRUSH oldBrush;
		COLORREF oldForeground, oldBackground;

		// Set the fill pattern.
		WORD *pattern = (!palette || (palette->fillPattern & 0xFFF0)) ?
			patternTable[PTN_SOLID_FILL] : patternTable[palette->fillPattern];
		int i;
		for (i = 0; i < 8 && pattern[i] == 0xFF; i++)
			;
		int solid = i == 8;

		// Create the fill brush and select it.
		if (!solid)
		{
			ZIL_BITMAP_HANDLE hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR)pattern);
			oldBrush = (HBRUSH)SelectObject(hDC, CreatePatternBrush(hBitmap));
			DeleteObject(hBitmap);
			oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
			oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
		}
		else
			oldBrush = (HBRUSH)SelectObject(hDC,
				CreateSolidBrush(MapColor(palette, FALSE)));

		int oldMode = SetBkMode(hDC, OPAQUE);

		if (startAngle == 0 && endAngle == 360)
			::Ellipse(hDC, x - xRadius, y - yRadius, x + xRadius, y + yRadius);
		else
			Pie(hDC, x - xRadius, y - yRadius, x + xRadius, y + yRadius,
				startX - 1, startY - 1, endX - 1, endY - 1);

		SetBkMode(hDC, oldMode);
		DeleteObject(SelectObject(hDC, oldBrush));
		if (!solid)
		{
			SetBkColor(hDC, oldBackground);
			SetTextColor(hDC, oldForeground);
		}
	}
	else
		// Draw a line ellipse.
		Arc(hDC, x - xRadius, y - yRadius, x + xRadius, y + yRadius,
			startX, startY, endX, endY);
	SetROP2(hDC, oldROP2);

	DeleteObject(SelectObject(hDC, oldPen));

	// Un-virtualize the display.
	if (cRegion)
	{
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
	}
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
	int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
	ZIL_ICON_HANDLE *icon)
{
	ZIL_BITMAP_HANDLE colorBitmap = 0, monoBitmap = 0;
	BitmapArrayToHandle(screenID, iconWidth, iconHeight, iconArray,
		palette, &colorBitmap, &monoBitmap);
	extern void WindowsBitmapToIcon(UI_DISPLAY *display,
		ZIL_BITMAP_HANDLE colorBitmap, ZIL_BITMAP_HANDLE monoBitmap, ZIL_ICON_HANDLE *icon);
	WindowsBitmapToIcon(this, colorBitmap, monoBitmap, icon);
	DeleteObject(colorBitmap);
	DeleteObject(monoBitmap);
}

void UI_MSWINDOWS_DISPLAY::IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
	int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray)
{
	ZIL_BITMAP_HANDLE colorBitmap = 0, monoBitmap = 0;
	extern void WindowsIconToBitmap(UI_DISPLAY *display, ZIL_ICON_HANDLE icon,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	WindowsIconToBitmap(this, icon, &colorBitmap, &monoBitmap);
	BitmapHandleToArray(screenID, colorBitmap, monoBitmap, iconWidth,
		iconHeight, iconArray);
}

void UI_MSWINDOWS_DISPLAY::DestroyIconHandle(ZIL_SCREENID, ZIL_ICON_HANDLE *icon)
{
	if (icon && *icon)
		DestroyIcon(*icon);
	*icon = 0;
}

void UI_MSWINDOWS_DISPLAY::Line(ZIL_SCREENID screenID, int x1, int y1,
	int x2, int y2, const UI_PALETTE *palette, int width, int xorInt,
	const UI_REGION *clipRegion)
{
	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, MinValue(x1, x2), MinValue(y1, y2), MaxValue(x1, x2), MaxValue(y1, y2));
	x1 -= _virtualRegion.left;
	y1 -= _virtualRegion.top;
	x2 -= _virtualRegion.left;
	y2 -= _virtualRegion.top;

	// Set the clipRegion. (Relative to hDC)
	HRGN cRegion = ZIL_NULLH(HRGN);
	if (clipRegion)
	{
		cRegion = CreateRectRgn(MaxValue(clipRegion->left - _virtualRegion.left, paintStruct.rcPaint.left),
			MaxValue(clipRegion->top - _virtualRegion.top, paintStruct.rcPaint.top),
			MinValue(clipRegion->right - _virtualRegion.left + 1, paintStruct.rcPaint.right),
			MinValue(clipRegion->bottom - _virtualRegion.top + 1, paintStruct.rcPaint.bottom));
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
		cRegion = CreateRectRgn(paintStruct.rcPaint.left, paintStruct.rcPaint.top,
			paintStruct.rcPaint.right, paintStruct.rcPaint.bottom);
	}

	// Draw the line on the window.
	COLORREF color;
	HPEN foreground, oldForeground;
	int oldROP2 = SetROP2(hDC, xorInt ? R2_XORPEN : R2_COPYPEN);
	color = MapColor(palette, TRUE);
	foreground = CreatePen(PS_SOLID, width, color);
	oldForeground = (HPEN)SelectObject(hDC, foreground);

	MoveToEx(hDC, x1, y1, ZIL_NULLP(POINT));
	LineTo(hDC, x2, y2);

	SetPixel(hDC, x2, y2, color);
	DeleteObject(SelectObject(hDC, oldForeground));
	SetROP2(hDC, oldROP2);

	// Un-virtualize the display.
	if (cRegion)
	{
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
	}
	VirtualPut(screenID);
}

ZIL_COLOR UI_MSWINDOWS_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
	// Check for a valid palette.
	if (!palette)
	{
		if (foreground)
			return (GetSysColor(COLOR_WINDOWTEXT));
		return (GetSysColor(COLOR_WINDOW));
	}

	// Match the color request based on the ZIL_COLOR type in fillPattern.
	ZIL_COLOR color = foreground ? palette->colorForeground : palette->colorBackground;
	if (color == BACKGROUND || palette->fillPattern == PTN_RGB_COLOR)
		return(color);
	else if (palette->fillPattern == PTN_SYSTEM_COLOR)
		return (GetSysColor((int)color));
	else if (color <= MAX_COLORMAP_INDEX)
		return (colorMap[(int)color].colorForeground);
	else
		return (RGB_BLACK);
}

void UI_MSWINDOWS_DISPLAY::Polygon(ZIL_SCREENID screenID, int numPoints,
	const int *polygonPoints, const UI_PALETTE *palette, int fill, int xorInt,
	const UI_REGION *clipRegion)
{
	// Determine the maximum region that bounds the polygon.
	int left = 0xFFFF, top = 0xFFFF;
	int right = 0, bottom = 0;
	POINT *winPolygon = new POINT[numPoints];
	if (!_virtualCount)
	{
		// Find the bounds only if it will be used by VirtualGet.
		for (int i = 0; i < numPoints; i++)
		{
			int column = polygonPoints[i*2];
			int line = polygonPoints[i*2+1];
			if (column < left)
				left = column;
			if (column > right)
				right = column;
			if (line < top)
				top = line;
			if (line > bottom)
				bottom = line;
		}
	}

	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, left, top, right, bottom);

	// Set up the polygon points.
	for (int i = 0; i < numPoints; i++)
	{
		winPolygon[i].x = polygonPoints[i*2] - _virtualRegion.left;
		winPolygon[i].y = polygonPoints[i*2+1] - _virtualRegion.top;
	}

	// Set the clipRegion. (Relative to hDC)
	HRGN cRegion = ZIL_NULLH(HRGN);
	if (clipRegion)
	{
		cRegion = CreateRectRgn(MaxValue(clipRegion->left - _virtualRegion.left, paintStruct.rcPaint.left),
			MaxValue(clipRegion->top - _virtualRegion.top, paintStruct.rcPaint.top),
			MinValue(clipRegion->right - _virtualRegion.left + 1, paintStruct.rcPaint.right),
			MinValue(clipRegion->bottom - _virtualRegion.top + 1, paintStruct.rcPaint.bottom));
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
		cRegion = CreateRectRgn(paintStruct.rcPaint.left, paintStruct.rcPaint.top,
			paintStruct.rcPaint.right, paintStruct.rcPaint.bottom);
	}

	int oldROP2 = SetROP2(hDC, xorInt ? R2_XORPEN : R2_COPYPEN);
	HANDLE oldPen = SelectObject(hDC, CreatePen(PS_SOLID, 1, MapColor(palette, TRUE)));
	int oldFillMode = SetPolyFillMode(hDC, WINDING);

	if (fill)
	{
		HBRUSH oldBrush;
		COLORREF oldForeground, oldBackground;

		// Set the fill pattern.
		WORD *pattern = (!palette || (palette->fillPattern & 0xFFF0)) ?
			patternTable[PTN_SOLID_FILL] : patternTable[palette->fillPattern];
		int i;
		for (i = 0; i < 8 && pattern[i] == 0xFF; i++)
			;
		int solid = i == 8;

		// Create the fill brush and select it.
		if (!solid)
		{
			ZIL_BITMAP_HANDLE hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR)pattern);
			oldBrush = (HBRUSH)SelectObject(hDC, CreatePatternBrush(hBitmap));
			DeleteObject(hBitmap);
			oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
			oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
		}
		else
			oldBrush = (HBRUSH)SelectObject(hDC,
				CreateSolidBrush(MapColor(palette, FALSE)));

		int oldMode = SetBkMode(hDC, OPAQUE);

		// Draw an filled polygon.
		::Polygon(hDC, winPolygon, numPoints);

		SetBkMode(hDC, oldMode);
		DeleteObject(SelectObject(hDC, oldBrush));
		if (!solid)
		{
			SetBkColor(hDC, oldBackground);
			SetTextColor(hDC, oldForeground);
		}
	}
	else
		// Draw a line polygon.
		Polyline(hDC, winPolygon, numPoints);
	delete winPolygon;
	SetPolyFillMode(hDC, oldFillMode);
	DeleteObject(SelectObject(hDC, oldPen));
	SetROP2(hDC, oldROP2);

	// Un-virtualize the display.
	if (cRegion)
	{
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
	}
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top,
	int right, int bottom, const UI_PALETTE *palette, int width, int fill,
	int xorInt, const UI_REGION *clipRegion)
{
	// Don't draw negative regions.
	if (left > right || top > bottom)
		return;

	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, left, top, right, bottom);

	// Change coordinates to be relative to hDC.
	RECT rect;
	rect.left = left - _virtualRegion.left;
	rect.top = top - _virtualRegion.top;
	rect.right = right - _virtualRegion.left + 1;
	rect.bottom = bottom - _virtualRegion.top + 1;

	// Set the clipRegion. (Relative to hDC)
	HRGN cRegion = ZIL_NULLH(HRGN);
	if (clipRegion)
	{
		cRegion = CreateRectRgn(MaxValue(clipRegion->left - _virtualRegion.left, paintStruct.rcPaint.left),
			MaxValue(clipRegion->top - _virtualRegion.top, paintStruct.rcPaint.top),
			MinValue(clipRegion->right - _virtualRegion.left + 1, paintStruct.rcPaint.right),
			MinValue(clipRegion->bottom - _virtualRegion.top + 1, paintStruct.rcPaint.bottom));
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
		cRegion = CreateRectRgn(paintStruct.rcPaint.left, paintStruct.rcPaint.top,
			paintStruct.rcPaint.right, paintStruct.rcPaint.bottom);
	}

	// Draw the outline.
	if (width)
	{
		HBRUSH borderBrush = CreateSolidBrush(MapColor(palette, TRUE));
		for (int i = 0; i < width; i++)
		{
			if (xorInt)
			{
				COLORREF oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
				DrawFocusRect(hDC, &rect);
				SetBkColor(hDC, oldBackground);
			}
			else
				::FrameRect(hDC, &rect, borderBrush);
			rect.left++;
			rect.top++;
			rect.right--;
			rect.bottom--;
		}
		DeleteObject(borderBrush);
	}

	// Fill the rectangle.
	if (fill)
	{
		// Set the fill pattern.
		WORD *pattern = (!palette || (palette->fillPattern & 0xFFF0)) ?
			patternTable[PTN_SOLID_FILL] : patternTable[palette->fillPattern];
		int i;
		for (i = 0; i < 8 && pattern[i] == 0xFF; i++)
			;
		int solid = i == 8;

		// Create the fill brush.
		HBRUSH fillBrush;
		if (!solid)
		{
			ZIL_BITMAP_HANDLE hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR)pattern);
			fillBrush = CreatePatternBrush(hBitmap);
			DeleteObject(hBitmap);
		}
		else
			fillBrush = CreateSolidBrush(MapColor(palette, FALSE));

		if (xorInt)
		{
			// Write into memory DC, and then bitblt.
			HDC hDCBits = CreateCompatibleDC(hDC);
			if (!solid)
			{
				SetTextColor(hDCBits, MapColor(palette, TRUE));
				SetBkColor(hDCBits, MapColor(palette, FALSE));
			}
			RECT tRect = { 0, 0, rect.right - rect.left + 1, rect.bottom - rect.top + 1 };

			ZIL_BITMAP_HANDLE hRectBitmap = CreateCompatibleBitmap(hDC,
				rect.right - rect.left + 1, rect.bottom - rect.top + 1);
			HANDLE oldObject = SelectObject(hDCBits, hRectBitmap);

			FillRect(hDCBits, &tRect, fillBrush);

			BitBlt(hDC, rect.left, rect.top, rect.right - rect.left + 1,
				rect.bottom - rect.top + 1, hDCBits, 0, 0, SRCINVERT);
			SelectObject(hDCBits, oldObject);
	        DeleteObject(hRectBitmap);
			DeleteDC(hDCBits);
		}
		else
		{
			COLORREF oldForeground, oldBackground;
			if (!solid)
			{
				oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
				oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
			}

			FillRect(hDC, &rect, fillBrush);

			if (!solid)
			{
				SetBkColor(hDC, oldBackground);
				SetTextColor(hDC, oldForeground);
			}
		}
		DeleteObject(fillBrush);
	}

	// Un-virtualize the display.
	if (cRegion)
	{
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
	}
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
	const UI_REGION &newRegion, ZIL_SCREENID screenID, const UI_REGION *)
{
	RECT rect;
	GetClientRect(screenID, &rect);
	VirtualGet(screenID, rect.left, rect.top, rect.right, rect.bottom);

	// Change coordinates to be relative to hDC.
	rect.left = oldRegion.left - _virtualRegion.left;
	rect.top = oldRegion.top - _virtualRegion.top;
	rect.right = oldRegion.right - _virtualRegion.left + 1;
	rect.bottom = oldRegion.bottom - _virtualRegion.top + 1;

	DrawFocusRect(hDC, &rect);

	rect.left = newRegion.left - _virtualRegion.left;
	rect.top = newRegion.top - _virtualRegion.top;
	rect.right = newRegion.right - _virtualRegion.left + 1;
	rect.bottom = newRegion.bottom - _virtualRegion.top + 1;

	DrawFocusRect(hDC, &rect);

	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::RegionMove(const UI_REGION &oldRegion, int newColumn,
	int newLine, ZIL_SCREENID oldScreenID, ZIL_SCREENID newScreenID)
{
	// Get the device context.
	HDC _hDestDC = (newScreenID != ID_DIRECT) ? GetDC(newScreenID) : hDC;
	HDC _hSrcDC = (oldScreenID != ID_DIRECT) ? GetDC(oldScreenID) : hDC;

	BitBlt(_hDestDC, newColumn, newLine, oldRegion.right - oldRegion.left + 1,
		oldRegion.bottom - oldRegion.top + 1, _hSrcDC, oldRegion.left,
		oldRegion.top, SRCCOPY);

	if (newScreenID != ID_DIRECT)
		ReleaseDC(newScreenID, _hDestDC);
	if (oldScreenID != ID_DIRECT)
		ReleaseDC(oldScreenID, _hSrcDC);
}

void UI_MSWINDOWS_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
	const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
	int xorInt, const UI_REGION *clipRegion, ZIL_LOGICAL_FONT logicalFont)
{
	// Make sure there is a valid string.
	if (!text || text[0] == '\0')
		return;

	// Virtualize the display. (This call sets up hDC.)
	ZIL_ICHAR *string = strdup(text);
	if (length != -1 && length < strlen(string))
		string[length] = 0;
	int right = left + TextWidth(string, screenID, logicalFont) - 1;
	int bottom = top + TextHeight(string, screenID, logicalFont) - 1;
	VirtualGet(screenID, left, top, right, bottom);
	left -= _virtualRegion.left;
	top -= _virtualRegion.top;
	right -= _virtualRegion.left;
	bottom -= _virtualRegion.top;

	int underscore = !FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE);
	logicalFont &= 0x0FFF;

	// Set the clipRegion. (Relative to hDC)
	HRGN cRegion = ZIL_NULLH(HRGN);
	if (clipRegion)
	{
		cRegion = CreateRectRgn(MaxValue(clipRegion->left - _virtualRegion.left, paintStruct.rcPaint.left),
			MaxValue(clipRegion->top - _virtualRegion.top, paintStruct.rcPaint.top),
			MinValue(clipRegion->right - _virtualRegion.left + 1, paintStruct.rcPaint.right),
			MinValue(clipRegion->bottom - _virtualRegion.top + 1, paintStruct.rcPaint.bottom));
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
		cRegion = CreateRectRgn(paintStruct.rcPaint.left, paintStruct.rcPaint.top,
			paintStruct.rcPaint.right, paintStruct.rcPaint.bottom);
	}

	// Set up the colors, fill and font and draw the text.
	COLORREF oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
	COLORREF oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
	int oldMode = SetBkMode(hDC, TRANSPARENT);
	HFONT oldFont = (HFONT)SelectObject(hDC, fontTable[logicalFont]);

#	define TEXT_ string
	if (length < 0)
		length = strlen(string);
	if (xorInt)
	{
		HDC hDCBits = CreateCompatibleDC(hDC);
		SetTextColor(hDCBits, MapColor(palette, TRUE));
		SelectObject(hDCBits, fontTable[logicalFont]);
		RECT rect = { 0, 0, right - left + 1, bottom - top + 1 };

		ZIL_BITMAP_HANDLE hTextBitmap = CreateCompatibleBitmap(hDC,
			right - left + 1, bottom - top + 1);
		HANDLE oldObject = SelectObject(hDCBits, hTextBitmap);

		if (fill)
		{
			// Set the fill pattern.
			WORD *pattern = (!palette || (palette->fillPattern & 0xFFF0)) ?
				patternTable[PTN_SOLID_FILL] : patternTable[palette->fillPattern];
			int i;
			for (i = 0; i < 8 && pattern[i] == 0xFF; i++)
				;
			int solid = i == 8;

			// Create the fill brush.
			HBRUSH fillBrush;
			if (!solid)
			{
				ZIL_BITMAP_HANDLE hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR)pattern);
				fillBrush = CreatePatternBrush(hBitmap);
				DeleteObject(hBitmap);
			}
			else
				fillBrush = CreateSolidBrush(MapColor(palette, FALSE));

			if (!solid)
				SetBkColor(hDCBits, MapColor(palette, FALSE));

			FillRect(hDCBits, &rect, fillBrush);

			DeleteObject(fillBrush);
		}
		else
			BitBlt(hDCBits, 0, 0, right - left + 1, bottom - top + 1, 0, 0, 0, BLACKNESS);

		SetBkMode(hDCBits, TRANSPARENT);
		if (underscore && strchr(string, ZIL_HOTMARK))
		{
			WORD format = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
			::DrawText(hDCBits, TEXT_, length, &rect, format);
		}
		else
			TextOut(hDCBits, rect.left, rect.top, TEXT_, length);
		BitBlt(hDC, left, top, right - left + 1, bottom - top + 1,
			hDCBits, 0, 0, SRCINVERT);
		SelectObject(hDCBits, oldObject);
        DeleteObject(hTextBitmap);
		DeleteDC(hDCBits);
	}
	else
	{
		if (fill)
			Rectangle(screenID, left + _virtualRegion.left,
				top + _virtualRegion.top,
				left + _virtualRegion.left + TextWidth(string, screenID, logicalFont) - 1,
				top + _virtualRegion.top + TextHeight(string, screenID, logicalFont) - 1,
				palette, 0, TRUE, xorInt, clipRegion);
		RECT rect = { left, top, right + 1, bottom + 1 };
		if (underscore && strchr(string, ZIL_HOTMARK))
		{
			WORD format = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
			::DrawText(hDC, TEXT_, length, &rect, format);
		}
		else
			TextOut(hDC, rect.left, rect.top, TEXT_, length);
	}

	SetTextColor(hDC, oldForeground);
	SetBkColor(hDC, oldBackground);
	SetBkMode(hDC, oldMode);
	SelectObject(hDC, oldFont);

	// Un-virtualize the display.
	if (cRegion)
	{
		SelectClipRgn(hDC, cRegion);
		DeleteObject(cRegion);
	}

	delete string;
	VirtualPut(screenID);
}

int UI_MSWINDOWS_DISPLAY::TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID screenID,
	ZIL_LOGICAL_FONT logicalFont)
{
	// Make sure there is a valid string.
	if (!string || !string[0])
		return (cellHeight - preSpace - postSpace);

	logicalFont &= 0x0FFF;
	HDC _hDC;
	if (screenID != ID_SCREEN)
		_hDC = GetDC(screenID);
	else
	{
		HDC tHDC = GetDC(GetDesktopWindow());
		_hDC = CreateCompatibleDC(tHDC);
		ReleaseDC(GetDesktopWindow(), tHDC);
	}

	// Get the string height.
	HFONT oldFont = (HFONT)SelectObject(_hDC, fontTable[logicalFont]);
	SIZE size;
	GetTextExtentPoint(_hDC, (ZIL_ICHAR *)string, strlen(string), &size);
	int height = size.cy;
	SelectObject(_hDC, oldFont);

	if (screenID != ID_SCREEN)
		ReleaseDC(screenID, _hDC);
	else
		DeleteDC(_hDC);

	// Return the text height.
	return (height);
}

int UI_MSWINDOWS_DISPLAY::TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID screenID,
	ZIL_LOGICAL_FONT logicalFont)
{
	// Make sure there is a valid string.
	if (!string || !string[0])
		return (0);

	logicalFont &= 0x0FFF;
	HDC _hDC;
	if (screenID != ID_SCREEN)
		_hDC = GetDC(screenID);
	else
	{
		HDC tHDC = GetDC(GetDesktopWindow());
		_hDC = CreateCompatibleDC(tHDC);
		ReleaseDC(GetDesktopWindow(), tHDC);
	}

	ZIL_ICHAR hotMarkStr[] = { ZIL_HOTMARK, 0 };

	// Get the string width.
	HFONT oldFont = (HFONT)SelectObject(_hDC, fontTable[logicalFont]);
	SIZE size;
	GetTextExtentPoint(_hDC, (ZIL_ICHAR *)string, strlen(string), &size);
	int width = size.cx;
	GetTextExtentPoint(_hDC, (ZIL_ICHAR *)hotMarkStr, 1, &size);
	int markLen = size.cx;
	SelectObject(_hDC, oldFont);

	if (screenID != ID_SCREEN)
		ReleaseDC(screenID, _hDC);
	else
		DeleteDC(_hDC);

	for (int i = strlen(string); i >= 0; i--)
		if (string[i] == ZIL_HOTMARK)
		{
			i--;
			width -= markLen - 1;
		}


	// Return the text width.
	return (width);
}

int UI_MSWINDOWS_DISPLAY::VirtualGet(ZIL_SCREENID screenID, int left, int top,
	int right, int bottom)
{
	if (--_virtualCount == -1)
	{
		if (screenID != ID_DIRECT)
			hDC = GetDC(screenID);
		GetClipBox(hDC, &paintStruct.rcPaint);
		_virtualRegion.left = left;
		_virtualRegion.top = top;
		_virtualRegion.right = right;
		_virtualRegion.bottom = bottom;
	}
	return (TRUE);
}

int UI_MSWINDOWS_DISPLAY::VirtualPut(ZIL_SCREENID screenID)
{
	if (++_virtualCount == 0)
	{
		if (screenID != ID_DIRECT && hDC)
			ReleaseDC(screenID, hDC);
		hDC = 0;
	}
	return (TRUE);
}

void WindowsIconToBitmap(UI_DISPLAY *, ZIL_ICON_HANDLE icon,
	ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
	ZIL_BITMAP_HANDLE hBitmap;
	ZIL_BITMAP_HANDLE hOldBitmap;
	ZIL_BITMAP_HANDLE hANDBitmap;
	ZIL_BITMAP_HANDLE hOldANDBitmap;
	HDC hDC = GetDC(0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HDC hANDMemDC = CreateCompatibleDC(hDC);
	int iconWidth = GetSystemMetrics(SM_CXICON);
	int iconHeight = GetSystemMetrics(SM_CYICON);

	if (colorBitmap)
	{
		// Select a bitmap of icon size into the memory DC.
		hBitmap = CreateCompatibleBitmap(hDC, iconWidth, iconHeight);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		// Draw the icon on to the bitmap.
		DrawIcon(hMemDC, 0, 0, icon);

		// Get the bitmap.
		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
		*colorBitmap = hBitmap;
	}

	if (monoBitmap)
	{
		// Select a bitmap of icon size into the memory DC.
		hBitmap = CreateBitmap(iconWidth, iconHeight, 1, 1, 0);
		hANDBitmap = CreateBitmap(iconWidth, iconHeight, 1, 1, 0);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		hOldANDBitmap = (HBITMAP)SelectObject(hANDMemDC, hANDBitmap);

		// Draw the icon on to the bitmap.
		BitBlt(hMemDC, 0, 0, iconWidth, iconHeight, 0, 0, 0, BLACKNESS);
		DrawIcon(hMemDC, 0, 0, icon);
		BitBlt(hANDMemDC, 0, 0, iconWidth, iconHeight, 0, 0, 0, WHITENESS);
		DrawIcon(hANDMemDC, 0, 0, icon);
		BitBlt(hMemDC, 0, 0, iconWidth, iconHeight, hANDMemDC, 0, 0, SRCERASE);

		// Get the bitmap.
		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
		hANDBitmap = (HBITMAP)SelectObject(hMemDC, hOldANDBitmap);
		*monoBitmap = hBitmap;

		BITMAP bitmap;
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);
		BitBlt(hDC, 0, 0, iconWidth, iconHeight, hMemDC, iconWidth, iconHeight, SRCCOPY);
	}

	// Clean up.
	DeleteDC(hMemDC);
	DeleteDC(hANDMemDC);
	ReleaseDC(0, hDC);
}

void WindowsBitmapToIcon(UI_DISPLAY *display, ZIL_BITMAP_HANDLE colorBitmap,
	ZIL_BITMAP_HANDLE monoBitmap, ZIL_ICON_HANDLE *icon)
{
	if (!colorBitmap)
		return;

	BITMAP colorInfo;
	BITMAP monoInfo;

	// Find size of the icon.
	GetObject(colorBitmap, sizeof(BITMAP), (LPSTR)&colorInfo);
	DWORD size = colorInfo.bmWidth * colorInfo.bmHeight * colorInfo.bmPlanes * colorInfo.bmBitsPixel;
	ZIL_UINT8 *colorBits = new ZIL_UINT8[(int)size + 1];
	ZIL_UINT8 *monoBits = new ZIL_UINT8[(int)size + 1];

	// Get the color bits.
	size = GetBitmapBits(colorBitmap, size, (LPSTR)colorBits);

	// If monochrome bitmap then use bits, else set bits to black.
	if (monoBitmap)
		GetObject(monoBitmap, sizeof(BITMAP), (LPSTR)&monoInfo);
	if (!monoBitmap || monoInfo.bmPlanes != 1 || monoInfo.bmBitsPixel != 1)
		memset(monoBits, 0, (size_t)size);
	else
		size = GetBitmapBits(monoBitmap, size, (LPSTR)monoBits);

	// Create the icon.
	*icon = CreateIcon(display->hInstance, colorInfo.bmWidth, colorInfo.bmHeight,
		(BYTE)colorInfo.bmPlanes, (BYTE)colorInfo.bmBitsPixel, monoBits, colorBits);

	// Clean up.
	delete colorBits;
	delete monoBits;
}


