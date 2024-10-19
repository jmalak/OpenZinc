//	Zinc Interface Library - Z_TEXT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_TEXT
#include "p_window.hpp"

// ----- UIW_TEXT -----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_TEXT_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TEXT *object = (UIW_TEXT *)data;
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
	SetScroll(FLAG_VERTICAL_SCROLL_BAR, object, SBF_VERTICAL);
	SetScroll(FLAG_HORIZONTAL_SCROLL_BAR, object, SBF_HORIZONTAL);
	// Load the object flags.
	SetFlag(FLAG_NO_WRAP, object->wnFlags, WNF_NO_WRAP);
	SetFlag(FLAG_AUTO_CLEAR, object->woFlags, WOF_AUTO_CLEAR);
}

int ZAF_OBJECT_EDITOR::UIW_TEXT_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TEXT *object = (UIW_TEXT *)data;

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
	int options = FALSE;
	options |= GetScroll(FLAG_VERTICAL_SCROLL_BAR, object, SBF_VERTICAL);
	options |= GetScroll(FLAG_HORIZONTAL_SCROLL_BAR, object, SBF_HORIZONTAL);
	if (options)
	{
		// Subtract, then Add support objects to ensure proper ordering.
		changed |= TRUE;
		UI_WINDOW_OBJECT *cscroll = object->Get(NUMID_C_SCROLL);
		UI_WINDOW_OBJECT *vscroll = object->Get(NUMID_VT_SCROLL);
		UI_WINDOW_OBJECT *hscroll = object->Get(NUMID_HZ_SCROLL);
		*object - cscroll - vscroll - hscroll;
		if (cscroll) // Match up the center scroll.
			delete cscroll;
		cscroll = ZIL_NULLP(UI_WINDOW_OBJECT);
		if (vscroll && hscroll && !cscroll)
			cscroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER);
		*object + cscroll + vscroll + hscroll;
	}
	// Store the object flags.
	changed |= GetFlag(FLAG_NO_WRAP, &object->wnFlags, WNF_NO_WRAP);
	changed |= GetFlag(FLAG_AUTO_CLEAR, &object->woFlags, WOF_AUTO_CLEAR);

	// Return the status.
	return (changed);
}

// ----- ZAF_TEXT -----------------------------------------------------------

ZAF_TEXT::ZAF_TEXT(int left, int top, int width, int height, ZIL_ICHAR *_text,
	int _maxLength, WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_TEXT(left, top, width, height, _text, _maxLength, _wnFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_TEXT::ZAF_TEXT(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TEXT(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_TEXT::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_TEXT::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_TEXT::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_TEXT(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_TEXT *text;
	if (file)
		text = new ZAF_TEXT(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "text");
		text = new ZAF_TEXT(0, 0, 20, 5, _text1, 256);
		*text + new ZAF_SCROLL_BAR(0, 0, 1, 5, SBF_VERTICAL);
	}
	return (text);
}
