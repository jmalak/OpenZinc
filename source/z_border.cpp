//	Zinc Interface Library - Z_BORDER.CPP
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

static ZIL_ICHAR _stringID[] = { 'N','U','M','I','D','_','B','O','R','D','E','R', 0 };

// ----- UIW_BORDER ---------------------------------------------------------

UIW_BORDER::UIW_BORDER(BDF_FLAGS _bdFlags) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	WOAF_OUTSIDE_REGION | WOAF_NON_CURRENT), bdFlags(_bdFlags)
{
	// Initialize the border information.
	UIW_BORDER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	if (display->isText)
		width = 1;
#   if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
	else
		width = 4;
#   elif defined(ZIL_MOTIF_STYLE)
	else
		width = 7;
#   endif
#endif
}

UIW_BORDER::~UIW_BORDER(void)
{
	// No border information needs to be destroyed.
}

int UIW_BORDER::DataGet(void)
{
	// Update the width with the environment.
	UIW_BORDER::OSDataGet();

	// Return the default border width.
	return (width);
}

void UIW_BORDER::DataSet(int newWidth)
{
	// Reset the default border width.
	width = (newWidth > 0) ? newWidth : 1;

	// Update the width with the environment.
	UIW_BORDER::OSDataSet();
}

void *UIW_BORDER::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_BORDER;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_BORDER);
		numberID = NUMID_BORDER;

		StringID(_stringID);
		woAdvancedFlags |= WOAF_NON_CURRENT;

		// Update the internal flag settings.
		UIW_BORDER::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UIW_BORDER::UpdateSettings(objectID);
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_BORDER::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_BORDER::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_BORDER::NewFunction(void) { return (UIW_BORDER::New); }

UIW_BORDER::UIW_BORDER(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	WOAF_OUTSIDE_REGION | WOAF_NON_CURRENT)
{
	// Initialize the border information.
	UIW_BORDER::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BORDER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_BORDER::Load(const ZIL_ICHAR *, ZIL_STORAGE_READ_ONLY *, ZIL_STORAGE_OBJECT_READ_ONLY *,
	UI_ITEM *, UI_ITEM *)
{
	// No border information needs to be loaded.
}
#endif

#if defined(ZIL_STORE)
void UIW_BORDER::Store(const ZIL_ICHAR *, ZIL_STORAGE *, ZIL_STORAGE_OBJECT *,
	UI_ITEM *objectTable, UI_ITEM *)
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
