//	Zinc Interface Library - Z_MAX.CPP
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

static ZIL_ICHAR _stringID[] = { 'N','U','M','I','D','_','M','A','X','I','M','I','Z','E', 0 };

#define ZIL_UIW_MAXIMIZE_BUTTON_DATA
#include "img_def.cpp"

// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------

UIW_MAXIMIZE_BUTTON::UIW_MAXIMIZE_BUTTON(void) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_SEND_MESSAGE | BTF_NO_TOGGLE | BTF_STATIC_BITMAPARRAY,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	ZIL_NULLF(ZIL_USER_FUNCTION), L_MAXIMIZE, ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the maximize button information.
	UIW_MAXIMIZE_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_MAXIMIZE_BUTTON::~UIW_MAXIMIZE_BUTTON(void)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);
#endif

	// Restore the maximize button information.
	text = ZIL_NULLP(ZIL_ICHAR);			// text is a static string.
	bitmapArray = ZIL_NULLP(ZIL_UINT8);		// bitmapArray is a static array.
}

void *UIW_MAXIMIZE_BUTTON::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_MAXIMIZE_BUTTON;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Check the default initialization.
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
		if (!defaultInitialized)
		{
			ZIL_DECORATION_MANAGER::SetDecorations(_className, TEXT_TABLE, BITMAP_TABLE);
			defaultInitialized = TRUE;
		}
		myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className);
#endif

		// Set the object identification and variables.
		searchID = WindowID(ID_MAXIMIZE_BUTTON);
		numberID = NUMID_MAXIMIZE;
		StringID(_stringID);

		woAdvancedFlags |= WOAF_NON_CURRENT;
		btFlags |= BTF_SEND_MESSAGE | BTF_NO_TOGGLE;
		hotKey = HOT_KEY_MAXIMIZE;
		depth = 1;

		// Update the internal flag settings.
		UIW_MAXIMIZE_BUTTON::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UIW_MAXIMIZE_BUTTON::UpdateSettings(objectID);
		break;

	default:
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_MAXIMIZE_BUTTON::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_MAXIMIZE_BUTTON::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_MAXIMIZE_BUTTON::NewFunction(void) { return (UIW_MAXIMIZE_BUTTON::New); }

UIW_MAXIMIZE_BUTTON::UIW_MAXIMIZE_BUTTON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_SEND_MESSAGE,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	ZIL_NULLF(ZIL_USER_FUNCTION), L_MAXIMIZE)
{
	// Initialize the maximize button information.
	UIW_MAXIMIZE_BUTTON::Load(name, directory, file, objectTable, userTable);
	UIW_MAXIMIZE_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_MAXIMIZE_BUTTON::Load(const ZIL_ICHAR *, ZIL_STORAGE_READ_ONLY *,
	ZIL_STORAGE_OBJECT_READ_ONLY *, UI_ITEM *, UI_ITEM *)
{
}
#endif

#if defined(ZIL_STORE)
void UIW_MAXIMIZE_BUTTON::Store(const ZIL_ICHAR *, ZIL_STORAGE *,
	ZIL_STORAGE_OBJECT *, UI_ITEM *objectTable, UI_ITEM *)
{
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
