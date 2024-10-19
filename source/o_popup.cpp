//	Zinc Interface Library - O_POPUP.CPP
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

// ----- UIW_POP_UP_MENU ----------------------------------------------------

UI_WINDOW_OBJECT *UIW_POP_UP_MENU::Add(UI_WINDOW_OBJECT *object)
{
	// Add the menu-item searchID.
	if (object)
		object->WindowID(ID_MENU_ITEM);

	// Add the item to the menu.
	UIW_WINDOW::Add(object);

	// Generate Sub-Menu if needed.
	if (parent && parent->screenID && !screenID)
	{
		// Create the menu.
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));

		// Attach menu to parent.
		MENUITEM item;
		WinSendMsg(parent->screenID, MM_QUERYITEM, MPFROM2SHORT(parent->NumberID(), TRUE), (MPARAM)&item);
		item.afStyle |= MIS_SUBMENU;
		item.hwndSubMenu = screenID; 
		WinSendMsg(parent->screenID, MM_SETITEM, MPFROM2SHORT(parent->NumberID(), TRUE), (MPARAM)&item);
	}

	// Return a pointer to the object.
	return (object);
}

UI_WINDOW_OBJECT *UIW_POP_UP_MENU::Subtract(UI_WINDOW_OBJECT *object)
{
	// Subtract the item from the menu.
	UIW_WINDOW::Subtract(object);

	// Remove Sub-Menu if empty.
	if (screenID && !first)
	{
		// Remove menu from parent.
		MENUITEM item;
		WinSendMsg(parent->screenID, MM_QUERYITEM, MPFROM2SHORT(parent->NumberID(), TRUE), (MPARAM)&item);
		item.afStyle &= ~MIS_SUBMENU;
		item.hwndSubMenu = 0; 
		WinSendMsg(parent->screenID, MM_SETITEM, MPFROM2SHORT(parent->NumberID(), TRUE), (MPARAM)&item);

		// Destroy menu.
		UI_EVENT deinitializeEvent(S_DEINITIALIZE);
		deinitializeEvent.windowObject = this;
		UIW_WINDOW::Event(deinitializeEvent);
	}

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UIW_POP_UP_MENU::Event(const UI_EVENT &event)
{
	static PFNWP _popupCallback = ZIL_NULLP(FNWP);

	// Check for OS/2 specific messages.
	EVENT_TYPE ccode = event.type;
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_INITMENU:
			if (LOWORD(event.message.mp1) == numberID)
			{
				current = ZIL_NULLP(UI_ELEMENT);
				if (parent)
					Event(UI_EVENT(S_CURRENT));
				return (0);
			}
			else
				return (UIW_WINDOW::Event(event));


		case WM_MENUEND:
			if (LOWORD(event.message.mp1) == numberID)
			{
				if (parent)
					Event(UI_EVENT(S_NON_CURRENT));
				else
				{
					if ((HWND)event.message.mp2 == screenID)
					{
						UI_EVENT tEvent(S_SUBTRACT_OBJECT);
						tEvent.windowObject = this;
						eventManager->Put(tEvent);
					}
					else
						UIW_WINDOW::Event(event);
				}
				return (0);
			}
			else
				return (UIW_WINDOW::Event(event));

		case WM_ADJUSTWINDOWPOS:
			if (!parent)
			{
				SWP *swp = (SWP *)event.message.mp1;
				trueRegion.right = trueRegion.left + swp->cx;
				trueRegion.bottom = trueRegion.top + swp->cy;
				swp->x = trueRegion.left;
				swp->y = display->lines - trueRegion.bottom;
			}
			break;

		case WM_MENUSELECT:
			return (UIW_WINDOW::Event(event));
		}

		return (DefaultCallback(event));
	}

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_POP_UP_MENU);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		flStyle &= ~WS_VISIBLE;
		break;

	case S_REGISTER_OBJECT:
		RegisterObject("UIW_POP_UP_MENU", WC_MENU, &_popupCallback, ZIL_NULLP(ZIL_ICHAR));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

