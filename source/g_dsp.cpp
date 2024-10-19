//	Zinc Interface Library - G_DSP.CPP
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


#include "ui_dsp.hpp"
#if defined(ZIL_NEXTSTEP)
extern "C"
{
#	include "nextc.hpp"
}
#endif
void z_dsp_dummy(void) { } // Bug fix for broken linkers.

// --------------------------------------------------------------------------
// ----- ZIL_MSDOS ----------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSDOS)
#	if defined(ZIL_SHADOW_BORDER) && defined(ZIL_MSWINDOWS_STYLE)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(BLACK, LIGHTBLUE), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, BLACK, CYAN, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
	PTN_SOLID_FILL, WHITE, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __markedTextPalette =
{
	' ', attrib(BLACK, CYAN), attrib(MONO_HIGH, MONO_BLACK),
	PTN_SOLID_FILL, BLACK, CYAN, BW_WHITE, BW_BLACK, GS_BLACK, GS_GRAY
};

#	elif defined(ZIL_STANDARD_BORDER) && defined(ZIL_MSWINDOWS_STYLE)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(BLACK, LIGHTBLUE), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, BLACK, CYAN, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
	PTN_SOLID_FILL, WHITE, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __markedTextPalette =
{
	' ', attrib(BLACK, CYAN), attrib(MONO_HIGH, MONO_BLACK),
	PTN_SOLID_FILL, BLACK, CYAN, BW_WHITE, BW_BLACK, GS_BLACK, GS_GRAY
};

#	elif defined(ZIL_MSWINDOWS_STYLE) && defined(ZIL_3D_BORDER)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(LIGHTGRAY, LIGHTGRAY), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, BLACK, CYAN, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
	PTN_SOLID_FILL, WHITE, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __markedTextPalette =
{
	' ', attrib(CYAN, BLACK), attrib(MONO_HIGH, MONO_BLACK),
	PTN_SOLID_FILL, BLACK, CYAN, BW_WHITE, BW_BLACK, GS_BLACK, GS_GRAY
};

#	elif defined(ZIL_OS2_STYLE)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(BLACK, LIGHTBLUE), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, CYAN, LIGHTGRAY, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
	PTN_SOLID_FILL, WHITE, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __markedTextPalette =
{
	' ', attrib(BLACK, CYAN), attrib(MONO_HIGH, MONO_BLACK),
	PTN_SOLID_FILL, WHITE, DARKGRAY, BW_WHITE, BW_BLACK, GS_BLACK, GS_GRAY
};

#	elif defined(ZIL_MOTIF_STYLE)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(BLACK, LIGHTBLUE), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, CYAN, BLUE, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
	PTN_SOLID_FILL, LIGHTGRAY, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __markedTextPalette =
{
	' ', attrib(BLACK, CYAN), attrib(MONO_HIGH, MONO_BLACK),
	PTN_SOLID_FILL, LIGHTGRAY, BLACK, BW_WHITE, BW_BLACK, GS_BLACK, GS_GRAY
};

#	else
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(BLACK, LIGHTBLUE), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, BLACK, CYAN, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
	PTN_SOLID_FILL, WHITE, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __markedTextPalette =
{
	' ', attrib(BLACK, CYAN), attrib(MONO_HIGH, MONO_BLACK),
	PTN_SOLID_FILL, BLACK, CYAN, BW_WHITE, BW_BLACK, GS_BLACK, GS_GRAY
};

#	endif

static UI_PALETTE ZIL_FARDATA __colorMap[16] =
{
	{ ' ', attrib(BLACK, BLACK), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(BLUE, BLUE), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, BLUE, BLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(GREEN, GREEN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, GREEN, GREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(CYAN, CYAN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, CYAN, CYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(RED, RED), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, RED, RED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(MAGENTA, MAGENTA), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, MAGENTA, MAGENTA, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(BROWN, BROWN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, BROWN, BROWN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTGRAY, LIGHTGRAY), attrib(MONO_BLACK, MONO_NORMAL),
		PTN_SOLID_FILL, LIGHTGRAY, LIGHTGRAY, BW_WHITE, BW_WHITE, GS_GRAY, GS_GRAY },
	{ ' ', attrib(DARKGRAY, DARKGRAY), attrib(MONO_DIM, MONO_DIM),
		PTN_SOLID_FILL, DARKGRAY, DARKGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTBLUE, LIGHTBLUE), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, LIGHTBLUE, LIGHTBLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTGREEN, LIGHTGREEN), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, LIGHTGREEN, LIGHTGREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTCYAN, LIGHTCYAN), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, LIGHTCYAN, LIGHTCYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTRED, LIGHTRED), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, LIGHTRED, LIGHTRED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTMAGENTA, LIGHTMAGENTA), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, LIGHTMAGENTA, LIGHTMAGENTA, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(YELLOW, YELLOW), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, YELLOW, YELLOW, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(WHITE, WHITE), attrib(MONO_NORMAL, MONO_NORMAL),
		PTN_SOLID_FILL, WHITE, WHITE, BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE }
};

UI_PALETTE *UI_DISPLAY::markPalette = &__markedTextPalette;

#endif

// --------------------------------------------------------------------------
// ----- ZIL_MSWINDOWS ------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSWINDOWS)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(BLACK, LIGHTBLUE), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, BLACK, CYAN, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
	PTN_SOLID_FILL, WHITE, WHITE, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __colorMap[16] =
{
	{ ' ', attrib(BLACK, BLACK), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_BLACK, RGB_BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(BLUE, BLUE), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_BLUE, RGB_BLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(GREEN, GREEN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_GREEN, RGB_GREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(CYAN, CYAN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_CYAN, RGB_CYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(RED, RED), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_RED, RGB_RED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(MAGENTA, MAGENTA), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_MAGENTA, RGB_MAGENTA, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(BROWN, BROWN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_BROWN, RGB_BROWN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTGRAY, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK),
		PTN_RGB_COLOR, RGB_LIGHTGRAY, RGB_LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(DARKGRAY, DARKGRAY), attrib(MONO_DIM, MONO_DIM),
		PTN_RGB_COLOR, RGB_DARKGRAY, RGB_DARKGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTBLUE, LIGHTBLUE), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_RGB_COLOR, RGB_LIGHTBLUE, RGB_LIGHTBLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTGREEN, LIGHTGREEN), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_RGB_COLOR, RGB_LIGHTGREEN, RGB_LIGHTGREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTCYAN, LIGHTCYAN), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_RGB_COLOR, RGB_LIGHTCYAN, RGB_LIGHTCYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTRED, LIGHTRED), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_RGB_COLOR, RGB_LIGHTRED, RGB_LIGHTRED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTMAGENTA, LIGHTMAGENTA), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_RGB_COLOR, RGB_LIGHTMAGENTA, RGB_LIGHTMAGENTA, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(YELLOW, YELLOW), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_RGB_COLOR, RGB_YELLOW, RGB_YELLOW, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(WHITE, WHITE), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_RGB_COLOR, RGB_WHITE, RGB_WHITE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK }
};

HFONT UI_PRINTER::fontTable[ZIL_MAXFONTS] =
{
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
};
#endif

// --------------------------------------------------------------------------
// ----- ZIL_OS2 ------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_OS2)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(BLACK, LIGHTBLUE), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_BACKGROUND_FILL, BLACK, CYAN, BW_BLACK, BW_WHITE, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	'\260', attrib(WHITE, WHITE), attrib(MONO_HIGH, MONO_HIGH),
	PTN_SOLID_FILL, WHITE, WHITE, BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE
};

static UI_PALETTE ZIL_FARDATA __colorMap[16] =
{
	{ ' ', attrib(BLACK, BLACK), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_BLACK, CLR_BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(BLUE, BLUE), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_DARKBLUE, CLR_DARKBLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(GREEN, GREEN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_DARKGREEN, CLR_DARKGREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(CYAN, CYAN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_DARKCYAN, CLR_DARKCYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(RED, RED), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_DARKRED, CLR_DARKRED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(MAGENTA, MAGENTA), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_DARKPINK, CLR_DARKPINK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(BROWN, BROWN), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_BROWN, CLR_BROWN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTGRAY, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_PALEGRAY, CLR_PALEGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(DARKGRAY, DARKGRAY), attrib(MONO_DIM, MONO_DIM),
		PTN_SYSTEM_COLOR, CLR_DARKGRAY, CLR_DARKGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTBLUE, LIGHTBLUE), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_BLUE, CLR_BLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTGREEN, LIGHTGREEN), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_GREEN, CLR_GREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTCYAN, LIGHTCYAN), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_CYAN, CLR_CYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTRED, LIGHTRED), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_RED, CLR_RED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(LIGHTMAGENTA, LIGHTMAGENTA), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_PINK, CLR_PINK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(YELLOW, YELLOW), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_YELLOW, CLR_YELLOW, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', attrib(WHITE, WHITE), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SYSTEM_COLOR, CLR_WHITE, CLR_WHITE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK }
};

FONTMETRICS UI_PRINTER::fontTable[ZIL_MAXFONTS];
#endif

// --------------------------------------------------------------------------
// ----- ZIL_XT -------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined (ZIL_XT)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', 0, 0, PTN_BACKGROUND_FILL, RGB_CYAN,	RGB_CYAN,	0, 0, 0, 0
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	' ', 0, 0, PTN_RGB_COLOR, 0x7F7F7F,	0x7F7F7F,	0, 0, 0, 0
};

static UI_PALETTE ZIL_FARDATA __colorMap[16] =
{
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_BLACK,	RGB_BLACK,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_BLUE,	RGB_BLUE,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_GREEN,	RGB_GREEN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_CYAN,	RGB_CYAN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_RED,	RGB_RED,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_MAGENTA,	RGB_MAGENTA,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_BROWN,	RGB_BROWN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTGRAY,	RGB_LIGHTGRAY,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_DARKGRAY,	RGB_DARKGRAY,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTBLUE,	RGB_LIGHTBLUE,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTGREEN,	RGB_LIGHTGREEN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTCYAN,	RGB_LIGHTCYAN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTRED,	RGB_LIGHTRED,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTMAGENTA,	RGB_LIGHTMAGENTA,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_YELLOW,	RGB_YELLOW,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_WHITE,	RGB_WHITE,	0, 0, 0, 0 }
};

UI_XT_DISPLAY::XFONT UI_XT_DISPLAY::fontTable[ZIL_MAXFONTS] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

// --------------------------------------------------------------------------
// ----- ZIL_CURSES ---------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_CURSES)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', attrib(CYAN, CYAN), A_NORMAL,
//	' ', attrib(CYAN, CYAN), A_REVERSE,
	PTN_BACKGROUND_FILL, BLACK, CYAN, 0, 0, 0, 0
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	' ', attrib(WHITE, BLACK), attrib(MONO_BLACK, MONO_NORMAL),
	PTN_SOLID_FILL, WHITE, BLACK, 0, 0, 0, 0
};

static UI_PALETTE ZIL_FARDATA __markedTextPalette =
{
	' ', attrib(BLACK, CYAN), A_REVERSE,
	PTN_SOLID_FILL, BLACK, CYAN, BW_WHITE, BW_BLACK, GS_BLACK, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __colorMap[8] =
{
	{ ' ', attrib(BLACK, BLACK), A_NORMAL,
		PTN_SOLID_FILL, BLACK, BLACK, 0, 0, 0, 0 },
	{ ' ', attrib(BLUE, BLUE), A_NORMAL,
		PTN_SOLID_FILL, BLUE, BLUE, 0, 0, 0, 0 },
	{ ' ', attrib(GREEN, GREEN), A_NORMAL,
		PTN_SOLID_FILL, GREEN, GREEN, 0, 0, 0, 0 },
	{ ' ', attrib(CYAN, CYAN), A_NORMAL,
		PTN_SOLID_FILL, CYAN, CYAN, 0, 0, 0, 0 },
	{ ' ', attrib(RED, RED), A_NORMAL,
		PTN_SOLID_FILL, RED, RED, 0, 0, 0, 0 },
	{ ' ', attrib(MAGENTA, MAGENTA), A_NORMAL,
		PTN_SOLID_FILL, MAGENTA, MAGENTA, 0, 0, 0, 0 },
	{ ' ', attrib(YELLOW, YELLOW), A_NORMAL,
		PTN_SOLID_FILL, YELLOW, YELLOW, 0, 0, 0, 0 },
	{ ' ', attrib(WHITE, WHITE), A_NORMAL,
		PTN_SOLID_FILL, WHITE, WHITE, 0, 0, 0, 0 },
};

UI_PALETTE *UI_DISPLAY::markPalette = &__markedTextPalette;

#endif

// --------------------------------------------------------------------------
// ----- ZIL_MACINTOSH ------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MACINTOSH)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', 0, 0,
		PTN_BACKGROUND_FILL, BLACK, CYAN, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	' ', 0, 0,
		PTN_INTERLEAVE_FILL, BLACK, BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK
};

static UI_PALETTE ZIL_FARDATA __colorMap[16] =
{
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_BLACK, RGB_BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_BLUE, RGB_BLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_GREEN, RGB_GREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_CYAN, RGB_CYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_RED, RGB_RED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_MAGENTA, RGB_MAGENTA, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_BROWN, RGB_BROWN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_LIGHTGRAY, RGB_LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_DARKGRAY, RGB_DARKGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_LIGHTBLUE, RGB_LIGHTBLUE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_LIGHTGREEN, RGB_LIGHTGREEN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_LIGHTCYAN, RGB_LIGHTCYAN, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_LIGHTRED, RGB_LIGHTRED, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_LIGHTMAGENTA, RGB_LIGHTMAGENTA, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_YELLOW, RGB_YELLOW, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK },
	{ ' ', 0, 0,
		PTN_RGB_COLOR, RGB_WHITE, RGB_WHITE, BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE }
};
#endif

// --------------------------------------------------------------------------
// ----- ZIL_NEXTSTEP ------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_NEXTSTEP)
static UI_PALETTE ZIL_FARDATA __backgroundPalette =
{
	' ', 0, 0, PTN_BACKGROUND_FILL, RGB_CYAN,	RGB_CYAN,	0, 0, 0, 0
};

static UI_PALETTE ZIL_FARDATA __xorPalette =
{
	' ', 0, 0, PTN_RGB_COLOR, 0x7F7F7F,	0x7F7F7F,	0, 0, 0, 0
};

static UI_PALETTE ZIL_FARDATA __colorMap[16] =
{
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_BLACK,	RGB_BLACK,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_BLUE,	RGB_BLUE,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_GREEN,	RGB_GREEN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_CYAN,	RGB_CYAN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_RED,	RGB_RED,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_MAGENTA,	RGB_MAGENTA,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_BROWN,	RGB_BROWN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTGRAY,	RGB_LIGHTGRAY,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_DARKGRAY,	RGB_DARKGRAY,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTBLUE,	RGB_LIGHTBLUE,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTGREEN,	RGB_LIGHTGREEN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTCYAN,	RGB_LIGHTCYAN,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTRED,	RGB_LIGHTRED,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_LIGHTMAGENTA,	RGB_LIGHTMAGENTA,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_YELLOW,	RGB_YELLOW,	0, 0, 0, 0 },
	{ ' ', 0, 0, PTN_RGB_COLOR, RGB_WHITE,	RGB_WHITE,	0, 0, 0, 0 }
};

UI_NEXTSTEP_DISPLAY::NEXTFONT UI_NEXTSTEP_DISPLAY::fontTable[ZIL_MAXFONTS] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

UI_PRINTER::NEXTFONT UI_PRINTER::fontTable[ZIL_MAXFONTS] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#endif

UI_PALETTE *UI_DISPLAY::backgroundPalette = &__backgroundPalette;
UI_PALETTE *UI_DISPLAY::xorPalette = &__xorPalette;
UI_PALETTE *UI_DISPLAY::colorMap = __colorMap;

ZIL_ICHAR UI_DISPLAY::codeSet[15] = { 'I','S','O','8','8','5','9','-','1',0 };

// ------UI_PRINTER ---------------------------------------------------------
UI_PRINTER::POSTSCRIPTFONT UI_PRINTER::psFontTable[ZIL_MAXFONTS] =
{
	{ "/Helvetica",                   9 },	// FNT_SMALL_FONT
	{ "/Helvetica",                  11 }, // FNT_DIALOG_FONT
	{ "/Helvetica",                  13 }, // FNT_SYSTEM_FONT
	{ "/Courier-Bold",               11 },
	{ "/Courier",                    11 },
	{ "/Helvetica-Bold",             11 },
	{ "/Times-Roman",                11 },
	{ "/Times-Bold",                 11 },
	{ "/Times-Italic",               11 },
	{ "/Courier-Oblique",            11 }
};

char *UI_PRINTER::psColorTable[16] =
{
   "0",   ".1",  ".2", ".25", ".3", ".4", ".5", ".8",
   ".45", ".55", ".6", ".65", ".7", ".8", ".9", "1"
};

// ------UI_TEXT_DISPLAY ----------------------------------------------------
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)

#if defined(ZIL_UNICODE)
	ZIL_ICHAR _tLowerShadow 			= 0x2584;
	ZIL_ICHAR _tUpperShadow 			= 0x2580;
	ZIL_ICHAR _tFullShadow 				= 0x2588;
#elif defined(ZIL_CURSES)
	ZIL_ICHAR _tLowerShadow 			= 0xDC;
	ZIL_ICHAR _tUpperShadow 			= 0xDF;
	ZIL_ICHAR _tFullShadow 				= 0xDB;
#elif defined(ZIL_ISO8859_1)
	ZIL_ICHAR _tLowerShadow 			= 0x91;
	ZIL_ICHAR _tUpperShadow 			= 0x90;
	ZIL_ICHAR _tFullShadow 				= 0x92;
#else // OEM
	ZIL_ICHAR _tLowerShadow 			= 0xdc;
	ZIL_ICHAR _tUpperShadow 			= 0xdf;
	ZIL_ICHAR _tFullShadow 				= 0xdb;
#endif

#endif
