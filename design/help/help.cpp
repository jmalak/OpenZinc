//	Zinc Interface Library - HELP.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "help.hpp"
#define USE_MSG_TABLE
#include "p_help.hpp"

extern ZIL_ICHAR *_hppDirectory;
extern ZIL_ICHAR *_helpDirectory;
extern ZIL_ICHAR *_currentDirectory;
extern ZIL_ICHAR *_parentDirectory;

// --------------------------------------------------------------------------
// ----- ZAF_HELP_EDITOR ----------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__DECCXX)
ZAF_HELP_EDITOR::ZAF_HELP_EDITOR() : UIW_WINDOW(0, 0, 0, 0) {}
#endif

ZAF_HELP_EDITOR::ZAF_HELP_EDITOR(WOAF_FLAGS _woAdvancedFlags,
	WOS_STATUS _woStatus) :
	// base initialization
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_help"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable),
	// member initialization
	msgTable(_serviceManager->ZMSG_msgTableName(), _serviceManager->Storage(&_storage, "p_help")),
	helpID(NO_HELP_CONTEXT)
{
	// Set the services.
	searchID = ID_HELP_EDITOR;
	woStatus |= _woStatus;
	woAdvancedFlags |= _woAdvancedFlags;
	directoryService = _serviceManager->Get(ID_DIRECTORY_SERVICE);
	storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	Information(I_COPY_TEXT, originalTitle);
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

ZAF_HELP_EDITOR::~ZAF_HELP_EDITOR(void)
{
	// Remove the resource storage.
	if (_storage)
		delete _storage;
}

EVENT_TYPE ZAF_HELP_EDITOR::Event(const UI_EVENT &event)
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

	// Default to base window class.
	switch (ccode)
	{
//	case S_SIZE:
	case S_RESTORE:
		ccode = UIW_WINDOW::Event(event);
		SetTitle();
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		StatusUpdate();
		helpSystem->ResetStorage(_storage, FALSE);
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

UI_WINDOW_OBJECT *ZAF_HELP_EDITOR::GetHelpWindow(void)
{
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_HELP_WINDOW)
			return (window);
	return (ZIL_NULLP(UI_WINDOW_OBJECT));
}

void ZAF_HELP_EDITOR::GenerateHPPEntries(void)
{
	ZIL_ICHAR *_contextName = msgTable.GetMessage(ZMSG_CONTEXT_NAME);
	ZIL_ICHAR *_format = msgTable.GetMessage(ZMSG_CONTEXT_FORMAT);

	// Check for a valid help directory.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return;

	// Generate the table entries.
	if (helpFile->ChDir(_hppDirectory))
	{
		helpFile->MkDir(_hppDirectory);
		helpFile->ChDir(_hppDirectory);
	}
	ZIL_STORAGE_OBJECT hppElement(*helpFile, _contextName, ID_HELP_CONTEXT, UIS_OPENCREATE | UIS_READWRITE);
	if (!helpFile->ChDir(_helpDirectory))
	{
		for (ZIL_ICHAR *entry = helpFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); entry; entry = helpFile->FindNextObject())
			if (strcmp(entry, _currentDirectory) &&
				strcmp(entry, _parentDirectory))
			{
				ZIL_STORAGE_OBJECT_READ_ONLY hObject(*helpFile, entry, 0);
				ZIL_ICHAR title[256]; title[0] = '\0';
				ZIL_ICHAR line[256];
				hObject.Load(title, 256);
				sprintf(line, _format, entry, hObject.objectID, title[0] ? title : _serviceManager->ZMSG_untitled());
				hppElement.Store(line);
			}
	}
	hppElement.Store(_blankString);	// End of HPP string entries.
}

void *ZAF_HELP_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
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

	case GET_HELP_IDENTIFICATION:
		{
		ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
		if (helpID == NO_HELP_CONTEXT &&
			storageService->Information(I_GET_STORAGE, &helpFile))
		{
			for (ZIL_ICHAR *name = helpFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); name; name = helpFile->FindNextObject())
			{
				ZIL_STORAGE_OBJECT_READ_ONLY hObject(*helpFile, name, 0);
				if (hObject.objectID > helpID)
 					helpID = hObject.objectID;
			}
		}
		*(UI_HELP_CONTEXT *)data = ++helpID;
		}
		break;

	case I_STATUS_UPDATE:
	case I_STATUS_CLEAR:
		StatusUpdate((UI_WINDOW_OBJECT *)data, (request == I_STATUS_CLEAR));
		break;

	case I_GENERATE_HPP:
		// Request from another service.
		GenerateHPPEntries();
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

	return (data);
}

void ZAF_HELP_EDITOR::SetTitle(void)
{
	static ZIL_ICHAR _format[] = { '%','s',' ','-',' ','%','s',' ','-',' ','%','s',0 };
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService)
		storageService->Information(I_GET_STORAGE, &helpFile);

	ZIL_ICHAR fullTitle[ZIL_MAXPATHLEN];
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
		strcpy(fullTitle, originalTitle);
	else if (helpFile)
	{
		ZIL_ICHAR pathName[ZIL_MAXPATHLEN], fileName[ZIL_MAXPATHLEN];
		helpFile->StorageName(pathName);
		helpFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), fileName);
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, fileName);
	}
	else
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, _serviceManager->ZMSG_noFile());
	Information(I_SET_TEXT, fullTitle);
}

void ZAF_HELP_EDITOR::StatusUpdate(UI_WINDOW_OBJECT *editObject, int clear)
{
	// ----- Modify the menu items. -----
	UI_WINDOW_OBJECT *field;
	WOF_FLAGS flag = WOF_NON_SELECTABLE;

	const ZIL_NUMBERID CONTEXT_STORE = 0x006F;
	field = Get(CONTEXT_STORE);
	if (clear)
		editObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	else if (!editObject)
		editObject = GetHelpWindow();
	if ((editObject && FlagSet(field->woFlags, flag)) || (!editObject && !FlagSet(field->woFlags, flag)))
	{
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID CONTEXT_STORE_AS = 0x0070;
		field = Get(CONTEXT_STORE_AS);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID CONTEXT_CLEAR = 0x0099;
		field = Get(CONTEXT_CLEAR);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID CONTEXT_CLEARALL = 0x009A;
		field = Get(CONTEXT_CLEARALL);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);
	}
}

// ----- File Options -------------------------------------------------------

EVENT_TYPE ZAF_HELP_EDITOR::FileClose(const UI_EVENT &event)
{
	// Make sure there is a help file.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return (directoryService->Event(S_ERROR));

	// Close the help file.
	delete helpFile;
	storageService->Information(I_SET_STORAGE, ZIL_NULLP(ZIL_STORAGE));
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::FileDelete(const UI_EVENT &event)
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

EVENT_TYPE ZAF_HELP_EDITOR::FileNew(const UI_EVENT &event)
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

	// See if there is a pre-existing help file.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &helpFile))
		delete helpFile;
	helpFile = newFile;
	storageService->Information(I_SET_STORAGE, helpFile);
	SetTitle();
	helpID = NO_HELP_CONTEXT; // reset the help context id #.
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::FileOpen(const UI_EVENT &event)
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

	// See if there is a pre-existing help file.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &helpFile))
		delete helpFile;
	helpFile = openFile;
	storageService->Information(I_SET_STORAGE, helpFile);
	SetTitle();
	helpID = NO_HELP_CONTEXT; // reset the help context id #.
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::FileRequest(const UI_EVENT &event)
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

EVENT_TYPE ZAF_HELP_EDITOR::FileSave(const UI_EVENT &event)
{
	// Make sure there is a help file.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return (directoryService->Event(S_ERROR));

	// Make sure the help file has a valid name.
	ZIL_ICHAR name[ZIL_MAXPATHLEN], pathName[ZIL_MAXPATHLEN];
	helpFile->StorageName(pathName);
	helpFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);
	if (!stricmp(name, _serviceManager->ZMSG_tempFileName()))
		return (Event(OPT_FILE_SAVEAS));

	// Save the current edit file with all the edit windows.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_HELP_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), helpFile);
	GenerateHPPEntries();
	storageService->Information(I_GENERATE_HPP, ZIL_NULLP(void));
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	helpFile->Save(revisions);
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::FileSaveAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data && event.type == -OPT_FILE_SAVEAS)
		return (event.type);

	// See if there is a pre-existing help file.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return (directoryService->Event(S_ERROR));
	else if (event.type == OPT_FILE_SAVEAS)
		return (FileRequest(event));

	// Save the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_HELP_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), helpFile);
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	if (helpFile->SaveAs(pathName, revisions))
	{
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		return (directoryService->Event(S_ERROR));
	}
	GenerateHPPEntries();
	storageService->Information(I_GENERATE_HPP, ZIL_NULLP(void));
	helpFile->Save(); // we must re-save to get base changes.
	SetTitle();
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Help Options -------------------------------------------------------

EVENT_TYPE ZAF_HELP_EDITOR::HelpClear(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *helpWindow = GetHelpWindow();
	if (!helpWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of existing window.
	UI_EVENT cEvent(OPT_RESOURCE_CLEAR);
	cEvent.data = helpWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the first edit window.
	{
		ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &helpFile);
		helpWindow->Store(helpWindow->StringID(), helpFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear the first help window.
	*windowManager - helpWindow;
	delete helpWindow;
	StatusUpdate(GetHelpWindow(), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::HelpClearAll(const UI_EVENT &event)
{
	// Check for a help window.
	UI_WINDOW_OBJECT *helpWindow = GetHelpWindow();
	if (!helpWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of edit windows.
	UI_EVENT cEvent(OPT_RESOURCE_CLEARALL);
	cEvent.data = helpWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the visible edit windows.
	{
		ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &helpFile);
		for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
			if (window->SearchID() == ID_HELP_WINDOW)
				window->Store(window->StringID(), helpFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear all the help windows.
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; )
		if (window->SearchID() == ID_HELP_WINDOW)
		{
			UI_WINDOW_OBJECT *helpWindow = window->Next();
			*windowManager - window;
			delete window;
			window = helpWindow;
		}
		else
			window = window->Next();
	StatusUpdate(GetHelpWindow(), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::HelpCreate(const UI_EVENT &event)
{
	UIW_HELP_WINDOW *helpWindow = new UIW_HELP_WINDOW(ZIL_NULLP(ZIL_ICHAR));
	*windowManager + helpWindow;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::HelpDelete(const UI_EVENT &event)
{
	// Delete the specified help resources.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return (directoryService->Event(S_ERROR));

	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	helpFile->ChDir(_helpDirectory);	// remove the help context.
	if (helpFile->DestroyObject(pathName))
		return (storageService->Event(S_ERROR));
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::HelpLoad(const UI_EVENT &event)
{
	// Make sure there is a help file.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return (directoryService->Event(S_ERROR));

	static UIW_HELP_WINDOW *helpWindow = ZIL_NULLP(UIW_HELP_WINDOW);
	if (!helpWindow)
	{
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		ZIL_ICHAR *name = (ZIL_ICHAR *)event.data;
		helpWindow = new UIW_HELP_WINDOW(name);
		helpWindow->Load(name, helpFile);
		eventManager->Put(event); // Wait till provider window is removed from screen.
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		return (event.type);
	}
	else
	{
		*windowManager + helpWindow;
		ZAF_SERVICE_MANAGER::_changedData = TRUE;
		helpWindow = ZIL_NULLP(UIW_HELP_WINDOW);
	}
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::HelpRequest(const UI_EVENT &event)
{
	// Make sure there is a help file.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return (directoryService->Event(S_ERROR));

	// Make a request to the resource service.
	if (helpFile->ChDir(_helpDirectory))
	{
		helpFile->MkDir(_helpDirectory);
		helpFile->ChDir(_helpDirectory);
	}
	int traverseLock = TRUE;
	storageService->Information(I_SET_TRAVERSE_LOCK, &traverseLock);
	storageService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	storageService->Information(I_SET_REQUEST, &ccode);
	storageService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::HelpStore(const UI_EVENT &)
{
	// Make sure there is a help window.
	UI_WINDOW_OBJECT *helpWindow = GetHelpWindow();
	if (!helpWindow)
		return (storageService->Event(S_ERROR));

	// Store the help context.
	UI_EVENT storeEvent(OPT_RESOURCE_STOREAS);
	storeEvent.data = helpWindow->StringID();
	return (HelpStoreAs(storeEvent));
}

EVENT_TYPE ZAF_HELP_EDITOR::HelpStoreAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Make sure there is a help file.
	ZIL_STORAGE *helpFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &helpFile))
		return (directoryService->Event(S_ERROR));

	// Make sure there is a help window.
	UI_WINDOW_OBJECT *helpWindow = GetHelpWindow();
	if (!helpWindow)
		return (storageService->Event(S_ERROR));

	// Store the help context.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	helpWindow->Store((ZIL_ICHAR *)event.data, helpFile);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Static tables ------------------------------------------------------

ZIL_ICHAR ZAF_HELP_EDITOR::_className[] = { 'Z','A','F','_','H','E','L','P','_','E','D','I','T','O','R',0 };

ZIL_STORAGE_READ_ONLY *ZAF_HELP_EDITOR::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
ZIL_ICHAR ZAF_HELP_EDITOR::_txtExtension[] = { '.','t','x','t',0 };

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_HELP_EDITOR::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
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
	{ ID_TEXT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TEXT::New), UIW_TEXT::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_HELP_EDITOR::_objectTable = __objectTable;

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_HELP_EDITOR::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_HELP_EDITOR::PROCESS ZAF_HELP_EDITOR::_processTable[] =
{
	{ OPT_FILE_CLOSE,				ZIL_PROCESS_REFERENCE(FileClose) },		// File requests.
	{ OPT_FILE_DELETE, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_SAVE, 				ZIL_PROCESS_REFERENCE(FileSave) },
	{ OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileSaveAs) },
	{ -OPT_FILE_DELETE, 			ZIL_PROCESS_REFERENCE(FileDelete) },		// File responses.
	{ -OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileNew) },
	{ -OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileOpen) },
	{ -OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileSaveAs) },

	{ OPT_RESOURCE_CREATE,			ZIL_PROCESS_REFERENCE(HelpCreate) },	// Help requests.
	{ OPT_RESOURCE_CLEAR,			ZIL_PROCESS_REFERENCE(HelpClear) },
	{ OPT_RESOURCE_CLEARALL,		ZIL_PROCESS_REFERENCE(HelpClearAll) },
	{ OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(HelpRequest) },
	{ OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(HelpRequest) },
	{ OPT_RESOURCE_STORE,			ZIL_PROCESS_REFERENCE(HelpStore) },
	{ OPT_RESOURCE_STOREAS,			ZIL_PROCESS_REFERENCE(HelpRequest) },
	{ -OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(HelpDelete) },	// Help responses.
	{ -OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(HelpLoad) },
	{ -OPT_RESOURCE_STOREAS,		ZIL_PROCESS_REFERENCE(HelpStoreAs) },

	{ OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(HelpExport) },
	{ -OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(HelpExport) },
	{ OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(HelpExport) },
	{ -OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(HelpExport) },
	{ OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(HelpImport) },
	{ -OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(HelpImport) },
	{ OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(HelpImport) },
	{ -OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(HelpImport) },

	{ 0, 0 }
};
