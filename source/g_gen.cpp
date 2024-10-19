//	Zinc Interface Library - UI_GEN.CPP
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


#include "ui_gen.hpp"

void z_gen_dummy(void) { } // Bug fix for broken linkers.

// ----- UI_ELEMENT ---------------------------------------------------------

// ----- UI_LIST ------------------------------------------------------------

// ----- UI_LIST_BLOCK ------------------------------------------------------

// ----- ZIL_INTERNATIONAL --------------------------------------------------

ZIL_STORAGE_READ_ONLY *ZIL_I18N::defaultStorage 	= ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
static ZIL_ICHAR ZIL_FARDATA _i18nName[] 			= { 'i','1','8','n','.','d','a','t',0 };
ZIL_ICHAR *ZIL_I18N::i18nName = _i18nName;

ZIL_INT32 ZIL_INTERNATIONAL::minutesWestGMT 		= 7 * 60; // MST

ZIL_ICHAR ZIL_INTERNATIONAL::zincLangString[] 		= { 'Z','I','N','C','_','L','A','N','G',0 };
ZIL_ICHAR ZIL_INTERNATIONAL::_blankString[] 		= { 0 };
ZIL_ICHAR ZIL_INTERNATIONAL::_errorString[]			= { 'E','r','r','o','r', 0 };
ZIL_ICHAR ZIL_INTERNATIONAL::machineName[32];

#if defined(ZIL_MACINTOSH)
OSType ZIL_INTERNATIONAL::fileCreator				= 'Anon';
OSType ZIL_INTERNATIONAL::fileType					= 'TEXT';
#endif

// ----- UI_PATH_ELEMENT & UI_PATH ------------------------------------------

ZIL_ICHAR ZIL_FARDATA __pathString[] 				= { 'P','A','T','H', 0 };
ZIL_ICHAR *UI_PATH::_pathString 		= __pathString;
ZIL_ICHAR ZIL_FARDATA __zincPathString[] 			= { 'Z','I','N','C','_','P','A','T','H', 0 };
ZIL_ICHAR *UI_PATH::_zincPathString	= __zincPathString;

// ----- ZIL_STORAGE_OBJECT & ZIL_STORAGE -------------------------------------

#if defined(ZIL_DOSEXTENDED)
int ZIL_STORAGE_READ_ONLY::cacheSize = 64;
#else
int ZIL_STORAGE_READ_ONLY::cacheSize = 32;
#endif
UI_PATH *ZIL_STORAGE_READ_ONLY::searchPath = ZIL_NULLP(UI_PATH);

static ZIL_ICHAR ZIL_FARDATA __parentDirectory[]	= { '.','.', 0 };
ZIL_ICHAR *_parentDirectory 				= __parentDirectory;
static ZIL_ICHAR ZIL_FARDATA __currentDirectory[] 	= { '.', 0 };
ZIL_ICHAR *_currentDirectory 				= __currentDirectory;
static ZIL_ICHAR ZIL_FARDATA __rootStorageDirectory[] = { ZIL_DIRECTORY_SEPARATOR, 0 };
ZIL_ICHAR *_rootStorageDirectory 			= __rootStorageDirectory;

static ZIL_ICHAR ZIL_FARDATA __bitmapDirectory[]	= { '~','U','I','_','B','I','T','M','A','P', 0 };
ZIL_ICHAR *_bitmapDirectory 		= __bitmapDirectory;
static ZIL_ICHAR ZIL_FARDATA __cppDirectory[] 		= { '~','U','I','_','C','P','P', 0 };
ZIL_ICHAR *_cppDirectory 			= __cppDirectory;
static ZIL_ICHAR ZIL_FARDATA __helpDirectory[]		= { '~','U','I','_','H','E','L','P', 0 };
ZIL_ICHAR *_helpDirectory 			= __helpDirectory;
static ZIL_ICHAR ZIL_FARDATA __hppDirectory[] 		= { '~','U','I','_','H','P','P', 0 };
ZIL_ICHAR *_hppDirectory 			= __hppDirectory;
static ZIL_ICHAR ZIL_FARDATA __iconDirectory[]		= { '~','U','I','_','I','C','O','N', 0 };
ZIL_ICHAR *_iconDirectory 			= __iconDirectory;
static ZIL_ICHAR ZIL_FARDATA __messageDirectory[] 	= { '~','U','I','_','M','E','S','S','A','G','E',0 };
ZIL_ICHAR *_messageDirectory 		= __messageDirectory;
static ZIL_ICHAR ZIL_FARDATA __mouseDirectory[] 	= { '~','U','I','_','M','O','U','S','E',0 };
ZIL_ICHAR *_mouseDirectory 			= __mouseDirectory;
static ZIL_ICHAR ZIL_FARDATA __windowDirectory[]	= { '~','U','I','W','_','W','I','N','D','O','W', 0 };
ZIL_ICHAR *_windowDirectory 		= __windowDirectory;

ZIL_ICHAR ZIL_FARDATA __zilBak[] 					= { '.','b','k','?', 0 };
ZIL_ICHAR *_zilBak 					= __zilBak;
ZIL_ICHAR ZIL_FARDATA __zilExtName[]				= { '.','d','a','t', 0 };
ZIL_ICHAR *_zilExtName 				= __zilExtName;

#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
extern "C" {
	// Keep Borland library from bringing in stuff for tmpnam().
	int _tmpnum = 0;
}
#endif

// ----- ZIL_UTIME ----------------------------------------------------------

ZIL_ICHAR ZIL_UTIME::_dayName[]		= { 'Z','I','L','_','D','A','Y',0 };
ZIL_ICHAR ZIL_UTIME::_monthName[]	= { 'Z','I','L','_','M','O','N','T','H',0 };
ZIL_ICHAR ZIL_UTIME::_timeName[]	= { 'Z','I','L','_','T','I','M','E',0 };
int ZIL_UTIME::defaultInitialized = FALSE;
