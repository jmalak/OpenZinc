//	Zinc Interface Library - Z_FMTSTR.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_FORMATTED_STRING
#include "p_window.hpp"

// ----- UIW_FORMATTED_STRING -----------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_FORMATTED_STRING_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_FORMATTED_STRING *object = (UIW_FORMATTED_STRING *)data;
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
	SetText(FIELD_TEXT, object->DataGet(TRUE));
	SetText(FIELD_EDIT, object->editMask);
	SetText(FIELD_DELETE, object->deleteText);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	SetFlag(FLAG_AUTO_CLEAR, object->woFlags, WOF_AUTO_CLEAR);
}

int ZAF_OBJECT_EDITOR::UIW_FORMATTED_STRING_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_FORMATTED_STRING *object = (UIW_FORMATTED_STRING *)data;

	// Store the object data.
	int changed = FALSE;
	ZIL_ICHAR *compressedText = ZIL_NULLP(ZIL_ICHAR);
	changed |= GetText(FIELD_TEXT, &compressedText);
	changed |= GetText(FIELD_EDIT, object->editMask);
	changed |= GetText(FIELD_DELETE, object->deleteText);
	if (changed)
	{
		int length = strlen(object->editMask);
		for (int i = strlen(object->deleteText); i < length; i++)
			object->deleteText[i] = ' ';
		object->deleteText[length] = 0;
		object->DataSet(compressedText);
		if (compressedText)
			delete compressedText;
		changed = FALSE; // DataSet automatically updates.
	}
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	changed |= GetFlag(FLAG_AUTO_CLEAR, &object->woFlags, WOF_AUTO_CLEAR);

	// Return the status.
	return (changed);
}

// ----- ZAF_FORMATTED_STRING -----------------------------------------------

ZAF_FORMATTED_STRING::ZAF_FORMATTED_STRING(int left, int top, int width,
	ZIL_ICHAR *_compressedText, ZIL_ICHAR *_editMask, ZIL_ICHAR *_deleteText,
	WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction) :
	UIW_FORMATTED_STRING(left, top, width, _compressedText, _editMask, _deleteText, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_FORMATTED_STRING::ZAF_FORMATTED_STRING(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_FORMATTED_STRING(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

EVENT_TYPE ZAF_FORMATTED_STRING::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT || ccode == S_SUBTRACT_OBJECT)
		ccode = S_ERROR;
	else
		ccode = UIW_FORMATTED_STRING::Event(event);
	return (ccode);
}

UI_WINDOW_OBJECT *ZAF_FORMATTED_STRING::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_FORMATTED_STRING(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_FORMATTED_STRING *fmtstr;
	if (file)
		fmtstr = new ZAF_FORMATTED_STRING(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32], _text2[32], _text3[32];
		NormalizeString(_text1, "__________");
		NormalizeString(_text2, "LNNNLLNNNLNNNN");
		NormalizeString(_text3, "(___) ___-____");
		fmtstr = new ZAF_FORMATTED_STRING(0, 0, 20, _text1, _text2, _text3);
	}
	return (fmtstr);
}
