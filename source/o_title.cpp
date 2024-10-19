//	Zinc Interface Library - O_TITLE.CPP
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

// ----- UIW_TITLE ----------------------------------------------------------

EVENT_TYPE UIW_TITLE::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	if (ccode == E_OS2)
		DefaultCallback(event);

	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		flFlag = FCF_TITLEBAR;
		parent->Information(I_SET_TITLE, this);
		parent->Information(I_SET_FLFLAG, &flFlag);
		break;

	case S_CREATE:
		screenID = WinWindowFromID(parent->screenID, FID_TITLEBAR);
		defaultCallback = (PFNWP)WinQueryWindowPtr(screenID, QWP_PFNWP);
		break;

	case S_CHANGED:
		trueRegion.bottom = trueRegion.top + WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) - 1;
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TITLE::OSDataSet(void)
{
	if (parent && parent->screenID)
	{
		ZIL_SCREENID parentFrameID;
		parent->Information(I_GET_FRAMEID, &parentFrameID);
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
		WinSetWindowText(parentFrameID, (PSZ)TEXT_);
	}
}

void UIW_TITLE::OSUpdateSettings(ZIL_OBJECTID objectID)
{
}
