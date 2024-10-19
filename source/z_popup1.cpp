//	Zinc Interface Library - Z_POPUP1.CPP
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

#define ZIL_UIW_POP_UP_ITEM_DATA
#include "img_def.cpp"

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(void) :
	UIW_BUTTON(0, 0, 1, ZIL_NULLP(ZIL_ICHAR), BTF_NO_3D, WOF_NO_FLAGS,
	ZIL_NULLF(ZIL_USER_FUNCTION), 0, ZIL_NULLP(ZIL_ICHAR)),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(MNIF_SEPARATOR)
{
	// Initialize the pull-down item seperator information.
	UIW_POP_UP_ITEM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(ZIL_ICHAR *_text, MNIF_FLAGS _mniFlags,
	BTF_FLAGS _btFlags, WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction,
	EVENT_TYPE _value) :
	UIW_BUTTON(0, 0, 0, _text, _btFlags, _woFlags, _userFunction, _value,
	ZIL_NULLP(ZIL_ICHAR)),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(_mniFlags)
{
	// Initialize the pull-down item information.
	UIW_POP_UP_ITEM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(int left, int top, int width, ZIL_ICHAR *_text,
	MNIF_FLAGS _mniFlags, BTF_FLAGS _btFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction, EVENT_TYPE _value) :
	UIW_BUTTON(left, top, width, _text, _btFlags, _woFlags, _userFunction, _value, ZIL_NULLP(ZIL_ICHAR)),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(_mniFlags)
{
	// Initialize the pull-down item information.
	UIW_POP_UP_ITEM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_POP_UP_ITEM::~UIW_POP_UP_ITEM(void)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);
#endif

	// No item information needs to be destroyed.
}

void *UIW_POP_UP_ITEM::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_POP_UP_ITEM;
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
		searchID = WindowID(ID_POP_UP_ITEM);
		font = FNT_SYSTEM_FONT;
		woFlags &= ~WOF_BORDER;
		btFlags |= BTF_NO_3D;
		if (!text || text[0] == '\0')
		{
			mniFlags = MNIF_SEPARATOR;
			woFlags |= WOF_NON_SELECTABLE;
		}

		menu.parent = this;
		menu.woFlags |= WOF_BORDER;
		menu.woAdvancedFlags |= WOAF_TEMPORARY | WOAF_NO_DESTROY;

		// Update the internal flag settings.
		UIW_POP_UP_ITEM::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UIW_POP_UP_ITEM::UpdateSettings(objectID);
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

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the button flag settings.
		if (objectID && objectID != ID_POP_UP_ITEM)
			data = UIW_BUTTON::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &mniFlags;
		else if (request == I_GET_FLAGS)
			*(MNIF_FLAGS *)data = mniFlags;
		else if (request == I_SET_FLAGS)
			mniFlags |= *(MNIF_FLAGS *)data;
		else
			mniFlags &= ~(*(MNIF_FLAGS *)data);
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

void UIW_POP_UP_ITEM::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the general flag settings.
	if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		mniFlags |= MNIF_NON_SELECTABLE;

	// Check the flags with the number identifications.
	if (FlagSet(mniFlags, MNIF_RESTORE))
		numberID = NUMID_OPT_RESTORE;
	else if (FlagSet(mniFlags, MNIF_MOVE))
		numberID = NUMID_OPT_MOVE;
	else if (FlagSet(mniFlags, MNIF_SIZE))
		numberID = NUMID_OPT_SIZE;
	else if (FlagSet(mniFlags, MNIF_MINIMIZE))
		numberID = NUMID_OPT_MINIMIZE;
	else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
		numberID = NUMID_OPT_MAXIMIZE;
	else if (FlagSet(mniFlags, MNIF_CLOSE))
		numberID = NUMID_OPT_CLOSE;
	else if (FlagSet(mniFlags, MNIF_SWITCH))
		numberID = NUMID_OPT_SWITCH;

	// Check for system commands
	if (FlagSet(mniFlags, 0x00FF) && mniFlags != MNIF_SEPARATOR)
		btFlags |= BTF_SEND_MESSAGE;

	if ((FlagSet(btFlags, BTF_SEND_MESSAGE) || FlagSet(mniFlags, MNIF_SEND_MESSAGE)) &&
		!userFunction)
	{
		btFlags |= BTF_SEND_MESSAGE;
		mniFlags |= MNIF_SEND_MESSAGE;
		userFunction = UIW_BUTTON::Message;
	}
	if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		mniFlags |= MNIF_NON_SELECTABLE;
	else
		mniFlags &= ~MNIF_NON_SELECTABLE;

	// Check the environment specific flag settings.
	UIW_POP_UP_ITEM::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_POP_UP_ITEM::NewFunction(void) { return (UIW_POP_UP_ITEM::New); }

UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BUTTON(0, 0, 1, ZIL_NULLP(ZIL_ICHAR), BTF_NO_3D, WOF_NO_FLAGS),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(MNIF_NO_FLAGS)
{
	// Initialize the pop-up item information.
	UIW_POP_UP_ITEM::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_POP_UP_ITEM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::DataSet(text);
}

void UIW_POP_UP_ITEM::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the pop-up item information.
	UIW_BUTTON::Load(name, directory, file, objectTable, userTable);
	file->Load(&mniFlags);
	menu.Load(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
}
#endif

#if defined(ZIL_STORE)
void UIW_POP_UP_ITEM::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the pop-up item information.
	UIW_BUTTON::Store(name, directory, file, objectTable, userTable);
	file->Store(mniFlags);
	menu.Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
}
#endif
