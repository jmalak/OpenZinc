//	Zinc Interface Library - O_MAX.CPP
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

// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------

EVENT_TYPE UIW_MAXIMIZE_BUTTON::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	switch (ccode)
	{
	case S_INITIALIZE:
		flFlag = FCF_MAXBUTTON;
		parent->Information(I_SET_FLFLAG, &flFlag);
		break;

	case S_CREATE:
		screenID = WinWindowFromID(parent->screenID, FID_MINMAX);
		defaultCallback = (PFNWP)WinQueryWindowPtr(screenID, QWP_PFNWP);
		break;

	case S_CHANGED:
		{
		trueRegion.bottom = trueRegion.top + WinQuerySysValue(HWND_DESKTOP, SV_CYMINMAXBUTTON) - 1;
		trueRegion.left = trueRegion.right - WinQuerySysValue(HWND_DESKTOP, SV_CXMINMAXBUTTON) / 2;
		}
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_MAXIMIZE_BUTTON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	/* unused parameters */ (void)objectID;
}
