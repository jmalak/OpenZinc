//	Zinc Interface Library - Z_TBAR.CPP
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

// ----- UIW_TOOL_BAR -------------------------------------------------------

UIW_TOOL_BAR::UIW_TOOL_BAR(int left, int top, int width, int height,
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags)
{
	// Initialize the menu information.
	wnFlags |= _wnFlags;
	UIW_TOOL_BAR::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_TOOL_BAR::~UIW_TOOL_BAR(void)
{
}

void *UIW_TOOL_BAR::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_TOOL_BAR;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_TOOL_BAR);
		if (!hotKey)
			hotKey = HOT_KEY_SUB_WINDOW;

		// Update the internal flag settings.
		UIW_TOOL_BAR::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_TOOL_BAR);

		// Update the internal flag settings.
		UIW_TOOL_BAR::UpdateSettings(objectID);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_TOOL_BAR::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_TOOL_BAR::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_TOOL_BAR::NewFunction(void) { return (UIW_TOOL_BAR::New); }

UIW_TOOL_BAR::UIW_TOOL_BAR(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS)
{
	// Initialize the text information.
	UIW_TOOL_BAR::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TOOL_BAR::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_TOOL_BAR::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the toolbar information.
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
}
#endif

#if defined(ZIL_STORE)
void UIW_TOOL_BAR::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the toolbar information.
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
}
#endif
