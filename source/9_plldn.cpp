//	Zinc Interface Library - W_PLLDN.CPP
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

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_MENU::Event(const UI_EVENT &event)
{
	int processed = FALSE;
	EVENT_TYPE ccode = S_UNKNOWN;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;
		WPARAM wParam = event.message.wParam;
		LPARAM lParam = event.message.lParam;

		processed = TRUE;

		switch (message)
		{
		case WM_MENUSELECT:
			{
			ZIL_NUMBERID wIDItem = LOWORD(wParam);
			WORD fwMenu = HIWORD(wParam);
			HMENU hmenu = (HMENU)lParam;
			HMENU mMenuID = GetSubMenu(hmenu, wIDItem);
			UIW_PULL_DOWN_ITEM *item = NULL;
			for (item = (UIW_PULL_DOWN_ITEM *)First();
				item; item = (UIW_PULL_DOWN_ITEM *)item->Next())
			{
				if ((!FlagSet(fwMenu, MF_POPUP) && wIDItem == item->NumberID()) ||
					(FlagSet(fwMenu, MF_POPUP) && mMenuID == item->menu.menuID))
						break;
			}
			if (item && item != current)
				UIW_WINDOW::Add(item);
			else if (current)
				Current()->Event(event);
			}
			break;

		case WM_DESTROY:
			{
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				if (!FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
				{
					object->Event(event);
					object->screenID = 0;
				}
			}
			break;

		default:
			break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = event.type;
		switch (ccode)
		{
		case S_INITIALIZE:
			{
			if (parent->parent)		// Sub-windows cannot have pull-down menus.
				return (S_UNKNOWN);
			UI_WINDOW_OBJECT::Event(event);
			menuID = CreateMenu();
			woStatus |= WOS_SYSTEM_OBJECT;
			woFlags |= WOF_SUPPORT_OBJECT;
			parent->Information(I_SET_MENU, &menuID);
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			{
				object->Event(event);
				EnableMenuItem(menuID, object->ListIndex(), MF_BYPOSITION |
					(FlagSet(woFlags, WOF_NON_SELECTABLE) ||
					FlagSet(object->woFlags, WOF_NON_SELECTABLE) ? MF_GRAYED : MF_ENABLED));
			}
			if (parent->screenID)
			{
				ZIL_SCREENID pFrameID;
				parent->Information(I_GET_FRAMEID, &pFrameID);
				SetMenu(pFrameID, menuID);
				DrawMenuBar(pFrameID);
			}
			}
			break;

		case S_REDISPLAY:
			if (parent->screenID)
			{
				ZIL_SCREENID pFrameID;
				parent->Information(I_GET_FRAMEID, &pFrameID);
				DrawMenuBar(pFrameID);
			}
			break;

		case S_ADD_OBJECT:
		case S_SUBTRACT_OBJECT:
		case S_DEINITIALIZE:
		case S_NON_CURRENT:
		case L_HELP:
			ccode = UIW_WINDOW::Event(event);
			break;

		default:
			ccode = S_UNKNOWN;
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PULL_DOWN_MENU::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (screenID && parent)
	{
		parent->Information(I_SET_MENU, &screenID);

		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			EnableMenuItem(menuID, object->ListIndex(), MF_BYPOSITION |
				(FlagSet(woFlags, WOF_NON_SELECTABLE) ||
				FlagSet(object->woFlags, WOF_NON_SELECTABLE) ? MF_GRAYED : MF_ENABLED));
		if (parent->screenID)
			DrawMenuBar(parent->screenID);
	}
	else
		UIW_WINDOW::Information(I_CHANGED_FLAGS, ZIL_NULLP(void), objectID);
}

