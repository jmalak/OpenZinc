//	M_DIRECT.CPP - File directory service object.
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// General declarations
#define USE_MSG_TABLE
#define USE_DRIVE_ERROR
#define USE_ZAF_DIRECTORY_SERVICE
#include "direct.hpp"
#include "p_direct.hpp"
#if defined(SCO_UNIX) || defined(_SINIX) || defined(__QNX__) || defined(_SUNOS4) || defined(__DECCXX)|| defined(__linux)
#	include <time.h>
#endif

const int ZMSG_TITLES			= 6100 - 0;		// Titles start at 0.
const int ZMSG_ERRORS			= 6100 - 100;	// Errors start at 100.

const EVENT_TYPE OPT_RESET_DRIVE		= 11000;
const EVENT_TYPE OPT_RESET_DIRECTORY	= 11001;
const EVENT_TYPE OPT_RESET_FILE			= 11002;
const EVENT_TYPE OPT_RESET_FILTER		= 11003;

extern ZIL_ICHAR *_currentDirectory;
extern ZIL_ICHAR *_parentDirectory;

#if defined(__DVX__)
	static ZIL_ICHAR _allDevices[]		= { '*','.','*', 0 };
#elif defined(ZIL_POSIX)
	static ZIL_ICHAR _allDevices[]		= { '*', 0 };
	static DIR *_directory				= ZIL_NULLP(DIR);
#endif

static NUMBERID DriveCharToId(char drive) { return (10100 + drive - 'a'); }
static NUMBERID DriveIntToId(int drive) { return (10100 + drive); }
static int DriveCharToInt(char drive) { return (tolower(drive) - 'a'); }
static char DriveIntToChar(int drive) { return (drive + 'a'); }

// --------------------------------------------------------------------------
// ----- ZAF_DIRECTORY_ITEM -------------------------------------------------
// --------------------------------------------------------------------------

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

class ZAF_DIRECTORY_ITEM : public UIW_BUTTON
{
	friend class ZAF_DIRECTORY_SERVICE;
public:
	ZAF_DIRECTORY_ITEM(int indentation, ZIL_ICHAR *pathName, ZIL_ICHAR *type);
	~ZAF_DIRECTORY_ITEM(void);

private:
	static struct ZAF_ITEM_SET _item[];

	static EVENT_TYPE DirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE DriveCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE FileCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ParentDirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

static ZIL_ICHAR _parentDirectoryName[] = { 'p','a','r','e','n','t','D','i','r','e','c','t','o','r','y', 0 };
static ZIL_ICHAR _currentDirectoryName[] = { 'c','u','r','r','e','n','t','D','i','r','e','c','t','o','r','y', 0 };
static ZIL_ICHAR _childDirectoryName[] = { 'c','h','i','l','d','D','i','r','e','c','t','o','r','y', 0 };
static ZIL_ICHAR _hardDriveName[] = { 'h','a','r','d','D','r','i','v','e', 0 };
static ZIL_ICHAR _networkDriveName[] = { 'n','e','t','w','o','r','k','D','r','i','v','e', 0 };
static ZIL_ICHAR _softDriveName[] = { 's','o','f','t','D','r','i','v','e', 0 };
static ZIL_ICHAR _fileName[] = { 'f','i','l','e', 0 };

struct ZAF_ITEM_SET ZAF_DIRECTORY_ITEM::_item[] =
{
	{ _parentDirectoryName, 0, 	0, 	0, 	0, 	0, ParentDirectoryCallback },
	{ _childDirectoryName, 	0, 	0, 	0, 	0, 	0, DirectoryCallback },
	{ _currentDirectoryName,0, 	0, 	0, 	0, 	0, 0 },
	{ _hardDriveName, 		0, 	0, 	0, 	0, 	0, DriveCallback},
	{ _networkDriveName, 	0, 	0, 	0, 	0, 	0, DriveCallback },
	{ _softDriveName, 		0, 	0, 	0, 	0, 	0, DriveCallback },
	{ _fileName, 			0, 	0, 	0, 	0, 	0, FileCallback },
	{ 0, 					0, 	0, 	0, 	0, 	0, 0 }
};

ZAF_DIRECTORY_ITEM::ZAF_DIRECTORY_ITEM(int indentation, ZIL_ICHAR *pathName, ZIL_ICHAR *type) :
	// Initialize the base class
	UIW_BUTTON(indentation, 0, 22, pathName,
	BTF_NO_3D | BTF_DOUBLE_CLICK | BTF_STATIC_BITMAPARRAY, WOF_NO_FLAGS)
{
	static int initialized = FALSE;
	if (!initialized)
	{
		extern ZIL_ICHAR *_bitmapDirectory;
		ZIL_STORAGE_READ_ONLY *directory = ZAF_DIRECTORY_SERVICE::_storage;
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
#			if !defined(ZIL_MOTIF)
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

ZAF_DIRECTORY_ITEM::~ZAF_DIRECTORY_ITEM(void)
{
	colorBitmap = monoBitmap = 0;
}

EVENT_TYPE ZAF_DIRECTORY_ITEM::DirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		event.type = OPT_RESET_DIRECTORY;
		event.data = object->Information(I_GET_TEXT, ZIL_NULLP(void));
		object->eventManager->Put(event);
	}
	return (ccode);
}

EVENT_TYPE ZAF_DIRECTORY_ITEM::DriveCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = OPT_RESET_DRIVE;
		event.data = object->Information(I_GET_TEXT, ZIL_NULLP(void));
		object->eventManager->Put(event);
	}
	return (ccode);
}

EVENT_TYPE ZAF_DIRECTORY_ITEM::FileCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || ccode == L_SELECT)
	{
		event.type = OPT_RESET_FILE;
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

EVENT_TYPE ZAF_DIRECTORY_ITEM::ParentDirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	static ZIL_ICHAR _fullPath[ZIL_MAXPATHLEN];
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		event.type = OPT_RESET_DIRECTORY;
		UI_WINDOW_OBJECT *tObject = object;
		while (tObject->Previous())
			tObject = tObject->Previous();
		_fullPath[0] = '\0';
		while (tObject != object)
		{
			ZIL_INTERNATIONAL::strcat(_fullPath, (ZIL_ICHAR *)tObject->Information(I_GET_TEXT, ZIL_NULLP(void)));
			ZIL_INTERNATIONAL::strcat(_fullPath, defaultCharMap->dirSepStr);
			tObject = tObject->Next();
		}
		ZIL_INTERNATIONAL::strcat(_fullPath, (ZIL_ICHAR *)object->Information(I_GET_TEXT, ZIL_NULLP(void)));
		if (!object->Previous())
			ZIL_INTERNATIONAL::strcat(_fullPath, defaultCharMap->dirSepStr);
		event.data = _fullPath;
		object->eventManager->Put(event);
	}
	return (ccode);
}

// --------------------------------------------------------------------------
// ----- ZAF_DIRECTORY_SERVICE ----------------------------------------------
// --------------------------------------------------------------------------

ZAF_DIRECTORY_SERVICE::ZAF_DIRECTORY_SERVICE(void) :
	// base initialization
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_direct"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable),
	// member initialization
	requestor(ZIL_NULLP(UI_WINDOW_OBJECT)), response(0),
	msgTable(_serviceManager->ZMSG_msgTableName(), _serviceManager->Storage(&_storage, "p_direct"))
{
	// Set the default information.
	searchID = ID_DIRECTORY_SERVICE;
	requestedFile[0] = '\0';

	if (windowManager)
		windowManager->Center(this);
	originalDrive = currentDrive = GetDrive();
	GetPath(originalPath);

	ZNC_DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);
	woAdvancedFlags |= WOAF_MODAL;
	strcpy(currentPath, originalPath);

	// Set up the directory and filter strings.
	Get(FIELD_DIRECTNAME)->Information(I_SET_TEXT, currentPath);
	Get(FIELD_FILENAME)->Information(I_SET_TEXT, _serviceManager->ZMSG_datFilter());
	Get(FIELD_FILE_FILTERS)->Information(I_SET_TEXT, _serviceManager->ZMSG_datFilter());

	// Set up the drive, directory and file fields.
	DriveUpdate();
	DirectoryUpdate();
	FileUpdate();
}

ZAF_DIRECTORY_SERVICE::~ZAF_DIRECTORY_SERVICE(void)
{
	// Restore the original drive and directory.
	SetDrive(originalDrive);
	SetPath(originalPath);

	// Remove the resource storage.
	if (_storage)
		delete _storage;

	// Free the color and monochrome bitmap handles.
	struct ZAF_ITEM_SET *_item = ZAF_DIRECTORY_ITEM::_item;
	for (int i = 0; _item[i].name; i++)
	{
		if (_item[i].bitmapArray)
			delete _item[i].bitmapArray;

		display->DestroyBitmapHandle(screenID, &_item[i].colorBitmap, &_item[i].monoBitmap);
	}
}

ZIL_ICHAR *ZAF_DIRECTORY_SERVICE::DeviceName(ZIL_ICHAR *deviceName)
{
	// Retrieve the current device name.
	ZIL_ICHAR *name;
	Get(FIELD_FILENAME)->Information(I_GET_TEXT, &name);
	strcpy(deviceName, name);
	return (deviceName);
}

EVENT_TYPE ZAF_DIRECTORY_SERVICE::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_OK:
		// Check for wild cards and blank names.
		ZIL_ICHAR *filterName;
		Get(FIELD_FILENAME)->Information(I_GET_TEXT, &filterName);
		if (!filterName || !filterName[0])
		{
			ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_BLANK_NAME);
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, message);
			break;
		}
		else if (strchr(filterName, '*') ||
			strchr(filterName, '?'))
		{
			Get(FIELD_FILE_FILTERS)->Information(I_SET_TEXT, filterName);
			FileUpdate();
			break;
		}

		// Check for a file overwrite.
		PathName(requestedFile);
		if ((response == OPT_FILE_NEW || response == OPT_FILE_SAVEAS || response == OPT_FILE_EXPORT) &&
			ZIL_STORAGE::ValidName(requestedFile, FALSE))
		{
			ZIL_ICHAR *title = response ?
				msgTable.GetMessage((int)(response - ZMSG_TITLES)) :
				msgTable.GetMessage(ZMSG_FILE_TNONE);
			ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_FILE_OVERWRITE);
			ZIL_ICHAR *fileName; Get(FIELD_FILENAME)->Information(I_GET_TEXT, &fileName);
			ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
				ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, fileName);
			UI_ERROR_STUB::Beep();
			if (window.Control() != ZIL_DLG_YES)
				break; // Do not overwrite the file.
		}

		// Check for valid files (service then programmer levels).
		if (!ZIL_STORAGE::ValidName(requestedFile, (response == OPT_FILE_NEW || response == OPT_FILE_SAVEAS || response == OPT_FILE_EXPORT)))
			return (ZAF_DIRECTORY_SERVICE::Event(S_ERROR));
		else if (requestor)
		{
			// Confirm deletion processes.
			if (response == OPT_FILE_DELETE)
			{
				ZIL_ICHAR *fileName; Get(FIELD_FILENAME)->Information(I_GET_TEXT, &fileName);
				ZIL_ICHAR *title = msgTable.GetMessage(ZMSG_FILE_TDELETE);
				ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_FILE_DELETE);
				ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
					ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, fileName);
				UI_ERROR_STUB::Beep();
				if (window.Control() != ZIL_DLG_YES)
					break; // Do not delete the file.
			}

			// Send control to the requesting object.
			eventManager->DeviceState(E_MOUSE, DM_WAIT);
			UI_EVENT rEvent(-response);
			rEvent.data = requestedFile;
			ccode = requestor->Event(rEvent); // Send a response to the requestor.
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
			if (ccode == S_ERROR)
				break; // Error message is handled by requestor.
		}
		// Continue to OPT_CLOSE.
	case OPT_CANCEL:
		// Remove the window from the window manager.
		if (requestor)
		{
			UI_EVENT rEvent(-response);
			requestor->Event(rEvent); // NULL data response tells requestor we're done.
		}
		if (ccode != OPT_CANCEL && ZAF_SERVICE_MANAGER::_queuedEvent)
			eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
		else
		{
			ZAF_SERVICE_MANAGER::_queuedEvent = 0;
			eventManager->Put(S_CLOSE, Q_BEGIN);
		}
		requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
		response = 0;
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case OPT_RESET_DRIVE:
	case OPT_RESET_DIRECTORY:
		// Reset the drive or directory information.
		{
		ZIL_ICHAR *data = (ZIL_ICHAR *)event.data;
		if (ccode == OPT_RESET_DRIVE)
		{
			// Set the drive response information.
			EVENT_TYPE errorCode = FALSE;
			ZAF_MESSAGE_WINDOW *errorWindow = ZIL_NULLP(ZAF_MESSAGE_WINDOW);
			eventManager->DeviceState(E_MOUSE, DM_WAIT);

			// Test the new drive.
#			if defined(ZIL_MSDOS) || defined(__DVX__)
			while (SetDrive(DriveCharToInt(*data)))
			{
				// Prepare the error message.
				if (!errorWindow)
				{
					ZIL_ICHAR *title = response ?
						msgTable.GetMessage(response - ZMSG_TITLES) :
						msgTable.GetMessage(ZMSG_FILE_TNONE);
					ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_DRIVE_ERROR);
					errorWindow = new ZAF_MESSAGE_WINDOW(title, ZIL_NULLP(ZIL_ICHAR),
						ZIL_MSG_CANCEL | ZIL_MSG_RETRY,
						ZIL_MSG_CANCEL, message, *data);
				}

				// Display the drive error.
				UI_ERROR_STUB::Beep();
				errorCode = errorWindow->Control();
				if (errorCode == ZIL_DLG_CANCEL)
				{
					errorCode = TRUE;
					break;
				}
				else
				{
					eventManager->DeviceState(E_MOUSE, DM_WAIT);
					errorCode = FALSE;
				}
			}
#			else
			errorCode = SetDrive(DriveCharToInt(*data)) ? TRUE : FALSE;
#			endif

			// Delete the error window, if any.
			if (errorWindow)
				delete errorWindow;
			eventManager->DeviceState(E_MOUSE, DM_VIEW);

			// Re-add the old drive information.
			if (errorCode)
			{
				UI_EVENT event(S_ADD_OBJECT);
				event.windowObject = Get(DriveIntToId(currentDrive));
				if (event.windowObject)
					Get(FIELD_DRIVES)->Event(event); // Restore the previous drive.
				break;
			}
		}
		else
		{
			// Set the directory.
			if (SetPath(data))
				break; // error on setpath.
		}
		}

		// Reset the drive and directory, then update the lists.
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		currentDrive = GetDrive();
		GetPath(currentPath);
		Get(FIELD_DIRECTNAME)->Information(I_SET_TEXT, currentPath);

		DirectoryUpdate();
		FileUpdate();
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case OPT_RESET_FILE:
		// Reset the filename.
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		Get(FIELD_FILENAME)->Information(I_SET_TEXT, event.data);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case OPT_RESET_FILTER:
		// Reset the filter selection.
		{
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		ZIL_ICHAR *filterString = _serviceManager->ZMSG_datFilter();
		Get(FIELD_FILE_FILTERS)->Information(I_GET_TEXT, &filterString);
		Get(FIELD_FILENAME)->Information(I_SET_TEXT, filterString);
		FileUpdate();
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		break;

	case OPT_FILE_DELETE:
		// Confirm deletion of the file.
		{
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		ZIL_ICHAR *title = msgTable.GetMessage(ZMSG_FILE_TDELETE);
		ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_FILE_DELETE);
		ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR), ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, pathName);
		UI_ERROR_STUB::Beep();
		if (window.Control() != ZIL_DLG_YES)
			ccode = S_ERROR;
		}
		break;

	case S_ERROR:
		// Report an invalid file error.
		if (errorSystem)
		{
			// Error messages are indexed by option #.
			ZIL_ICHAR *message = response ?
				msgTable.GetMessage((int)(response - ZMSG_ERRORS)) :
				msgTable.GetMessage(ZMSG_FILE_NONE_ERROR);
			ZIL_ICHAR *resName; Get(FIELD_FILENAME)->Information(I_GET_TEXT, &resName);
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, message, resName);
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

ZIL_ICHAR *ZAF_DIRECTORY_SERVICE::PathName(ZIL_ICHAR *pathName)
{
	static ZIL_ICHAR _2String[] = { '%','s','%','s', 0 };
	static ZIL_ICHAR _3String[] = { '%','s','%','s','%','s', 0 };

	// Get the filename and check for default extensions.
	ZIL_ICHAR *name;
	Get(FIELD_FILENAME)->Information(I_GET_TEXT, &name);
	extern ZIL_ICHAR *_zilExtName;
	ZIL_ICHAR *extension = _zilExtName;
	UI_WINDOW_OBJECT *service = _serviceManager->Get(ID_STORAGE_SERVICE);
	if (service)
		service->Information(I_GET_DEFAULT_EXTENSION, &extension);
	if (!strchr(name, '.'))
	{
		strcat(name, extension);
		Get(FIELD_FILENAME)->Information(I_SET_TEXT, name);
	}

	// Determine the current directory and device name.
	int length = strlen(currentPath);
	if (strchr(name, ':') || strchr(name, defaultCharMap->dirSepStr[0]))
		strcpy(pathName, name); // field has full path.
	else if (!length || currentPath[length-1] != defaultCharMap->dirSepStr[0])
		sprintf(pathName, _3String, currentPath, defaultCharMap->dirSepStr, name);
	else
		sprintf(pathName, _2String, currentPath, name);
	return (pathName);
}

// --------------------------------------------------------------------------
// ----- Directory Support Functions ----------------------------------------
// --------------------------------------------------------------------------

int DirectoryCompare(void *object1, void *object2)
{
	if (((UI_WINDOW_OBJECT *)object1)->userFlags)
		return (-1);
	else if (((UI_WINDOW_OBJECT *)object2)->userFlags)
		return (1);
	return (UIW_WINDOW::StringCompare(object1, object2));
}

void ZAF_DIRECTORY_SERVICE::DirectoryUpdate(void)
{
	ZNC_DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);

	// Set up the Directory field.
	UI_WINDOW_OBJECT *directory = Get(LIST_DIRECTORIES);
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = _storage;
	if (directory)
	{
		directory->Information(I_DESTROY_LIST, ZIL_NULLP(void));

		int indentation = 0;
		UIW_BUTTON *cDirectoryField = ZIL_NULLP(UIW_BUTTON);
		if (*currentPath)
		{
			ZIL_ICHAR subString[ZIL_MAXPATHLEN];
			strcpy(subString, currentPath);
			int length = strlen(subString);
			strrepc(subString, defaultCharMap->dirSepStr[0], '\0');
			for (int i = 0; i < length; )
			{
				int j = i + strlen(&subString[i]) + 1;
				cDirectoryField = new ZAF_DIRECTORY_ITEM(indentation++, &subString[i],
					(j < length) ? _parentDirectoryName : _currentDirectoryName);
				if (j >= length)
					cDirectoryField->woStatus |= (WOS_SELECTED | WOS_CURRENT);
				cDirectoryField->userFlags = DEVICE_DIRECTORY;
				event.data = cDirectoryField;
				directory->Event(event);
				i = j;
			}
		}

		Open();
		for (int error = FindFirst(_allDevices, device, DEVICE_DIRECTORY); !error; error = FindNext(device))
			if (device.Inherited(DEVICE_DIRECTORY) &&
				strcmp(device.name, _currentDirectory) &&
				strcmp(device.name, _parentDirectory))
			{
				event.data = new ZAF_DIRECTORY_ITEM(indentation, device.name, _childDirectoryName);
				directory->Event(event);
			}
		Close();

		if (directory->screenID)
			directory->Event(S_REDISPLAY);
	}
	defaultStorage = saveStorage;
}

// --------------------------------------------------------------------------
// ----- Drive Support Functions --------------------------------------------
// --------------------------------------------------------------------------

void ZAF_DIRECTORY_SERVICE::DriveUpdate(void)
{
	ZNC_DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);

	// Set up the Drive field.
	UI_WINDOW_OBJECT *drive = Get(FIELD_DRIVES);
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = _storage;
	if (drive)
	{
		drive->Information(I_DESTROY_LIST, ZIL_NULLP(void));

		Open();
		void *cDriveField = ZIL_NULLP(void);
		for (int error = FindFirst(_allDevices, device, DEVICE_DRIVE); !error; error = FindNext(device))
		{
			ZIL_ICHAR *itemType = _hardDriveName;
			if (device.Inherited(DEVICE_DRIVE_REMOTE))
				itemType = _networkDriveName;
			else if (device.Inherited(DEVICE_DRIVE_REMOVABLE))
				itemType = _softDriveName;
			UIW_BUTTON *button = new ZAF_DIRECTORY_ITEM(0, device.name, itemType);
			button->NumberID(DriveCharToId(*device.name));
			event.data = button;
			if (DriveCharToInt(*device.name) == currentDrive)
				cDriveField = event.data;
			drive->Event(event);
		}
		Close();

		if (cDriveField)
		{
			event.data = cDriveField;
			drive->Event(event);
		}
		if (drive->screenID)
			drive->Event(S_REDISPLAY);
	}
	defaultStorage = saveStorage;
}

// --------------------------------------------------------------------------
// ----- File Support Functions ---------------------------------------------
// --------------------------------------------------------------------------

void ZAF_DIRECTORY_SERVICE::FileUpdate(void)
{
	ZNC_DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);

	// Set up the File field.
	UI_WINDOW_OBJECT *file = Get(LIST_FILES);
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = _storage;
	if (file)
	{
		file->Information(I_DESTROY_LIST, ZIL_NULLP(void));

		ZIL_ICHAR *filterString = _serviceManager->ZMSG_datFilter();
		Get(FIELD_FILE_FILTERS)->Information(I_GET_TEXT, &filterString);

		Open();
		for (int error = FindFirst(_allDevices, device, DEVICE_FILE); !error; error = FindNext(device))
			if (device.Inherited(DEVICE_FILE) &&
				!WildStrcmp(device.name, filterString))
			{
				event.data = new ZAF_DIRECTORY_ITEM(0, device.name, _fileName);
				file->Event(event);
			}
		Close();

		if (file->screenID)
			file->Event(S_REDISPLAY);
	}
	defaultStorage = saveStorage;
}

// --------------------------------------------------------------------------
// ----- Filter Support Functions -------------------------------------------
// --------------------------------------------------------------------------

EVENT_TYPE FilterCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = OPT_RESET_FILTER;
		event.data = object->Information(I_GET_TEXT, ZIL_NULLP(void));
		object->eventManager->Put(event);
	}
	return (ccode);
}

// --------------------------------------------------------------------------
// ----- Filename Support Function ------------------------------------------
// --------------------------------------------------------------------------

EVENT_TYPE FilenameCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = OPT_OK;
		object->eventManager->Put(event);
	}
	return (ccode);
}

// --------------------------------------------------------------------------
// ----- Miscellaneous Compiler-Dependent Functions -------------------------
// --------------------------------------------------------------------------

#if defined(__DVX__)
#	define DRIVE_ABORT 		2
#	define IOCTL_CHANGEABLE	8
#	define IOCTL_LOCAL_REMOTE	9

static int __far systemerror(unsigned, unsigned, unsigned far *)
{
	return (DRIVE_ABORT);
}
static int ioctl(int handle, int functionID)
{
	union REGS regs;

	// Check for a valid function number (8 or 9).
	if (functionID != 8 && functionID != 9)
		return (-1);
	regs.h.ah = 0x44;
	regs.h.al = functionID;
	regs.h.bl = handle;
	int386(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (functionID == 8 ? regs.w.ax : regs.w.dx));
}

static DEVICE_CLASSIFICATION drivetype(int drive)
{
	// Set the critical error handler.
	static int seterror = TRUE;
	if (seterror)
	{
		_harderr(systemerror);
		seterror = FALSE;
	}

	// Determine the drive type.
	DEVICE_CLASSIFICATION driveType = DEVICE_UNKNOWN;
	int error = ioctl(drive + 1, IOCTL_LOCAL_REMOTE);
	if (error == -1)
		;
	else if (FlagSet(error, 0x1000))
		driveType = DEVICE_DRIVE_REMOTE;
	else if (ioctl(drive + 1, IOCTL_CHANGEABLE) == 1)
		driveType = DEVICE_DRIVE_FIXED;
	else
		driveType = DEVICE_DRIVE_REMOVABLE;
	return (driveType);
}
#elif defined(ZIL_MSWINDOWS)
static DEVICE_CLASSIFICATION drivetype(int drive)
{
#	if defined(ZIL_WINNT)
	ZIL_ICHAR sDrive[ZIL_MAXPATHLEN];
	ZIL_ICHAR format[] = { '%','c',':','%','s', 0 };
	ZIL_INTERNATIONAL::sprintf(sDrive, format, DriveIntToChar(drive), ZIL_INTERNATIONAL::defaultCharMap->dirSepStr);
	DEVICE_CLASSIFICATION driveType = GetDriveType(sDrive);
#	else
	DEVICE_CLASSIFICATION driveType = GetDriveType(drive);
#	endif
	if (driveType == DRIVE_FIXED)
		driveType = DEVICE_DRIVE_FIXED;
	else if (driveType == DRIVE_REMOVABLE)
		driveType = DEVICE_DRIVE_REMOVABLE;
	else if (driveType == DRIVE_REMOTE)
		driveType = DEVICE_DRIVE_REMOTE;
	else
		driveType = DEVICE_UNKNOWN;
	return (driveType);
}
#elif defined(ZIL_POSIX)
static DEVICE_CLASSIFICATION drivetype(int)
{
	// POSIX systems do not have drives.
	return (DEVICE_UNKNOWN);
}
#endif

void ZAF_DIRECTORY_SERVICE::Close(void)
{
#if defined(ZIL_POSIX)
	closedir(_directory);
#endif
}

int ZAF_DIRECTORY_SERVICE::DeviceInfo(ZIL_ICHAR *fullName, ZNC_DEVICE_INFO &info)
{
	// Check for NULL pathnames.
	ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
	if (!fullName)
	{
		PathName(pathName);
		fullName = pathName;
	}

#if defined(ZIL_POSIX)
	struct stat statx;
	int error = stat(fullName, &statx);
	if (!error)
	{
		info.classification = S_ISDIR(statx.st_mode) ? DEVICE_DIRECTORY : DEVICE_FILE;
		ZIL_STORAGE::StripFullPath(fullName, ZIL_NULLP(ZIL_ICHAR), info.name);
		info.size.Import((ZIL_IBIGNUM)statx.st_size);
		struct tm *time = localtime(&statx.st_mtime);
		info.date.Import(1900 + time->tm_year, time->tm_mon + 1, time->tm_mday);
		info.time.Import(time->tm_hour, time->tm_min, time->tm_sec);
	}
#elif defined(__WATCOMC__)
	char *_fullName = MapText(fullName);
	int error = _dos_findfirst(_fullName, _A_SUBDIR, &info._reserved) ? -1 : 0;
	if (!error)
	{
		info.classification = (info._reserved.attrib == _A_SUBDIR) ? DEVICE_DIRECTORY : DEVICE_FILE;
		ZIL_ICHAR _name[ZIL_MAXPATHLEN];
		UnMapText(info._reserved.name, _name);
		strcpy(info.name, _name);
		strlwr(info.name);
		info.size.Import((long)info._reserved.size);
		info.date.Import(info._reserved.wr_date);
		info.time.Import(info._reserved.wr_time);
	}
	delete _fullName;
#endif

	return (error);
}

int ZAF_DIRECTORY_SERVICE::FindFirst(const ZIL_ICHAR *fullName, ZNC_DEVICE_INFO &info, DEVICE_CLASSIFICATION classification)
{
	info.reserved = 0;
	info.classification = classification;

	// ----- drive code. -----
	if (info.Inherited(DEVICE_DRIVE))
		return (FindNext(info));

	// ----- directory and file code. -----
#if defined(ZIL_POSIX)
	int error = 0;
#elif defined(__WATCOMC__)
	char *_fullName = MapText(fullName);
	int error = _dos_findfirst(_fullName, _A_SUBDIR, &info._reserved) ? -1 : 0;
	delete _fullName;
#endif

	if (!error)
		return (FindNext(info));
	return (error);
}

int ZAF_DIRECTORY_SERVICE::FindNext(ZNC_DEVICE_INFO &info)
{
	const int LAST_DRIVE = 25;

	// ----- drive code. -----
	if (info.Inherited(DEVICE_DRIVE))
	{
#if defined(ZIL_POSIX)
		int drive = LAST_DRIVE + 1;
#elif defined(__WATCOMC__)
		int drive = info.reserved ? (int)info._reserved.size : 0;
#endif

		DEVICE_CLASSIFICATION driveType = drivetype(drive);
		while (!driveType && drive <= LAST_DRIVE)
		{
			drive++;
			driveType = drivetype(drive);
		}
		if (driveType)
		{
			static ZIL_ICHAR _1Char[] = { '%','c',':', 0 };
			info.reserved++;
			info.classification = driveType;
			sprintf(info.name, _1Char, DriveIntToChar(drive));
			info.date.Import();
			info.time.Import();
			info.size.Import((ZIL_IBIGNUM)0);
#if defined(ZIL_POSIX)
			// Stub for POSIX systems.
#elif defined(__WATCOMC__)
			info._reserved.size = drive + 1;
#endif
		}

		return (driveType ? 0 : -1);
	}

	// ----- directory and file code. -----
#if defined(ZIL_POSIX)
	info._reserved = readdir(_directory);
	int error = info._reserved ? 0 : -1;
	if (!error)
	{
		info.reserved++;
		struct stat statx;
		::stat(info._reserved->d_name, &statx);
		info.classification = S_ISDIR(statx.st_mode) ? DEVICE_DIRECTORY : DEVICE_FILE;
		ZIL_ICHAR _name[ZIL_MAXPATHLEN];
		UnMapText(info._reserved->d_name, _name);
		strcpy(info.name, _name);
	}
#elif defined(__WATCOMC__)
	int error = info.reserved ? (::_dos_findnext(&info._reserved) ? -1 : 0) : 0;
	if (!error)
	{
		info.reserved++;
		info.classification = (info._reserved.attrib == _A_SUBDIR) ? DEVICE_DIRECTORY : DEVICE_FILE;
		ZIL_ICHAR _name[ZIL_MAXPATHLEN];
		UnMapText(info._reserved.name, _name);
		strcpy(info.name, _name);
		strlwr(info.name);
	}
#endif

	return (error);
}

int ZAF_DIRECTORY_SERVICE::GetDrive(void)
{
#if defined(ZIL_POSIX)
	return (-1);
#else
	unsigned drive;
	_dos_getdrive(&drive);
	return (drive - 1);
#endif
}

int ZAF_DIRECTORY_SERVICE::GetPath(ZIL_ICHAR *pathName)
{
	int error = getcwd(pathName, ZIL_MAXPATHLEN) ? 0 : -1;
#if !defined(ZIL_POSIX)
	if (!error)
		strlwr(pathName);
#endif
	return (error);
}

void *ZAF_DIRECTORY_SERVICE::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the storage service.
		*windowManager + this;
		break;

	case I_SET_REQUESTOR:
		requestor = (UI_WINDOW_OBJECT *)data;
		break;

	case I_SET_REQUEST:
		{
		// Set the request.
		response = *(EVENT_TYPE *)data;
		ZIL_ICHAR *title = response ?
			msgTable.GetMessage((int)(response - ZMSG_TITLES)) :
			msgTable.GetMessage(ZMSG_FILE_TNONE);
		UIW_WINDOW::Information(I_SET_TEXT, title);

		// Clear the file name.
		UI_WINDOW_OBJECT *field = Get(FIELD_FILENAME);
		field->Information(I_SET_TEXT, _blankString);
		Add(field);
		}
		break;

	case I_SET_FILTER:
		Get(FIELD_FILENAME)->Information(I_SET_TEXT, data, objectID);
		Get(FIELD_FILE_FILTERS)->Information(I_SET_TEXT, data, objectID);
		FileUpdate();
		break;

	case I_GET_TEXT:
	case I_SET_TEXT:
		return (Get(FIELD_FILENAME)->Information(request, data, objectID));

	// ----- Base class requests -----
	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	return (data);
}

void ZAF_DIRECTORY_SERVICE::Open(void)
{
#if defined(ZIL_POSIX)
	_directory = opendir(".");
#endif
}

int ZAF_DIRECTORY_SERVICE::SetDrive(int drive)
{
#if defined(ZIL_POSIX)
	return (-1);
#else
	ZIL_ICHAR tempPath[ZIL_MAXPATHLEN];
	unsigned temp, oldDrive;
	_dos_getdrive(&oldDrive);
	_dos_setdrive(drive + 1, &temp);
	_dos_getdrive(&temp);

	if ((temp - 1 == drive) && getcwd(tempPath, ZIL_MAXPATHLEN))
		return (0);
	_dos_setdrive(oldDrive, &temp);
	return (-1);
#endif
}

int ZAF_DIRECTORY_SERVICE::SetPath(ZIL_ICHAR *pathName)
{
	return (chdir(pathName));
}

// --------------------------------------------------------------------------
// ----- Static Member Variables --------------------------------------------
// --------------------------------------------------------------------------

ZIL_ICHAR ZAF_DIRECTORY_SERVICE::_className[] = { 'Z','A','F','_','D','I','R','E','C','T','O','R','Y','_','S','E','R','V','I','C','E',0 };

ZIL_STORAGE_READ_ONLY *ZAF_DIRECTORY_SERVICE::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

static ZIL_ICHAR _FilenameCallback[] = { 'F','i','l','e','n','a','m','e','C','a','l','l','b','a','c','k', 0 };
static ZIL_ICHAR _FilterCallback[] = { 'F','i','l','t','e','r','C','a','l','l','b','a','c','k', 0 };
static ZIL_ICHAR _DirectoryCompare[] = { 'D','i','r','e','c','t','o','r','y','C','o','m','p','a','r','e', 0 };

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(FilenameCallback), _FilenameCallback, 0 },
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(FilterCallback), _FilterCallback, 0 },
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(DirectoryCompare), _DirectoryCompare, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_DIRECTORY_SERVICE::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_COMBO_BOX, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_COMBO_BOX::New), UIW_COMBO_BOX::_className, 0 },
	{ ID_ICON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_ICON::New), UIW_ICON::_className, 0 },
	{ ID_PROMPT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PROMPT::New), UIW_PROMPT::_className, 0 },
	{ ID_SCROLL_BAR, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SCROLL_BAR::New), UIW_SCROLL_BAR::_className, 0 },
	{ ID_STRING, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TEXT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TEXT::New), UIW_TEXT::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_VT_LIST, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_VT_LIST::New), UIW_VT_LIST::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_DIRECTORY_SERVICE::_objectTable = __objectTable;
