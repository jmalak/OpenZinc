//	Zinc Interface Library - Z_SYS.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_SYSTEM_BUTTON
#include "p_window.hpp"

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_SYSTEM_BUTTON_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_SYSTEM_BUTTON *object = (UIW_SYSTEM_BUTTON *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
	SetFlag(FLAG_GENERIC, object->syFlags, SYF_GENERIC);
}

int ZAF_OBJECT_EDITOR::UIW_SYSTEM_BUTTON_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_SYSTEM_BUTTON *object = (UIW_SYSTEM_BUTTON *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetFlag(FLAG_GENERIC, &object->syFlags, SYF_GENERIC);

	// Return the status.
	return (changed);
}

// ----- ZAF_SYSTEM_BUTTON --------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_POP_UP_ITEM, // selection
	ID_END // end
};

ZAF_SYSTEM_BUTTON::ZAF_SYSTEM_BUTTON(SYF_FLAGS _syFlags) :
	UIW_SYSTEM_BUTTON(_syFlags)
{
	// Set the associated edit flags.
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_SYSTEM_BUTTON::ZAF_SYSTEM_BUTTON(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_SYSTEM_BUTTON(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_SYSTEM_BUTTON::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		menu.support.Index(event.windowObject) == -1 &&
		menu.UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idTable[i] != ID_END; i++)
			if (_idTable[i] == objectID)
			{
				ccode = UIW_SYSTEM_BUTTON::Event(event);
				Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		menu.support.Index(event.windowObject) == -1 &&
		menu.UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_SYSTEM_BUTTON::Event(event);
	return (ccode);
}

void *ZAF_SYSTEM_BUTTON::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_SYSTEM_BUTTON::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_SYSTEM_BUTTON::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_SYSTEM_BUTTON(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_SYSTEM_BUTTON *sysButton;
	if (file)
		sysButton = new ZAF_SYSTEM_BUTTON(name, file, object, objectTable, userTable);
	else
		sysButton = new ZAF_SYSTEM_BUTTON;
	return (sysButton);
}

