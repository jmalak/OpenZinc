//	Zinc Interface Library - Z_MAX.CPP

//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_MAXIMIZE_BUTTON
#include "p_window.hpp"

// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_MAXIMIZE_BUTTON_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_MAXIMIZE_BUTTON *object = (UIW_MAXIMIZE_BUTTON *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
}

int ZAF_OBJECT_EDITOR::UIW_MAXIMIZE_BUTTON_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_MAXIMIZE_BUTTON *object = (UIW_MAXIMIZE_BUTTON *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);

	// Return the status.
	return (changed);
}

// ----- ZAF_MAXIMIZE_BUTTON ------------------------------------------------

ZAF_MAXIMIZE_BUTTON::ZAF_MAXIMIZE_BUTTON(void) :
	UIW_MAXIMIZE_BUTTON()
{
}

ZAF_MAXIMIZE_BUTTON::ZAF_MAXIMIZE_BUTTON(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_MAXIMIZE_BUTTON(name, file, object, objectTable, userTable)
{
}

EVENT_TYPE ZAF_MAXIMIZE_BUTTON::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_MAXIMIZE_BUTTON::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_MAXIMIZE_BUTTON::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_MAXIMIZE_BUTTON(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_MAXIMIZE_BUTTON *maxButton;
	if (file)
		maxButton = new ZAF_MAXIMIZE_BUTTON(name, file, object, objectTable, userTable);
	else
		maxButton = new ZAF_MAXIMIZE_BUTTON;
	return (maxButton);
}
