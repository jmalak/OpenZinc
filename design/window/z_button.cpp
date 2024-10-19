//	Zinc Interface Library - Z_BUTTON.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_BUTTON
#include "p_window.hpp"

// ----- UIW_BUTTON ---------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_BUTTON_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_BUTTON *object = (UIW_BUTTON *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_NO_ALLOCATE_DATA |
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	SetText(FIELD_TEXT, object->text);
	int value = (int)object->value;
	SetValue(FIELD_VALUE, value);
	SetImage(FIELD_IMAGE, object->bitmapName, ID_BITMAP_IMAGE);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags (group 1).
	SetFlag(FLAG_NORMAL_BUTTON,
		FlagSet(object->btFlags, BTF_RADIO_BUTTON | BTF_CHECK_BOX), 0);
	SetFlag(FLAG_RADIO_BUTTON, object->btFlags, BTF_RADIO_BUTTON);
	SetFlag(FLAG_CHECK_BOX, object->btFlags, BTF_CHECK_BOX);
	// Load the object flags (group 2).
	SetFlag(FLAG_NORMAL_HEIGHT,
		FlagSet(object->btFlags, BTF_AUTO_SIZE), 0);
	SetFlag(FLAG_AUTO_SIZE, object->btFlags, BTF_AUTO_SIZE);
	// Load the object flags (group 3).
	SetFlag(FLAG_DEPTH_2,
		FlagSet(object->btFlags, BTF_NO_3D), 0);
	SetFlag(FLAG_NO_DEPTH, object->btFlags, BTF_NO_3D);
	// Load the object flags (group 4).
	SetFlag(FLAG_2_STATE,
		FlagSet(object->btFlags, BTF_NO_TOGGLE), 0);
	SetFlag(FLAG_1_STATE, object->btFlags, BTF_NO_TOGGLE);
	// Load the object flags (group 5).
	SetFlag(FLAG_NORMAL_ACTION, 
		FlagSet(object->btFlags, BTF_DOWN_CLICK | BTF_DOUBLE_CLICK | BTF_REPEAT), 0);
	SetFlag(FLAG_DOWN_CLICK, object->btFlags, BTF_DOWN_CLICK);
	SetFlag(FLAG_DOUBLE_CLICK, object->btFlags, BTF_DOUBLE_CLICK);
	SetFlag(FLAG_REPEAT_ACTION, object->btFlags, BTF_REPEAT);
	// Load the object flags (group 6).
	SetFlag(FLAG_MESSAGE_BUTTON, object->btFlags, BTF_SEND_MESSAGE);
	SetFlag(FLAG_DEFAULT_BUTTON, object->btFlags, BTF_DEFAULT_BUTTON);
}

int ZAF_OBJECT_EDITOR::UIW_BUTTON_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_BUTTON *object = (UIW_BUTTON *)data;

	// Store the object data.
	int value;
	int changed = FALSE;
	if (GetText(FIELD_TEXT, &object->text))
		object->DataSet(object->text); // Needed to update hotkey.
	if (GetValue(FIELD_VALUE, &value))
	{
		changed |= TRUE;
		object->value = (EVENT_TYPE)value;
	}
	changed |= GetImage(FIELD_IMAGE, &object->bitmapName, ID_BITMAP_IMAGE);
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags (group 1).
	changed |= GetFlag(FLAG_MESSAGE_BUTTON, &object->btFlags, BTF_SEND_MESSAGE);
	changed |= GetFlag(FLAG_RADIO_BUTTON, &object->btFlags, BTF_RADIO_BUTTON);
	changed |= GetFlag(FLAG_CHECK_BOX, &object->btFlags, BTF_CHECK_BOX);
	// Store the object flags (group 2).
	if (GetFlag(FLAG_AUTO_SIZE, &object->btFlags, BTF_AUTO_SIZE))
	{
		changed |= TRUE;
		object->relative.top = object->relative.bottom - (display->cellHeight - display->preSpace - display->postSpace) + 1;
	}
	// Store the object flags (group 3).
	changed |= GetFlag(FLAG_NO_DEPTH, &object->btFlags, BTF_NO_3D);
	// Store the object flags (group 4).
	changed |= GetFlag(FLAG_1_STATE, &object->btFlags, BTF_NO_TOGGLE);
	// Store the object flags (group 5).
	changed |= GetFlag(FLAG_DOWN_CLICK, &object->btFlags, BTF_DOWN_CLICK);
	changed |= GetFlag(FLAG_DOUBLE_CLICK, &object->btFlags, BTF_DOUBLE_CLICK);
	changed |= GetFlag(FLAG_REPEAT_ACTION, &object->btFlags, BTF_REPEAT);
	// Load the object flags (group 6).
	changed |= GetFlag(FLAG_MESSAGE_BUTTON, &object->btFlags, BTF_SEND_MESSAGE);
	changed |= GetFlag(FLAG_DEFAULT_BUTTON, &object->btFlags, BTF_DEFAULT_BUTTON);

	// Return the status.
	return (changed);
}

// ----- ZAF_BUTTON ---------------------------------------------------------

ZAF_BUTTON::ZAF_BUTTON(int left, int top, int width, ZIL_ICHAR *_text,
	BTF_FLAGS _btFlags, WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction,
	EVENT_TYPE _value, ZIL_ICHAR *_bitmapName) :
	UIW_BUTTON(left, top, width, _text, _btFlags, _woFlags, _userFunction, _value, _bitmapName)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_BUTTON::ZAF_BUTTON(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_BUTTON(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_BUTTON::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_BUTTON::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_BUTTON::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_BUTTON(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_BUTTON *button;
	if (file)
		button = new ZAF_BUTTON(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "button");
		button = new ZAF_BUTTON(0, 0, 12, _text1);
	}
	return (button);
}

UI_WINDOW_OBJECT *ZAF_BUTTON::CheckBoxNew(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_BUTTON(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_BUTTON *button;
	if (file)
		button = new ZAF_BUTTON(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "Check-box");
		button = new ZAF_BUTTON(0, 0, 20, _text1, BTF_CHECK_BOX);
	}
	return (button);
}

UI_WINDOW_OBJECT *ZAF_BUTTON::RadioButtonNew(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_BUTTON(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_BUTTON *button;
	if (file)
		button = new ZAF_BUTTON(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "Radio-button");
		button = new ZAF_BUTTON(0, 0, 20, _text1, BTF_RADIO_BUTTON);
	}
	return (button);
}
