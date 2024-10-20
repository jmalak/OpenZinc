//	Zinc Interface Library - O_POPUP1.CPP
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

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_ITEM::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event; (void)ccode;

	// OS/2 automatically displays text when return value is FALSE.
	return (FALSE);
}

EVENT_TYPE UIW_POP_UP_ITEM::Event(const UI_EVENT &event)
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
	ccode = LogicalEvent(event, ID_POP_UP_ITEM);
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

		if (FlagSet(mniFlags, MNIF_RESTORE))
			NumberID(SC_RESTORE);
		else if (FlagSet(mniFlags, MNIF_MOVE))
			NumberID(SC_MOVE);
		else if (FlagSet(mniFlags, MNIF_SIZE))
			NumberID(SC_SIZE);
		else if (FlagSet(mniFlags, MNIF_MINIMIZE))
			NumberID(SC_MINIMIZE);
		else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
			NumberID(SC_MAXIMIZE);
		else if (FlagSet(mniFlags, MNIF_CLOSE))
			NumberID(SC_CLOSE);
		else if (FlagSet(mniFlags, MNIF_SWITCH))
			NumberID(SC_TASKMANAGER);
		ccode = UI_WINDOW_OBJECT::Event(event);
//		menu.woFlags |= WOF_SUPPORT_OBJECT;
		if (menu.First())
			menu.Event(event);
		break;

	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (!screenID)
		{
			screenID = parent->screenID;
			if (FlagSet(mniFlags, MNIF_SEPARATOR))
				flStyle = MIS_SEPARATOR;
			else if (menu.First())
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
			if (FlagSet(mniFlags, MNIF_CHECK_MARK))
			{
//				item.afAttribute |= MIA_NODISMISS;
				if (FlagSet(woStatus, WOS_SELECTED))
					item.afAttribute |= MIA_CHECKED;
			}
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

	case S_DEINITIALIZE:
		if (screenID && event.windowObject == this)
			WinSendMsg(screenID, MM_DELETEITEM, MPFROM2SHORT(numberID, FALSE), (MPARAM)0);
		menu.Event(event);
		UI_WINDOW_OBJECT::Event(event);
		break;

	case L_SELECT:
	case S_REDISPLAY:
		ccode = UIW_BUTTON::Event(event);
		if (FlagSet(mniFlags, MNIF_CHECK_MARK))
		{
			WinSendMsg(screenID, MM_SETITEMATTR, MPFROM2SHORT(numberID, FALSE),
				MPFROM2SHORT(MIA_CHECKED, (FlagSet(woStatus, WOS_SELECTED)) ? MIA_CHECKED : 0));
		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_POP_UP_ITEM::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	/* unused parameters */ (void)objectID;

	if (screenID)
	{
		ULONG attributes = 0;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			attributes |= MIA_DISABLED;
		if (FlagSet(mniFlags, MNIF_CHECK_MARK) && FlagSet(woStatus, WOS_SELECTED))
			attributes |= MIA_CHECKED;
		WinSendMsg(screenID, MM_SETITEMATTR, MPFROM2SHORT(numberID, FALSE),
			MPFROM2SHORT(MIA_DISABLED | MIA_CHECKED, attributes));
		parent->Event(UI_EVENT(S_REDISPLAY));
	}
}
