//	N_DIRECT.CPP - File directory service object.
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// General declarations
#define USE_DRIVE_ERROR
#define USE_ZAF_DIRECTORY_SERVICE
#include "direct.hpp"
#include "p_direct.hpp"
#include <sys/stat.h>
#include <time.h>
extern "Objective-C"
{
#import <appkit/OpenPanel.h>
}

const int ZMSG_TITLES			= 6100 - 0;		// Titles start at 0.
const int ZMSG_ERRORS			= 6100 - 100;	// Errors start at 100.
const int ZMSG_FILE_NONE		= 10;
const int ZMSG_FILE_NONE_ERROR	= 110;

const EVENT_TYPE OPT_RESET_DRIVE		= 11000;
const EVENT_TYPE OPT_RESET_DIRECTORY	= 11001;
const EVENT_TYPE OPT_RESET_FILE			= 11002;
const EVENT_TYPE OPT_RESET_FILTER		= 11003;

static ZIL_ICHAR currentFilename[ZIL_MAXPATHLEN];
static long currentDirID;
static short currentVRefNum;
id mySavePanel;
id myOpenPanel;

// --------------------------------------------------------------------------
// ----- ZAF_DIRECTORY_SERVICE ----------------------------------------------
// --------------------------------------------------------------------------

ZAF_DIRECTORY_SERVICE::ZAF_DIRECTORY_SERVICE(void) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	requestor(ZIL_NULLP(UI_WINDOW_OBJECT)), response(0),
	msgTable(_serviceManager->ZMSG_msgTableName(), _serviceManager->Storage(&_storage, "p_direct"))
{
	// Check for a load error.
	searchID = ID_DIRECTORY_SERVICE;
	requestedFile[0] = '\0';
	mySavePanel = [SavePanel new];
	myOpenPanel = [OpenPanel new];
}

ZAF_DIRECTORY_SERVICE::~ZAF_DIRECTORY_SERVICE(void)
{
	// Restore the original drive and directory.
	SetDrive(originalDrive);
	SetPath(originalPath);

	// Remove the resource storage.
	if (_storage)
		delete _storage;
}

ZIL_ICHAR *ZAF_DIRECTORY_SERVICE::DeviceName(ZIL_ICHAR *deviceName)
{
	// Retrieve the current device name.
	ZIL_ICHAR name[ZIL_MAXPATHLEN];
	PathName(name);
	strcpy(deviceName, name);
	return (deviceName);
}

EVENT_TYPE ZAF_DIRECTORY_SERVICE::Event(const UI_EVENT &event)
{
	if (event.type == S_ERROR)
	{
		// Report an invalid file error.
		if (errorSystem)
		{
			// Error messages are indexed by option #.
			ZIL_ICHAR *message = response ?
				msgTable.GetMessage(response - ZMSG_ERRORS) :
				msgTable.GetMessage(ZMSG_FILE_NONE_ERROR);
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, message, currentFilename);
		}
		else
			UI_ERROR_STUB::Beep();
	}

	return (event.type);
}

void *ZAF_DIRECTORY_SERVICE::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	ZIL_ICHAR **fname;
	//const char **fname;
	//const char *filename;
	ZIL_ICHAR *filename;
	ZIL_INT16 choseFile = 0;
	
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:

		// Activate the storage service.
		// Check for default extensions.
		
		extern ZIL_ICHAR *_zilExtName;
		ZIL_ICHAR *extension = _zilExtName;
		UI_WINDOW_OBJECT *service = _serviceManager->Get(ID_STORAGE_SERVICE);
		if (service)
			service->Information(I_GET_DEFAULT_EXTENSION, &extension);
		if (response == OPT_FILE_NEW || response == OPT_FILE_SAVEAS)
		{
			char *tBuff = MapText(currentFilename);
			[mySavePanel setRequiredFileType:&extension[1]];
			if ([mySavePanel runModal])
			{
				filename = [mySavePanel filename];
				choseFile = 1;
			
			}
			delete tBuff;
		}
		else
		{	
			const char *const zincFileTypes[2] = { &extension[1], NULL };
			if ([myOpenPanel runModalForTypes:zincFileTypes])
			{
				fname = [myOpenPanel filenames];
				
				ZIL_ICHAR *_currentFilename = UnMapText(*fname);
				strcpy(currentFilename, _currentFilename);
				delete _currentFilename;
				filename = [myOpenPanel filename];
				choseFile = 1;
			}
		}
		
		// Send a response to the requestor.
		if (requestor)
		{
			if (choseFile)
			{
				UI_EVENT rEvent(-response);
				rEvent.data = filename;
				requestor->Event(rEvent);
				UI_WINDOW_OBJECT::eventManager->DeviceState(E_MOUSE, DM_VIEW);
			}
			else
			{
				UI_EVENT rEvent(-response);
				requestor->Event(rEvent); // NULL data response tells requestor we're done.
			}
		}
		requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
		response = 0;
		break;	
	case I_SET_REQUESTOR:
		requestor = (UI_WINDOW_OBJECT *)data;
		break;

	case I_SET_REQUEST:
		// Set the request.
		response = *(EVENT_TYPE *)data;
		break;

	// ----- Base class requests -----
	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}
	return (data);
}

ZIL_ICHAR *ZAF_DIRECTORY_SERVICE::PathName(ZIL_ICHAR *pathName)
{	
	// Check for default extensions.
	extern ZIL_ICHAR *_zilExtName;
	ZIL_ICHAR *extension = _zilExtName;
	UI_WINDOW_OBJECT *service = _serviceManager->Get(ID_STORAGE_SERVICE);
	if (service)
		service->Information(I_GET_DEFAULT_EXTENSION, &extension);
	if (!strchr(pathName, '.'))
		strcat(pathName, extension);
	return (pathName);
}

// --------------------------------------------------------------------------
// ----- Miscellaneous Compiler-Dependent Functions -------------------------
// --------------------------------------------------------------------------

void ZAF_DIRECTORY_SERVICE::Close(void)
{
}

int ZAF_DIRECTORY_SERVICE::DeviceInfo(ZIL_ICHAR *fullName, ZNC_DEVICE_INFO &info)
{
	// Check for NULL pathnames.
	if (!fullName)
		fullName = currentFilename;

	struct stat statx;
	int error = stat(fullName, &statx);
	if (!error)
	{
		info.classification = (((statx.st_mode) & (_S_IFMT)) == (_S_IFDIR)) ? DEVICE_DIRECTORY : DEVICE_FILE;
		ZIL_STORAGE::StripFullPath(fullName, ZIL_NULLP(ZIL_ICHAR), info.name);
		info.size.Import(statx.st_size);
		struct tm *time = localtime(&statx.st_mtime);
		info.date.Import(1900 + time->tm_year, time->tm_mon + 1, time->tm_mday);
		info.time.Import(time->tm_hour, time->tm_min, time->tm_sec);
	}

	return (error);
}

int ZAF_DIRECTORY_SERVICE::DriveError(int drive)
{
	// NEXTSTEP handles its own drive error handling.
	return (TRUE);
}

int ZAF_DIRECTORY_SERVICE::GetDrive(void)
{
	return (-1);
}

int ZAF_DIRECTORY_SERVICE::SetDrive(int drive)
{
	return (-1);
}

int ZAF_DIRECTORY_SERVICE::SetPath(ZIL_ICHAR *pathName)
{
	return (-1);
}

// --------------------------------------------------------------------------
// ----- Static Member Variables --------------------------------------------
// --------------------------------------------------------------------------

ZIL_ICHAR ZAF_DIRECTORY_SERVICE::_className[] = { 'Z','A','F','_','D','I','R','E','C','T','O','R','Y','_','S','E','R','V','I','C','E',0 };

ZIL_STORAGE_READ_ONLY *ZAF_DIRECTORY_SERVICE::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

static UI_ITEM __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_DIRECTORY_SERVICE::_userTable = __userTable;

static UI_ITEM __objectTable[] =
{
	{ ID_BORDER, ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_COMBO_BOX, ZIL_VOIDF(UIW_COMBO_BOX::New), UIW_COMBO_BOX::_className, 0 },
	{ ID_ICON, ZIL_VOIDF(UIW_ICON::New), UIW_ICON::_className, 0 },
	{ ID_PROMPT, ZIL_VOIDF(UIW_PROMPT::New), UIW_PROMPT::_className, 0 },
	{ ID_SCROLL_BAR, ZIL_VOIDF(UIW_SCROLL_BAR::New), UIW_SCROLL_BAR::_className, 0 },
	{ ID_STRING, ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TEXT, ZIL_VOIDF(UIW_TEXT::New), UIW_TEXT::_className, 0 },
	{ ID_TITLE, ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_VT_LIST, ZIL_VOIDF(UIW_VT_LIST::New), UIW_VT_LIST::_className, 0 },
	{ ID_WINDOW, ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_DIRECTORY_SERVICE::_objectTable = __objectTable;
