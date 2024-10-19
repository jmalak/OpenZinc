//	Zinc Interface Library - I18N.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <storage.hpp>
#include "i18n.hpp"

ZIL_ICHAR *ZAF_I18N_EDITOR::_i18nDirectory = ZIL_NULLP(ZIL_ICHAR);
ZIL_ICHAR ZAF_I18N_EDITOR::_languageDirectory[] = { 'L','A','N','G','U','A','G','E',0 };
ZIL_ICHAR ZAF_I18N_EDITOR::_localeDirectory[] = { 'L','O','C','A','L','E',0 };
ZIL_ICHAR *ZAF_I18N_EDITOR::_defaultLanguage = ZIL_NULLP(ZIL_ICHAR);
ZIL_ICHAR *ZAF_I18N_EDITOR::_defaultLocale = ZIL_NULLP(ZIL_ICHAR);

extern ZIL_ICHAR *_parentDirectory;
extern ZIL_ICHAR *_currentDirectory;

const EVENT_TYPE OPT_LANGUAGE_CREATE		= 6310;
const EVENT_TYPE OPT_LOCALE_CREATE			= 6311;

// --------------------------------------------------------------------------
// ----- ZAF_I18N_EDITOR ----------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__DECCXX)
ZAF_I18N_EDITOR::ZAF_I18N_EDITOR() : UIW_WINDOW(0, 0, 0, 0) {}
#endif

ZAF_I18N_EDITOR::ZAF_I18N_EDITOR(WOAF_FLAGS _woAdvancedFlags,
	WOS_STATUS _woStatus) :
	// base initialization
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_i18n"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable)
{
	// Set the services.
	searchID = ID_I18N_EDITOR;
	woStatus |= _woStatus;
	woAdvancedFlags |= _woAdvancedFlags;
	directoryService = _serviceManager->Get(ID_DIRECTORY_SERVICE);
	storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	Information(I_COPY_TEXT, originalTitle);
	StatusUpdate();

	// Center the window on the display.
	windowManager->Center(this);
	relative.bottom = relative.Height() - 1;
	relative.top = 0;

	// Initialize the default language/locale service.
	if (!_defaultLanguage)
		_defaultLanguage = strdup(languageManager.defaultName);
	if (!_defaultLocale)
		_defaultLocale = strdup(localeManager.defaultName);

	// Remove the File option if this is a child-service.
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
	{
		UI_WINDOW_OBJECT *menuItem = Get(_serviceManager->ZMSG_fileOption());
		menuItem->woFlags |= WOF_NON_SELECTABLE;
	}
	SetTitle();
}

ZAF_I18N_EDITOR::~ZAF_I18N_EDITOR(void)
{
	// Remove the resource storage.
	if (_storage)
		delete _storage;
	delete _defaultLanguage;
	_defaultLanguage = ZIL_NULLP(ZIL_ICHAR);
	delete _defaultLocale;
	_defaultLocale = ZIL_NULLP(ZIL_ICHAR);
}

EVENT_TYPE ZAF_I18N_EDITOR::Event(const UI_EVENT &event)
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

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(_storage, FALSE);
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

void *ZAF_I18N_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the i18n service.
		*windowManager + this;
		if (FlagSet(woStatus, WOS_MINIMIZED))
		{
			// Restore the minimized window.
			UI_EVENT restoreEvent(L_RESTORE);
			restoreEvent.windowObject = this;
			Event(restoreEvent);
		}
		break;

	case I_STATUS_UPDATE:
	case I_STATUS_CLEAR:
		StatusUpdate((UI_WINDOW_OBJECT *)data, (request == I_STATUS_CLEAR));
		break;

	case I_GET_ORIGINAL_TITLE:
		if (!data)
			return (originalTitle);
		*(ZIL_ICHAR **)data = originalTitle;
		break;

	// ----- Base class requests -----
	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}
	return (data);
}

UI_WINDOW_OBJECT *ZAF_I18N_EDITOR::GetDefaultsWindow(void)
{
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_I18N_WINDOW)
			return (window);
	return (ZIL_NULLP(UI_WINDOW_OBJECT));
}

void ZAF_I18N_EDITOR::SetTitle(void)
{
	static ZIL_ICHAR _format[] = { '%','s',' ','-',' ','%','s',' ','-',' ','%','s',0 };
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService)
		storageService->Information(I_GET_STORAGE, &defaultsFile);

	// Check for a file and make sure it isn't a child-service (locked).
	ZIL_ICHAR fullTitle[ZIL_MAXPATHLEN];
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
		strcpy(fullTitle, originalTitle);
	else if (defaultsFile)
	{
		ZIL_ICHAR pathName[ZIL_MAXPATHLEN], fileName[ZIL_MAXPATHLEN];
		defaultsFile->StorageName(pathName);
		defaultsFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), fileName);
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, fileName);
	}
	else
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, _serviceManager->ZMSG_noFile());
	Information(I_SET_TEXT, fullTitle);
}

void ZAF_I18N_EDITOR::StatusUpdate(UI_WINDOW_OBJECT *editObject, int clear)
{
	// ----- Modify the menu items. -----
	UI_WINDOW_OBJECT *field;
	WOF_FLAGS flag = WOF_NON_SELECTABLE;

	// Get the i18n type.
	I18_FLAGS i18Flags = I18_NO_FLAGS;
	if (clear)
		editObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	else if (!editObject)
		editObject = GetDefaultsWindow();
	if (editObject)
		editObject->Information(I_GET_FLAGS, &i18Flags);

	const ZIL_NUMBERID FLD_LNG_STORE = 0x004D;
	field = Get(FLD_LNG_STORE);
	if ((i18Flags == I18_LANGUAGE_SET && FlagSet(field->woFlags, flag)) || (i18Flags != I18_LANGUAGE_SET && !FlagSet(field->woFlags, flag)))
	{
		INFO_REQUEST request = (i18Flags == I18_LANGUAGE_SET) ? I_CLEAR_FLAGS : I_SET_FLAGS;
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID FLD_LNG_STORE_AS = 0x004E;
		field = Get(FLD_LNG_STORE_AS);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID FLD_LNG_CLEAR = 0x0068;
		field = Get(FLD_LNG_CLEAR);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID FLD_LNG_CLEARALL = 0x0069;
		field = Get(FLD_LNG_CLEARALL);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);
	}

	const ZIL_NUMBERID FLD_LOC_STORE = 0x0054;
	field = Get(FLD_LOC_STORE);
	if ((i18Flags == I18_LOCALE_SET && FlagSet(field->woFlags, flag)) || (i18Flags != I18_LOCALE_SET && !FlagSet(field->woFlags, flag)))
	{
		INFO_REQUEST request = (i18Flags == I18_LOCALE_SET) ? I_CLEAR_FLAGS : I_SET_FLAGS;
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID FLD_LOC_STORE_AS = 0x0055;
		field = Get(FLD_LOC_STORE_AS);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID FLD_LOC_CLEAR = 0x006A;
		field = Get(FLD_LOC_CLEAR);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID FLD_LOC_CLEARALL = 0x006B;
		field = Get(FLD_LOC_CLEARALL);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);
	}
}

// ----- File Options -------------------------------------------------------

EVENT_TYPE ZAF_I18N_EDITOR::FileClose(const UI_EVENT &event)
{
	// Make sure there is a defaults file.
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
		return (directoryService->Event(S_ERROR));

	// Close the defaults file.
	delete defaultsFile;
	storageService->Information(I_SET_STORAGE, ZIL_NULLP(ZIL_STORAGE));
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::FileDelete(const UI_EVENT &event)
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

EVENT_TYPE ZAF_I18N_EDITOR::FileNew(const UI_EVENT &event)
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

	// See if there is a pre-existing defaults file.
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &defaultsFile))
		delete defaultsFile;
	defaultsFile = newFile;
	storageService->Information(I_SET_STORAGE, defaultsFile);
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::FileOpen(const UI_EVENT &event)
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

	// See if there is a pre-existing defaults file.
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &defaultsFile))
		delete defaultsFile;
	defaultsFile = openFile;
	storageService->Information(I_SET_STORAGE, defaultsFile);
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::FileRequest(const UI_EVENT &event)
{
	// Make sure there is a directory service.
	if (!directoryService)
		return (S_UNKNOWN);

	// Make a request to the directory service.
	directoryService->Information(I_SET_FILTER, _serviceManager->ZMSG_datFilter());
	directoryService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	directoryService->Information(I_SET_REQUEST, &ccode);
	directoryService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::FileSave(const UI_EVENT &event)
{
	// Make sure there is a defaults file.
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
		return (directoryService->Event(S_ERROR));

	// Make sure the defaults file has a valid name.
	ZIL_ICHAR name[ZIL_MAXPATHLEN], pathName[ZIL_MAXPATHLEN];
	defaultsFile->StorageName(pathName);
	defaultsFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);
	if (!stricmp(name, _serviceManager->ZMSG_tempFileName()))
		return (Event(OPT_FILE_SAVEAS));

	// Save the current message file with all the message windows.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_I18N_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), defaultsFile);
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	defaultsFile->Save(revisions);
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(L_EXIT);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::FileSaveAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data && event.type == -OPT_FILE_SAVEAS)
		return (event.type);

	// See if there is a pre-existing defaults file.
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
		return (directoryService->Event(S_ERROR));
	else if (event.type == OPT_FILE_SAVEAS)
		return (FileRequest(event));

	// Save the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_I18N_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), defaultsFile);
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	if (defaultsFile->SaveAs(pathName, revisions))
	{
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		return (directoryService->Event(S_ERROR));
	}
	SetTitle();
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(L_EXIT);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Defaults Options ----------------------------------------------------

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsClear(const UI_EVENT &event)
{
	// Check for an defaults window.
	UI_WINDOW_OBJECT *defaultsWindow = GetDefaultsWindow();
	if (!defaultsWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of existing window.
	UI_EVENT cEvent(OPT_RESOURCE_CLEAR);
	cEvent.data = defaultsWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the first edit window.
	{
		ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &defaultsFile);
		defaultsWindow->Store(defaultsWindow->StringID(), defaultsFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear the first defaults window.
	*windowManager - defaultsWindow;
	delete defaultsWindow;
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsClearAll(const UI_EVENT &event)
{
	// Check for a message window.
	UI_WINDOW_OBJECT *defaultsWindow = GetDefaultsWindow();
	if (!defaultsWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of edit windows.
	UI_EVENT cEvent(OPT_RESOURCE_CLEARALL);
	cEvent.data = defaultsWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the visible edit windows.
	{
		ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &defaultsFile);
		for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
			if (window->SearchID() == ID_I18N_WINDOW)
				window->Store(window->StringID(), defaultsFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear all the defaults windows.
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; )
		if (window->SearchID() == ID_I18N_WINDOW)
		{
			UI_WINDOW_OBJECT *defaultsWindow = window->Next();
			*windowManager - window;
			delete window;
			window = defaultsWindow;
		}
		else
			window = window->Next();
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::LanguageCreate(const UI_EVENT &event)
{
	*windowManager + new ZAF_LANGUAGE_WINDOW(ZIL_NULLP(ZIL_ICHAR));
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::LocaleCreate(const UI_EVENT &event)
{
	*windowManager + new ZAF_LOCALE_WINDOW(ZIL_NULLP(ZIL_ICHAR));
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsDelete(const UI_EVENT &event)
{
	// Delete the specified defaults.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
		return (directoryService->Event(S_ERROR));

	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_I18N::Traverse(defaultsFile, _i18nDirectory);
	ZAF_STORAGE_SERVICE::RRmDir(defaultsFile, pathName); // remove the defaults.
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsLoad(const UI_EVENT &event)
{
	// Make sure there is an defaults file.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
		return (directoryService->Event(S_ERROR));

	static UIW_WINDOW *defaultsWindow = ZIL_NULLP(UIW_WINDOW);
	if (!defaultsWindow)
	{
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		ZIL_ICHAR *name = (ZIL_ICHAR *)event.data;
		if (_i18nDirectory == _languageDirectory)
			defaultsWindow = new ZAF_LANGUAGE_WINDOW(name);
		else
			defaultsWindow = new ZAF_LOCALE_WINDOW(name);
		eventManager->Put(event); // Wait till provider window is removed from screen.
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}
	else
	{
		*windowManager + defaultsWindow;
		ZAF_SERVICE_MANAGER::_changedData = TRUE;
		defaultsWindow = ZIL_NULLP(UIW_WINDOW);
	}
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsRequest(const UI_EVENT &event)
{
	// Make sure there is an defaults file.
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
		return (directoryService->Event(S_ERROR));

	// Make a request to the resource service.
	ZIL_ICHAR *name = event.windowObject ? event.windowObject->StringID() : _i18nDirectory;
	if (!strcmp(_languageDirectory, name))
		_i18nDirectory = _languageDirectory;
	else
		_i18nDirectory = _localeDirectory;
	ZIL_I18N::Traverse(defaultsFile, _i18nDirectory, TRUE);
	int traverseLock = TRUE;
	storageService->Information(I_SET_TRAVERSE_LOCK, &traverseLock);
	storageService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	storageService->Information(I_SET_REQUEST, &ccode);
	storageService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsStore(const UI_EVENT &)
{
	// Make sure there is a defaults window.
	UI_WINDOW_OBJECT *defaultsWindow = GetDefaultsWindow();
	if (!defaultsWindow)
		return (storageService->Event(S_ERROR));

	// Store the i18n default information.
	UI_EVENT storeEvent(OPT_RESOURCE_STOREAS);
	storeEvent.data = defaultsWindow->StringID();
	return (DefaultsStoreAs(storeEvent));
}

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsStoreAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Make sure there is an defaults file.
	ZIL_STORAGE *defaultsFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
		return (directoryService->Event(S_ERROR));

	// Make sure there is a defaults window.
	UI_WINDOW_OBJECT *defaultsWindow = GetDefaultsWindow();
	if (!defaultsWindow)
		return (storageService->Event(S_ERROR));

	// Store the i18n default information.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	defaultsWindow->Store((ZIL_ICHAR *)event.data, defaultsFile);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Static tables ------------------------------------------------------

ZIL_ICHAR ZAF_I18N_EDITOR::_className[] = { 'Z','A','F','_','D','E','F','A','U','L','T','S','_','E','D','I','T','O','R',0 };

ZIL_STORAGE_READ_ONLY *ZAF_I18N_EDITOR::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
ZIL_ICHAR ZAF_I18N_EDITOR::_txtExtension[] = { '.','t','x','t',0 };

ZIL_ICHAR _CheckFlagSelection[] = { 'C','h','e','c','k','F','l','a','g','S','e','l','e','c','t','i','o','n', 0 };
static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(ZAF_LOCALE_WINDOW::CheckFlagSelection), _CheckFlagSelection, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_I18N_EDITOR::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
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
	{ ID_SCROLL_BAR, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SCROLL_BAR::New), UIW_SCROLL_BAR::_className, 0 },
	{ ID_STRING, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_VT_LIST, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_VT_LIST::New), UIW_VT_LIST::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_I18N_EDITOR::_objectTable = __objectTable;

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_I18N_EDITOR::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_I18N_EDITOR::PROCESS ZAF_I18N_EDITOR::_processTable[] =
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

	{ OPT_RESOURCE_CLEAR,			ZIL_PROCESS_REFERENCE(DefaultsClear) },	// Defaults requests.
	{ OPT_RESOURCE_CLEARALL,		ZIL_PROCESS_REFERENCE(DefaultsClearAll) },
	{ OPT_LANGUAGE_CREATE,			ZIL_PROCESS_REFERENCE(LanguageCreate) },	
	{ OPT_LOCALE_CREATE,			ZIL_PROCESS_REFERENCE(LocaleCreate) },	
	{ OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(DefaultsRequest) },
	{ OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(DefaultsRequest) },
	{ OPT_RESOURCE_STORE,			ZIL_PROCESS_REFERENCE(DefaultsStore) },
	{ OPT_RESOURCE_STOREAS,			ZIL_PROCESS_REFERENCE(DefaultsRequest) },
	{ -OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(DefaultsDelete) },	// Defaults responses.
	{ -OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(DefaultsLoad) },
	{ -OPT_RESOURCE_STOREAS,		ZIL_PROCESS_REFERENCE(DefaultsStoreAs) },

	{ OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(DefaultsExport) },
	{ -OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(DefaultsExport) },
	{ OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(DefaultsExport) },
	{ -OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(DefaultsExport) },
	{ OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(DefaultsImport) },
	{ -OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(DefaultsImport) },
	{ OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(DefaultsImport) },
	{ -OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(DefaultsImport) },

	{ 0, 0 }
};
