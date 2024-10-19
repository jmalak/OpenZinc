//	Zinc Interface Library - Z_POPUP1.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_POP_UP_ITEM
#include "p_window.hpp"

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_POP_UP_ITEM_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_POP_UP_ITEM *object = (UIW_POP_UP_ITEM *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_CALLBACK;
	woFlagsAllowed = WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_TEXT, object->text);
	int value = (int)object->value;
	SetValue(FIELD_VALUE, value);
	SetText(FIELD_MENU_NAME, object->menu.StringID());
	value = object->menu.NumberID();
	SetValue(FIELD_MENU_ID, value);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SORT, object->menu.wnFlags, WNF_AUTO_SORT);
	SetFlag(FLAG_NO_WRAP, object->menu.wnFlags, WNF_NO_WRAP);
	SetFlag(FLAG_SELECT_MULTIPLE, object->menu.wnFlags, WNF_SELECT_MULTIPLE);
	SetFlag(FLAG_CHECK_MARK, object->mniFlags, MNIF_CHECK_MARK);
	SetFlag(FLAG_SEND_MESSAGE, object->mniFlags, MNIF_SEND_MESSAGE);
	SetFlag(FLAG_SEPARATOR, object->mniFlags, MNIF_SEPARATOR);
	// type
	MNIF_FLAGS mniMask = MNIF_MAXIMIZE | MNIF_MINIMIZE | MNIF_MOVE |
		MNIF_SIZE | MNIF_SWITCH | MNIF_RESTORE | MNIF_CLOSE | MNIF_ABOUT;
	SetFlag(FLAG_MNIF_NORMAL, FlagSet(object->mniFlags, mniMask), 0);
	SetFlag(FLAG_MNIF_ABOUT, object->mniFlags, MNIF_ABOUT);
	SetFlag(FLAG_MNIF_MAXIMIZE, object->mniFlags, MNIF_MAXIMIZE);
	SetFlag(FLAG_MNIF_MINIMIZE, object->mniFlags, MNIF_MINIMIZE);
	SetFlag(FLAG_MNIF_MOVE, object->mniFlags, MNIF_MOVE);
	SetFlag(FLAG_MNIF_SIZE, object->mniFlags, MNIF_SIZE);
	SetFlag(FLAG_MNIF_SWITCH, object->mniFlags, MNIF_SWITCH);
	SetFlag(FLAG_MNIF_RESTORE, object->mniFlags, MNIF_RESTORE);
	SetFlag(FLAG_MNIF_CLOSE, object->mniFlags, MNIF_CLOSE);
}

int ZAF_OBJECT_EDITOR::UIW_POP_UP_ITEM_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_POP_UP_ITEM *object = (UIW_POP_UP_ITEM *)data;

	// Store the object data.
	int value;
	int changed = FALSE;
	if (GetText(FIELD_TEXT, &object->text))
	{
		// Check for entered tab character sequence and convert.
		ZIL_ICHAR *temp = object->text;
		for (int i = 0; i < strlen(temp); i++)
			if (temp[i] == '\\' && temp[i+1] == 't')
			{
				temp[i++] = '\t';
				memmove(&temp[i], &temp[i+1], (strlen(&temp[i+1])+1) * sizeof(ZIL_ICHAR));
				break;
			}
	}
		object->DataSet(object->text); // Needed to update hotkey.
	if (GetValue(FIELD_VALUE, &value))
	{
		changed |= TRUE;
		object->value = (EVENT_TYPE)value;
	}
	changed |= GetText(FIELD_MENU_NAME, object->menu.StringID());
	if (GetValue(FIELD_MENU_ID, &value))
	{
		changed |= TRUE;
		object->menu.numberID = (ZIL_NUMBERID)value;
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
	changed |= GetFlag(FLAG_CHECK_MARK, &object->mniFlags, MNIF_CHECK_MARK);
	changed |= GetFlag(FLAG_SEND_MESSAGE, &object->mniFlags, MNIF_SEND_MESSAGE);
	changed |= GetFlag(FLAG_SEPARATOR, &object->mniFlags, MNIF_SEPARATOR);
	// type
	changed |= GetFlag(FLAG_MNIF_ABOUT, &object->mniFlags, MNIF_ABOUT);
	changed |= GetFlag(FLAG_MNIF_MAXIMIZE, &object->mniFlags, MNIF_MAXIMIZE);
	changed |= GetFlag(FLAG_MNIF_MINIMIZE, &object->mniFlags, MNIF_MINIMIZE);
	changed |= GetFlag(FLAG_MNIF_MOVE, &object->mniFlags, MNIF_MOVE);
	changed |= GetFlag(FLAG_MNIF_SIZE, &object->mniFlags, MNIF_SIZE);
	changed |= GetFlag(FLAG_MNIF_SWITCH, &object->mniFlags, MNIF_SWITCH);
	changed |= GetFlag(FLAG_MNIF_RESTORE, &object->mniFlags, MNIF_RESTORE);
	changed |= GetFlag(FLAG_MNIF_CLOSE, &object->mniFlags, MNIF_CLOSE);

	// Check overlapping flags.
	if (FlagSet(object->mniFlags, MNIF_SEND_MESSAGE))
		object->btFlags |= BTF_SEND_MESSAGE;
	else
		object->btFlags &= ~BTF_SEND_MESSAGE;
	if (FlagSet(object->woFlags, WOF_NON_SELECTABLE))
		object->mniFlags |= MNIF_NON_SELECTABLE;
	else
		object->mniFlags &= ~MNIF_NON_SELECTABLE;

	// Return the status.
	return (changed);
}

// ----- ZAF_POP_UP_ITEM ----------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_POP_UP_ITEM, // selection
	ID_END // end
};

ZAF_POP_UP_ITEM::ZAF_POP_UP_ITEM(ZIL_ICHAR *_text, MNIF_FLAGS _mniFlags,
	BTF_FLAGS _btFlags, WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction,
	EVENT_TYPE _value) :
	UIW_POP_UP_ITEM(_text, _mniFlags, _btFlags, _woFlags, _userFunction, _value)
{
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_POP_UP_ITEM::ZAF_POP_UP_ITEM(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_POP_UP_ITEM(name, file, object, objectTable, userTable)
{
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_POP_UP_ITEM::Event(const UI_EVENT &event)
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
				ccode = UIW_POP_UP_ITEM::Event(event);
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
		ccode = UIW_POP_UP_ITEM::Event(event);
	return (ccode);
}

void *ZAF_POP_UP_ITEM::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_POP_UP_ITEM::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_POP_UP_ITEM::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_POP_UP_ITEM(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_POP_UP_ITEM *item;
	if (file)
		item = new ZAF_POP_UP_ITEM(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "item");
		item = new ZAF_POP_UP_ITEM(_text1);
	}
	return (item);
}
