//	Zinc Interface Library - Z_TITLE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_TITLE
#include "p_window.hpp"

// ----- UIW_TITLE ----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_TITLE_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TITLE *object = (UIW_TITLE *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_TITLE, object->text);
	SetText(FIELD_STRINGID, object->stringID);
}

int ZAF_OBJECT_EDITOR::UIW_TITLE_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TITLE *object = (UIW_TITLE *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_TITLE, &object->text);
	changed |= GetText(FIELD_STRINGID, object->stringID);

	// Return the status.
	return (changed);
}

// ----- ZAF_TITLE ----------------------------------------------------------

ZAF_TITLE::ZAF_TITLE(ZIL_ICHAR *_text, WOF_FLAGS _woFlags) :
	UIW_TITLE(_text, _woFlags)
{
}

ZAF_TITLE::ZAF_TITLE(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TITLE(name, file, object, objectTable, userTable)
{
}

EVENT_TYPE ZAF_TITLE::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_TITLE::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_TITLE::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_TITLE(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_TITLE *title;
	if (file)
		title = new ZAF_TITLE(name, file, object, objectTable, userTable);
	else
		title = new ZAF_TITLE(_serviceManager->ZMSG_untitled());
	return (title);
}

