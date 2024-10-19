//	Zinc Interface Library - Z_PLLDN1.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_PULL_DOWN_ITEM
#include "p_window.hpp"

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_PULL_DOWN_ITEM_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_PULL_DOWN_ITEM *object = (UIW_PULL_DOWN_ITEM *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_CALLBACK;
	woFlagsAllowed = WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_TEXT, object->text);
	int value = (int)object->value;
	SetValue(FIELD_VALUE, value);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SORT, object->menu.wnFlags, WNF_AUTO_SORT);
	SetFlag(FLAG_NO_WRAP, object->menu.wnFlags, WNF_NO_WRAP);
	SetFlag(FLAG_SELECT_MULTIPLE, object->menu.wnFlags, WNF_SELECT_MULTIPLE);
	SetFlag(FLAG_SEND_MESSAGE, object->btFlags, BTF_SEND_MESSAGE);
	// All other WNF_FLAGS and BTF_FLAGS are automatic.
}

int ZAF_OBJECT_EDITOR::UIW_PULL_DOWN_ITEM_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_PULL_DOWN_ITEM *object = (UIW_PULL_DOWN_ITEM *)data;

	// Store the object data.
	int value;
	int changed = FALSE;
	if (GetText(FIELD_TEXT, &object->text))
		object->DataSet(object->text); // Needed to update hotkey.
	if (GetValue(FIELD_VALUE, &value))
	{
		changed |= TRUE;
		object->value = (EVENT_TYPE)value;
	}
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	if (GetFlag(FLAG_AUTO_SORT, &object->menu.wnFlags, WNF_AUTO_SORT))
	{
		changed |= TRUE;
		if (FlagSet(object->menu.wnFlags, WNF_AUTO_SORT))
		{
			object->menu.compareFunction = UIW_WINDOW::StringCompare;
			object->menu.Sort();
		}
		else
			object->menu.compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION);
	}
	changed |= GetFlag(FLAG_NO_WRAP, &object->menu.wnFlags, WNF_NO_WRAP);
	changed |= GetFlag(FLAG_SELECT_MULTIPLE, &object->menu.wnFlags, WNF_SELECT_MULTIPLE);
	changed |= GetFlag(FLAG_SEND_MESSAGE, &object->btFlags, BTF_SEND_MESSAGE);

	// Return the status.
	return (changed);
}

// ----- ZAF_PULL_DOWN_ITEM -------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_POP_UP_ITEM, // selection
	ID_END // end
};

ZAF_PULL_DOWN_ITEM::ZAF_PULL_DOWN_ITEM(ZIL_ICHAR *_text,
	WNF_FLAGS _wnFlags, ZIL_USER_FUNCTION _userFunction, EVENT_TYPE _value) :
	UIW_PULL_DOWN_ITEM(_text, _wnFlags, _userFunction, _value)
{
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_PULL_DOWN_ITEM::ZAF_PULL_DOWN_ITEM(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_PULL_DOWN_ITEM(name, file, object, objectTable, userTable)
{
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_PULL_DOWN_ITEM::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		menu.support.Index(event.windowObject) == -1 &&
		menu.UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idTable[i] != ID_END; i++)
			if (_idTable[i] == objectID)
			{
				ccode = UIW_PULL_DOWN_ITEM::Event(event);
				Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		menu.support.Index(event.windowObject) == -1 &&
		menu.UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_PULL_DOWN_ITEM::Event(event);
	return (ccode);
}

void *ZAF_PULL_DOWN_ITEM::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_PULL_DOWN_ITEM::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_PULL_DOWN_ITEM::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_PULL_DOWN_ITEM(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_PULL_DOWN_ITEM *item;
	if (file)
		item = new ZAF_PULL_DOWN_ITEM(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "item");
		item = new ZAF_PULL_DOWN_ITEM(_text1);
	}
	return (item);
}

