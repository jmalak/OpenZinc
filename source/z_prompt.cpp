//	Zinc Interface Library - Z_PROMPT.CPP
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

// ----- UIW_PROMPT ---------------------------------------------------------

UIW_PROMPT::UIW_PROMPT(int left, int top, ZIL_ICHAR *_text, WOF_FLAGS _woFlags) :
	UI_WINDOW_OBJECT(left, top, 0, 1, (_woFlags & ~(WOF_JUSTIFY_RIGHT|WOF_JUSTIFY_CENTER)) | WOF_NON_SELECTABLE, WOAF_NO_FLAGS),
	text(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the prompt information.
	UIW_PROMPT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_PROMPT::DataSet(_text);
}

UIW_PROMPT::UIW_PROMPT(int left, int top, int width, ZIL_ICHAR *_text, WOF_FLAGS _woFlags) :
	UI_WINDOW_OBJECT(left, top, width, 1, _woFlags | WOF_NON_SELECTABLE, WOAF_NO_FLAGS),
	text(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the prompt information.
	UIW_PROMPT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_PROMPT::DataSet(_text);
}

UIW_PROMPT::~UIW_PROMPT(void)
{
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
}

ZIL_ICHAR *UIW_PROMPT::DataGet(void)
{
	return (text);
}

void UIW_PROMPT::DataSet(ZIL_ICHAR *_text)
{
	// Reset the prompt and check for a hot key.
	if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = _text;
	else
		text = strdup(_text);
	hotKey = HotKey(text);

	// Update the string with the environment.
	UIW_PROMPT::OSDataSet();
}

void *UIW_PROMPT::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_PROMPT;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_PROMPT);

		hotKey = HotKey(text);

		// Update the internal flag settings.
		UIW_PROMPT::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UI_WINDOW_OBJECT::Information(I_CHANGED_FLAGS, data, ID_PROMPT);

		// Update the internal flag settings.
		UIW_PROMPT::UpdateSettings(objectID);
		break;

	case I_GET_TEXT:
		// Get the prompt text.
		if (!data)
			return (DataGet());
		*(ZIL_ICHAR **)data = DataGet();
		break;

	case I_COPY_TEXT:
		// Copy the prompt text.
		data = strcpy((ZIL_ICHAR *)data, DataGet());
		break;

	case I_SET_TEXT:
		// Set the prompt text.
		DataSet((ZIL_ICHAR *)data);
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_PROMPT::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_PROMPT::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_PROMPT::NewFunction(void) { return (UIW_PROMPT::New); }

UIW_PROMPT::UIW_PROMPT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 0, 1, WOF_NON_SELECTABLE, WOAF_NO_FLAGS)
{
	// Initialize the prompt information.
	UIW_PROMPT::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_PROMPT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_PROMPT::DataSet(text);
}

void UIW_PROMPT::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Read the prompt information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	file->Load(&text);
}
#endif

#if defined(ZIL_STORE)
void UIW_PROMPT::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Write the prompt information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(text);
}
#endif

