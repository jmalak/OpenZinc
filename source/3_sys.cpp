//	Zinc Interface Library - W_SYS.CPP
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


#include "ui_win.hpp"

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

EVENT_TYPE UIW_SYSTEM_BUTTON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_SYSMENU;
		menu.parent = this;
		menu.woFlags |= WOF_SUPPORT_OBJECT;
		for (UI_WINDOW_OBJECT *object = menu.First(); object; object = object->Next())
			object->Event(event);
		parent->Information(I_SET_DWSTYLE, &dwStyle);
		}
		break;

	case S_CREATE:
		if (!FlagSet(syFlags, SYF_GENERIC))
		{
			ZIL_SCREENID pFrameID;
			parent->Information(I_GET_FRAMEID, &pFrameID);
			menu.menuID = GetSystemMenu(pFrameID, FALSE);
			while (DeleteMenu(menu.menuID, 0, MF_BYPOSITION))
				;
			for (UI_WINDOW_OBJECT *object = menu.First(); object; object = object->Next())
				object->Event(event);

		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case L_SELECT:
		if (parent)
			Root()->Event(UI_EVENT(E_MSWINDOWS, Root()->screenID,
				WM_SYSCOMMAND, SC_KEYMENU, ' '));
		break;

	default:
		ccode = S_UNKNOWN;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_SYSTEM_BUTTON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	/* unused parameters */ (void)objectID;

	if (parent)
		parent->Information(I_SET_DWSTYLE, &dwStyle);
}

