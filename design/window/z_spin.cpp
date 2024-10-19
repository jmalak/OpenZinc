//	Zinc Interface Library - Z_SPIN.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_SPIN_CONTROL
#include "p_window.hpp"

static UI_ITEM _objectTable[] =
{
	{ ID_BIGNUM, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BIGNUM::New), 			UIW_BIGNUM::_className, 			0 },
	{ ID_DATE, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_DATE::New), 				UIW_DATE::_className, 				0 },
	{ ID_INTEGER, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_INTEGER::New), 			UIW_INTEGER::_className, 			0 },
	{ ID_REAL, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_REAL::New), 				UIW_REAL::_className, 				0 },
	{ ID_TIME, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TIME::New), 				UIW_TIME::_className, 				0 },
	{ ID_END, 				ZIL_NULLP(void), 						ZIL_NULLP(ZIL_ICHAR), 				0 }
};

// ----- UIW_SPIN_CONTROL ---------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_SPIN_CONTROL_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_SPIN_CONTROL *object = (UIW_SPIN_CONTROL *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_NO_ALLOCATE_DATA | WOF_AUTO_CLEAR |
		WOF_VIEW_ONLY | WOF_UNANSWERED | WOF_INVALID | WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	int delta = (int)object->delta;
	SetValue(FIELD_DELTA, delta);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Load the object flags.
	SetFlag(FLAG_NO_WRAP, object->wnFlags, WNF_NO_WRAP);

	// Load the object type.
	UI_WINDOW_OBJECT *field = notebook->Get(FIELD_SPIN_TYPE);
	UI_EVENT event(S_ADD_OBJECT);
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = ZAF_WINDOW_EDITOR::_storage;
	ZIL_OBJECTID _searchID = object->fieldObject->searchID;
	for (int i = 0; _objectTable[i].value != ID_END; i++)
	{
		ZIL_ICHAR *name = nameTable.GetMessage((ZIL_NUMBERID)_objectTable[i].value);
		event.windowObject = new UIW_BUTTON(0, 0, 20, name,
			BTF_NO_3D, WOF_NO_FLAGS, ZIL_NULLF(ZIL_USER_FUNCTION),
			_objectTable[i].value, _objectTable[i].text);
		event.windowObject->StringID(_objectTable[i].text);
		if (object->fieldObject->Inherited((ZIL_OBJECTID)_objectTable[i].value))
			_searchID = (ZIL_OBJECTID)_objectTable[i].value;
		field->Event(event);
	}
	ZIL_ICHAR *name = nameTable.GetMessage(_searchID);
	field->Information(I_SET_TEXT, name);
	defaultStorage = saveStorage;
}

int ZAF_OBJECT_EDITOR::UIW_SPIN_CONTROL_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_SPIN_CONTROL *object = (UIW_SPIN_CONTROL *)data;

	// Store the object data.
	int changed = FALSE;
	int delta;
	if (GetValue(FIELD_DELTA, &delta))
	{
		changed |= TRUE;
		object->delta = delta;
	}
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	// Store the object flags.
	changed |= GetFlag(FLAG_NO_WRAP, &object->wnFlags, WNF_NO_WRAP);

	// Store the object type.
	UI_WINDOW_OBJECT *field = notebook->Get(FIELD_SPIN_TYPE);
	UI_WINDOW_OBJECT *selection = ZIL_NULLP(UI_WINDOW_OBJECT);
	field->Information(I_GET_CURRENT, &selection);
	if (selection &&
		FlagSet(field->woStatus, WOS_CHANGED) &&
		stricmp(object->fieldObject->ClassName(), selection->StringID()))
	{
		field->woStatus &= ~WOS_CHANGED;
		UI_WINDOW_OBJECT *windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
		UI_EVENT addEvent(S_ADD_OBJECT);
		addEvent.windowObject = (UI_WINDOW_OBJECT *)windowEditor->Information(I_GET_DUPLICATE_OBJECT, selection->StringID());
		object->Event(addEvent);
		changed |= TRUE;
	}

	// Return the status.
	return (changed);
}

// ----- ZAF_SPIN_CONTROL ---------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_END // end
};

ZAF_SPIN_CONTROL::ZAF_SPIN_CONTROL(int left, int top, int width,
	UI_WINDOW_OBJECT *_fieldObject, ZIL_INT32 _delta,
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction) :
	UIW_SPIN_CONTROL(left, top, width, _fieldObject, _delta, _wnFlags, _woFlags, _userFunction)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_SPIN_CONTROL::ZAF_SPIN_CONTROL(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_SPIN_CONTROL(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_SPIN_CONTROL::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idTable[i] != ID_END; i++)
			if (_idTable[i] == objectID)
			{
				ccode = UIW_SPIN_CONTROL::Event(event);
				UIW_SPIN_CONTROL::Event(UI_EVENT(S_REDISPLAY));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT)
		return (S_ERROR); // Cannot remove any children (must be replaced by adding).
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_SPIN_CONTROL::Event(event);
	return (ccode);
}

void *ZAF_SPIN_CONTROL::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_SPIN_CONTROL::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_SPIN_CONTROL::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_SPIN_CONTROL(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_SPIN_CONTROL *spin;
	if (file)
		spin = new ZAF_SPIN_CONTROL(name, file, object, objectTable, userTable);
	else
	{
		UIW_INTEGER *field = new UIW_INTEGER(0, 0, 20, ZIL_NULLP(int));
		spin = new ZAF_SPIN_CONTROL(0, 0, 20, field);
	}
	return (spin);
}

