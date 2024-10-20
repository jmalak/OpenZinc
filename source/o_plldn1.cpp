//	Zinc Interface Library - O_PLLDN1.CPP
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

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_ITEM::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event; (void)ccode;

	// OS/2 automatically displays text when return is FALSE.
	return (FALSE);
}

EVENT_TYPE UIW_PULL_DOWN_ITEM::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_MENUSELECT:
			if (LOWORD(event.message.mp1) != 0xFFFF)
			{
				UI_EVENT tEvent;
				if (HIWORD(event.message.mp1) != 0)
				{
					tEvent.type = L_SELECT;
					Event(tEvent);
					return (TRUE);
				}
				else
				{
					tEvent.type = S_ADD_OBJECT;
					tEvent.data = this;
					parent->Event(tEvent);
				}
			}
			return (FALSE);

		case WM_HELP:
			Event(UI_EVENT(L_HELP));
			return(0);
		}
		return (DefaultCallback(event));
	}

	// Check for Zinc specific messages.
	switch (ccode)
	{
	case S_INITIALIZE:
		if (FlagSet(btFlags, BTF_SEND_MESSAGE) && !userFunction)
			userFunction = UIW_BUTTON::Message;
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (menu.First())
			menu.Event(event);
		break;

	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (!screenID)
		{
			screenID = parent->screenID;
			if (menu.First())
			{
				menu.Event(event);
				flStyle |= MIS_SUBMENU;
			}
			else
				flStyle |= MIS_SYSCOMMAND;
			MENUITEM item;
			item.iPosition = MIT_END;
			item.afStyle = flStyle;
			item.afAttribute = 0;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				item.afAttribute |= MIA_DISABLED;
			item.id = numberID;
			item.hwndSubMenu = menu.screenID;
			item.hItem = 0;
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
			char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#			define TEXT_ text
#endif
			WinSendMsg(screenID, MM_INSERTITEM, (MPARAM)&item, (MPARAM)TEXT_);
		}
		break;

	case S_REGISTER_OBJECT:
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case S_DEINITIALIZE:
		if (screenID && event.windowObject == this)
			WinSendMsg(screenID, MM_DELETEITEM, MPFROM2SHORT(numberID, FALSE), (MPARAM)0);
		menu.Event(event);
		UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PULL_DOWN_ITEM::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	/* unused parameters */ (void)objectID;

	if (screenID)
	{
		WinSendMsg(screenID, MM_SETITEMATTR, MPFROM2SHORT(numberID, FALSE),
			MPFROM2SHORT(MIA_DISABLED, FlagSet(woFlags, WOF_NON_SELECTABLE) ? MIA_DISABLED : 0));
		parent->Event(UI_EVENT(S_REDISPLAY));
	}
}
