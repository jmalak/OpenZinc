//	Zinc Interface Library - Z_PROMPT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_PROMPT
#include "p_window.hpp"

// ----- UIW_PROMPT ---------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_PROMPT_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_PROMPT *object = (UIW_PROMPT *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT |
		WOF_PIXEL | WOF_MINICELL;
	if (object->parent->Inherited(ID_LIST))
		woAdvancedFlagsAllowed |= WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_TEXT, object->text);
	SetText(FIELD_STRINGID, object->stringID);
}

int ZAF_OBJECT_EDITOR::UIW_PROMPT_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_PROMPT *object = (UIW_PROMPT *)data;

	// Store the object data.
	int changed = FALSE;
	if (GetText(FIELD_TEXT, &object->text))
		object->DataSet(object->text); // Needed to update hotkey.
	changed |= GetText(FIELD_STRINGID, object->stringID);

	// Return the status.
	return (changed);
}

// ----- ZAF_PROMPT ---------------------------------------------------------

ZAF_PROMPT::ZAF_PROMPT(int left, int top, ZIL_ICHAR *_text, WOF_FLAGS _woFlags) :
	UIW_PROMPT(left, top, _text, _woFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_PROMPT::ZAF_PROMPT(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_PROMPT(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_PROMPT::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_PROMPT::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_PROMPT::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_PROMPT(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_PROMPT *prompt;
	if (file)
		prompt = new ZAF_PROMPT(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "prompt:");
		prompt = new ZAF_PROMPT(0, 0, _text1);
	}
	return (prompt);
}
