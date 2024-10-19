//	Zinc Interface Library - O_PROMPT.CPP
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


#define INCL_WINSTATICS			// OS/2 static messages and flags.
#include "ui_win.hpp"

// ----- UIW_PROMPT ---------------------------------------------------------

EVENT_TYPE UIW_PROMPT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);

	// Draw the prompt information.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	DrawText(screenID, region, text, lastPalette,
		Inherited(ID_LIST_ITEM) ? TRUE : FALSE, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_PROMPT::Event(const UI_EVENT &event)
{
	static PFNWP _promptCallback = ZIL_NULLP(FNWP);
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
		return (UI_WINDOW_OBJECT::Event(event));

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_PROMPT);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		flStyle |= SS_TEXT | DT_VCENTER | DT_MNEMONIC;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			flStyle |= DT_CENTER;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			flStyle |= DT_RIGHT;
		else
			flStyle |= DT_LEFT;
		if (FlagSet(woFlags, WOF_BORDER))
			woStatus |= WOS_OWNERDRAW;
		else
			woStatus &= ~WOS_OWNERDRAW;
		break;

	case S_CHANGED:
		if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))
		{
			trueRegion.right = trueRegion.left + display->TextWidth(text, screenID, font);
			relative.right = relative.left + trueRegion.Width() - 1;
		}
		if (FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL))
			trueRegion.bottom = trueRegion.top + display->TextHeight(text, screenID, font);
		break;

	case S_REGISTER_OBJECT:
		RegisterObject("UIW_PROMPT", WC_STATIC, &_promptCallback, text);
		Event(UI_EVENT(S_SIZE, 0, relative));
		ULONG color;
		if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
			ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
			WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(color), &color);
		break;

	case E_KEY:
		if (event.rawCode == 0xFFFFFFFF && next)
		{
			UI_EVENT addEvent(S_ADD_OBJECT);
			addEvent.windowObject = Next();
			parent->Event(addEvent);
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PROMPT::OSDataSet(void)
{
	// Redisplay the prompt.
	if (screenID)
	{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#	define TEXT_ text
#endif
		WinSetWindowText(screenID, (PSZ)TEXT_);
		if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))
			UI_WINDOW_OBJECT::Event(UI_EVENT(S_SIZE, 0, relative));
		Event(UI_EVENT(S_REDISPLAY, 0));
	}
}

void UIW_PROMPT::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-displayed.
	if (objectID == ID_PROMPT && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
