//	Zinc Interface Library - O_SYS.CPP
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

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

static MRESULT EXPENTRY SysCallback(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch (msg)
	{
	case WM_MENUSELECT:
		if (WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT), FID_CLIENT), msg, mp1, mp2))
			return (MRESULT)TRUE;
	}
	FNWP *callback = (FNWP *)WinQueryWindowULong(hwnd, QWL_USER);
	return callback(hwnd, msg, mp1, mp2);
}

EVENT_TYPE UIW_SYSTEM_BUTTON::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	switch (ccode)
	{
	case S_INITIALIZE:
		flFlag = FCF_SYSMENU;
		parent->Information(I_SET_FLFLAG, &flFlag);
		menu.NumberID(SC_SYSMENU);
		menu.parent = this;
		menu.woFlags |= WOF_SUPPORT_OBJECT;

		if (menu.First())
			menu.Event(event);
		break;

	case S_CREATE:
		{
		ZIL_SCREENID parentFrameID;
		parent->Information(I_GET_FRAMEID, &parentFrameID);
		screenID = WinWindowFromID(parentFrameID, FID_SYSMENU);
		defaultCallback = (PFNWP)WinQueryWindowPtr(screenID, QWP_PFNWP);

		WinSetWindowULong(screenID, QWL_USER, (ULONG)WinSubclassWindow(screenID, SysCallback));

		MENUITEM item;
		if (!WinSendMsg(screenID, MM_QUERYITEM, (MPARAM)SC_SYSMENU, (MPARAM)&item))
			break;
		menu.screenID = item.hwndSubMenu;
		if (!FlagSet(syFlags, SYF_GENERIC))
		{
			short usItem = LOWORD(WinSendMsg(menu.screenID, MM_ITEMIDFROMPOSITION, (MPARAM)0, (MPARAM)0));
			while (usItem != MIT_ERROR)
			{
				WinSendMsg(menu.screenID, MM_DELETEITEM, MPFROM2SHORT(usItem, TRUE), (MPARAM)0);
				usItem = LOWORD(WinSendMsg(menu.screenID, MM_ITEMIDFROMPOSITION, (MPARAM)0, (MPARAM)0));
			}
			menu.Event(event);
		}
		else if (parent->parent)
		{
			int position = 0;
			short usItem = LOWORD(WinSendMsg(menu.screenID, MM_ITEMIDFROMPOSITION, (MPARAM)position++, (MPARAM)0));
			while (usItem != MIT_ERROR)
			{
				ZIL_ICHAR buffer[64];
				WinSendMsg(menu.screenID, MM_QUERYITEMTEXT, MPFROM2SHORT(usItem, 64), (MPARAM)buffer);
				int index = 0;
				while (buffer[index])
				{
					if (buffer[index] == '\t')
					{
						buffer[index] = '\0';
						break;
					}
					index++;
				}
				WinSendMsg(menu.screenID, MM_SETITEMTEXT, (MPARAM)usItem, (MPARAM)buffer);
				usItem = LOWORD(WinSendMsg(menu.screenID, MM_ITEMIDFROMPOSITION, (MPARAM)position++, (MPARAM)0));
			}
		}
		}
		break;

	case S_CHANGED:
		trueRegion.bottom = trueRegion.top + WinQuerySysValue(HWND_DESKTOP, SV_CYMINMAXBUTTON) - 1;
		trueRegion.right = trueRegion.left + WinQuerySysValue(HWND_DESKTOP, SV_CXMINMAXBUTTON) / 2;
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
	case S_DEINITIALIZE:
		menu.Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_SYSTEM_BUTTON::OSUpdateSettings(ZIL_OBJECTID)
{
}
