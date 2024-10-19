//	Zinc Interface Library - Z_STRING.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_STRING
#include "p_window.hpp"

// ----- UIW_STRING ---------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_STRING_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_STRING *object = (UIW_STRING *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_NO_ALLOCATE_DATA | WOF_AUTO_CLEAR | 
		WOF_VIEW_ONLY | WOF_UNANSWERED | WOF_INVALID | WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	SetText(FIELD_TEXT, object->DataGet());
	SetValue(FIELD_MAXLENGTH, object->maxLength);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_VARIABLE_NAME, object->stFlags, STF_VARIABLE_NAME);
	SetFlag(FLAG_LOWER_CASE, object->stFlags, STF_LOWER_CASE);
	SetFlag(FLAG_STF_UPPER_CASE, object->stFlags, STF_UPPER_CASE);
	SetFlag(FLAG_PASSWORD, object->stFlags, STF_PASSWORD);
	SetFlag(FLAG_NO_FLAGS, FlagSet(object->stFlags, STF_LOWER_CASE | STF_UPPER_CASE | STF_PASSWORD), 0);
	// general
	SetFlag(FLAG_AUTO_CLEAR, object->woFlags, WOF_AUTO_CLEAR);
}

int ZAF_OBJECT_EDITOR::UIW_STRING_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_STRING *object = (UIW_STRING *)data;

	// Store the object data.
	int changed = FALSE;
	int maxLength = -1;
	changed |= GetValue(FIELD_MAXLENGTH, &maxLength);
	ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
	changed |= GetText(FIELD_TEXT, &text);
	if (changed)
	{
		object->DataSet(text, maxLength);
		if (text)
			delete text;
		changed = FALSE; // DataSet automatically updates.
	}
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	changed |= GetFlag(FLAG_VARIABLE_NAME, &object->stFlags, STF_VARIABLE_NAME);
	changed |= GetFlag(FLAG_LOWER_CASE, &object->stFlags, STF_LOWER_CASE);
	changed |= GetFlag(FLAG_STF_UPPER_CASE, &object->stFlags, STF_UPPER_CASE);
	changed |= GetFlag(FLAG_PASSWORD, &object->stFlags, STF_PASSWORD);
	// general
	changed |= GetFlag(FLAG_AUTO_CLEAR, &object->woFlags, WOF_AUTO_CLEAR);

	// Return the status.
	return (changed);
}

// ----- UIW_STRING ---------------------------------------------------------

ZAF_STRING::ZAF_STRING(int left, int top, int width, ZIL_ICHAR *_text,
	int _maxLength, STF_FLAGS _stFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, _text, _maxLength, _stFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_STRING::ZAF_STRING(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STRING(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_STRING::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_STRING::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_STRING::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_STRING(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_STRING *string;
	if (file)
		string = new ZAF_STRING(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "string");
		string = new ZAF_STRING(0, 0, 20, _text1, 32);
	}
	return (string);
}
