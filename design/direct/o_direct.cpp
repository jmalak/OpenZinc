//	O_DIRECT.CPP - File directory service object.
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// General declarations
#define INCL_WINSTDFILE
#define INCL_DOSERRORS
#define USE_MSG_TABLE
#define USE_DRIVE_ERROR
#define USE_ZAF_DIRECTORY_SERVICE
#include "direct.hpp"
#include "p_direct.hpp"

const int ZMSG_TITLES			= 6100 - 0;		// Titles start at 0.
const int ZMSG_ERRORS			= 6100 - 100;	// Errors start at 100.

const EVENT_TYPE OPT_RESET_DRIVE		= 11000;
const EVENT_TYPE OPT_RESET_DIRECTORY	= 11001;
const EVENT_TYPE OPT_RESET_FILE			= 11002;
const EVENT_TYPE OPT_RESET_FILTER		= 11003;

static FILEDLG fileDlg;
#if defined(ZIL_UNICODE)
	static char fileDlgTitle[100];
#endif

// --------------------------------------------------------------------------
// ----- ZAF_DIRECTORY_SERVICE ----------------------------------------------
// --------------------------------------------------------------------------

ZAF_DIRECTORY_SERVICE::ZAF_DIRECTORY_SERVICE(void) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	requestor(ZIL_NULLP(UI_WINDOW_OBJECT)), response(0),
	msgTable(_serviceManager->ZMSG_msgTableName(), _serviceManager->Storage(&_storage, "p_direct"))
{
	// Set defaults.
	searchID = ID_DIRECTORY_SERVICE;
	requestedFile[0] = '\0';
	originalDrive = currentDrive = GetDrive();
	memset(&fileDlg, 0, sizeof(FILEDLG));
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
			ZIL_ICHAR errorFile[CCHMAXPATH];
			ZIL_STORAGE_READ_ONLY::StripFullPath(requestedFile, ZIL_NULLP(ZIL_ICHAR), errorFile);

			// Error messages are indexed by option #.
			ZIL_ICHAR *message = response ?
				msgTable.GetMessage(response - ZMSG_ERRORS) :
				msgTable.GetMessage(ZMSG_FILE_NONE_ERROR);
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, message, errorFile);
		}
		else
			UI_ERROR_STUB::Beep();
	}

	return (event.type);
}

void *ZAF_DIRECTORY_SERVICE::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the storage service.
		{
		fileDlg.cbSize = sizeof(FILEDLG);
		fileDlg.fl = FDS_CENTER | FDS_HELPBUTTON;
		if (response == OPT_FILE_SAVEAS)
			fileDlg.fl |= FDS_SAVEAS_DIALOG | FDS_CENTER | FDS_HELPBUTTON;
		else
			fileDlg.fl |= FDS_OPEN_DIALOG;

		int finished = FALSE;
		while (!finished)
		{
			ZIL_SCREENID taskID;
			windowManager->Information(I_GET_TASKID, &taskID);
			if (WinFileDlg(HWND_DESKTOP, taskID, &fileDlg))
			{
				if (fileDlg.lReturn == DID_OK)
				{
					EVENT_TYPE ccode = 0;

#if defined(ZIL_UNICODE)
					UnMapText(fileDlg.szFullFile, requestedFile);
#else
					strcpy(requestedFile, fileDlg.szFullFile);
#endif
					PathName(requestedFile);

					// Check for a file overwrite.
					if ((response == OPT_FILE_NEW || response == OPT_FILE_SAVEAS || response == OPT_FILE_EXPORT) &&
						ZIL_STORAGE::ValidName(requestedFile, FALSE))
					{
						ZIL_ICHAR *title = response ?
							msgTable.GetMessage(response - ZMSG_TITLES) :
							msgTable.GetMessage(ZMSG_FILE_TNONE);
						ZIL_ICHAR *message = msgTable.GetMessage(ZMSG_FILE_OVERWRITE);
						ZIL_ICHAR fileName[CCHMAXPATH];
						ZIL_STORAGE_READ_ONLY::StripFullPath(requestedFile, ZIL_NULLP(ZIL_ICHAR), fileName);
						ZAF_MESSAGE_WINDOW window(title, ZIL_NULLP(ZIL_ICHAR),
							ZIL_MSG_YES | ZIL_MSG_NO, ZIL_MSG_YES, message, fileName);
						UI_ERROR_STUB::Beep();
						if (window.Control() != ZIL_DLG_YES)
							continue;
					}

					// Check for valid files (service then programmer levels).
					if (!ZIL_STORAGE::ValidName(requestedFile, (response == OPT_FILE_NEW || response == OPT_FILE_SAVEAS || response == OPT_FILE_EXPORT)))
						ccode = S_ERROR;
					else if (requestor)
					{
						eventManager->DeviceState(E_MOUSE, DM_WAIT);
						UI_EVENT rEvent(-response);
						rEvent.data = requestedFile;
						ccode = requestor->Event(rEvent); // Send a response to the requestor.
						eventManager->DeviceState(E_MOUSE, DM_VIEW);
					}

					if (ccode == S_ERROR)
						Event(UI_EVENT(S_ERROR));
					else if (requestor)
					{
						UI_EVENT rEvent(-response);
						rEvent.data = ZIL_NULLP(void);
						requestor->Event(rEvent); // NULL data response tells requestor we're done.

						if (ZAF_SERVICE_MANAGER::_queuedEvent)
							eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);

						finished = TRUE;
					}
				}
				else
				{
					ZAF_SERVICE_MANAGER::_queuedEvent = 0;
					response = 0;
					finished = TRUE;
				}
			}
		}
		}
		break;

	case I_SET_REQUESTOR:
		requestor = (UI_WINDOW_OBJECT *)data;
		break;

	case I_SET_REQUEST:
		// Set the request.
		response = *(EVENT_TYPE *)data;
#if defined(ZIL_UNICODE)
		fileDlg.pszTitle = fileDlgTitle;
		MapText(response ?
			msgTable.GetMessage(response - ZMSG_TITLES) :
			msgTable.GetMessage(ZMSG_FILE_TNONE), fileDlgTitle);
#else
		fileDlg.pszTitle = response ?
			(PSZ)msgTable.GetMessage(response - ZMSG_TITLES) :
			(PSZ)msgTable.GetMessage(ZMSG_FILE_TNONE);
#endif
		break;

	case I_SET_FILTER:
#if defined(ZIL_UNICODE)
		MapText((ZIL_ICHAR *)data, fileDlg.szFullFile);
#else
		strcpy(fileDlg.szFullFile, (ZIL_ICHAR *)data);
#endif
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
	// Get the filename.
#if defined(ZIL_UNICODE)
	UnMapText(fileDlg.szFullFile, pathName);
#else
	strcpy(pathName, fileDlg.szFullFile);
#endif

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

int ZAF_DIRECTORY_SERVICE::DeviceInfo(ZIL_ICHAR *fullName, ZNC_DEVICE_INFO &info)
{
	// Check for NULL pathnames.
	ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
	if (!fullName)
	{
		PathName(pathName);
		fullName = pathName;
	}

	HDIR directory = HDIR_SYSTEM;
	ULONG oneBlock = 1;
	int error = DosFindFirst((PSZ)fullName, &directory, FILE_DIRECTORY,
		&info._reserved, sizeof(info._reserved), &oneBlock, FIL_STANDARD) ? -1 : 0;
	if (!error)
	{
		info.classification = (info._reserved.attrFile == FILE_DIRECTORY) ? DEVICE_DIRECTORY : DEVICE_FILE;
#if defined(ZIL_UNICODE)
		UnMapText(info._reserved.achName, info.name);
#else
		strcpy(info.name, info._reserved.achName);
#endif
		strlwr(info.name);
		info.size.Import((long)info._reserved.cbFile);
		info.date.Import(1980 + info._reserved.fdateLastWrite.year,
			info._reserved.fdateLastWrite.month,
			info._reserved.fdateLastWrite.day);
		info.time.Import(info._reserved.ftimeLastWrite.hours,
			info._reserved.ftimeLastWrite.minutes,
			info._reserved.ftimeLastWrite.twosecs * 2);
	}

	return (error);
}

int ZAF_DIRECTORY_SERVICE::DriveError(int)
{
	// OS/2 does its own drive error handling.
	return (TRUE);
}

int ZAF_DIRECTORY_SERVICE::GetDrive(void)
{
	ULONG drive, logical;
	DosQueryCurrentDisk(&drive, &logical);
	return (drive - 1);
}

int ZAF_DIRECTORY_SERVICE::SetDrive(int drive)
{
	ULONG temp, logical;
	DosQueryCurrentDisk(&temp, &logical);
	if (DosSetDefaultDisk(drive + 1) == ERROR_INVALID_DRIVE)
	{
		DosSetDefaultDisk(temp);
		return (-1);
	}
	return (0);
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

