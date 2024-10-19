//	Zinc Interface Library - Z_POPUP.CPP
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

ZIL_ICHAR _tabString[] = { '\t', 0 };

// ----- UIW_POP_UP_MENU ----------------------------------------------------

UIW_POP_UP_MENU::UIW_POP_UP_MENU(int left, int top, WNF_FLAGS _wnFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(left, top, 0, 0, _woFlags, _woAdvancedFlags)
{
	// Initialize the menu information.
	wnFlags = _wnFlags;
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_POP_UP_MENU::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_POP_UP_MENU::UIW_POP_UP_MENU(int left, int top, WNF_FLAGS _wnFlags, UI_ITEM *item) :
	UIW_WINDOW(left, top, 0, 0, WOF_BORDER)
{
	// Initialize the menu information.
	wnFlags = _wnFlags;
	UIW_POP_UP_MENU::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Add the pop-up menu items.
	for (int i = 0; item[i].text; i++)
		Add(new UIW_POP_UP_ITEM(item[i].text, item[i].flags, BTF_NO_3D,
			WOF_NO_FLAGS, (ZIL_USER_FUNCTION)item[i].data, item[i].value));
}

UIW_POP_UP_MENU::~UIW_POP_UP_MENU(void)
{
	// No menu information needs to be destroyed.
}

void *UIW_POP_UP_MENU::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_POP_UP_MENU;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		WindowID(ID_MENU);
		searchID = WindowID(ID_POP_UP_MENU);

		wnFlags |= WNF_CONTINUE_SELECT;
		font = FNT_SYSTEM_FONT;
		break;

#if defined (ZIL_MSDOS) || defined (ZIL_CURSES) || defined (ZIL_MOTIF)
	case I_GET_TABCOLUMN:
		*(int *)data = tabColumn;
		break;
#endif

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// Defined in A_POPUP.CPP.
#if !defined(ZIL_MACINTOSH) && !defined(ZIL_OS2)
UI_WINDOW_OBJECT *UIW_POP_UP_MENU::Subtract(UI_WINDOW_OBJECT *object)
{
	return (UIW_WINDOW::Subtract(object));
}
#endif

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_POP_UP_MENU::NewFunction(void) { return (UIW_POP_UP_MENU::New); }

UIW_POP_UP_MENU::UIW_POP_UP_MENU(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS, WOAF_NO_FLAGS)
{
	// Initialize the menu information.
	UIW_POP_UP_MENU::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_POP_UP_MENU::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_POP_UP_MENU::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the pop-up menu information.
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
}
#endif

#if defined(ZIL_STORE)
void UIW_POP_UP_MENU::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the pop-up menu information.
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
}
#endif
