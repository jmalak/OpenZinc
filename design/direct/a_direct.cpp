//	A_DIRECT.CPP - File directory service object.
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// General declarations
#define USE_DRIVE_ERROR
#define USE_ZAF_DIRECTORY_SERVICE
#include "direct.hpp"
#include "p_direct.hpp"

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
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the storage service.
		{
		Point where = { 0, 0 };
		SFReply reply;
		if (response == OPT_FILE_NEW || response == OPT_FILE_SAVEAS || response == OPT_FILE_EXPORT)
		{
			char *tBuff = MapText(currentFilename);
			SFPutFile(where, ZIL_NULLP(Byte), c2pstr(tBuff), ZIL_NULLF(DlgHookUPP), &reply);
			delete tBuff;
		}
		else
		{
			SFTypeList typeList;
			SFGetFile(where, ZIL_NULLP(Byte), ZIL_NULLF(FileFilterUPP), -1, typeList,
				ZIL_NULLF(DlgHookUPP), &reply);
		}
		if (reply.good)
		{
			WDPBRec wdpb;
			wdpb.ioNamePtr = 0;
			wdpb.ioVRefNum = reply.vRefNum;
			wdpb.ioWDIndex = 0;
			wdpb.ioWDProcID = 0L;
			wdpb.ioWDVRefNum = 0;
			if (PBGetWDInfo(&wdpb, FALSE) == noErr)
			{
				currentVRefNum = wdpb.ioVRefNum;
				currentDirID = wdpb.ioWDDirID;
				ZIL_ICHAR *_currentFilename = UnMapText(p2cstr(reply.fName));
				strcpy(currentFilename, _currentFilename);
				delete _currentFilename;

				// Send a response to the requestor.
				PathName(requestedFile);
				if (requestor)
				{
					UI_WINDOW_OBJECT::eventManager->DeviceState(E_MOUSE, DM_WAIT);
					UI_EVENT rEvent(-response);
					rEvent.data = requestedFile;
					requestor->Event(rEvent);
					UI_WINDOW_OBJECT::eventManager->DeviceState(E_MOUSE, DM_VIEW);
				}

				// Set the correct Macintosh file type for newly-created files.
				if (response == OPT_FILE_NEW || response == OPT_FILE_SAVEAS || response == OPT_FILE_EXPORT)
				{
					FSSpec fileSpec;
					FInfo fileInfo;
					Str255 fsFilename;
					GetPathNameFromDirID(fsFilename, currentDirID, currentVRefNum);
					pstrcat(fsFilename, c2pstr((char *)reply.fName));
					FSMakeFSSpec(0, 0, fsFilename, &fileSpec);
					if (FSpGetFInfo(&fileSpec, &fileInfo) == noErr)
					{
						fileInfo.fdType = 'TEXT';
						FSpSetFInfo(&fileSpec, &fileInfo);
					}
				}
				if (requestor)
				{
					UI_EVENT rEvent(-response);
					requestor->Event(rEvent); // NULL data response tells requestor we're done.
				}
				if (ZAF_SERVICE_MANAGER::_queuedEvent)
					eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
				ZAF_SERVICE_MANAGER::_queuedEvent = 0;
				requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
				response = 0;
				break;
			}
		}
		else
			ZAF_SERVICE_MANAGER::_queuedEvent = 0;
		currentFilename[0] = 0;
		currentVRefNum = 0;
		currentDirID = 0L;
		if (requestor)
		{
			UI_EVENT rEvent(-response);
			requestor->Event(rEvent); // NULL data response tells requestor we're done.
		}
		requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
		response = 0;
		}
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
	if (currentFilename[0])
	{
#if defined(ZIL_UNICODE)
		// Get the full path (all but the filename).
		Str255 mbName;
		GetPathNameFromDirID(mbName, currentDirID, currentVRefNum);
		p2cstr((StringPtr)mbName);

		// Convert the path to a Unicode string.
		ConvertFromFilename(pathName, (ZIL_FILE_CHAR *)mbName);
#else
		// Get the full path (all but the filename).
		GetPathNameFromDirID((StringPtr)pathName, currentDirID, currentVRefNum);
		p2cstr((StringPtr)pathName);
#endif

		// Get the filename and check for default extensions.
		extern ZIL_ICHAR *_zilExtName;
		ZIL_ICHAR *extension = _zilExtName;
		UI_WINDOW_OBJECT *service = _serviceManager->Get(ID_STORAGE_SERVICE);
		if (service)
			service->Information(I_GET_DEFAULT_EXTENSION, &extension);
		if (!strchr(currentFilename, '.'))
			strcat(currentFilename, extension);

		// Add the filename to the full path.
		strcat(pathName, currentFilename);
	}
	else
		pathName[0] = 0;

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

	int error = -1;
	Str255 pFilename;
	char *_pFilename = MapText(fullName);
	::strcpy((char *)pFilename, _pFilename);
	delete _pFilename;
	c2pstr((char *)pFilename);
	HFileInfo *fpb = (HFileInfo *)&info._reserved;
	DirInfo *dpb = (DirInfo *)&info._reserved;
	fpb->ioNamePtr = pFilename;
	fpb->ioFDirIndex = 0;
	fpb->ioVRefNum = currentVRefNum;
	fpb->ioDirID = currentDirID;

	if (PBGetCatInfo(&info._reserved, FALSE) == noErr)
	{
		DateTimeRec lDate;
		if (fpb->ioFlAttrib & 0x10)
		{
			info.classification = DEVICE_DIRECTORY;
			info.size.Import((ZIL_INT32)0);
			Secs2Date(dpb->ioDrMdDat, &lDate);
		}
		else
		{
			info.classification = DEVICE_FILE;
			info.size.Import((ZIL_INT32)(fpb->ioFlLgLen + fpb->ioFlRLgLen));
			Secs2Date(fpb->ioFlMdDat, &lDate);
		}
		p2cstr(pFilename);
		ZIL_ICHAR *_fullName = UnMapText((char *)pFilename);
		strcpy(currentFilename, _fullName);
		strcpy(info.name, _fullName);
		delete _fullName;
		info.date.Import(lDate.year, lDate.month, lDate.day);
		info.time.Import(lDate.hour, lDate.minute, lDate.second);
		error = 0;
	}

	return (error);
}

int ZAF_DIRECTORY_SERVICE::DriveError(int)
{
	// Macintosh handles its own drive error handling.
	return (TRUE);
}

int ZAF_DIRECTORY_SERVICE::GetDrive(void)
{
	long freeBytes;
	Str255 volName;
	if (GetVInfo(0, (StringPtr)&volName, &currentVRefNum, &freeBytes) == noErr)
		return (currentVRefNum);

	return (0);
}

int ZAF_DIRECTORY_SERVICE::SetDrive(int)
{
	return (-1);
}

int ZAF_DIRECTORY_SERVICE::SetPath(ZIL_ICHAR *)
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
