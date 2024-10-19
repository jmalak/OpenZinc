//	Zinc Interface Library - O_TEXT.CPP
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


#define INCL_WINMLE				// OS/2 multi-line messages and flags.
#include "ui_win.hpp"

// ----- UIW_TEXT -----------------------------------------------------------

#if defined(ZIL_EDIT)
static MRESULT EXPENTRY EditCallback(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch (msg)
	{
	case WM_BUTTON1DOWN:
	case WM_BUTTON1DBLCLK:
	case WM_MOUSEMOVE:
		{
		POINTL point;
		point.x = LOWORD(mp1);
		point.y = HIWORD(mp1);
		HWND hwndParent = WinQueryWindow(hwnd, QW_PARENT);
		WinMapWindowPoints(hwnd, hwndParent, &point, 1);
		WinPostMsg(hwndParent, msg, MPFROM2SHORT(point.x, point.y), mp2);
		}
		return ((void *)TRUE);
	}
	FNWP *callback = (FNWP *)WinQueryWindowULong(hwnd, QWL_USER);
	return callback(hwnd, msg, mp1, mp2);
}
#endif

EVENT_TYPE UIW_TEXT::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	// OS/2 automatically displays text when return is FALSE.
	return (FALSE);
}

EVENT_TYPE UIW_TEXT::Event(const UI_EVENT &event)
{
	static PFNWP _textCallback = ZIL_NULLP(FNWP);
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		if (event.message.msg == WM_CONTROL)
		{
			if (HIWORD(event.message.mp1) == MLN_CHANGE)
				woStatus |= WOS_CHANGED;
			return (0);
		}
		// Prevent Help System Beep caused by extranious up click.
		// (Probably bug in OS/2 MLE object.)
		else if (event.message.msg == WM_BUTTON1UP && WinQueryCapture(HWND_DESKTOP) != screenID)
		   	return (0);
		else
			return (UI_WINDOW_OBJECT::Event(event));
	}

	// Check for Zinc specific messages.
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (!FlagSet(woFlags, WOF_NON_FIELD_REGION))
			flStyle |= MLS_IGNORETAB;
		if (!FlagSet(wnFlags, WNF_NO_WRAP))
			flStyle |= MLS_WORDWRAP;
		if (FlagSet(woFlags, WOF_BORDER))
			flStyle |= MLS_BORDER;
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			flStyle |= MLS_READONLY;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woStatus, WOS_EDIT_MODE))
			flStyle |= WS_DISABLED;
		break;

	case S_REGISTER_OBJECT:
		RegisterObject("UIW_TEXT", WC_MLE, &_textCallback, text);
		// Set the font.
		if (UI_OS2_DISPLAY::fontTable[font & ~FNT_IGNORE_UNDERSCORE].lMatch)
		{
			int tFont = font & ~FNT_IGNORE_UNDERSCORE;
			FATTRS drawFont;
			drawFont.usRecordLength = sizeof(FATTRS);
			drawFont.fsSelection = UI_OS2_DISPLAY::fontTable[tFont].fsSelection;
			drawFont.lMatch = UI_OS2_DISPLAY::fontTable[tFont].lMatch;
			::strcpy(drawFont.szFacename, UI_OS2_DISPLAY::fontTable[tFont].szFacename);
			drawFont.idRegistry = UI_OS2_DISPLAY::fontTable[tFont].idRegistry;
			drawFont.usCodePage = UI_OS2_DISPLAY::fontTable[tFont].usCodePage;
			drawFont.lMaxBaselineExt = UI_OS2_DISPLAY::fontTable[tFont].lMaxBaselineExt;
			drawFont.lAveCharWidth = UI_OS2_DISPLAY::fontTable[tFont].lAveCharWidth;
			drawFont.fsType = UI_OS2_DISPLAY::fontTable[tFont].fsType;
			drawFont.fsFontUse = FATTR_FONTUSE_NOMIX;
			WinSendMsg(screenID, MLM_SETFONT, (MPARAM)&drawFont, (MPARAM)0);
		}
		WinSendMsg(screenID, MLM_SETTEXTLIMIT, (MPARAM)maxLength, (MPARAM)0);
		if (FlagSet(woFlags, WOF_VIEW_ONLY) && FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
		{
			ULONG color;
			if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
				ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
				WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(color), &color);
		}

#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			ZIL_SCREENID childID = WinQueryWindow(screenID, QW_TOP);
			while (childID)
			{
				WinSetWindowULong(childID, QWL_USER, (ULONG)WinSubclassWindow(childID, EditCallback));
				childID = WinQueryWindow(childID, QW_NEXT);
			}		
		}
#endif
		break;

	case S_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woFlags, WOF_AUTO_CLEAR))
			WinSendMsg(screenID, MLM_SETSEL, (MPARAM)0, (MPARAM)0x7FFF0000);
		break;

	default:
//		ccode = UI_WINDOW_OBJECT::Event(event);
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

int UIW_TEXT::CursorOffset(int offset)
{
	if (offset > -1)
		WinSendMsg(screenID, MLM_SETSEL, (MPARAM)(IPT)offset, (MPARAM)(IPT)offset);

	IPT returnValue = (IPT)WinSendMsg(screenID, MLM_QUERYSEL, (MPARAM)MLFQS_CURSORSEL, (MPARAM)0);
	return ((int)returnValue);
}

void UIW_TEXT::GetCursorPos(UI_POSITION *position)
{
	position->column = (IPT)WinSendMsg(screenID, MLM_QUERYSEL, (MPARAM)MLFQS_CURSORSEL, (MPARAM)0) -
		(IPT)WinSendMsg(screenID, MLM_CHARFROMLINE, (MPARAM)-1, (MPARAM)0);
	position->line = (IPT)WinSendMsg(screenID, MLM_LINEFROMCHAR, (MPARAM)-1, (MPARAM)0);
}

void UIW_TEXT::SetCursorPos(const UI_POSITION &position)
{
	IPT offset = (IPT)WinSendMsg(screenID, MLM_CHARFROMLINE, (MPARAM)(LONG)position.line, (MPARAM)0);
	offset += position.column;
	WinSendMsg(screenID, MLM_SETSEL, (MPARAM)offset, (MPARAM)offset);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TEXT::OSDataGet(void)
{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
	{
		int length = (maxLength + 1 ) * sizeof(ZIL_ICHAR);
		char *tmpBuff = new char[length];
		WinQueryWindowText(screenID, length, (PSZ)tmpBuff);
		UnMapText(tmpBuff, text);
		delete tmpBuff;
	}
#else
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
		WinQueryWindowText(screenID, maxLength + 1, (PSZ)text);
#endif
}

void UIW_TEXT::OSDataSet(void)
{
	if (screenID)
	{
		WinSendMsg(screenID, MLM_SETTEXTLIMIT, (MPARAM)maxLength, (MPARAM)0);
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(text);
		WinSetWindowText(screenID, (PSZ)TEXT_);
		delete TEXT_;
#else
		WinSetWindowText(screenID, (PSZ)text);
#endif
	}
}

void UIW_TEXT::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TEXT && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
