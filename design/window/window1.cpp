//	Zinc Interface Library - WINDOW1.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"

#define SPECIAL_GROUPING ID_WINDOW
#define GENERAL_GROUPING ID_WINDOW_OBJECT
const EVENT_TYPE L_BEGIN_GROUPING			= 6015;
const EVENT_TYPE L_CONTINUE_GROUPING		= 6016;
const EVENT_TYPE L_END_GROUPING				= 6017;
static UI_EVENT_MAP ZIL_FARDATA _eventTable[] =
{
#if defined(ZIL_MSDOS)
	// General grouping operations.
	{ GENERAL_GROUPING,	L_BEGIN_GROUPING,		E_MOUSE,		S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ GENERAL_GROUPING,	L_CONTINUE_GROUPING,	E_MOUSE, 		S_CTRL | M_LEFT,				0 },
	{ GENERAL_GROUPING,	L_END_GROUPING,			E_MOUSE, 		S_CTRL | M_LEFT_CHANGE,			0 },
	{ GENERAL_GROUPING,	L_BEGIN_GROUPING,		E_MOUSE,		S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	// Command-line invoked grouping operations.
	{ SPECIAL_GROUPING, L_BEGIN_GROUPING,		E_MOUSE, 		M_LEFT | M_LEFT_CHANGE,			0 },
	{ SPECIAL_GROUPING, L_BEGIN_GROUPING,		E_MOUSE, 		M_LEFT | M_RIGHT | M_LEFT_CHANGE | M_RIGHT_CHANGE,	0 },
	{ SPECIAL_GROUPING, L_CONTINUE_GROUPING,	E_MOUSE, 		M_LEFT,							0 },
	{ SPECIAL_GROUPING, L_CONTINUE_GROUPING,	E_MOUSE, 		M_LEFT | M_RIGHT,				0 },
	{ SPECIAL_GROUPING, L_END_GROUPING,			E_MOUSE, 		M_LEFT_CHANGE,					0 },
	{ SPECIAL_GROUPING, L_END_GROUPING,			E_MOUSE, 		M_RIGHT | M_LEFT_CHANGE,		0 },
#elif defined(ZIL_MSWINDOWS)
	// General grouping operations.
	{ GENERAL_GROUPING,	L_BEGIN_GROUPING,		WM_LBUTTONDOWN,	S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ GENERAL_GROUPING,	L_CONTINUE_GROUPING,	WM_MOUSEMOVE, 	S_CTRL | M_LEFT,				0 },
	{ GENERAL_GROUPING,	L_END_GROUPING,			WM_LBUTTONUP, 	S_CTRL | M_LEFT_CHANGE,			0 },
	// Command-line invoked grouping operations.
	{ SPECIAL_GROUPING, L_BEGIN_GROUPING,		WM_LBUTTONDOWN, M_LEFT | M_LEFT_CHANGE,			0 },
	{ SPECIAL_GROUPING, L_CONTINUE_GROUPING,	WM_MOUSEMOVE, 	M_LEFT,							0 },
	{ SPECIAL_GROUPING, L_END_GROUPING,			WM_LBUTTONUP, 	M_LEFT_CHANGE,					0 },
#elif defined(ZIL_OS2)
	// General grouping operations.
	{ GENERAL_GROUPING,	L_BEGIN_GROUPING,		WM_BUTTON1DOWN,	S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ GENERAL_GROUPING,	L_CONTINUE_GROUPING,	WM_MOUSEMOVE, 	S_CTRL | M_LEFT,				0 },
	{ GENERAL_GROUPING,	L_END_GROUPING,			WM_BUTTON1UP, 	S_CTRL | M_LEFT_CHANGE,			0 },
	// Command-line invoked grouping operations.
	{ SPECIAL_GROUPING, L_BEGIN_GROUPING,		WM_BUTTON1DOWN, M_LEFT | M_LEFT_CHANGE,			0 },
	{ SPECIAL_GROUPING, L_CONTINUE_GROUPING,	WM_MOUSEMOVE, 	M_LEFT,							0 },
	{ SPECIAL_GROUPING, L_END_GROUPING,			WM_BUTTON1UP, 	M_LEFT_CHANGE,					0 },
#elif defined(ZIL_MOTIF)
	{ GENERAL_GROUPING,	L_BEGIN_GROUPING,		ButtonPress,	M_LEFT | M_LEFT_CHANGE,		S_CTRL },
	{ GENERAL_GROUPING,	L_CONTINUE_GROUPING,	MotionNotify,	M_LEFT,						S_CTRL },
	{ GENERAL_GROUPING,	L_END_GROUPING,			ButtonRelease,	M_LEFT_CHANGE,				S_CTRL },
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		ButtonPress,	M_LEFT | M_LEFT_CHANGE,		0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	MotionNotify,	M_LEFT,						0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			ButtonRelease,	M_LEFT_CHANGE,				0 },
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		ButtonPress,	M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	MotionNotify,	M_RIGHT,					0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			ButtonRelease,	M_RIGHT_CHANGE,				0 },
#elif defined(ZIL_MACINTOSH)
	{ GENERAL_GROUPING,	L_BEGIN_GROUPING,		mouseDown,		M_LEFT | M_LEFT_CHANGE,		S_CMD },
	{ GENERAL_GROUPING,	L_CONTINUE_GROUPING,	osEvt,			mouseMovedMessage,			S_CMD },
	{ GENERAL_GROUPING,	L_END_GROUPING,			mouseUp,		M_LEFT_CHANGE,				S_CMD },
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		mouseDown,		M_LEFT | M_LEFT_CHANGE,		0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	osEvt,			mouseMovedMessage,			0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			mouseUp,		M_LEFT_CHANGE,				0 },
#elif defined(ZIL_NEXTSTEP)
	{ GENERAL_GROUPING,	L_BEGIN_GROUPING,		NX_MOUSEDOWN,	M_LEFT | M_LEFT_CHANGE,		S_CTRL },
	{ GENERAL_GROUPING,	L_CONTINUE_GROUPING,	NX_MOUSEMOVED,	M_LEFT,						S_CTRL },
	{ GENERAL_GROUPING,	L_END_GROUPING,			NX_MOUSEUP,	M_LEFT_CHANGE,				S_CTRL },
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		NX_MOUSEDOWN,	M_LEFT | M_LEFT_CHANGE,		0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	NX_MOUSEDRAGGED,	M_LEFT,						0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			NX_MOUSEUP,	M_LEFT_CHANGE,				0 },
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		NX_RMOUSEDOWN,	M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	NX_RMOUSEDRAGGED,	M_RIGHT,					0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			NX_RMOUSEUP,	M_RIGHT_CHANGE,				0 },
#endif
	{ ID_END, 0, 0, 0 } // End of array.
};

// --------------------------------------------------------------------------
// ----- ZAF_WINDOW ---------------------------------------------------------
// --------------------------------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_TEXT, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, ID_SCROLL_BAR, ID_HZ_SCROLL, ID_VT_SCROLL, ID_COMBO_BOX, ID_SPIN_CONTROL, // control
	ID_HZ_LIST, ID_VT_LIST, ID_TOOL_BAR, ID_STATUS_BAR, ID_PULL_DOWN_MENU, ID_PULL_DOWN_ITEM, // selection
	ID_PROMPT, ID_GROUP, ID_ICON, ID_NOTEBOOK, ID_TABLE, ID_WINDOW, ID_DERIVED_OBJECT, // static
	ID_GEOMETRY_MANAGER, ID_EDIT_GROUP, ID_END // end
};

static ZIL_OBJECTID _idPageTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_TEXT, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, ID_SCROLL_BAR, ID_HZ_SCROLL, ID_VT_SCROLL, ID_COMBO_BOX, ID_SPIN_CONTROL, // control
	ID_HZ_LIST, ID_VT_LIST, // selection
	ID_PROMPT, ID_GROUP, ID_ICON, ID_NOTEBOOK, ID_TABLE, ID_WINDOW, ID_DERIVED_OBJECT, // static
	ID_GEOMETRY_MANAGER, ID_END // end
};

ZAF_WINDOW::ZAF_WINDOW(int left, int top, int width, int height,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags,
	UI_HELP_CONTEXT _helpContext, UI_WINDOW_OBJECT *_minObject) :
	// Initialize the base class.
	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags, _helpContext, _minObject)
{
	// Set the default string and numberID.
	ZAF_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	StringID(_serviceManager->ZMSG_tempResourceName());
	numberID = 1;

	// Add the default objects.
	Add(new ZAF_BORDER);
	Add(new ZAF_MAXIMIZE_BUTTON);
	Add(new ZAF_MINIMIZE_BUTTON);
	Add(new ZAF_SYSTEM_BUTTON(SYF_GENERIC));
	Add(new ZAF_TITLE(_serviceManager->ZMSG_untitled()));

	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP | WOAF_LOCKED;
}

ZAF_WINDOW::ZAF_WINDOW(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS),
	editObject(ZIL_NULLP(UI_WINDOW_OBJECT)),
	msgTable(_serviceManager->ZMSG_msgTableName(), ZAF_WINDOW_EDITOR::_storage)
{
	if (!objectTable)
		objectTable = _objectTable;
	if (!userTable)
		userTable = _userTable;
	ZAF_WINDOW::Load(name, file, object, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	ZAF_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP | WOAF_LOCKED;
}

ZAF_WINDOW::~ZAF_WINDOW(void)
{
	if (Index(editGroup) == -1)
		delete editGroup;
}

EVENT_TYPE ZAF_WINDOW::Event(const UI_EVENT &event)
{
	// Check for user controls.
	int i, j;
	EVENT_TYPE ccode = event.type;
	if ((ccode >= D_DESIGNER_FIRST && ccode <= D_DESIGNER_LAST) ||
		(ccode <= -D_DESIGNER_FIRST && ccode >= -D_DESIGNER_LAST))
	{
		for (i = 0; _processTable[i].option; i++)
			if (_processTable[i].option == ccode)
				return ((this->*(_processTable[i].Process))(event));
	}

	// Switch on the event type (using general grouping operations).
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	if (event.InputType() == E_MOUSE)
		ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, GENERAL_GROUPING);
	switch (ccode)
	{
	case D_EDIT_OBJECT:
		{
		editObject = event.windowObject;
		if (editObject->parent && editObject->parent->Inherited(ID_NOTEBOOK)) // TEMPORARY
			editObject = editObject->parent;
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		ZIL_OBJECTID searchID = editObject->SearchID();

		// Check for derived objects.
		UI_ITEM *table = ZAF_WINDOW::_objectTable;
		if (searchID >= ID_DERIVED_START)
		{
			for (i = 0; table[i].value != ID_END; i++)
				if (table[i].value == searchID)
					break;
			for (j = 0; table[j].value != ID_END; j++)
				if (table[i].data == table[j].data)
					break;
			searchID = (ZIL_OBJECTID)table[j].value; // Reset value to the base class.
		}

		// Find a matching edit object.
		for (i = 0; table[i].value != ID_END; i++)
			if (table[i].value == searchID)
				break;
		if (table[i].value != ID_END)
			*windowManager + new ZAF_OBJECT_EDITOR(table[i].text, editObject);
		else if (editObject->Inherited(ID_WINDOW))
			*windowManager + new ZAF_OBJECT_EDITOR(UIW_WINDOW::_className, editObject);
		else
			*windowManager + new ZAF_OBJECT_EDITOR(UI_WINDOW_OBJECT::_className, editObject);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		break;

	case D_SET_OBJECT:
		editObject = event.windowObject;
		Information(I_STATUS_UPDATE, editObject);
		break;

	case D_SET_POSITION:
		// Check for the reset option.
		if (FlagSet(event.rawCode, M_LEFT))
			windowEditor->Information(I_GET_CREATE_OBJECT, &object);
		else
			windowEditor->Information(I_RESET_CREATE_OBJECT, ZIL_NULLP(void));

		// Place the object.
		if (object)
		{
			// Compute the object position within its parent window.
			UI_EVENT addEvent = event;
			TruetoRelative(editObject, event.position, addEvent.position);
			int width = object->relative.Width();
			int height = object->relative.Height();
			object->relative.left = addEvent.position.column / display->cellWidth;
			object->relative.top = addEvent.position.line / display->cellHeight;
			object->relative.right = object->relative.left + width - 1;
			object->relative.bottom = object->relative.top + height - 1;

			// Place the object.
			addEvent.windowObject = object;
			EditPaste(addEvent);
		}
		break;

	case S_DEINITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (!ZAF_SERVICE_MANAGER::_queuedEvent)
			windowEditor->Information(I_STATUS_CLEAR, ZIL_NULLP(void));
		break;

	case L_BEGIN_GROUPING:
		if (!parent) // grouping only works at main level.
		{
			UI_POSITION position = event.position;
			NormalizePosition(this, event, position);
			UI_EVENT dEvent = event;
			dEvent.position = position;

			ccode = UIW_WINDOW::Event(event);
			EVENT_TYPE changed = EditGroupUndefine(dEvent);
			changed |= EditGroupDefine(dEvent);
			if (changed)
				Event(S_REDISPLAY);
		}
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_ADD_OBJECT:
		if (parent && parent->Inherited(ID_NOTEBOOK) &&
			(event.windowObject->SearchID() == ID_WINDOW ||
			 event.windowObject->SearchID() == ID_NOTEBOOK))
			ccode = parent->Event(event);
		else if (support.Index(event.windowObject) == -1 &&
			UIW_WINDOW::Index(event.windowObject) == -1)
		{
			// Get the object identification.
			ZIL_OBJECTID objectID = event.windowObject->SearchID();
			ZIL_OBJECTID *idTable = (parent && parent->Inherited(ID_NOTEBOOK)) ? _idPageTable : _idTable;

			// Add the object to the window.
			for (int i = 0; idTable[i] != ID_END; i++)
				if (idTable[i] == objectID && (objectID != ID_EDIT_GROUP || !parent)) // prevent paste to sub-objects
				{
					// Check for double addition of pull-down menu.
					if (objectID == ID_PULL_DOWN_MENU)
					{
						for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
							if (object->Inherited(ID_PULL_DOWN_MENU))
								return (S_ERROR);
					}

					// Check for addition of a pull-down item
					else if (objectID == ID_PULL_DOWN_ITEM)
					{
						for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
							if (object->Inherited(ID_PULL_DOWN_MENU))
								return (object->Event(event));
						return (S_ERROR);
					}

					// Call the base class to add the object.
					ccode = UIW_WINDOW::Event(event);
					object = event.windowObject;
					if (!object->parent ||
						FlagSet(object->woFlags, WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT) ||
						FlagSet(object->parent->woFlags, WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT))
#if defined(ZIL_MOTIF)
					{
						UI_EVENT cEvent(S_CHANGED);
						cEvent.region = Root()->relative;
						Root()->Event(cEvent);
					}
#else
						Root()->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
#endif
					else
						UIW_WINDOW::Event(UI_EVENT(S_REDISPLAY));
					return (ccode);
				}
			ccode = S_ERROR;
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;

	case S_SUBTRACT_OBJECT:
		if (support.Index(event.windowObject) == -1 &&
			UIW_WINDOW::Index(event.windowObject) == -1)
			return (S_ERROR);
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_DRAG_MOVE_OBJECT:
	case S_DRAG_COPY_OBJECT:
	case S_DRAG_DEFAULT:
		{
		ZIL_OBJECTID *idTable = (parent && parent->Inherited(ID_NOTEBOOK)) ? _idPageTable : _idTable;
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, idTable, event);
		}
		break;

	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
	case S_DROP_DEFAULT:
		{
		ZIL_OBJECTID *idTable = (parent && parent->Inherited(ID_NOTEBOOK)) ? _idPageTable : _idTable;
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, idTable, event);
		}
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		Information(I_STATUS_UPDATE, ZIL_NULLP(void));
		helpSystem->ResetStorage(ZAF_WINDOW_EDITOR::_storage, FALSE);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *ZAF_WINDOW::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object information.
		editObject = this;
		// Check for the window editor.
		windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
		storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
		editGroup = new ZAF_EDIT_GROUP;
		break;

	case I_STATUS_UPDATE:
	case I_STATUS_CLEAR:
		windowEditor->Information(request, (data ? data : (editObject ? editObject : this)));
		break;

	case I_GET_ADDTABLE:
		*(ZIL_OBJECTID **)data = (parent && parent->Inherited(ID_NOTEBOOK)) ? _idPageTable : _idTable;
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void ZAF_WINDOW::Load(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the edit window information.
	ZIL_STORAGE *_deltaFile = ZAF_WINDOW_EDITOR::_deltaFile;
	if (_deltaFile)
		UIW_WINDOW::DeltaLoad(name, name, ZIL_NULLP(ZIL_ICHAR), directory, _deltaFile, objectTable, userTable);
	else
		UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
}

UI_WINDOW_OBJECT *ZAF_WINDOW::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_WINDOW(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_WINDOW *window;
	if (file)
		window = new ZAF_WINDOW(name, file, object, objectTable, userTable);
	else
	{
		window = new ZAF_WINDOW(0, 0, 20, 5);
		window->wnFlags |= WNF_SELECT_MULTIPLE;
	}
	return (window);
}

void ZAF_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
	ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Check for an invalid file name.
	if (name)
		StringID(name);
	else
		name = StringID();
	if (!objectTable)
		objectTable = _objectTable;
	if (!userTable)
		userTable = _userTable;

	// Store the object, checking first for the edit group.
	ZIL_STORAGE *_deltaFile = ZAF_WINDOW_EDITOR::_deltaFile;
	if (Index(editGroup) >= 0)
	{
		int saveCount = editGroup->Count();
		UI_WINDOW_OBJECT *item = editGroup->First();
		UI_WINDOW_OBJECT **saveObject = new UI_WINDOW_OBJECT *[saveCount];
		int leftOffset = editGroup->relative.left;
		int topOffset = editGroup->relative.top;
		for (saveCount = 0; item; saveCount++, item = editGroup->First())
		{
			saveObject[saveCount] = item;
			editGroup->UI_LIST::Subtract(item);
			item->relative.left += leftOffset;
			item->relative.top += topOffset;
			item->relative.right += leftOffset;
			item->relative.bottom += topOffset;
			UI_LIST::Add(item);
		}
		UI_LIST::Subtract(editGroup);
		if (_deltaFile)
		{
			UIW_WINDOW::DeltaStore(name, name, ZIL_NULLP(ZIL_ICHAR), file, _deltaFile, objectTable, userTable);
			_deltaFile->Save();
		}
		else
			UIW_WINDOW::Store(name, file, object, objectTable, userTable);
		for (int i = 0; i < saveCount; i++)
		{
			item = saveObject[i];
			UI_LIST::Subtract(item);
			item->relative.left -= leftOffset;
			item->relative.top -= topOffset;
			item->relative.right -= leftOffset;
			item->relative.bottom -= topOffset;
			editGroup->UI_LIST::Add(item);
		}
		UI_LIST::Add(editGroup);
		delete saveObject;
	}
	else if (_deltaFile)
	{
		UIW_WINDOW::DeltaStore(name, name, ZIL_NULLP(ZIL_ICHAR), file, _deltaFile, objectTable, userTable);
		_deltaFile->Save();
	}
	else
		UIW_WINDOW::Store(name, file, object, objectTable, userTable);
}

// ----- Edit Options -------------------------------------------------------

EVENT_TYPE ZAF_WINDOW::EditCopy(const UI_EVENT &event)
{
	// Store the edit object.
	static ZIL_ICHAR name[] = { 'p','a','s','t','e',0 };
	ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE);
	ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
	editObject->Store(name, &directory, &file);
	file.Seek(0);

	// Load a copy of the object.
	ZIL_NEW_FUNCTION newFunction = editObject->NewFunction();
	UI_WINDOW_OBJECT *object = (newFunction)(name, &directory, &file, _objectTable, _userTable);
	object->searchID = editObject->searchID; // preserve derived types.

	// Update general application information.
	windowEditor->Information(I_SET_PASTE_OBJECT, object);
	if (editObject->parent)
		editObject = editObject->parent;
	Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW::EditCut(const UI_EVENT &event)
{
	// This option should not occur.
	if (editObject == this ||
		editObject->SearchID() == ID_TABLE_RECORD ||
		editObject->SearchID() == ID_TABLE_HEADER)
	{
		UI_ERROR_STUB::Beep();
		return (S_ERROR);
	}

	// Remove the object's geometry management (if any).
	UI_GEOMETRY_MANAGER *gmgr = (UI_GEOMETRY_MANAGER *)editObject->parent->Get(NUMID_GEOMETRY);
	if (gmgr)
	{
		for (UI_CONSTRAINT *constraint = gmgr->First(); constraint; )
		{
			UI_CONSTRAINT *temp = constraint->Next();
			if (editObject == (UI_WINDOW_OBJECT *)constraint->Information(I_GET_OBJECT, ZIL_NULLP(void)))
			{
				gmgr->Subtract(constraint);
				delete constraint;
			}
			constraint = temp;
		}
	}

	// Cut the edit object.
	UI_EVENT subEvent(S_SUBTRACT_OBJECT);
	subEvent.windowObject = editObject;
	UI_WINDOW_OBJECT *pObject = editObject->parent;
	if (pObject->Event(subEvent) == S_ERROR)
	{
		UI_ERROR_STUB::Beep();
		return (S_ERROR);
	}
	windowEditor->Information(I_SET_PASTE_OBJECT, editObject);
	if (editObject == editGroup)
		editGroup = new ZAF_EDIT_GROUP;
	editObject = Current() ? Current() : this; // reset editObject.
	Event(S_REDISPLAY);

	// Update general application information.
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW::EditDelete(const UI_EVENT &event)
{
	// This option should not occur.
	if (editObject == this ||
		editObject->SearchID() == ID_TABLE_RECORD ||
		editObject->SearchID() == ID_TABLE_HEADER)
	{
		UI_ERROR_STUB::Beep();
		return (S_ERROR);
	}

	// Remove the object's geometry management (if any).
	UI_GEOMETRY_MANAGER *gmgr = (UI_GEOMETRY_MANAGER *)editObject->parent->Get(NUMID_GEOMETRY);
	if (gmgr)
	{
		for (UI_CONSTRAINT *constraint = gmgr->First(); constraint; )
		{
			UI_CONSTRAINT *temp = constraint->Next();
			if (editObject == (UI_WINDOW_OBJECT *)constraint->Information(I_GET_OBJECT, ZIL_NULLP(void)))
			{
				gmgr->Subtract(constraint);
				delete constraint;
			}
			constraint = temp;
		}
	}

	// Delete the edit object.
	UI_EVENT subEvent(S_SUBTRACT_OBJECT);
	subEvent.windowObject = editObject;
	UI_WINDOW_OBJECT *pObject = editObject->parent;
	if (pObject->Event(subEvent) == S_ERROR)
	{
		UI_ERROR_STUB::Beep();
		return (S_ERROR);
	}
	if (editObject == editGroup)
		editGroup = new ZAF_EDIT_GROUP;
	delete editObject;
	editObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	pObject->Information(I_GET_CURRENT, &editObject);
	if (!editObject)
		editObject = Current() ? Current() : this; // reset editObject.
#if defined(ZIL_MOTIF)
	UI_EVENT cEvent(S_CHANGED);
	cEvent.region = relative;
	Event(cEvent);
#else
	Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
#endif

	// Update general application information.
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW::EditGroupDefine(const UI_EVENT &_event)
{
	// Make sure the specified window is current.
	if (this != windowManager->First())
		*windowManager + this;

	// Draw the binding rectangle.
	UI_POSITION origin;
	UI_REGION newRegion, oldRegion;
	UI_REGION trueNormalized = trueRegion;
	UI_EVENT event = _event;
	EVENT_TYPE ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, SPECIAL_GROUPING);
#	if defined(ZIL_MSWINDOWS)
	SetCapture(screenID);
#	elif defined(ZIL_OS2)
	WinSetCapture(HWND_DESKTOP, screenID);
#	elif defined(ZIL_MOTIF)
	display->VirtualGet(screenID, 0, 0, trueRegion.Width() - 1, trueRegion.Height() - 1);
#	endif
	newRegion.left = newRegion.right = newRegion.top = newRegion.bottom = -1;

	eventManager->DeviceState(E_MOUSE, DM_POSITION_IMAGE);
	do
	{
		// Get the user input (using special grouping operations).
		if (ccode != L_BEGIN_GROUPING)
		{
			eventManager->Get(event);
			ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, SPECIAL_GROUPING);

			// Normalize the coordinates for later use.
			NormalizePosition(this, event, event.position);
		}
#		if defined(ZIL_MACINTOSH)
		else
		{
			// Normalize the coordinates for later use.
			NormalizePosition(this, event, event.position);
		}

		// Toolbox windows are in global coordinates.
		ZIL_DEFINE_RECT(tRect, trueRegion.top, trueRegion.left, trueRegion.bottom, trueRegion.right);
		if (FlagsSet(woStatus, WOS_MACINTOSH_WINDOW | WOS_SYSTEM_OBJECT))
		{
			GlobalToLocal(&topLeft(tRect));
			GlobalToLocal(&botRight(tRect));
		}
		trueNormalized.Assign(tRect);
#		endif

		// Check for mouse events.
		if (ccode == L_END_GROUPING ||
			(ccode == L_BEGIN_GROUPING && !trueNormalized.Overlap(event.position)))
		{
			// System must process begin/end mouse messages.
			eventManager->Put(event, Q_BEGIN);
			break;
		}
		else if (event.InputType() != E_MOUSE)
        {
			// Account for system events.
			windowManager->Event(event);
            continue;
        }
		else if (!trueNormalized.Overlap(event.position))
            continue; // Mouse doesn't overlap boundary.

		// Modify the group coordinates.
		if (ccode == L_BEGIN_GROUPING)
		{
			origin = event.position;
			newRegion.left = newRegion.right = origin.column;
			newRegion.top = newRegion.bottom = origin.line;
			oldRegion = newRegion;
#			if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
			display->VirtualGet(screenID, trueRegion);
			display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
			display->VirtualPut(screenID);
#			else
			display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE);
#			endif
			ccode = L_CONTINUE_GROUPING;
		}
		else if (ccode == L_CONTINUE_GROUPING)
		{
			// Change the group position.
			if (event.position.column > origin.column)
				newRegion.right = event.position.column, newRegion.left = origin.column;
			else
				newRegion.left = event.position.column, newRegion.right = origin.column;
			if (event.position.line > origin.line)
				newRegion.bottom = event.position.line, newRegion.top = origin.line;
			else
				newRegion.top = event.position.line, newRegion.bottom = origin.line;
			// Change the group rectangle.
			if (oldRegion != newRegion)
			{
#				if defined(ZIL_MSWINDOWS)||  defined(ZIL_OS2) || defined(ZIL_MOTIF)
				display->VirtualGet(screenID, trueRegion);
				display->Rectangle(screenID, oldRegion, display->xorPalette, 1, FALSE, TRUE);
				display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
				display->VirtualPut(screenID);
#				else
				display->RectangleXORDiff(oldRegion, newRegion);
#				endif
				oldRegion = newRegion;
			}
		}
	} while (ccode != L_END_GROUPING);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);

#	if defined(ZIL_MSWINDOWS)
	display->VirtualGet(screenID, trueRegion);
	display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->VirtualPut(screenID);
	newRegion.left -= trueRegion.left;
	newRegion.top -= trueRegion.top;
	newRegion.right -= trueRegion.left;
	newRegion.bottom -= trueRegion.top;
	ReleaseCapture();
#	elif defined(ZIL_OS2)
	display->VirtualGet(screenID, trueRegion);
	display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->VirtualPut(screenID);
	newRegion.left -= trueRegion.left;
	newRegion.top -= trueRegion.top;
	newRegion.right -= trueRegion.left;
	newRegion.bottom -= trueRegion.top;
	WinSetCapture(HWND_DESKTOP, 0);
#	elif defined(ZIL_MOTIF)
	display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->VirtualPut(screenID);
#	else
	display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE);
#	endif

	// Determine the new group objects.
	if (_event.type == OPT_EDIT_GROUP_DEFINE)
		EditGroupUndefine(_event);
	UI_GEOMETRY_MANAGER *gmgr = (UI_GEOMETRY_MANAGER *)Get(NUMID_GEOMETRY);
	for (UI_WINDOW_OBJECT *object = First(); object; )
	{
		// Get the next object.
		UI_WINDOW_OBJECT *temp = object->Next();
		if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION) && newRegion.Overlap(object->trueRegion))
		{
			// Subtract the object from the general window.
			Subtract(object);

			// Remove the object's geometry management (if any).
			if (gmgr)
			{
				for (UI_CONSTRAINT *constraint = gmgr->First(); constraint; )
				{
					UI_CONSTRAINT *temp = constraint->Next();
					if (object == (UI_WINDOW_OBJECT *)constraint->Information(I_GET_OBJECT, ZIL_NULLP(void)))
					{
						gmgr->Subtract(constraint);
						delete constraint;
					}
					constraint = temp;
				}
			}

			// Add the object to the edit group.
			editGroup->Add(object);
		}
		object = temp;
	}

	// Update the edit group information.
	if (editGroup->First())
	{
		Add(editGroup);
		editObject = editGroup;
		Information(I_STATUS_UPDATE, editGroup);
		if (_event.type == OPT_EDIT_GROUP_DEFINE)
			Event(S_REDISPLAY);
	}
	else
		Information(I_STATUS_UPDATE, ZIL_NULLP(void));

	// If user initiated object needs to be re-displayed.
	return (editGroup->First() ? TRUE : FALSE);
}

EVENT_TYPE ZAF_WINDOW::EditGroupUndefine(const UI_EVENT &event)
{
	// Check for items in the group.
	if (!editGroup->First())
		return (FALSE);

	// Remove the grouped items.
	for (UI_WINDOW_OBJECT *object = editGroup->First(); object; object = editGroup->First())
	{
		editGroup->Subtract(object);
		Add(object);
	}
	Subtract(editGroup);

	// If user initiated object needs to be re-displayed.
	if (event.type == OPT_EDIT_GROUP_UNDEFINE)
		Event(S_REDISPLAY);
	Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (TRUE);
}

EVENT_TYPE ZAF_WINDOW::EditMove(const UI_EVENT &event)
{
	// Move the edit object.
	if (editObject && editObject->parent)
	{
		UI_EVENT moveEvent(L_MOVE);
		moveEvent.rawCode = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		moveEvent.position.column = editObject->trueRegion.right;
		moveEvent.position.line = editObject->trueRegion.bottom;
		editObject->Modify(moveEvent);
	}
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW::EditObject(const UI_EVENT &)
{
	// Copy the edit object.
	UI_EVENT eEvent(D_EDIT_OBJECT);
	eEvent.windowObject = editObject;
	return (Event(eEvent));
}

EVENT_TYPE ZAF_WINDOW::EditPaste(const UI_EVENT &event)
{
	// Set the place object.
	UI_WINDOW_OBJECT *object = event.windowObject;
	if (event.type == OPT_EDIT_PASTE)
	{
		windowEditor->Information(I_GET_PASTE_OBJECT, &object);
		if (object->Inherited(ID_EDIT_GROUP) ||
			object->Inherited(ID_PULL_DOWN_ITEM))
			editObject = this;
	}
	if (!object)
	{
		UI_ERROR_STUB::Beep();
		return (S_ERROR);
	}

	// Add the object, then redisplay the window.
	UI_EVENT addEvent(S_ADD_OBJECT);
	addEvent.windowObject = object;
	// Preserve derived information (if any).
	ZIL_OBJECTID _searchID = object->searchID;
	object->searchID = object->windowID[0];

	// place the edit object.
	if (object->Inherited(ID_EDIT_GROUP))
	{
		if (Index(editGroup) >= 0)
		{
			UI_EVENT rEvent(S_DISPLAY_ACTIVE);
			rEvent.region = editGroup->trueRegion;
			EditGroupUndefine(event);
			Event(rEvent);
		}
		UI_WINDOW_OBJECT *temp;
		addEvent.type = S_SUBTRACT_OBJECT;
		for (object->Information(I_GET_FIRST, &temp); temp; object->Information(I_GET_FIRST, &temp))
		{
			addEvent.windowObject = temp;
			object->Event(addEvent);
			editGroup->Add(temp);
		}
		delete object;
		addEvent.type = S_ADD_OBJECT;
		addEvent.windowObject = object = editGroup;
		object->woStatus &= ~WOS_EDIT_MODE;
		editObject->Event(addEvent);
		object->woStatus |= WOS_EDIT_MODE;
		editObject = object;
	}
	// check for menu item placement.
	else if (object->Inherited(ID_PULL_DOWN_ITEM))
	{
		UI_WINDOW_OBJECT *menu = NULL;
		for (menu = (UI_WINDOW_OBJECT *)support.First(); menu; menu = menu->Next())
			if (menu->Inherited(ID_PULL_DOWN_MENU))
			{
				menu->Event(addEvent);
				break;
			}
		if (!menu) // no menu was found.
		{
			UI_ERROR_STUB::Beep();
			delete object;
			return (S_ERROR);
		}
	}
	else if (!editObject || editObject->Event(addEvent) == S_ERROR)
	{
		UI_ERROR_STUB::Beep();
		delete object;
		return (S_ERROR);
	}

	// Update the edit object (must be event.windowObject, not object).
	editObject = addEvent.windowObject;
	if (editObject == object)
		editObject->searchID = _searchID;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	Information(I_STATUS_UPDATE, object);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW::EditSize(const UI_EVENT &event)
{
	// Move the edit object.
	if (editObject && editObject->parent)
	{
		UI_EVENT sizeEvent(L_SIZE);
		sizeEvent.rawCode = M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		sizeEvent.position.column = editObject->trueRegion.right;
		sizeEvent.position.line = editObject->trueRegion.bottom;
		editObject->Modify(sizeEvent);
	}
	return (event.type);
}

// ----- Static tables ------------------------------------------------------

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_WINDOW::_userTable = __userTable;

extern ZIL_ICHAR _radioName[];
extern ZIL_ICHAR _checkName[];
extern ZIL_ICHAR _hzScrollName[];
extern ZIL_ICHAR _vtScrollName[];

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BIGNUM, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_BIGNUM::New), 			UIW_BIGNUM::_className, 			0 },
	{ ID_BORDER, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_BORDER::New), 			UIW_BORDER::_className, 			0 },
	{ ID_BUTTON, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_BUTTON::New), 			UIW_BUTTON::_className, 			0 },
	{ ID_CHECK_BOX, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_BUTTON::CheckBoxNew), 	_checkName, 						0 },
	{ ID_COMBO_BOX, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_COMBO_BOX::New), 			UIW_COMBO_BOX::_className, 			0 },
	{ ID_DATE, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DATE::New), 				UIW_DATE::_className, 				0 },
	{ ID_FORMATTED_STRING, 	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_FORMATTED_STRING::New),	UIW_FORMATTED_STRING::_className,	0 },
	{ ID_GROUP, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_GROUP::New), 				UIW_GROUP::_className, 				0 },
	{ ID_HZ_LIST, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_HZ_LIST::New), 			UIW_HZ_LIST::_className, 			0 },
	{ ID_HZ_SCROLL, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_SCROLL_BAR::HzSlideNew), 	_hzScrollName, 						0 },
	{ ID_ICON, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_ICON::New), 				UIW_ICON::_className, 				0 },
	{ ID_INTEGER, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_INTEGER::New), 			UIW_INTEGER::_className, 			0 },
	{ ID_MAXIMIZE_BUTTON, 	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_MAXIMIZE_BUTTON::New), 	UIW_MAXIMIZE_BUTTON::_className, 	0 },
	{ ID_MINIMIZE_BUTTON, 	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_MINIMIZE_BUTTON::New), 	UIW_MINIMIZE_BUTTON::_className, 	0 },
	{ ID_NOTEBOOK, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_NOTEBOOK::New), 			UIW_NOTEBOOK::_className, 			0 },
	{ ID_POP_UP_ITEM, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_POP_UP_ITEM::New), 		UIW_POP_UP_ITEM::_className, 		0 },
	{ ID_POP_UP_MENU, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_POP_UP_MENU::New), 		UIW_POP_UP_MENU::_className, 		0 },
	{ ID_PROMPT, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_PROMPT::New), 			UIW_PROMPT::_className, 			0 },
	{ ID_PULL_DOWN_ITEM, 	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_PULL_DOWN_ITEM::New), 	UIW_PULL_DOWN_ITEM::_className, 	0 },
	{ ID_PULL_DOWN_MENU, 	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_PULL_DOWN_MENU::New), 	UIW_PULL_DOWN_MENU::_className, 	0 },
	{ ID_RADIO_BUTTON, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_BUTTON::RadioButtonNew), 	_radioName, 						0 },
	{ ID_REAL, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_REAL::New), 				UIW_REAL::_className, 				0 },
	{ ID_SCROLL_BAR, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_SCROLL_BAR::New), 		UIW_SCROLL_BAR::_className, 		0 },
	{ ID_SPIN_CONTROL, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_SPIN_CONTROL::New), 		UIW_SPIN_CONTROL::_className, 		0 },
	{ ID_STATUS_BAR, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_STATUS_BAR::New), 		UIW_STATUS_BAR::_className, 		0 },
	{ ID_STRING, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_STRING::New), 			UIW_STRING::_className, 			0 },
	{ ID_SYSTEM_BUTTON, 	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_SYSTEM_BUTTON::New), 		UIW_SYSTEM_BUTTON::_className, 		0 },
	{ ID_TABLE, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_TABLE::New), 				UIW_TABLE::_className, 				0 },
	{ ID_TABLE_HEADER, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_TABLE_HEADER::New), 		UIW_TABLE_HEADER::_className, 		0 },
	{ ID_TABLE_RECORD, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_TABLE_RECORD::New), 		UIW_TABLE_RECORD::_className, 		0 },
	{ ID_TEXT, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_TEXT::New), 				UIW_TEXT::_className, 				0 },
	{ ID_TIME, 				(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_TIME::New), 				UIW_TIME::_className, 				0 },
	{ ID_TITLE, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_TITLE::New), 				UIW_TITLE::_className, 				0 },
	{ ID_TOOL_BAR, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_TOOL_BAR::New), 			UIW_TOOL_BAR::_className, 			0 },
	{ ID_VT_LIST, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_VT_LIST::New), 			UIW_VT_LIST::_className, 			0 },
	{ ID_VT_SCROLL, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_SCROLL_BAR::VtSlideNew), 	_vtScrollName, 						0 },
	{ ID_WINDOW, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_WINDOW::New), 			UIW_WINDOW::_className, 			0 },
	// ----- Geometry Management -----
	{ ID_GEOMETRY_MANAGER, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(UI_GEOMETRY_MANAGER::New), 		UI_GEOMETRY_MANAGER::_className, 		0 },
	{ ID_ATTACHMENT, 			(UI_WINDOW_OBJECT*)ZIL_VOIDF(UI_ATTACHMENT::New), 			   	UI_ATTACHMENT::_className, 				0 },
	{ ID_DIMENSION_CONSTRAINT,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(UI_DIMENSION_CONSTRAINT::New),	UI_DIMENSION_CONSTRAINT::_className,	0 },
	{ ID_RELATIVE_CONSTRAINT, 	(UI_WINDOW_OBJECT*)ZIL_VOIDF(UI_RELATIVE_CONSTRAINT::New), 	UI_RELATIVE_CONSTRAINT::_className, 	0 },
	// ----- Special Objects -----
	{ ID_EDIT_GROUP, 		(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_EDIT_GROUP::New),			ZAF_EDIT_GROUP::_className, 		0 },
	{ ID_DERIVED_OBJECT,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	// ----- Derived Objects -----
	{ ID_DERIVED_START+0,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+1,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+2,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+3,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+4,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+5,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+6,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+7,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+8,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+9,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+10,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+11,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+12,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+13,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+14,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+15,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+16,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+17,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+18,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+19,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+20,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+21,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+22,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+23,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+24,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+25,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+26,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+27,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+28,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	{ ID_DERIVED_START+29,	(UI_WINDOW_OBJECT*)ZIL_VOIDF(ZAF_DERIVED_OBJECT::New),		ZAF_DERIVED_OBJECT::_className, 	0 },
	// ----- End of Array -----
	{ ID_END, 				ZIL_NULLP(void), 						ZIL_NULLP(ZIL_ICHAR), 				0 }
};
UI_ITEM *ZAF_WINDOW::_objectTable = __objectTable;
UI_ITEM *ZAF_WINDOW::_deriveTable = ZIL_NULLP(UI_ITEM);

#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_WINDOW::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_WINDOW::PROCESS ZAF_WINDOW::_processTable[] =
{
	{ OPT_EDIT_COPY, 			ZIL_PROCESS_REFERENCE(EditCopy) },				// edit requests
	{ OPT_EDIT_CUT, 			ZIL_PROCESS_REFERENCE(EditCut) },
	{ OPT_EDIT_DELETE, 			ZIL_PROCESS_REFERENCE(EditDelete) },
	{ OPT_EDIT_GROUP_DEFINE, 	ZIL_PROCESS_REFERENCE(EditGroupDefine) },
	{ OPT_EDIT_GROUP_UNDEFINE, 	ZIL_PROCESS_REFERENCE(EditGroupUndefine) },
	{ OPT_EDIT_MOVE, 			ZIL_PROCESS_REFERENCE(EditMove) },
	{ OPT_EDIT_OBJECT, 			ZIL_PROCESS_REFERENCE(EditObject) },
	{ OPT_EDIT_PASTE, 			ZIL_PROCESS_REFERENCE(EditPaste) },
	{ OPT_EDIT_SIZE, 			ZIL_PROCESS_REFERENCE(EditSize) },
	{ 0, 0 }
};
