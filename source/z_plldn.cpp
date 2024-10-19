//	Zinc Interface Library - Z_PLLDN.CPP
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

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

UIW_PULL_DOWN_MENU::UIW_PULL_DOWN_MENU(int _indentation, WOF_FLAGS _woFlags,
	WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(0, 0, 0, 0, _woFlags | WOF_SUPPORT_OBJECT, _woAdvancedFlags),
	indentation(_indentation)
{
	// Initialize the pull-down menu information.
	UIW_PULL_DOWN_MENU::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_PULL_DOWN_MENU::UIW_PULL_DOWN_MENU(int _indentation, UI_ITEM *item) :
	UIW_WINDOW(0, 0, 0, 0, WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT, WOAF_NO_FLAGS),
	indentation(_indentation)
{
	// Initialize the pull-down menu information.
	UIW_PULL_DOWN_MENU::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Add the pull-down menu items.
	for (int i = 0; item[i].text; i++)
		Add(new UIW_PULL_DOWN_ITEM(item[i].text, item[i].flags,
			(ZIL_USER_FUNCTION)item[i].data, item[i].value));
}

UIW_PULL_DOWN_MENU::~UIW_PULL_DOWN_MENU(void)
{
	// No menu information needs to be destroyed.
}

// Defined in A_PLLDN.CPP.
#if !defined(ZIL_MACINTOSH)
UI_WINDOW_OBJECT *UIW_PULL_DOWN_MENU::Add(UI_WINDOW_OBJECT *object)
{
	// Add the menu-item searchID.
	if (object)
		object->WindowID(ID_MENU_ITEM);
	// Add the item to the menu.
	return (UIW_WINDOW::Add(object));
}
#endif

void *UIW_PULL_DOWN_MENU::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_PULL_DOWN_MENU;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		WindowID(ID_MENU);
		searchID = WindowID(ID_PULL_DOWN_MENU);

		font = FNT_SYSTEM_FONT;
		hotKey = HOT_KEY_SUB_WINDOW;
		wnFlags |= WNF_CONTINUE_SELECT;
		if (indentation)
		{
			ZIL_ICHAR string[64];
			for (int i = 0; i < indentation; i++)
				string[i] = ' ';
			string[indentation] = '\0';
			UIW_PULL_DOWN_ITEM *item = new UIW_PULL_DOWN_ITEM(string);
			item->woFlags |= WOF_NON_SELECTABLE;
			Add(item);
		}

		// Update the internal flag settings.
		UIW_PULL_DOWN_MENU::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UIW_PULL_DOWN_MENU::UpdateSettings(objectID);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// Defined in A_PLLDN.CPP.
#if !defined(ZIL_MACINTOSH)
UI_WINDOW_OBJECT *UIW_PULL_DOWN_MENU::Subtract(UI_WINDOW_OBJECT *object)
{
	return (UIW_WINDOW::Subtract(object));
}
#endif

void UIW_PULL_DOWN_MENU::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_PULL_DOWN_MENU::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_PULL_DOWN_MENU::NewFunction(void) { return (UIW_PULL_DOWN_MENU::New); }

UIW_PULL_DOWN_MENU::UIW_PULL_DOWN_MENU(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 0, 0, WOF_BORDER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	WOAF_NO_FLAGS),
	indentation(0)
{
	// Initialize the pull-down menu information.
	UIW_PULL_DOWN_MENU::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_PULL_DOWN_MENU::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_PULL_DOWN_MENU::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Read the file information.
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
}
#endif

#if defined(ZIL_STORE)
void UIW_PULL_DOWN_MENU::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the pull-down menu information.
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
}
#endif
