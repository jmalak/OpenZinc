//	Zinc Interface Library - W_MAX.CPP
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
	// Initialize the maximize button.
	if (event.type == S_INITIALIZE)
	{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_MAXIMIZEBOX;
		parent->Information(I_SET_DWSTYLE, &dwStyle);
	}
	return (event.type);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_MAXIMIZE_BUTTON::OSUpdateSettings(ZIL_OBJECTID)
{
	if (parent)
		parent->Information(I_SET_DWSTYLE, &dwStyle);
}


