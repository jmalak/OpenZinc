//	Zinc Interface Library - O_BORDER.CPP
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

// ----- UIW_BORDER ---------------------------------------------------------

EVENT_TYPE UIW_BORDER::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event; (void)ccode;

	// OS/2 automatically displays the border.
	return (FALSE);
}

EVENT_TYPE UIW_BORDER::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	switch (ccode)
	{
	case S_INITIALIZE:
		if (FlagSet(parent->woAdvancedFlags, WOAF_DIALOG_OBJECT))
			flFlag = FCF_DLGBORDER;
		else if (FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE))
			flFlag = FCF_BORDER;
		else
			flFlag = FCF_SIZEBORDER;
		parent->Information(I_SET_FLFLAG, &flFlag);
		break;

	case S_CHANGED:
		{
		LONG borderWidth, borderHeight;
		if (FlagSet(flFlag, FCF_DLGBORDER))
		{
			borderWidth = WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME);
			borderHeight = WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME);
		}
		else if (FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE))
		{
			borderWidth = WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);
			borderHeight = WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);
		}
		else
		{
			borderWidth = WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
			borderHeight = WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
		}
		trueRegion.left += borderWidth;
		trueRegion.top += borderHeight;
		trueRegion.right -= borderWidth;
		trueRegion.bottom -= borderHeight;
		}
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_BORDER::OSDataGet(void)
{
	width = WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
}

void UIW_BORDER::OSDataSet(void)
{
	WinSetSysValue(HWND_DESKTOP, SV_CXSIZEBORDER, (LONG)width);
	WinSetSysValue(HWND_DESKTOP, SV_CYSIZEBORDER, (LONG)width);
}

void UIW_BORDER::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	/* unused parameters */ (void)objectID;

	// No code necessary.
}
