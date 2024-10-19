//	Zinc Interface Library - Z_REAL.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_REAL
#include "p_window.hpp"

// ----- UIW_REAL -----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_REAL_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_REAL *object = (UIW_REAL *)data;
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
	SetValue(FIELD_VALUE, *object->number);
	SetText(FIELD_RANGE, object->range);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	int precision = NMF_DIGITS(object->nmFlags);
	SetValue(FIELD_PRECISION, precision);
	SetFlag(FLAG_SCIENTIFIC, object->nmFlags, NMF_SCIENTIFIC);
	SetFlag(FLAG_AUTO_CLEAR, object->woFlags, WOF_AUTO_CLEAR);
}

int ZAF_OBJECT_EDITOR::UIW_REAL_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_REAL *object = (UIW_REAL *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetValue(FIELD_VALUE, object->number);
	changed |= GetText(FIELD_RANGE, &object->range);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object data.
	int digits;
	if (GetValue(FIELD_PRECISION, &digits))
	{
		changed |= TRUE;
		object->nmFlags &= ~NMF_DECIMAL_FLAGS;
		object->nmFlags |= NMF_DECIMAL(digits);
	}
	changed |= GetFlag(FLAG_SCIENTIFIC, &object->nmFlags, NMF_SCIENTIFIC);
	changed |= GetFlag(FLAG_AUTO_CLEAR, &object->woFlags, WOF_AUTO_CLEAR);

	// Return the status.
	return (changed);
}

// ----- ZAF_REAL -----------------------------------------------------------

ZAF_REAL::ZAF_REAL(int left, int top, int width, double *_value,
	const ZIL_ICHAR *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_REAL(left, top, width, _value, _range, _nmFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_REAL::ZAF_REAL(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_REAL(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_REAL::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_REAL::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_REAL::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_REAL(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_REAL *real;
	if (file)
		real = new ZAF_REAL(name, file, object, objectTable, userTable);
	else
	{
		real = new ZAF_REAL(0, 0, 15, ZIL_NULLP(double));
		real->DataSet(ZIL_NULLP(double));
	}
	return (real);
}
