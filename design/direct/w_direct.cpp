//	W_DIRECT.CPP - File directory service object.
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// General declarations
#define USE_MSG_TABLE
#define USE_DRIVE_ERROR
#define USE_ZAF_DIRECTORY_SERVICE
#include "direct.hpp"
#include "p_direct.hpp"
#include <commdlg.h>

#if defined(ZIL_WINNT)
#	define TIME_DAY 				8640000L
#endif

const int ZMSG_TITLES			= 6100 - 0;		// Titles start at 0.
const int ZMSG_ERRORS			= 6100 - 100;	// Errors start at 100.

const EVENT_TYPE OPT_RESET_DRIVE		= 11000;
const EVENT_TYPE OPT_RESET_DIRECTORY	= 11001;
const EVENT_TYPE OPT_RESET_FILE			= 11002;
const EVENT_TYPE OPT_RESET_FILTER		= 11003;

static OPENFILENAME _fileDlg;
#if defined(ZIL_UNICODE) && defined(ZIL_WINNT)
static wchar_t _filter[] = L"Data Files (*.dat)\0*.dat\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
#else
static char _filter[] = "Data Files (*.dat)\0*.dat\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
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

	_fileDlg.lStructSize = sizeof(_fileDlg);
	_fileDlg.hwndOwner = 0;
	_fileDlg.hInstance = 0;
	_fileDlg.lpstrFilter = _filter;
	_fileDlg.lpstrCustomFilter = 0;
	_fileDlg.nMaxCustFilter = 0;
	_fileDlg.nFilterIndex = 0;
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
	_fileDlg.lpstrFile = new char[ZIL_MAXPATHLEN];
	MapText(requestedFile, _fileDlg.lpstrFile);
	_fileDlg.nMaxFile = ZIL_MAXPATHLEN;
	_fileDlg.lpstrFileTitle =0;// _fileDlg.lpstrFile;
	_fileDlg.nMaxFileTitle = _fileDlg.nMaxFile;
#else
	_fileDlg.lpstrFile = requestedFile;
	_fileDlg.lpstrFileTitle = 0;//requestedFile;
	_fileDlg.nMaxFile = sizeof(requestedFile);
	_fileDlg.nMaxFileTitle = sizeof(requestedFile);
#endif
	_fileDlg.lpstrInitialDir = 0;
	_fileDlg.lpstrTitle = 0;
	_fileDlg.Flags = OFN_HIDEREADONLY;
	_fileDlg.nFileOffset = 0;
	_fileDlg.nFileExtension = 0;
	_fileDlg.lpstrDefExt = 0;
	_fileDlg.lCustData = 0;
	_fileDlg.lpfnHook = 0;
	_fileDlg.lpTemplateName = 0;
}

ZAF_DIRECTORY_SERVICE::~ZAF_DIRECTORY_SERVICE(void)
{
	// Restore the original drive and directory.
	SetDrive(originalDrive);
	SetPath(originalPath);

	// Remove the resource storage.
	if (_storage)
		delete _storage;
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
	delete _fileDlg.lpstrFile;
	delete (char *)_fileDlg.lpstrTitle;
#endif
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
			ZIL_ICHAR errorFile[256];
			ZIL_STORAGE_READ_ONLY::StripFullPath(requestedFile, ZIL_NULLP(ZIL_ICHAR), errorFile);

			// Error messages are indexed by option #.
			ZIL_ICHAR *message = response ?
				msgTable.GetMessage((int)(response - ZMSG_ERRORS)) :
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
		if (requestor)
		{
			UI_EVENT rEvent(-response);
			UI_WINDOW_OBJECT::eventManager->DeviceState(E_MOUSE, DM_WAIT);
			_fileDlg.hwndOwner = requestor->screenID;
			if ((response == OPT_FILE_OPEN && GetOpenFileName(&_fileDlg)) ||
				(response != OPT_FILE_OPEN && GetSaveFileName(&_fileDlg)))
			{
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
				UnMapText(_fileDlg.lpstrFile, requestedFile);
#endif
				rEvent.data = requestedFile;
				requestor->Event(rEvent);
			}
			rEvent.data = ZIL_NULLP(void);
			requestor->Event(rEvent); // NULL data response tells requestor we're done.
			UI_WINDOW_OBJECT::eventManager->DeviceState(E_MOUSE, DM_VIEW);
			_fileDlg.hwndOwner = 0;
		}
		break;

	case I_SET_REQUESTOR:
		requestor = (UI_WINDOW_OBJECT *)data;
		break;

	case I_SET_REQUEST:
		// Set the request.
		response = *(EVENT_TYPE *)data;
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
		{
		ZIL_ICHAR *tmp = response ?
			msgTable.GetMessage((int)(response - ZMSG_TITLES)) :
			msgTable.GetMessage(ZMSG_FILE_TNONE);
		_fileDlg.lpstrTitle = MapText(tmp);
		}
#else
		_fileDlg.lpstrTitle = response ?
			msgTable.GetMessage((int)(response - ZMSG_TITLES)) :
			msgTable.GetMessage(ZMSG_FILE_TNONE);
#endif
		break;

	case I_SET_FILTER:
		strcpy(requestedFile, (ZIL_ICHAR *)data);
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

#if defined(ZIL_WINNT)
	HANDLE directory = FindFirstFile(fullName, &info._reserved);
	int error = (directory == INVALID_HANDLE_VALUE) ? -1 : 0;
	if (!error)
	{
		info.classification = (info._reserved.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.cFileName);
		info.size.Import((long)info._reserved.nFileSizeLow);
		strlwr(info.name);
		WORD dateCode, timeCode;
		FileTimeToDosDateTime(&info._reserved.ftLastWriteTime, &dateCode, &timeCode);

		// Adjust for GMT.
		TIME_ZONE_INFORMATION tZInfo;
		GetTimeZoneInformation(&tZInfo);
		info.date.Import(dateCode);
		info.time.Import(timeCode);
		ZIL_TIME bias;
		int cYear, cMonth, cDay;
		ZIL_DATE cDate;
		cDate.Export(&cYear, &cMonth, &cDay);
		if ((cMonth > tZInfo.DaylightDate.wMonth ||
			cMonth == tZInfo.DaylightDate.wMonth && cDay >= tZInfo.DaylightDate.wDay) &&
			(cMonth < tZInfo.StandardDate.wMonth ||
			cMonth == tZInfo.StandardDate.wMonth && cDay < tZInfo.StandardDate.wDay))
				tZInfo.Bias += tZInfo.DaylightBias;
		else
			tZInfo.Bias += tZInfo.StandardBias;
		if (tZInfo.Bias < 0)
		{
			ZIL_TIME diff;
			bias.Import(-tZInfo.Bias / 60, -tZInfo.Bias % 60);
			diff = ZIL_TIME(23, 59, 59, 99) - bias;
			if (diff < info.time)
			{
				++info.date;
				info.time -= TIME_DAY;
			}
			info.time = info.time + bias;
		}
		else
		{
			bias.Import(tZInfo.Bias / 60, tZInfo.Bias % 60);
			if (bias > info.time)
			{
				--info.date;
				info.time += TIME_DAY;
			}
			info.time = info.time - bias;
		}
	}
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	char *_fullName = MapText(fullName);
	int error = ::findfirst(_fullName, &info._reserved, FA_DIREC);
	if (!error)
	{
		info.classification = (info._reserved.ff_attrib == FA_DIREC) ? DEVICE_DIRECTORY : DEVICE_FILE;
		ZIL_ICHAR _name[ZIL_MAXPATHLEN];
		UnMapText(info._reserved.ff_name, _name);
		strcpy(info.name, _name);
		strlwr(info.name);
		info.size.Import(info._reserved.ff_fsize);
		info.date.Import(info._reserved.ff_fdate);
		info.time.Import(info._reserved.ff_ftime);
	}
	delete _fullName;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
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
#elif defined(__ZTC__)
	info._reserved = ::findfirst(fullName, FA_DIREC);
	int error = info._reserved ? 0 : -1;
	if (!error)
	{
		info.classification = (info._reserved->attribute == FA_DIREC) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved->name);
		strlwr(info.name);
		info.size.Import((long)info._reserved->size);
		info.date.Import(info._reserved->date);
		info.time.Import(info._reserved->time);
	}
#endif

	return (error);
}

int ZAF_DIRECTORY_SERVICE::DriveError(int)
{
	// OS/2 does its own drive error handling.
	return (TRUE);
}

int ZAF_DIRECTORY_SERVICE::GetDrive(void)
{
#if defined(ZIL_WINNT)
	ZIL_ICHAR currentDrive[ZIL_MAXPATHLEN];
	GetCurrentDirectory(ZIL_MAXPATHLEN, currentDrive);
	return (DriveCharToInt(*currentDrive));
#else
	unsigned drive;
	_dos_getdrive(&drive);
	return (drive - 1);
#endif
}

int ZAF_DIRECTORY_SERVICE::SetDrive(int drive)
{
#if defined(ZIL_WINNT)
	ZIL_ICHAR newDrive[ZIL_MAXPATHLEN];
	ZIL_ICHAR format[] = { '%','c',':', 0 };
	sprintf(newDrive, format, DriveIntToChar(drive));
	SetCurrentDirectory(newDrive);
	GetCurrentDirectory(ZIL_MAXPATHLEN, newDrive);
	if (DriveCharToInt(*newDrive) == drive)
		return (0);
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

