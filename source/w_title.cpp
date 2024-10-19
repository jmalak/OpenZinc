//	Zinc Interface Library - W_TITLE.CPP
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
	// Initialize the title.
	if (event.type == S_INITIALIZE)
	{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_CAPTION;
		parent->Information(I_SET_TITLE, this);
		parent->Information(I_SET_DWSTYLE, &dwStyle);
	}
	return (event.type);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TITLE::OSDataSet(void)
{
	if (parent && parent->screenID)
	{
		ZIL_SCREENID parentID;
		parent->Information(I_GET_FRAMEID, &parentID);
#if defined(ZIL_UNICODE) 
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
		SendMessage(parentID, WM_SETTEXT, 0, (LPARAM)TEXT_);
	}
}

void UIW_TITLE::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (parent)
	{
		parent->Information(I_SET_TITLE, this);
		parent->Information(I_SET_DWSTYLE, &dwStyle);
	}
}

