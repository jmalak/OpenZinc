//	Zinc Interface Library - Z_PLLDN1.CPP
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

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

UIW_PULL_DOWN_ITEM::UIW_PULL_DOWN_ITEM(ZIL_ICHAR *_text, WNF_FLAGS _wnFlags,
	ZIL_USER_FUNCTION _userFunction, EVENT_TYPE _value) :
	UIW_BUTTON(0, 0, strlen(_text), _text,
	BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D, WOF_JUSTIFY_CENTER,
	_userFunction, _value),
	menu(0, 0, _wnFlags, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
	// Initialize the menu item information.
	UIW_PULL_DOWN_ITEM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_PULL_DOWN_ITEM::UIW_PULL_DOWN_ITEM(ZIL_ICHAR *_text, WNF_FLAGS _wnFlags,
	UI_ITEM *item) :
	UIW_BUTTON(0, 0, strlen(_text), _text,
	BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D, WOF_JUSTIFY_CENTER),
	menu(0, 0, _wnFlags, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
	// Initialize the menu information.
	UIW_PULL_DOWN_ITEM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	WOF_FLAGS _woFlags;
	// Add the pop-up menu items.
	for (int i = 0; item[i].text; i++)
	{
		_woFlags = FlagSet(item[i].flags, MNIF_NON_SELECTABLE) ?
			WOF_NON_SELECTABLE : WOF_NO_FLAGS;
		Add(new UIW_POP_UP_ITEM(item[i].text, item[i].flags, BTF_NO_3D,
			_woFlags, (ZIL_USER_FUNCTION)item[i].data, item[i].value));
	}
}

UIW_PULL_DOWN_ITEM::~UIW_PULL_DOWN_ITEM(void)
{
	// No item information needs to be destroyed.
}

void *UIW_PULL_DOWN_ITEM::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_PULL_DOWN_ITEM;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
#if defined(ZIL_MOTIF) || defined(ZIL_NEXTSTEP)
		UIW_BUTTON::Information(request, data, objectID);
#endif

		searchID = WindowID(ID_PULL_DOWN_ITEM);
		font = FNT_SYSTEM_FONT;
		menu.parent = this;

		// Update the internal flag settings.
		UIW_PULL_DOWN_ITEM::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UIW_PULL_DOWN_ITEM::UpdateSettings(objectID);
		break;

	case I_GET_NUMBERID_OBJECT:
	case I_GET_STRINGID_OBJECT:
		// Get the field that matches the identification.
		{
		void *match = UI_WINDOW_OBJECT::Information(request, data, objectID);
		if (!match)
			match = menu.Information(request, data, objectID);
		data = match;
		}
		break;

	case I_PRINT_INFORMATION:
	case I_PRINT_USER_FUNCTION:
		UI_WINDOW_OBJECT::Information(request, data, objectID);
		menu.Information(request, data, objectID);
		break;

	case I_GET_FIRST:
	case I_GET_LAST:
	case I_GET_CURRENT:
	case I_GET_SUPPORT_FIRST:
	case I_GET_SUPPORT_LAST:
	case I_GET_SUPPORT_CURRENT:
		return (menu.Information(request, data, objectID));

	case I_GET_SUBWINDOW:
		if (!data)
			return &menu;
		*(UI_WINDOW_OBJECT **)data = &menu;
		break;

	default:
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_PULL_DOWN_ITEM::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_PULL_DOWN_ITEM::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_PULL_DOWN_ITEM::NewFunction(void) { return (UIW_PULL_DOWN_ITEM::New); }

UIW_PULL_DOWN_ITEM::UIW_PULL_DOWN_ITEM(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BUTTON(0, 0, 10, ZIL_NULLP(ZIL_ICHAR), BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D,
	WOF_JUSTIFY_CENTER),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
	// Initialize the pull-down item information.
	UIW_PULL_DOWN_ITEM::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_PULL_DOWN_ITEM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::DataSet(text);
}

void UIW_PULL_DOWN_ITEM::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Read the file information.
	UIW_BUTTON::Load(name, directory, file, objectTable, userTable);
	menu.Load(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
}
#endif

#if defined(ZIL_STORE)
void UIW_PULL_DOWN_ITEM::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the pull-down item information.
	UIW_BUTTON::Store(name, directory, file, objectTable, userTable);
	menu.Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
}
#endif
