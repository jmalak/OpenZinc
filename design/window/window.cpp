//	Zinc Interface Library - WINDOW.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <storage.hpp>
#include "window.hpp"
#define USE_MSG_TABLE
#include "p_window.hpp"

extern ZIL_ICHAR *_rootStorageDirectory;
extern ZIL_ICHAR *_windowDirectory;
extern ZIL_ICHAR *_cppDirectory;
extern ZIL_ICHAR *_hppDirectory;

static ZIL_ICHAR _testName[] = { 'T','E','S','T','_','W','I','N','D','O','W',0 };
static ZIL_ICHAR _deriveTableName[] = { 'D','E','R','I','V','E','_','T','A','B','L','E', 0 };
static ZIL_ICHAR _defaultExtension[] = { '.','d','a','t', 0 };

// --------------------------------------------------------------------------
// ----- ZAF_OBJECT_BUTTON --------------------------------------------------
// --------------------------------------------------------------------------

class ZAF_OBJECT_BUTTON : public UIW_BUTTON
{
public:
	static int _width;
	static int _height;

	ZAF_OBJECT_BUTTON(ZIL_OBJECTID objectID, ZIL_ICHAR *name);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	static EVENT_TYPE Message(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

int ZAF_OBJECT_BUTTON::_width = 10;
int ZAF_OBJECT_BUTTON::_height = 10;

ZAF_OBJECT_BUTTON::ZAF_OBJECT_BUTTON(ZIL_OBJECTID objectID, ZIL_ICHAR *name) :
	UIW_BUTTON(0, 0, 2, ZIL_NULLP(ZIL_ICHAR), BTF_AUTO_SIZE | BTF_NO_TOGGLE,
	WOF_JUSTIFY_CENTER, Message, objectID, name)
{
	font = FNT_SMALL_FONT;
// line below commented out by jdh as drop and drag interferes with
// expected behavior in 32 bit windows
//	woAdvancedFlags |=  WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT;

	StringID(name);
	RegionConvert(relative, TRUE);
	relative.right = relative.left + _width - 1;
	relative.bottom = relative.top + _height - 1;
}

EVENT_TYPE ZAF_OBJECT_BUTTON::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_BUTTON);
	if (ccode == S_INITIALIZE)
	{
		// Default to the base class, then reset the width and height.
		btFlags &= ~BTF_AUTO_SIZE;
		ccode = UIW_BUTTON::Event(event);
		if (bitmapWidth > _width)
			_width = bitmapWidth;
		if (bitmapHeight > _height)
			_height = bitmapHeight;

		// Redo the relative button positions based on the largest bitmap.
		for (UI_WINDOW_OBJECT *object = this; !next && object; object = object->Previous())
		{
			object->relative.right = object->relative.left + _width + 7;
			object->relative.bottom = object->relative.top + _height + 5;
		}
	}
	else if (ccode == S_CREATE)
	{
		ccode = UI_WINDOW_OBJECT::Event(event);
		UIW_BUTTON::Event(S_REGISTER_OBJECT);
	}
	else
		ccode = UIW_BUTTON::Event(event);
	return (ccode);
}

EVENT_TYPE ZAF_OBJECT_BUTTON::Message(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = D_CREATE_OBJECT;
		event.windowObject = object;
		object->eventManager->Put(event);
	}
	return (ccode);
}

// --------------------------------------------------------------------------
// ----- ZAF_WINDOW_EDITOR --------------------------------------------------
// --------------------------------------------------------------------------

const EVENT_TYPE OPT_OBJECT_CREATE	 	= 14000;

#if defined(__DECCXX)
ZAF_WINDOW_EDITOR::ZAF_WINDOW_EDITOR() : UIW_WINDOW (0, 0, 0, 0) {}
#endif

ZAF_WINDOW_EDITOR::ZAF_WINDOW_EDITOR(WOAF_FLAGS _woAdvancedFlags,
	WOS_STATUS _woStatus) :
	// Initialize the base class.
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_window"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable),
	// Initialize the class members.
	msgTable(_serviceManager->ZMSG_msgTableName(), _serviceManager->Storage(&_storage, "p_window")),
	nameTable(_nameTableName, _serviceManager->Storage(&_storage, "p_window"))
{
	// Set the static storage and storage services.
	searchID = ID_WINDOW_EDITOR;
	woStatus |= _woStatus;
	woAdvancedFlags |= _woAdvancedFlags;
	createObject = resetObject = pasteObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	directoryService = _serviceManager->Get(ID_DIRECTORY_SERVICE);
	storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	Information(I_COPY_TEXT, originalTitle);
	SetTitle();

	// Initialize the derive table.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	storageService->Information(I_GET_STORAGE, &editFile);
	ZAF_WINDOW::_deriveTable = ResetDeriveTable(editFile, ZAF_WINDOW::_objectTable);

	// Create the toolbar and add the palette buttons.
	if (!display->isText)
	{
		ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
		defaultStorage = ZAF_WINDOW_EDITOR::_storage;
		UIW_TOOL_BAR *toolbar = new UIW_TOOL_BAR(0, 0, 0, 0);
		for (int i = 0; ZAF_OBJECT_EDITOR::_classTable[i].objectID != ID_END; i++)
			if (ZAF_OBJECT_EDITOR::_classTable[i].showObject)
			{
				ZAF_OBJECT_BUTTON *button = new ZAF_OBJECT_BUTTON(
					ZAF_OBJECT_EDITOR::_classTable[i].objectID,
					ZAF_OBJECT_EDITOR::_classTable[i].className);
// line below commented out by jdh as drop and drag interferes with
// expected behavior in 32 bit windows
//				button->woAdvancedFlags |= WOAF_COPY_DRAG_OBJECT;
				*toolbar + button;
			}
		defaultStorage = saveStorage;
		Add(toolbar);
	}

	// Initialize the default options.
	ZAF_WINDOW_PREFERENCES::LoadOptions();
	ZAF_OBJECT_EDITOR::InitializeStaticMembers();
	StatusUpdate();

	// Center the window on the screen.
	windowManager->Center(this);
	relative.bottom = relative.Height() - 1;
	relative.top = 0;

	// Remove the File option if this is a child-service.
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
	{
		UI_WINDOW_OBJECT *menuItem = Get(_serviceManager->ZMSG_fileOption());
		menuItem->woFlags |= WOF_NON_SELECTABLE;
	}
	SetTitle();
}

ZAF_WINDOW_EDITOR::~ZAF_WINDOW_EDITOR(void)
{
	// Clean up static resources.
	ZAF_OBJECT_EDITOR::RestoreStaticMembers();

	// Remove the resource storage.
	if (_storage)
		delete _storage;
	if (_deltaFile)
		delete _deltaFile;
	if (pasteObject)
		delete pasteObject;
}

EVENT_TYPE ZAF_WINDOW_EDITOR::Event(const UI_EVENT &event)
{
	// Check for user controls.
	EVENT_TYPE ccode = event.type;
	if ((ccode >= D_DESIGNER_FIRST && ccode <= D_DESIGNER_LAST) ||
		(ccode <= -D_DESIGNER_FIRST && ccode >= -D_DESIGNER_LAST))
	{
		for (int i = 0; _processTable[i].option; i++)
			if (_processTable[i].option == ccode)
				return ((this->*(_processTable[i].Process))(event));
	}

	// Switch on the event type.
	switch (ccode)
	{
	case D_CREATE_OBJECT:
	case OPT_OBJECT_CREATE:
		if (createObject != event.windowObject)
		{
			createObject = event.windowObject;
			resetObject = ZIL_NULLP(UI_WINDOW_OBJECT);
			StatusUpdate();
		}
		break;

	case OPT_OBJECT_CLEAR:
		resetObject = createObject = ZIL_NULLP(UI_WINDOW_OBJECT);
		StatusUpdate();
		break;

	case OPT_OBJECT_RESET:
		Information(I_RESET_CREATE_OBJECT, ZIL_NULLP(void));
		StatusUpdate();
		break;

	case S_RESTORE:
		ccode = UIW_WINDOW::Event(event);
		SetTitle();
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(_storage, FALSE);
		break;

	case S_OPEN_DOCUMENT:
		if (event.text)
		{
			if (ZAF_SERVICE_MANAGER::_changedData)
				FileQuerySave(event);	// Continue saving process.
			FileOpen(event);
			delete event.text;
		}
		break;

	case OPT_ACTIVATE_SERVICE:
		ccode = _serviceManager->Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *ZAF_WINDOW_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the storage service.
		*windowManager + this;
		if (FlagSet(woStatus, WOS_MINIMIZED))
		{
			// Restore the minimized window.
			UI_EVENT restoreEvent(L_RESTORE);
			restoreEvent.windowObject = this;
			Event(restoreEvent);
		}
		break;

	case I_GET_STORAGE:
	case I_SET_STORAGE:
		data = storageService->Information(request, data, objectID);
		break;

	case I_GET_PASTE_OBJECT:
		if (pasteObject)
		{
			// Preserve the storage (save storage allows bitmaps & icons).
			ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
			storageService->Information(I_GET_STORAGE, &defaultStorage);

			// Get a duplicate of the paste object.
			static ZIL_ICHAR name[] = { 'p','a','s','t','e',0 };
			ZIL_NEW_FUNCTION newFunction = pasteObject->NewFunction();
			ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY);
			ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
			pasteObject->Store(name, &directory, &file);
			file.Seek(0);

			UI_WINDOW_OBJECT *window = (newFunction)(name, &directory, &file, ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);
			window->relative.right -= window->relative.left;
			window->relative.bottom -= window->relative.top;
			window->relative.left = window->relative.top = 0;
			window->searchID = pasteObject->searchID; // preserve derived types.
			window->numberID = 0;
			window->stringID[0] = '\0';
			// Make sure geometry manager links are set, before clearing the IDs.
			UI_WINDOW_OBJECT *gmgr = window->Get(NUMID_GEOMETRY);
			if (gmgr)
				gmgr->Event(S_INITIALIZE);
			ClearNumberIDs(window);

			// Restore the storage.
			*(UI_WINDOW_OBJECT **)data = window;
			defaultStorage = saveStorage;
		}
		else
			*(UI_WINDOW_OBJECT **)data = ZIL_NULLP(UI_WINDOW_OBJECT);
		break;

	case I_SET_PASTE_OBJECT:
		if (pasteObject)
			delete pasteObject;
		pasteObject = (UI_WINDOW_OBJECT *)data;
		break;

	case I_GET_DUPLICATE_OBJECT:
		return (CreateObject((ZIL_ICHAR *)data));

	case I_GET_CREATE_OBJECT:
		if (createObject)
			*(UI_WINDOW_OBJECT **)data = CreateObject(ZIL_NULLP(ZIL_ICHAR));
		else
		{
			*(UI_WINDOW_OBJECT **)data = ZIL_NULLP(UI_WINDOW_OBJECT);
			break;
		}
		// Continue to I_RESET_CREATE_OBJECT.

	case I_RESET_CREATE_OBJECT:
		if (createObject)
		{
			resetObject = createObject;
			createObject = ZIL_NULLP(UI_WINDOW_OBJECT);
		}
		else
		{
			createObject = resetObject;
			resetObject = ZIL_NULLP(UI_WINDOW_OBJECT);
		}
		StatusUpdate();
		break;

	case I_STATUS_UPDATE:
	case I_STATUS_CLEAR:
		StatusUpdate((UI_WINDOW_OBJECT *)data, (request == I_STATUS_CLEAR));
		break;

	case I_SET_TEXT:
		if (data)
			data = UIW_WINDOW::Information(request, data, objectID);
		else
			SetTitle(); // window specific update for delta storage.
		break;

	case I_GET_ORIGINAL_TITLE:
		if (!data)
			return (originalTitle);
		*(ZIL_ICHAR **)data = originalTitle;
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void ZAF_WINDOW_EDITOR::ClearNumberIDs(UI_WINDOW_OBJECT *window)
{
	extern ZIL_ICHAR *_genericFieldID;
	ZIL_ICHAR format[32];
	strcpy(format, _genericFieldID);
	int length = strlen(format);

	// Clear the window numberID & stringID.
	itoa(window->numberID, &format[length], 10);
	if (!stricmp(format, window->stringID))
		window->stringID[0] = '\0';
	window->numberID = 0;

	// Check for special sub-types.
	UI_WINDOW_OBJECT *subWindow = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUBWINDOW, &subWindow);
	if (subWindow && subWindow->NumberID() < (ZIL_NUMBERID)0xFF00)
	{
		itoa(subWindow->numberID, &format[length], 10);
		if (!stricmp(format, subWindow->stringID))
			subWindow->stringID[0] = '\0';
		subWindow->numberID = 0;
	}

	// Clear the sub-objects (if any).
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUPPORT_FIRST, &object);
	if (!object)
		window->Information(I_GET_FIRST, &object);
	while (object)
	{
		// Clear the sub-item.
		if (object->NumberID() < (ZIL_NUMBERID)0xFF00)
			ClearNumberIDs(object);
		if (!object->Next() && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
			window->Information(I_GET_FIRST, &object);
		else
			object = object->Next();
	}
}

UI_WINDOW_OBJECT *ZAF_WINDOW_EDITOR::CreateObject(ZIL_ICHAR *name)
{
	// Check for a valid name or create object.
	if (!name && !createObject)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
	else if (!name && createObject)
		name = createObject->StringID();

	// Create the specified object.
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	for (int i = 0; ZAF_OBJECT_EDITOR::_classTable[i].objectID != ID_END; i++)
		if (!stricmp(ZAF_OBJECT_EDITOR::_classTable[i].className, name))
		{
			// Reset the default storage for bitmaps & icons in window file.
			ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
			defaultStorage = ZAF_WINDOW_EDITOR::_storage;

			// Create, then reset the information for automatic placement.
			ZIL_NEW_FUNCTION newFunction = (ZIL_NEW_FUNCTION)ZAF_OBJECT_EDITOR::_classTable[i].New;
			object = (newFunction)(ZIL_NULLP(const ZIL_ICHAR), ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
				ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));

			// Set the object position.
			object->relative.right -= object->relative.left;
			object->relative.bottom -= object->relative.top;
			object->relative.left = object->relative.top = 0;

			// Set the number and string identification.
			UI_WINDOW_OBJECT *root = GetEditWindow();
			ZIL_ICHAR _stringID[32];
			extern ZIL_ICHAR *_genericFieldID;
			strcpy(_stringID, _genericFieldID);
			itoa(root->NumberID(), &_stringID[strlen(_stringID)], 10);
			object->StringID(_stringID);
			object->NumberID(root->NumberID());
			root->NumberID(root->NumberID()+1);

			// Restore the default storage.
			defaultStorage = saveStorage;
			break;
		}
	return (object);
}

UI_ITEM *ZAF_WINDOW_EDITOR::ResetDeriveTable(ZIL_STORAGE *editFile, UI_ITEM *objectTable)
{
	// Reset the derive table entries.
	UI_ITEM *deriveTable = ZIL_NULLP(UI_ITEM);
	int i;
	for (i = 0; !deriveTable && objectTable[i].value != ID_END; i++)
		if (objectTable[i].value >= ID_DERIVED_START)
		{
			deriveTable = &objectTable[i];
			for (int j = 0; deriveTable[j].value != ID_END; j++)
			{
				deriveTable[j].data = (UI_WINDOW_OBJECT*)ZAF_DERIVED_OBJECT::New;
				if (deriveTable[j].text != ZAF_DERIVED_OBJECT::_className)
					delete deriveTable[j].text;
				deriveTable[j].text = ZAF_DERIVED_OBJECT::_className;
				deriveTable[j].flags = 0;
			}
		}

	// Load entries from the edit file, if any.
	if (editFile)
	{
		editFile->ChDir(_rootStorageDirectory);
		ZIL_STORAGE_OBJECT_READ_ONLY deriveFile(*editFile, _deriveTableName, 0);
		if (!deriveFile.objectError)
		{
			ZIL_OBJECTID value;
			while (deriveFile.Load(&value) && value >= ID_DERIVED_START)
			{
				int offset = 0;
				// Find the matching derive table entry.
				for (i = 0; deriveTable[i].value != ID_END; i++)
					if (deriveTable[i].value == value)
					{
						offset = i;
						break;
					}
				deriveFile.Load(&value);
				// Find the matching object (base classe) table entry.
				deriveTable[offset].data = ZIL_NULLP(void);
				for (i = 0; objectTable[i].value != ID_END; i++)
					if (objectTable[i].value == value)
					{
						deriveTable[offset].data = objectTable[i].data;
						deriveTable[offset].flags = value;
						break;
					}
				deriveFile.Load(&deriveTable[offset].text);
			}
		}
	}

	// Return a pointer to the derive table.
	return (deriveTable);
}

UI_WINDOW_OBJECT *ZAF_WINDOW_EDITOR::GetEditWindow(void)
{
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (FlagSet(window->woStatus, WOS_EDIT_MODE))
			return (window);
	return (ZIL_NULLP(UI_WINDOW_OBJECT));
}

void ZAF_WINDOW_EDITOR::ModifySearchID(UI_WINDOW_OBJECT *window, ZIL_OBJECTID oldSearchID, ZIL_OBJECTID newSearchID)
{
	// Check for matching searchID.
	if (window->searchID == oldSearchID)
		window->searchID = newSearchID;

	// Check for special sub-types.
	UI_WINDOW_OBJECT *subWindow = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUBWINDOW, &subWindow);
	if (subWindow && subWindow->searchID == oldSearchID)
		subWindow->searchID = newSearchID;

	// Check the sub-objects (if any).
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUPPORT_FIRST, &object);
	if (!object)
		window->Information(I_GET_FIRST, &object);
	while (object)
	{
		// Check the sub-item.
		ModifySearchID(object, oldSearchID, newSearchID);
		if (!object->Next() && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
			window->Information(I_GET_FIRST, &object);
		else
			object = object->Next();
	}
}

void ZAF_WINDOW_EDITOR::SetTitle(void)
{
	static ZIL_ICHAR _format[] = { '%','s',' ','-',' ','%','s',' ','-',' ','%','s',0 };
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService)
		storageService->Information(I_GET_STORAGE, &editFile);

	ZIL_ICHAR fullTitle[ZIL_MAXPATHLEN];
	if (editFile)
	{
		ZIL_ICHAR pathName[ZIL_MAXPATHLEN], fileName[ZIL_MAXPATHLEN];
		editFile->StorageName(pathName);
		editFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), fileName);
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, fileName);
	}
	else
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, _serviceManager->ZMSG_noFile());
	if (_deltaFile)
	{
		static ZIL_ICHAR _deltaTitle[] = { ' ','(','D',')',0 };
		strcat(fullTitle, _deltaTitle);
	}
	Information(I_SET_TEXT, fullTitle);
}

void ZAF_WINDOW_EDITOR::ShiftHelpIDs(ZIL_STORAGE *srcFile, ZIL_STORAGE *dstFile, UI_WINDOW_OBJECT *window)
{
	// This algorithm creates a conversion table from old to new help
	// contexts.  This is a recursive routine that creates a help context
	// entry in the dstFile to match the specified object's help context
	// entry.  _context is a static buffer that contains help context
	// entries from the source window that need to be placed in the
	// destination file.

	// Set up the context pairs.
	static int _totalContexts = 0;
	static UI_HELP_CONTEXT _lastContext = 0;
	static struct HELP_PAIR
	{
		UI_HELP_CONTEXT oldHelpContext;
		UI_HELP_CONTEXT newHelpContext;
	} *_context = ZIL_NULLP(HELP_PAIR);

	// Initialize the static variables.
	if (!window->parent)
	{
		// Move to the help directory.
		extern ZIL_ICHAR *_rootStorageDirectory;
		srcFile->ChDir(_rootStorageDirectory);
		dstFile->ChDir(_rootStorageDirectory);
		extern ZIL_ICHAR *_helpDirectory;
		srcFile->ChDir(_helpDirectory);
		if (dstFile->ChDir(_helpDirectory))
		{
			dstFile->MkDir(_helpDirectory);
			dstFile->ChDir(_helpDirectory);
		}

		// Determine the last help context.
		_totalContexts = 0;
		_lastContext = 1;
		_context = ZIL_NULLP(HELP_PAIR);
		for (ZIL_ICHAR *name = dstFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); name; name = dstFile->FindNextObject())
		{
			ZIL_STORAGE_OBJECT_READ_ONLY hObject(*dstFile, name, 0);
			if (hObject.objectID > _lastContext)
				_lastContext = hObject.objectID + 1;
		}
	}

	// Check for an affected help context.
	UI_HELP_CONTEXT helpContext = window->helpContext;
	for (int i = 0; helpContext && i < _totalContexts; i++)
		if (helpContext == _context[i].oldHelpContext)
		{
			window->helpContext = _context[i].newHelpContext;
			helpContext = 0;
			break;
		}
	if (helpContext)
	{
		// Check for a full context buffer.
		if (_totalContexts % 50 == 0)
		{
			HELP_PAIR *temp = _context;
			_context = new HELP_PAIR[_totalContexts + 50];
			for (int i = 0; i < _totalContexts; i++)
			{
				_context[i].oldHelpContext = temp[i].oldHelpContext;
				_context[i].newHelpContext = temp[i].newHelpContext;
			}
			delete temp;
		}

		// Create a new help context.
		_context[_totalContexts].oldHelpContext = helpContext;

		// Store the help information, if any.
		int length;
		char buffer[1024];
		ZIL_ICHAR *helpName = srcFile->FindFirstID(helpContext);
		if (helpName)
		{
			_context[_totalContexts].newHelpContext = ++_lastContext;
			ZIL_STORAGE_OBJECT_READ_ONLY srcObject(*srcFile, helpName, 0);
			ZIL_STORAGE_OBJECT dstObject(*dstFile, helpName, _lastContext, UIS_CREATE | UIS_READWRITE);
			while ((length = srcObject.Load(buffer, 1024, 1)) > 0)
				dstObject.Store(buffer, length, 1);
		}
		else // cannot find a valid help context.
			_context[_totalContexts].newHelpContext = NO_HELP_CONTEXT;
		window->helpContext = _context[_totalContexts].newHelpContext;
		_totalContexts++;
	}

	// Check the sub-objects (if any).
	if (window->NumberID() == NUMID_GEOMETRY)
		return; // Constraints can't have help contexts.
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUPPORT_FIRST, &object);
	if (!object)
		window->Information(I_GET_FIRST, &object);
	while (object)
	{
		// Check the sub-item.
		ShiftHelpIDs(srcFile, dstFile, object);
		if (!object->Next() &&
			FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
			window->Information(I_GET_FIRST, &object);
		else
			object = object->Next();
	}

	// Restore the static variables.
	if (!window->parent)
	{
		// Restore to the window directory.
		srcFile->ChDir(_rootStorageDirectory);
		dstFile->ChDir(_rootStorageDirectory);
		extern ZIL_ICHAR *_windowDirectory;
		srcFile->ChDir(_windowDirectory);
		dstFile->ChDir(_windowDirectory);

		// Clear the context array.
		if (_context)
			delete _context;
	}
}

void ZAF_WINDOW_EDITOR::StatusUpdate(UI_WINDOW_OBJECT *editObject, int clear)
{
	// Temporary status field identifiers.
	const ZIL_NUMBERID OBJECT_FIELD                     = 0x01F4;
	const ZIL_NUMBERID STRINGID_FIELD                   = 0x01F5;
	const ZIL_NUMBERID POSITION_FIELD                   = 0x01F6;
	const ZIL_NUMBERID SIZE_FIELD                       = 0x01F7;
	const ZIL_NUMBERID PLACE_OBJECT_FIELD               = 0x01F8;
	static int initialized = FALSE;

	if (!initialized)
	{
		Get(OBJECT_FIELD)->Font(FNT_SMALL_FONT);
		Get(STRINGID_FIELD)->Font(FNT_SMALL_FONT);
		Get(POSITION_FIELD)->Font(FNT_SMALL_FONT);
		Get(SIZE_FIELD)->Font(FNT_SMALL_FONT);
		Get(PLACE_OBJECT_FIELD)->Font(FNT_SMALL_FONT);
		initialized = TRUE;
	}
	static UI_WINDOW_OBJECT *lastEdit = ZIL_NULLP(UI_WINDOW_OBJECT);

	// Set the edit object information.
	ZIL_ICHAR *tempData;
	if (editObject)
	{
		// Set the object type.
		ZIL_OBJECTID objectType = ID_WINDOW;
		ZIL_ICHAR *name = ZIL_NULLP(ZIL_ICHAR);
		int i;
		for (i = 0; ZAF_WINDOW::_objectTable[i].value != ID_END; i++)
			if (ZAF_WINDOW::_objectTable[i].value == editObject->searchID)
			{
				objectType = (ZIL_OBJECTID)ZAF_WINDOW::_objectTable[i].value;
				break;
			}
		name = (objectType < ID_DERIVED_START) ? nameTable.GetMessage(objectType) : ZAF_WINDOW::_objectTable[i].text;
		if (!name && ZAF_WINDOW::_objectTable[i].value != ID_END)
			name = ZAF_WINDOW::_objectTable[i].text;

		// Set the object name.
		Get(OBJECT_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, name))
			Get(OBJECT_FIELD)->Information(I_SET_TEXT, name);

		// Set the string identification.
		Get(STRINGID_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, editObject->StringID()))
			Get(STRINGID_FIELD)->Information(I_SET_TEXT, editObject->StringID());

		// Set the position and size.
		ZIL_ICHAR position[32], size[32];
		static ZIL_ICHAR _format[] = { '%','s',' ','%','d',',',' ','%','d',0 };
		UI_REGION region;
		ZIL_ICHAR *coordinate;
		if (FlagSet(editObject->woFlags, WOF_PIXEL))
			coordinate = msgTable.GetMessage(ZMSG_PIXEL);
		else if (FlagSet(editObject->woFlags, WOF_MINICELL))
			coordinate = msgTable.GetMessage(ZMSG_MINICELL);
		else
			coordinate = msgTable.GetMessage(ZMSG_CELL);
		ZAF_OBJECT_EDITOR::NaturalCoordinate(editObject, region, editObject->woFlags);

		// Set the position.
		sprintf(position, _format, coordinate, region.left, region.top);
		Get(POSITION_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, position))
			Get(POSITION_FIELD)->Information(I_SET_TEXT, position);

		// Set the size.
		sprintf(size, _format, coordinate, region.Width(), region.Height());
		Get(SIZE_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, size))
			Get(SIZE_FIELD)->Information(I_SET_TEXT, size);
		lastEdit = editObject->parent;
	}
	else if (clear)
	{
		// Clear the last edit.
		lastEdit = ZIL_NULLP(UI_WINDOW_OBJECT);
		// Set the object name.
		Get(OBJECT_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, _blankString))
			Get(OBJECT_FIELD)->Information(I_SET_TEXT, _blankString);
		// Set the string identification.
		Get(STRINGID_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, _blankString))
			Get(STRINGID_FIELD)->Information(I_SET_TEXT, _blankString);
		// Set the position.
		Get(POSITION_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, _blankString))
			Get(POSITION_FIELD)->Information(I_SET_TEXT, _blankString);
		// Set the size.
		Get(SIZE_FIELD)->Information(I_GET_TEXT, &tempData);
		if (strcmp(tempData, _blankString))
			Get(SIZE_FIELD)->Information(I_SET_TEXT, _blankString);
	}

	// Set the create object information.
	ZIL_ICHAR *name = ZIL_NULLP(ZIL_ICHAR);
	for (int i = 0; !name && createObject && ZAF_WINDOW::_objectTable[i].value != ID_END; i++)
		if (!stricmp(createObject->StringID(), ZAF_WINDOW::_objectTable[i].text))
			name = nameTable.GetMessage((int)ZAF_WINDOW::_objectTable[i].value);
	if (!name)
		name = createObject ? createObject->StringID() : _blankString;
	Get(PLACE_OBJECT_FIELD)->Information(I_GET_TEXT, &tempData);
	if (strcmp(tempData, name))
		Get(PLACE_OBJECT_FIELD)->Information(I_SET_TEXT, name);

	// ----- Modify the menu items. -----
	UI_WINDOW_OBJECT *field;
	WOF_FLAGS flag = WOF_NON_SELECTABLE;

	const ZIL_NUMBERID FLD_EDIT = 0x0002;
	field = Get(FLD_EDIT);
	if (clear)
		editObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	else if (!editObject)
		editObject = GetEditWindow();
	if ((editObject && FlagSet(field->woFlags, flag)) || (!editObject && !FlagSet(field->woFlags, flag)))
	{
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));

		const ZIL_NUMBERID FLD_STORE = 0x006F;
		field = Get(FLD_STORE);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));

		const ZIL_NUMBERID FLD_STOREAS = 0x0070;
		field = Get(FLD_STOREAS);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));

		const ZIL_NUMBERID FLD_CLEAR = 0x009A;
		field = Get(FLD_CLEAR);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));

		const ZIL_NUMBERID FLD_CLEARALL = 0x009B;
		field = Get(FLD_CLEARALL);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));

		const ZIL_NUMBERID FLD_TEST = 0x00AD;
		field = Get(FLD_TEST);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
	}

	const ZIL_NUMBERID FLD_PASTE = 0x0032;
	field = Get(FLD_PASTE);
	if ((pasteObject && FlagSet(field->woFlags, flag)) || (!pasteObject && !FlagSet(field->woFlags, flag)))
	{
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
	}
	const ZIL_NUMBERID FLD_CUT = 0x0030;
	field = Get(FLD_CUT);
	if ((editObject && FlagSet(field->woFlags, flag)) || (!editObject && !FlagSet(field->woFlags, flag)))
	{
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));

		const ZIL_NUMBERID FLD_DELETE = 0x0033;
		field = Get(FLD_DELETE);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
	}
}

// ----- Edit Options -------------------------------------------------------

EVENT_TYPE ZAF_WINDOW_EDITOR::EditRequest(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *editWindow = GetEditWindow();
	if (!editWindow)
		return (storageService->Event(S_ERROR));
	return (editWindow->Event(event));
}

// ----- File Options -------------------------------------------------------

EVENT_TYPE ZAF_WINDOW_EDITOR::FileClose(const UI_EVENT &event)
{
	// Close the edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &editFile))
		return (directoryService->Event(S_ERROR));
	else if (editFile && ZAF_SERVICE_MANAGER::_changedData)
		FileQuerySave(event); // Continue saving process.

	// Close the message file.
	delete editFile;
	storageService->Information(I_SET_STORAGE, ZIL_NULLP(ZIL_STORAGE));
	SetTitle();
	ZAF_WINDOW::_deriveTable = ResetDeriveTable(ZIL_NULLP(ZIL_STORAGE), ZAF_WINDOW::_objectTable);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FileDelete(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Delete the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	if (unlink(pathName))
		return (directoryService->Event(S_ERROR));
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FileNew(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Try to create the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_STORAGE *newFile = new ZIL_STORAGE(pathName, UIS_CREATE | UIS_READWRITE);
	if (newFile->storageError)
	{
		delete newFile;
		return (directoryService->Event(S_ERROR));
	}

	// See if there is a pre-existing edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &editFile))
		delete editFile;
	editFile = newFile;
	storageService->Information(I_SET_STORAGE, editFile);
	SetTitle();
	ZAF_WINDOW::_deriveTable = ResetDeriveTable(editFile, ZAF_WINDOW::_objectTable);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FileOpen(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Try to open the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_STORAGE *openFile = new ZIL_STORAGE(pathName, UIS_READWRITE);
	if (openFile->storageError)
	{
		delete openFile;
		return (directoryService->Event(S_ERROR));
	}

	// See if there is a pre-existing edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &editFile))
		delete editFile;
	editFile = openFile;
	storageService->Information(I_SET_STORAGE, editFile);
	SetTitle();
	ZAF_WINDOW::_deriveTable = ResetDeriveTable(editFile, ZAF_WINDOW::_objectTable);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FilePreferences(const UI_EVENT &event)
{
	*windowManager + new ZAF_WINDOW_PREFERENCES;
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FileRequest(const UI_EVENT &event)
{
	// Make sure there is a directory service.
	if (!directoryService)
		return (S_UNKNOWN);
	else if ((event.type == OPT_FILE_NEW || event.type == OPT_FILE_OPEN) &&
		ZAF_SERVICE_MANAGER::_changedData)
		FileQuerySave(event); // Continue saving process.

	// Make a request to the directory service.
	directoryService->Information(I_SET_FILTER, _serviceManager->ZMSG_datFilter());
	directoryService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	directoryService->Information(I_SET_REQUEST, &ccode);
	directoryService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FileQuerySave(const UI_EVENT &)
{
	// Construct the title, message and file name.
	ZIL_ICHAR *title = msgTable.GetMessage(ZMSG_SAVE_TITLE);
	ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_SAVE_MESSAGE);
	ZIL_ICHAR name[ZIL_MAXPATHLEN], pathName[ZIL_MAXPATHLEN];
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	storageService->Information(I_GET_STORAGE, &editFile);
	editFile->StorageName(pathName);
	editFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);

	// Prompt the user to save the file.
	ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
		ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, name);
	UI_ERROR_STUB::Beep();
	if (window.Control() == ZIL_DLG_YES)
		return (FileSave(OPT_FILE_SAVE)); // Continue saving process.
	return (FALSE);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FileSave(const UI_EVENT &event)
{
	// Make sure there is an edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &editFile))
		return (directoryService->Event(S_ERROR));

	// Make sure the edit file has a valid name.
	ZIL_ICHAR name[ZIL_MAXPATHLEN], pathName[ZIL_MAXPATHLEN];
	editFile->StorageName(pathName);
	editFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);
	if (!stricmp(name, _serviceManager->ZMSG_tempFileName()))
		return (Event(OPT_FILE_SAVEAS)); // Continue saving process.

	// Save the current edit file.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (FlagSet(window->woStatus, WOS_EDIT_MODE))
			window->Store(window->StringID(), editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
	objectTable = ZAF_WINDOW::_objectTable;

	// Generate help service information.
	UI_WINDOW_OBJECT *helpService = _serviceManager->Get(ID_HELP_EDITOR);
	if (helpService)
		helpService->Information(I_GENERATE_HPP, ZIL_NULLP(void));

	// Save general file information.
	storageService->Information(I_GENERATE_CPP, ZIL_NULLP(void)); // CPP must be first.
	storageService->Information(I_GENERATE_HPP, ZIL_NULLP(void));
	objectTable = ZIL_NULLP(UI_ITEM);
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	editFile->Save(revisions);
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::FileSaveAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data && event.type == -OPT_FILE_SAVEAS)
		return (event.type);

	// See if there is a pre-existing edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &editFile))
		return (directoryService->Event(S_ERROR));
	else if (event.type == OPT_FILE_SAVEAS)
		return (FileRequest(event));

	// Save the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	if (!strrchr(pathName, '.'))
		strcat(pathName, _defaultExtension);
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (FlagSet(window->woStatus, WOS_EDIT_MODE))
			window->Store(window->StringID(), editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	if (editFile->SaveAs(pathName, revisions))
	{
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		return (directoryService->Event(S_ERROR));
	}
	objectTable = ZAF_WINDOW::_objectTable;

	// Generate help service information.
	UI_WINDOW_OBJECT *helpService = _serviceManager->Get(ID_HELP_EDITOR);
	if (helpService)
		helpService->Information(I_GENERATE_HPP, ZIL_NULLP(void));

	// Save general file information.
	storageService->Information(I_GENERATE_CPP, ZIL_NULLP(void));
	storageService->Information(I_GENERATE_HPP, ZIL_NULLP(void));
	objectTable = ZIL_NULLP(UI_ITEM);
	editFile->Save(); // we must re-save to get base changes.
	SetTitle();
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Window Options -----------------------------------------------------

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowClear(const UI_EVENT &event)
{
	// Check for an edit window.
	UI_WINDOW_OBJECT *editWindow = GetEditWindow();
	if (!editWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of existing window.
	UI_EVENT cEvent(OPT_RESOURCE_CLEAR);
	cEvent.data = editWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the first edit window.
	{
		ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &editFile);
		editWindow->Store(editWindow->StringID(), editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear the first edit window.
	*windowManager - editWindow;
	delete editWindow;
	StatusUpdate(GetEditWindow(), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowClearAll(const UI_EVENT &event)
{
	// Check for an edit window.
	UI_WINDOW_OBJECT *editWindow = GetEditWindow();
	if (!editWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of edit windows.
	UI_EVENT cEvent(OPT_RESOURCE_CLEARALL);
	cEvent.data = editWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the visible edit windows.
	{
		ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &editFile);
		for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
			if (FlagSet(window->woStatus, WOS_EDIT_MODE))
				window->Store(window->StringID(), editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear all the edit windows.
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; )
		if (FlagSet(window->woStatus, WOS_EDIT_MODE))
		{
			UI_WINDOW_OBJECT *editWindow = window->Next();
			*windowManager - window;
			delete window;
			window = editWindow;
		}
		else
			window = window->Next();
	StatusUpdate(ZIL_NULLP(UI_WINDOW_OBJECT), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowCreate(const UI_EVENT &event)
{
	int width = 50;
	int height = 7;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;
	UIW_WINDOW *editWindow = new ZAF_WINDOW(left, top, width, height);

	*windowManager + editWindow;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowDelete(const UI_EVENT &event)
{
	// Delete the specified window resources.
	if (!event.data) // end service response has no data.
		return (event.type);
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &editFile))
		return (directoryService->Event(S_ERROR));

	// Delete the window.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	editFile->ChDir(_windowDirectory);	// remove the window.
	if (editFile->DestroyObject(pathName))
		return (storageService->Event(S_ERROR));
	editFile->ChDir(_cppDirectory); // remove the cpp entries.
	ZAF_STORAGE_SERVICE::RRmDir(editFile, pathName);
	editFile->ChDir(_hppDirectory); // remove the hpp entries.
	editFile->DestroyObject(pathName);
	editFile->storageError = 0;
	editFile->ChDir(_windowDirectory);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowLoad(const UI_EVENT &event)
{
	// Make sure there is an edit file.
	if (!event.data) // end service response has no data.
		return (event.type);
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &editFile))
		return (directoryService->Event(S_ERROR));

	// Load the window.
	static UIW_WINDOW *editWindow = ZIL_NULLP(UIW_WINDOW);
	if (!editWindow)
	{
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		editWindow = new ZAF_WINDOW(pathName, editFile,
			ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
			ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);
		eventManager->Put(event); // Wait till provider window is removed from screen.
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		return (event.type);
	}
	else
	{
		*windowManager + editWindow;
		ZAF_SERVICE_MANAGER::_changedData = TRUE;
		editWindow = ZIL_NULLP(UIW_WINDOW);
	}
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowRequest(const UI_EVENT &event)
{
	// Make sure there is an edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &editFile))
		return (directoryService->Event(S_ERROR));

	// Make a request to the resource service.
	if (editFile->ChDir(_windowDirectory))
	{
		editFile->MkDir(_windowDirectory);
		editFile->ChDir(_windowDirectory);
	}
	int traverseLock = TRUE;
	storageService->Information(I_SET_TRAVERSE_LOCK, &traverseLock);
	storageService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	storageService->Information(I_SET_REQUEST, &ccode);
	storageService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowStore(const UI_EVENT &)
{
	// Make sure there is an edit window.
	UI_WINDOW_OBJECT *editWindow = GetEditWindow();
	if (!editWindow)
		return (storageService->Event(S_ERROR));

	// Store the message table.
	UI_EVENT storeEvent(OPT_RESOURCE_STOREAS);
	storeEvent.data = editWindow->StringID();
	return (WindowStoreAs(storeEvent));
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowStoreAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Make sure there is an edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &editFile))
		return (directoryService->Event(S_ERROR));

	// Make sure there is an edit window.
	UI_WINDOW_OBJECT *editWindow = GetEditWindow();
	if (!editWindow)
		return (storageService->Event(S_ERROR));

	// Store the window.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	editWindow->Store((ZIL_ICHAR *)event.data, editFile);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowTest(const UI_EVENT &event)
{
	ZIL_STORAGE_READ_ONLY *tDefaultStorage;
	UI_LIST tWindowList;

	// Check for a read-only drive.
	static ZIL_ICHAR name[] = { 't','e','s','t',0 };
	ZIL_STORAGE testFile(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY);
	if (testFile.storageError)
	{
		ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_ERROR_DRIVE);
		errorSystem->ReportError(windowManager, WOS_NO_STATUS, message);
		return (S_ERROR);
	}

	// Check for valid test windows.
	UI_WINDOW_OBJECT *window = NULL;
	for (window = windowManager->First(); window && !ZAF_SERVICE_MANAGER::_testMode; window = window->Next())
		if (FlagSet(window->woStatus, WOS_EDIT_MODE))
			ZAF_SERVICE_MANAGER::_testMode = TRUE;
	if (!ZAF_SERVICE_MANAGER::_testMode)
		return (S_ERROR);

	// Save the screenID, help system and default storage.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	tDefaultStorage = defaultStorage;

	windowManager->screenID = ID_SCREEN;
	storageService->Information(I_GET_STORAGE, &defaultStorage);

	// Set up the test screen.
	for (window = windowManager->Last(); window; window = windowManager->Last())
	{
		if (FlagSet(window->woStatus, WOS_EDIT_MODE))
			window->Store(window->StringID(), &testFile);
		*windowManager - window;
		tWindowList + window;
	}

	// Add the test objects.
	UI_ITEM *saveObjectTable = UI_WINDOW_OBJECT::objectTable;
	UI_ITEM *saveUserTable = UI_WINDOW_OBJECT::userTable;
	UI_WINDOW_OBJECT::objectTable = ZAF_WINDOW::_objectTable;
	UI_WINDOW_OBJECT::userTable = ZAF_WINDOW::_userTable;
	*windowManager + new UIW_WINDOW(_testName, _storage, ZIL_NULLP(ZIL_STORAGE_OBJECT), _objectTable, _userTable);
	for (window = (UI_WINDOW_OBJECT *)tWindowList.First(); window; window = window->Next())
		if (FlagSet(window->woStatus, WOS_EDIT_MODE))
		{
			UIW_WINDOW *testWindow = new UIW_WINDOW(window->StringID(), &testFile, ZIL_NULLP(ZIL_STORAGE_OBJECT), ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);
			testWindow->woAdvancedFlags &= ~(WOAF_MODAL | WOAF_NO_DESTROY);
			if (FlagSet(testWindow->woStatus, WOS_READ_ERROR))
				delete testWindow;
			else
				*windowManager + testWindow;
		}
	eventManager->DeviceState(E_MOUSE, DM_VIEW);

	// DOS makes windows active in the window subtraction loop above
	// and so the other windows helpSystems become active.
	helpSystem->ResetStorage(defaultStorage, FALSE);

	// Get the user input.
	EVENT_TYPE ccode;
	UI_EVENT tEvent;
	do
	{
		eventManager->Get(tEvent);
		ccode = tEvent.type;
		if (ccode != L_EXIT_FUNCTION && ccode != L_EXIT)
			ccode = windowManager->Event(tEvent);
	} while (tEvent.type != OPT_RESOURCE_TEST);
	ZAF_SERVICE_MANAGER::_testMode = FALSE;

	// Restore the test screen.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (window = windowManager->First(); window; window = windowManager->First())
	{
		*windowManager - window;
		// Don't delete system generated windows (help, error).
		if (!FlagSet(window->woAdvancedFlags, WOAF_NO_DESTROY))
			delete window;
	}
	for (window = (UI_WINDOW_OBJECT *)tWindowList.First(); window; window = (UI_WINDOW_OBJECT *)tWindowList.First())
	{
		tWindowList - window;
		*windowManager + window;
	}
	UI_WINDOW_OBJECT::objectTable = saveObjectTable;
	UI_WINDOW_OBJECT::userTable = saveUserTable;
	eventManager->DeviceState(E_MOUSE, DM_VIEW);

	// Restore the screenID, help system and default storage.
	windowManager->screenID = screenID;
	defaultStorage = tDefaultStorage;
	helpSystem->ResetStorage(_storage, FALSE);

	return (event.type);
}

// ----- Static tables ------------------------------------------------------

ZIL_STORAGE_READ_ONLY *ZAF_WINDOW_EDITOR::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

ZIL_ICHAR ZAF_WINDOW_EDITOR::_className[] = { 'Z','A','F','_','W','I','N','D','O','W','_','E','D','I','T','O','R',0 };
ZIL_ICHAR ZAF_WINDOW_EDITOR::_nameTableName[] = { 'O','B','J','_','T','A','B','L','E',0 };
ZIL_ICHAR ZAF_WINDOW_EDITOR::_txtExtension[] = { '.','t','x','t',0 };
ZIL_STORAGE *ZAF_WINDOW_EDITOR::_deltaFile = ZIL_NULLP(ZIL_STORAGE);

static ZIL_ICHAR _CheckCoordinates[] = { 'C','h','e','c','k','C','o','o','r','d','i','n','a','t','e','s',0 };
static ZIL_ICHAR _CheckFlagSelection[] = { 'C','h','e','c','k','F','l','a','g','S','e','l','e','c','t','i','o','n', 0 };
static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(ZAF_OBJECT_EDITOR::CheckCoordinates), _CheckCoordinates, 0 },
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(ZAF_OBJECT_EDITOR::CheckFlagSelection), _CheckFlagSelection, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_WINDOW_EDITOR::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BIGNUM, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BIGNUM::New), UIW_BIGNUM::_className, 0 },
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_COMBO_BOX, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_COMBO_BOX::New), UIW_COMBO_BOX::_className, 0 },
	{ ID_DATE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_DATE::New), UIW_DATE::_className, 0 },
	{ ID_GROUP, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_GROUP::New), UIW_GROUP::_className, 0 },
	{ ID_ICON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_ICON::New), UIW_ICON::_className, 0 },
	{ ID_INTEGER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_INTEGER::New), UIW_INTEGER::_className, 0 },
	{ ID_MAXIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MAXIMIZE_BUTTON::New), UIW_MAXIMIZE_BUTTON::_className, 0 },
	{ ID_MINIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MINIMIZE_BUTTON::New), UIW_MINIMIZE_BUTTON::_className, 0 },
	{ ID_POP_UP_ITEM, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_ITEM::New), UIW_POP_UP_ITEM::_className, 0 },
	{ ID_POP_UP_MENU, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_MENU::New), UIW_POP_UP_MENU::_className, 0 },
	{ ID_PROMPT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PROMPT::New), UIW_PROMPT::_className, 0 },
	{ ID_PULL_DOWN_ITEM, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_ITEM::New), UIW_PULL_DOWN_ITEM::_className, 0 },
	{ ID_PULL_DOWN_MENU, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_MENU::New), UIW_PULL_DOWN_MENU::_className, 0 },
	{ ID_REAL, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_REAL::New), UIW_REAL::_className, 0 },
	{ ID_SCROLL_BAR, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SCROLL_BAR::New), UIW_SCROLL_BAR::_className, 0 },
	{ ID_STATUS_BAR, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STATUS_BAR::New), UIW_STATUS_BAR::_className, 0 },
	{ ID_STRING, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TEXT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TEXT::New), UIW_TEXT::_className, 0 },
	{ ID_TIME, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TIME::New), UIW_TIME::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_VT_LIST, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_VT_LIST::New), UIW_VT_LIST::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_WINDOW_EDITOR::_objectTable = __objectTable;

#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_WINDOW_EDITOR::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_WINDOW_EDITOR::PROCESS ZAF_WINDOW_EDITOR::_processTable[] =
{
	{ OPT_FILE_CLOSE,				ZIL_PROCESS_REFERENCE(FileClose) },		// file requests
	{ OPT_FILE_DELETE, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_PREFERENCES, 		ZIL_PROCESS_REFERENCE(FilePreferences) },
	{ OPT_FILE_SAVE, 				ZIL_PROCESS_REFERENCE(FileSave) },
	{ OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileSaveAs) },
	{ -OPT_FILE_DELETE, 			ZIL_PROCESS_REFERENCE(FileDelete) },		// file responses
	{ -OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileNew) },
	{ -OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileOpen) },
	{ -OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileSaveAs) },

	{ OPT_RESOURCE_CLEAR,			ZIL_PROCESS_REFERENCE(WindowClear) },		// resource requests
	{ OPT_RESOURCE_CLEARALL,		ZIL_PROCESS_REFERENCE(WindowClearAll) },
	{ OPT_RESOURCE_CREATE,			ZIL_PROCESS_REFERENCE(WindowCreate) },
	{ OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(WindowRequest) },
	{ OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(WindowRequest) },
	{ OPT_RESOURCE_STORE,			ZIL_PROCESS_REFERENCE(WindowStore) },
	{ OPT_RESOURCE_STOREAS,			ZIL_PROCESS_REFERENCE(WindowRequest) },
	{ OPT_RESOURCE_TEST,			ZIL_PROCESS_REFERENCE(WindowTest) },
	{ -OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(WindowDelete) },		// resource responses
	{ -OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(WindowImport) },
	{ -OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(WindowLoad) },
	{ -OPT_RESOURCE_STOREAS,		ZIL_PROCESS_REFERENCE(WindowStoreAs) },

	{ OPT_EDIT_COPY, 				ZIL_PROCESS_REFERENCE(EditRequest) },		// edit requests
	{ OPT_EDIT_CUT, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_DELETE, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_GROUP_DEFINE, 		ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_GROUP_UNDEFINE, 		ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_MOVE, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_OBJECT, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_PASTE, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_SIZE, 				ZIL_PROCESS_REFERENCE(EditRequest) },

	{ OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(WindowExport) },
	{ -OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(WindowExport) },
	{ OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(WindowExport) },
	{ -OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(WindowExport) },
	{ OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(WindowImport) },
	{ -OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(WindowImport) },
	{ OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(WindowImport) },
	{ -OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(WindowImport) },

	{ OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(WindowExport) },

	{ 0, 0 }
};
