//	Zinc Interface Library - O_GROUP.CPP
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

// ----- UIW_GROUP ----------------------------------------------------------

EVENT_TYPE UIW_GROUP::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	return UIW_WINDOW::DrawItem(event, ccode);
}

EVENT_TYPE UIW_GROUP::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		if (event.message.msg == WM_WINDOWPOSCHANGED)
		{
			SWP *swp = (SWP *)event.message.mp1;
			if (FlagSet(swp[0].fl, SWP_SIZE))
			{
				HWND staticID = WinQueryWindow(screenID, QW_TOP);
				WinSetWindowPos(staticID, 0, 0, 0, (short)swp[0].cx,
					(short)swp[0].cy, SWP_SIZE);
			}
		}
		return (UIW_WINDOW::Event(event));
	}

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_GROUP);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woStatus, WOS_EDIT_MODE))
			flStyle |= WS_DISABLED;
		break;

	case S_CREATE:
		{
		ccode = UIW_WINDOW::Event(event);
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
		WinCreateWindow(screenID, WC_STATIC, (PSZ)TEXT_,
			flStyle | SS_GROUPBOX | DT_MNEMONIC, 0, 0,
			trueRegion.Width(), trueRegion.Height(), screenID, HWND_BOTTOM, 0,
			ZIL_NULLP(void), ZIL_NULLP(void));
		}
		break;

	case S_REGISTER_OBJECT:
		{
		UI_WINDOW_OBJECT::Event(event);
		ULONG color;
		if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
			ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
			WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(color), &color);
		}
		break;

	case L_PREVIOUS:
	case L_NEXT:
/* START BLOCK COMMENT
**			if (!HasWrappedData())
**			{
**				UI_WINDOW_OBJECT *object = Current();
**				UIW_WINDOW::Event(event);
**				if (object == Current() && current)
**				{
**					object = (ccode == L_NEXT) ? First() : Last();
**					while (object && (object->IsNonselectable() || object->IsNoncurrent()))
**						object = (ccode == L_NEXT) ? object->Next() : object->Previous();
**					if (object)
**						SetCurrent(object);
**					ccode = S_UNKNOWN;
**				}
**			}
**			else
END BLOCK COMMENT */
			ccode = parent->Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_GROUP::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);
	object->trueRegion.top -= display->cellHeight / 4;
	object->trueRegion.bottom -= display->cellHeight / 4;
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_GROUP::OSDataSet(void)
{
	if (screenID)
		Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
}

void UIW_GROUP::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_GROUP && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
