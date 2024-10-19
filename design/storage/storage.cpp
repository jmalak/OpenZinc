//	Zinc Interface Library - STORAGE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

extern "C"
{
#include <stdio.h>
}
#if defined(__MWERKS__)
#	include <unix.h>								// For _fcreator.
#endif
#include "storage.hpp"
#define USE_MSG_TABLE
#define USE_ZAF_STORAGE_SERVICE
#include "p_store.hpp"

const int ZMSG_TITLES				= 6300 - 0;		// Titles start at 0.
const int ZMSG_ERRORS				= 6300 - 100;	// Errors start at 100.
const int ZMSG_CONFIRMATIONS		= 6300 - 200;	// Confirmations start at 300.

const EVENT_TYPE OPT_RESET_PARENT		= 7000;
const EVENT_TYPE OPT_RESET_CHILD		= 7001;
const EVENT_TYPE OPT_RESET_OBJECT		= 7002;

const unsigned STORAGE_UNKNOWN			= 7000;
const unsigned STORAGE_DIRECTORY		= 7001;
const unsigned STORAGE_OBJECT			= 7002;

#define ID_DERIVED_START 3500

extern ZIL_ICHAR *_blankString;
extern ZIL_ICHAR *_parentDirectory;
extern ZIL_ICHAR *_currentDirectory;
extern ZIL_ICHAR *_zilExtName;
extern ZIL_ICHAR *_cppDirectory;
extern ZIL_ICHAR *_hppDirectory;
extern ZIL_ICHAR *_rootStorageDirectory;
extern ZIL_ICHAR *_objectIDName;
extern ZIL_ICHAR *_userFunctionName;
extern ZIL_ICHAR *_compareFunctionName;

static ZIL_ICHAR _cppExtension[] = { '.','c','p','p',0 };
static ZIL_ICHAR _hppExtension[] = { '.','h','p','p',0 };
static ZIL_ICHAR _deriveTableName[] = { 'D','E','R','I','V','E','_','T','A','B','L','E', 0 };
static ZIL_ICHAR _derivedObjectsName[] = { 'D','E','R','I','V','E','D','_','O','B','J','E','C','T','S', 0 };

// ----- Overloaded 8-bit functions -----------------------------------------

#if defined(ZIL_UNICODE)
static FILE *fopen(ZIL_ICHAR *uname, char *extension)
{
	ZIL_FILE_CHAR *name = ZIL_INTERNATIONAL::ConvertToFilename(uname);
	FILE *file = ::fopen(name, extension);
	delete name;
	return (file);
}
static int fprintf(FILE *stream, const char *format, const ZIL_ICHAR *us1, const ZIL_ICHAR *us2 = 0, const ZIL_ICHAR *us3 = 0)
{
	char s1[256], s2[256], s3[256];
	s1[0] = s2[0] = s3[0] = 0;
	if (us1) ZIL_INTERNATIONAL::MapText(us1, s1);
	if (us2) ZIL_INTERNATIONAL::MapText(us2, s2);
	if (us3) ZIL_INTERNATIONAL::MapText(us3, s3);
	return (::fprintf(stream, format, s1, s2, s3));
}
#endif

// --------------------------------------------------------------------------
// ----- ZAF_STORAGE_ITEM ---------------------------------------------------
// --------------------------------------------------------------------------

class ZAF_STORAGE_ITEM : public UIW_BUTTON
{
	friend class ZAF_STORAGE_SERVICE;
public:
	ZAF_STORAGE_ITEM(int indentation, ZIL_ICHAR *pathName, ZIL_ICHAR *type, WOF_FLAGS woFlags);
	~ZAF_STORAGE_ITEM(void);

	struct ZAF_ITEM_SET
	{
		ZIL_ICHAR *name;
		int bitmapWidth;
		int bitmapHeight;
		ZIL_UINT8 *bitmapArray;
		ZIL_BITMAP_HANDLE colorBitmap;
		ZIL_BITMAP_HANDLE monoBitmap;
		ZIL_USER_FUNCTION userFunction;
	};

private:
	static ZAF_ITEM_SET _item[];
	static EVENT_TYPE ChildDirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ObjectCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ParentDirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

static ZIL_ICHAR _parentItem[] = { 'p','a','r','e','n','t','D','i','r','e','c','t','o','r','y', 0 };
static ZIL_ICHAR _currentItem[] = { 'c','u','r','r','e','n','t','D','i','r','e','c','t','o','r','y', 0 };
static ZIL_ICHAR _childItem[] = { 'c','h','i','l','d','D','i','r','e','c','t','o','r','y', 0 };
static ZIL_ICHAR _objectItem[] = { 'o','b','j','e','c','t', 0 };
static ZIL_ICHAR _windowItem[] = { 'U','I','W','_','W','I','N','D','O','W', 0 };
static ZIL_ICHAR _bitmapItem[] = { 'U','I','_','B','I','T','M','A','P', 0 };
static ZIL_ICHAR _iconItem[] = { 'U','I','_','I','C','O','N', 0 };
static ZIL_ICHAR _mouseItem[] = { 'U','I','_','M','O','U','S','E', 0 };
static ZIL_ICHAR _helpItem[] = { 'U','I','_','H','E','L','P', 0 };
static ZIL_ICHAR _messageItem[] = { 'U','I','_','M','E','S','S','A','G','E', 0 };

ZAF_STORAGE_ITEM::ZAF_ITEM_SET ZAF_STORAGE_ITEM::_item[] =
{
	{ _parentItem, 	0, 	0, 	0, 	0, 	0, ParentDirectoryCallback },
	{ _childItem, 	0, 	0, 	0, 	0, 	0, ChildDirectoryCallback},
	{ _currentItem,	0, 	0, 	0, 	0, 	0, 0 },
	{ _windowItem,	0, 	0, 	0, 	0, 	0, ObjectCallback },
	{ _bitmapItem,	0, 	0, 	0, 	0, 	0, ObjectCallback },
	{ _mouseItem,	0, 	0, 	0, 	0, 	0, ObjectCallback },
	{ _iconItem,	0, 	0, 	0, 	0, 	0, ObjectCallback },
	{ _helpItem,	0, 	0, 	0, 	0, 	0, ObjectCallback },
	{ _messageItem,	0, 	0, 	0, 	0, 	0, ObjectCallback },
	{ _objectItem, 	0, 	0, 	0, 	0, 	0, ObjectCallback },
	{ 0, 			0, 	0, 	0, 	0, 	0, 0 }
};

ZAF_STORAGE_ITEM::ZAF_STORAGE_ITEM(int indentation, ZIL_ICHAR *pathName, ZIL_ICHAR *type, WOF_FLAGS _woFlags) :
	UIW_BUTTON(indentation, 0, 50, pathName,
	BTF_NO_3D | BTF_DOUBLE_CLICK | BTF_STATIC_BITMAPARRAY, _woFlags)
{
	static int initialized = FALSE;
	if (!initialized)
	{
		extern ZIL_ICHAR *_bitmapDirectory;
		ZIL_STORAGE_READ_ONLY *directory = ZAF_STORAGE_SERVICE::_storage;
		directory->ChDir(_bitmapDirectory);
		for (int i = 0; _item[i].name; i++)
		{
			ZIL_STORAGE_OBJECT_READ_ONLY bFile(*directory, _item[i].name, ID_BITMAP_IMAGE);
			ZIL_INT16 _value;
			bFile.Load(&_value); _item[i].bitmapWidth = _value;
			bFile.Load(&_value); _item[i].bitmapHeight = _value;
			_item[i].bitmapArray = new ZIL_UINT8[_item[i].bitmapWidth * _item[i].bitmapHeight];
			bFile.Load(_item[i].bitmapArray, sizeof(ZIL_UINT8), _item[i].bitmapWidth * _item[i].bitmapHeight);
			// Convert the bitmap array to a handle if possible.
#			if !defined(ZIL_OS2) && !defined(ZIL_MOTIF) && !defined(ZIL_NEXTSTEP)
			display->BitmapArrayToHandle(screenID,
				_item[i].bitmapWidth, _item[i].bitmapHeight, _item[i].bitmapArray,
				ZIL_NULLP(UI_PALETTE), &_item[i].colorBitmap, &_item[i].monoBitmap);
			if (_item[i].colorBitmap)
			{
				delete _item[i].bitmapArray;
				_item[i].bitmapArray = ZIL_NULLP(ZIL_UINT8);
			}
#			endif
		}
		initialized = TRUE;
	}

	// Find the matching item.
	for (int i = 0; _item[i].name; i++)
		if (!stricmp(_item[i].name, type) || 	// the item matches.
			!_item[i+1].name)					// default to persist.
		{
			bitmapWidth = _item[i].bitmapWidth;
			bitmapHeight = _item[i].bitmapHeight;
			bitmapArray = _item[i].bitmapArray;
			colorBitmap = _item[i].colorBitmap;
			monoBitmap = _item[i].monoBitmap;
			userFunction = _item[i].userFunction;
			break;
		}
}

ZAF_STORAGE_ITEM::~ZAF_STORAGE_ITEM(void)
{
	colorBitmap = monoBitmap = 0;
}

EVENT_TYPE ZAF_STORAGE_ITEM::ChildDirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		event.type = OPT_RESET_CHILD;
		event.windowObject = object;
		object->eventManager->Put(event);
	}
	return (ccode);
}

EVENT_TYPE ZAF_STORAGE_ITEM::ObjectCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || ccode == L_SELECT)
	{
		event.type = OPT_RESET_OBJECT;
		event.data = object->Information(I_GET_TEXT, ZIL_NULLP(void));
		object->eventManager->Put(event);

		if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
		{
			event.type = OPT_OK;
			object->eventManager->Put(event);
		}
	}
	return (ccode);
}

EVENT_TYPE ZAF_STORAGE_ITEM::ParentDirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		event.type = OPT_RESET_PARENT;
		event.windowObject = object;
		object->eventManager->Put(event);
	}
	return (ccode);
}

// --------------------------------------------------------------------------
// ----- ZAF_STORAGE_SERVICE ------------------------------------------------
// --------------------------------------------------------------------------

ZAF_STORAGE_SERVICE::ZAF_STORAGE_SERVICE(int argc, ZIL_ICHAR **argv) :
	// base initialization
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_store"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable),
	// member initialization
	revisions(0), traverseLock(FALSE), writeCPP(TRUE), writeHPP(TRUE), writeDelta(FALSE),
	response(0), requestFile(ZIL_NULLP(ZIL_STORAGE)),
	requestor(ZIL_NULLP(UI_WINDOW_OBJECT)),
	msgTable(_serviceManager->ZMSG_msgTableName(), _serviceManager->Storage(&_storage, "p_store"))
{
	// Set the searchID and default variables.
	searchID = ID_STORAGE_SERVICE;
	strcpy(defaultExtension, _zilExtName);

	// Position the window on the screen.
	dirList = (UIW_VT_LIST *)Get(LIST_DIRECTORIES);
	objList = (UIW_VT_LIST *)Get(LIST_OBJECTS);

	Subtract(dirList);
	UI_REGION region = dirList->trueRegion;
	int listWidth = region.Width();
	int listHeight = region.Height();
	region.right += 1;
	region.bottom += 1;
	UIW_GROUP *group = new UIW_GROUP(region.left, region.top, region.right - region.left, region.bottom - region.top, "");
	region.right -= 1;
	region.bottom -= 1;
	dirList->trueRegion.left = dirList->relative.left = dirList->trueRegion.top = dirList->relative.top = 0;
	dirList->trueRegion.right = dirList->relative.right = listWidth - 2;
	dirList->trueRegion.bottom = dirList->relative.bottom = listHeight - 2;
	*group + dirList;
	Add(group);
	
	Subtract(objList);
	region = objList->trueRegion;
	listWidth = region.Width();
	listHeight = region.Height();
	region.right += 1;
	region.bottom += 1;
	group = new UIW_GROUP(region.left, region.top, region.right - region.left, region.bottom - region.top, "");
	region.right -= 1;
	region.bottom -= 1;
	objList->trueRegion.left = objList->relative.left = objList->trueRegion.top = objList->relative.top = 0;
	objList->trueRegion.right = objList->relative.right = listWidth - 2;
	objList->trueRegion.bottom = objList->relative.bottom = listHeight - 2;
	*group + objList;
	Add(group);

	if (windowManager)
		windowManager->Center(this);

	// Set up the initial path and open the default request file.
	strcpy(currentPath, _rootStorageDirectory);
	if (argc > 1)
	{
		UI_EVENT event(-OPT_FILE_OPEN);
		event.data = argv[1];
		Event(event);
		Get(FIELD_DIRECTNAME)->Information(I_SET_TEXT, currentPath);
		if (requestFile)
			ListUpdate();
	}
	if (!requestFile)
		Information(I_SET_STORAGE, ZIL_NULLP(ZIL_STORAGE));
}

ZAF_STORAGE_SERVICE::~ZAF_STORAGE_SERVICE(void)
{
	// Remove the resource storage.
	if (requestFile)
		delete requestFile;
	if (_storage)
		delete _storage;

	// Free the color and monochrome bitmap handles.
	ZAF_STORAGE_ITEM::ZAF_ITEM_SET *_item = ZAF_STORAGE_ITEM::_item;
	for (int i = 0; _item[i].name; i++)
	{
		if (_item[i].bitmapArray)
			delete _item[i].bitmapArray;

		display->DestroyBitmapHandle(screenID, &_item[i].colorBitmap, &_item[i].monoBitmap);
	}
}

int ZAF_STORAGE_SERVICE::DirectoryCompare(void *object1, void *object2)
{
	int result =
		(int)((UI_WINDOW_OBJECT *)object1)->userFlags -
		(int)((UI_WINDOW_OBJECT *)object2)->userFlags;
	return (result ? result : UIW_WINDOW::StringCompare(object1, object2));
}

EVENT_TYPE ZAF_STORAGE_SERVICE::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_OK:
		{
		// Check for object validity.
		ZIL_ICHAR *name; Get(FIELD_OBJECTNAME)->Information(I_GET_TEXT, &name);
		if (!name || !name[0])
		{
			ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_BLANK_NAME);
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, message);
			break;
		}
		if (!requestor ||
			(requestor->SearchID() != ID_IMAGE_EDITOR &&
			 requestor->SearchID() != ID_IMAGE_WINDOW &&
			 requestor->SearchID() != ID_I18N_EDITOR &&
			 requestor->SearchID() != ID_I18N_WINDOW)) // TEMPORARY
			strupr(name); // convert non-icon names to upper-case.
		int objectFound = requestFile->FindFirstObject(name) ? TRUE : FALSE;

		// Check for import selection.
		if (requestor && (response == OPT_RESOURCE_IMPORT || response == OPT_RESOURCE_EXPORT || response == OPT_RESOURCE_DELETE))
		{
			// Send a response to the requestor for each selected item.
			eventManager->DeviceState(E_MOUSE, DM_WAIT);
			UI_EVENT rEvent(-response);
			for (UI_WINDOW_OBJECT *object = objList->First(); object; object = object->Next())
				if (FlagSet(object->woStatus, WOS_SELECTED))
				{
					object->Information(I_GET_TEXT, &name);
					Get(FIELD_OBJECTNAME)->Information(I_SET_TEXT, name);
					// Confirm deletion processes (first time only).
					rEvent.data = name;
					if (response == OPT_RESOURCE_DELETE && !rEvent.data)
					{
						ZIL_ICHAR *title = msgTable.GetMessage(ZMSG_RES_TDELETE);
						ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_RES_DELETE_MULTIPLE);
						ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
							ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, name);
						UI_ERROR_STUB::Beep();
						if (window.Control() != ZIL_DLG_YES)
							continue; // Do not overwrite the file.
					}
					requestor->Event(rEvent);
				}

			// Resource specified only in object-name field.
			if (!rEvent.data && !objectFound)
				return (ZAF_STORAGE_SERVICE::Event(S_ERROR));
			else if (!rEvent.data && objectFound)
			{
				rEvent.data = name;
				if (response == OPT_RESOURCE_DELETE)
				{
					ZIL_ICHAR *title = msgTable.GetMessage(ZMSG_RES_TDELETE);
					ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_RES_DELETE);
					ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
						ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, name);
					UI_ERROR_STUB::Beep();
					if (window.Control() == ZIL_DLG_YES)
						ccode = requestor->Event(rEvent);
				}
				else
					ccode = requestor->Event(rEvent);
			}

			// Send request to requestor.
			rEvent.data = ZIL_NULLP(void);
			requestor->Event(rEvent); // NULL response tells requestor we're done.
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
			if (ccode != S_ERROR)
			{
				eventManager->Put(S_CLOSE, Q_BEGIN);
				requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
			}
			break;
		}

		// Process all other requests.
		if (objectFound && (response == OPT_RESOURCE_CREATE || response == OPT_RESOURCE_STOREAS || response == OPT_RESOURCE_EXPORT))
		{
			// Query for user overwrite of object.
			ZIL_ICHAR *title = response ?
				msgTable.GetMessage((int)(response - ZMSG_TITLES)) :
				msgTable.GetMessage(ZMSG_RES_TNONE);
			ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_RES_OVERWRITE);
			ZIL_ICHAR *objectName; Get(FIELD_OBJECTNAME)->Information(I_GET_TEXT, &objectName);
			ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
				ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, objectName);
			UI_ERROR_STUB::Beep();
			if (window.Control() != ZIL_DLG_YES)
				break; // Do not overwrite the file.
		}

		// Process object-name interactions.
		if (!objectFound && response != OPT_RESOURCE_CREATE && response != OPT_RESOURCE_STOREAS)
			return (ZAF_STORAGE_SERVICE::Event(S_ERROR));
		else if (requestor)
		{
			eventManager->DeviceState(E_MOUSE, DM_WAIT);
			UI_EVENT rEvent(-response);
			rEvent.data = name;
			ccode = requestor->Event(rEvent); // Send a response to the requestor.
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
			if (ccode == S_ERROR)
				break; // Error message is handled by requestor.
		}
		}
		// Continue to OPT_CANCEL.
	case OPT_CANCEL:
		// Remove the window from the window manager
		if (requestor)
		{
			UI_EVENT rEvent(-response);
			requestor->Event(rEvent); // NULL data response tells requestor we're done.
		}
		eventManager->Put(S_CLOSE, Q_BEGIN);
		requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
		response = 0;
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case OPT_RESET_PARENT:
	case OPT_RESET_CHILD:
		// Reset the parent or child directory information.
		if (ccode == OPT_RESET_PARENT)
		{
			UI_WINDOW_OBJECT *object = event.windowObject;
			while (object->Previous())
				object = object->Previous();

			strcpy(currentPath, _rootStorageDirectory);
			while (object != event.windowObject)
			{
				object = object->Next();

				ZIL_ICHAR *currentDirectory;
				object->Information(I_GET_TEXT, &currentDirectory);
				strcat(currentPath, currentDirectory);

				if (object != event.windowObject)
					strcat(currentPath, _rootStorageDirectory);
			}
		}
		else
		{
			if (strcmp(currentPath, _rootStorageDirectory))
				strcat(currentPath, _rootStorageDirectory);

			ZIL_ICHAR *currentDirectory;
			event.windowObject->Information(I_GET_TEXT, &currentDirectory);
			strcat(currentPath, currentDirectory);
		}

		// Re-display the directory and object information.
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		requestFile->ChDir(currentPath);
		Get(FIELD_DIRECTNAME)->Information(I_SET_TEXT, currentPath);

		ListUpdate();
		dirList->Event(S_REDISPLAY);
		objList->Event(S_REDISPLAY);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case OPT_RESET_OBJECT:
		// Reset the object.
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		Get(FIELD_OBJECTNAME)->Information(I_SET_TEXT, event.data);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case NEG_OPT_FILE_OPEN:
		// Try to open the specified file.
		if (event.data)
		{
			ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
			strcpy(pathName, (ZIL_ICHAR *)event.data);
			if (!strchr(pathName, '.'))
				strcat(pathName, defaultExtension);
			ZIL_STORAGE *openFile = new ZIL_STORAGE(pathName, UIS_READWRITE);
			if (openFile->storageError)
			{
				delete openFile;
				errorSystem->ReportError(windowManager, WOS_NO_STATUS,
					msgTable.GetMessage(ZMSG_FILE_OPEN), pathName);
				return (S_ERROR);
			}
			else if (requestFile)
				delete requestFile;
			requestFile = openFile;
		}
		break;

	// ----- Special confirmation requests from applications. -----
	case OPT_RESOURCE_CLEAR:
	case OPT_RESOURCE_CLEARALL:
		{
		ZAF_MESSAGE_WINDOW confirm(msgTable.GetMessage((int)(response - ZMSG_TITLES)),
			ZIL_NULLP(ZIL_ICHAR), ZIL_MSG_YES | ZIL_MSG_NO | ZIL_MSG_CANCEL,
			ZIL_MSG_YES, msgTable.GetMessage((int)(ccode - ZMSG_CONFIRMATIONS)),
			event.data);
		UI_ERROR_STUB::Beep();
		return (confirm.Control());
		}

	case NEG_OPT_RESOURCE_IMPORT:
	case NEG_OPT_RESOURCE_EXPORT:
		{
		ZIL_ICHAR *title = msgTable.GetMessage((int)(0 - ccode - ZMSG_TITLES));
		ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_RES_OVERWRITE);
		ZIL_ICHAR *name = (ZIL_ICHAR *)event.data;
		ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
			ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, name);
		UI_ERROR_STUB::Beep();
		return ((window.Control() == ZIL_MSG_YES) ? ccode : 0);
		}

	// ----- Special error requests from applications. -----
	case S_ERROR:
		// Report an invalid storage error.
		if (errorSystem)
		{
			// Error messages are indexed by option #.
			ZIL_ICHAR *name; Get(FIELD_OBJECTNAME)->Information(I_GET_TEXT, &name);
			ZIL_ICHAR *message = response ?
				msgTable.GetMessage((int)(response - ZMSG_ERRORS)) :
				msgTable.GetMessage(ZMSG_RES_NONE_ERROR);
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, message, name);
		}
		else
			UI_ERROR_STUB::Beep();
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(_storage, FALSE);
		break;

	default:
		// Default to the base window event.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	return (ccode);
}

int ZAF_STORAGE_SERVICE::GenerateCPP(void)
{
#if defined(__MWERKS__)
	// File creator for fopen().
	_fcreator = 'Anon';
#endif

	static ZIL_ICHAR _ui[] = { 'U','I','_',0 };
	static ZIL_ICHAR _uiw[] = { 'U','I','W','_',0 };

	// Get the title and format information.
	if (!writeCPP || !requestFile)
		return (FALSE);
	ZIL_ICHAR *scppTitle = msgTable.GetMessage(ZMSG_SCPP_TITLE);
	ZIL_ICHAR *scppFormat = msgTable.GetMessage(ZMSG_SCPP_ENTRY);

	// Create the ascii CPP file.
	ZIL_ICHAR *entry;
	ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
	requestFile->StorageName(pathName);
	requestFile->ChangeExtension(pathName, _cppExtension);
	FILE *file = fopen(pathName, "w+");
	_serviceManager->StatusUpdate(scppTitle);

	// ----- Write the pre-file information. -----
	ZIL_ICHAR fileName[ZIL_MAXPATHLEN];
	fprintf(file, "#include <ui_win.hpp>\n");
	fprintf(file, "#define USE_DERIVED_OBJECTS\n", fileName);
	requestFile->ChangeExtension(pathName, _hppExtension);
	requestFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), fileName);
	int i;
	for (i = 0; fileName[i]; i++)
		fileName[i] = ToLower(fileName[i]);
	fprintf(file, "#include \"%s\"\n", fileName);
	fprintf(file, "void z_jump_dummy(void) { }   // Bug fix for broken linkers.\n\n");

	// ----- Write the user/compare information. -----
	const int MAX_ENTRIES = 100;
	ZIL_ICHAR **userEntry = ZIL_NULLP(ZIL_ICHAR *);
	ZIL_ICHAR **compareEntry = ZIL_NULLP(ZIL_ICHAR *);
	int totalUserEntries = 0, totalCompareEntries = 0;
	requestFile->ChDir(_cppDirectory);
	for (entry = requestFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); entry; entry = requestFile->FindNextObject())
		if (strcmp(entry, _currentDirectory) &&
			strcmp(entry, _parentDirectory))
		{
			_serviceManager->StatusUpdate(ZIL_NULLP(ZIL_ICHAR), scppFormat, entry);
			// Look for user functions.
			requestFile->ChDir(entry);
			ZIL_ICHAR functionName[256];

			// Get the user functions.
			ZIL_STORAGE_OBJECT_READ_ONLY userFile(*requestFile, _userFunctionName, 0);
			while (userFile.Load(functionName, 128) > 0 && functionName[0])
			{
				// Try to find a matching entry.
				int match;
				for (match = 0; match < totalUserEntries; match++)
					if (!stricmp(userEntry[match], functionName))
						break;
				// Check for a full entry table.
				if (match == totalUserEntries)
				{
					if (totalUserEntries % MAX_ENTRIES == 0)
					{
						ZIL_ICHAR **_userEntry = userEntry;
						userEntry = new ZIL_ICHAR *[totalUserEntries + MAX_ENTRIES];
						for (int i = 0; i < totalUserEntries; i++)
							userEntry[i] = _userEntry[i];
						if (_userEntry)
							delete _userEntry;
					}
					userEntry[totalUserEntries++] = strdup(functionName);
				}
			}

			// Get the compare functions.
			ZIL_STORAGE_OBJECT_READ_ONLY compareFile(*requestFile, _compareFunctionName, 0);
			while (compareFile.Load(functionName, 128) > 0 && functionName[0])
			{
				// Try to find a matching entry.
				int match;
				for (match = 0; match < totalCompareEntries; match++)
					if (!stricmp(compareEntry[match], functionName))
						break;
				// Check for a full entry table.
				if (match == totalCompareEntries)
				{
					if (totalCompareEntries % MAX_ENTRIES == 0)
					{
						ZIL_ICHAR **_compareEntry = compareEntry;
						compareEntry = new ZIL_ICHAR *[totalCompareEntries + MAX_ENTRIES];
						for (int i = 0; i < totalCompareEntries; i++)
							compareEntry[i] = _compareEntry[i];
						if (_compareEntry)
							delete _compareEntry;
					}
					compareEntry[totalCompareEntries++] = strdup(functionName);
				}
			}
			requestFile->ChDir(_cppDirectory);
		}

	const int PASS_EXTERN = 1;
	const int PASS_STRING = 2;
	const int PASS_TABLE = 3;
	for (int pass = PASS_EXTERN; pass <= PASS_TABLE; pass++)
	{
		// Print the table header.
		if (pass == PASS_TABLE)
		{
			fprintf(file, "static UI_ITEM _userTable[] =\n");
			fprintf(file, "{\n");
		}
		// Print the user function entries.
		for (i = 0; i < totalUserEntries; i++)
			if (pass == PASS_EXTERN)
				fprintf(file, "extern EVENT_TYPE %s(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);\n", userEntry[i]);
			else if (pass == PASS_STRING)
			{
				fprintf(file, "static ZIL_ICHAR _%s[] = { ", userEntry[i]);
				for (ZIL_ICHAR *data = userEntry[i]; *data; data++)
					::fprintf(file, "'%c',", *data);
				fprintf(file, "0 };\n");
			}
			else // PASS_TABLE
				fprintf(file, "\t{ 0, (EVENT_TYPE*)ZIL_VOIDF(%s), _%s, 0 },\n", userEntry[i], userEntry[i]);
		// Print the compare function entries.
		for (i = 0; i < totalCompareEntries; i++)
			if (pass == PASS_EXTERN)
				fprintf(file, "extern int %s(void *, void *);\n", compareEntry[i]);
			else if (pass == PASS_STRING)
			{
				fprintf(file, "static ZIL_ICHAR _%s[] = { ", compareEntry[i]);
				for (ZIL_ICHAR *data = compareEntry[i]; *data; data++)
					::fprintf(file, "'%c',", *data);
				fprintf(file, "0 };\n");
			}
			else // PASS_TABLE
				fprintf(file, "\t{ 0, (EVENT_TYPE*)ZIL_VOIDF(%s), _%s, 0 },\n", compareEntry[i], compareEntry[i]);
		// Print the table trailer.
		if (pass == PASS_TABLE)
		{
			fprintf(file, "\t{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }\n");
			fprintf(file, "};\n");
			fprintf(file, "UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;\n\n");
		}
		else if (totalUserEntries || totalCompareEntries)
			fprintf(file, "\n");
	}
	for (i = 0; i < totalUserEntries; i++)
		delete userEntry[i];
	for (i = 0; i < totalCompareEntries; i++)
		delete compareEntry[i];

	// ----- Write the object table information. -----
	fprintf(file, "static UI_ITEM _objectTable[] =\n");
	fprintf(file, "{\n");
	requestFile->ChDir(_cppDirectory);
	// reset the used objects (using general object table).
	if (objectTable)
	{
//		for (i = 0; objectTable[i].value != ID_END && objectTable[i].value < ID_DERIVED_START; i++)
		for (i = 0; objectTable[i].value != ID_END; i++)
			objectTable[i].flags = FALSE;
		// flag newly used objects.
		for (entry = requestFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); entry; entry = requestFile->FindNextObject())
			if (entry[0] != '.')
			{
				// Look for object identifiers.
				_serviceManager->StatusUpdate(ZIL_NULLP(ZIL_ICHAR), scppFormat, entry);
				ZIL_OBJECTID objectID;
				requestFile->ChDir(entry);
				ZIL_STORAGE_OBJECT_READ_ONLY objectFile(*requestFile, _objectIDName, 0);
				do
				{
					objectID = 0;
					objectFile.Load(&objectID);
					for (int i = 0; objectTable[i].value != ID_END; i++)
//						if (objectTable[i].value == objectID && objectTable[i].value < ID_DERIVED_START)
						if (objectTable[i].value == objectID)
						{
//							objectTable[i].flags = TRUE;
							objectTable[i].flags = objectID;
							break;
						}
				} while (objectID);
				requestFile->ChDir(_cppDirectory);
			}
		// print newly used objects.
		for (i = 0; objectTable[i].value != ID_END; i++)
			if (objectTable[i].flags)
			{
				ZIL_ICHAR *text = objectTable[i].text;
				if (!strncmp(_uiw, text, 4))
					fprintf(file, "\t{ ID_%s, (UI_WINDOW_OBJECT*)ZIL_VOIDF(%s::New), %s::_className, 0 },\n", &text[4], text, text);
				else if (!strncmp(_ui, text, 3))
					fprintf(file, "\t{ ID_%s, (UI_WINDOW_OBJECT*)ZIL_VOIDF(%s::New), %s::_className, 0 },\n", &text[3], text, text);
				else
					fprintf(file, "\t{ ID_%s, (UI_WINDOW_OBJECT*)ZIL_VOIDF(%s::New), %s::_className, 0 },\n", text, text, text);
			}
	}
	fprintf(file, "\t{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }\n");
	fprintf(file, "};\n");
	fprintf(file, "UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;\n\n");

	// ----- Write the derived object table information. -----
	requestFile->ChDir(_rootStorageDirectory);
	// Store the derived object table.
	ZIL_STORAGE_OBJECT *writeFile = new ZIL_STORAGE_OBJECT(*requestFile, _deriveTableName, 0, UIS_CREATE | UIS_READWRITE);
	for (i = 0; objectTable[i].value != ID_END; i++)
		if (objectTable[i].flags && objectTable[i].value >= 3500)
		{
			writeFile->Store((ZIL_OBJECTID)objectTable[i].value);
			for (int j = 0; objectTable[j].data; j++)
				if (objectTable[j].data == objectTable[i].data)
				{
					writeFile->Store((ZIL_OBJECTID)objectTable[j].value);
					break;
				}
			writeFile->Store(objectTable[i].text);
		}
	writeFile->Store(ID_END);
	delete writeFile;
	// Store the HPP entries.
	requestFile->ChDir(_rootStorageDirectory);
	requestFile->ChDir(_hppDirectory);
	writeFile = new ZIL_STORAGE_OBJECT(*requestFile, _derivedObjectsName, 0, UIS_CREATE | UIS_READWRITE);
	for (i = 0; objectTable[i].value != ID_END; i++)
		if (objectTable[i].flags && objectTable[i].value >= 3500)
		{
			ZIL_ICHAR buffer[128];
			static ZIL_ICHAR ZIL_FARDATA _idFormat[] = { 'c','o','n','s','t',' ','Z','I','L','_','U','S','E','R','_','O','B','J','E','C','T','I','D',' ','I','D','_','%','-','2','4','s',' ','=',' ','%','0','4','d',';', 0 };
			ZIL_ICHAR *text = objectTable[i].text;
			if (!strncmp(_uiw, text, 4))
				sprintf(buffer, _idFormat, &text[4], objectTable[i].value);
			else if (!strncmp(_ui, text, 3))
				sprintf(buffer, _idFormat, &text[3], objectTable[i].value);
			else
				sprintf(buffer, _idFormat, text, objectTable[i].value);
			writeFile->Store(buffer);
		}
	writeFile->Store(_blankString);
	delete writeFile;

	// Close the file.
	_serviceManager->StatusUpdate();
	fclose (file);
	return (TRUE);
}

int ZAF_STORAGE_SERVICE::GenerateHPP(void)
{
#if defined(__MWERKS__)
	// File creator for fopen().
	_fcreator = 'Anon';
#endif

	// Get the title and format information.
	if (!writeHPP || !requestFile)
		return (FALSE);
	ZIL_ICHAR *shppTitle = msgTable.GetMessage(ZMSG_SHPP_TITLE);
	ZIL_ICHAR *shppFormat = msgTable.GetMessage(ZMSG_SHPP_ENTRY);

	// Create the ascii HPP file.
	ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
	requestFile->StorageName(pathName);
	requestFile->ChangeExtension(pathName, _hppExtension);
	FILE *file = fopen(pathName, "w+");
	_serviceManager->StatusUpdate(shppTitle);

	// Write the HPP entries for each window.
	requestFile->ChDir(_hppDirectory);
	for (ZIL_ICHAR *entry = requestFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); entry; entry = requestFile->FindNextObject())
	{
		// Write out the window entries.
		if (strcmp(entry, _currentDirectory) &&
			strcmp(entry, _parentDirectory))
		{
			_serviceManager->StatusUpdate(ZIL_NULLP(ZIL_ICHAR), shppFormat, entry);
			ZIL_ICHAR line[256];
			fprintf(file, "#if defined(USE_%s)\n", entry);
			ZIL_STORAGE_OBJECT_READ_ONLY hppEntry(*requestFile, entry, 0);
			for (hppEntry.Load(line, 256); strcmp(line, _blankString); hppEntry.Load(line, 256))
				fprintf(file, "%s\n", line);
			fprintf(file, "#endif\n\n");
		}
	}

	// Close the file.
	_serviceManager->StatusUpdate();
	fclose (file);
	return (TRUE);
}

void *ZAF_STORAGE_SERVICE::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the storage service.
		*windowManager + this;
		break;

	case I_SET_REQUEST:
		{
		// Set the request.
		response = *(EVENT_TYPE *)data;
		ZIL_ICHAR *title = response ?
			msgTable.GetMessage((int)(response - ZMSG_TITLES)) :
			msgTable.GetMessage(ZMSG_RES_TNONE);
		UIW_WINDOW::Information(I_SET_TEXT, title);

		// Check the object selection and clear the storage name.
		if (response == OPT_RESOURCE_DELETE ||
			response == OPT_RESOURCE_IMPORT ||
			response == OPT_RESOURCE_EXPORT)
			objList->wnFlags |= WNF_SELECT_MULTIPLE;
		else
			objList->wnFlags &= ~WNF_SELECT_MULTIPLE;
		UI_WINDOW_OBJECT *field = Get(FIELD_OBJECTNAME);
		field->Information(I_SET_TEXT, _blankString);
		Add(field);
		}
		break;

	case I_SET_REQUESTOR:
		// Set the information request.
		requestor = (UI_WINDOW_OBJECT *)data;
		if (requestFile)
		{
			requestFile->GetCWD(currentPath, ZIL_MAXPATHLEN);
			Get(FIELD_DIRECTNAME)->Information(I_SET_TEXT, currentPath);
			ListUpdate();
		}
		break;

	case I_GET_TEXT:
	case I_SET_TEXT:
		return (Get(FIELD_OBJECTNAME)->Information(request, data, objectID));

	// ----- Storage service requests -----
	case I_GET_STORAGE:
		// Get the storage.
		if (!data || !requestFile)
			return (requestFile);
		*(ZIL_STORAGE **)data = requestFile;
		break;
	case I_SET_STORAGE:
		// Set the information request.
		requestFile = data ? (ZIL_STORAGE *)data : ZIL_NULLP(ZIL_STORAGE);
		if (requestFile) // reset the current storage.
		{
			requestFile->GetCWD(currentPath, ZIL_MAXPATHLEN);
			Get(FIELD_DIRECTNAME)->Information(I_SET_TEXT, currentPath);
			ListUpdate();
		}
		else // create a temporary storage.
		{
			ZIL_STORAGE *newFile = new ZIL_STORAGE(_serviceManager->ZMSG_tempFileName(), UIS_CREATE | UIS_READWRITE);
			if (newFile->storageError)
			{
				delete newFile;
				break;
			}
			requestFile = newFile;
		}

		// Check for valid directories.
		extern ZIL_ICHAR *_rootStorageDirectory;

		// Check the icon directory.
		extern ZIL_ICHAR *_iconDirectory;
		requestFile->ChDir(_rootStorageDirectory);
		if (requestFile->ChDir(_iconDirectory))
		{
			requestFile->MkDir(_iconDirectory);
			requestFile->ChDir(_iconDirectory);
			_storage->ChDir(_rootStorageDirectory);
			_storage->ChDir(_iconDirectory);
			// Set the initial icons.
			static ZIL_ICHAR *name[] =
			{
				UIW_ICON::_applicationIconName,
				UIW_ICON::_asteriskIconName,
				UIW_ICON::_exclamationIconName,
				UIW_ICON::_handIconName,
				UIW_ICON::_questionIconName,
				ZIL_NULLP(ZIL_ICHAR)
			};
			for (int i = 0; name[i]; i++)
			{
				// Store the image information.
				int length;
				char buffer[1024];
				ZIL_STORAGE_OBJECT_READ_ONLY srcObject(*_storage, name[i], 0);
				ZIL_STORAGE_OBJECT dstObject(*requestFile, name[i], 0, UIS_OPENCREATE | UIS_READWRITE);
				while ((length = srcObject.Load(buffer, 1024, 1)) > 0)
					dstObject.Store(buffer, length, 1);
			}
		}

		// Check the bitmap directory.
		extern ZIL_ICHAR *_bitmapDirectory;
		requestFile->ChDir(_rootStorageDirectory);
		if (requestFile->ChDir(_bitmapDirectory))
		{
			requestFile->MkDir(_bitmapDirectory);
			requestFile->ChDir(_bitmapDirectory);
		}

		// Return to root.
		requestFile->ChDir(_rootStorageDirectory);
		break;

	case I_GET_REVISIONS:
		*(int *)data = revisions;
		break;
	case I_SET_REVISIONS:
		revisions = *(int *)data;
		break;

	case I_GET_DEFAULT_EXTENSION:
		*(ZIL_ICHAR **)data = defaultExtension;
		break;
	case I_SET_DEFAULT_EXTENSION:
		if (data)
			strcpy(defaultExtension, (ZIL_ICHAR *)data);
		else
			defaultExtension[0] = 0;
		break;

	case I_GET_WRITE_HPP:
		*(int *)data = writeHPP;
		break;
	case I_SET_WRITE_HPP:
		writeHPP = *(int *)data;
		break;
	case I_GENERATE_HPP:
		GenerateHPP();
		break;

	case I_GET_WRITE_CPP:
		*(int *)data = writeCPP;
		break;
	case I_SET_WRITE_CPP:
		writeCPP = *(int *)data;
		break;
	case I_GENERATE_CPP:
		GenerateCPP();
		break;

	case I_SET_TRAVERSE_LOCK:
		traverseLock = *(int *)data;
		break;

	// ----- Base class requests -----
	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}
	return (data);
}

void ZAF_STORAGE_SERVICE::ListUpdate(void)
{
	ZIL_ICHAR *pathName = currentPath;
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = _storage;
	WOF_FLAGS dirFlags = traverseLock ? WOF_NON_SELECTABLE : WOF_NO_FLAGS;

	// Clear the directory entries.
	objList->Destroy();
	dirList->Destroy();

	// Determine the current path.
	int indentation = 0;
	UIW_BUTTON *item;
	if (!pathName || !strcmp(pathName, _rootStorageDirectory))
		pathName = _blankString;
	else
	{
		ZIL_ICHAR directoryName[ZIL_MAXPATHLEN];
		ZIL_ICHAR *searchName = strstr(pathName, _rootStorageDirectory);
		while (searchName)
		{
			int length = (int)(searchName - pathName);
			if (length)
				strncpy(directoryName, pathName, length);
			directoryName[length] = '\0';
			item = new ZAF_STORAGE_ITEM(indentation, directoryName, _parentItem, dirFlags);
			item->userFlags = STORAGE_DIRECTORY + indentation++;
			dirList->Add(item);
			pathName = searchName + 1;
			searchName = strstr(pathName, _rootStorageDirectory);
		}
	}

	// Determine the current directory.
	item = new ZAF_STORAGE_ITEM(indentation, pathName, _currentItem, dirFlags);
	item->userFlags = STORAGE_DIRECTORY + indentation++;
	dirList->Add(item);

	ZIL_ICHAR objectBitmapName[64];
	strcpy(objectBitmapName, pathName[0] ? pathName : _objectItem);

	// Determine directories and objects of current directory.
	pathName = requestFile->FindFirstObject(_serviceManager->ZMSG_allObjects());
	while (pathName)
	{
		if (strcmp(pathName, _parentDirectory) &&
			strcmp(pathName, _currentDirectory))
		{
			ZIL_STORAGE_OBJECT_READ_ONLY object(*requestFile, pathName, 0);
			ZIL_STATS_INFO *stats = object.Stats();
			if (FlagSet(stats->useCount, ZIL_DIRECTORY_TAG) && !traverseLock)
			{
				item = new ZAF_STORAGE_ITEM(indentation, pathName, _childItem, WOF_NO_FLAGS);
				item->userFlags = STORAGE_DIRECTORY + indentation;
				dirList->Add(item);
			}
			else
			{
				item = new ZAF_STORAGE_ITEM(0, pathName, objectBitmapName, WOF_NO_FLAGS);
				item->userFlags = STORAGE_OBJECT + indentation;
				objList->Add(item);
			}
		}
		pathName = requestFile->FindNextObject();
	}
	defaultStorage = saveStorage;
#if defined(ZIL_NEXTSTEP)
	Event(S_REDISPLAY);
#endif	
}

EVENT_TYPE ZAF_STORAGE_SERVICE::ObjectNameCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = OPT_OK;
		object->eventManager->Put(event);
	}
	return (ccode);
}

int ZAF_STORAGE_SERVICE::RRmDir(ZIL_STORAGE *storage, const ZIL_ICHAR *name)
{
	// Open the directory to delete the entries.
	ZIL_STORAGE_DIRECTORY *directory = storage->OpenDir(name);
	if (directory == ZIL_NULLP(ZIL_STORAGE_DIRECTORY))
		return (-1);

	// Delete each directory entry.
	for (directoryEntry *entry = directory->ReadDir(); entry; entry = directory->ReadDir())
		if (strcmp(entry->stringID, _currentDirectory) &&
			strcmp(entry->stringID, _parentDirectory))
		{
			ZIL_ICHAR path[ZIL_MAXPATHLEN];
			strcpy(path, name);
			strcat(path, _rootStorageDirectory);
			strcat(path, entry->stringID);
			if (RRmDir(storage, path) < 0 && storage->DestroyObject(path) < 0)
				return (-1);
		}
	delete directory;
	storage->RmDir(name);
	storage->storageError = 0;
	return (0);
}

// ----- Static tables ------------------------------------------------------

ZIL_ICHAR ZAF_STORAGE_SERVICE::_className[] = { 'Z','A','F','_','S','T','O','R','A','G','E','_','S','E','R','V','I','C','E',0 };

ZIL_STORAGE_READ_ONLY *ZAF_STORAGE_SERVICE::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

ZIL_ICHAR _ObjectNameCallback[] = { 'O','b','j','e','c','t','N','a','m','e','C','a','l','l','b','a','c','k', 0 };
ZIL_ICHAR _DirectoryCompare[] = { 'D','i','r','e','c','t','o','r','y','C','o','m','p','a','r','e', 0 };

UI_ITEM ZAF_STORAGE_SERVICE::_userTable[] =
{
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(ObjectNameCallback), _ObjectNameCallback, 0 },
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(DirectoryCompare), _DirectoryCompare, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};

UI_ITEM ZAF_STORAGE_SERVICE::_objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_PROMPT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PROMPT::New), UIW_PROMPT::_className, 0 },
	{ ID_SCROLL_BAR, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SCROLL_BAR::New), UIW_SCROLL_BAR::_className, 0 },
	{ ID_STRING, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_VT_LIST, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_VT_LIST::New), UIW_VT_LIST::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
