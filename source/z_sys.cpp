//	Zinc Interface Library - Z_SYS.CPP
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

static ZIL_ICHAR _stringID[] = { 'N','U','M','I','D','_','S','Y','S','T','E','M', 0 };

#define ZIL_UIW_SYSTEM_BUTTON_DATA
#include "lang_def.cpp"
#include "img_def.cpp"

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

UIW_SYSTEM_BUTTON::UIW_SYSTEM_BUTTON(UI_ITEM *item) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_FLAGS | BTF_STATIC_BITMAPARRAY,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	ZIL_NULLF(ZIL_USER_FUNCTION), 0, ZIL_NULLP(ZIL_ICHAR)),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	syFlags(SYF_NO_FLAGS)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	systemMenu = ZIL_NULLP(UIW_POP_UP_MENU);
#endif

	// Initialize the system button information.
	UIW_SYSTEM_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Add the pop-up menu items.
	for (int i = 0; item[i].text; i++)
		Add(new UIW_POP_UP_ITEM(item[i].text, item[i].flags, BTF_NO_3D,
			WOF_NO_FLAGS, (ZIL_USER_FUNCTION)item[i].data, item[i].value));
}

UIW_SYSTEM_BUTTON::UIW_SYSTEM_BUTTON(SYF_FLAGS _syFlags) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_FLAGS,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	ZIL_NULLF(ZIL_USER_FUNCTION), 0, ZIL_NULLP(ZIL_ICHAR)),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	syFlags(_syFlags)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	systemMenu = ZIL_NULLP(UIW_POP_UP_MENU);
#endif

	// Initialize the system button information.
	UIW_SYSTEM_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_SYSTEM_BUTTON::~UIW_SYSTEM_BUTTON(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);
	if (systemMenu)
		delete systemMenu;
#endif

	text = ZIL_NULLP(ZIL_ICHAR);		   	// text is a static string.
	bitmapArray = ZIL_NULLP(ZIL_UINT8);		// bitmapArray is a static array.
}

UIW_SYSTEM_BUTTON *UIW_SYSTEM_BUTTON::Generic(void)
{
	return (new UIW_SYSTEM_BUTTON(SYF_GENERIC));
}

void *UIW_SYSTEM_BUTTON::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_SYSTEM_BUTTON;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Check the default initialization.
		if (!defaultInitialized)
		{
			ZIL_LANGUAGE_MANAGER::SetLanguage(_className, STRINGS);
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
			ZIL_DECORATION_MANAGER::SetDecorations(_className, TEXT_TABLE, BITMAP_TABLE);
#endif
			defaultInitialized = TRUE;
		}
		myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className);
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
		myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className);
#endif

		// Set the object identification and variables.
		searchID = WindowID(ID_SYSTEM_BUTTON);
		numberID = NUMID_SYSTEM;
		StringID(_stringID);

		woAdvancedFlags |= WOAF_NON_CURRENT;
		hotKey = HOT_KEY_SYSTEM;
		depth = 1;

		menu.parent = this;

		// Update the internal flag settings.
		UIW_SYSTEM_BUTTON::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UIW_SYSTEM_BUTTON::UpdateSettings(objectID);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the button flag settings.
		if (objectID && objectID != ID_SYSTEM_BUTTON)
			data = UIW_BUTTON::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &syFlags;
		else if (request == I_GET_FLAGS)
			*(SYF_FLAGS *)data = syFlags;
		else if (request == I_SET_FLAGS)
			syFlags |= *(SYF_FLAGS *)data;
		else
			syFlags &= ~(*(SYF_FLAGS *)data);
		break;

	case I_PRINT_INFORMATION:
	case I_PRINT_USER_FUNCTION:
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

	default:
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_SYSTEM_BUTTON::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_SYSTEM_BUTTON::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_SYSTEM_BUTTON::NewFunction(void) { return (UIW_SYSTEM_BUTTON::New); }

UIW_SYSTEM_BUTTON::UIW_SYSTEM_BUTTON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_FLAGS,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	systemMenu = ZIL_NULLP(UIW_POP_UP_MENU);
#endif

	// Initialize the system button information.
	UIW_SYSTEM_BUTTON::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_SYSTEM_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_SYSTEM_BUTTON::Load(const ZIL_ICHAR *, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the system button information.
	file->Load(&syFlags);
	if (!FlagSet(syFlags, SYF_GENERIC))
		menu.Load(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
#if defined(ZIL_EDIT)
	if (FlagSet(menu.woStatus, WOS_EDIT_MODE))
		menu.designerAdvancedFlags = menu.woAdvancedFlags;
#endif
}
#endif

#if defined(ZIL_STORE)
void UIW_SYSTEM_BUTTON::Store(const ZIL_ICHAR *, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the system button information.
	file->Store(syFlags);
	if (!FlagSet(syFlags, SYF_GENERIC))
		menu.Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);

	// Flag the object as being used.
	if (!objectTable)
		objectTable = UI_WINDOW_OBJECT::objectTable;
	for (int i = 0; objectTable && objectTable[i].value != ID_END; i++)
		if (objectTable[i].value == searchID)
		{
			objectTable[i].flags = TRUE;
			break;
		}
}
#endif

