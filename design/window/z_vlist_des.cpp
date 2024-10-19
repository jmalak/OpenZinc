//	Zinc Interface Library - Z_VLIST.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_VT_LIST
#include "p_window.hpp"

// ----- UIW_VT_LIST --------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_VT_LIST_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_VT_LIST *object = (UIW_VT_LIST *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	SetText(FIELD_COMPARE_FUNCTION, object->compareFunctionName);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	SetScroll(FLAG_VERTICAL_SCROLL_BAR, object, SBF_VERTICAL);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SORT, object->wnFlags, WNF_AUTO_SORT);
	SetFlag(FLAG_BITMAP_CHILDREN, object->wnFlags, WNF_BITMAP_CHILDREN);
	SetFlag(FLAG_NO_WRAP, object->wnFlags, WNF_NO_WRAP);
	SetFlag(FLAG_AUTO_SELECT, object->wnFlags, WNF_AUTO_SELECT);
	SetFlag(FLAG_CONTINUE_SELECT, object->wnFlags, WNF_CONTINUE_SELECT);
	SetFlag(FLAG_SELECT_MULTIPLE, object->wnFlags, WNF_SELECT_MULTIPLE);
	// All other WNF_FLAGS are automatic.
}

int ZAF_OBJECT_EDITOR::UIW_VT_LIST_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_VT_LIST *object = (UIW_VT_LIST *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_COMPARE_FUNCTION, &object->compareFunctionName);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	changed |= GetScroll(FLAG_VERTICAL_SCROLL_BAR, object, SBF_VERTICAL);
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
	changed |= GetFlag(FLAG_BITMAP_CHILDREN, &object->wnFlags, WNF_BITMAP_CHILDREN);
	changed |= GetFlag(FLAG_NO_WRAP, &object->wnFlags, WNF_NO_WRAP);
	changed |= GetFlag(FLAG_AUTO_SELECT, &object->wnFlags, WNF_AUTO_SELECT);
	changed |= GetFlag(FLAG_CONTINUE_SELECT, &object->wnFlags, WNF_CONTINUE_SELECT);
	changed |= GetFlag(FLAG_SELECT_MULTIPLE, &object->wnFlags, WNF_SELECT_MULTIPLE);

	// Return the status.
	return (changed);
}

// ----- ZAF_VT_LIST --------------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, // control
	ID_PROMPT, ID_ICON, // static
	ID_END // end
};

ZAF_VT_LIST::ZAF_VT_LIST(int left, int top, int width, int height,
	ZIL_COMPARE_FUNCTION _compareFunction, WNF_FLAGS _wnFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_VT_LIST(left, top, width, height, _compareFunction, _wnFlags, _woFlags, _woAdvancedFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_VT_LIST::ZAF_VT_LIST(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_VT_LIST(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_VT_LIST::Event(const UI_EVENT &event)
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
				event.windowObject->relative.left = 0;
				ccode = UIW_VT_LIST::Event(event);
				UIW_VT_LIST::Event(UI_EVENT(S_REDISPLAY));
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
		ccode = UIW_VT_LIST::Event(event);
	return (ccode);
}

void *ZAF_VT_LIST::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_VT_LIST::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_VT_LIST::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_VT_LIST(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_VT_LIST *vtList;
	if (file)
		vtList = new ZAF_VT_LIST(name, file, object, objectTable, userTable);
	else
	{
		vtList = new ZAF_VT_LIST(0, 0, 20, 5);
		*vtList + new ZAF_SCROLL_BAR(0, 0, 1, 5, SBF_VERTICAL);
	}
	return (vtList);
}

