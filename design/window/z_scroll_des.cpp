//	Zinc Interface Library - Z_SCROLL.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_SCROLL_BAR
#include "p_window.hpp"

// ----- UIW_SCROLL_BAR -----------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_SCROLL_BAR_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_SCROLL_BAR *object = (UIW_SCROLL_BAR *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_PIXEL | WOF_MINICELL |
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed = WOAF_NON_CURRENT;

	// Load the object data.
	int value = object->scroll.minimum;
	SetValue(FIELD_MINIMUM, value);
	value = object->scroll.maximum;
	SetValue(FIELD_MAXIMUM, value);
	value = object->scroll.current;
	SetValue(FIELD_CURRENT, value);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	if (FlagSet(object->sbFlags, SBF_VERTICAL | SBF_CORNER))
	{
		notebook->Get(FLAG_HZ_SLIDER)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_HORIZONTAL_SCROLL_BAR)->woFlags |= WOF_NON_SELECTABLE;
	}
	if (FlagSet(object->sbFlags, SBF_HORIZONTAL | SBF_CORNER))
	{
		notebook->Get(FLAG_VT_SLIDER)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_VERTICAL_SCROLL_BAR)->woFlags |= WOF_NON_SELECTABLE;
	}
	notebook->Get(FLAG_CORNER_SCROLL_BAR)->woFlags |= WOF_NON_SELECTABLE;

	if (object->sbFlags == (SBF_SLIDER | SBF_VERTICAL))
		SetFlag(FLAG_VT_SLIDER, object->sbFlags, SBF_SLIDER | SBF_VERTICAL);
	if (object->sbFlags == (SBF_SLIDER | SBF_HORIZONTAL))
		SetFlag(FLAG_HZ_SLIDER, object->sbFlags, SBF_SLIDER | SBF_HORIZONTAL);
	if (object->sbFlags == SBF_VERTICAL)
		SetFlag(FLAG_VERTICAL_SCROLL_BAR, object->sbFlags, SBF_VERTICAL);
	if (object->sbFlags == SBF_HORIZONTAL)
		SetFlag(FLAG_HORIZONTAL_SCROLL_BAR, object->sbFlags, SBF_HORIZONTAL);
	if (object->sbFlags == SBF_CORNER)
		SetFlag(FLAG_CORNER_SCROLL_BAR, object->sbFlags, SBF_CORNER);
}

int ZAF_OBJECT_EDITOR::UIW_SCROLL_BAR_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_SCROLL_BAR *object = (UIW_SCROLL_BAR *)data;

	// Store the object data.
	int changed = FALSE;
	int value;
	if (GetValue(FIELD_MINIMUM, &value))
	{
		changed |= TRUE;
		object->scroll.minimum = value;
	}
	if (GetValue(FIELD_MAXIMUM, &value))
	{
		changed |= TRUE;
		object->scroll.maximum = value;
	}
	if (GetValue(FIELD_CURRENT, &value))
	{
		changed |= TRUE;
		object->scroll.current = value;
	}
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	if (FlagSet(notebook->Get(FLAG_VT_SLIDER)->woStatus, WOS_SELECTED) &&
		object->sbFlags != (SBF_SLIDER | SBF_VERTICAL))
	{
		changed |= TRUE;
		object->sbFlags = SBF_SLIDER | SBF_VERTICAL;
	}
	else if (FlagSet(notebook->Get(FLAG_HZ_SLIDER)->woStatus, WOS_SELECTED) &&
		object->sbFlags != (SBF_SLIDER | SBF_HORIZONTAL))
	{
		changed |= TRUE;
		object->sbFlags = SBF_SLIDER | SBF_HORIZONTAL;
	}
	else if (FlagSet(notebook->Get(FLAG_VERTICAL_SCROLL_BAR)->woStatus, WOS_SELECTED) &&
		object->sbFlags != SBF_VERTICAL)
	{
		changed |= TRUE;
		object->sbFlags = SBF_VERTICAL;
	}
	else if (FlagSet(notebook->Get(FLAG_HORIZONTAL_SCROLL_BAR)->woStatus, WOS_SELECTED) &&
		object->sbFlags != SBF_HORIZONTAL)
	{
		changed |= TRUE;
		object->sbFlags = SBF_HORIZONTAL;
	}
	else if (FlagSet(notebook->Get(FLAG_CORNER_SCROLL_BAR)->woStatus, WOS_SELECTED) &&
		object->sbFlags != SBF_CORNER)
	{
		changed |= TRUE;
		object->sbFlags = SBF_CORNER;
	}

	// Return the status.
	return (changed);
}

// ----- ZAF_SCROLL_BAR -----------------------------------------------------

ZAF_SCROLL_BAR::ZAF_SCROLL_BAR(int left, int top, int width, int height,
	SBF_FLAGS _sbFlags, WOF_FLAGS _woFlags) :
	UIW_SCROLL_BAR(left, top, width, height, _sbFlags, _woFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_SCROLL_BAR::ZAF_SCROLL_BAR(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_SCROLL_BAR(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_SCROLL_BAR::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_SCROLL_BAR::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_SCROLL_BAR::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_SCROLL_BAR(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_SCROLL_BAR *scrollbar;
	if (file)
		scrollbar = new ZAF_SCROLL_BAR(name, file, object, objectTable, userTable);
	else
		scrollbar = new ZAF_SCROLL_BAR(0, 0, 15, 0, SBF_VERTICAL, WOF_BORDER);
	return (scrollbar);
}

UI_WINDOW_OBJECT *ZAF_SCROLL_BAR::HzSlideNew(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_SCROLL_BAR(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_SCROLL_BAR *scrollbar;
	if (file)
		scrollbar = new ZAF_SCROLL_BAR(name, file, object, objectTable, userTable);
	else
		scrollbar = new ZAF_SCROLL_BAR(0, 0, 15, 0, SBF_SCALE | SBF_HORIZONTAL, WOF_BORDER);
	return (scrollbar);
}

UI_WINDOW_OBJECT *ZAF_SCROLL_BAR::VtSlideNew(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_SCROLL_BAR(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_SCROLL_BAR *scrollbar;
	if (file)
		scrollbar = new ZAF_SCROLL_BAR(name, file, object, objectTable, userTable);
	else
		scrollbar = new ZAF_SCROLL_BAR(0, 0, 0, 5, SBF_SCALE | SBF_VERTICAL, WOF_BORDER);
	return (scrollbar);
}
