//	Zinc Interface Library - W_POPUP1.CPP
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

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_ITEM::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Windows automatically displays text.

	/* unused parameters */ (void)event; (void)ccode;

	return (FALSE);
}

EVENT_TYPE UIW_POP_UP_ITEM::Event(const UI_EVENT &event)
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
		case WM_COMMAND:
			Event(UI_EVENT(L_SELECT));
			return (0);

		case WM_MENUSELECT:
			{
#if defined(ZIL_WINNT)
			WORD wIDItem = LOWORD(wParam);
			WORD fwMenu = HIWORD(wParam);
			HMENU hmenu = (HMENU)lParam;
			HMENU mMenuID = GetSubMenu(hmenu, wIDItem);
#else
			WORD wIDItem = wParam;
			WORD fwMenu = LOWORD(lParam);
			HMENU mMenuID = (HMENU)wParam;
#endif
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
			// Check for system commands.
			if (FlagSet(mniFlags, 0x00FF))
			{
				if (FlagSet(mniFlags, MNIF_RESTORE))
					value = L_RESTORE;
				else if (FlagSet(mniFlags, MNIF_MOVE))
					value = L_MOVE;
  				else if (FlagSet(mniFlags, MNIF_SIZE))
					value = L_SIZE;
				else if (FlagSet(mniFlags, MNIF_MINIMIZE))
					value = L_MINIMIZE;
				else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
					value = L_MAXIMIZE;
				else if (FlagSet(mniFlags, MNIF_CLOSE))
					value = S_CLOSE;

				for (UI_WINDOW_OBJECT *object = this; object; object = object->parent)
					if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT) && object->parent)
					{
						if (value < 0)
							value += S_MDICHILD_EVENT;
						else
							value += L_MDICHILD_EVENT;
						break;
					}
			}
			UI_WINDOW_OBJECT::Event(event);
			if (parent && parent->parent && parent->parent->Inherited(ID_SYSTEM_BUTTON))
				break;
			// Continue to S_CREATE.
		case S_CREATE:
			if (ccode == S_CREATE && (!parent->parent || !parent->parent->Inherited(ID_SYSTEM_BUTTON)))
				break;
			menu.woFlags |= WOF_SUPPORT_OBJECT;
			if (menu.First())
			{
				menu.Event(event);
				WORD wFlags = MF_STRING | MF_POPUP;
				if (FlagSet(woFlags, WOF_NON_SELECTABLE))
					wFlags |= MF_GRAYED;
				else if (FlagSet(mniFlags, MNIF_CHECK_MARK) && FlagSet(woStatus, WOS_SELECTED))
					wFlags |= MF_CHECKED;
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
				char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#				define TEXT_ text
#endif

				if (parent->menuID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
					ModifyMenu(parent->menuID, ListIndex(),
						wFlags | MF_BYPOSITION, (UINT)menu.menuID, TEXT_);
				else
					AppendMenu(parent->menuID, wFlags, (UINT)menu.menuID, TEXT_);
			}
			else
			{
				WORD tNumberID = numberID;
				if (FlagSet(mniFlags, MNIF_RESTORE))
					tNumberID = SC_RESTORE;
				else if (FlagSet(mniFlags, MNIF_MOVE))
					tNumberID = SC_MOVE;
				else if (FlagSet(mniFlags, MNIF_SIZE))
					tNumberID = SC_SIZE;
				else if (FlagSet(mniFlags, MNIF_MINIMIZE))
					tNumberID = SC_MINIMIZE;
				else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
					tNumberID = SC_MAXIMIZE;
				else if (FlagSet(mniFlags, MNIF_CLOSE))
					tNumberID = SC_CLOSE;
				else if (FlagSet(mniFlags, MNIF_SWITCH))
					tNumberID = SC_TASKLIST;
				WORD wFlags = MF_STRING;
				if (FlagSet(mniFlags, MNIF_SEPARATOR))
					wFlags = MF_SEPARATOR;
				else if (FlagSet(woFlags, WOF_NON_SELECTABLE))
					wFlags |= MF_GRAYED;
				else if (FlagSet(mniFlags, MNIF_CHECK_MARK) && FlagSet(woStatus, WOS_SELECTED))
					wFlags |= MF_CHECKED;
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
				char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#				define TEXT_ text
#endif
				if (parent->menuID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
					ModifyMenu(parent->menuID, ListIndex(),
						wFlags | MF_BYPOSITION, tNumberID, TEXT_);
				else
					AppendMenu(parent->menuID, wFlags, tNumberID, TEXT_);
			}
			menuID = parent->menuID;
			break;

		case S_DEINITIALIZE:
			break;

		case S_ADD_OBJECT:
		case S_SUBTRACT_OBJECT:
			menu.Event(event);
			break;

		case S_CURRENT:
		case S_NON_CURRENT:
			ccode = UIW_BUTTON::Event(event);
			break;

		case L_SELECT:
			ccode = UIW_BUTTON::Event(event);
			if (FlagSet(mniFlags, MNIF_CHECK_MARK))
			{
				WORD wFlags = MF_BYPOSITION;
				if (FlagSet(woStatus, WOS_SELECTED))
					wFlags |= MF_CHECKED;
				else
					wFlags |= MF_UNCHECKED;
				CheckMenuItem(parent->menuID, ListIndex(), wFlags);
			}
			break;

		case L_HELP:
			if ((menu.First() && menu.Event(event) == S_UNKNOWN) || !menu.First())
				ccode = UIW_BUTTON::Event(event);
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

void UIW_POP_UP_ITEM::OSUpdateSettings(ZIL_OBJECTID)
{
	if (parent && parent->menuID)
	{
		int position = ListIndex();
		WORD wFlags = MF_BYPOSITION;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			wFlags |= MF_GRAYED;
		else
			wFlags |= MF_ENABLED;
		EnableMenuItem(parent->menuID, position, wFlags);
		if (FlagSet(mniFlags, MNIF_CHECK_MARK))
		{
			WORD wFlags = MF_BYPOSITION;
			if (FlagSet(woStatus, WOS_SELECTED))
				wFlags |= MF_CHECKED;
			else
				wFlags |= MF_UNCHECKED;
			CheckMenuItem(parent->menuID, position, wFlags);
		}
	}
}

