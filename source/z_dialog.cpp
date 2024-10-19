//	Zinc Interface Library - Z_DIALOG.CPP
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


extern "C"
{
#include <stdio.h>
#include <stdarg.h>
}
#include "ui_win.hpp"

ZAF_DIALOG_WINDOW::ZAF_DIALOG_WINDOW(int _left, int _top, int _width,
	int _height, WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags,
	UI_HELP_CONTEXT _helpContext) :
	UIW_WINDOW(_left, _top, _width, _height, _woFlags,
	_woAdvancedFlags | WOAF_DIALOG_OBJECT | WOAF_MODAL | WOAF_NO_DESTROY,
	_helpContext, ZIL_NULLP(UI_WINDOW_OBJECT))
{
	searchID = windowID[0] = ID_DIALOG_WINDOW;
	windowID[1] = ID_WINDOW;
}

ZAF_DIALOG_WINDOW::~ZAF_DIALOG_WINDOW(void)
{
}

EVENT_TYPE ZAF_DIALOG_WINDOW::Control(void)
{
	windowManager->Add(this);

#if defined(ZIL_NEXTSTEP)
	[NXApp runModalFor:screenID];
#endif
	// Wait for user response.
	EVENT_TYPE returnValue = 0;
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		if (event.type >= DLG_DIALOG_FIRST && event.type <= DLG_DIALOG_LAST)
			returnValue = event.type;
		else
			windowManager->Event(event);
	} while (!returnValue && screenID);

	windowManager->Subtract(this);
	return returnValue;
}

#if defined(ZIL_LOAD)
ZAF_DIALOG_WINDOW::ZAF_DIALOG_WINDOW(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
	ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(name, file, object, objectTable, userTable)
{
	searchID = windowID[0] = ID_DIALOG_WINDOW;
	windowID[1] = ID_WINDOW;
	woAdvancedFlags |= WOAF_DIALOG_OBJECT | WOAF_MODAL | WOAF_NO_DESTROY;
}
#endif

#if defined(ZIL_STORE)
void ZAF_DIALOG_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
	ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UIW_WINDOW::Store(name, file, object, objectTable, userTable);
}
#endif
