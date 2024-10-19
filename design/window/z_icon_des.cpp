//	Zinc Interface Library - Z_ICON.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_ICON
#include "p_window.hpp"

// ----- UIW_ICON -----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_ICON_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_ICON *object = (UIW_ICON *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT |
		WOF_PIXEL | WOF_MINICELL |
		WOF_NO_ALLOCATE_DATA | 
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	SetText(FIELD_TITLE, object->title);
	SetImage(FIELD_IMAGE, object->iconName, ID_ICON_IMAGE);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_UP_CLICK,
		FlagSet(object->icFlags, ICF_DOUBLE_CLICK), 0);
	SetFlag(FLAG_DOUBLE_CLICK, object->icFlags, ICF_DOUBLE_CLICK);
	// Object flags not used: ICF_MINIMIZE_OBJECT, ICF_STATIC_ICONARRAY
}

int ZAF_OBJECT_EDITOR::UIW_ICON_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_ICON *object = (UIW_ICON *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_TITLE, &object->title);
	changed |= GetImage(FIELD_IMAGE, &object->iconName, ID_ICON_IMAGE);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	changed |= GetFlag(FLAG_DOUBLE_CLICK, &object->icFlags, ICF_DOUBLE_CLICK);

	// Return the status.
	return (changed);
}

// ----- ZAF_ICON -----------------------------------------------------------

ZAF_ICON::ZAF_ICON(int left, int top, ZIL_ICHAR *_iconName,
	ZIL_ICHAR *_title, ICF_FLAGS _icFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_ICON(left, top, _iconName, _title, _icFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_ICON::ZAF_ICON(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_ICON(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_ICON::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_ICON::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_ICON::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_ICON(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_ICON *icon;
	if (file)
		icon = new ZAF_ICON(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "blankIcon");
		icon = new ZAF_ICON(0, 0, _text1);
	}
	return (icon);
}
