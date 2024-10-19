//	Zinc Interface Library - SERVICE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !(__BORLANDC__ >= 0x0500)
extern "C"
{
#endif
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#if !defined(__MWERKS__)
#	include <string.h>
#endif
#if !(__BORLANDC__ >= 0x0500)
}
#endif
#define USE_RAW_KEYS
#include "service.hpp"
#define USE_HELP_CONTEXTS
#define USE_MSG_TABLE
#define USE_STATUS_WINDOW
#define USE_SAVE_APPLICATION
#include "p_servic.hpp"

extern ZIL_ICHAR *_parentDirectory;
extern ZIL_ICHAR *_currentDirectory;
extern ZIL_ICHAR *_rootStorageDirectory;

// ----- Following code is exact copy of "z_stdarg.hpp" -----

const int ZIL_MAXPARAMLEN				= 512;

class ZIL_EXPORT_CLASS ZIL_STDARG : public ZIL_INTERNATIONAL
{
public:
	static int vsprintf(ZIL_ICHAR *buffer, const ZIL_ICHAR *format, va_list argList);
	static int vsscanf(ZIL_ICHAR *buffer, const ZIL_ICHAR *format, va_list argList);
	static void RearrangeArgs(int isScanf, void *newBuffer,
		const ZIL_ICHAR *format, const va_list ap, ZIL_ICHAR *newFormat,
		va_list *toRet);
	static int _vsprintf(ZIL_ICHAR *fp, const ZIL_ICHAR *fmt, va_list ap);
	static int _vsscanf(ZIL_ICHAR *fp, const ZIL_ICHAR *fmt, va_list ap);
};

// --------------------------------------------------------------------------
// ----- ZAF_SUBLIST --------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZAF_SUBLIST : public UIW_VT_LIST
{
public:
	ZAF_SUBLIST(int left, int top, int width, int height);
	EVENT_TYPE Event(const UI_EVENT &event);
};

ZAF_SUBLIST::ZAF_SUBLIST(int left, int top, int width, int height) :
	UIW_VT_LIST(left, top, width, height, ZIL_NULLF(ZIL_COMPARE_FUNCTION),
	WNF_NO_WRAP | WNF_CONTINUE_SELECT | WNF_BITMAP_CHILDREN)
{
	Add(new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL));
}

EVENT_TYPE ZAF_SUBLIST::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	if (event.InputType() == E_KEY)
		ccode = UI_EVENT_MAP::MapEvent(_serviceManager->_eventTable, event, ID_WINDOW);
	switch (ccode)
	{
	case OPT_DELETE:
	case OPT_ADD:
	case OPT_MOVE_UP:
	case OPT_MOVE_DOWN:
		{
		// Dispatch to the object editor.
		UI_EVENT pEvent(ccode);
		ccode = Root()->Event(pEvent);
		}
		break;

	default:
		ccode = UIW_VT_LIST::Event(event);
		break;
	}
	return (ccode);
}

// --------------------------------------------------------------------------
// ----- ZAF_SERVICE_MANAGER ------------------------------------------------
// --------------------------------------------------------------------------

ZAF_SERVICE_MANAGER::ZAF_SERVICE_MANAGER(void) :
	// base initialization
	UIW_WINDOW(_className, ZAF_SERVICE_MANAGER::Storage(&_storage, "p_servic"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable),
	// member initialization
	msgTable(ZAF_SERVICE_MANAGER::_msgTableName, ZAF_SERVICE_MANAGER::Storage(&_storage, "p_servic")),
	serviceList()
{
	// Set the identification and application name.
	searchID = ID_SERVICE_MANAGER;
	windowManager->Information(I_SET_TEXT, msgTable.GetMessage(ZMSG_APPLICATION_TAG));

	// Center the window on the screen.
	windowManager->Center(this);
	relative.bottom = relative.Height() - 1;
	relative.top = 0;

	// Set the exit function.
	windowManager->exitFunction = Exit;

	// Set the error & help systems system
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = _storage;

	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem->ResetStorage(_storage, FALSE);

	// Display the demo copyright.
#if 0
	ZIL_ICHAR demoCopyright[32];
	NormalizeString(demoCopyright, "COPYRIGHT");
	ZAF_DIALOG_WINDOW window(demoCopyright, _storage,
		ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable);
	windowManager->Center(&window);
	if (window.Control() == ZIL_DLG_CANCEL)
	{
		delete display;
		exit(1);
	}
#endif

	defaultStorage = saveStorage;
}

ZAF_SERVICE_MANAGER::~ZAF_SERVICE_MANAGER(void)
{
	// Remove the service modules
	if (windowManager)
	{
		for (SERVICE_ELEMENT *service = serviceList.First(); service; service = service->Next())
			if (windowManager->Index(service->window) != -1)
				*windowManager - service->window;
		*windowManager - this;
	}

	// Remove all windows from the window manager.
	for (UI_WINDOW_OBJECT *object = windowManager->First(); object; object = windowManager->First())
	{
		*windowManager - object;
		if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
			delete object;
	}

	// Remove the storage.
	if (_storage)
		delete _storage;

	// Delete associated services
	delete UI_WINDOW_OBJECT::errorSystem;
	delete UI_WINDOW_OBJECT::helpSystem;
}

UI_WINDOW_OBJECT *ZAF_SERVICE_MANAGER::Add(UI_WINDOW_OBJECT *window)
{
	// Create a service element.
	window->woAdvancedFlags |= WOAF_NO_DESTROY;
	serviceList.Add(new SERVICE_ELEMENT(window));

	// Add the service to the window manager.
	if (FlagSet(window->woStatus, WOS_REDISPLAY))
	{
		*windowManager + window;
		if (!FlagSet(window->woAdvancedFlags, WOAF_LOCKED))
			windowManager->screenID = window->screenID;
	}
	return (window);
}

UIW_VT_LIST *ZAF_SERVICE_MANAGER::CtrlList(UIW_WINDOW *window, ZIL_NUMBERID fieldID)
{
	// Replace sub-list with derived object (TEMPORARY PATCH).
	UI_WINDOW_OBJECT *replaceField = window->Get(fieldID);
	ZAF_SUBLIST *ctrlList = new ZAF_SUBLIST(replaceField->relative.left,
		replaceField->relative.top, replaceField->relative.Width(),
		replaceField->relative.Height());
	ctrlList->NumberID(replaceField->NumberID());
	ctrlList->StringID(replaceField->StringID());
	window->Subtract(replaceField);
	delete replaceField;
	window->Add(ctrlList);
	window->Add(ctrlList); // Make it the current object.
	return (ctrlList);
}

EVENT_TYPE ZAF_SERVICE_MANAGER::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UIW_WINDOW::DrawItem(event, ccode);

	UI_REGION region = clipList.First()->region;
	for (SERVICE_ELEMENT *service = serviceList.First(); service; service = service->Next())
	{
		ZIL_ICHAR *serviceName = service->ClassName();
		if (serviceName)
		{
			region.bottom = region.top + display->cellHeight - display->preSpace - display->postSpace;
			DrawText(screenID, region, serviceName, lastPalette, FALSE, ccode);
			region.top = region.bottom + 1;
		}
	}

	return (FALSE);
}

EVENT_TYPE ZAF_SERVICE_MANAGER::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	if (ccode == OPT_ACTIVATE_SERVICE)
	{
		UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
		object = (UI_WINDOW_OBJECT *)Information(I_GET_OBJECTID_SERVICE, &event.windowObject->userFlags);
		if (object)
		{
			// Restore the requested editor.
			UI_EVENT minimizeEvent(L_RESTORE);
			UI_WINDOW_OBJECT *minimizeObject = windowManager->First();
			*windowManager + object;
			if (FlagSet(object->woStatus, WOS_MINIMIZED))
			{
				minimizeEvent.windowObject = object;
				object->Event(minimizeEvent);
			}

#if defined(ZIL_MACINTOSH)
			// Minimize the current editor (only on Macintosh).
			minimizeEvent.type = L_MINIMIZE;
			minimizeEvent.windowObject = minimizeObject;
			minimizeObject->Event(minimizeEvent);
#endif
		}
	}
	else
		ccode = UIW_WINDOW::Event(event);

	return (ccode);
}

EVENT_TYPE ZAF_SERVICE_MANAGER::Exit(UI_DISPLAY *, UI_EVENT_MANAGER *, UI_WINDOW_MANAGER *windowManager)
{
	// Check for an end-of-application response.
	if (_queuedEvent == L_EXIT)
		return (L_EXIT);

	// See if there is an edit file.
	ZIL_STORAGE *editFile = ZIL_NULLP(ZIL_STORAGE);
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	if (storageService && _changedData)
		storageService->Information(I_GET_STORAGE, &editFile);

	if (editFile)
	{
		// See if the edit file has a valid name.
		ZIL_ICHAR name[ZIL_MAXPATHLEN], pathName[ZIL_MAXPATHLEN];
		editFile->StorageName(pathName);
		editFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);

		// Display the appropriate exit message.
		UI_ERROR_STUB::Beep();
		ZIL_ICHAR windowName[32];
		NormalizeString(windowName, "SAVE_APPLICATION");
		ZAF_DIALOG_WINDOW *window = new ZAF_DIALOG_WINDOW(windowName, _storage,
			ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable);

		ZIL_ICHAR *format;
		UI_WINDOW_OBJECT *field = window->Get(FIELD_FILE_SAVE);
		field->Information(I_GET_TEXT, &format);
		sprintf(pathName, format, name);
		field->Information(I_SET_TEXT, pathName);

		windowManager->Center(window);
		*windowManager + window;
		EVENT_TYPE ccode = window->Control();
		*windowManager - window;
		delete window;

		// Save & exit according to the return value.
		if (ccode == ZIL_DLG_YES && !stricmp(name, _serviceManager->ZMSG_tempFileName()))
		{
			_queuedEvent = L_EXIT;
			windowManager->First()->Event(OPT_FILE_SAVE);
			return (S_CONTINUE);
		}
		else if (ccode == ZIL_DLG_YES)
			windowManager->First()->Event(OPT_FILE_SAVE);
		if (ccode == ZIL_DLG_CANCEL)
			return (S_CONTINUE);
	}

	_queuedEvent = L_EXIT;
	return (L_EXIT);
}

void *ZAF_SERVICE_MANAGER::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_GET_CLASSNAME_SERVICE:
		{
		// Try to find the named service.
		ZIL_ICHAR *requestedService = (ZIL_ICHAR *)data;
		for (SERVICE_ELEMENT *service = serviceList.First(); service; service = service->Next())
		{
			ZIL_ICHAR *serviceName = service->window->ClassName();
			if (serviceName && !stricmp(serviceName, requestedService))
				return (service->window);
		}
		return (ZIL_NULLP(void));
		}

	case I_GET_OBJECTID_SERVICE:
		{
		// Try to find the named service.
		ZIL_OBJECTID requestedService = *(ZIL_OBJECTID *)data;
		for (SERVICE_ELEMENT *service = serviceList.First(); service; service = service->Next())
			if (service->window->SearchID() == requestedService)
				return (service->window);
		return (ZIL_NULLP(void));
		}

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return match information.
	return (data);
}

void ZAF_SERVICE_MANAGER::StatusUpdate(ZIL_ICHAR *title, ZIL_ICHAR *format, ...)
{
	static UIW_WINDOW *statusWindow = ZIL_NULLP(UIW_WINDOW);
	static UI_WINDOW_OBJECT *statusField = ZIL_NULLP(UI_WINDOW_OBJECT);

	// Set-up the window name.
	if (!format && !title && statusWindow)
	{
		// Clear the window.
		*windowManager - statusWindow;
		delete statusWindow;
		statusWindow = ZIL_NULLP(UIW_WINDOW);
		statusField = ZIL_NULLP(UI_WINDOW_OBJECT);
		return;
	}
	else if (!format && !title)
		return;

	// Format the message.
	ZIL_ICHAR message[ZIL_MAXPARAMLEN];
	if (format)
	{
		va_list args;
	 	va_start(args, format);
		ZIL_STDARG::vsprintf(message, format, args);
		va_end(args);
	}

	// Display the message information.
	if (!statusWindow)
	{
		ZIL_ICHAR windowName[32];
		NormalizeString(windowName, "STATUS_WINDOW");
		statusWindow = new UIW_WINDOW(windowName, _storage,
			ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable);
		windowManager->Center(statusWindow);
		statusField = statusWindow->Get(STATUS_FIELD);
		if (title)
			statusWindow->Information(I_SET_TEXT, title);
		if (format)
			statusField->Information(I_SET_TEXT, message);
		*windowManager + statusWindow;
	}
	else
	{
		if (title)
			statusWindow->Information(I_SET_TEXT, title);
		if (format)
			statusField->Information(I_SET_TEXT, message);
	}

#if defined(ZIL_MSWINDOWS)
	UpdateWindow(statusWindow->screenID);
#elif defined(ZIL_OS2)
	WinUpdateWindow(statusWindow->screenID);
#endif
}

void ZAF_SERVICE_MANAGER::UpdateToolsMenus(void)
{
	static const ZIL_ICHAR toolsMenuID[] = { 'T', 'O', 'O', 'L', 'S', 0 };
	for (SERVICE_ELEMENT *service = serviceList.First(); service; service = service->Next())
	{
		UI_WINDOW_OBJECT *toolsMenu = service->window->Get(toolsMenuID);
		if (toolsMenu)
		{
			// Add a menu item corresponding to each service available.
			UI_EVENT addEvent(S_ADD_OBJECT);
			for (SERVICE_ELEMENT *toolsService = serviceList.First(); toolsService; toolsService = toolsService->Next())
			{
				if (service != toolsService && toolsService->window->Get(toolsMenuID))
				{
					ZIL_ICHAR *serviceTitle = ZIL_NULLP(ZIL_ICHAR);
					toolsService->window->Information(I_GET_ORIGINAL_TITLE, &serviceTitle);
					addEvent.windowObject = new UIW_POP_UP_ITEM(serviceTitle, MNIF_SEND_MESSAGE, BTF_NO_3D,
						WOF_NO_FLAGS, ZIL_NULLF(ZIL_USER_FUNCTION), OPT_ACTIVATE_SERVICE);
					addEvent.windowObject->userFlags = toolsService->window->SearchID();
					toolsMenu->Event(addEvent);
				}
			}
		}
	}
}

ZIL_STORAGE_READ_ONLY *ZAF_SERVICE_MANAGER::Storage(ZIL_STORAGE_READ_ONLY **_storage, const char *moduleName)
{
	// Initialize the appropriate file extensions.
	static ZIL_ICHAR _znc[] = { '.','z','n','c',0 };
	static ZIL_ICHAR _txt[] = { '.','z','_','t',0 };

	// Open the module.
	ZIL_STORAGE_READ_ONLY *storage = *_storage;
	if (!storage)
	{
		// Open the appropriate file system.
		ZIL_ICHAR fileName[ZIL_MAXPATHLEN];
		if (display->isText)
		{
			NormalizeString(fileName, moduleName);
			ZIL_INTERNATIONAL::strcat(fileName, _txt);
			*_storage = storage = new ZIL_STORAGE_READ_ONLY(fileName);
		}
		if (storage && storage->storageError)
		{
			delete storage;
			*_storage = storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
		}

		// Open the default file.
		if (!storage)
		{
			NormalizeString(fileName, moduleName);
			ZIL_INTERNATIONAL::strcat(fileName, _znc);
			*_storage = storage = new ZIL_STORAGE_READ_ONLY(fileName);
		}
		if (storage->storageError) // check for application error.
		{
			ZIL_ICHAR message[128];
			NormalizeString(message, "Critical Error!  The file \"%s\" cannot be located.");
			if (errorSystem)
				errorSystem->ReportError(windowManager, WOS_NO_STATUS, message, fileName);
			else
				UI_ERROR_STUB::Beep();
			delete display;
			exit(-1);
		}
	}
	return (storage);
}

UI_WINDOW_OBJECT *ZAF_SERVICE_MANAGER::Subtract(UI_WINDOW_OBJECT *window)
{
	UI_WINDOW_OBJECT *next = ZIL_NULLP(UI_WINDOW_OBJECT);
	for (SERVICE_ELEMENT *service = (SERVICE_ELEMENT *)serviceList.First();
		service; service = (SERVICE_ELEMENT *)service->Next())
		if (service->window == window)
		{
			next = ((SERVICE_ELEMENT *)(service->Next()))->window;
			serviceList.Subtract(service);
		}
	return (next);
}

// ----- General message requests -------------------------------------------

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_allFilter(void)
{
	return (_allFilter);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_allObjects(void)
{
	return (_allObjects);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_configName(void)
{
	return (_configName);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_configPath(void)
{
	return (_configPath);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_datFilter(void)
{
	return (_datFilter);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_fileOption(void)
{
	return (_fileOption);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_mode(void)
{
#	if defined(ZIL_UNICODE)
	return (msgTable.GetMessage(ZMSG_MODE_UNICODE));
#	elif defined(OEM)
	return (msgTable.GetMessage(ZMSG_MODE_OEM));
#	else
	return (msgTable.GetMessage(ZMSG_MODE_ISO));
#	endif
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_msgTableName(void)
{
	return (_msgTableName);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_none(void)
{
	return (msgTable.GetMessage(ZMSG_NONE));
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_newline(void)
{
	return (_newline);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_noFile(void)
{
	return (msgTable.GetMessage(ZMSG_NO_FILE));
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_tempResourceName(void)
{
	static ZIL_ICHAR _tempName[256];

	ZIL_DATE date; int month, day;
	date.Export(ZIL_NULLP(int), &month, &day);
	ZIL_TIME time; int hour, minute, second;
	time.Export(&hour, &minute, &second);
	sprintf(_tempName, msgTable.GetMessage(ZMSG_TEMP_RESOURCENAME), month, day, hour, minute, second);
	return (_tempName);
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_tempFileName(void)
{
	return (msgTable.GetMessage(ZMSG_TEMP_FILENAME));
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_undefined(void)
{
	return (msgTable.GetMessage(ZMSG_UNDEFINED));
}

ZIL_ICHAR *ZAF_SERVICE_MANAGER::ZMSG_untitled(void)
{
	return (msgTable.GetMessage(ZMSG_UNTITLED));
}

// --------------------------------------------------------------------------
// ----- Static Member Variables --------------------------------------------
// --------------------------------------------------------------------------

int ZAF_SERVICE_MANAGER::_changedData = FALSE;
EVENT_TYPE ZAF_SERVICE_MANAGER::_queuedEvent = 0;
int ZAF_SERVICE_MANAGER::_testMode = FALSE;

ZIL_ICHAR ZAF_SERVICE_MANAGER::_allFilter[] = { '*','.','*',0 };
ZIL_ICHAR ZAF_SERVICE_MANAGER::_allObjects[] = { '*', 0 };
ZIL_ICHAR ZAF_SERVICE_MANAGER::_configName[] = { 'z','i','n','c','.','c','f','g',0 };
ZIL_ICHAR ZAF_SERVICE_MANAGER::_configPath[] = { '~','C','O','N','F','I','G',0 };
ZIL_ICHAR ZAF_SERVICE_MANAGER::_datFilter[] = { '*','.','d','a','t',0 };
ZIL_ICHAR ZAF_SERVICE_MANAGER::_fileOption[] = { 'F','I','L','E',0 };
ZIL_ICHAR ZAF_SERVICE_MANAGER::_msgTableName[] = { 'M','S','G','_','T','A','B','L','E',0 };
#if defined(ZIL_MACINTOSH)
	ZIL_ICHAR ZAF_SERVICE_MANAGER::_newline[] = { '\r',0 };
#elif defined(ZIL_MOTIF)
	ZIL_ICHAR ZAF_SERVICE_MANAGER::_newline[] = { '\n',0 };
#else
	ZIL_ICHAR ZAF_SERVICE_MANAGER::_newline[] = { '\r','\n',0 };
#endif

ZIL_ICHAR ZAF_SERVICE_MANAGER::_className[] = { 'Z','A','F','_','S','E','R','V','I','C','E','_','M','A','N','A','G','E','R',0 };

ZIL_STORAGE_READ_ONLY *ZAF_SERVICE_MANAGER::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

ZAF_SERVICE_MANAGER *_serviceManager = ZIL_NULLP(ZAF_SERVICE_MANAGER);

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_SERVICE_MANAGER::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_ICON,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_ICON::New), UIW_ICON::_className, 0 },
	{ ID_MAXIMIZE_BUTTON,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MAXIMIZE_BUTTON::New), UIW_MAXIMIZE_BUTTON::_className, 0 },
	{ ID_MINIMIZE_BUTTON,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MINIMIZE_BUTTON::New), UIW_MINIMIZE_BUTTON::_className, 0 },
	{ ID_POP_UP_ITEM,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_ITEM::New), UIW_POP_UP_ITEM::_className, 0 },
	{ ID_POP_UP_MENU,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_MENU::New), UIW_POP_UP_MENU::_className, 0 },
	{ ID_PULL_DOWN_ITEM,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_ITEM::New), UIW_PULL_DOWN_ITEM::_className, 0 },
	{ ID_PULL_DOWN_MENU,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_MENU::New), UIW_PULL_DOWN_MENU::_className, 0 },
	{ ID_SCROLL_BAR,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SCROLL_BAR::New), UIW_SCROLL_BAR::_className, 0 },
	{ ID_STRING,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TEXT,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TEXT::New), UIW_TEXT::_className, 0 },
	{ ID_TITLE,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_WINDOW,  (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_SERVICE_MANAGER::_objectTable = __objectTable;

static UI_EVENT_MAP ZIL_FARDATA __eventTable[] =
{
#if defined(ZIL_MSDOS)
	{ ID_WINDOW,	OPT_DELETE,		E_KEY, 		CTRL_WHITE_DELETE,		S_CTRL },
	{ ID_WINDOW, 	OPT_DELETE,		E_KEY, 		CTRL_GRAY_DELETE,		S_CTRL },
	{ ID_WINDOW, 	OPT_DELETE,		E_KEY, 		ALT_D, 					S_ALT },
	{ ID_WINDOW,	OPT_MOVE_UP,	E_KEY, 		CTRL_WHITE_UP_ARROW,	S_CTRL },
	{ ID_WINDOW, 	OPT_MOVE_UP,	E_KEY, 		CTRL_GRAY_UP_ARROW, 	S_CTRL },
	{ ID_WINDOW, 	OPT_MOVE_UP,	E_KEY, 		ALT_A, 				 	S_ALT },
	{ ID_WINDOW,	OPT_MOVE_DOWN,	E_KEY, 		CTRL_WHITE_DOWN_ARROW,	S_CTRL },
	{ ID_WINDOW, 	OPT_MOVE_DOWN,	E_KEY, 		CTRL_GRAY_DOWN_ARROW, 	S_CTRL },
	{ ID_WINDOW, 	OPT_MOVE_DOWN,	E_KEY, 		ALT_Z, 					S_ALT },
#elif defined(ZIL_MSWINDOWS)
	{ ID_WINDOW, 	OPT_DELETE,		WM_KEYDOWN,	CTRL_WHITE_DELETE },
	{ ID_WINDOW, 	OPT_DELETE,		WM_KEYDOWN,	CTRL_GRAY_DELETE },
	{ ID_WINDOW, 	OPT_MOVE_UP,	WM_KEYDOWN, CTRL_WHITE_UP_ARROW },
	{ ID_WINDOW, 	OPT_MOVE_UP,	WM_KEYDOWN, CTRL_GRAY_UP_ARROW },
	{ ID_WINDOW, 	OPT_MOVE_DOWN,	WM_KEYDOWN, CTRL_WHITE_DOWN_ARROW },
	{ ID_WINDOW, 	OPT_MOVE_DOWN,	WM_KEYDOWN, CTRL_GRAY_DOWN_ARROW },
#elif defined(ZIL_OS2)
	{ ID_WINDOW,	OPT_DELETE,		WM_CHAR,	CTRL_WHITE_DELETE },
	{ ID_WINDOW, 	OPT_DELETE,		WM_CHAR,	CTRL_GRAY_DELETE },
	{ ID_WINDOW,	OPT_MOVE_UP,	WM_CHAR, 	CTRL_WHITE_UP_ARROW },
	{ ID_WINDOW, 	OPT_MOVE_UP,	WM_CHAR, 	CTRL_GRAY_UP_ARROW },
	{ ID_WINDOW,	OPT_MOVE_DOWN,	WM_CHAR, 	CTRL_WHITE_DOWN_ARROW },
	{ ID_WINDOW, 	OPT_MOVE_DOWN,	WM_CHAR, 	CTRL_GRAY_DOWN_ARROW },
#elif defined(ZIL_MOTIF)
#	if defined(__hpux)
	{ ID_WINDOW,	OPT_DELETE,		KeyPress,	osfXK_Delete, ControlMask },
	{ ID_WINDOW,	OPT_DELETE,		KeyPress,	osfXK_BackSpace, ControlMask },
	{ ID_WINDOW,	OPT_MOVE_UP,	KeyPress, 	osfXK_Up, ControlMask },
	{ ID_WINDOW,	OPT_MOVE_DOWN,	KeyPress, 	osfXK_Down, ControlMask },
#	endif
	{ ID_WINDOW,	OPT_DELETE,		KeyPress,	XK_Delete, ControlMask },
	{ ID_WINDOW,	OPT_DELETE,		KeyPress,	XK_BackSpace, ControlMask },
	{ ID_WINDOW,	OPT_MOVE_UP,	KeyPress, 	XK_Up, ControlMask },
	{ ID_WINDOW,	OPT_MOVE_DOWN,	KeyPress, 	XK_Down, ControlMask },
#elif defined(ZIL_CURSES)
	{ ID_WINDOW, 	OPT_DELETE,		E_KEY, 		BACKSPACE },
	{ ID_WINDOW, 	OPT_DELETE,		E_KEY, 		'd', S_ALT },
	{ ID_WINDOW,	OPT_MOVE_UP,	E_KEY, 		UP_ARROW, S_ALT },
	{ ID_WINDOW, 	OPT_MOVE_UP,	E_KEY, 		'a', S_ALT },
	{ ID_WINDOW,	OPT_MOVE_DOWN,	E_KEY, 		DOWN_ARROW, S_ALT },
	{ ID_WINDOW, 	OPT_MOVE_DOWN,	E_KEY, 		'z', S_ALT },
#elif defined(ZIL_MACINTOSH)
	{ ID_WINDOW,	OPT_DELETE,		keyDown,	WHITE_DELETE,			S_OPT },
	{ ID_WINDOW, 	OPT_DELETE,		keyDown,	GRAY_DELETE,			S_OPT },
	{ ID_WINDOW,	OPT_MOVE_UP,	keyDown, 	WHITE_UP_ARROW,			S_OPT },
	{ ID_WINDOW, 	OPT_MOVE_UP,	keyDown, 	GRAY_UP_ARROW,			S_OPT },
	{ ID_WINDOW,	OPT_MOVE_DOWN,	keyDown, 	WHITE_DOWN_ARROW,		S_OPT },
	{ ID_WINDOW, 	OPT_MOVE_DOWN,	keyDown, 	GRAY_DOWN_ARROW,		S_OPT },
#elif defined(ZIL_NEXTSTEP)
	{ ID_WINDOW,	OPT_DELETE,		NX_KEYDOWN,		BACKSPACE, },
	{ ID_WINDOW,	OPT_MOVE_UP,	NX_KEYDOWN, 	WHITE_UP_ARROW,	 },
	{ ID_WINDOW,	OPT_MOVE_DOWN,	NX_KEYDOWN, 	WHITE_DOWN_ARROW, },
#endif
	{ ID_END, 0, 0, 0 }	// End of array.
};
UI_EVENT_MAP *ZAF_SERVICE_MANAGER::_eventTable = __eventTable;
