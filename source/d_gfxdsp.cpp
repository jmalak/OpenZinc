//	D_GFXDSP.CPP - GFX display class.
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


extern "C"
{
#	include "gfx.h"
#	include "gfx_pro.h"
}
#include "ui_dsp.hpp"

// Include graphics defines for the GFX library.
extern "C" void AFdecl _gfx_set_w2_ega_mode(void);
extern "C" void AFdecl _gfx_set_ega_modify_type(int);
extern "C" void AFdecl _gfx_clean_up_ega(void);

#define E_RES_MONO			0x4
#define TurnOnEGA()			if (_gfx.gfx_mode & E_RES_MONO) (_gfx.color_flags & XOR_PEL) ? _gfx_set_ega_modify_type(3) : _gfx_set_w2_ega_mode();
#define TurnOffEGA()		if (_gfx.gfx_mode & E_RES_MONO) _gfx_clean_up_ega();

#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
	static ZIL_ICHAR _tabString[] = { '\t', 0 };
	extern void WPrintFont(int arg, ...);
	extern int WGetLineLen(const ZIL_ICHAR *text, int max_n_char);
	extern int WOpenFont(const ZIL_ICHAR *font_name);
	extern int WCloseFont(int fh);
	extern int WFontColor(int fgnd_color, int bkgnd_color);
	extern int WFontAlign(int align);
	extern void WFontSize(int *width, int *height);
	extern int WCurrFont(int fh);
#else // !ZIL_UNICODE
#	if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
#		define GFX_SMALL_FONT	GFX_iso_smal_font
#		define GFX_DIALOG_FONT	GFX_iso_dial_font
#		define GFX_SYSTEM_FONT	GFX_iso_syst_font
#	else
#		define GFX_SMALL_FONT 	GFX_oem_smal_font
#		define GFX_DIALOG_FONT	GFX_oem_dial_font
#		define GFX_SYSTEM_FONT	GFX_oem_syst_font
#	endif
	extern FONT *GFX_SMALL_FONT(void);
	extern FONT *GFX_DIALOG_FONT(void);
	extern FONT *GFX_SYSTEM_FONT(void);
	static int _loadFont = -1;
#endif // ZIL_UNICODE

static ZIL_ICHAR _operatingSystem[] = { 'D','O','S',0 };
static ZIL_ICHAR _windowingSystem[] = { 'G','F','X',0 };
static ZIL_ICHAR _Mstr[] = { 'M', 0 };
static ZIL_ICHAR _codeSet[] = { 'I','B','M','_',0 };

UI_PATH *UI_GRAPHICS_DISPLAY::searchPath = ZIL_NULLP(UI_PATH);

// ----- UI_GRAPHICS_DISPLAY ------------------------------------------------

UI_GRAPHICS_DISPLAY::UI_GRAPHICS_DISPLAY(int mode) :
	UI_DISPLAY(FALSE, _operatingSystem, _windowingSystem)
{
	int cp = I_GetCodePage();
	strcpy(codeSet, _codeSet);
	itoa(cp, &codeSet[strlen(codeSet)], 10, 0);

	// Initialize the GFX display variables.
	_virtualCount = 0;
	_stopDevice = FALSE;
	_fillPattern = -1;
	_backgroundColor = -1;
	_foregroundColor = -1;
	_fillAttributes = FILL_SOLID + NO_FRAME;
	_outlineAttributes = 0;

	// Try to initialize the graphics mode.
	if (mode >= 0x100)		// Initialize for super-vga.
	{
		int i = 0;
		if (IdentifySVGA())
		{
			int *svga_modes;
			GetSVGAInfo(&svga_modes, 0);
			for (i = 0x107; i >= 0x100; i--)
				if (svga_modes[i - 0x100] && i <= mode)
				{
					SetSVGAMode(i);
					break;
				}
		}
		if (i < 0x100)
			if (Screen(4) == 0)
				return;
	}
	else if (Screen(mode) == 0) // Initialize for all other modes.
		return;

#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
	// Initialize the font variable.
	GRAPHICSFONT font = { 0, 0, 0 };

	// Use a temporary path if one was not installed in main().
	int pathInstalled = searchPath ? TRUE : FALSE;
	static ZIL_ICHAR name[] = { 'u','n','i','c','o','d','e','.','f','n','t', 0 };
	const ZIL_ICHAR *pathName = name;
	ZIL_ICHAR fileName[ZIL_MAXPATHLEN];

	// Try to open the UNICODE graphics font file.
	while (!installed && pathName)
	{
		// Try to open the font file
		font.font = WOpenFont(pathName);
		if (font.font != FAILURE)
		{
			installed = TRUE;
			WCurrFont(font.font);
			WFontSize(&font.maxWidth, &font.maxHeight);
			UI_GRAPHICS_DISPLAY::fontTable[FNT_DIALOG_FONT] = font;
			UI_GRAPHICS_DISPLAY::fontTable[FNT_SMALL_FONT] = font;
			UI_GRAPHICS_DISPLAY::fontTable[FNT_SYSTEM_FONT] = font;
			break;
		}
		else if (pathName == name) // Retreive the first path-name.
		{
			if (!pathInstalled)
				searchPath = new UI_PATH;
			pathName = searchPath->FirstPathName();
		}
		else // Retreive the next path-name.
			pathName = searchPath->NextPathName();

		if (pathName) // Construct a new path-name.
		{
			ZIL_STORAGE::AppendFullPath(fileName, pathName, name);
			pathName = fileName;
		}
	}

	// Delete path if it was installed temporarily.
	if (!pathInstalled && searchPath)
	{
		delete searchPath;
		searchPath = ZIL_NULLP(UI_PATH);
	}

	// Error out if we couldn't find the appropriate font file.
	if (!installed)
	{
		Screen(0);
		FreeGFXWorkspace();
		return;
	}

	// Reset the font information.
	SetFont(FNT_NONE);

	// Determine the cell coordinates based on the dialog font.
	cellWidth = TextWidth(_Mstr, ID_SCREEN, FNT_DIALOG_FONT); // Stroked font.
	cellHeight = TextHeight(ZIL_NULLP(ZIL_ICHAR), ID_SCREEN, FNT_DIALOG_FONT) +
		preSpace + postSpace + 4 + 4;  // 4 above the text, 4 below the text.

	// Leave room for input method for codpages 932, 938, 942, 949, 950
	if (932 <= cp && cp <= 950 || cp == 1381)
	{
		_gfx.max_y -= 16;
		_gfx.screen_y_res -= 16;
		_gfx.view_lr_y = _gfx.max_y;
	}
#else
	// Initialize the font variable.
	GRAPHICSFONT font = { 0, 0, 0 };
	FONT *tmp;

	// Initialize the linked-in small font.
	font.font = OpenMemFont(tmp = GFX_SMALL_FONT());
	if (font.font >= 0 && _loadFont)
	{
		font.maxWidth = tmp->widest_cell - 1;
		font.maxHeight = tmp->height - 1;
		UI_GRAPHICS_DISPLAY::fontTable[FNT_SMALL_FONT] = font;
	}

	// Initialize the linked-in dialog font.
	font.font = OpenMemFont(tmp = GFX_DIALOG_FONT());
	if (font.font >= 0 && _loadFont)
	{
		font.maxWidth = tmp->widest_cell - 1;
		font.maxHeight = tmp->height - 1;
		UI_GRAPHICS_DISPLAY::fontTable[FNT_DIALOG_FONT] = font;
	}

	// Initialize the linked-in system font.
	font.font = OpenMemFont(tmp = GFX_SYSTEM_FONT());
	if (font.font >= 0 && _loadFont)
	{
		font.maxWidth = tmp->widest_cell - 1;
		font.maxHeight = tmp->height - 1;
		UI_GRAPHICS_DISPLAY::fontTable[FNT_SYSTEM_FONT] = font;
	}
	_loadFont = 0;
	// Reset the font information.
	SetFont(FNT_NONE);

	// Determine the cell coordinates based on the dialog font.
	cellWidth = (fontTable[FNT_DIALOG_FONT].font == ROM_8x8) ?
		TextWidth(_Mstr, ID_SCREEN, FNT_DIALOG_FONT) : 	// Bitmap font.
		TextWidth(_Mstr, ID_SCREEN, FNT_DIALOG_FONT) - 2; // Stroked font.
	cellHeight = TextHeight(ZIL_NULLP(ZIL_ICHAR), ID_SCREEN, FNT_DIALOG_FONT) +
		preSpace + postSpace + 4 + 4;  // 4 above the text, 4 below the text.
#endif

	// Initialize additional member variables.
	maxColors = _gfx.n_colors;
	isMono = (maxColors == 2);
	columns = _gfx.screen_x_res;
	lines = _gfx.screen_y_res;

	// Reset some palette colors if in >= VGA mode.
	if (maxColors >= 15 && columns >= 639 && lines >= 479)
	{
		unsigned char pal1[] = { 63, 63, 50 };  // A lighter yellow.
		unsigned char pal2[] = { 50, 50, 50 };	 // A lighter lightgray.
		unsigned char pal3[] = { 35, 35, 35 };	 // A lighter darkgray.
		MapVGAPalette(62, 1, pal1);
		MapVGAPalette(7, 1, pal2);
		MapVGAPalette(56, 1, pal3);
	}

	// Fill the screen according to the specified palette.
	SetPattern(backgroundPalette, FALSE);
	Box(-1, -1, columns, lines, _fillAttributes | NO_FRAME);

	// Define the screen display region.
	Add(0,new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
	installed = TRUE;
}

UI_GRAPHICS_DISPLAY::~UI_GRAPHICS_DISPLAY(void)
{
	// Restore the display.
	if (installed)
	{
#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
		WCurrFont(0);
		WCloseFont(fontTable[FNT_DIALOG_FONT].font);
#endif
		FreeGFXWorkspace();
		Screen(0);
		Cls();
	}
}

void UI_GRAPHICS_DISPLAY::Bitmap(ZIL_SCREENID screenID, int column, int line,
	int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
	const UI_PALETTE *palette, const UI_REGION *clipRegion, ZIL_BITMAP_HANDLE *_colorBitmap,
	ZIL_BITMAP_HANDLE *_monoBitmap)
{
	// Make sure there is a valid image
	ZIL_BITMAP_HANDLE colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE), monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	if (_colorBitmap)
		colorBitmap = *_colorBitmap;
	if (_monoBitmap)
		monoBitmap = *_monoBitmap;
	if (!bitmapArray && !colorBitmap)
		return;

	// Assign the bitmap to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, column, line,
		column + bitmapWidth - 1, line + bitmapHeight - 1))
		return;
	if (!palette)
		palette = colorMap;

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);

	// Draw the bitmap on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

			if (monoBitmap)
			{
				PutPic(column, line, (PIC *)monoBitmap, 'A');
				PutPic(column, line, (PIC *)colorBitmap, 'X');
			}
			else if (colorBitmap)
				PutPic(column, line, (PIC *)colorBitmap, 'P');
			else
			{
				TurnOnEGA();
				for (int y = tRegion.top; y <= tRegion.bottom; y++)
				{
					const ZIL_UINT8 *pixel = bitmapArray + (y - line) * bitmapWidth + tRegion.left - column;
					for (int x = tRegion.left; x <= tRegion.right; x++, pixel++)
						if (*pixel != BACKGROUND)
							Pset(x, y, *pixel);
				}
				TurnOffEGA();
			}

			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

void UI_GRAPHICS_DISPLAY::BitmapArrayToHandle(ZIL_SCREENID, int, int,
	const ZIL_UINT8 *, const UI_PALETTE *, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
	// This routine was broken by recent GFX update.
	if (colorBitmap)
	{
		*colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
		*monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	}
}

void UI_GRAPHICS_DISPLAY::BitmapHandleToArray(ZIL_SCREENID, ZIL_BITMAP_HANDLE, ZIL_BITMAP_HANDLE,
	int *, int *, ZIL_UINT8 **bitmapArray)
{
	// This routine was broken by recent GFX update.
	if (bitmapArray)
		*bitmapArray = ZIL_NULLP(ZIL_UINT8);
}

void UI_GRAPHICS_DISPLAY::DestroyBitmapHandle(ZIL_SCREENID, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
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

void UI_GRAPHICS_DISPLAY::Ellipse(ZIL_SCREENID screenID, int x, int y, int startAngle,
	int endAngle, int xRadius, int yRadius, const UI_PALETTE *palette,
	int fill, int xorInt, const UI_REGION *clipRegion)
{
	// Assign the ellipse to the region structure.
	UI_REGION region, tRegion;
	if (!xRadius || !yRadius)
		return;
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

	// Prepare the display for output
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xorInt);
	int angleWidth = (startAngle > endAngle) ?
		(360 - startAngle) + endAngle : endAngle - startAngle;

	// Draw the ellipse on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

			// Draw an ellipse.
			if (startAngle == 0 && endAngle == 360)
			{
				Oval(x, y, xRadius, yRadius, fill ? _fillAttributes : _outlineAttributes);
			}
			// Draw an arc or sector.
			else
			{
				if (fill)
					OvalPieSlice(x, y, xRadius, yRadius, startAngle * 10, angleWidth * 10, _fillAttributes);
				else
					OvalArc(x, y, xRadius, yRadius, startAngle * 10, angleWidth * 10, _outlineAttributes);
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

int UI_GRAPHICS_DISPLAY::DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine)
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
		_gfx.min_x = 0;
		_gfx.min_y = 0;
		_gfx.max_x = columns - 1;
		_gfx.max_y = lines - 1;
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
					*backup = Point(newX, newY);
				if (*image != BACKGROUND)
					Pset(newX, newY, *image);
			}
		}
		screen = view->screen;
		for (y = view->region.top; y <= view->region.bottom; y++)
			for (x = view->region.left; x <= view->region.right; x++, screen++)
				if (*screen != BACKGROUND)
					Pset(x, y, *screen);
		memcpy(view->screen, view->backup, width * height);
		view->region.left = newColumn;
		view->region.top = newLine;
		view->region.right = newColumn + width - 1;
		view->region.bottom = newLine + height - 1;
	}

	_stopDevice = FALSE;
	return (TRUE);
}

int UI_GRAPHICS_DISPLAY::DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line,
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

void UI_GRAPHICS_DISPLAY::IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
	int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
	ZIL_ICON_HANDLE *hIcon)
{
	BitmapArrayToHandle(screenID, iconWidth, iconHeight, iconArray, palette,
		&hIcon->colorBitmap, &hIcon->monoBitmap);
}

void UI_GRAPHICS_DISPLAY::IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE hIcon,
	int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray)
{
	BitmapHandleToArray(screenID, hIcon.colorBitmap, hIcon.monoBitmap, iconWidth,
		iconHeight, iconArray);
}

void UI_GRAPHICS_DISPLAY::DestroyIconHandle(ZIL_SCREENID, ZIL_ICON_HANDLE *icon)
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

void UI_GRAPHICS_DISPLAY::Line(ZIL_SCREENID screenID, int x1, int y1, int x2, int y2,
	const UI_PALETTE *palette, int width, int xorInt, const UI_REGION *clipRegion)
{
	// Assign the line to a region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, MinValue(x1, x2), MinValue(y1, y2),
		MaxValue(x1, x2) + width, MaxValue(y1, y2) + width))
		return;

	// Prepare the display for output.
	int slope = x1 == x2 ? 1 : (y1 - y2) / (x1 - x2);
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xorInt);
	
	// Draw the line on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;
			for (int i = 0; i < width; i++)
				if (slope)
					DrawLine(x1 + i, y1, x2 + i, y2, _outlineAttributes);
				else
					DrawLine(x1, y1 + i, x2, y2 + i, _outlineAttributes);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

ZIL_COLOR UI_GRAPHICS_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
	// Match the color request based on the type of display.
	if (maxColors == 2)
		return (foreground ? palette->bwForeground : palette->bwBackground);
	else if (maxColors < 16)
		return (foreground ? palette->grayScaleForeground : palette->grayScaleBackground);
	return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_GRAPHICS_DISPLAY::Polygon(ZIL_SCREENID screenID, int numPoints,
	const int *polygonPoints, const UI_PALETTE *palette, int fill, int xorInt,
	const UI_REGION *clipRegion)
{
	// Assign the polygon to the region structure.
	int left = 0x0FFF, top = 0x0FFF, right = 0, bottom = 0;
	for (int i = 0; i < numPoints * 2; i += 2)
	{
		left = MinValue(left, polygonPoints[i]);
		top = MinValue(top, polygonPoints[i+1]);
		right = MaxValue(right, polygonPoints[i]);
		bottom = MaxValue(bottom, polygonPoints[i+1]);
	}
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xorInt);

	// Draw the polygon on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;
#if defined(DOS386)
			PolyLine(0, 0, (int *)polygonPoints, numPoints,
				 fill ? _fillAttributes : _outlineAttributes);
#else
			PolyLine(0, 0, (int far *)polygonPoints, numPoints,
				 fill ? _fillAttributes : _outlineAttributes);
#endif
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

void UI_GRAPHICS_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
	int bottom, const UI_PALETTE *palette, int width, int fill, int xorInt,
	const UI_REGION *clipRegion)
{
	// Assign the rectangle to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xorInt);

	// Draw the rectangle on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

			if (fill)
				Box(left - 1, top - 1, right + 1, bottom + 1, _fillAttributes | NO_FRAME);
			else if (width < 0)
			{
				// Draws a dashed rounded rectangle to indicate focus
				UI_REGION newClip = clipRegion ? *clipRegion : region;
				SetLinePat(0x5555);
				if (newClip.top <= top)
					DrawLine(MaxValue(left+1, newClip.left+1), top,
						MinValue(right-1, newClip.right - 1), top, _outlineAttributes + LINE_PAT);
				if (newClip.right >= right)
					DrawLine(right, MaxValue(top+1, newClip.top+1), right,
						MinValue(bottom-1, newClip.bottom - 1), _outlineAttributes + LINE_PAT);
				if (newClip.bottom >= bottom)
					DrawLine(MaxValue(left+1, newClip.left+1), bottom,
						MinValue(right-1, newClip.right-1), bottom, _outlineAttributes + LINE_PAT);
				if (newClip.left <= left)
					DrawLine(left, MaxValue(top+1, newClip.top+1), left,
						MinValue(bottom-1, newClip.bottom-1), _outlineAttributes + LINE_PAT);
				SetLinePat(0xFFFF);
			}
			if (!fill || _backgroundColor != _foregroundColor)
				for (int i = 0; i < width; i++)
					Box(left + i, top + i, right - i, bottom - i, _outlineAttributes);

			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

void UI_GRAPHICS_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
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
		_gfx.min_x = clipRegion->left;
		_gfx.min_y = clipRegion->top;
		_gfx.max_x = clipRegion->right;
		_gfx.max_y = clipRegion->bottom;
	}
	else
	{
		_gfx.min_x = 0;
		_gfx.min_y = 0;
		_gfx.max_x = columns - 1;
		_gfx.max_y = lines - 1;
	}
	Box(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom, _outlineAttributes);
	Box(newRegion.left, newRegion.top, newRegion.right, newRegion.bottom, _outlineAttributes);

	// Update the screen.
	VirtualPut(ID_DIRECT);
}

void UI_GRAPHICS_DISPLAY::RegionDefine(ZIL_SCREENID screenID, int left, int top,
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

void UI_GRAPHICS_DISPLAY::RegionMove(const UI_REGION &oldRegion,
	int newColumn, int newLine, ZIL_SCREENID oldScreenID, ZIL_SCREENID)
{
	VirtualGet(oldScreenID, MinValue(oldRegion.left, newColumn), MinValue(oldRegion.top, newLine),
		MaxValue(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
		MaxValue(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top)));

	_gfx.min_x = 0;
	_gfx.min_y = 0;
	_gfx.max_x = columns - 1;
	_gfx.max_y = lines - 1;

	PIC *image;
	if ((image = GetPic(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom)) != ZIL_NULLP(PIC))
	{
		PutPic(newColumn, newLine, image, 'P');
		FreePic(image);
	}
	else
	{
		if (oldRegion.top < newLine)
		{
			for (int i = oldRegion.bottom - oldRegion.top; i >= 0; i--)
				if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
				{
					image = GetPic(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i);
					PutPic(newColumn, newLine + i, image, 'P');
					FreePic(image);
				}
		}
		else
		{
			int height = oldRegion.bottom - oldRegion.top;
			for (int i = 0; i <= height; i++)
				if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
				{
					image = GetPic(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i);
					PutPic(newColumn, newLine + i, image, 'P');
					FreePic(image);
				}
		}
	}

	VirtualPut(oldScreenID);
}

#if defined(ZIL_UNICODE) && !defined(ZIL_EXTENDED)
char *UnicodeToISO(const ZIL_ICHAR *str)
{
	static char tmpBuff[256];
	int i;
	for (i = 0; str[i]; i++)
		tmpBuff[i] = str[i];
	tmpBuff[i] = str[i];
	return (tmpBuff);
}
#endif

void UI_GRAPHICS_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
	const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
	int xorInt, const UI_REGION *clipRegion, ZIL_LOGICAL_FONT logicalFont)
{
	// Make sure we have a valid string.
	if (!text || !text[0] || !palette)
		return;

	// Set up the fill line.
	static ZIL_ICHAR ZIL_FARDATA fillLine[256];
	if (length < 0)
		length = strlen(text);
	else if (length > 255)
		length = 255;
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';

	// Determine the hot-key values and shrink the string to remove ZIL_HOTMARK.
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
#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
				int markLen = WGetLineLen(hotKeyBuf, 1);
#else
#	if defined(ZIL_UNICODE)
				char *TEXT_ = UnicodeToISO(hotKeyBuf);
#	else
#				define TEXT_ hotKeyBuf
#	endif
				int markLen = GetLineLen(CurrFont(DFLT), TEXT_, 1);
#endif
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

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xorInt);
#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
	WFontColor((xorInt ? WHITE | XOR_PEL : _foregroundColor),
		   (fill && !xorInt ? _backgroundColor : XPARENT));
#else
	FontColor(CurrFont(DFLT),
		  (xorInt ? WHITE | XOR_PEL : _foregroundColor),
		  (fill && !xorInt ? _backgroundColor : XPARENT));
#endif
	
	// Draw the rectangle on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
			WPrintFont(left, top, fillLine, length);
#else
#	if defined(ZIL_UNICODE)
			char *TEXT_ = UnicodeToISO(fillLine);
#	else
#			undef TEXT_
#			define TEXT_ fillLine
#	endif
			PrintFont(left, top, TEXT_, length);
#endif
			if (hotKey)
				DrawLine(hotKeyLeft, bottom, hotKeyRight, bottom, _outlineAttributes);

			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

void UI_GRAPHICS_DISPLAY::SetFont(ZIL_LOGICAL_FONT logicalFont)
{
	static ZIL_LOGICAL_FONT _logicalFont = FNT_NONE;
	static GRAPHICSFONT _fontInfo;
	GRAPHICSFONT fontInfo;

	// Check for invalid or new font set.
	if (_logicalFont == FNT_NONE || logicalFont == FNT_NONE)
	{
		logicalFont = FNT_DIALOG_FONT;
		fontInfo = fontTable[FNT_DIALOG_FONT];
#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
		WCurrFont(fontInfo.font);
		WFontAlign(TOP_LINE);
#else
		FontAlign(fontInfo.font, TOP_LINE);
		if (fontInfo.font)
			CurrFont(fontInfo.font);
#endif
		LineJustify(JUSTIFY_START);
		LineDirection(TEXT_L_TO_R);
		_logicalFont = logicalFont;
		_fontInfo = fontInfo;
	}
	else
	{
		logicalFont &= 0x0FFF;
		if (logicalFont == _logicalFont)
			return;
	}

	// Update the font information.
	fontInfo = fontTable[logicalFont];
	if (fontInfo.font != _fontInfo.font)
	{
#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
		WCurrFont(fontInfo.font);
		WFontAlign(TOP_LINE);
#else
		FontAlign(fontInfo.font, TOP_LINE);
		if (fontInfo.font)
			CurrFont(fontInfo.font);
#endif
		if (fontInfo.font)
			CurrFont(fontInfo.font);
	}
	_fontInfo = fontInfo;
	_logicalFont = logicalFont;
}

void UI_GRAPHICS_DISPLAY::SetPattern(const UI_PALETTE *palette, int xorInt)
{
	int fillPattern = palette->fillPattern;
	int backgroundColor = MapColor(palette, FALSE);
	int foregroundColor = MapColor(palette, TRUE);

	if (fillPattern != _fillPattern || backgroundColor != _backgroundColor)
	{
		_fillPattern = fillPattern;
		_backgroundColor = backgroundColor;
		if (fillPattern == PTN_SOLID_FILL)
		{
			_fillAttributes = FILL_SOLID + foregroundColor;
			SetFillSolid(backgroundColor);
		}
		else
		{
			_fillAttributes = FILL_PAT + foregroundColor;
			SetFillPat(patternTable[fillPattern], foregroundColor, backgroundColor);
		}
	}
	if (foregroundColor != _foregroundColor)
	{
		_foregroundColor = foregroundColor;
		_outlineAttributes = (_outlineAttributes & COLOR_FLAGS) + foregroundColor;
		_fillAttributes = (_fillAttributes & COLOR_FLAGS) + foregroundColor;
	}

	if (xorInt)
	{
		_fillAttributes |= XOR_PEL;
		_outlineAttributes |= XOR_PEL;
	}
	else
	{
		_fillAttributes &= ~XOR_PEL;
		_outlineAttributes &= ~XOR_PEL;
	}
}

int UI_GRAPHICS_DISPLAY::TextHeight(const ZIL_ICHAR *, ZIL_SCREENID, ZIL_LOGICAL_FONT logicalFont)
{
	logicalFont &= 0x0FFF;
	SetFont(logicalFont);
	if (fontTable[logicalFont].maxHeight)
		return (fontTable[logicalFont].maxHeight);
	else
		return (GetFontHeight(DFLT));
}

int UI_GRAPHICS_DISPLAY::TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID, ZIL_LOGICAL_FONT logicalFont)
{
	if (!string || !(*string))
		return (0);
	SetFont(logicalFont & 0x0FFF);
	int lineLen =  strlen(string);
#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
	int length =  WGetLineLen(string, lineLen);
	int spaceLen = WGetLineLen(_blankString, 1);
	int tabLen = WGetLineLen(_tabString, 1);
#else
#	if defined(ZIL_UNICODE)
	char *TEXT_ = UnicodeToISO(string);
#	else
#	undef TEXT_
#	define TEXT_ (ZIL_ICHAR *)string
#	endif
	int length = GetLineLen(CurrFont(DFLT), TEXT_, ::strlen(TEXT_));
	int spaceLen = GetLineLen(CurrFont(DFLT), " ", 1);
	int tabLen = GetLineLen(CurrFont(DFLT), "\t", 1);
#endif
	for (int i=0; i < lineLen; i++)
	{
		if (!FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE) &&	(string[i] == ZIL_HOTMARK || string[i] == '~'))
		{
			ZIL_ICHAR hotKeyBuf[2];
			hotKeyBuf[0] = string[i];
			hotKeyBuf[1] = '\0';
#if defined(ZIL_UNICODE) && defined(ZIL_DOSEXTENDED)
			length -= WGetLineLen(hotKeyBuf, 1);
#else
#	if defined(ZIL_UNICODE)
			char *TEXT_ = UnicodeToISO(hotKeyBuf);
#	else
#			undef TEXT_
#			define TEXT_ hotKeyBuf
#	endif
			length -= GetLineLen(CurrFont(DFLT), TEXT_, 1);
#endif
			if ((string[i] == ZIL_HOTMARK && string[i+1] == ZIL_HOTMARK) ||
				(string[i] == '~' && string[i+1] == '~'))
				i++;
		}
		if (string[i] == '\t')
			length += spaceLen - tabLen;
	}
	return (length);
}

int UI_GRAPHICS_DISPLAY::VirtualGet(ZIL_SCREENID, int left, int top, int right, int bottom)
{
	static int i, x, y;
	static UI_DISPLAY_IMAGE *view;
	static ZIL_UINT8 *screen;

	if (--_virtualCount == -1)
	{
		_gfx.min_x = 0;
		_gfx.min_y = 0;
		_gfx.max_x = columns - 1;
		_gfx.max_y = lines - 1;
		_gfx.color_flags &= ~XOR_PEL;

		_stopDevice = TRUE;
		_virtualRegion.left = MaxValue(0, left);
		_virtualRegion.top = MaxValue(0, top);
		_virtualRegion.right = MinValue(columns - 1, right);
		_virtualRegion.bottom = MinValue(lines - 1, bottom);

		TurnOnEGA();
		for (i = ZIL_MAXIMAGES - 1; i >= 0; i--)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				view = &displayImage[i];
				screen = view->screen;
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++)
						if (*screen != BACKGROUND)
							Pset (x, y, *screen);
			}
		TurnOffEGA();
	}
	return (TRUE);
}

int UI_GRAPHICS_DISPLAY::VirtualPut(ZIL_SCREENID)
{
	static int i, x, y;
	static UI_DISPLAY_IMAGE *view;
	static ZIL_UINT8 *screen, *image;

	if (++_virtualCount == 0)
	{
		_gfx.min_x = 0;
		_gfx.min_y = 0;
		_gfx.max_x = columns - 1;
		_gfx.max_y = lines - 1;
		_gfx.color_flags &= ~XOR_PEL;

		TurnOnEGA();
		for (i = 0; i < ZIL_MAXIMAGES; i++)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				view = &displayImage[i];
				screen = view->screen;
				image = view->image;
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++, image++)
						if (*image != BACKGROUND)
						{
							*screen = Point(x, y);
							Pset (x, y, *image);
						}
			}
		_stopDevice = FALSE;
		TurnOffEGA();
	}
	else if (_virtualCount > 0)
		_virtualCount = 0;
	return (TRUE);
}

#if (defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)) && (__BORLANDC__ < 0x0300)
GRAPHICSFONT UI_GRAPHICS_DISPLAY::fontTable[ZIL_MAXFONTS] =
#elif defined(__ZTC__) && __ZTC__ < 0x310
GRAPHICSFONT UI_GRAPHICS_DISPLAY::fontTable[ZIL_MAXFONTS] =
#else
UI_GRAPHICS_DISPLAY::GRAPHICSFONT UI_GRAPHICS_DISPLAY::fontTable[ZIL_MAXFONTS] =
#endif
{
	{ ROM_8x14, 8, 14 },	// FNT_SMALL_FONT
	{ ROM_8x14, 8, 14 },	// FNT_DIALOG_FONT
	{ ROM_8x14, 8, 14 },	// FNT_SYSTEM_FONT
	{ ROM_8x14, 8, 14 },
	{ ROM_8x14, 8, 14 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 }
};

#if (defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)) && (__BORLANDC__ < 0x0300)
GRAPHICSPATTERN UI_GRAPHICS_DISPLAY::patternTable[ZIL_MAXPATTERNS] =
#elif defined(__ZTC__) && __ZTC__ < 0x310
GRAPHICSPATTERN UI_GRAPHICS_DISPLAY::patternTable[ZIL_MAXPATTERNS] =
#else
UI_GRAPHICS_DISPLAY::GRAPHICSPATTERN UI_GRAPHICS_DISPLAY::patternTable[ZIL_MAXPATTERNS] =
#endif
{
	// The first two bytes indicate the number of bytes per row and number of rows.
	{ 1, 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// EMPTY_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// SOLID_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LINE_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LTSLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// SLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// BKSLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LTBKSLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// HATCH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// XHATCH_FILL
	{ 1, 8, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },	// INTERLEAVE_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// WIDE_DOT_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// CLOSE_DOT_FILL

	{ 1, 8, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },	// PTN_BACKGROUND_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

