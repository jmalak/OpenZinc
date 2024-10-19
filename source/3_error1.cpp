//	Zinc Interface Library - W_ERROR.CPP
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
#include <stdarg.h>
#include "ui_win.hpp"

static ZIL_ICHAR _asterisk[] 		= { 'A','S','T','E','R','I','S','K', 0 };
static ZIL_ICHAR _exclamation[] 	= { 'E','X','C','L','A','M','A','T','I','O','N', 0 };
static ZIL_ICHAR _hand[] 			= { 'H','A','N','D', 0 };
static ZIL_ICHAR _information[]	= { 'I','N','F','O','R','M','A','T','I','O','N', 0 };
static ZIL_ICHAR _stop[] 			= { 'S','T','O','P', 0 };
static ZIL_ICHAR _question[] 		= { 'Q','U','E','S','T','I','O','N', 0 };

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

struct ICON_ITEM
{
	ZIL_ICHAR *name;
	WORD flag;
};

UIS_STATUS UI_ERROR_SYSTEM::ErrorMessage(UI_WINDOW_MANAGER *windowManager,
	UIS_STATUS errorStatus, ZIL_ICHAR *message, ZIL_ICHAR *titleMessage)
{
	// Beep the system.
	Beep();
	if (!windowManager || !message)
		return (WOS_NO_STATUS);

	WORD wType = MB_TASKMODAL;

	struct ICON_ITEM _iconTable[] = {
		{ _asterisk, 		MB_ICONASTERISK },
		{ _exclamation, 	MB_ICONEXCLAMATION },
		{ _hand, 			MB_ICONHAND },
		{ _information,		MB_ICONINFORMATION },
		{ _stop,	 		MB_ICONSTOP },
		{ _question, 		MB_ICONQUESTION },
		{ 0, 0 } };

	ZIL_ICHAR *name = myLanguage->GetMessage(MSG_ICONNAME);
	int i;
	for (i = 0;	_iconTable[i].name && ZIL_INTERNATIONAL::strcmp(name, _iconTable[i].name); i++)
		;
	wType |= _iconTable[i].flag;

	if (FlagSet(errorStatus, WOS_INVALID))
		wType |= MB_OKCANCEL;
	else
		wType |= MB_OK;
	int response = MessageBox(0, message, titleMessage ? titleMessage : ZIL_NULLP(ZIL_ICHAR), wType);
	return ((response == IDOK) ? WOS_INVALID : WOS_NO_STATUS);
}


