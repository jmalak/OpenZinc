//	Zinc Interface Library - Z_PLLDN.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_PULL_DOWN_MENU
#include "p_window.hpp"

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_PULL_DOWN_MENU_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_PULL_DOWN_MENU *object = (UIW_PULL_DOWN_MENU *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	SetValue(FIELD_INDENTATION, object->indentation);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SORT, object->wnFlags, WNF_AUTO_SORT);
}

int ZAF_OBJECT_EDITOR::UIW_PULL_DOWN_MENU_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_PULL_DOWN_MENU *object = (UIW_PULL_DOWN_MENU *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	changed |= GetValue(FIELD_INDENTATION, &object->indentation);
	// Store the object flags.
	if (GetFlag(FLAG_AUTO_SORT, &object->wnFlags, WNF_AUTO_SORT))
	{
		changed |= TRUE;
		if (FlagSet(object->wnFlags, WNF_AUTO_SORT))
		{
			object->compareFunction = UIW_WINDOW::StringCompare;
			object->Sort();
		}
		else
			object->compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION);
	}

	// Return the status.
	return (changed);
}

// ----- ZAF_PULL_DOWN_MENU -------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_PULL_DOWN_ITEM, // selection
	ID_END // end
};

ZAF_PULL_DOWN_MENU::ZAF_PULL_DOWN_MENU(int _indentation,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_PULL_DOWN_MENU(_indentation, _woFlags, _woAdvancedFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_PULL_DOWN_MENU::ZAF_PULL_DOWN_MENU(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_PULL_DOWN_MENU(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_PULL_DOWN_MENU::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idTable[i] != ID_END; i++)
			if (_idTable[i] == objectID)
			{
				ccode = UIW_PULL_DOWN_MENU::Event(event);
				Root()->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_SUBTRACT_OBJECT &&
		(support.First() == support.Last() && First() == Last()))
		ccode = S_ERROR; // cannot delete the last menu item.
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_PULL_DOWN_MENU::Event(event);
	return (ccode);
}

void *ZAF_PULL_DOWN_MENU::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_PULL_DOWN_MENU::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_PULL_DOWN_MENU::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_PULL_DOWN_MENU(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_PULL_DOWN_MENU *menu;
	if (file)
		menu = new ZAF_PULL_DOWN_MENU(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "item");
		menu = new ZAF_PULL_DOWN_MENU;
		*menu + new ZAF_PULL_DOWN_ITEM(_text1);
	}
	return (menu);
}

