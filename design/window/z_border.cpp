//	Zinc Interface Library - Z_BORDER.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_BORDER
#include "p_window.hpp"

// ----- UIW_BORDER ---------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_BORDER_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_BORDER *object = (UIW_BORDER *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
}

int ZAF_OBJECT_EDITOR::UIW_BORDER_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_BORDER *object = (UIW_BORDER *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);

	// Return the status.
	return (changed);
}

// ----- ZAF_BORDER ---------------------------------------------------------

ZAF_BORDER::ZAF_BORDER(BDF_FLAGS _bdFlags) :
	UIW_BORDER(_bdFlags)
{
}

ZAF_BORDER::ZAF_BORDER(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BORDER(name, file, object, objectTable, userTable)
{
}

EVENT_TYPE ZAF_BORDER::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_BORDER::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_BORDER::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_BORDER(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_BORDER *border;
	if (file)
		border = new ZAF_BORDER(name, file, object, objectTable, userTable);
	else
		border = new ZAF_BORDER;
	return (border);
}
