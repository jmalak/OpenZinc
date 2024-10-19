//	Zinc Interface Library - O_ERROR.CPP
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


#define INCL_WINDIALOGS			// OS/2 message windows.
#include <stdio.h>
#include <stdarg.h>
#include "ui_win.hpp"

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

UIS_STATUS UI_ERROR_SYSTEM::ErrorMessage(UI_WINDOW_MANAGER *windowManager,
	UIS_STATUS errorStatus, ZIL_ICHAR *message, ZIL_ICHAR *titleMessage)
{
	// Beep the system.
	Beep();
	if (!windowManager || !message)
		return (WOS_NO_STATUS);

	ULONG flStyle = FlagSet(errorStatus, WOS_INVALID) ? MB_OKCANCEL : MB_OK;
	flStyle |= MB_ICONEXCLAMATION | MB_SYSTEMMODAL;
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
	char *TEXT_ = ZIL_INTERNATIONAL::MapText(message, ZIL_NULLP(char), FALSE);
	char *mappedTitle = titleMessage ? ZIL_INTERNATIONAL::MapText(titleMessage) : ZIL_NULLP(char);
#	define TITLE mappedTitle
#else
#	define TEXT_ message
#	define TITLE titleMessage
#endif
	int response = WinMessageBox(HWND_DESKTOP, windowManager->First() ?
		windowManager->First()->screenID : HWND_DESKTOP, (PSZ)TEXT_,
		(PSZ)TITLE, 0, flStyle);
#	if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
	if (mappedTitle)
		delete mappedTitle;
#	endif
	return ((response == MBID_OK) ? WOS_INVALID : WOS_NO_STATUS);
}
