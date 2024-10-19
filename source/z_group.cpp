//	Zinc Interface Library - Z_GROUP.CPP
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

// ----- UIW_GROUP ----------------------------------------------------------

UIW_GROUP::UIW_GROUP(int left, int top, int width, int height, ZIL_ICHAR *_text,
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags), text(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the group information.
	wnFlags |= _wnFlags;
	DataSet(_text);
	UIW_GROUP::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_GROUP::~UIW_GROUP(void)
{
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
}

ZIL_ICHAR *UIW_GROUP::DataGet(void)
{
	return (text);
}

void UIW_GROUP::DataSet(ZIL_ICHAR *_text)
{
	// Reset the group string and check for a hot key.
	if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = _text;
	else
		text = strdup(_text);

	// Check for a hotkey.
	hotKey = HotKey(text);
	if (!hotKey)
		hotKey = HOT_KEY_SUB_WINDOW;

	// Update the string with the environment.
	UIW_GROUP::OSDataSet();
}

void *UIW_GROUP::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_GROUP;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_GROUP);
		hotKey = HotKey(text);
		if (!hotKey)
			hotKey = HOT_KEY_SUB_WINDOW;

		// Update the internal flag settings.
		UIW_GROUP::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_GROUP);

		// Update the internal flag settings.
		UIW_GROUP::UpdateSettings(objectID);
		break;

	case I_GET_TEXT:
		// Get the group text.
		if (!data)
			return (DataGet());
		*(ZIL_ICHAR **)data = DataGet();
		break;

	case I_COPY_TEXT:
		// Copy the group text.
		data = strcpy((ZIL_ICHAR *)data, DataGet());
		break;

	case I_SET_TEXT:
		// Set the group text.
		DataSet((ZIL_ICHAR *)data);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_GROUP::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_GROUP::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_GROUP::NewFunction(void) { return (UIW_GROUP::New); }

UIW_GROUP::UIW_GROUP(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 7, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS)
{
	// Initialize the group information.
	UIW_GROUP::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_GROUP::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_GROUP::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the group information.
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
	file->Load(&text);
}
#endif

#if defined(ZIL_STORE)
void UIW_GROUP::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the group information.
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
	file->Store(text);
}
#endif
