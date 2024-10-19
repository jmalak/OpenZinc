//	Zinc Interface Library - Z_TBAR.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_TOOL_BAR
#include "p_window.hpp"


// ----- UIW_TOOL_BAR -------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_TOOL_BAR_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TOOL_BAR *object = (UIW_TOOL_BAR *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_SUPPORT_OBJECT |
		WOF_PIXEL | WOF_MINICELL |
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed = WOAF_NON_CURRENT;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_SELECT_MULTIPLE, object->wnFlags, WNF_SELECT_MULTIPLE);
	SetFlag(FLAG_NO_WRAP, object->wnFlags, WNF_NO_WRAP);
}

int ZAF_OBJECT_EDITOR::UIW_TOOL_BAR_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TOOL_BAR *object = (UIW_TOOL_BAR *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	if (GetFlag(FLAG_NO_WRAP, &object->wnFlags, WNF_NO_WRAP))
	{
		changed |= TRUE;
		for (UI_WINDOW_OBJECT *field = object->First(); field; field = field->Next())
			if (FlagSet(object->wnFlags, WNF_NO_WRAP))
			{
				field->relative.top += display->preSpace;
				field->trueRegion.top += display->preSpace;
				field->relative.bottom -= display->postSpace + 1;
				field->trueRegion.bottom -= display->postSpace + 1;
			}
			else
			{
				field->relative.top -= display->preSpace;
				field->trueRegion.top -= display->preSpace;
				field->relative.bottom += display->postSpace - 1;
				field->trueRegion.bottom += display->postSpace - 1;
			}
	}
	changed |= GetFlag(FLAG_SELECT_MULTIPLE, &object->wnFlags, WNF_SELECT_MULTIPLE);

	// Return the status.
	return (changed);
}

// ----- UIW_TOOL_BAR -------------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, ID_COMBO_BOX, ID_SPIN_CONTROL, // control
	ID_PROMPT, ID_ICON, // static
	ID_GEOMETRY_MANAGER, ID_END // end
};

ZAF_TOOL_BAR::ZAF_TOOL_BAR(int left, int top, int width, int height,
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_TOOL_BAR(left, top, width, height, _wnFlags, _woFlags, _woAdvancedFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_TOOL_BAR::ZAF_TOOL_BAR(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TOOL_BAR(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_TOOL_BAR::Event(const UI_EVENT &event)
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
				ccode = UIW_TOOL_BAR::Event(event);
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
		ccode = UIW_TOOL_BAR::Event(event);
	return (ccode);
}

void *ZAF_TOOL_BAR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_TOOL_BAR::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_TOOL_BAR::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_TOOL_BAR(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_TOOL_BAR *tbar;
	if (file)
		tbar = new ZAF_TOOL_BAR(name, file, object, objectTable, userTable);
	else
	{
		tbar = new ZAF_TOOL_BAR(0, 0, 20, 1, WNF_NO_FLAGS,
			WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT);
	}
	return (tbar);
}
