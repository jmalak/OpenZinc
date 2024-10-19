//	Zinc Interface Library - MESSAGE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "message.hpp"

extern ZIL_ICHAR *_hppDirectory;
extern ZIL_ICHAR *_messageDirectory;

const EVENT_TYPE OPT_NAME			= 6015;

// --------------------------------------------------------------------------
// ----- ZAF_MESSAGE_EDITOR -------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__DECCXX)
ZAF_MESSAGE_EDITOR::ZAF_MESSAGE_EDITOR() : UIW_WINDOW(0, 0, 0, 0) {}
#endif

ZAF_MESSAGE_EDITOR::ZAF_MESSAGE_EDITOR(WOAF_FLAGS _woAdvancedFlags,
	WOS_STATUS _woStatus) :
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_messag"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable)
{
	// Set the storage services.
	searchID = ID_MESSAGE_EDITOR;
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

ZAF_MESSAGE_EDITOR::~ZAF_MESSAGE_EDITOR(void)
{
	// Remove the resource storage.
	if (_storage)
		delete _storage;
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::Event(const UI_EVENT &event)
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

void *ZAF_MESSAGE_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
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

UI_WINDOW_OBJECT *ZAF_MESSAGE_EDITOR::GetMessageWindow(void)
{
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_MESSAGE_WINDOW)
			return (window);
	return (ZIL_NULLP(UI_WINDOW_OBJECT));
}

void ZAF_MESSAGE_EDITOR::SetTitle(void)
{
	static ZIL_ICHAR _format[] = { '%','s',' ','-',' ','%','s',' ','-',' ','%','s',0 };
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService)
		storageService->Information(I_GET_STORAGE, &messageFile);

	// Check for a file and make sure it isn't a child-service (locked).
	ZIL_ICHAR fullTitle[ZIL_MAXPATHLEN];
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
		strcpy(fullTitle, originalTitle);
	else if (messageFile)
	{
		ZIL_ICHAR pathName[ZIL_MAXPATHLEN], fileName[ZIL_MAXPATHLEN];
		messageFile->StorageName(pathName);
		messageFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), fileName);
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, fileName);
	}
	else
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, _serviceManager->ZMSG_noFile());
	Information(I_SET_TEXT, fullTitle);
}

void ZAF_MESSAGE_EDITOR::StatusUpdate(UI_WINDOW_OBJECT *editObject, int clear)
{
	// ----- Modify the menu items. -----
	UI_WINDOW_OBJECT *field;
	WOF_FLAGS flag = WOF_NON_SELECTABLE;

	const ZIL_NUMBERID MESSAGE_STORE = 0x006F;
	field = Get(MESSAGE_STORE);
	if (clear)
		editObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	else if (!editObject)
		editObject = GetMessageWindow();
	if ((editObject && FlagSet(field->woFlags, flag)) || (!editObject && !FlagSet(field->woFlags, flag)))
	{
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID MESSAGE_STORE_AS = 0x0070;
		field = Get(MESSAGE_STORE_AS);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID MESSAGE_CLEAR = 0x0099;
		field = Get(MESSAGE_CLEAR);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID MESSAGE_CLEARALL = 0x009A;
		field = Get(MESSAGE_CLEARALL);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);
	}
}

// ----- File Options -------------------------------------------------------

EVENT_TYPE ZAF_MESSAGE_EDITOR::FileClose(const UI_EVENT &event)
{
	// Make sure there is a message file.
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &messageFile))
		return (directoryService->Event(S_ERROR));

	// Close the message file.
	delete messageFile;
	storageService->Information(I_SET_STORAGE, ZIL_NULLP(ZIL_STORAGE));
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::FileDelete(const UI_EVENT &event)
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

EVENT_TYPE ZAF_MESSAGE_EDITOR::FileNew(const UI_EVENT &event)
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

	// See if there is a pre-existing message file.
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &messageFile))
		delete messageFile;
	messageFile = newFile;
	storageService->Information(I_SET_STORAGE, messageFile);
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::FileOpen(const UI_EVENT &event)
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

	// See if there is a pre-existing message file.
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &messageFile))
		delete messageFile;
	messageFile = openFile;
	storageService->Information(I_SET_STORAGE, messageFile);
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::FilePreferences(const UI_EVENT &event)
{
	*windowManager + new ZAF_MESSAGE_PREFERENCES;
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::FileRequest(const UI_EVENT &event)
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

EVENT_TYPE ZAF_MESSAGE_EDITOR::FileSave(const UI_EVENT &event)
{
	// Make sure there is a message file.
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &messageFile))
		return (directoryService->Event(S_ERROR));

	// Make sure the message file has a valid name.
	ZIL_ICHAR name[ZIL_MAXPATHLEN], pathName[ZIL_MAXPATHLEN];
	messageFile->StorageName(pathName);
	messageFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);
	if (!stricmp(name, _serviceManager->ZMSG_tempFileName()))
		return (Event(OPT_FILE_SAVEAS));

	// Save the current message file.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_MESSAGE_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), messageFile);
	storageService->Information(I_GENERATE_HPP, ZIL_NULLP(void));
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	messageFile->Save(revisions);
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::FileSaveAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data && event.type == -OPT_FILE_SAVEAS)
		return (event.type);

	// See if there is a pre-existing message file.
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &messageFile))
		return (directoryService->Event(S_ERROR));
	else if (event.type == OPT_FILE_SAVEAS)
		return (FileRequest(event));

	// Save the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_MESSAGE_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), messageFile);
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	if (messageFile->SaveAs(pathName, revisions))
	{
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		return (directoryService->Event(S_ERROR));
	}
	storageService->Information(I_GENERATE_HPP, ZIL_NULLP(void));
	messageFile->Save(); // we must re-save to get base changes.
	SetTitle();
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Message Options ----------------------------------------------------

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageClear(const UI_EVENT &event)
{
	// Check for a message window.
	UI_WINDOW_OBJECT *messageWindow = GetMessageWindow();
	if (!messageWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of existing window.
	UI_EVENT cEvent(OPT_RESOURCE_CLEAR);
	cEvent.data = messageWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the first edit window.
	{
		ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &messageFile);
		messageWindow->Store(messageWindow->StringID(), messageFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear the first message window.
	*windowManager - messageWindow;
	delete messageWindow;
	StatusUpdate(GetMessageWindow(), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageClearAll(const UI_EVENT &event)
{
	// Check for a message window.
	UI_WINDOW_OBJECT *messageWindow = GetMessageWindow();
	if (!messageWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of edit windows.
	UI_EVENT cEvent(OPT_RESOURCE_CLEARALL);
	cEvent.data = messageWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the visible edit windows.
	{
		ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &messageFile);
		for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
			if (window->SearchID() == ID_MESSAGE_WINDOW)
				window->Store(window->StringID(), messageFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear all the message windows.
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; )
		if (window->SearchID() == ID_MESSAGE_WINDOW)
		{
			UI_WINDOW_OBJECT *messageWindow = window->Next();
			*windowManager - window;
			delete window;
			window = messageWindow;
		}
		else
			window = window->Next();
	StatusUpdate(GetMessageWindow(), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageCreate(const UI_EVENT &event)
{
	*windowManager + new UIW_MESSAGE_WINDOW(ZIL_NULLP(ZIL_ICHAR));
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageDelete(const UI_EVENT &event)
{
	// Delete the specified message resources.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &messageFile))
		return (directoryService->Event(S_ERROR));

	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	messageFile->ChDir(_messageDirectory);	// remove the message.
	if (messageFile->DestroyObject(pathName))
		return (storageService->Event(S_ERROR));
	messageFile->ChDir(_hppDirectory); 	// remove the hpp entry.
	messageFile->DestroyObject(pathName);
	messageFile->storageError = 0;
	messageFile->ChDir(_messageDirectory);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageLoad(const UI_EVENT &event)
{
	// Make sure there is a message file.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &messageFile))
		return (directoryService->Event(S_ERROR));

	static UIW_MESSAGE_WINDOW *messageWindow = ZIL_NULLP(UIW_MESSAGE_WINDOW);
	if (!messageWindow)
	{
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		ZIL_ICHAR *name = (ZIL_ICHAR *)event.data;
		messageWindow = new UIW_MESSAGE_WINDOW(name);
		messageWindow->Load(name, messageFile);
		eventManager->Put(event); // Wait till provider window is removed from screen.
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}
	else
	{
		*windowManager + messageWindow;
		ZAF_SERVICE_MANAGER::_changedData = TRUE;
		messageWindow = ZIL_NULLP(UIW_MESSAGE_WINDOW);
	}
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageRequest(const UI_EVENT &event)
{
	// Make sure there is a message file.
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &messageFile))
		return (directoryService->Event(S_ERROR));

	// Make a request to the resource service.
	if (messageFile->ChDir(_messageDirectory))
	{
		messageFile->MkDir(_messageDirectory);
		messageFile->ChDir(_messageDirectory);
	}
	int traverseLock = TRUE;
	storageService->Information(I_SET_TRAVERSE_LOCK, &traverseLock);
	storageService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	storageService->Information(I_SET_REQUEST, &ccode);
	storageService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageStore(const UI_EVENT &)
{
	// Make sure there is a message window.
	UI_WINDOW_OBJECT *messageWindow = GetMessageWindow();
	if (!messageWindow)
		return (storageService->Event(S_ERROR));

	// Store the message table.
	UI_EVENT storeEvent(OPT_RESOURCE_STOREAS);
	storeEvent.data = messageWindow->StringID();
	return (MessageStoreAs(storeEvent));
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageStoreAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Make sure there is a message file.
	ZIL_STORAGE *messageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &messageFile))
		return (directoryService->Event(S_ERROR));

	// Make sure there is a message window.
	UI_WINDOW_OBJECT *messageWindow = GetMessageWindow();
	if (!messageWindow)
		return (storageService->Event(S_ERROR));

	// Store the message table.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	messageWindow->Store((ZIL_ICHAR *)event.data, messageFile);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Static tables ------------------------------------------------------

ZIL_ICHAR ZAF_MESSAGE_EDITOR::_className[] = { 'Z','A','F','_','M','E','S','S','A','G','E','_','E','D','I','T','O','R',0 };

ZIL_STORAGE_READ_ONLY *ZAF_MESSAGE_EDITOR::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
ZIL_ICHAR ZAF_MESSAGE_EDITOR::_txtExtension[] = { '.','t','x','t',0 };

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_MESSAGE_EDITOR::_userTable = __userTable;

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
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_VT_LIST, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_VT_LIST::New), UIW_VT_LIST::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_MESSAGE_EDITOR::_objectTable = __objectTable;

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_MESSAGE_EDITOR::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

//original function below has been commented out by jdh to allow
//more specific identification of function. gcc complains
//ZAF_MESSAGE_EDITOR::PROCESS ZAF_MESSAGE_EDITOR::_processTable[] =
//{
//	{ OPT_FILE_CLOSE,			ZIL_PROCESS_REFERENCE(FileClose) },		// File requests.
//	{ OPT_FILE_DELETE, 			ZIL_PROCESS_REFERENCE(FileRequest) },
//	{ OPT_FILE_NEW, 			ZIL_PROCESS_REFERENCE(FileRequest) },
//	{ OPT_FILE_OPEN, 			ZIL_PROCESS_REFERENCE(FileRequest) },
//	{ OPT_FILE_SAVE, 			ZIL_PROCESS_REFERENCE(FileSave) },
//	{ OPT_FILE_SAVEAS,			ZIL_PROCESS_REFERENCE(FileSaveAs) },
//	{ -OPT_FILE_DELETE, 		ZIL_PROCESS_REFERENCE(FileDelete) },		// File responses.
//	{ -OPT_FILE_NEW, 			ZIL_PROCESS_REFERENCE(FileNew) },
//	{ -OPT_FILE_OPEN, 			ZIL_PROCESS_REFERENCE(FileOpen) },
//	{ -OPT_FILE_SAVEAS,			ZIL_PROCESS_REFERENCE(FileSaveAs) },
//
//	{ OPT_RESOURCE_CLEAR,		ZIL_PROCESS_REFERENCE(MessageClear) },		// Message requests.
//	{ OPT_RESOURCE_CLEARALL,	ZIL_PROCESS_REFERENCE(MessageClearAll) },
//	{ OPT_RESOURCE_CREATE,		ZIL_PROCESS_REFERENCE(MessageCreate) },	
//	{ OPT_RESOURCE_DELETE,		ZIL_PROCESS_REFERENCE(MessageRequest) },
//	{ OPT_RESOURCE_IMPORT,		ZIL_PROCESS_REFERENCE(MessageRequest) },
//	{ OPT_RESOURCE_LOAD,		ZIL_PROCESS_REFERENCE(MessageRequest) },
//	{ OPT_RESOURCE_STORE,		ZIL_PROCESS_REFERENCE(MessageStore) },
//	{ OPT_RESOURCE_STOREAS,		ZIL_PROCESS_REFERENCE(MessageRequest) },
//	{ -OPT_RESOURCE_DELETE,		ZIL_PROCESS_REFERENCE(MessageDelete) },	// Message responses.
//	{ -OPT_RESOURCE_LOAD,		ZIL_PROCESS_REFERENCE(MessageLoad) },
//	{ -OPT_RESOURCE_STOREAS,	ZIL_PROCESS_REFERENCE(MessageStoreAs) },
//
//	{ OPT_FILE_EXPORT,			ZIL_PROCESS_REFERENCE(MessageExport) },
//	{ -OPT_FILE_EXPORT,			ZIL_PROCESS_REFERENCE(MessageExport) },
//	{ OPT_RESOURCE_EXPORT,		ZIL_PROCESS_REFERENCE(MessageExport) },
//	{ -OPT_RESOURCE_EXPORT,		ZIL_PROCESS_REFERENCE(MessageExport) },
//	{ OPT_FILE_IMPORT,			ZIL_PROCESS_REFERENCE(MessageImport) },
//	{ -OPT_FILE_IMPORT,			ZIL_PROCESS_REFERENCE(MessageImport) },
//	{ OPT_RESOURCE_IMPORT,		ZIL_PROCESS_REFERENCE(MessageImport) },
//	{ -OPT_RESOURCE_IMPORT,		ZIL_PROCESS_REFERENCE(MessageImport) },
//
//	{ 0, 0 }
//};

ZAF_MESSAGE_EDITOR::PROCESS ZAF_MESSAGE_EDITOR::_processTable[] =
{
	{ OPT_FILE_CLOSE,			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileClose) },		// File requests.
	{ OPT_FILE_DELETE, 			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileRequest) },
	{ OPT_FILE_NEW, 			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileRequest) },
	{ OPT_FILE_OPEN, 			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileRequest) },
	{ OPT_FILE_SAVE, 			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileSave) },
	{ OPT_FILE_SAVEAS,			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileSaveAs) },
	{ -OPT_FILE_DELETE, 		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileDelete) },		// File responses.
	{ -OPT_FILE_NEW, 			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileNew) },
	{ -OPT_FILE_OPEN, 			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileOpen) },
	{ -OPT_FILE_SAVEAS,			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::FileSaveAs) },

	{ OPT_RESOURCE_CLEAR,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageClear) },		// Message requests.
	{ OPT_RESOURCE_CLEARALL,	ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageClearAll) },
	{ OPT_RESOURCE_CREATE,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageCreate) },	
	{ OPT_RESOURCE_DELETE,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageRequest) },
	{ OPT_RESOURCE_IMPORT,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageRequest) },
	{ OPT_RESOURCE_LOAD,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageRequest) },
	{ OPT_RESOURCE_STORE,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageStore) },
	{ OPT_RESOURCE_STOREAS,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageRequest) },
	{ -OPT_RESOURCE_DELETE,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageDelete) },	// Message responses.
	{ -OPT_RESOURCE_LOAD,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageLoad) },
	{ -OPT_RESOURCE_STOREAS,	ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageStoreAs) },

	{ OPT_FILE_EXPORT,			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageExport) },
	{ -OPT_FILE_EXPORT,			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageExport) },
	{ OPT_RESOURCE_EXPORT,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageExport) },
	{ -OPT_RESOURCE_EXPORT,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageExport) },
	{ OPT_FILE_IMPORT,			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageImport) },
	{ -OPT_FILE_IMPORT,			ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageImport) },
	{ OPT_RESOURCE_IMPORT,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageImport) },
	{ -OPT_RESOURCE_IMPORT,		ZIL_PROCESS_REFERENCE(&ZAF_MESSAGE_EDITOR::MessageImport) },

	{ 0, 0 }
};
