//	Zinc Interface Library - Z_COMBO.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_COMBO_BOX
#include "p_window.hpp"


// ----- UIW_COMBO_BOX ------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_COMBO_BOX_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_COMBO_BOX *object = (UIW_COMBO_BOX *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_AUTO_CLEAR |
		WOF_VIEW_ONLY | WOF_UNANSWERED | WOF_INVALID | WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	SetText(FIELD_COMPARE_FUNCTION, object->list.compareFunctionName);
	int height = (object->list.relative.Height() + display->preSpace + display->postSpace) / display->cellHeight;
	SetValue(FIELD_LIST_HEIGHT, height);
	SetText(FIELD_LIST_NAME, object->list.StringID());
	int value = object->list.NumberID();
	SetValue(FIELD_LIST_ID, value);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	SetScroll(FLAG_VERTICAL_SCROLL_BAR, &object->list, SBF_VERTICAL);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SORT, object->wnFlags, WNF_AUTO_SORT);
	SetFlag(FLAG_BITMAP_CHILDREN, object->wnFlags, WNF_BITMAP_CHILDREN);
	SetFlag(FLAG_NO_WRAP, object->wnFlags, WNF_NO_WRAP);
	// All other WNF_FLAGS are automatic.
}

int ZAF_OBJECT_EDITOR::UIW_COMBO_BOX_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_COMBO_BOX *object = (UIW_COMBO_BOX *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_COMPARE_FUNCTION, &object->list.compareFunctionName);
	int height;
	if (GetValue(FIELD_LIST_HEIGHT, &height))
	{
		changed |= TRUE;
		object->list.trueRegion.top = 0;
		object->list.trueRegion.bottom = height * display->cellHeight - 1;
		object->list.relative = object->list.trueRegion;
	}
	changed |= GetText(FIELD_LIST_NAME, object->list.StringID());
	int value;
	if (GetValue(FIELD_LIST_ID, &value))
	{
		changed |= TRUE;
		object->list.numberID = (ZIL_NUMBERID)value;
	}
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	changed |= GetScroll(FLAG_VERTICAL_SCROLL_BAR, &object->list, SBF_VERTICAL);
	// Store the object flags.
	if (GetFlag(FLAG_AUTO_SORT, &object->wnFlags, WNF_AUTO_SORT))
	{
		changed |= TRUE;
		object->list.wnFlags |= WNF_AUTO_SORT;
		if (FlagSet(object->wnFlags, WNF_AUTO_SORT))
		{
			object->compareFunction = UIW_WINDOW::StringCompare;
			object->Sort();
		}
		else
			object->compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION);
	}
	changed |= GetFlag(FLAG_BITMAP_CHILDREN, &object->wnFlags, WNF_BITMAP_CHILDREN);
	if (FlagSet(object->wnFlags, WNF_BITMAP_CHILDREN))
		object->list.wnFlags |= WNF_BITMAP_CHILDREN;
	else
		object->list.wnFlags &= ~WNF_BITMAP_CHILDREN;
	changed |= GetFlag(FLAG_NO_WRAP, &object->wnFlags, WNF_NO_WRAP);

	// Return the status.
	return (changed);
}

// ----- ZAF_COMBO_BOX ------------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, // control
	ID_ICON, // static
	ID_END // end
};

ZAF_COMBO_BOX::ZAF_COMBO_BOX(int left, int top, int width, int height,
	ZIL_COMPARE_FUNCTION _compareFunction, WNF_FLAGS _wnFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_COMBO_BOX(left, top, width, height, _compareFunction, _wnFlags, _woFlags, _woAdvancedFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_COMBO_BOX::ZAF_COMBO_BOX(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_COMBO_BOX(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_COMBO_BOX::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		list.support.Index(event.windowObject) == -1 &&
		list.UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idTable[i] != ID_END; i++)
			if (_idTable[i] == objectID)
			{
				event.windowObject->relative.left = 0;
				ccode = UIW_COMBO_BOX::Event(event);
				UIW_COMBO_BOX::Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		list.support.Index(event.windowObject) == -1 &&
		list.UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_COMBO_BOX::Event(event);
	return (ccode);
}

void *ZAF_COMBO_BOX::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_COMBO_BOX::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_COMBO_BOX::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_COMBO_BOX(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_COMBO_BOX *combobox;
	if (file)
		combobox = new ZAF_COMBO_BOX(name, file, object, objectTable, userTable);
	else
		combobox = new ZAF_COMBO_BOX(0, 0, 20, 4);
	return (combobox);
}
