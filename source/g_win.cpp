//	Zinc Interface Library - G_WIN.CPP
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


#define USE_RAW_KEYS
#include "ui_win.hpp"

// This is a good default stack for Zinc programs.  It may need to be
// different for some programs and can be overridden in the users program.
#if defined(__ZTC__) && !defined(DOS386) && !defined(ZIL_MACINTOSH)
extern "C" {
unsigned _stack = 20480;
};
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
extern unsigned _stklen = 20480;
#endif

void z_win_dummy(void) { } // Bug fix for broken linkers.

// ----- UI_PALETTE_MAP -----------------------------------------------------

// ----- UI_EVENT_MAP -------------------------------------------------------

#if defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS)
static UI_EVENT_MAP ZIL_FARDATA hotKeyMapTable[] =
{
	// ID_WINDOW_OBJECT.
	{ ID_WINDOW_OBJECT, 	HOT_KEY_SYSTEM,		E_KEY, 		ALT_PERIOD },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_SYSTEM,		E_KEY, 		ALT_SPACE },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		ALT_WHITE_MINUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		ALT_GRAY_MINUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		ALT_F9 },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		ALT_WHITE_PLUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		ALT_GRAY_PLUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		ALT_F10 },

	{ ID_WINDOW_OBJECT, 	'q',				E_KEY, 		ALT_Q },
	{ ID_WINDOW_OBJECT, 	'w',				E_KEY, 		ALT_W },
	{ ID_WINDOW_OBJECT, 	'e',				E_KEY, 		ALT_E },
	{ ID_WINDOW_OBJECT, 	'r',				E_KEY, 		ALT_R },
	{ ID_WINDOW_OBJECT, 	't',				E_KEY, 		ALT_T },
	{ ID_WINDOW_OBJECT, 	'y',				E_KEY, 		ALT_Y },
	{ ID_WINDOW_OBJECT, 	'u',				E_KEY, 		ALT_U },
	{ ID_WINDOW_OBJECT, 	'i',				E_KEY, 		ALT_I },
	{ ID_WINDOW_OBJECT, 	'o',				E_KEY, 		ALT_O },
	{ ID_WINDOW_OBJECT, 	'p',				E_KEY, 		ALT_P },

	{ ID_WINDOW_OBJECT, 	'a',				E_KEY, 		ALT_A },
	{ ID_WINDOW_OBJECT, 	's',				E_KEY, 		ALT_S },
	{ ID_WINDOW_OBJECT, 	'd',				E_KEY, 		ALT_D },
	{ ID_WINDOW_OBJECT, 	'f',				E_KEY, 		ALT_F },
	{ ID_WINDOW_OBJECT, 	'g',				E_KEY, 		ALT_G },
	{ ID_WINDOW_OBJECT, 	'h',				E_KEY, 		ALT_H },
	{ ID_WINDOW_OBJECT, 	'j',				E_KEY, 		ALT_J },
	{ ID_WINDOW_OBJECT, 	'k',				E_KEY, 		ALT_K },
	{ ID_WINDOW_OBJECT, 	'l',				E_KEY, 		ALT_L },

	{ ID_WINDOW_OBJECT, 	'z',				E_KEY, 		ALT_Z },
	{ ID_WINDOW_OBJECT, 	'x',				E_KEY, 		ALT_X },
	{ ID_WINDOW_OBJECT, 	'c',				E_KEY, 		ALT_C },
	{ ID_WINDOW_OBJECT, 	'v',				E_KEY, 		ALT_V },
	{ ID_WINDOW_OBJECT, 	'b',				E_KEY, 		ALT_B },
	{ ID_WINDOW_OBJECT, 	'n',				E_KEY, 		ALT_N },
	{ ID_WINDOW_OBJECT, 	'm',				E_KEY, 		ALT_M },

#if defined(ZIL_MSWINDOWS)
	// Extended chars.
	{ ID_WINDOW_OBJECT, 	'\340',				E_KEY, 		ALT_A_GRAVE },
	{ ID_WINDOW_OBJECT, 	'\341',				E_KEY, 		ALT_A_ACUTE },
	{ ID_WINDOW_OBJECT, 	'\342',				E_KEY, 		ALT_A_CIRCUMFLEX },
	{ ID_WINDOW_OBJECT, 	'\343',				E_KEY, 		ALT_A_TILDE },
	{ ID_WINDOW_OBJECT, 	'\344',				E_KEY, 		ALT_A_DIARESIS },
	{ ID_WINDOW_OBJECT, 	'\345',				E_KEY, 		ALT_A_RING },
	{ ID_WINDOW_OBJECT, 	'\346',				E_KEY, 		ALT_A_E },
	{ ID_WINDOW_OBJECT, 	'\347',				E_KEY, 		ALT_C_CEDILLA },
	{ ID_WINDOW_OBJECT, 	'\350',				E_KEY, 		ALT_E_GRAVE },
	{ ID_WINDOW_OBJECT, 	'\351',				E_KEY, 		ALT_E_ACUTE },
	{ ID_WINDOW_OBJECT, 	'\352',				E_KEY, 		ALT_E_CIRCUMFLEX },
	{ ID_WINDOW_OBJECT, 	'\353',				E_KEY, 		ALT_E_DIARESIS },
	{ ID_WINDOW_OBJECT, 	'\354',				E_KEY, 		ALT_I_GRAVE },
	{ ID_WINDOW_OBJECT, 	'\355',				E_KEY, 		ALT_I_ACUTE },
	{ ID_WINDOW_OBJECT, 	'\356',				E_KEY, 		ALT_I_CIRCUMFLEX },
	{ ID_WINDOW_OBJECT, 	'\357',				E_KEY, 		ALT_I_DIARESIS },
	{ ID_WINDOW_OBJECT, 	'\360',				E_KEY, 		ALT_ETH },
	{ ID_WINDOW_OBJECT, 	'\361',				E_KEY, 		ALT_N_TILDE },
	{ ID_WINDOW_OBJECT, 	'\362',				E_KEY, 		ALT_O_GRAVE },
	{ ID_WINDOW_OBJECT, 	'\363',				E_KEY, 		ALT_O_ACUTE },
	{ ID_WINDOW_OBJECT, 	'\364',				E_KEY, 		ALT_O_CIRCUMFLEX },
	{ ID_WINDOW_OBJECT, 	'\365',				E_KEY, 		ALT_O_TILDE },
	{ ID_WINDOW_OBJECT, 	'\366',				E_KEY, 		ALT_O_DIARESIS },
	{ ID_WINDOW_OBJECT, 	'\370',				E_KEY, 		ALT_O_SLASH },
	{ ID_WINDOW_OBJECT, 	'\371',				E_KEY, 		ALT_U_GRAVE },
	{ ID_WINDOW_OBJECT, 	'\372',				E_KEY, 		ALT_U_ACUTE },
	{ ID_WINDOW_OBJECT, 	'\373',				E_KEY, 		ALT_U_CIRCUMFLEX },
	{ ID_WINDOW_OBJECT, 	'\374',				E_KEY, 		ALT_U_DIARESIS },
	{ ID_WINDOW_OBJECT, 	'\375',				E_KEY, 		ALT_Y_ACUTE },
	{ ID_WINDOW_OBJECT, 	'\376',				E_KEY, 		ALT_THORN },
#endif

	{ ID_WINDOW_OBJECT, 	'1',				E_KEY, 		ALT_1 },
	{ ID_WINDOW_OBJECT, 	'2',				E_KEY, 		ALT_2 },
	{ ID_WINDOW_OBJECT, 	'3',				E_KEY, 		ALT_3 },
	{ ID_WINDOW_OBJECT, 	'4',				E_KEY, 		ALT_4 },
	{ ID_WINDOW_OBJECT, 	'5',				E_KEY, 		ALT_5 },
	{ ID_WINDOW_OBJECT, 	'6',				E_KEY, 		ALT_6 },
	{ ID_WINDOW_OBJECT, 	'7',				E_KEY, 		ALT_7 },
	{ ID_WINDOW_OBJECT, 	'8',				E_KEY, 		ALT_8 },
	{ ID_WINDOW_OBJECT, 	'9',				E_KEY, 		ALT_9 },
	{ ID_WINDOW_OBJECT, 	'0',				E_KEY, 		ALT_0 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_XT) || defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP)
static UI_EVENT_MAP ZIL_FARDATA hotKeyMapTable[] =
{
	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_CURSES)
static UI_EVENT_MAP ZIL_FARDATA hotKeyMapTable[] =
{
	// ID_WINDOW_OBJECT.
	{ ID_WINDOW_OBJECT, 	HOT_KEY_SYSTEM,		E_KEY, 		'.', S_ALT },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_SYSTEM,		E_KEY, 		' ', S_ALT },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		'-', S_ALT },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		F9, S_ALT },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		'+', S_ALT },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		F10, S_ALT },

	{ ID_WINDOW_OBJECT, 	'q',				E_KEY, 		'q', S_ALT },
	{ ID_WINDOW_OBJECT, 	'w',				E_KEY, 		'w', S_ALT },
	{ ID_WINDOW_OBJECT, 	'e',				E_KEY, 		'e', S_ALT },
	{ ID_WINDOW_OBJECT, 	'r',				E_KEY, 		'r', S_ALT },
	{ ID_WINDOW_OBJECT, 	't',				E_KEY, 		't', S_ALT },
	{ ID_WINDOW_OBJECT, 	'y',				E_KEY, 		'y', S_ALT },
	{ ID_WINDOW_OBJECT, 	'u',				E_KEY, 		'u', S_ALT },
	{ ID_WINDOW_OBJECT, 	'i',				E_KEY, 		'i', S_ALT },
	{ ID_WINDOW_OBJECT, 	'o',				E_KEY, 		'o', S_ALT },
	{ ID_WINDOW_OBJECT, 	'p',				E_KEY, 		'p', S_ALT },

	{ ID_WINDOW_OBJECT, 	'a',				E_KEY, 		'a', S_ALT },
	{ ID_WINDOW_OBJECT, 	's',				E_KEY, 		's', S_ALT },
	{ ID_WINDOW_OBJECT, 	'd',				E_KEY, 		'd', S_ALT },
	{ ID_WINDOW_OBJECT, 	'f',				E_KEY, 		'f', S_ALT },
	{ ID_WINDOW_OBJECT, 	'g',				E_KEY, 		'g', S_ALT },
	{ ID_WINDOW_OBJECT, 	'h',				E_KEY, 		'h', S_ALT },
	{ ID_WINDOW_OBJECT, 	'j',				E_KEY, 		'j', S_ALT },
	{ ID_WINDOW_OBJECT, 	'k',				E_KEY, 		'k', S_ALT },
	{ ID_WINDOW_OBJECT, 	'l',				E_KEY, 		'l', S_ALT },

	{ ID_WINDOW_OBJECT, 	'z',				E_KEY, 		'z', S_ALT },
	{ ID_WINDOW_OBJECT, 	'x',				E_KEY, 		'x', S_ALT },
	{ ID_WINDOW_OBJECT, 	'c',				E_KEY, 		'c', S_ALT },
	{ ID_WINDOW_OBJECT, 	'v',				E_KEY, 		'v', S_ALT },
	{ ID_WINDOW_OBJECT, 	'b',				E_KEY, 		'b', S_ALT },
	{ ID_WINDOW_OBJECT, 	'n',				E_KEY, 		'n', S_ALT },
	{ ID_WINDOW_OBJECT, 	'm',				E_KEY, 		'm', S_ALT },

	{ ID_WINDOW_OBJECT, 	'1',				E_KEY, 		'1', S_ALT },
	{ ID_WINDOW_OBJECT, 	'2',				E_KEY, 		'2', S_ALT },
	{ ID_WINDOW_OBJECT, 	'3',				E_KEY, 		'3', S_ALT },
	{ ID_WINDOW_OBJECT, 	'4',				E_KEY, 		'4', S_ALT },
	{ ID_WINDOW_OBJECT, 	'5',				E_KEY, 		'5', S_ALT },
	{ ID_WINDOW_OBJECT, 	'6',				E_KEY, 		'6', S_ALT },
	{ ID_WINDOW_OBJECT, 	'7',				E_KEY, 		'7', S_ALT },
	{ ID_WINDOW_OBJECT, 	'8',				E_KEY, 		'8', S_ALT },
	{ ID_WINDOW_OBJECT, 	'9',				E_KEY, 		'9', S_ALT },
	{ ID_WINDOW_OBJECT, 	'0',				E_KEY, 		'0', S_ALT },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

UI_EVENT_MAP *_hotKeyMapTable = hotKeyMapTable;

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

#if defined(ZIL_XT)
int UI_WINDOW_OBJECT::repeatRate = 75;
int UI_WINDOW_OBJECT::delayLength = 500;
#else
int UI_WINDOW_OBJECT::repeatRate = 7;
int UI_WINDOW_OBJECT::delayLength = 50;
#endif

int UI_WINDOW_OBJECT::doubleClickRate = 50;
WOS_STATUS UI_WINDOW_OBJECT::defaultStatus = WOS_NO_STATUS;
UI_DISPLAY *UI_WINDOW_OBJECT::display = ZIL_NULLP(UI_DISPLAY);
UI_EVENT_MANAGER *UI_WINDOW_OBJECT::eventManager = ZIL_NULLP(UI_EVENT_MANAGER);
UI_WINDOW_MANAGER *UI_WINDOW_OBJECT::windowManager = ZIL_NULLP(UI_WINDOW_MANAGER);
UI_ERROR_STUB *UI_WINDOW_OBJECT::errorSystem = ZIL_NULLP(UI_ERROR_STUB);
UI_HELP_STUB *UI_WINDOW_OBJECT::helpSystem = ZIL_NULLP(UI_HELP_STUB);
ZIL_STORAGE_READ_ONLY *UI_WINDOW_OBJECT::defaultStorage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	ZIL_ICHAR *UI_WINDOW_OBJECT::pasteBuffer = ZIL_NULLP(ZIL_ICHAR);
	int UI_WINDOW_OBJECT::pasteLength = 0;
#endif

static ZIL_ICHAR ZIL_FARDATA __objectIDName[] 		= { 'O','B','J','E','C','T','I','D', 0 };
ZIL_ICHAR *_objectIDName 			= __objectIDName;
static ZIL_ICHAR ZIL_FARDATA __userFunctionName[] 	= { 'U','S','E','R','_','F','U','N','C','T','I','O','N', 0 };
ZIL_ICHAR *_userFunctionName 		= __userFunctionName;
static ZIL_ICHAR ZIL_FARDATA __compareFunctionName[]= { 'C','O','M','P','A','R','E','_','F','U','N','C','T','I','O','N', 0 };
ZIL_ICHAR *_compareFunctionName 	= __compareFunctionName;
static ZIL_ICHAR ZIL_FARDATA __idFormat[] 			= { 'c','o','n','s','t',' ','Z','I','L','_','N','U','M','B','E','R','I','D',' ','%','-','3','2','s',' ','=',' ','0','x','%','0','4','X',';', 0 };
ZIL_ICHAR *_idFormat 				= __idFormat;
static ZIL_ICHAR ZIL_FARDATA __genericFieldID[] 		= { 'F','I','E','L','D','_', 0 };
ZIL_ICHAR *_genericFieldID 			= __genericFieldID;

ZIL_ICHAR UI_WINDOW_OBJECT::_className[] = { 'U','I','_','W','I','N','D','O','W','_','O','B','J','E','C','T', 0 };

// ----- UIW_WINDOW ---------------------------------------------------------

ZIL_ICHAR UIW_WINDOW::_className[] = { 'U','I','W','_','W','I','N','D','O','W', 0 };
int UIW_WINDOW::defaultInitialized = FALSE;

// ----- UI_WINDOW_MANAGER --------------------------------------------------

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
ZIL_SCREENID UI_WINDOW_MANAGER::currentScreenID = ID_SCREEN;
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_XT)
ZIL_SCREENID UI_WINDOW_MANAGER::taskID = 0;
#endif
#if (ZIL_MOTIF >= 1002)
Atom UI_WINDOW_MANAGER::ZINC_DND;
#endif

// ----- UIW_BORDER ---------------------------------------------------------

#if defined(ZIL_MACINTOSH)
	int UIW_BORDER::width = 16;
#elif defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
	int UIW_BORDER::width = 4;
#elif defined(ZIL_MOTIF_STYLE)
	int UIW_BORDER::width = 7;
#else
	int UIW_BORDER::width = 0;
#endif

ZIL_ICHAR UIW_BORDER::_className[] = { 'U','I','W','_','B','O','R','D','E','R', 0 };

// ----- UIW_PROMPT ---------------------------------------------------------

ZIL_ICHAR UIW_PROMPT::_className[] = { 'U','I','W','_','P','R','O','M','P','T', 0 };

// ----- UIW_BUTTON ---------------------------------------------------------

ZIL_ICHAR UIW_BUTTON::_className[] = { 'U','I','W','_','B','U','T','T','O','N', 0 };
int UIW_BUTTON::defaultInitialized = FALSE;

// ----- UIW_IMAGE ----------------------------------------------------------

ZIL_ICHAR UIW_IMAGE::_className[] = { 'U','I','W','_','I','M','A','G','E', 0 };

// ----- UIW_TITLE ----------------------------------------------------------

ZIL_ICHAR UIW_TITLE::_className[] = { 'U','I','W','_','T','I','T','L','E', 0 };

// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------

ZIL_ICHAR UIW_MAXIMIZE_BUTTON::_className[] = { 'U','I','W','_','M','A','X','I','M','I','Z','E','_','B','U','T','T','O','N', 0 };
int UIW_MAXIMIZE_BUTTON::defaultInitialized = FALSE;

// ----- UIW_MINIMIZE_BUTTON ------------------------------------------------

ZIL_ICHAR UIW_MINIMIZE_BUTTON::_className[] = { 'U','I','W','_','M','I','N','I','M','I','Z','E','_','B','U','T','T','O','N', 0 };
int UIW_MINIMIZE_BUTTON::defaultInitialized = FALSE;

// ----- UIW_ICON -----------------------------------------------------------

ZIL_ICHAR UIW_ICON::_applicationIconName[]	= { 'A','P','P','L','I','C','A','T','I','O','N', 0 };
ZIL_ICHAR UIW_ICON::_asteriskIconName[] 	= { 'A','S','T','E','R','I','S','K', 0 };
ZIL_ICHAR UIW_ICON::_exclamationIconName[]	= { 'E','X','C','L','A','M','A','T','I','O','N', 0 };
ZIL_ICHAR UIW_ICON::_handIconName[] 		= { 'H','A','N','D', 0 };
ZIL_ICHAR UIW_ICON::_questionIconName[] 	= { 'Q','U','E','S','T','I','O','N', 0 };

ZIL_ICHAR UIW_ICON::_className[] = { 'U','I','W','_','I','C','O','N', 0 };

// ----- UIW_POP_UP_MENU ----------------------------------------------------

ZIL_ICHAR UIW_POP_UP_MENU::_className[] = { 'U','I','W','_','P','O','P','_','U','P','_','M','E','N','U', 0 };

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

ZIL_ICHAR UIW_POP_UP_ITEM::_className[] = { 'U','I','W','_','P','O','P','_','U','P','_','I','T','E','M', 0 };
int UIW_POP_UP_ITEM::defaultInitialized = FALSE;

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

ZIL_ICHAR UIW_PULL_DOWN_MENU::_className[] = { 'U','I','W','_','P','U','L','L','_','D','O','W','N','_','M','E','N','U', 0 };

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

ZIL_ICHAR UIW_PULL_DOWN_ITEM::_className[] = { 'U','I','W','_','P','U','L','L','_','D','O','W','N','_','I','T','E','M', 0 };

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

ZIL_ICHAR UIW_SYSTEM_BUTTON::_className[] = { 'U','I','W','_','S','Y','S','T','E','M','_','B','U','T','T','O','N', 0 };
int UIW_SYSTEM_BUTTON::defaultInitialized = FALSE;

// ----- UIW_STRING ---------------------------------------------------------

ZIL_ICHAR UIW_STRING::_className[] = { 'U','I','W','_','S','T','R','I','N','G', 0 };

// ----- UIW_DATE -----------------------------------------------------------

ZIL_ICHAR UIW_DATE::_className[] = { 'U','I','W','_','D','A','T','E', 0 };
int UIW_DATE::defaultInitialized = FALSE;

// ----- UIW_FORMATTED_STRING -----------------------------------------------

ZIL_ICHAR UIW_FORMATTED_STRING::_className[] = { 'U','I','W','_','F','O','R','M','A','T','T','E','D','_','S','T','R','I','N','G', 0 };

// ----- UIW_BIGNUM ---------------------------------------------------------

ZIL_ICHAR UIW_BIGNUM::_className[] = { 'U','I','W','_','B','I','G','N','U','M', 0 };
int UIW_BIGNUM::defaultInitialized = FALSE;

// ----- UIW_INTEGER --------------------------------------------------------

ZIL_ICHAR UIW_INTEGER::_className[] = { 'U','I','W','_','I','N','T','E','G','E','R', 0 };
int UIW_INTEGER::defaultInitialized = FALSE;

// ----- UIW_REAL -----------------------------------------------------------

ZIL_ICHAR UIW_REAL::_className[] = { 'U','I','W','_','R','E','A','L', 0 };
int UIW_REAL::defaultInitialized = FALSE;

// ----- UIW_TIME -----------------------------------------------------------

ZIL_ICHAR UIW_TIME::_className[] = { 'U','I','W','_','T','I','M','E', 0 };
int UIW_TIME::defaultInitialized = FALSE;

// ----- UIW_TEXT -----------------------------------------------------------

ZIL_ICHAR UIW_TEXT::_className[] = { 'U','I','W','_','T','E','X','T', 0 };

// ----- UIW_GROUP ----------------------------------------------------------

ZIL_ICHAR UIW_GROUP::_className[] = { 'U','I','W','_','G','R','O','U','P', 0 };

// ----- UIW_VT_LIST --------------------------------------------------------

ZIL_ICHAR UIW_VT_LIST::_className[] = { 'U','I','W','_','V','T','_','L','I','S','T', 0 };

// ----- UIW_HZ_LIST ---------------------------------------------------------

ZIL_ICHAR UIW_HZ_LIST::_className[] = { 'U','I','W','_','H','Z','_','L','I','S','T', 0 };

// ----- UIW_COMBO_BOX ------------------------------------------------------

ZIL_ICHAR UIW_COMBO_BOX::_className[] = { 'U','I','W','_','C','O','M','B','O','_','B','O','X', 0 };
#if defined(ZIL_MOTIF)
Cursor UIW_COMBO_BOX::popupCursor = 0;
#endif

// ----- UIW_SPIN_CONTROL ---------------------------------------------------
	
ZIL_ICHAR UIW_SPIN_CONTROL::_className[] = { 'U','I','W','_','S','P','I','N','_','C','O','N','T','R','O','L', 0 };
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_XT) || (defined(ZIL_MSWINDOWS) && !defined(ZIL_WIN32)) || defined(ZIL_NEXTSTEP)
int UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::defaultInitialized = FALSE;
#endif

// ----- UIW_SCROLL_BAR -----------------------------------------------------

ZIL_ICHAR UIW_SCROLL_BAR::_className[] = { 'U','I','W','_','S','C','R','O','L','L','_','B','A','R', 0 };

// ----- UIW_TABLE ----------------------------------------------------------

ZIL_ICHAR UIW_TABLE::_className[] = { 'U','I','W','_','T','A','B','L','E', 0 };
ZIL_ICHAR UIW_TABLE_RECORD::_className[] = { 'U','I','W','_','T','A','B','L','E','_','R','E','C','O','R','D', 0 };
ZIL_ICHAR UIW_TABLE_HEADER::_className[] = { 'U','I','W','_','T','A','B','L','E','_','H','E','A','D','E','R', 0 };

// ----- UIW_TOOL_BAR -------------------------------------------------------

ZIL_ICHAR UIW_TOOL_BAR::_className[] = { 'U','I','W','_','T','O','O','L','_','B','A','R', 0 };

// ----- UIW_STATUS_BAR -----------------------------------------------------

ZIL_ICHAR UIW_STATUS_BAR::_className[] = { 'U','I','W','_','S','T','A','T','U','S','_','B','A','R', 0 };

// ----- UI_GEOMETRY_MANAGER ------------------------------------------------

ZIL_ICHAR UI_GEOMETRY_MANAGER::_className[] = { 'U','I','_','G','E','O','M','E','T','R','Y','_','M','A','N','A','G','E','R', 0 };

// ----- UIW_NOTEBOOK -------------------------------------------------------

int UIW_NOTEBOOK::borderWidth = 2;
int UIW_NOTEBOOK::shadowWidth = 3;
ZIL_ICHAR UIW_NOTEBOOK::_className[] = { 'U','I','W','_','N','O','T','E','B','O','O','K', 0 };

// ----- UI_APPLICATION -----------------------------------------------------

static ZIL_ICHAR ZIL_FARDATA __textmode1[] = { '/','t', 0 };
static ZIL_ICHAR ZIL_FARDATA __textmode2[] = { '-','t', 0 };
static ZIL_ICHAR ZIL_FARDATA __textmode3[] = { '/','t','e','x','t', 0 };
static ZIL_ICHAR ZIL_FARDATA __textmode4[] = { '-','t','e','x','t', 0 };


static ZIL_LANGUAGE_ELEMENT ZIL_FARDATA __textSwitches[] =
{
	{ __textmode1, 0xFFFF, { 0 } },
	{ __textmode2, 0xFFFF, { 0 } },
	{ __textmode3, 0xFFFF, { 0 } },
	{ __textmode4, 0xFFFF, { 0 } },
	{ 0, 0, { 0 } }
};
ZIL_LANGUAGE_ELEMENT *UI_APPLICATION::_textSwitches = __textSwitches;

static ZIL_ICHAR ZIL_FARDATA __svgamode[] = { '/','s','v','g','a', 0 };

static ZIL_LANGUAGE_ELEMENT ZIL_FARDATA __graphicSwitches[] =
{
	{ __svgamode, 0x104, { 0 } },
	{ 0, 0, { 0 } }
};
ZIL_LANGUAGE_ELEMENT *UI_APPLICATION::_graphicSwitches = __graphicSwitches;

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

ZIL_ICHAR UI_ERROR_SYSTEM::_className[] = { 'U','I','_','E','R','R','O','R','_','S','Y','S','T','E','M',0 };
int UI_ERROR_SYSTEM::defaultInitialized = FALSE;

// ----- UI_HELP_SYSTEM -----------------------------------------------------

ZIL_ICHAR UI_HELP_SYSTEM::_className[] = { 'U','I','_','H','E','L','P','_','S','Y','S','T','E','M',0 };
int UI_HELP_SYSTEM::defaultInitialized = FALSE;

#if defined(ZIL_3x_COMPAT)
	DTF_FLAGS UIW_DATE::rangeFlags = DTF_US_FORMAT;
	NMF_FLAGS UIW_BIGNUM::rangeFlags = NMF_NO_FLAGS;
	TMF_FLAGS UIW_TIME::rangeFlags = TMF_SECONDS | TMF_HUNDREDTHS;
#endif

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
// ----- UIW_COMBO_BUTTON ---------------------------------------------------

int UIW_COMBO_BOX::UIW_COMBO_BUTTON::defaultInitialized = FALSE;

// ----- UIW_COMBO_BUTTON ---------------------------------------------------

int UIW_SCROLL_BAR::UIW_SCROLL_BUTTON::defaultInitialized = FALSE;

#endif

// ----- ZAF_MESSAGE_WINDOW--------------------------------------------------

int ZAF_MESSAGE_WINDOW::defaultInitialized = FALSE;
ZIL_ICHAR ZAF_MESSAGE_WINDOW::_className[] = { 'Z','A','F','_','M','E','S','S','A','G','E','_','W','I','N','D','O','W', 0 };

// ----- UI_CONSTRAINT ------------------------------------------------------
ZIL_ICHAR UI_CONSTRAINT::_className[] =
{ 'U','I','_','C','O','N','S','T','R','A','I','N','T',0 };

//------ UI_RELATIVE_CONSTRAINT ---------------------------------------------
ZIL_ICHAR UI_RELATIVE_CONSTRAINT::_className[] =
{ 'U','I','_','R','E','L','A','T','I','V','E','_','C','O','N','S','T','R','A','I','N','T',0 };

// ----- UI_DIMENSION_CONSTRAINT --------------------------------------------
ZIL_ICHAR UI_DIMENSION_CONSTRAINT::_className[] =
{ 'U','I','_','D','I','M','E','N','S','I','O','N','_','C','O','N','S','T','R','A','I','N','T',0 };

// ----- UI_ATTACHMENT ------------------------------------------------------
ZIL_ICHAR UI_ATTACHMENT::_className[] =
{ 'U','I','_','A','T','T','A','C','H','M','E','N','T',0 };

