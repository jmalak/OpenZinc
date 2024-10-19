//	Zinc Interface Library - Z_GROUP.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_GROUP
#include "p_window.hpp"

// ----- UIW_GROUP ----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_GROUP_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_GROUP *object = (UIW_GROUP *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed = WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	SetText(TEXT_FIELD, object->text);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SELECT, object->wnFlags, WNF_AUTO_SELECT);
	SetFlag(FLAG_SELECT_MULTIPLE, object->wnFlags, WNF_SELECT_MULTIPLE);
	SetFlag(FLAG_NO_WRAP, object->wnFlags, WNF_NO_WRAP);
}

int ZAF_OBJECT_EDITOR::UIW_GROUP_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_GROUP *object = (UIW_GROUP *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(TEXT_FIELD, &object->text);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	changed |= GetFlag(FLAG_AUTO_SELECT, &object->wnFlags, WNF_AUTO_SELECT);
	changed |= GetFlag(FLAG_SELECT_MULTIPLE, &object->wnFlags, WNF_SELECT_MULTIPLE);
	changed |= GetFlag(FLAG_NO_WRAP, &object->wnFlags, WNF_NO_WRAP);

	// Return the status.
	return (changed);
}

// ----- ZAF_GROUP ----------------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_TEXT, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, ID_SCROLL_BAR, ID_HZ_SCROLL, ID_VT_SCROLL, ID_COMBO_BOX, ID_SPIN_CONTROL, // control
	ID_HZ_LIST, ID_VT_LIST, // selection
	ID_PROMPT, ID_GROUP, ID_ICON, ID_WINDOW, ID_DERIVED_OBJECT, // static
	ID_GEOMETRY_MANAGER, ID_END // end
};

ZAF_GROUP::ZAF_GROUP(int left, int top, int width, int height,
	ZIL_ICHAR *_text, WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags) :
	UIW_GROUP(left, top, width, height, _text, _wnFlags, _woFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_GROUP::ZAF_GROUP(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_GROUP(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_GROUP::Event(const UI_EVENT &event)
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
				if (event.windowObject->relative.top == 0)
				{
					event.windowObject->relative.top++;
					event.windowObject->relative.bottom++;
				}
				ccode = UIW_GROUP::Event(event);
				UIW_GROUP::Event(UI_EVENT(S_REDISPLAY));
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
		ccode = UIW_GROUP::Event(event);
	return (ccode);
}

void *ZAF_GROUP::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_GROUP::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_GROUP::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_GROUP(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_GROUP *group;
	if (file)
		group = new ZAF_GROUP(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "Group");
		group = new ZAF_GROUP(0, 0, 20, 5, _text1, WNF_NO_FLAGS);
	}
	return (group);
}

