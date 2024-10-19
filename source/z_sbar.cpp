//	Z_SBAR.CPP (STATUSBAR) - This file contains the status bar class.
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

// ----- UIW_STATUS_BAR -------------------------------------------------------

UIW_STATUS_BAR::UIW_STATUS_BAR(int _height, WOF_FLAGS _woFlags) :
	UIW_WINDOW(0, 0, 0, _height, _woFlags | WOF_NON_FIELD_REGION, WOAF_NON_CURRENT)
{
	// Initialize the status bar information.
	height = _height;
	UIW_STATUS_BAR::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_STATUS_BAR::~UIW_STATUS_BAR(void)
{
}

UI_WINDOW_OBJECT *UIW_STATUS_BAR::Add(UI_WINDOW_OBJECT *object)
{
	// Add the list-item searchID.
	if (object)
	{
		object->WindowID(ID_STATUS_ITEM);
#		if !defined (ZIL_MOTIF)
		object->woStatus |= WOS_OWNERDRAW;
#		endif	
	}
	// Add the item to the list.
	return (UIW_WINDOW::Add(object));
}

void *UIW_STATUS_BAR::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_STATUS_BAR;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_STATUS_BAR);
		woStatus |= WOS_OWNERDRAW;
		if (!hotKey)
			hotKey = HOT_KEY_SUB_WINDOW;

		// Update the internal flag settings.
		UIW_STATUS_BAR::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_STATUS_BAR);

		// Update the internal flag settings.
		UIW_STATUS_BAR::UpdateSettings(objectID);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

UI_WINDOW_OBJECT *UIW_STATUS_BAR::Subtract(UI_WINDOW_OBJECT *object)
{
	return (UIW_WINDOW::Subtract(object));
}

void UIW_STATUS_BAR::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_STATUS_BAR::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_STATUS_BAR::NewFunction(void) { return (UIW_STATUS_BAR::New); }

UIW_STATUS_BAR::UIW_STATUS_BAR(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS)
{
	// Initialize the text information.
	UIW_STATUS_BAR::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STATUS_BAR::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_STATUS_BAR::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the statusbar information.
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
	ZIL_INT16 _value; file->Load(&_value); height = _value;
}
#endif

#if defined(ZIL_STORE)
void UIW_STATUS_BAR::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the statusbar information.
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
	ZIL_INT16 _value = height; file->Store(_value);
}
#endif

