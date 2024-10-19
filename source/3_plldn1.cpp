//	Zinc Interface Library - W_PLLDN1.CPP
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

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	return (ccode);
}

EVENT_TYPE UIW_PULL_DOWN_ITEM::Event(const UI_EVENT &event)
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
			WORD wIDItem = LOWORD(wParam);
			WORD fwMenu = HIWORD(wParam);
			HMENU hmenu = (HMENU)lParam;
			HMENU mMenuID = GetSubMenu(hmenu, wIDItem);
			UIW_POP_UP_ITEM *item = NULL;
			for (item = (UIW_POP_UP_ITEM *)menu.First();
				item; item = (UIW_POP_UP_ITEM *)item->Next())
			{
				if ((!FlagSet(fwMenu, MF_POPUP) && wIDItem == item->NumberID()) ||
 					(FlagSet(fwMenu, MF_POPUP) && mMenuID == item->menu.menuID))
						break;
			}
			if (item && item != menu.Current())
				menu.Add(item);
			else if (menu.Current())
				menu.Current()->Event(event);
			}
			break;

		case WM_DESTROY:
			{
			for (UI_WINDOW_OBJECT *object = menu.First(); object; object = object->Next())
				if (!FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
				{
					object->Event(event);
					object->menuID = 0;
				}
			menu.menuID = 0;
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
			menu.woFlags |= WOF_SUPPORT_OBJECT;
#			define TEXT_ text
			if (menu.First())
			{
				menu.Event(event);
				WORD wFlags = MF_STRING | MF_POPUP;
				if (FlagSet(woFlags, WOF_NON_SELECTABLE))
					wFlags |= MF_GRAYED;
				if (parent->menuID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
					ModifyMenu(parent->menuID, ListIndex(),
						wFlags | MF_BYPOSITION,
						(UINT)menu.menuID, TEXT_);
				else
					AppendMenu(parent->menuID, wFlags, (UINT)menu.menuID, TEXT_);
			}
			else
			{
				WORD wFlags = MF_STRING;
				if (FlagSet(woFlags, WOF_NON_SELECTABLE))
					wFlags |= MF_GRAYED;
				if (FlagSet(btFlags, BTF_SEND_MESSAGE) && !userFunction)
					userFunction = UIW_BUTTON::Message;
				AppendMenu(parent->menuID, wFlags, numberID, TEXT_);
			}
			menuID = parent->menuID;
			}
			break;

		case S_DEINITIALIZE:
			if (parent->menuID)
				DeleteMenu(parent->menuID, numberID, MF_BYCOMMAND);
			break;

		case S_REDISPLAY:
			// Parent must be redisplayed in Windows.
			parent->Event(event);
			break;

		case S_ADD_OBJECT:
		case S_SUBTRACT_OBJECT:
			menu.Event(event);
			break;

		case L_SELECT:
		case S_CURRENT:
		case S_NON_CURRENT:
			ccode = UIW_BUTTON::Event(event);
			break;

		case L_HELP:
			{
				if (menu.First() && menu.Event(event) == S_UNKNOWN)
					ccode = UIW_BUTTON::Event(event);
			}
			break;

		default:
			ccode = UIW_BUTTON::Event(event);
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PULL_DOWN_ITEM::OSUpdateSettings(ZIL_OBJECTID)
{
	if (parent && parent->menuID)
	{
		WORD wFlags = MF_BYPOSITION;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			wFlags |= MF_GRAYED;
		else
			wFlags |= MF_ENABLED;
		EnableMenuItem(parent->menuID, ListIndex(), wFlags);
		ZIL_SCREENID pFrameID;
		Root()->Information(I_GET_FRAMEID, &pFrameID);
		DrawMenuBar(pFrameID);
	}
}

