//	Zinc Interface Library - UI_DSP.HPP
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


// ----- Include file and environment definition ----------------------------

#if !defined(UI_DSP_HPP)
#	define UI_DSP_HPP
//#	if !defined(UI_GEN_HPP)
#		include <ui_gen.hpp>
//#	endif

// ----- ZIL_SCREENID, ZIL_BITMAP_HANDLE, ZIL_ICON_HANDLE, ZIL_SCREEN_CELL --
#if defined(ZIL_NEXTSTEP)
#	include "nextc.hpp"
#endif

#if defined(ZIL_MSDOS)
	typedef ZIL_INT32 ZIL_SCREENID;
	typedef void *ZIL_BITMAP_HANDLE;
	typedef ZIL_BITMAP_HANDLE ZIL_IMAGE_HANDLE;
	typedef ZIL_UINT16 ZIL_SCREEN_CELL;
	struct ZIL_ICON_HANDLE
	{
		ZIL_BITMAP_HANDLE colorBitmap;
		ZIL_BITMAP_HANDLE monoBitmap;
	};
	const ZIL_SCREENID ID_DIRECT			= 0xFFFF;	// special screen values.
	const ZIL_SCREENID ID_SCREEN			= 0x0001;
#elif defined(ZIL_WINNT)
	typedef HWND ZIL_SCREENID;
	typedef HBITMAP ZIL_BITMAP_HANDLE;
	typedef ZIL_BITMAP_HANDLE ZIL_IMAGE_HANDLE;
	typedef HICON ZIL_ICON_HANDLE;
    // special screen values.  (must use #define because IDs are pointers if STRICT.)
#	define ID_DIRECT						(ZIL_SCREENID)0x0000
#	define ID_SCREEN						(ZIL_SCREENID)0x0000
#elif defined(ZIL_MSWINDOWS)
	typedef HWND ZIL_SCREENID;
	typedef HBITMAP ZIL_BITMAP_HANDLE;
	typedef ZIL_BITMAP_HANDLE ZIL_IMAGE_HANDLE;
	typedef HICON ZIL_ICON_HANDLE;
    // special screen values.  (must use #define because IDs are pointers if STRICT.)
#	define ID_DIRECT						(ZIL_SCREENID)0xFFFF
#	define ID_SCREEN						(ZIL_SCREENID)0x0000
#elif defined(ZIL_OS2)
	typedef HWND ZIL_SCREENID;
	typedef HBITMAP ZIL_BITMAP_HANDLE;
	typedef ZIL_BITMAP_HANDLE ZIL_IMAGE_HANDLE;
	typedef HPOINTER ZIL_ICON_HANDLE;
	const ZIL_SCREENID ID_DIRECT			= 0xFFFF;	// special screen values.
	const ZIL_SCREENID ID_SCREEN			= 0x0000;
#elif defined(ZIL_XT)
	typedef Widget ZIL_SCREENID;
	typedef Pixmap ZIL_BITMAP_HANDLE;
	typedef ZIL_BITMAP_HANDLE ZIL_IMAGE_HANDLE;
	typedef Pixmap ZIL_ICON_HANDLE;
	const ZIL_SCREENID ID_DIRECT			= 0x0000;	// special screen values.
	const ZIL_SCREENID ID_SCREEN			= 0x0000;
#elif defined(ZIL_CURSES)
	typedef ZIL_INT32 ZIL_SCREENID;
	typedef void *ZIL_BITMAP_HANDLE;
	typedef ZIL_BITMAP_HANDLE ZIL_IMAGE_HANDLE;
	typedef chtype ZIL_SCREEN_CELL;
	struct ZIL_ICON_HANDLE
	{
		ZIL_BITMAP_HANDLE colorBitmap;
		ZIL_BITMAP_HANDLE monoBitmap;
	};
	const ZIL_SCREENID ID_DIRECT			= 0xFFFF;	// special screen values.
	const ZIL_SCREENID ID_SCREEN			= 0x0001;
#elif defined(ZIL_MACINTOSH)
	typedef Handle ZIL_SCREENID;
	typedef BitMapHandle ZIL_BITMAP_HANDLE;
	typedef PicHandle ZIL_IMAGE_HANDLE;
	struct ZIL_ICON_HANDLE
	{
		Handle iconHandle;
		ZIL_BITMAP_HANDLE monoBitmap;
		ZIL_BITMAP_HANDLE colorBitmap;
	};
	const ZIL_SCREENID ID_DIRECT			= (ZIL_SCREENID)0xFFFFFFFFL;	// special screen values.
	const ZIL_SCREENID ID_SCREEN			= 0x00000000L;
	const short appleID						= 1;	// apple menu number.
	const short lastMenuID					= 235;	// last possible menu number.
#elif defined(ZIL_NEXTSTEP)
	typedef id ZIL_SCREENID;
	typedef id ZIL_BITMAP_HANDLE;
	typedef ZIL_BITMAP_HANDLE ZIL_IMAGE_HANDLE;
	typedef id ZIL_ICON_HANDLE;
	const ZIL_SCREENID ID_DIRECT			= 0x0000;	// special screen values.
	const ZIL_SCREENID ID_SCREEN			= 0x0000;
#endif

// --------------------------------------------------------------------------
// ----- UI_POSITION --------------------------------------------------------
// --------------------------------------------------------------------------

struct ZIL_EXPORT_CLASS UI_POSITION
{
	int column, line;

#if defined(ZIL_MSWINDOWS)
	void Assign(const POINT &point) { column = point.x, line = point.y; }
#elif defined(ZIL_OS2)
	void Assign(const POINTL &point) { column = point.x, line = point.y; }
#elif defined(ZIL_MACINTOSH)
	void Assign(const Point &point) { column = point.h, line = point.v; }
#elif defined(ZIL_NEXTSTEP)
	void Assign(const NXPoint &point) { column = (int)point.x, line = (int)point.y; }
#endif
	int operator==(const UI_POSITION &position) const
		{ return (position.column == column && position.line == line); }
	int operator!=(const UI_POSITION &position) const
		{ return (position.column != column || position.line != line); }
	int operator<(const UI_POSITION &position) const
		{ return (position.column < column || position.line < line); }
	int operator>(const UI_POSITION &position) const
		{ return (position.column > column || position.line > line); }
	int operator>=(const UI_POSITION &position) const
		{ return (position.column >= column || position.line >= line); }
	int operator<=(const UI_POSITION &position) const
		{ return (position.column <= column || position.line <= line); }
	UI_POSITION &operator++(void) { column++, line++; return (*this); }
	UI_POSITION &operator--(void) { column--, line--; return (*this); }
	UI_POSITION &operator+=(int offset)
		{ column += offset, line += offset; return (*this); }
	UI_POSITION &operator-=(int offset)
		{ column -= offset, line -= offset; return (*this); }
};

// --------------------------------------------------------------------------
// ----- UI_REGION, UI_REGION_ELEMENT, UI_REGION_LIST -----------------------
// --------------------------------------------------------------------------

struct ZIL_EXPORT_CLASS UI_REGION
{
public:
	int left, top, right, bottom;

#if defined(ZIL_MSWINDOWS)
	void Assign(const RECT &rect)
		{ left = rect.left, top = rect.top, right = rect.right - 1, bottom = rect.bottom - 1; }
#elif defined(ZIL_OS2)
	void Assign(const RECTL &rect)
		{ left = rect.xLeft, top = rect.yBottom, right = rect.xRight, bottom = rect.yTop; }
#elif defined(ZIL_MACINTOSH)
	void Assign(const Rect &rect)
		{ left = rect.left, top = rect.top, right = rect.right, bottom = rect.bottom; }
#elif defined(ZIL_NEXTSTEP)
	void Assign(const NXRect &rect)
		{ left = (int)rect.origin.x, top = (int)rect.origin.y, right = left + (int)rect.size.width,
		bottom = top + (int)rect.size.height; }
#endif
	int Encompassed(const UI_REGION &region) const
		{ return (left >= region.left && top >= region.top &&
		  right <= region.right && bottom <= region.bottom); }
	int Height(void) const { return (bottom - top + 1); }
	int Overlap(const UI_REGION &region) const
		{ return (MaxValue(region.left, left) <= MinValue(region.right, right) &&
		  MaxValue(region.top, top) <= MinValue(region.bottom, bottom)); }
	int Overlap(const UI_POSITION &position) const
		{ return (position.column >= left && position.column <= right &&
		  position.line >= top && position.line <= bottom); }
	int Touching(const UI_POSITION &position) const
		{ return (((position.column == left || position.column == right) &&
				    position.line >= top && position.line <= bottom) ||
				  ((position.line == top || position.line == bottom) &&
				    position.column >= left && position.column <= right)); }
	int Overlap(const UI_REGION &region, UI_REGION &result) const
		{ result.left = MaxValue(left, region.left);
		  result.top = MaxValue(top, region.top);
		  result.right = MinValue(right, region.right);
		  result.bottom = MinValue(bottom, region.bottom);
		  return (result.left <= result.right && result.top <= result.bottom);
		}
	int Width(void) const { return (right - left + 1); }

	int operator==(const UI_REGION &region) const
		{ return (region.left == left && region.top == top &&
			region.right == right && region.bottom == bottom); }
	int operator!=(const UI_REGION &region) const
		{ return (region.left != left || region.top != top ||
			region.right != right || region.bottom != bottom); }
	UI_REGION &operator++(void)
		{ left--, top--, right++, bottom++; return (*this); }
	UI_REGION &operator--(void)
		{ left++, top++, right--, bottom--; return (*this); }
	UI_REGION &operator+=(int offset)
		{ left -= offset, top -= offset, right += offset, bottom += offset; return (*this); }
	UI_REGION &operator-=(int offset)
		{ left += offset, top += offset, right -= offset, bottom -= offset; return (*this); }
};

class ZIL_EXPORT_CLASS UI_REGION_ELEMENT : public UI_ELEMENT
{
public:
	ZIL_SCREENID screenID;
	UI_REGION region;

	UI_REGION_ELEMENT(ZIL_SCREENID screenID, const UI_REGION &_region);
	UI_REGION_ELEMENT(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	~UI_REGION_ELEMENT(void);

	// Element members.
	UI_REGION_ELEMENT *Next(void) { return((UI_REGION_ELEMENT *)next); }
	UI_REGION_ELEMENT *Previous(void) { return((UI_REGION_ELEMENT *)previous); }
};

class ZIL_EXPORT_CLASS UI_REGION_LIST : public UI_LIST
{
public:
	UI_REGION_LIST(void);
	~UI_REGION_LIST(void);
	void Split(ZIL_SCREENID screenID, const UI_REGION &region, int allocateBelow = FALSE);

	// List members.
	UI_REGION_ELEMENT *Current(void) { return((UI_REGION_ELEMENT *)current); }
	UI_REGION_ELEMENT *First(void) { return((UI_REGION_ELEMENT *)first); }
	UI_REGION_ELEMENT *Last(void) { return((UI_REGION_ELEMENT *)last); }
};

// ----- Color information --------------------------------------------------

#if defined(ZIL_CURSES)
#	if defined(SCO_UNIX)
#		define attrib(foreground, background) (COLOR_PAIR(foreground << 3 | background))
#	else
#		define attrib(foreground, background) (0)
#	endif
#else
#	define attrib(foreground, background) (((background) << 4) + (foreground))
#endif

#if defined(ZIL_MSDOS)
	typedef int ZIL_COLOR;
#elif defined(ZIL_OS2)
	typedef COLOR ZIL_COLOR;
	const ZIL_COLOR RGB_MAGENTA				= 0xB000B0;
	const ZIL_COLOR RGB_BROWN				= 0xA52A2A;
	const ZIL_COLOR RGB_LIGHTGRAY			= 0xC0C0C0;
	const ZIL_COLOR RGB_DARKGRAY			= 0x606060;
	const ZIL_COLOR RGB_LIGHTBLUE			= 0x0000FF;
	const ZIL_COLOR RGB_LIGHTGREEN			= 0x00FF00;
	const ZIL_COLOR RGB_LIGHTCYAN			= 0x00FFFF;
	const ZIL_COLOR RGB_LIGHTRED			= 0xFF0000;
	const ZIL_COLOR RGB_LIGHTMAGENTA		= 0xFF00FF;
#elif defined(ZIL_MSWINDOWS)
	typedef DWORD ZIL_COLOR;
	const ZIL_COLOR RGB_BLACK				= 0x00000000L;	// system rgb colors
	const ZIL_COLOR RGB_BLUE				= 0x00800000L;
	const ZIL_COLOR RGB_GREEN				= 0x00008000L;
	const ZIL_COLOR RGB_CYAN				= 0x00808000L;
	const ZIL_COLOR RGB_RED					= 0x00000080L;
	const ZIL_COLOR RGB_MAGENTA				= 0x00800080L;
	const ZIL_COLOR RGB_BROWN				= 0x00008080L;
	const ZIL_COLOR RGB_LIGHTGRAY			= 0x00C0C0C0L;
	const ZIL_COLOR RGB_DARKGRAY			= 0x00808080L;
	const ZIL_COLOR RGB_LIGHTBLUE			= 0x00FF0000L;
	const ZIL_COLOR RGB_LIGHTGREEN			= 0x0000FF00L;
	const ZIL_COLOR RGB_LIGHTCYAN			= 0x00FFFF00L;
	const ZIL_COLOR RGB_LIGHTRED			= 0x000000FFL;
	const ZIL_COLOR RGB_LIGHTMAGENTA		= 0x00FF00FFL;
	const ZIL_COLOR RGB_YELLOW				= 0x0000FFFFL;
	const ZIL_COLOR RGB_WHITE				= 0x00FFFFFFL;
#elif defined(ZIL_X11)
	typedef unsigned long ZIL_COLOR;
	const ZIL_COLOR RGB_BLACK				= 0x000000;		// system rgb colors
	const ZIL_COLOR RGB_BLUE				= 0x0000B0;
	const ZIL_COLOR RGB_GREEN				= 0x00B000;
	const ZIL_COLOR RGB_CYAN				= 0x00B0B0;
	const ZIL_COLOR RGB_RED					= 0xB00000;
	const ZIL_COLOR RGB_MAGENTA				= 0xB000B0;
	const ZIL_COLOR RGB_BROWN				= 0xA52A2A;
	const ZIL_COLOR RGB_LIGHTGRAY			= 0xC0C0C0;
	const ZIL_COLOR RGB_DARKGRAY			= 0x606060;
	const ZIL_COLOR RGB_LIGHTBLUE			= 0x0000FF;
	const ZIL_COLOR RGB_LIGHTGREEN			= 0x00FF00;
	const ZIL_COLOR RGB_LIGHTCYAN			= 0x00FFFF;
	const ZIL_COLOR RGB_LIGHTRED			= 0xFF0000;
	const ZIL_COLOR RGB_LIGHTMAGENTA		= 0xFF00FF;
	const ZIL_COLOR RGB_YELLOW				= 0xFFFF00;
	const ZIL_COLOR RGB_WHITE				= 0xFFFFFF;
#elif defined(ZIL_NEXTSTEP)
	typedef unsigned long ZIL_COLOR;
	const ZIL_COLOR RGB_BLACK				= 0x000000;		// system rgb colors
	const ZIL_COLOR RGB_BLUE				= 0x0000B0;
	const ZIL_COLOR RGB_GREEN				= 0x00B000;
	const ZIL_COLOR RGB_CYAN				= 0x00B0B0;
	const ZIL_COLOR RGB_RED					= 0xB00000;
	const ZIL_COLOR RGB_MAGENTA				= 0xB000B0;
	const ZIL_COLOR RGB_BROWN				= 0xA52A2A;
	const ZIL_COLOR RGB_LIGHTGRAY			= 0xAAAAAA;
	const ZIL_COLOR RGB_DARKGRAY			= 0x555555;
	const ZIL_COLOR RGB_LIGHTBLUE			= 0x0000FF;
	const ZIL_COLOR RGB_LIGHTGREEN			= 0x00FF00;
	const ZIL_COLOR RGB_LIGHTCYAN			= 0x00FFFF;
	const ZIL_COLOR RGB_LIGHTRED			= 0xFF0000;
	const ZIL_COLOR RGB_LIGHTMAGENTA		= 0xFF00FF;
	const ZIL_COLOR RGB_YELLOW				= 0xFFFF00;
	const ZIL_COLOR RGB_WHITE				= 0xFFFFFF;
#elif defined(ZIL_MACINTOSH)
	typedef unsigned long ZIL_COLOR;
	const ZIL_COLOR RGB_BLACK				= 0x00000000L;	// system rgb colors
	const ZIL_COLOR RGB_BLUE				= 0x000000D4L;
	const ZIL_COLOR RGB_GREEN				= 0x00008011L;
	const ZIL_COLOR RGB_CYAN				= 0x0002ABEAL;
	const ZIL_COLOR RGB_RED					= 0x00DD0806L;
	const ZIL_COLOR RGB_MAGENTA				= 0x00F20884L;
	const ZIL_COLOR RGB_BROWN				= 0x008B2D13L;
	const ZIL_COLOR RGB_LIGHTGRAY			= 0x00C0C0C0L;
	const ZIL_COLOR RGB_DARKGRAY			= 0x00808080L;
	const ZIL_COLOR RGB_LIGHTBLUE			= 0x000060FFL;
	const ZIL_COLOR RGB_LIGHTGREEN			= 0x0000FF11L;
	const ZIL_COLOR RGB_LIGHTCYAN			= 0x0002FFFFL;
	const ZIL_COLOR RGB_LIGHTRED			= 0x00FF2000L;
	const ZIL_COLOR RGB_LIGHTMAGENTA		= 0x00FF80FFL;
	const ZIL_COLOR RGB_YELLOW				= 0x00FCF305L;
	const ZIL_COLOR RGB_WHITE				= 0x00FFFFFFL;
#elif defined(ZIL_CURSES)
	typedef chtype ZIL_COLOR;
#endif

#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_X11) || defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP)
	const ZIL_COLOR MONO_BLACK				= 0x00;			// monochrome
	const ZIL_COLOR MONO_DIM				= 0x08;
	const ZIL_COLOR MONO_NORMAL				= 0x07;
	const ZIL_COLOR MONO_HIGH				= 0x0F;

	const ZIL_COLOR BW_BLACK				= 0x00;			// black & white
	const ZIL_COLOR BW_WHITE				= 0x01;

	const ZIL_COLOR GS_BLACK				= 0x00;			// gray scale
	const ZIL_COLOR GS_GRAY					= 0x01;
	const ZIL_COLOR GS_BLINKING				= 0x02;
	const ZIL_COLOR GS_WHITE				= 0x03;

	const ZIL_COLOR BACKGROUND				= 0xFF;			// colors
	const ZIL_COLOR MAX_COLORMAP_INDEX	    = 0x0F;

#	if !defined(__COLORS) && !defined(_BGI_COLORS)
#		define __COLORS
#		define _BGI_COLORS
		const ZIL_COLOR BLACK				= 0x00;
		const ZIL_COLOR BLUE				= 0x01;
		const ZIL_COLOR GREEN				= 0x02;
		const ZIL_COLOR CYAN				= 0x03;
		const ZIL_COLOR RED					= 0x04;
		const ZIL_COLOR MAGENTA				= 0x05;
		const ZIL_COLOR BROWN				= 0x06;
		const ZIL_COLOR LIGHTGRAY			= 0x07;
		const ZIL_COLOR DARKGRAY			= 0x08;
		const ZIL_COLOR LIGHTBLUE			= 0x09;
		const ZIL_COLOR LIGHTGREEN			= 0x0A;
		const ZIL_COLOR LIGHTCYAN			= 0x0B;
		const ZIL_COLOR LIGHTRED			= 0x0C;
		const ZIL_COLOR LIGHTMAGENTA		= 0x0D;
		const ZIL_COLOR YELLOW				= 0x0E;
		const ZIL_COLOR WHITE				= 0x0F;
#	endif

#elif defined(ZIL_CURSES)
	const ZIL_COLOR MONO_BLACK				= 0;			// monochrome
	const ZIL_COLOR MONO_DIM				= 0;
	const ZIL_COLOR MONO_NORMAL				= 0;
	const ZIL_COLOR MONO_HIGH				= 0;

	const ZIL_COLOR BW_BLACK				= 0;			// black & white
	const ZIL_COLOR BW_WHITE				= 0;

	const ZIL_COLOR GS_BLACK				= 0;			// gray scale
	const ZIL_COLOR GS_GRAY					= 0;
	const ZIL_COLOR GS_BLINKING				= 0;
	const ZIL_COLOR GS_WHITE				= 0;

	const ZIL_COLOR BACKGROUND				= 0xFF;			// colors
	const ZIL_COLOR MAX_COLORMAP_INDEX	    = 0x0F;

#	if defined(SCO_UNIX)
		const ZIL_COLOR BLACK				= COLOR_BLACK;
		const ZIL_COLOR BLUE				= COLOR_BLUE;
		const ZIL_COLOR GREEN				= COLOR_GREEN;
		const ZIL_COLOR CYAN				= COLOR_CYAN;
		const ZIL_COLOR RED					= COLOR_RED;
		const ZIL_COLOR MAGENTA				= COLOR_MAGENTA;
		const ZIL_COLOR BROWN				= COLOR_YELLOW;
		const ZIL_COLOR LIGHTGRAY			= COLOR_WHITE;
		const ZIL_COLOR DARKGRAY			= COLOR_BLACK;
		const ZIL_COLOR LIGHTBLUE			= COLOR_BLUE;
		const ZIL_COLOR LIGHTGREEN			= COLOR_GREEN;
		const ZIL_COLOR LIGHTCYAN			= COLOR_CYAN;
		const ZIL_COLOR LIGHTRED			= COLOR_RED;
		const ZIL_COLOR LIGHTMAGENTA		= COLOR_MAGENTA;
		const ZIL_COLOR YELLOW				= COLOR_YELLOW;
		const ZIL_COLOR WHITE				= COLOR_WHITE;
#	else
		const ZIL_COLOR BLACK				= 0;
		const ZIL_COLOR BLUE				= 0;
		const ZIL_COLOR GREEN				= 0;
		const ZIL_COLOR CYAN				= 0;
		const ZIL_COLOR RED					= 0;
		const ZIL_COLOR MAGENTA				= 0;
		const ZIL_COLOR BROWN				= 0;
		const ZIL_COLOR LIGHTGRAY			= 0;
		const ZIL_COLOR DARKGRAY			= 0;
		const ZIL_COLOR LIGHTBLUE			= 0;
		const ZIL_COLOR LIGHTGREEN			= 0;
		const ZIL_COLOR LIGHTCYAN			= 0;
		const ZIL_COLOR LIGHTRED			= 0;
		const ZIL_COLOR LIGHTMAGENTA		= 0;
		const ZIL_COLOR YELLOW				= 0;
		const ZIL_COLOR WHITE				= 0;
#	endif
#endif

// ----- Font information ---------------------------------------------------

#define ZIL_MAXFONTS						10
typedef ZIL_UINT16 ZIL_LOGICAL_FONT;
const ZIL_LOGICAL_FONT FNT_SMALL_FONT		= 0x0000;	// Base logical fonts.
const ZIL_LOGICAL_FONT FNT_DIALOG_FONT		= 0x0001;
const ZIL_LOGICAL_FONT FNT_SYSTEM_FONT		= 0x0002;
const ZIL_LOGICAL_FONT FNT_NONE				= 0xFFFF;	

// Ignore the & character on the Text() function (used with fill).
const ZIL_LOGICAL_FONT FNT_IGNORE_UNDERSCORE = 0x1000;

// ----- Image information --------------------------------------------------

// --- ZIL_IMAGE_TYPE ---
#define ZIL_MAXIMAGES						2
typedef unsigned ZIL_IMAGE_TYPE;
const ZIL_IMAGE_TYPE IMT_MOUSE_IMAGE		= 0;
const ZIL_IMAGE_TYPE IMT_CURSOR_IMAGE		= 1;

// ----- Pattern information ------------------------------------------------

#define ZIL_MAXPATTERNS							15
typedef ZIL_INT16 ZIL_LOGICAL_PATTERN;
const ZIL_LOGICAL_PATTERN PTN_SOLID_FILL		= 0x0001;
const ZIL_LOGICAL_PATTERN PTN_INTERLEAVE_FILL	= 0x0009;
const ZIL_LOGICAL_PATTERN PTN_BACKGROUND_FILL	= 0x000C;
const ZIL_LOGICAL_PATTERN PTN_SYSTEM_COLOR		= 0x00F0;
const ZIL_LOGICAL_PATTERN PTN_RGB_COLOR	    	= 0x00F1;

// --------------------------------------------------------------------------
// ----- UI_PALETTE ---------------------------------------------------------
// --------------------------------------------------------------------------

struct ZIL_EXPORT_CLASS UI_PALETTE
{
	// --- Text mode ---
	ZIL_ICHAR fillCharacter;				// Fill character.
	ZIL_COLOR colorAttribute;				// Color attribute.
	ZIL_COLOR monoAttribute;				// Mono attribute.

	// --- Graphics mode ---
	ZIL_LOGICAL_PATTERN fillPattern;		// Fill pattern.
	ZIL_COLOR colorForeground;				// EGA/VGA colors.
	ZIL_COLOR colorBackground;
	ZIL_COLOR bwForeground;					// Black & White colors (2 color).
	ZIL_COLOR bwBackground;
	ZIL_COLOR grayScaleForeground;			// Monochrome colors (3+ color).
	ZIL_COLOR grayScaleBackground;
};

// --------------------------------------------------------------------------
// ----- UI_DISPLAY ---------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_DISPLAY : public ZIL_INTERNATIONAL
{
public:
	int installed;
	int isText;
	int isMono;
	int columns, lines;
	int cellWidth, cellHeight;
	int preSpace, postSpace;
	ZIL_INT32 miniNumeratorX, miniDenominatorX;
	ZIL_INT32 miniNumeratorY, miniDenominatorY;
	ZIL_ICHAR *operatingSystem;
	ZIL_ICHAR *windowingSystem;

	static UI_PALETTE *backgroundPalette;
	static UI_PALETTE *xorPalette;
	static UI_PALETTE *colorMap;

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	static UI_PALETTE *markPalette;
#elif defined(ZIL_MSWINDOWS)
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	int nCmdShow;
#elif defined(ZIL_OS2)
	HAB hab;
#elif defined(ZIL_XT)
	XtAppContext appContext;
	Widget topShell;
	Display *xDisplay;
	Screen *xScreen;
	int xScreenNumber;
	GC xGC;
	GC xorGC;
	char *appClass;
	Pixmap interleaveStipple;
#	if defined(ZIL_UNICODE)
	XIC ic;
#	endif
#endif

	virtual ~UI_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	void Rectangle(ZIL_SCREENID screenID, const UI_REGION &region,
		const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION))
		{ Rectangle(screenID, region.left, region.top, region.right,
		  region.bottom, palette, width, fill, _xor, clipRegion); }
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	void RegionDefine(ZIL_SCREENID screenID, const UI_REGION &region)
		{ RegionDefine(screenID, region.left, region.top, region.right, region.bottom); }
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		ZIL_SCREENID oldScreenID = ID_SCREEN, ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	int VirtualGet(ZIL_SCREENID screenID, const UI_REGION &region)
		{ return (VirtualGet(screenID, region.left, region.top, region.right, region.bottom)); }
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line, int width, int height, ZIL_UINT8 *image);

protected:
	struct ZIL_EXPORT_CLASS UI_DISPLAY_IMAGE
	{
		UI_REGION region;
		ZIL_UINT8 *image;
		ZIL_UINT8 *screen;
		ZIL_UINT8 *backup;
	};
	
	UI_DISPLAY_IMAGE displayImage[ZIL_MAXIMAGES];

	UI_DISPLAY(int isText, const ZIL_ICHAR *operatingSystem = ZIL_NULLP(ZIL_ICHAR),
		const ZIL_ICHAR *windowingSystem = ZIL_NULLP(ZIL_ICHAR));
	int RegionInitialize(UI_REGION &region, const UI_REGION *clipRegion,
		int left, int top, int right, int bottom);
public:
	// Character encoding
	static ZIL_ICHAR codeSet[15];
};

// --------------------------------------------------------------------------
// ----- UI_BGI_DISPLAY -----------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSDOS) && (defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__))
class ZIL_EXPORT_CLASS UI_BGI_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct BGIFONT
	{
		int font;
		int charSize;
		int multX, divX;
		int multY, divY;
		int maxWidth, maxHeight;
	};
	typedef char BGIPATTERN[8];

	static UI_PATH *searchPath;
	static BGIFONT fontTable[ZIL_MAXFONTS];
	static BGIPATTERN patternTable[ZIL_MAXPATTERNS];

	UI_BGI_DISPLAY(int driver = 0, int mode = 0);
	virtual ~UI_BGI_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		ZIL_SCREENID oldScreenID = ID_SCREEN, ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID
		screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line, int width, int height, ZIL_UINT8 *image);

protected:
	int maxColors;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;

	void SetFont(ZIL_LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// --------------------------------------------------------------------------
// ----- UI_META_DISPLAY ----------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSDOS)
class UI_META_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct METAFONT
	{
		char *fontPtr;
		char *fontName;
		int size;
		int maxWidth, maxHeight;
	};
	struct METAPATTERN
	{
		short patWidth;
		short patHeight;
		char patAlign;
		char patFlags;
		short patRowBytes;
		short patBits;
		short patPlanes;
		char patData[32];
	};

	static METAFONT fontTable[ZIL_MAXFONTS];
	static METAPATTERN patternTable[ZIL_MAXPATTERNS];

	UI_META_DISPLAY(int maxWidth = 640, int maxHeight = 480, int colorsMax = 16);
	virtual ~UI_META_DISPLAY(void);

	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		ZIL_SCREENID oldScreenID = ID_SCREEN, ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID
		screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line, int width, int height, ZIL_UINT8 *image);


protected:
	int maxColors;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;

	void SetFont(ZIL_LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// --------------------------------------------------------------------------
// ----- UI_GRAPHICS_DISPLAY ------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSDOS)
class UI_GRAPHICS_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct GRAPHICSFONT
	{
		int font;
		int maxWidth, maxHeight;
	};
	typedef unsigned char GRAPHICSPATTERN[10];

	static UI_PATH *searchPath;
	static GRAPHICSFONT fontTable[ZIL_MAXFONTS];
	static GRAPHICSPATTERN patternTable[ZIL_MAXPATTERNS];

	UI_GRAPHICS_DISPLAY(int mode = 4);
	virtual ~UI_GRAPHICS_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
   	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn,
		int newLine, ZIL_SCREENID oldScreenID = ID_SCREEN,
		ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line, int width, int height, ZIL_UINT8 *image);

protected:
	int maxColors;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;
	int _fillAttributes;
	int _outlineAttributes;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;

	void SetFont(ZIL_LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// --------------------------------------------------------------------------
// ----- UI_XT_DISPLAY ---------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_XT)

#	if defined(ZIL_MOTIF)
#		define UI_MOTIF_DISPLAY UI_XT_DISPLAY
#	endif

class ZIL_EXPORT_CLASS UI_XT_DISPLAY : public UI_DISPLAY
{
public:
	struct XFONT
	{
#	if defined(ZIL_MOTIF)
		XmFontList fontList;
#	endif
#	if defined(ZIL_UNICODE)
		XFontSet fontSet;
#	else
		XFontStruct *fontStruct;
#	endif
	};

	static XFONT fontTable[ZIL_MAXFONTS];

	UI_XT_DISPLAY(int *argc = ZIL_NULLP(int), char **argv = ZIL_NULLP(char *),
		char *appClass = "ZincApp",
		XrmOptionDescList options = ZIL_NULLP(XrmOptionDescRec),
		Cardinal numOptions = 0, String *fallbackResources = ZIL_NULLP(String));
	virtual ~UI_XT_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn,
		int newLine, ZIL_SCREENID oldScreenID = ID_SCREEN,
		ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);
};
#endif

// --------------------------------------------------------------------------
// ----- UI_MSC_DISPLAY -----------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSDOS) && defined(_MSC_VER)
class ZIL_EXPORT_CLASS UI_MSC_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct ZIL_EXPORT_CLASS MSCFONT
	{
		char *typeFace;
		char *options;
	};
	typedef unsigned char MSCPATTERN[8];

	static UI_PATH *searchPath;
	static MSCFONT fontTable[ZIL_MAXFONTS];
	static MSCPATTERN patternTable[ZIL_MAXPATTERNS];

	UI_MSC_DISPLAY(int mode = 0);
	virtual ~UI_MSC_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1,
		int _xor = FALSE, const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		ZIL_SCREENID oldScreenID = ID_SCREEN, ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line, int width, int height, ZIL_UINT8 *image);

protected:
	int maxColors;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;

	void SetFont(ZIL_LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// --------------------------------------------------------------------------
// ----- UI_MSWINDOWS_DISPLAY -----------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSWINDOWS)
class ZIL_EXPORT_CLASS UI_MSWINDOWS_DISPLAY : public UI_DISPLAY
{
public:
	static HDC hDC;
	static PAINTSTRUCT paintStruct;
	static HFONT fontTable[ZIL_MAXFONTS];
	static WORD patternTable[ZIL_MAXPATTERNS][8];

	UI_MSWINDOWS_DISPLAY(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow);
	virtual ~UI_MSWINDOWS_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn,
		int newLine, ZIL_SCREENID oldScreenID = ID_SCREEN,
		ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

protected:
	int maxColors;
};
#endif

// --------------------------------------------------------------------------
// ----- UI_OS2_DISPLAY -----------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_OS2)
class ZIL_EXPORT_CLASS UI_OS2_DISPLAY : public UI_DISPLAY
{
public:
	static HPS hps;
	static FONTMETRICS fontTable[ZIL_MAXFONTS];

	UI_OS2_DISPLAY(void);
	virtual ~UI_OS2_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn,
		int newLine, ZIL_SCREENID oldScreenID = ID_SCREEN,
		ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

protected:
	int maxColors;

	void SetFont(ZIL_LOGICAL_FONT logicalFont);
};
#endif

// --------------------------------------------------------------------------
// ----- UI_TEXT_DISPLAY ----------------------------------------------------
// --------------------------------------------------------------------------

// --- TDM_MODE ---
typedef int TDM_MODE;
const TDM_MODE TDM_NONE						= 0xFF;
const TDM_MODE TDM_AUTO						= -1;
const TDM_MODE TDM_BW_25x40 				= 0;
const TDM_MODE TDM_25x40 					= 1;
const TDM_MODE TDM_BW_25x80 				= 2;
const TDM_MODE TDM_25x80					= 3;
const TDM_MODE TDM_MONO_25x80				= 7;
const TDM_MODE TDM_43x80					= 64;

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
class ZIL_EXPORT_CLASS UI_TEXT_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	TDM_MODE mode;

	UI_TEXT_DISPLAY(TDM_MODE _mode = TDM_AUTO);
	virtual ~UI_TEXT_DISPLAY(void);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground = 1);
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn,
		int newLine, ZIL_SCREENID oldScreenID = ID_SCREEN,
		ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line, int width, int height, ZIL_UINT8 *image);

protected:
	ZIL_SCREEN_CELL *_screen;
	int _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;

	// I18N member variables and functions.
public:
	static ZIL_ICHAR _tCornerUL[];
	static ZIL_ICHAR _tCornerUR[];
	static ZIL_ICHAR _tCornerLL[];
	static ZIL_ICHAR _tCornerLR[];
	static ZIL_ICHAR _tHorizontal[];
	static ZIL_ICHAR _tVertical[];
};
#endif

// --------------------------------------------------------------------------
// ----- UI_WCC_DISPLAY -----------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSDOS) && defined(__WATCOMC__)
class ZIL_EXPORT_CLASS UI_WCC_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct ZIL_EXPORT_CLASS WCCFONT
	{
		char *typeFace;
		char *options;
	};
	typedef unsigned char WCCPATTERN[8];

	static UI_PATH *searchPath;
	static WCCFONT fontTable[ZIL_MAXFONTS];
	static WCCPATTERN patternTable[ZIL_MAXPATTERNS];

	UI_WCC_DISPLAY(int mode = 0);
	virtual ~UI_WCC_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		ZIL_SCREENID oldScreenID = ID_SCREEN, ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line, int width, int height, ZIL_UINT8 *image);

protected:
	int maxColors;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;

	void SetFont(ZIL_LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// --------------------------------------------------------------------------
// ----- UI_MACINTOSH_DISPLAY -----------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MACINTOSH)

class ZIL_EXPORT_CLASS UI_WINDOW_OBJECT;

class ZIL_EXPORT_CLASS UI_MACINTOSH_DISPLAY : public UI_DISPLAY
{
public:
	// Forward declaration of classes used by UI_MACINTOSH_DISPLAY.
	friend class ZIL_EXPORT_CLASS UI_WINDOW_OBJECT;

	struct MACFONT
	{
		short		font;
		Style		face;
		short		mode;
		short		size;
		FontInfo	fInfo;
	};

	static MACFONT fontTable[ZIL_MAXFONTS];
	static CTabHandle pixMapColorTable;

	static Boolean usedMenuID[lastMenuID + 1];
	static MenuHandle appleMenu;
	static UI_WINDOW_OBJECT *appleAbout;
	static UI_WINDOW_OBJECT *menuBar;

	static RgnHandle clipSaveRgn;

	UI_MACINTOSH_DISPLAY(void);
	virtual ~UI_MACINTOSH_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION), ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	static RGBColor MapRGBColor(ZIL_COLOR fromColor);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		ZIL_SCREENID oldScreenID = ID_SCREEN, ZIL_SCREENID newScreenID = ID_SCREEN);
	virtual void Text(ZIL_SCREENID screenID, int left, int top, const ZIL_ICHAR *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID screenID = ID_SCREEN,
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID screenID = ID_SCREEN,
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

protected:
	int maxColors;
};
#endif

// --------------------------------------------------------------------------
// ----- UI_NEXTSTEP_DISPLAY -----------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_NEXTSTEP)
class ZIL_EXPORT_CLASS UI_NEXTSTEP_DISPLAY : public UI_DISPLAY
{
public:
	// Foreward declaration of classes used by UI_NEXTSTEP_DISPLAY.
	friend class ZIL_EXPORT_CLASS UI_WINDOW_OBJECT;

	struct NEXTFONT
	{
		id font;
	};

	static NEXTFONT fontTable[ZIL_MAXFONTS];
	static UI_WINDOW_OBJECT *menuBar;
	UI_NEXTSTEP_DISPLAY(void);
	virtual ~UI_NEXTSTEP_DISPLAY(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION), ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void BitmapHandleToArray(ZIL_SCREENID screenID, ZIL_BITMAP_HANDLE colorBitmap,
		ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight,
		ZIL_UINT8 **bitmapArray);
	virtual void DestroyBitmapHandle(ZIL_SCREENID screenID,
		ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap);
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
		int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
		ZIL_ICON_HANDLE *icon);
	virtual void IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
		int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray);
	virtual void DestroyIconHandle(ZIL_SCREENID screenID,  ZIL_ICON_HANDLE *icon);
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void MapNSColor(const UI_PALETTE *palette, int foreground, NXColor *nextColor);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RectangleXORDiff(const UI_REGION &oldRegion,
		const UI_REGION &newRegion, ZIL_SCREENID screenID = ID_SCREEN,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		ZIL_SCREENID oldScreenID = ID_SCREEN, ZIL_SCREENID newScreenID = ID_SCREEN);
virtual void RegionDefine(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual void Text(ZIL_SCREENID screenID, int left, int top, const ZIL_ICHAR *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID screenID = ID_SCREEN,
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID screenID = ID_SCREEN,
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

protected:
	int maxColors;
};
#endif

// --------------------------------------------------------------------------
// ----- UI_PRINTER ---------------------------------------------------------
// --------------------------------------------------------------------------

typedef int ZIL_PRINTER_MODE;
const ZIL_PRINTER_MODE PRM_DEFAULT        = 0;
const ZIL_PRINTER_MODE PRM_POSTSCRIPT     = 1;
const ZIL_PRINTER_MODE PRM_LASER          = 2;
const ZIL_PRINTER_MODE PRM_DOTMATRIX9     = 3;
const ZIL_PRINTER_MODE PRM_DOTMATRIX24    = 4;
const ZIL_PRINTER_MODE PRM_LANDSCAPE		= 5;

class ZIL_EXPORT_CLASS UI_PRINTER : public UI_DISPLAY
{
public:
	ZIL_PRINTER_MODE printerMode;
	UI_DISPLAY *display;

	struct ZIL_EXPORT_CLASS POSTSCRIPTFONT
	{
		char *typeFace;
		short pointSize;
	};
	static POSTSCRIPTFONT psFontTable[ZIL_MAXFONTS];

#if defined(ZIL_MSDOS)
	struct PRINTERFONT
	{
		short symbolset;
		short spacing;
		short pitch;
		short height;
		short style;
		short weight;
		short family;
	};
	static PRINTERFONT fontTable[ZIL_MAXFONTS];
	int lPort;
#elif defined(ZIL_MSWINDOWS)
	HDC hdc;
	static HFONT fontTable[ZIL_MAXFONTS];
#elif defined(ZIL_OS2)
	HDC hdc;
	static FONTMETRICS fontTable[ZIL_MAXFONTS];
#elif defined(ZIL_MACINTOSH)
	GrafPtr displayPort;
	TPPrPort printerPort;
	THPrint printJob;
	struct PRINTERFONT
	{
		short		font;
		Style		face;
		short		mode;
		short		size;
		FontInfo	fInfo;
	};
	static PRINTERFONT fontTable[ZIL_MAXFONTS];
#elif defined(ZIL_NEXTSTEP)
	struct NEXTFONT
	{
		id font;
	};
	static NEXTFONT fontTable[ZIL_MAXFONTS];
#endif

	UI_PRINTER(UI_DISPLAY *_display = ZIL_NULLP(UI_DISPLAY));
	virtual ~UI_PRINTER(void);
	virtual void Bitmap(ZIL_SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
		const UI_PALETTE *palette = ZIL_NULLP(UI_PALETTE),
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_BITMAP_HANDLE *colorBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE),
		ZIL_BITMAP_HANDLE *monoBitmap = ZIL_NULLP(ZIL_BITMAP_HANDLE));
	virtual void Ellipse(ZIL_SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Line(ZIL_SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual ZIL_COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(ZIL_SCREENID screenID, int numPoints,
		const int *polygonPoints, const UI_PALETTE *palette,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	void Rectangle(ZIL_SCREENID screenID, const UI_REGION &region,
		const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION))
		{ Rectangle(screenID, region.left, region.top, region.right,
		  region.bottom, palette, width, fill, _xor, clipRegion); }
	virtual void Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1,
		int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION));
	virtual void Text(ZIL_SCREENID screenID, int left, int top,
		const ZIL_ICHAR *text, const UI_PALETTE *palette, int length = -1,
		int fill = TRUE, int _xor = FALSE,
		const UI_REGION *clipRegion = ZIL_NULLP(UI_REGION),
		ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const ZIL_ICHAR *string,
		ZIL_SCREENID screenID = ID_SCREEN, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);
	int VirtualGet(ZIL_SCREENID screenID, const UI_REGION &region)
		{ return (VirtualGet(screenID, region.left, region.top, region.right, region.bottom)); }
	virtual int VirtualGet(ZIL_SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(ZIL_SCREENID screenID);

	// New routines for Printer.
	virtual int  BeginPrintJob(ZIL_PRINTER_MODE = PRM_DEFAULT, char *_fileName = ZIL_NULLP(char));
	virtual void EndPrintJob(void);
	virtual void BeginPage(void);
	virtual void BeginPage(int left, int top, int right, int bottom,
		int orientation = 0, int resolution = 0);
	virtual void EndPage(void);
	virtual void ScreenDump(ZIL_SCREENID screenID, UI_REGION &region,
		ZIL_PRINTER_MODE = PRM_DEFAULT, char *_fileName = ZIL_NULLP(char));
	virtual void TextFormat(ZIL_SCREENID, int x, int y, ZIL_ICHAR *text,
		const UI_PALETTE *palette, int width = -1, ZIL_LOGICAL_FONT font = FNT_DIALOG_FONT);

private:
	static char *psColorTable[16];
#if defined(ZIL_MSDOS)
	char *tmpFileName;
	char port[5];
	int pageWidth, pageHeight, resolution;
	int currentPrintheadPositionX;	// dot matrix support
	int currentPrintheadPositionY;	// dot matrix support
	int dotMatrixLinesPerPage;    	// dot matrix support
	int dotMatrixColumnsPerPage;  	// dot matrix support
	int dotMatrixBottomMargin;  // dot matrix support
	int GetDMByte(char *byte, int bytePos, int width);
	int MonoColor(ZIL_COLOR color);
	void PrintLine(int x1, int y1, int x2, int y2, int color);
	void SetBit(char &byteValue, int bitPos, int color);
	void SetPoint(int column, int line, int color);
	void PrintDotMatrixString(char *);
#elif defined(ZIL_POSIX)
	char *systemCommand;
	char *tmpName;
#elif defined(ZIL_OS2)
	HPS hps;
#endif
};

#if defined(ZIL_OLD_DEFS)
// --- Version 3.6 compatibility ---
#define SCREENID							ZIL_SCREENID
#define HBITMAP								ZIL_BITMAP_HANDLE
#define SCREEN_CELL							ZIL_SCREEN_CELL
#define HICON								ZIL_ICON_HANDLE
#define COLOR								ZIL_COLOR
#define MAX_LOGICAL_FONTS					ZIL_MAXFONTS
#define LOGICAL_FONT						ZIL_LOGICAL_FONT
#define IGNORE_UNDERSCORE					FNT_IGNORE_UNDERSCORE
#define MAX_DISPLAY_IMAGES					ZIL_MAXIMAGES
#define IMAGE_TYPE							ZIL_IMAGE_TYPE
#define MOUSE_IMAGE							IMT_MOUSE_IMAGE
#define CURSOR_IMAGE						IMT_CURSOR_IMAGE
#define MAX_LOGICAL_PATTERNS				ZIL_MAXPATTERNS
#define IMAGE_LOGICAL_PATTERN				ZIL_LOGICAL_PATTERN
#define MOTIFFONT							XFONT
#define UI_MOTIF_DISPLAY					UI_XT_DISPLAY

// --- Version 2.0 and 1.0 compatibility ---
#	define _backgroundPalette				UI_DISPLAY::backgroundPalette
#	define _xorPalette						UI_DISPLAY::xorPalette
#	define _colorMap						UI_DISPLAY::colorMap
#	define UI_DOS_TEXT_DISPLAY				UI_TEXT_DISPLAY
#	define UI_DOS_BGI_DISPLAY				UI_BGI_DISPLAY
#endif

#endif
