//	Zinc Interface Library - Z_WIN3.CPP
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

UIW_WINDOW *UIW_WINDOW::Generic(int left, int top, int width, int height,
	ZIL_ICHAR *title, UI_WINDOW_OBJECT *minObject, WOF_FLAGS woFlags,
	WOAF_FLAGS woAdvancedFlags, UI_HELP_CONTEXT helpContext)
{
	// Create the window and add default window objects.
	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height,
		woFlags, woAdvancedFlags, helpContext, minObject);
	window->Add(new UIW_BORDER);
	window->Add(new UIW_MAXIMIZE_BUTTON);
	window->Add(new UIW_MINIMIZE_BUTTON);
	window->Add(new UIW_SYSTEM_BUTTON(SYF_GENERIC));
	window->Add(new UIW_TITLE(title));

	// Return a pointer to the new window.
	return (window);
}
