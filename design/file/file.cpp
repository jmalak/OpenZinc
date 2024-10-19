//	Zinc Interface Library - FILE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__MWERKS__)
#	include <unix.h>
#else
#	include <fcntl.h>
#endif
#include "file.hpp"
#define USE_FILE_EDITOR
#include "p_file.hpp"

// -- format requests ---
const EVENT_TYPE OPT_FORMAT_CODE_PAGE_850	= 7000;
const EVENT_TYPE OPT_FORMAT_ISO_8859_1		= 7001;
const EVENT_TYPE OPT_FORMAT_BIG_FIVE		= 7002;
const EVENT_TYPE OPT_FORMAT_SHIFT_JIS		= 7003;
const EVENT_TYPE OPT_FORMAT_UNICODE			= 7004;

// --------------------------------------------------------------------------
// ----- ZAF_FILE_EDITOR ----------------------------------------------------
// --------------------------------------------------------------------------

ZAF_FILE_EDITOR::ZAF_FILE_EDITOR(WOAF_FLAGS _woAdvancedFlags,
	WOS_STATUS _woStatus) :
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_file"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable)
{
	// Set the storage services.
	searchID = ID_FILE_EDITOR;
	woStatus |= _woStatus;
	woAdvancedFlags |= _woAdvancedFlags;
	currentFile[0] = '\0';
	directoryService = _serviceManager->Get(ID_DIRECTORY_SERVICE);
	Information(I_COPY_TEXT, originalTitle);

	// Center the window on the display.
	windowManager->Center(this);
	relative.bottom = relative.Height() - 1;
	relative.top = 0;

	// Create and initialize the text field.
	textField = new UIW_TEXT(0, 0, 0, 0, ZIL_NULLP(ZIL_ICHAR), 1024,
		WNF_NO_WRAP, WOF_NON_FIELD_REGION);
	textField->Add(new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER));
	textField->Add(new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL));
	textField->Add(new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL));
	Add(textField);

	// Initialize the unicode, code page and code set variables.
	unicode = FALSE;
	codePage = 850;
	strcpy(codeSet, _blankString);

	// Remove the File option if this is a child-service.
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
	{
		UI_WINDOW_OBJECT *menuItem = Get(_serviceManager->ZMSG_fileOption());
		menuItem->woFlags |= WOF_NON_SELECTABLE;
	}
	SetTitle();
}

ZAF_FILE_EDITOR::~ZAF_FILE_EDITOR(void)
{
	// Remove the resource storage.
	if (_storage)
		delete _storage;
	FileClose(0);
}

EVENT_TYPE ZAF_FILE_EDITOR::Event(const UI_EVENT &event)
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
	case S_RESTORE:
		ccode = UIW_WINDOW::Event(event);
		SetTitle();
		break;

	case OPT_ACTIVATE_SERVICE:
		ccode = _serviceManager->Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	return (ccode);
}

void *ZAF_FILE_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ORIGINAL_TITLE)
	{
		if (!data)
			return (originalTitle);
		*(ZIL_ICHAR **)data = originalTitle;
	}
	else
		data = UIW_WINDOW::Information(request, data, objectID);

	return (data);
}

EVENT_TYPE ZAF_FILE_EDITOR::Format(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	unicode = (ccode == OPT_FORMAT_UNICODE) ? TRUE : FALSE;
	codePage = (ccode == OPT_FORMAT_CODE_PAGE_850) ? 850 : 0;
	if (ccode == OPT_FORMAT_BIG_FIVE || ccode == OPT_FORMAT_SHIFT_JIS)
		strcpy(codeSet, event.windowObject->StringID());
	else
		codeSet[0] = '\0';
	MNIF_FLAGS mniFlag = MNIF_CHECK_MARK;
	event.windowObject->Information(I_SET_FLAGS, &mniFlag, ID_POP_UP_ITEM);
	event.windowObject->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

	return (event.type);
}

void ZAF_FILE_EDITOR::SetTitle(void)
{
	static ZIL_ICHAR _format[] = { '%','s',' ','-',' ','%','s',' ','-',' ','%','s',0 };

	// Check for a file and make sure it isn't a child-service (locked).
	ZIL_ICHAR fullTitle[ZIL_MAXPATHLEN];
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
		strcpy(fullTitle, originalTitle);
	else if (currentFile[0])
	{
		ZIL_ICHAR fileName[ZIL_MAXPATHLEN];
		ZIL_STORAGE::StripFullPath(currentFile, ZIL_NULLP(ZIL_ICHAR), fileName);
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, fileName);
	}
	else
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, _serviceManager->ZMSG_noFile());
	Information(I_SET_TEXT, fullTitle);
}

// ----- File options -------------------------------------------------------

EVENT_TYPE ZAF_FILE_EDITOR::FileClose(const UI_EVENT &event)
{
	// Close the edit file by re-setting currentFile.
	textField->DataSet(_blankString);
	currentFile[0] = '\0';
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_FILE_EDITOR::FileDelete(const UI_EVENT &event)
{
	// Delete the specified file.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	if (unlink(pathName))
		return (directoryService->Event(S_ERROR));
	return (event.type);
}

EVENT_TYPE ZAF_FILE_EDITOR::FileNew(const UI_EVENT &event)
{
	// Create the specified file by re-setting the text.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	textField->DataSet(_blankString);

	strcpy(currentFile, pathName);
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_FILE_EDITOR::FileOpen(const UI_EVENT &event)
{
	// Try to open the specified file.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_FILE rFile(pathName, UIS_READ | UIS_BINARY);
	if (rFile.GetError())
		return (directoryService->Event(S_ERROR));
	strcpy(currentFile, pathName);

	int length = (int)rFile.Length();
	char *isoText = new char[length+1];
	length = rFile.Read(isoText, length, 1);
	isoText[length] = '\0';

	ZIL_ICHAR *unicodeText = new ZIL_ICHAR[length+1];
	if (unicode) // Unicode
	{
		memcpy(unicodeText, isoText, length);
		unicodeText[length/sizeof(ZIL_ICHAR)] = '\0';
	}
	else if (*codeSet) // Mixed-byte (Big Five, Shift JIS)
	{
		ZIL_MAP_CHARS map(codeSet, ZIL_NULLP(ZIL_ICHAR));
		map.UnMapText(isoText, unicodeText, FALSE);
	}
	else if (codePage) // Code-page 850
	{
		UnMapText(isoText, unicodeText, FALSE);
	}
	else // ISO 8859-1
	{
#if defined(ZIL_UNICODE)
		ISOtoUNICODE(isoText, unicodeText);
#else
		memcpy(unicodeText, isoText, length);
		unicodeText[length] = '\0';
#endif
	}

	length = strlen(unicodeText);
	textField->DataSet(unicodeText, length + 2048);

	delete unicodeText;
	delete isoText;

	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_FILE_EDITOR::FileRequest(const UI_EVENT &event)
{
	if (!directoryService)
		return (S_UNKNOWN);

	// Make a request to the directory service.
	directoryService->Information(I_SET_FILTER, _serviceManager->ZMSG_allFilter());
	directoryService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	directoryService->Information(I_SET_REQUEST, &ccode);
	directoryService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_FILE_EDITOR::FileSave(const UI_EVENT &event)
{
	// Check for a valid file.
	ZIL_FILE wFile(currentFile, UIS_CREATE | UIS_READWRITE | UIS_BINARY);
	if (wFile.GetError())
		return (S_ERROR);

	ZIL_ICHAR *unicodeText = textField->DataGet();
	if (unicode) // Unicode
		wFile.Write(unicodeText);
	else if (*codeSet) // Mixed-byte (Big Five, Shift JIS)
	{
		ZIL_MAP_CHARS map(codeSet, ZIL_NULLP(ZIL_ICHAR));
		char *text = map.MapText(unicodeText);
		wFile.Write(text);
		delete text;
	}
	else if (codePage) // Code-page 850
	{
		char *text = MapText(unicodeText);
		wFile.Write(text);
		delete text;
	}
	else // ISO 8859-1
		wFile.Write(unicodeText);

	return (event.type);
}

EVENT_TYPE ZAF_FILE_EDITOR::FileSaveAs(const UI_EVENT &event)
{
	// Save the specified file.
	if (event.data)
		strcpy(currentFile, (ZIL_ICHAR *)event.data);
	else
		currentFile[0] = 0;

	EVENT_TYPE ccode = FileSave(event);
	if (event.data && ccode != S_ERROR)
		SetTitle();

	return (ccode);
}

// ----- Static tables ------------------------------------------------------

ZIL_ICHAR ZAF_FILE_EDITOR::_className[] = { 'Z','A','F','_','F','I','L','E','_','E','D','I','T','O','R',0 };

ZIL_STORAGE_READ_ONLY *ZAF_FILE_EDITOR::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_FILE_EDITOR::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_MAXIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MAXIMIZE_BUTTON::New), UIW_MAXIMIZE_BUTTON::_className, 0 },
	{ ID_MINIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MINIMIZE_BUTTON::New), UIW_MINIMIZE_BUTTON::_className, 0 },
	{ ID_POP_UP_ITEM, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_ITEM::New), UIW_POP_UP_ITEM::_className, 0 },
	{ ID_POP_UP_MENU, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_MENU::New), UIW_POP_UP_MENU::_className, 0 },
	{ ID_PULL_DOWN_ITEM, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_ITEM::New), UIW_PULL_DOWN_ITEM::_className, 0 },
	{ ID_PULL_DOWN_MENU, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_MENU::New), UIW_PULL_DOWN_MENU::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_FILE_EDITOR::_objectTable = __objectTable;

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_FILE_EDITOR::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_FILE_EDITOR::PROCESS ZAF_FILE_EDITOR::_processTable[] =
{
	{ OPT_FILE_CLOSE,				ZIL_PROCESS_REFERENCE(FileClose) },		// File requests.
	{ OPT_FILE_DELETE, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_SAVE, 				ZIL_PROCESS_REFERENCE(FileSave) },
	{ OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ -OPT_FILE_DELETE, 			ZIL_PROCESS_REFERENCE(FileDelete) },		// File responses.
	{ -OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileNew) },
	{ -OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileOpen) },
	{ -OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileSaveAs) },

	{ OPT_FORMAT_CODE_PAGE_850,		ZIL_PROCESS_REFERENCE(Format) },
	{ OPT_FORMAT_ISO_8859_1, 		ZIL_PROCESS_REFERENCE(Format) },
	{ OPT_FORMAT_BIG_FIVE, 			ZIL_PROCESS_REFERENCE(Format) },
	{ OPT_FORMAT_SHIFT_JIS, 		ZIL_PROCESS_REFERENCE(Format) },
	{ OPT_FORMAT_UNICODE, 			ZIL_PROCESS_REFERENCE(Format) },

	{ 0, 0 }
};
