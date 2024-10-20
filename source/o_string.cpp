//	Zinc Interface Library - O_STRING.CPP
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


#define INCL_WINENTRYFIELDS		// OS/2 entry field messages and flags.
#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#include "ui_win.hpp"

// ----- UIW_STRING ---------------------------------------------------------

EVENT_TYPE UIW_STRING::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif
	const int STRING_OFFSET = 4;

	/* unused parameters */ (void)event;

	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);
	lastPalette = LogicalPalette(ccode);

	// Draw the string information.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE);
	if (FlagSet(woStatus, WOS_CURRENT))
		DrawFocus(screenID, trueRegion, ccode);
	region.left += STRING_OFFSET;
	if (parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
		region.right -= STRING_OFFSET;
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_STRING::Event(const UI_EVENT &event)
{
	static PFNWP _stringCallback = ZIL_NULLP(FNWP);
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_CHAR:
			{
			ccode = LogicalEvent(event);
			USHORT keyFlags = LOWORD(event.message.mp1);
			if (!FlagSet(keyFlags, KC_CHAR) && (ccode == L_UP || ccode == L_DOWN))
			{
				parent->Event(UI_EVENT(ccode));
				return (0);
			}
			else if (FlagSet(stFlags, STF_VARIABLE_NAME | STF_LOWER_CASE | STF_UPPER_CASE) &&
				FlagSet(LOWORD(event.message.mp1), KC_CHAR))
			{
				UI_EVENT tEvent = event;
				USHORT key = LOWORD(event.message.mp2);
				if (FlagSet(stFlags, STF_VARIABLE_NAME) && key == ' ')
				{
					tEvent.message.mp1 = (MPARAM)((ULONG)event.message.mp1 & ~KC_VIRTUALKEY);
					tEvent.message.mp2 = (MPARAM)'_';
				}
				if (FlagSet(stFlags, STF_LOWER_CASE) && IsUpper(key))
					tEvent.message.mp2 = (MPARAM)ToLower(key);
				else if (FlagSet(stFlags, STF_UPPER_CASE) && IsLower(key))
					tEvent.message.mp2 = (MPARAM)ToUpper(key);
				return (UI_WINDOW_OBJECT::Event(tEvent));
			}
			}
			break;

		case WM_CONTROL:
			if (FlagSet(HIWORD(event.message.mp1), EN_CHANGE))
				woStatus |= WOS_CHANGED;
			return (0);
		}
 		return (UI_WINDOW_OBJECT::Event(event));
	}

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);

		// Check the environment specific flag settings.
		flStyle |= ES_AUTOSCROLL;
		if (FlagSet(woFlags, WOF_BORDER))
			flStyle |= ES_MARGIN;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			flStyle |= ES_CENTER;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			flStyle |= ES_RIGHT;
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			flStyle |= ES_READONLY;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woStatus, WOS_EDIT_MODE))
			flStyle |= WS_DISABLED;
		if (FlagSet(stFlags, STF_PASSWORD))
			flStyle |= ES_UNREADABLE;
		break;

	case S_CHANGED:
		trueRegion.bottom = MaxValue(trueRegion.bottom, trueRegion.top + display->cellHeight -
			display->preSpace - display->postSpace - 1);
		break;

	case S_REGISTER_OBJECT:
		ENTRYFDATA controlData;
		controlData.cb = sizeof(ENTRYFDATA);
		controlData.cchEditLimit = maxLength;
		controlData.ichMinSel = controlData.ichMaxSel = 0;
		RegisterObject("UIW_STRING", WC_ENTRYFIELD, &_stringCallback, text, &controlData);
		if (FlagSet(woFlags, WOF_VIEW_ONLY) && FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
		{
			ULONG color;
			if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
				ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
				WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(color), &color);
		}
		break;

	case S_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woFlags, WOF_AUTO_CLEAR))
		{
			WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(0, 0), (MPARAM)0);
			WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(0, 0x7FFF), (MPARAM)0);
		}
		else
			WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(0, 0), (MPARAM)0);
		break;

	case S_NON_CURRENT:
		DataGet();
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_STRING::OSDataGet(void)
{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
	if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) && !FlagSet(woStatus, WOS_OWNERDRAW))
	{
		int length = (maxLength + 1) * sizeof(ZIL_ICHAR);
		char *tmpBuff = new char[length];
		WinQueryWindowText(screenID, length, (PSZ)tmpBuff);
		UnMapText(tmpBuff, text);
		delete tmpBuff;
	}
#else
	if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) && !FlagSet(woStatus, WOS_OWNERDRAW))
		WinQueryWindowText(screenID, maxLength + 1, (PSZ)text);
#endif
}

void UIW_STRING::OSDataSet(void)
{
	if (screenID)
	{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
		if (parent->Inherited(ID_COMBO_BOX) && !FlagSet(parent->woStatus, WOS_OWNERDRAW))
			WinSendMsg(parent->screenID, LM_SETITEMTEXT, (MPARAM)ListIndex(), (MPARAM)TEXT_);
		else
		{
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) && !FlagSet(woStatus, WOS_OWNERDRAW))
			{
				WinSendMsg(screenID, EM_SETTEXTLIMIT, (MPARAM)maxLength, (MPARAM)0);
				WinSetWindowText(screenID, (PSZ)TEXT_);
				if (FlagSet(woFlags, WOF_AUTO_CLEAR) && FlagSet(woStatus, WOS_CURRENT))
					WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(0, 0x7FFF), (MPARAM)0);
				else
					WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(0, 0), (MPARAM)0);
			}
			else
				Event(UI_EVENT(S_REDISPLAY));
		}
	}
}

void UIW_STRING::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (objectID == ID_STRING && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
