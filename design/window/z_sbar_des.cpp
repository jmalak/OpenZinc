//	Zinc Interface Library - Z_STATUS.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_STATUS_BAR
#include "p_window.hpp"

// ----- UIW_STATUS_BAR -----------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_STATUS_BAR_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_STATUS_BAR *object = (UIW_STATUS_BAR *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_GEOMETRY;
	woFlagsAllowed =
		WOF_PIXEL | WOF_MINICELL |
		WOF_SUPPORT_OBJECT;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetValue(HEIGHT_FIELD, object->height);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
}

int ZAF_OBJECT_EDITOR::UIW_STATUS_BAR_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_STATUS_BAR *object = (UIW_STATUS_BAR *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetValue(HEIGHT_FIELD, &object->height);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);

	// Return the status.
	return (changed);
}

// ----- ZAF_STATUS_BAR -----------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_TEXT, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, // control
	ID_PROMPT, ID_ICON, // static
	ID_GEOMETRY_MANAGER, ID_END // end
};

ZAF_STATUS_BAR::ZAF_STATUS_BAR(int _height, WOF_FLAGS _woFlags) :
	UIW_STATUS_BAR(_height, _woFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_STATUS_BAR::ZAF_STATUS_BAR(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STATUS_BAR(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_STATUS_BAR::Event(const UI_EVENT &event)
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
				ccode = UIW_STATUS_BAR::Event(event);
				Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_ADD_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_STATUS_BAR::Event(event);
	return (ccode);
}

void *ZAF_STATUS_BAR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_STATUS_BAR::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_STATUS_BAR::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_STATUS_BAR(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_STATUS_BAR *sbar;
	if (file)
		sbar = new ZAF_STATUS_BAR(name, file, object, objectTable, userTable);
	else
		sbar = new ZAF_STATUS_BAR;
	return (sbar);
}

