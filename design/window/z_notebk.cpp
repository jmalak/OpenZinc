//	Zinc Interface Library - Z_NOTEBK.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_NOTEBOOK
#include "p_window.hpp"

// ----- UIW_NOTEBOOK -------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_NOTEBOOK_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_NOTEBOOK *object = (UIW_NOTEBOOK *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
}

int ZAF_OBJECT_EDITOR::UIW_NOTEBOOK_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_NOTEBOOK *object = (UIW_NOTEBOOK *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);

	// Return the status.
	return (changed);
}

// ----- ZAF_NOTEBOOK -------------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_TEXT, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, ID_SCROLL_BAR, ID_HZ_SCROLL, ID_VT_SCROLL, ID_COMBO_BOX, ID_SPIN_CONTROL, // control
	ID_HZ_LIST, ID_VT_LIST, // selection
	ID_PROMPT, ID_GROUP, ID_ICON, ID_TABLE, ID_WINDOW, ID_DERIVED_OBJECT, // static
	ID_GEOMETRY_MANAGER, ID_END // end
};

ZAF_NOTEBOOK::ZAF_NOTEBOOK(void) :
	UIW_NOTEBOOK()
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_NOTEBOOK::ZAF_NOTEBOOK(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_NOTEBOOK(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_NOTEBOOK::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		if (objectID == ID_NOTEBOOK) // Check for notebook.
		{
			UIW_WINDOW *window = new ZAF_WINDOW(0, 0, 20, 5);
			window->NumberID(Root()->NumberID());
			Root()->NumberID(window->NumberID()+1);
			Add(window);
			parent->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
			UI_WINDOW_OBJECT **tmpObject = (UI_WINDOW_OBJECT **)&event.windowObject;
			*tmpObject = window;
			return (ccode);
		}
		for (int i = 0; _idTable[i] != ID_END; i++)
			if (_idTable[i] == objectID)
			{
				ccode = UIW_NOTEBOOK::Event(event);
				parent->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT)
	{
		// Cannot delete last page or tabs.
		int index = Index(event.windowObject);
		if (First() == Last() || index == -1)
			return (S_ERROR);

		// Notify any potentially affected editors (dirList & objList).
		// tab only-it is automatically deleted by UIW_NOTEBOOK.
		for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (object->Inherited(ID_BUTTON) && index-- == 0)
			{
				for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
					if (window->SearchID() == ID_OBJECT_EDITOR)
						window->Information(I_CHECK_EDIT_DELETE, object);
				break;
			}
		ccode = UIW_NOTEBOOK::Event(event);
	}
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_NOTEBOOK::Event(event);
	return (ccode);
}

void *ZAF_NOTEBOOK::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_NOTEBOOK::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_NOTEBOOK::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_NOTEBOOK(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_NOTEBOOK *notebook;
	if (file)
		notebook = new UIW_NOTEBOOK(name, file, object, objectTable, userTable);
	else
	{
		notebook = new ZAF_NOTEBOOK;
		*notebook + new ZAF_WINDOW(0, 0, 20, 5);
	}
	return (notebook);
}

