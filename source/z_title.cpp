//	Zinc Interface Library - Z_TITLE.CPP
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

static ZIL_ICHAR _stringID[] = { 'N','U','M','I','D','_','T','I','T','L','E', 0 };

// ----- UIW_TITLE ----------------------------------------------------------

UIW_TITLE::UIW_TITLE(ZIL_ICHAR *text, WOF_FLAGS _woFlags) :
	UIW_BUTTON(0, 0, 0, text, BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D,
	_woFlags | WOF_BORDER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION)
{
#if defined(ZIL_MOTIF) && defined(ZIL_UNICODE)
	mappedTitle = ZIL_NULLP(char);
#endif

	// Initialize the title information.
	UIW_TITLE::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_TITLE::~UIW_TITLE(void)
{
#if defined(ZIL_MOTIF) && defined(ZIL_UNICODE)
	if (mappedTitle)
		delete mappedTitle;
#endif
}

ZIL_ICHAR *UIW_TITLE::DataGet(void)
{
	// Return the text information.
 	return (text);
}

void UIW_TITLE::DataSet(ZIL_ICHAR *_text)
{
	// Reset the title.
	if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = _text;
	else
		text = strdup(_text);

	// Update the string with the environment.
	UIW_TITLE::OSDataSet();
}

void *UIW_TITLE::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_TITLE;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_TITLE);
		numberID = NUMID_TITLE;
		StringID(_stringID);

		woAdvancedFlags |= WOAF_NON_CURRENT;
		font = FNT_SYSTEM_FONT | FNT_IGNORE_UNDERSCORE;
		depth = 0;

		// Update the internal flag settings.
		UIW_TITLE::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UIW_TITLE::UpdateSettings(objectID);
		break;

	case I_GET_TEXT:
		// Get the title text.
		if (!data)
			return (DataGet());
		*(ZIL_ICHAR **)data = DataGet();
		break;

	case I_COPY_TEXT:
		// Copy the title text.
		data = strcpy((ZIL_ICHAR *)data, DataGet() ? DataGet() : _blankString);
		break;

	case I_SET_TEXT:
		// Set the title text.
		DataSet((ZIL_ICHAR *)data);
		break;

	default:
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_TITLE::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_TITLE::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_TITLE::NewFunction(void) { return (UIW_TITLE::New); }

UIW_TITLE::UIW_TITLE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION)
{
#if defined(ZIL_MOTIF) && defined(ZIL_UNICODE)
	mappedTitle = ZIL_NULLP(char);
#endif

	// Initialize the title information.
	UIW_TITLE::Load(name, directory, file, objectTable, userTable);
	UIW_TITLE::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_TITLE::Load(const ZIL_ICHAR *, ZIL_STORAGE_READ_ONLY *, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *, UI_ITEM *)
{
	// Load the title information.
	file->Load(&text);
}
#endif

#if defined(ZIL_STORE)
void UIW_TITLE::Store(const ZIL_ICHAR *, ZIL_STORAGE *, ZIL_STORAGE_OBJECT *file,
	UI_ITEM *objectTable, UI_ITEM *)
{
	// Store the title information.
	file->Store(text);

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
