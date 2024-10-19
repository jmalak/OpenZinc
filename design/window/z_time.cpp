//	Zinc Interface Library - Z_TIME.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_TIME
#include "p_window.hpp"

// ----- UIW_TIME -----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_TIME_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TIME *object = (UIW_TIME *)data;
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
	SetValue(FIELD_VALUE, object->time);
	SetText(FIELD_RANGE, object->range);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	// format
	SetFlag(FLAG_HUNDREDTHS, object->tmFlags, TMF_HUNDREDTHS);
	SetFlag(FLAG_SECONDS, object->tmFlags, TMF_SECONDS);
	SetFlag(FLAG_NO_MINUTES, object->tmFlags, TMF_NO_MINUTES);
	SetFlag(FLAG_NO_HOURS, object->tmFlags, TMF_NO_HOURS);
	SetFlag(FLAG_ZERO_FILL, object->tmFlags, TMF_ZERO_FILL);
	SetFlag(FLAG_SYSTEM, object->tmFlags, TMF_SYSTEM);
	// conversion
	SetFlag(FLAG_NORMAL_CASE,
		FlagSet(object->tmFlags, TMF_LOWER_CASE | TMF_UPPER_CASE), 0);
	SetFlag(FLAG_LOWER_CASE, object->tmFlags, TMF_LOWER_CASE);
	SetFlag(FLAG_UPPER_CASE, object->tmFlags, TMF_UPPER_CASE);
	// separators
	SetFlag(FLAG_DEFAULT_SEPARATOR,
		FlagSet(object->tmFlags, TMF_COLON_SEPARATOR | TMF_NO_SEPARATOR), 0);
	SetFlag(FLAG_NO_SEPARATOR, object->tmFlags, TMF_NO_SEPARATOR);
	SetFlag(FLAG_COLON_SEPARATOR, object->tmFlags, TMF_COLON_SEPARATOR);
	// country
	SetFlag(FLAG_DEFAULT_FORMAT,
		FlagSet(object->tmFlags, TMF_TWELVE_HOUR | TMF_TWENTY_FOUR_HOUR), 0);
	SetFlag(FLAG_12_HOUR, object->tmFlags, TMF_TWELVE_HOUR);
	SetFlag(FLAG_24_HOUR, object->tmFlags, TMF_TWENTY_FOUR_HOUR);
	// general
	SetFlag(FLAG_AUTO_CLEAR, object->woFlags, WOF_AUTO_CLEAR);
}

int ZAF_OBJECT_EDITOR::UIW_TIME_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TIME *object = (UIW_TIME *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetValue(FIELD_VALUE, object->time);
	changed |= GetText(FIELD_RANGE, &object->range);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	changed |= GetFlag(FLAG_HUNDREDTHS, &object->tmFlags, TMF_HUNDREDTHS);
	changed |= GetFlag(FLAG_SECONDS, &object->tmFlags, TMF_SECONDS);
	changed |= GetFlag(FLAG_NO_MINUTES, &object->tmFlags, TMF_NO_MINUTES);
	changed |= GetFlag(FLAG_NO_HOURS, &object->tmFlags, TMF_NO_HOURS);
	changed |= GetFlag(FLAG_ZERO_FILL, &object->tmFlags, TMF_ZERO_FILL);
	changed |= GetFlag(FLAG_SYSTEM, &object->tmFlags, TMF_SYSTEM);
	// conversion
	changed |= GetFlag(FLAG_LOWER_CASE, &object->tmFlags, TMF_LOWER_CASE);
	changed |= GetFlag(FLAG_UPPER_CASE, &object->tmFlags, TMF_UPPER_CASE);
	// separators
	changed |= GetFlag(FLAG_NO_SEPARATOR, &object->tmFlags, TMF_NO_SEPARATOR);
	changed |= GetFlag(FLAG_COLON_SEPARATOR, &object->tmFlags, TMF_COLON_SEPARATOR);
	// country
	changed |= GetFlag(FLAG_12_HOUR, &object->tmFlags, TMF_TWELVE_HOUR);
	changed |= GetFlag(FLAG_24_HOUR, &object->tmFlags, TMF_TWENTY_FOUR_HOUR);
	// general
	changed |= GetFlag(FLAG_AUTO_CLEAR, &object->woFlags, WOF_AUTO_CLEAR);

	// Return the status.
	return (changed);
}

// ----- ZAF_TIME -----------------------------------------------------------

ZAF_TIME::ZAF_TIME(int left, int top, int width, ZIL_TIME *_time,
	const ZIL_ICHAR *_range, TMF_FLAGS _tmFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_TIME(left, top, width, _time, _range, _tmFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_TIME::ZAF_TIME(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TIME(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_TIME::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_TIME::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_TIME::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_TIME(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_TIME *time;
	if (file)
		time = new ZAF_TIME(name, file, object, objectTable, userTable);
	else
		time = new ZAF_TIME(0, 0, 20, ZIL_NULLP(ZIL_TIME));
	return (time);
}
