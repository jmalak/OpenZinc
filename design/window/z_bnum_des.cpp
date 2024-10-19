//	Zinc Interface Library - Z_BNUM.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_BIGNUM
#include "p_window.hpp"

// ----- UIW_BIGNUM ---------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_BIGNUM_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_BIGNUM *object = (UIW_BIGNUM *)data;
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
	SetValue(FIELD_VALUE, object->number);
	SetText(FIELD_RANGE, object->range);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	int precision = NMF_DIGITS(object->nmFlags);
	SetValue(FIELD_PRECISION, precision);
	SetFlag(FLAG_AUTO_CLEAR, object->woFlags, WOF_AUTO_CLEAR);
	SetFlag(FLAG_CURRENCY, object->nmFlags, NMF_CURRENCY);
	SetFlag(FLAG_CREDIT, object->nmFlags, NMF_CREDIT);
	SetFlag(FLAG_COMMA, object->nmFlags, NMF_COMMAS);
	SetFlag(FLAG_PERCENT, object->nmFlags, NMF_PERCENT);
}

int ZAF_OBJECT_EDITOR::UIW_BIGNUM_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_BIGNUM *object = (UIW_BIGNUM *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetValue(FIELD_VALUE, object->number);
	changed |= GetText(FIELD_RANGE, &object->range);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	int precision;
	if (GetValue(FIELD_PRECISION, &precision))
	{
		changed |= TRUE;
		object->nmFlags &= ~NMF_DECIMAL_FLAGS;
		object->nmFlags |= NMF_DECIMAL(precision);
	}
	changed |= GetFlag(FLAG_AUTO_CLEAR, &object->woFlags, WOF_AUTO_CLEAR);
	changed |= GetFlag(FLAG_CURRENCY, &object->nmFlags, NMF_CURRENCY);
	changed |= GetFlag(FLAG_CREDIT, &object->nmFlags, NMF_CREDIT);
	changed |= GetFlag(FLAG_COMMA, &object->nmFlags, NMF_COMMAS);
	changed |= GetFlag(FLAG_PERCENT, &object->nmFlags, NMF_PERCENT);

	// Return the status.
	return (changed);
}

// ----- ZAF_BIGNUM ---------------------------------------------------------

ZAF_BIGNUM::ZAF_BIGNUM(int left, int top, int width, ZIL_BIGNUM *_value,
	const ZIL_ICHAR *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_BIGNUM(left, top, width, _value, _range, _nmFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_BIGNUM::ZAF_BIGNUM(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BIGNUM(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_BIGNUM::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_BIGNUM::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_BIGNUM::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_BIGNUM(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_BIGNUM *bignum;
	if (file)
		bignum = new ZAF_BIGNUM(name, file, object, objectTable, userTable);
	else
		bignum = new ZAF_BIGNUM(0, 0, 15, ZIL_NULLP(ZIL_BIGNUM));
	return (bignum);
}

