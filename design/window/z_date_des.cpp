//	Zinc Interface Library - Z_DATE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_DATE
#include "p_window.hpp"

// ----- UIW_DATE -----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_DATE_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_DATE *object = (UIW_DATE *)data;
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
	SetValue(FIELD_VALUE, object->date);
	SetText(FIELD_RANGE, object->range);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	// format
	SetFlag(FLAG_SHORT_DAY, object->dtFlags, DTF_SHORT_DAY);
	SetFlag(FLAG_DAY_OF_WEEK, object->dtFlags, DTF_DAY_OF_WEEK);
	SetFlag(FLAG_SHORT_MONTH, object->dtFlags, DTF_SHORT_MONTH);
	SetFlag(FLAG_MONTH, object->dtFlags, DTF_ALPHA_MONTH);
	SetFlag(FLAG_SHORT_YEAR, object->dtFlags, DTF_SHORT_YEAR);
	SetFlag(FLAG_UPPER_CASE, object->dtFlags, DTF_UPPER_CASE);
	SetFlag(FLAG_ZERO_FILL, object->dtFlags, DTF_ZERO_FILL);
	SetFlag(FLAG_SYSTEM, object->dtFlags, DTF_SYSTEM);
	// separators
	SetFlag(FLAG_DEFAULT_SEPARATOR,
		FlagSet(object->dtFlags, DTF_DASH | DTF_SLASH), 0);
	SetFlag(FLAG_DASH_SEPARATOR, object->dtFlags, DTF_DASH);
	SetFlag(FLAG_SLASH_SEPARATOR, object->dtFlags, DTF_SLASH);
	// country
	SetFlag(FLAG_DEFAULT_FORMAT,
		FlagSet(object->dtFlags, DTF_ASIAN_FORMAT | DTF_EUROPEAN_FORMAT | DTF_MILITARY_FORMAT | DTF_US_FORMAT), 0);
	SetFlag(FLAG_ASIAN_FORMAT, object->dtFlags, DTF_ASIAN_FORMAT);
	SetFlag(FLAG_EUROPEAN_FORMAT, object->dtFlags, DTF_EUROPEAN_FORMAT);
	SetFlag(FLAG_MILITARY_FORMAT, object->dtFlags, DTF_MILITARY_FORMAT);
	SetFlag(FLAG_US_FORMAT, object->dtFlags, DTF_US_FORMAT);
	// general
	SetFlag(FLAG_AUTO_CLEAR, object->woFlags, WOF_AUTO_CLEAR);
}

int ZAF_OBJECT_EDITOR::UIW_DATE_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_DATE *object = (UIW_DATE *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetValue(FIELD_VALUE, object->date);
	changed |= GetText(FIELD_RANGE, &object->range);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	// format
	changed |= GetFlag(FLAG_SHORT_DAY, &object->dtFlags, DTF_SHORT_DAY);
	changed |= GetFlag(FLAG_DAY_OF_WEEK, &object->dtFlags, DTF_DAY_OF_WEEK);
	changed |= GetFlag(FLAG_SHORT_MONTH, &object->dtFlags, DTF_SHORT_MONTH);
	changed |= GetFlag(FLAG_MONTH, &object->dtFlags, DTF_ALPHA_MONTH);
	changed |= GetFlag(FLAG_SHORT_YEAR, &object->dtFlags, DTF_SHORT_YEAR);
	changed |= GetFlag(FLAG_UPPER_CASE, &object->dtFlags, DTF_UPPER_CASE);
	changed |= GetFlag(FLAG_ZERO_FILL, &object->dtFlags, DTF_ZERO_FILL);
	changed |= GetFlag(FLAG_SYSTEM, &object->dtFlags, DTF_SYSTEM);
	// separators
	changed |= GetFlag(FLAG_DASH_SEPARATOR, &object->dtFlags, DTF_DASH);
	changed |= GetFlag(FLAG_SLASH_SEPARATOR, &object->dtFlags, DTF_SLASH);
	// country
	changed |= GetFlag(FLAG_ASIAN_FORMAT, &object->dtFlags, DTF_ASIAN_FORMAT);
	changed |= GetFlag(FLAG_EUROPEAN_FORMAT, &object->dtFlags, DTF_EUROPEAN_FORMAT);
	changed |= GetFlag(FLAG_MILITARY_FORMAT, &object->dtFlags, DTF_MILITARY_FORMAT);
	changed |= GetFlag(FLAG_US_FORMAT, &object->dtFlags, DTF_US_FORMAT);
	// general
	changed |= GetFlag(FLAG_AUTO_CLEAR, &object->woFlags, WOF_AUTO_CLEAR);

	// Return the status.
	return (changed);
}

// ----- ZAF_DATE -----------------------------------------------------------

ZAF_DATE::ZAF_DATE(int left, int top, int width, ZIL_DATE *_date, 
	const ZIL_ICHAR *_range, DTF_FLAGS _dtFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_DATE(left, top, width, _date, _range, _dtFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_DATE::ZAF_DATE(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_DATE(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_DATE::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_DATE::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_DATE::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_DATE(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_DATE *date;
	if (file)
		date = new ZAF_DATE(name, file, object, objectTable, userTable);
	else
		date = new ZAF_DATE(0, 0, 20, ZIL_NULLP(ZIL_DATE));
	return (date);
}
