//	Zinc Interface Library - Z_MIN.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_MINIMIZE_BUTTON
#include "p_window.hpp"

// ----- UIW_MINIMIZE_BUTTON ------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_MINIMIZE_BUTTON_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_MINIMIZE_BUTTON *object = (UIW_MINIMIZE_BUTTON *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
}

int ZAF_OBJECT_EDITOR::UIW_MINIMIZE_BUTTON_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_MINIMIZE_BUTTON *object = (UIW_MINIMIZE_BUTTON *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);

	// Return the status.
	return (changed);
}

// ----- ZAF_MINIMIZE_BUTTON ------------------------------------------------

ZAF_MINIMIZE_BUTTON::ZAF_MINIMIZE_BUTTON(void) :
	UIW_MINIMIZE_BUTTON()
{
}

ZAF_MINIMIZE_BUTTON::ZAF_MINIMIZE_BUTTON(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_MINIMIZE_BUTTON(name, file, object, objectTable, userTable)
{
}

EVENT_TYPE ZAF_MINIMIZE_BUTTON::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_MINIMIZE_BUTTON::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_MINIMIZE_BUTTON::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_MINIMIZE_BUTTON(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_MINIMIZE_BUTTON *minButton;
	if (file)
		minButton = new ZAF_MINIMIZE_BUTTON(name, file, object, objectTable, userTable);
	else
		minButton = new ZAF_MINIMIZE_BUTTON;
	return (minButton);
}
