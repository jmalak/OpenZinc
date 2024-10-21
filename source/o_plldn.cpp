//	Zinc Interface Library - O_PLLDN.CPP
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


#define INCL_WINMENUS			// OS/2 menu messages and flags.
#include "ui_win.hpp"

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_MENU::Event(const UI_EVENT &event)
{
	static PFNWP _plldnCallback = ZIL_NULLP(FNWP);
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_INITMENU:
		case WM_MENUEND:
			return(UIW_WINDOW::Event(event));
		}
		return (DefaultCallback(event));
	}

//	UI_WINDOW_OBJECT *object;

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_PULL_DOWN_MENU);
	switch (ccode)
	{
	case S_INITIALIZE:
		numberID = FID_MENU;
		woFlags |= WOF_SUPPORT_OBJECT;
		UIW_WINDOW::Event(event);
	 	flStyle |= MS_ACTIONBAR;
		break;

	case S_DEINITIALIZE:
		UIW_WINDOW::Event(event);
		if (event.windowObject == this)
		{
			ZIL_SCREENID parentFrameID;
			parent->Information(I_GET_FRAMEID, &parentFrameID);
			WinSendMsg(parentFrameID, WM_UPDATEFRAME, (MPARAM)flFlag, (MPARAM)0);
		}
		break;

	case S_CHANGED:
		{
		int height = WinQuerySysValue(HWND_DESKTOP, SV_CYMENU);
		if (screenID)
		{
			SWP swp;
			memset(&swp, 0, sizeof(swp));
			swp.cx = trueRegion.Width();
			swp.cy = trueRegion.Height();
			swp.fl = SWP_SIZE;
			WinSendMsg(screenID, WM_ADJUSTWINDOWPOS, (MPARAM)&swp, 0);
			height = swp.cy;
		}
		trueRegion.bottom = trueRegion.top + height - 1;
		}
		break;

	case S_REGISTER_OBJECT:
		RegisterObject("UIW_PULL_DOWN_MENU", WC_MENU, &_plldnCallback, ZIL_NULLP(ZIL_ICHAR));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PULL_DOWN_MENU::OSUpdateSettings(ZIL_OBJECTID)
{
	if (screenID && parent)
	{
		parent->Information(I_SET_MENU, &screenID);

		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			WinSendMsg(screenID, MM_SETITEMATTR, MPFROM2SHORT(object->NumberID(), FALSE),
				MPFROM2SHORT(MIA_DISABLED, (FlagSet(woFlags, WOF_NON_SELECTABLE) ||
				FlagSet(object->woFlags, WOF_NON_SELECTABLE)) ? MIA_DISABLED : 0));
	}
}
