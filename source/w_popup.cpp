//	Zinc Interface Library - W_POPUP.CPP
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

// ----- UIW_POP_UP_MENU ----------------------------------------------------

UI_WINDOW_OBJECT *UIW_POP_UP_MENU::Add(UI_WINDOW_OBJECT *object)
{
	// Add the menu-item searchID.
	if (object)
		object->WindowID(ID_MENU_ITEM);

	// Add the item to the menu.
	UIW_WINDOW::Add(object);

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UIW_POP_UP_MENU::Event(const UI_EVENT &event)
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
			WORD wIDItem = wParam;
			WORD fwMenu = LOWORD(lParam);
			HMENU mMenuID = (HMENU)wParam;
			UIW_POP_UP_ITEM *item = NULL;
			for (item = (UIW_POP_UP_ITEM *)First();
				item; item = (UIW_POP_UP_ITEM *)item->Next())
			{
				if ((!FlagSet(fwMenu, MF_POPUP) && wIDItem == item->NumberID()) ||
					(FlagSet(fwMenu, MF_POPUP) && mMenuID == item->menu.menuID))
						break;
			}

			if (item && item != Current())
				Add(item);
			else if (Current())
				Current()->Event(event);
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
			UI_WINDOW_OBJECT::Event(event);
			menuID = CreatePopupMenu();
			woFlags |= WOF_SUPPORT_OBJECT;
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				object->Event(event);
			}
			break;

		case S_CHANGED:
		case S_CREATE:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		case S_ADD_OBJECT:
		case S_SUBTRACT_OBJECT:
		case S_DEINITIALIZE:
		case L_HELP:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

