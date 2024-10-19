//	Zinc Interface Library - Z_COMBO.CPP
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

// ----- UIW_COMBO_BOX ------------------------------------------------------

UIW_COMBO_BOX::UIW_COMBO_BOX(int left, int top, int width, int height,
	ZIL_COMPARE_FUNCTION _compareFunction, WNF_FLAGS _wnFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(left, top, width, 1, _woFlags, _woAdvancedFlags),
	list(0, 0, width, display->isText ? height + 2 : height, _compareFunction,
	_wnFlags | WNF_AUTO_SELECT, _woFlags | WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	text(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the combo-box information.
	wnFlags = _wnFlags | WNF_AUTO_SELECT;
	UIW_COMBO_BOX::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_COMBO_BOX::UIW_COMBO_BOX(int left, int top, int width, int height,
	ZIL_COMPARE_FUNCTION _compareFunction, WOF_FLAGS flagSetting, UI_ITEM *item) :
	UIW_WINDOW(left, top, width, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	list(0, 0, width, display->isText ? height + 2 : height, _compareFunction,
	WNF_NO_WRAP | WNF_AUTO_SELECT, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY), text(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the combo-box information.
	wnFlags |= WNF_NO_WRAP | WNF_AUTO_SELECT;
	UIW_COMBO_BOX::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Add the string items.
	for (int i = 0; item[i].text; i++)
	{
		UIW_STRING *string = new UIW_STRING(0, 0, width, item[i].text, -1,
			item[i].flags, WOF_BORDER | WOF_AUTO_CLEAR,
			(ZIL_USER_FUNCTION)item[i].data);
		if (FlagSet(flagSetting, item[i].value))
			string->woStatus |= WOS_SELECTED;
		list.Add(string);
	}
}

UIW_COMBO_BOX::~UIW_COMBO_BOX(void)
{
	if (text)
		delete text;
}

UI_WINDOW_OBJECT *UIW_COMBO_BOX::Add(UI_WINDOW_OBJECT *object)
{
	// Add the list-item searchID.
	if (object)
	{
		object->WindowID(ID_LIST_ITEM);
#if defined(ZIL_MOTIF)
		if (FlagSet(object->woAdvancedFlags, WOAF_COPY_DRAG_OBJECT))
			woAdvancedFlags |= WOAF_COPY_DRAG_OBJECT;
		if (FlagSet(object->woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT))
			woAdvancedFlags |= WOAF_MOVE_DRAG_OBJECT;
#endif
	}
	// Add the item to the list.
	return (list.Add(object));
}

ZIL_ICHAR *UIW_COMBO_BOX::DataGet(void)
{
	if (!text)
		text = new ZIL_ICHAR[256];
	if (!screenID || FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		list.Information(I_COPY_TEXT, text);
	else
		UIW_COMBO_BOX::OSDataGet();

	return text;
}

void UIW_COMBO_BOX::DataSet(ZIL_ICHAR *_text)
{
	if (!text)
		text = new ZIL_ICHAR[256];
	strcpy(text, _text ? _text : _blankString);
	// Try to find a matching element.
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
	{
		ZIL_ICHAR *ctext = ZIL_NULLP(ZIL_ICHAR);
		object->Information(I_GET_TEXT, &ctext);
		if (ctext && !stricmp(text, ctext))
		{
			Add(object);
			Event(S_REDISPLAY);
			break;
		}
	}
	UIW_COMBO_BOX::OSDataSet();
}

void UIW_COMBO_BOX::Destroy(void)
{
	// Destroy the list elements (high level).
	UIW_COMBO_BOX::OSDestroy();

	// Destroy the list elements (low level).
	list.Destroy();
}

void *UIW_COMBO_BOX::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_COMBO_BOX;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		WindowID(ID_LIST);
		searchID = WindowID(ID_COMBO_BOX);
		list.WindowID(ID_COMBO_BOX);

		font = FNT_DIALOG_FONT;
		list.parent = this;
		list.wnFlags |= WNF_AUTO_SELECT;
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
		// Add the combo fields.
		button = new UIW_COMBO_BUTTON;
		UIW_WINDOW::Add(button);
		string = new UIW_COMBO_STRING;
		UIW_WINDOW::Add(string);
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY))
			string->woFlags |= WOF_VIEW_ONLY;
#elif defined(ZIL_MACINTOSH)
		string = ZIL_NULLP(UIW_STRING);
#endif
		// Update the internal flag settings.
		UIW_COMBO_BOX::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:

	{
		ZIL_ICHAR *tempText=0;
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			tempText = strdup(DataGet());
		// Check the flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_COMBO_BOX);
		list.Information(I_CHANGED_FLAGS, data, ID_COMBO_BOX);

		// Update the internal flag settings.
		UIW_COMBO_BOX::UpdateSettings(objectID);
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
		{
			DataSet(tempText);
			delete tempText;
		}
	}
		break;

	case I_GET_BITMAP_ARRAY:
	case I_SET_BITMAP_ARRAY:
	case I_GET_FIRST:
	case I_GET_LAST:
	case I_GET_CURRENT:
	case I_GET_SUPPORT_FIRST:
	case I_GET_SUPPORT_LAST:
	case I_GET_SUPPORT_CURRENT:
		if (objectID == ID_WINDOW)
			data = UIW_WINDOW::Information(request, data, objectID);
		else
			data = list.Information(request, data, objectID);
		break;

	case I_GET_SUBWINDOW:
		if (!data)
			return &list;
		*(UI_WINDOW_OBJECT **)data = &list;
		break;

	case I_GET_TEXT:
		// Get the string text.
		if (!data)
			return (UIW_COMBO_BOX::DataGet());
		*(ZIL_ICHAR **)data = UIW_COMBO_BOX::DataGet();
		break;

	case I_COPY_TEXT:
		// Copy the string text.
		data = strcpy((ZIL_ICHAR *)data, UIW_COMBO_BOX::DataGet());
		break;

	case I_SET_TEXT:
		// Set the string text.
		UIW_COMBO_BOX::DataSet((ZIL_ICHAR *)data);
		break;

	case I_MATCH_INPUT:
		UIW_COMBO_BOX::OSMatchInput();
		break;

	case I_RESET_SELECTION:
		UIW_COMBO_BOX::OSResetSelection((UI_WINDOW_OBJECT *)data);
		break;

	case I_DESTROY_LIST:
		Destroy();
		break;

	case I_GET_NUMBERID_OBJECT:
	case I_GET_STRINGID_OBJECT:
		{
		void *match = UI_WINDOW_OBJECT::Information(request, data, objectID);
		if (!match)
			match = list.Information(request, data, objectID);
		data = match;
		}
		break;

	case I_PRINT_INFORMATION:
	case I_PRINT_USER_FUNCTION:
		UI_WINDOW_OBJECT::Information(request, data, objectID);
		list.Information(request, data, objectID);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_COMBO_BOX::Sort(void)
{
	list.UI_LIST::Sort();
	UIW_COMBO_BOX::OSSort();
}

UI_WINDOW_OBJECT *UIW_COMBO_BOX::Subtract(UI_WINDOW_OBJECT *object)
{
	return (list.Subtract(object));
}

void UIW_COMBO_BOX::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the general flag settings.
	if (FlagSet(list.wnFlags, WNF_BITMAP_CHILDREN))
		wnFlags |= WNF_BITMAP_CHILDREN;
	else
		wnFlags &= ~WNF_BITMAP_CHILDREN;
	if (compareFunction)
		list.compareFunction = compareFunction;
	if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
	{
		woStatus |= WOS_OWNERDRAW;
		list.woStatus |= WOS_OWNERDRAW;
	}
	else
	{
		woStatus &= ~WOS_OWNERDRAW;
		list.woStatus &= ~WOS_OWNERDRAW;
	}

	// Check the environment specific flag settings.
	UIW_COMBO_BOX::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_COMBO_BOX::NewFunction(void) { return (UIW_COMBO_BOX::New); }

UIW_COMBO_BOX::UIW_COMBO_BOX(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 1, WOF_NO_FLAGS),
	list(0, 0, 20, display->isText ? 7 : 5, ZIL_NULLF(ZIL_COMPARE_FUNCTION), WNF_NO_WRAP | WNF_AUTO_SELECT,
	WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	text(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the text information.
	UIW_COMBO_BOX::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_COMBO_BOX::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_COMBO_BOX::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the combo-box information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	list.Load(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
	wnFlags |= list.wnFlags;
}
#endif

#if defined(ZIL_STORE)
void UIW_COMBO_BOX::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the combo-box information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	list.Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
}
#endif
