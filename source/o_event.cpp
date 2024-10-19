//	Zinc Interface Library - O_EVENT.CPP
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


#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_WINSTDDRAG			// OS/2 drag messages and flags.
#include "ui_evt.hpp"

// ----- UI_EVENT -----------------------------------------------------------

UI_EVENT::UI_EVENT(EVENT_TYPE _type, HWND hwnd, ULONG msg, MPARAM mp1,
	MPARAM mp2) : type(_type)
{
	// Set up the OS/2 event message.
	rawCode = modifiers = 0;
	message.hwnd = hwnd;
	message.msg = msg;
	message.mp1 = mp1;
	message.mp2 = mp2;

	// Check for mouse or keyboard information.
	if (msg == WM_CHAR)
	{
		USHORT keyFlags = LOWORD(mp1);
		key.shiftState = (FlagSet(keyFlags, KC_CTRL) ? S_CTRL : 0) |
			(FlagSet(keyFlags, KC_ALT) ? S_ALT : 0) |
			(FlagSet(keyFlags, KC_SHIFT) ? S_SHIFT : 0);
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char buffer[3];
		buffer[0] = FlagSet(keyFlags, KC_CHAR) ? LOWORD(mp2) : 0;
#	if defined (ZIL_UNICODE)
/* START BLOCK COMMENT
**			if (mblen(buffer) > 1)
**			{
**				buffer[1] = 
**			}
END BLOCK COMMENT */
#	endif
		key.value = ZIL_INTERNATIONAL::UnMapChar(buffer);

#else
		key.value = FlagSet(keyFlags, KC_CHAR) ? LOWORD(mp2) : 0;
#endif
		rawCode = LONGFROMMP(mp1) & 0xFF000000 | key.shiftState;
		modifiers = keyFlags & KC_KEYUP;
	}
	else if ((msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) ||
		(msg >= WM_DRAGFIRST && msg <= WM_DRAGLAST))
	{
		POINTL point;
		point.x = (short)LOWORD(mp1);
		point.y = (short)HIWORD(mp1);

		HWND hwndTrue = hwnd;
		if (WinQueryWindowUShort(hwnd, QWS_ID) == FID_CLIENT)
			hwndTrue = WinQueryWindow(hwnd, QW_PARENT);
		HWND parentHwnd = WinQueryWindow(hwndTrue, QW_PARENT);
		if (!parentHwnd)
			parentHwnd = HWND_DESKTOP;

		SWP swp;
		SWP trueRegionSwp;
		SWP parentSwp;
		WinQueryWindowPos(hwnd, &swp);
		WinQueryWindowPos(hwndTrue, &trueRegionSwp);
		WinQueryWindowPos(parentHwnd, &parentSwp);

		position.column = point.x + trueRegionSwp.x;
		position.line = parentSwp.cy - trueRegionSwp.cy - trueRegionSwp.y + swp.cy - point.y - 1;

		static ZIL_RAW_CODE buttonState = 0;
		static ZIL_SCREENID button1ID = 0;
		static ZIL_SCREENID button2ID = 0;
		static ZIL_SCREENID button3ID = 0;
		if (msg == WM_BUTTON1DOWN || msg == WM_BUTTON1DBLCLK)
		{
			rawCode = M_LEFT | M_LEFT_CHANGE;
			buttonState |= M_LEFT;
			if (!button1ID || !WinIsChild(button1ID, hwnd))
				button1ID = hwnd;
		}
		else if (msg == WM_BUTTON1UP)
		{
			rawCode = M_LEFT_CHANGE;
			buttonState &= ~M_LEFT;
			button1ID = 0;
		}
		else if (msg == WM_BUTTON2DOWN || msg == WM_BUTTON2DBLCLK)
		{
			rawCode = M_RIGHT | M_RIGHT_CHANGE;
			buttonState |= M_RIGHT;
			if (!button2ID || !WinIsChild(button2ID, hwnd))
				button2ID = hwnd;
		}
		else if (msg == WM_BUTTON2UP)
		{
			rawCode = M_RIGHT_CHANGE;
			buttonState &= ~M_RIGHT;
			button2ID = 0;
		}
		else if (msg == WM_BUTTON3DOWN || msg == WM_BUTTON3DBLCLK)
		{
			rawCode = M_MIDDLE | M_MIDDLE_CHANGE;
			buttonState |= M_MIDDLE;
			if (!button3ID || !WinIsChild(button3ID, hwnd))
				button3ID = hwnd;
		}
		else if (msg == WM_BUTTON3UP)
		{
			rawCode = M_MIDDLE_CHANGE;
			buttonState &= ~M_MIDDLE;
			button3ID = 0;
		}
		else if (msg == DM_DROP)
		{
			buttonState = 0;
			button1ID = button2ID = button3ID = 0;
		}
		else if (msg == WM_MOUSEMOVE)
		{
			rawCode = buttonState;
			if (!button1ID || !WinIsChild(button1ID, hwnd))
				rawCode &= ~M_LEFT;
			if (!button2ID || !WinIsChild(button2ID, hwnd))
				rawCode &= ~M_RIGHT;
			if (!button3ID || !WinIsChild(button3ID, hwnd))
				rawCode &= ~M_MIDDLE;
		}
	}
}

EVENT_TYPE UI_EVENT::InputType(void) const
{
	// Check for mouse or keyboard information.
	if (type == E_OS2)
	{
		if (message.msg == WM_CHAR)
			return (E_KEY);
		else if (message.msg >= WM_MOUSEFIRST && message.msg <= WM_MOUSELAST)
			return (E_MOUSE);
	}
	return (type);
}

