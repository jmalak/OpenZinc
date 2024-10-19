//	Zinc Interface Library - Z_VLIST.CPP
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

// ----- UIW_VT_LIST --------------------------------------------------------

UIW_VT_LIST::UIW_VT_LIST(int left, int top, int width, int height,
	ZIL_COMPARE_FUNCTION _compareFunction, WNF_FLAGS _wnFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags)
{
	// Initialize the list information.
	wnFlags |= _wnFlags;
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	if (_compareFunction)
		compareFunction = _compareFunction;
	UIW_VT_LIST::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_VT_LIST::UIW_VT_LIST(int left, int top, int width, int height,
	ZIL_COMPARE_FUNCTION _compareFunction, WOF_FLAGS flagSetting, UI_ITEM *item) :
	UIW_WINDOW(left, top, width, height, WOF_BORDER, WOAF_NO_FLAGS)
{
	// Initialize the list information.
	wnFlags |= WNF_NO_WRAP;
	if (_compareFunction)
		compareFunction = _compareFunction;
	UIW_VT_LIST::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Add the string items.
	for (int i = 0; item[i].text; i++)
	{
		UIW_STRING *string = new UIW_STRING(0, 0, width, item[i].text, -1,
			item[i].flags, WOF_BORDER | WOF_AUTO_CLEAR, (ZIL_USER_FUNCTION)item[i].data);
		if (FlagSet(flagSetting, item[i].value))
			string->woStatus |= WOS_SELECTED;
		Add(string);
	}
}

UIW_VT_LIST::~UIW_VT_LIST(void)
{
	// No list information needs to be destroyed.
}

// Defined in A_VLIST.CPP and N_VLIST.CPP.
#if !defined(ZIL_MACINTOSH) && !defined(ZIL_NEXTSTEP)
UI_WINDOW_OBJECT *UIW_VT_LIST::Add(UI_WINDOW_OBJECT *object)
{
	// Add the list-item searchID.
	if (object)
	{
		if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
			object->WindowID(ID_LIST_ITEM);
#if defined(ZIL_MOTIF)
		if (FlagSet(object->woAdvancedFlags, WOAF_COPY_DRAG_OBJECT))
			woAdvancedFlags |= WOAF_COPY_DRAG_OBJECT;
		if (FlagSet(object->woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT))
			woAdvancedFlags |= WOAF_MOVE_DRAG_OBJECT;
#endif
	}
	// Add the item to the list.
	return (UIW_WINDOW::Add(object));
}
#endif

void UIW_VT_LIST::Destroy(void)
{
	// Destroy the list elements (high level).
	UIW_VT_LIST::OSDestroy();

	// Reset scroll information.
	vScrollInfo.minimum = vScrollInfo.current = scroll.top = 0;

	// Destroy the list elements (low level).
	UI_LIST::Destroy();
}

void *UIW_VT_LIST::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_VT_LIST;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		WindowID(ID_LIST);
		searchID = WindowID(ID_VT_LIST);
		font = FNT_DIALOG_FONT;
#if defined(ZIL_MOTIF)
		comboShell = 0;
#endif

		// Update the internal flag settings.
		UIW_VT_LIST::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_VT_LIST);

		// Update the internal flag settings.
		UIW_VT_LIST::UpdateSettings(objectID);
		break;

	case I_COPY_TEXT:
	case I_GET_TEXT:
	case I_SET_TEXT:
	case I_GET_BITMAP_ARRAY:
	case I_SET_BITMAP_ARRAY:
		return (current ? Current()->Information(request, data, objectID) : ZIL_NULLP(void));

	case I_DESTROY_LIST:
		Destroy();
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_VT_LIST::Sort(void)
{
	UI_LIST::Sort();
	UIW_VT_LIST::OSSort();
}

// Defined in A_VLIST.CPP and N_VLIST.CPP.
#if !defined(ZIL_MACINTOSH) && !defined(ZIL_NEXTSTEP)
UI_WINDOW_OBJECT *UIW_VT_LIST::Subtract(UI_WINDOW_OBJECT *object)
{
	return (UIW_WINDOW::Subtract(object));
}
#endif

void UIW_VT_LIST::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_VT_LIST::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_VT_LIST::NewFunction(void) { return (UIW_VT_LIST::New); }

UIW_VT_LIST::UIW_VT_LIST(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS)
{
	// Initialize the list information.
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_VT_LIST::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_VT_LIST::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the vertical list information.
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
}
#endif

#if defined(ZIL_STORE)
void UIW_VT_LIST::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the vertical list information.
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
}
#endif

