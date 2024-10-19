//	Zinc Interface Library - Z_POPUP.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_POP_UP_MENU
#include "p_window.hpp"

// ----- UIW_POP_UP_MENU ----------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_POP_UP_MENU_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_POP_UP_MENU *object = (UIW_POP_UP_MENU *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SORT, object->wnFlags, WNF_AUTO_SORT);
}

int ZAF_OBJECT_EDITOR::UIW_POP_UP_MENU_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_POP_UP_MENU *object = (UIW_POP_UP_MENU *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
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

// ----- ZAF_POP_UP_MENU ----------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_POP_UP_ITEM, // selection
	ID_END // end
};

ZAF_POP_UP_MENU::ZAF_POP_UP_MENU(int left, int top, WNF_FLAGS _wnFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_POP_UP_MENU(left, top, _wnFlags, _woFlags, _woAdvancedFlags)
{
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_POP_UP_MENU::ZAF_POP_UP_MENU(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_POP_UP_MENU(name, file, object, objectTable, userTable)
{
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_POP_UP_MENU::Event(const UI_EVENT &event)
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
				ccode = UIW_POP_UP_MENU::Event(event);
				Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_POP_UP_MENU::Event(event);
	return (ccode);
}

void *ZAF_POP_UP_MENU::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_POP_UP_MENU::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_POP_UP_MENU::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_POP_UP_MENU(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_POP_UP_MENU *menu;
	if (file)
		menu = new ZAF_POP_UP_MENU(name, file, object, objectTable, userTable);
	else
		menu = new ZAF_POP_UP_MENU(0, 0, WNF_NO_FLAGS);
	return (menu);
}

