//	Zinc Interface Library - W_BORDER.CPP
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

EVENT_TYPE UIW_BORDER::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	// Windows automatically displays the border.
	return (FALSE);
}

EVENT_TYPE UIW_BORDER::Event(const UI_EVENT &event)
{
	// Initialize the border.
	if (event.type == S_INITIALIZE)
	{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_THICKFRAME | WS_BORDER;
		if (!FlagSet(parent->woAdvancedFlags, WOAF_DIALOG_OBJECT))
			parent->Information(I_SET_DWSTYLE, &dwStyle);
	}
	return (event.type);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_BORDER::OSDataGet(void)
{
	//  Ask Windows what the current size is.
	WOAF_FLAGS flags = WOAF_NO_FLAGS;
	if (parent)
		flags = parent->woAdvancedFlags;

	if (FlagSet(flags, WOAF_DIALOG_OBJECT))
		width = GetSystemMetrics(SM_CXDLGFRAME);
	else if (FlagSet(flags, WOAF_NO_SIZE))
		width = GetSystemMetrics(SM_CXBORDER);
	else
		width = GetSystemMetrics(SM_CXFRAME);
}

void UIW_BORDER::OSDataSet(void)
{
	// Windows does not allow the programmer to set the border width.
	//  So ask Windows what the current size is.
	UIW_BORDER::OSDataGet();
}

void UIW_BORDER::OSUpdateSettings(ZIL_OBJECTID)
{
	if (parent)
		parent->Information(I_SET_DWSTYLE, &dwStyle);
}


