//	Zinc Interface Library - BGIDSP.CPP
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


#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include "ui_dsp.hpp"

#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
#	define SmallFont  ISOSmallFont
#	define DialogFont ISODialogFont
#	define SystemFont ISOSystemFont
#else
#	define SmallFont  OEMSmallFont
#	define DialogFont OEMDialogFont
#	define SystemFont OEMSystemFont
#endif

extern "C"
{
	extern void SmallFont(void);
	extern void DialogFont(void);
	extern void SystemFont(void);
}

// ----- Static member variables --------------------------------------------

UI_PATH *UI_BGI_DISPLAY::searchPath = ZIL_NULLP(UI_PATH);

#if __BORLANDC__ >= 0x0300
UI_BGI_DISPLAY::BGIFONT UI_BGI_DISPLAY::fontTable[ZIL_MAXFONTS] =
#else
BGIFONT UI_BGI_DISPLAY::fontTable[ZIL_MAXFONTS] =
#endif
{
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },	// FNT_SMALL_FONT
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },	// FNT_DIALOG_FONT
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },	// FNT_SYSTEM_FONT
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 },
	{ DEFAULT_FONT, 1, 1, 1, 1, 1, 8, 8 }
};

#if __BORLANDC__ >= 0x0300
UI_BGI_DISPLAY::BGIPATTERN UI_BGI_DISPLAY::patternTable[ZIL_MAXPATTERNS] =
#else
BGIPATTERN UI_BGI_DISPLAY::patternTable[ZIL_MAXPATTERNS] =
#endif
{
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// EMPTY_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// SOLID_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LINE_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LTSLASH_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// SLASH_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// BKSLASH_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LTBKSLASH_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// HATCH_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// XHATCH_FILL
	{ 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },	// INTERLEAVE_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// WIDE_DOT_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// CLOSE_DOT_FILL

	{ 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },	// PTN_BACKGROUND_FILL
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

// ----- UI_BGI_DISPLAY -----------------------------------------------------

static ZIL_ICHAR _operatingSystem[] = { 'D','O','S',0 };
static ZIL_ICHAR _windowingSystem[] = { 'B','G','I',0 };
static ZIL_ICHAR _Mstr[] = { 'M', 0 };
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
static ZIL_ICHAR _codeSet[] = { 'I','S','O','8','8','5','9','-','1',0 };
#else
static ZIL_ICHAR _codeSet[] = { 'I','B','M','_','4','3','7',0 };
#endif

UI_BGI_DISPLAY::UI_BGI_DISPLAY(int driver, int mode) :
	UI_DISPLAY(FALSE, _operatingSystem, _windowingSystem)
{
	strcpy(codeSet, _codeSet);

	// Initialize the basic member variables
	_virtualCount = 0;
	_stopDevice = FALSE;

	// Find the type of display and initialize the driver.

	if (driver == DETECT || driver == 256)
		detectgraph(&driver, &mode);
	int tDriver, tMode;

	// Use temporary path if not installed in main().
	int pathInstalled = searchPath ? TRUE : FALSE;
	if (!pathInstalled)
		searchPath = new UI_PATH;
	UI_PATH_ELEMENT *pathElement = searchPath->Last();
	do
	{
		tDriver = driver;
		tMode = mode;
#if defined(ZIL_UNICODE)
		char *TEXT_ = MapText(pathElement->pathName, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ pathElement->pathName
#endif
		initgraph(&tDriver, &tMode, TEXT_);
		pathElement = pathElement->Previous();
	} while (tDriver == -3 && pathElement);
	if (tDriver < 0)
		return;
	driver = tDriver;
	mode = tMode;

	// Delete path if it was installed temporarily.
	if (!pathInstalled && searchPath)
	{
		delete searchPath;
		searchPath = ZIL_NULLP(UI_PATH);
	}

	// Initialize additional member variables.
	columns = getmaxx() + 1;
	lines = getmaxy() + 1;
	maxColors = getmaxcolor() + 1;
	isMono = (maxColors == 2);

	// Reset some palette colors if in >= VGA mode.
	if (maxColors >= 15 && columns >= 639 && lines >= 479)
	{
		setrgbpalette(LIGHTGRAY, 50, 50, 50); 	// A lighter lightgray.
#if defined(DOS32PP)
	  	setrgbpalette(DARKGRAY, 35, 35, 35);	// A lighter darkgray.
		setrgbpalette(YELLOW, 63, 63, 50);		// A lighter yellow.
#else
	  	setrgbpalette(56, 35, 35, 35);			// A lighter darkgray.
		setrgbpalette(62, 63, 63, 50);			// A lighter yellow.
#endif
	}

	// Register the system, dialog, and small fonts that were linked in.
	BGIFONT BGIFont = { 0, 0, 1, 1, 1, 1, 0, 0 };

#if defined(DOS32PP)
	// Initialize the small font.
	BGIFont.font = installuserfont("iso_smal.fnt");
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 10;
		BGIFont.maxHeight = 11;
		UI_BGI_DISPLAY::fontTable[FNT_SMALL_FONT] = BGIFont;
	}

	// Initialize the dialog font.
	BGIFont.font = installuserfont("iso_dial.fnt");
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 11;
		BGIFont.maxHeight = 11;
		UI_BGI_DISPLAY::fontTable[FNT_DIALOG_FONT] = BGIFont;
	}

	// Initialize the system font.
	BGIFont.font = installuserfont("iso_syst.fnt");
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 11;
		BGIFont.maxHeight = 13;
		UI_BGI_DISPLAY::fontTable[FNT_SYSTEM_FONT] = BGIFont;
	}
#else
	// Initialize the small font.
	BGIFont.font = registerfarbgifont(SmallFont);
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 10;
		BGIFont.maxHeight = 11;
		UI_BGI_DISPLAY::fontTable[FNT_SMALL_FONT] = BGIFont;
	}

	// Initialize the dialog font.
	BGIFont.font = registerfarbgifont(DialogFont);
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 11;
		BGIFont.maxHeight = 11;
		UI_BGI_DISPLAY::fontTable[FNT_DIALOG_FONT] = BGIFont;
	}

	// Initialize the system font.
	BGIFont.font = registerfarbgifont(SystemFont);
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 11;
		BGIFont.maxHeight = 13;
		UI_BGI_DISPLAY::fontTable[FNT_SYSTEM_FONT] = BGIFont;
	}
#endif

	// Reset the font information.
	SetFont(FNT_DIALOG_FONT);

	// Determine the cell coordinates based on the dialog font.
	cellWidth = (fontTable[FNT_DIALOG_FONT].font == DEFAULT_FONT) ?
		TextWidth(_Mstr, ID_SCREEN, FNT_DIALOG_FONT) : 	// Bitmap font.
		TextWidth(_Mstr, ID_SCREEN, FNT_DIALOG_FONT) - 2; // Stroked font.
	cellHeight = TextHeight(ZIL_NULLP(ZIL_ICHAR), ID_SCREEN, FNT_DIALOG_FONT) +
		preSpace + postSpace + 4 + 4;  // 4 above the text, 4 below the text.

	// Fill the screen according to the specified palette.
	SetPattern(backgroundPalette, FALSE);
	setviewport(0, 0, columns - 1, lines - 1, TRUE);
	bar(0, 0, columns - 1, lines - 1);

	// Define the screen display region.
	Add(ZIL_NULLP(UI_ELEMENT), new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
	installed = TRUE;
}

UI_BGI_DISPLAY::~UI_BGI_DISPLAY(void)
{
	// Restore the display.
	SetFont(FNT_NONE);
	if (installed)
		closegraph();
}

void UI_BGI_DISPLAY::Bitmap(ZIL_SCREENID screenID, int column, int line,
	int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
	const UI_PALETTE *palette, const UI_REGION *clipRegion, ZIL_BITMAP_HANDLE *,
	ZIL_BITMAP_HANDLE *)
{
	// Assign the bitmap to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, column, line,
		column + bitmapWidth - 1, line + bitmapHeight - 1))
		return;
	if (!palette)
		palette = colorMap;

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
			setviewport(0, 0, columns - 1, lines - 1, TRUE);
			for (int y = tRegion.top; y <= tRegion.bottom; y++)
			{
				const ZIL_UINT8 *pixel = bitmapArray + (y - line) * bitmapWidth + tRegion.left - column;
				for (int x = tRegion.left; x <= tRegion.right; x++, pixel++)
					if (*pixel != BACKGROUND)
						putpixel(x, y, MapColor(palette+*pixel, TRUE));
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::Ellipse(ZIL_SCREENID screenID, int x, int y,
	int startAngle, int endAngle, int xRadius, int yRadius,
	const UI_PALETTE *palette, int fill, int xorInt, const UI_REGION *clipRegion)
{
	// Assign the ellipse to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, x - xRadius, y - yRadius,
		x + xRadius, y + yRadius))
		return;
	while (startAngle < 0)
		startAngle += 360, endAngle += 360;
	if (startAngle > 360)
		startAngle %= 360;
	while (endAngle < 0)
		endAngle += 360;
	if (endAngle > 360)
		endAngle %= 360;

	// Draw the ellipse on the display.
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xorInt);
			}
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			if (fill && startAngle == 0 && endAngle == 360)
				fillellipse(x - tRegion.left, y - tRegion.top, xRadius, yRadius);
			else if (fill)
				sector(x - tRegion.left, y - tRegion.top, startAngle, endAngle, xRadius, yRadius);
			else
				ellipse(x - tRegion.left, y - tRegion.top, startAngle, endAngle, xRadius, yRadius);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

int UI_BGI_DISPLAY::DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine)
{
	static int i, x, y, oldX, newX, oldY, newY;
	static int width, height, overlap, deltaX, deltaY;
	static UI_DISPLAY_IMAGE *view;
	static UI_REGION newRegion, virtualRegion;
	static ZIL_UINT8 *image, *screen, *backup, *tScreen;

	// Make sure the image type is valid.
	view = &displayImage[imageType];
	if (!view->image || _stopDevice || _virtualCount != 0 ||
		(view->region.left == newColumn && view->region.top == newLine))
		return (FALSE);
	_stopDevice = TRUE;

	width = view->region.right - view->region.left + 1;
	height = view->region.bottom - view->region.top + 1;

	overlap = FALSE;
	deltaX = newColumn - view->region.left;
	deltaY = newLine - view->region.top;

	newRegion.left = view->region.left + deltaX;
	newRegion.right = view->region.right + deltaX;
	newRegion.top = view->region.top + deltaY;
	newRegion.bottom = view->region.bottom + deltaY;

	for (i = 0; i < ZIL_MAXIMAGES && !overlap; i++)
		overlap = (i != imageType &&
			   (view->region.Overlap(displayImage[i].region) ||
			    newRegion.Overlap(displayImage[i].region)));

	if (overlap)
	{
		virtualRegion.left = MinValue(view->region.left, newRegion.left);
		virtualRegion.top = MinValue(view->region.top, newRegion.top);
		virtualRegion.right = MaxValue(view->region.right, newRegion.right);
		virtualRegion.bottom = MaxValue(view->region.bottom, newRegion.bottom);

		VirtualGet(0, virtualRegion.left, virtualRegion.top,
			   virtualRegion.right, virtualRegion.bottom);

		view->region.left += deltaX;
		view->region.right += deltaX;
		view->region.top += deltaY;
		view->region.bottom += deltaY;

		VirtualPut(0);
	}
	else
	{
		// Compute the update coordinates.
		setviewport(0, 0, columns - 1, lines - 1, TRUE);
		image = view->image;
		screen = view->screen;
		backup = view->backup;

		oldY = view->region.top;
		newY = newLine;
		for (y = 0; y < height; y++, oldY++, newY++)
		{
			tScreen = &screen[deltaY * width + deltaX];
			oldX = view->region.left;
			newX = newColumn;
			for (x = 0; x < width; x++, oldX++, newX++, image++, screen++, backup++, tScreen++)
			{
				if (*image == BACKGROUND)
					*backup = BACKGROUND;
				else if (*image != BACKGROUND &&
					 x + deltaX >= 0 && x + deltaX < width &&
					 y + deltaY >= 0 && y + deltaY < height &&
					 *tScreen != BACKGROUND)
				{
					*backup = *tScreen;
					*tScreen = BACKGROUND;
				}
				else
					*backup = getpixel(newX, newY);
				if (*image != BACKGROUND)
					putpixel(newX, newY, *image);
			}
		}
		screen = view->screen;
		for (y = view->region.top; y <= view->region.bottom; y++)
			for (x = view->region.left; x <= view->region.right; x++, screen++)
				if (*screen != BACKGROUND)
					putpixel(x, y, *screen);
		memcpy(view->screen, view->backup, width * height);
		view->region.left = newColumn;
		view->region.top = newLine;
		view->region.right = newColumn + width - 1;
		view->region.bottom = newLine + height - 1;
	}

	_stopDevice = FALSE;
	return (TRUE);
}

int UI_BGI_DISPLAY::DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line,
	int width, int height, ZIL_UINT8 *image)
{
	if (_stopDevice)
		return (FALSE);
	_stopDevice = TRUE;

	UI_DISPLAY_IMAGE *view = &displayImage[imageType];
	UI_REGION virtualRegion;
	if (view->image && view->region.left < columns && view->region.top < lines)
	{
		virtualRegion.left = MinValue(view->region.left, column);
		virtualRegion.top = MinValue(view->region.top, line);
		virtualRegion.right = MaxValue(view->region.right, column + width - 1);
		virtualRegion.bottom = MaxValue(view->region.bottom, line + height - 1);
	}
	else
	{
		virtualRegion.left = column;
		virtualRegion.top = line;
		virtualRegion.right = column + width - 1;
		virtualRegion.bottom = line + height - 1;
	}

	VirtualGet(0, virtualRegion.left, virtualRegion.top, virtualRegion.right,
		virtualRegion.bottom);

	view->image = image;
	view->region.left = column;
	view->region.top = line;
	view->region.right = column + width - 1;
	view->region.bottom = line + height - 1;
	if (view->image)
	{
		if (view->screen)
		{
			delete view->screen;
			delete view->backup;
		}
		view->screen = new ZIL_UINT8[width * height];
		view->backup = new ZIL_UINT8[width * height];
		memset(view->screen, BACKGROUND, width * height);
	}

	VirtualPut(0);
	_stopDevice = FALSE;
	return (TRUE);
}

void UI_BGI_DISPLAY::Line(ZIL_SCREENID screenID, int x1, int y1,
	int x2, int y2, const UI_PALETTE *palette, int width, int xorInt,
	const UI_REGION *clipRegion)
{
	// Assign the line to a region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, MinValue(x1, x2), MinValue(y1, y2),
		MaxValue(x1, x2) + width, MaxValue(y1, y2) + width))
		return;

	// Draw the line on the display.
	int changedScreen = FALSE;
	int slope;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xorInt);
				slope = x1 == x2 ? 1 : (y1 - y2) / (x1 - x2);
			}
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			for (int i = 0; i < width; i++)
				if (slope)
					line(x1 - tRegion.left + i, y1 - tRegion.top,
						x2 - tRegion.left + i, y2 - tRegion.top);
				else
					line(x1 - tRegion.left, y1 - tRegion.top + i,
						x2 - tRegion.left, y2 - tRegion.top + i);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

ZIL_COLOR UI_BGI_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
	// Match the color request based on the type of display.
	if (maxColors == 2)
		return (foreground ? palette->bwForeground : palette->bwBackground);
	else if (maxColors < 16)
		return (foreground ? palette->grayScaleForeground : palette->grayScaleBackground);
	return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_BGI_DISPLAY::Polygon(ZIL_SCREENID screenID, int numPoints,
	const int *polygonPoints, const UI_PALETTE *palette, int fill, int xorInt,
	const UI_REGION *clipRegion)
{
	// Assign the polygon to the region structure.
	int left = 0x0FFF, top = 0x0FFF, right = 0, bottom = 0;
	int i;
	for (i = 0; i < numPoints * 2; i += 2)
	{
		left = MinValue(left, polygonPoints[i]);
		top = MinValue(top, polygonPoints[i+1]);
		right = MaxValue(right, polygonPoints[i]);
		bottom = MaxValue(bottom, polygonPoints[i+1]);
	}
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;
	
	// Draw the polygon on the display.
	int changedScreen = FALSE;
	int *tPolygon = new int[numPoints * 2];
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xorInt);
			}
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			for (i = 0; i < numPoints * 2; i += 2)
			{
				tPolygon[i] = polygonPoints[i] - tRegion.left;
				tPolygon[i+1] = polygonPoints[i+1] - tRegion.top;
			}
			if (fill)
				fillpoly(numPoints, tPolygon);
			else
				drawpoly(numPoints, tPolygon);
			if (screenID == ID_DIRECT)
				break;
		}
	delete tPolygon;

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top,
	int right, int bottom, const UI_PALETTE *palette, int width, int fill,
	int xorInt, const UI_REGION *clipRegion)
{
	// Assign the rectangle to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xorInt);
			}
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			if (fill && xorInt)			// Patch for Borland bar() xorInt bug.
			{
				for (int i = 0; i < tRegion.right - tRegion.left; i++)
					line(i, top - tRegion.top, i, bottom - tRegion.top);
			}
			else if (fill)
				bar(left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);
			else if (width < 0)
			{
				// Draws a dashed rounded rectangle to indicate focus
				setlinestyle(USERBIT_LINE, 0x5555, NORM_WIDTH);
				line(left-tRegion.left+1, top-tRegion.top, right-tRegion.left-1, top-tRegion.top);
				line(right-tRegion.left, top-tRegion.top+1, right-tRegion.left, bottom-tRegion.top-1);
				line(left-tRegion.left+1, bottom-tRegion.top, right-tRegion.left-1, bottom-tRegion.top);
				line(left-tRegion.left, top-tRegion.top+1, left-tRegion.left, bottom-tRegion.top-1);
				setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
			}
			for (int i = 0; i < width; i++)
				rectangle(left - (tRegion.left - i), top - (tRegion.top - i),
					right - (tRegion.left + i), bottom - (tRegion.top + i));
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
	const UI_REGION &newRegion, ZIL_SCREENID, const UI_REGION *clipRegion)
{
	// See if the regions match.
	if (oldRegion.left == newRegion.left && oldRegion.top == newRegion.top &&
		oldRegion.right == newRegion.right && oldRegion.bottom == newRegion.bottom)
		return;

	// Hide the screen devices.
	UI_REGION region;
	if (!RegionInitialize(region, ZIL_NULLP(UI_REGION),
		MinValue(oldRegion.left, newRegion.left), MinValue(oldRegion.top, newRegion.top),
		MaxValue(oldRegion.right, newRegion.right), MaxValue(oldRegion.bottom, newRegion.bottom)))
		return;
	VirtualGet(ID_DIRECT, region.left, region.top, region.right, region.bottom);

	// Draw the XOR rectangles on the display.
	SetPattern(xorPalette, TRUE);
	if (clipRegion)
	{
		setviewport(clipRegion->left, clipRegion->top, clipRegion->right, clipRegion->bottom, TRUE);
		rectangle(oldRegion.left - clipRegion->left, oldRegion.top - clipRegion->top,
			oldRegion.right - clipRegion->left, oldRegion.bottom - clipRegion->top);
		rectangle(newRegion.left - clipRegion->left, newRegion.top - clipRegion->top,
			newRegion.right - clipRegion->left, newRegion.bottom - clipRegion->top);
	}
	else
	{
		setviewport(0, 0, columns - 1, lines - 1, TRUE);
		rectangle(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
		rectangle(newRegion.left, newRegion.top, newRegion.right, newRegion.bottom);
	}

	// Update the screen.
	VirtualPut(ID_DIRECT);
}

void UI_BGI_DISPLAY::RegionDefine(ZIL_SCREENID screenID, int left, int top,
	int right, int bottom)
{
	// See if it is a full screen definition.
	UI_REGION region = { left, top, right, bottom };
	if (region.left <= 0 && region.top <= 0 && region.right >= columns - 1 && region.bottom >= lines - 1)
	{
		UI_REGION_LIST::Destroy();
		Add(0, new UI_REGION_ELEMENT(screenID, 0, 0, columns - 1, lines - 1));
		return;
	}

	// Throw away regions with a negative width or height.
	if (region.right < region.left || region.bottom < region.top)
		return;
	
	// Throw away regions that are completely off the screen.
	if (region.left >= columns || region.right < 0 ||
		region.top >= lines || region.bottom < 0)
		return;

	// Clip regions partially off the screen to fit on the screen.
	if (region.left < 0)
		region.left = 0;
	if (region.right >= columns)
		region.right = columns - 1;
	if (region.top < 0)
		region.top = 0;
	if (region.bottom >= lines)
		region.bottom = lines - 1;

	// Split any overlapping regions.
	Split(screenID, region, FALSE);

	// Define the new display region.
	UI_REGION_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region));
}

void UI_BGI_DISPLAY::RegionMove(const UI_REGION &oldRegion,
	int newColumn, int newLine, ZIL_SCREENID oldScreenID, ZIL_SCREENID)
{
	VirtualGet(oldScreenID, MinValue(oldRegion.left, newColumn), MinValue(oldRegion.top, newLine),
		MaxValue(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
		MaxValue(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top)));

	void far *buffer = ZIL_NULLP(void);
	setviewport(0, 0, columns - 1, lines - 1, TRUE);
	unsigned size = imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
	if (size && (size != 0xFFFF) && (buffer = new char[size]) != ZIL_NULLP(void))
	{
		getimage(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom, buffer);
		putimage(newColumn, newLine, buffer, COPY_PUT);
	}
	else if (oldRegion.top < newLine)
	{
		size = imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
		buffer = new char[size];
		int height = oldRegion.bottom - oldRegion.top;
		for (int i = height; i >= 0; i--)
			if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
			{
				getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
				putimage(newColumn, newLine + i, buffer, COPY_PUT);
			}
	}
	else
	{
		size = imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
		buffer = new char[size];
		int height = oldRegion.bottom - oldRegion.top;
		for (int i = 0; i <= height; i++)
			if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
			{
				getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
				putimage(newColumn, newLine + i, buffer, COPY_PUT);
			}
	}
	delete buffer;

	VirtualPut(oldScreenID);
}

void UI_BGI_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
	const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
	int xorInt, const UI_REGION *clipRegion, ZIL_LOGICAL_FONT logicalFont)
{
	// Make sure we have a valid string.
	if (!text || !text[0] || !palette)
		return;

	// Set up the fill line.
	static ZIL_ICHAR fillLine[256];
	if (length < 0)
		length = strlen(text);
	if (length > 255)
		length = 255;
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';

	// Determine the hot-key values and shrink the string to remove '&'.
	ZIL_ICHAR *hotKey = ZIL_NULLP(ZIL_ICHAR);
	int hotKeyLeft, hotKeyRight;

	if (!FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE))
	{
		ZIL_ICHAR hotKeyBuf[2];
		ZIL_ICHAR *hotChar = fillLine;
		for (int i=0; i < length; i++, hotChar++)
		{
			if ((*hotChar == ZIL_HOTMARK && hotChar[1] == ZIL_HOTMARK) ||
				(*hotChar == '~' && hotChar[1] == '~'))
				memmove(hotChar, hotChar + 1, strlen(hotChar) * sizeof(ZIL_ICHAR));
			else if (*hotChar == ZIL_HOTMARK || *hotChar == '~')
			{
				hotKeyBuf[0] = hotChar[1];
				hotKeyBuf[1] =	*hotChar = '\0';
				hotKeyLeft = left + TextWidth(fillLine, screenID, logicalFont | FNT_IGNORE_UNDERSCORE);
				memmove(hotChar, hotChar + 1, (strlen(hotChar + 1) + 1) * sizeof(ZIL_ICHAR));
				hotKey = hotChar;
				int markLen = TextWidth(hotKeyBuf, screenID, logicalFont | FNT_IGNORE_UNDERSCORE);
				hotKeyRight = hotKeyLeft + markLen - 1;
			}
		}
	}

	// Check for special characters.
	strrepc(fillLine, '\t', ' ');
	strrepc(fillLine, '\r', ' ');
	strrepc(fillLine, '\n', ' ');

	// Assign the rectangle to the region structure.
	SetFont(logicalFont);
	UI_REGION region, tRegion;
	int width = TextWidth(fillLine, screenID, logicalFont | FNT_IGNORE_UNDERSCORE);
	int height = TextHeight(fillLine, screenID, logicalFont);
	int right = left + width;
	int bottom = top + height;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xorInt);
			}
			setviewport(tRegion.left, tRegion.top, tRegion.right - 1, tRegion.bottom, TRUE);
			if (fill)
				bar(0, 0, tRegion.right - tRegion.left, tRegion.bottom - tRegion.top);

#if defined(ZIL_UNICODE)
			char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
#else
#			undef TEXT_
#			define TEXT_ fillLine
#endif
#if defined(DOS32PP)
			// Temporary fix for PowerPack 32 bit bug.
			outtextxy(left - tRegion.left, top + 2 - tRegion.top, TEXT_);
#else
			outtextxy(left - tRegion.left, top - tRegion.top, TEXT_);
#endif
			if (hotKey)
				line(hotKeyLeft - tRegion.left, bottom - tRegion.top,
					hotKeyRight - tRegion.left, bottom - tRegion.top);
			if (screenID == ID_DIRECT)
				break;

		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::SetFont(ZIL_LOGICAL_FONT logicalFont)
{
	static ZIL_LOGICAL_FONT _logicalFont = FNT_NONE;
	static BGIFONT _fontInfo;
	BGIFONT fontInfo;

	// Check for invalid or new font set.
	if (logicalFont != FNT_NONE && logicalFont != _logicalFont)
	{
		fontInfo = fontTable[logicalFont & 0x0FFF];
#	if defined(DOS32PP)
		settextstyle(fontInfo.font, HORIZ_DIR, 0);
#	else
		if (fontInfo.font != _fontInfo.font ||
			fontInfo.charSize != _fontInfo.charSize)
		{
			settextstyle(fontInfo.font, HORIZ_DIR, fontInfo.charSize);
			if (!fontInfo.charSize)
				setusercharsize(fontInfo.multX, fontInfo.divX, fontInfo.multY, fontInfo.divY);
		}
		else if (fontInfo.multX != _fontInfo.multX || fontInfo.divX != _fontInfo.divX ||
			fontInfo.multY != _fontInfo.multY || fontInfo.divY != _fontInfo.divY)
			setusercharsize(fontInfo.multX, fontInfo.divX, fontInfo.multY, fontInfo.divY);
#	endif
		_fontInfo = fontInfo;
	}
	_logicalFont = logicalFont;
}

void UI_BGI_DISPLAY::SetPattern(const UI_PALETTE *palette, int xorInt)
{
#if defined (DOS32PP)
	int fillPattern = palette->fillPattern;
	int backgroundColor = MapColor(palette, FALSE);
	int foregroundColor = MapColor(palette, TRUE);

	// PowerPack 32 bug - pattern and fill always need to be reset.
	if (maxColors == 2)
		setfillstyle(fillPattern, backgroundColor);
	else
		setfillpattern(patternTable[fillPattern], backgroundColor);

	setcolor(foregroundColor);
	setwritemode(xorInt ? XOR_PUT : COPY_PUT);
#else
	static int _fillPattern = -1;
	static int _backgroundColor = -1;
	static int _foregroundColor = -1;
	static int _xorInt = -1;
	int fillPattern = palette->fillPattern;
	int backgroundColor = MapColor(palette, FALSE);
	int foregroundColor = MapColor(palette, TRUE);

	if (fillPattern != _fillPattern || backgroundColor != _backgroundColor)
	{
		if (maxColors == 2)
			setfillstyle(fillPattern, backgroundColor);
		else
			setfillpattern(patternTable[fillPattern], backgroundColor);
		_fillPattern = fillPattern;
		_backgroundColor = backgroundColor;
	}
	if (foregroundColor != _foregroundColor)
	{
		setcolor(foregroundColor);
		_foregroundColor = foregroundColor;
	}
	if (xorInt != _xorInt)
	{
		setwritemode(xorInt ? XOR_PUT : COPY_PUT);
		_xorInt = xorInt;
	}
#endif
}

int UI_BGI_DISPLAY::TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID, ZIL_LOGICAL_FONT logicalFont)
{
	SetFont(logicalFont);
	if (fontTable[logicalFont & 0x0FFF].maxHeight)
		return (fontTable[logicalFont & 0x0FFF].maxHeight);
	if (string && *string)
	{
#if defined(ZIL_UNICODE)
		char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
#else
#		undef TEXT_
#		define TEXT_ (ZIL_ICHAR *)string
#endif
		return (textheight(TEXT_));
	}
	return (textheight("Mq"));
}

int UI_BGI_DISPLAY::TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID, ZIL_LOGICAL_FONT logicalFont)
{
	if (!string || !(*string))
		return (0);
	SetFont(logicalFont);
#if defined(ZIL_UNICODE)
	char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
#else
#	undef TEXT_
#	define TEXT_ (ZIL_ICHAR *)string
#endif
	int length = textwidth(TEXT_);

	ZIL_ICHAR *tabPtr = strchr((ZIL_ICHAR *)string, '\t');
	while (tabPtr)
	{
		length += textwidth(" ");
		tabPtr = strchr(tabPtr + 1, '\t');
	}

	int lineLen = strlen(string);
	for (int i=0; i < lineLen; i++)
		if (!FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE) &&	(string[i] == ZIL_HOTMARK || string[i] == '~'))
		{
			char hotKeyBuf[2];
			hotKeyBuf[0] = string[i];
			hotKeyBuf[1] = '\0';
			length -= textwidth(hotKeyBuf);
			if ((string[i] == ZIL_HOTMARK && string[i+1] == ZIL_HOTMARK) ||
				(string[i] == '~' && string[i+1] == '~'))
				i++;
		}
	return (length);
}

int UI_BGI_DISPLAY::VirtualGet(ZIL_SCREENID, int left, int top, int right, int bottom)
{
	static int i, x, y;
	static UI_DISPLAY_IMAGE *view;
	static ZIL_UINT8 *screen;

	if (--_virtualCount == -1)
	{
 		_stopDevice = TRUE;
		_virtualRegion.left = MaxValue(0, left);
		_virtualRegion.top = MaxValue(0, top);
		_virtualRegion.right = MinValue(columns - 1, right);
		_virtualRegion.bottom = MinValue(lines - 1, bottom);
		for (i = ZIL_MAXIMAGES - 1; i >= 0; i--)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				view = &displayImage[i];
				screen = view->screen;
				setviewport(0, 0, columns - 1, lines - 1, TRUE);
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++)
						if (*screen != BACKGROUND)
							putpixel(x, y, *screen);
			}
	}
	return (TRUE);
}

int UI_BGI_DISPLAY::VirtualPut(ZIL_SCREENID)
{
	static int i, x, y;
	static UI_DISPLAY_IMAGE *view;
	static ZIL_UINT8 *screen, *image;

	if (++_virtualCount == 0)
	{
		for (i = 0; i < ZIL_MAXIMAGES; i++)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				view = &displayImage[i];
				screen = view->screen;
				image = view->image;
				setviewport(0, 0, columns - 1, lines - 1, TRUE);
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++, image++)
						if (*image != BACKGROUND)
						{
							*screen = getpixel(x, y);
							putpixel(x, y, *image);
						}
			}
		_stopDevice = FALSE;
	}
	else if (_virtualCount > 0)
		_virtualCount = 0;
	return (TRUE);
}

void UI_BGI_DISPLAY::BitmapArrayToHandle(ZIL_SCREENID , int ,
	int , const ZIL_UINT8 *, const UI_PALETTE *,
	ZIL_BITMAP_HANDLE *, ZIL_BITMAP_HANDLE *)
{
}

void UI_BGI_DISPLAY::BitmapHandleToArray(ZIL_SCREENID , ZIL_BITMAP_HANDLE ,
		ZIL_BITMAP_HANDLE , int *, int *,
		ZIL_UINT8 **)
{
}

void UI_BGI_DISPLAY::DestroyBitmapHandle(ZIL_SCREENID, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
	if (colorBitmap && *colorBitmap)
	{
		delete *colorBitmap;
		*colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	}
	if (monoBitmap && *monoBitmap)
	{
		delete *monoBitmap;
		*monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	}
}

void UI_BGI_DISPLAY::IconArrayToHandle(ZIL_SCREENID , int ,
	int , const ZIL_UINT8 *, const UI_PALETTE *,
	ZIL_ICON_HANDLE *)
{
}

void UI_BGI_DISPLAY::IconHandleToArray(ZIL_SCREENID , ZIL_ICON_HANDLE ,
	int *, int *, ZIL_UINT8 **)
{
}

void UI_BGI_DISPLAY::DestroyIconHandle(ZIL_SCREENID, ZIL_ICON_HANDLE *icon)
{
	if (icon && icon->colorBitmap)
	{
		delete icon->colorBitmap;
		icon->colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	}
	if (icon && icon->monoBitmap)
	{
		delete icon->monoBitmap;
		icon->monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);

	}
}

