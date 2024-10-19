//	Zinc Interface Library - LOCALE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_ZAF_LOCALE_WINDOW
#include "p_i18n.hpp"

static ZIL_ICHAR _tableName[] = { 'L','O','C','A','L','E',0 };

// ----- ZAF_LOCALE_WINDOW ------------------------------------------------

static ZIL_ICHAR _localeWindow[] = { 'Z','A','F','_','L','O','C','A','L','E','_','W','I','N','D','O','W',0 };
static ZIL_ICHAR _dateName[] = { 'L','O','C','_','D','A','T','E',0 };
static ZIL_ICHAR _timeName[] = { 'L','O','C','_','T','I','M','E',0 };
static ZIL_ICHAR _numberName[] = { 'L','O','C','_','N','U','M','B','E','R',0 };
static ZIL_ICHAR _currencyName[] = { 'L','O','C','_','C','U','R','R','E','N','C','Y',0 };

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined (__linux__)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_LOCALE_WINDOW::x
#else
#	define ZIL_PROCESS_REFERENCE(x)	x
#endif

ZAF_LOCALE_WINDOW::PROCESS ZAF_LOCALE_WINDOW::_processTable[] =
{
	{ _dateName, 			ZIL_PROCESS_REFERENCE(LOC_DATE_DataLoad), 		ZIL_PROCESS_REFERENCE(LOC_DATE_DataStore) },
	{ _numberName, 			ZIL_PROCESS_REFERENCE(LOC_NUMBER_DataLoad), 	ZIL_PROCESS_REFERENCE(LOC_NUMBER_DataStore) },
	{ _timeName, 			ZIL_PROCESS_REFERENCE(LOC_TIME_DataLoad), 		ZIL_PROCESS_REFERENCE(LOC_TIME_DataStore) },
	{ _currencyName, 		ZIL_PROCESS_REFERENCE(LOC_CURRENCY_DataLoad), 	ZIL_PROCESS_REFERENCE(LOC_CURRENCY_DataStore) },

	{ ZIL_NULLP(ZIL_ICHAR), 0, 0 }
};

ZAF_LOCALE_WINDOW::ZAF_LOCALE_WINDOW(ZIL_ICHAR *name) :
	UIW_WINDOW(_localeWindow, ZAF_I18N_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT), ZAF_I18N_EDITOR::_objectTable,
	ZAF_I18N_EDITOR::_userTable)
{
	// Set the help context title.
	searchID = ID_I18N_WINDOW;
	if (!name)
	{
		StringID(_serviceManager->ZMSG_tempResourceName());
		Information(I_SET_TEXT, StringID());
	}
	else
		Information(I_SET_TEXT, StringID(name));

	// Center the window on the screen.
	windowManager->Center(this);

	// Add the notebook items.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	notebook = new UIW_NOTEBOOK;
	for (int i = 0; _processTable[i].className; i++)
		*notebook + new UIW_WINDOW(_processTable[i].className,
			ZAF_I18N_EDITOR::_storage, ZIL_NULLP(ZIL_STORAGE_OBJECT),
			ZAF_I18N_EDITOR::_objectTable, ZAF_I18N_EDITOR::_userTable);
	Add(notebook);
	ZIL_STORAGE *localeFile = ZIL_NULLP(ZIL_STORAGE);
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	storageService->Information(I_GET_STORAGE, &localeFile);
	Load(name, localeFile);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}

ZAF_LOCALE_WINDOW::~ZAF_LOCALE_WINDOW(void)
{
}

EVENT_TYPE ZAF_LOCALE_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_STORE:
		// Let the i18n editor handle the store request.
		ZAF_I18N_EDITOR::_i18nDirectory = ZAF_I18N_EDITOR::_localeDirectory;
		_serviceManager->Get(ID_I18N_EDITOR)->Event(OPT_RESOURCE_STORE);
		break;

	case OPT_STORE_AS:
		// Let the i18n editor handle the store-as request.
		ZAF_I18N_EDITOR::_i18nDirectory = ZAF_I18N_EDITOR::_localeDirectory;
		_serviceManager->Get(ID_I18N_EDITOR)->Event(OPT_RESOURCE_STOREAS);
		break;

	case OPT_CLOSE:
		// Close the window.
		eventManager->Put(S_CLOSE);
		break;

	case OPT_DEFAULT:
		{
		Load(ZAF_I18N_EDITOR::_defaultLocale, ZAF_I18N_EDITOR::_storage);
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			object->Event(S_REDISPLAY); // Reflect default changes to the notebook.
		}
		break;

	case OPT_HELP:
		// Call the help system.
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_DEINITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (!ZAF_SERVICE_MANAGER::_queuedEvent)
			_serviceManager->Get(ID_I18N_EDITOR)->Information(I_STATUS_CLEAR, ZIL_NULLP(void));
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(ZAF_I18N_EDITOR::_storage, FALSE);
		_serviceManager->Get(ID_I18N_EDITOR)->Information(I_STATUS_UPDATE, this);
		break;

	default:
		// Default to the base UIW_WINDOW class.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void ZAF_LOCALE_WINDOW::GetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS *currentFlags, UIF_FLAGS flag)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!FlagSet(*currentFlags, flag) && FlagSet(field->woStatus, WOS_SELECTED))
		*currentFlags |= flag;
	else if (FlagSet(*currentFlags, flag) && !FlagSet(field->woStatus, WOS_SELECTED))
		*currentFlags &= ~flag;
}

void ZAF_LOCALE_WINDOW::GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	ZIL_ICHAR *newText;
	field->Information(I_GET_TEXT, &newText);
	strcpy(text, newText);
}

void ZAF_LOCALE_WINDOW::GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR **text)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (*text)
		delete *text;
	field->Information(I_GET_TEXT, text);
	*text = strdup(*text);
}

void ZAF_LOCALE_WINDOW::GetValue(ZIL_NUMBERID fieldID, int *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	field->Information(I_GET_VALUE, value);
}

void *ZAF_LOCALE_WINDOW::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	case I_GET_FLAGS:
		// Get the i18n flag settings.
		*(I18_FLAGS *)data = I18_LOCALE_SET;
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}
	return (data);
}

void ZAF_LOCALE_WINDOW::SetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS currentFlags, UIF_FLAGS flag)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field)
		;
	else if (FlagSet(currentFlags, flag) || (!currentFlags && !flag))
		field->woStatus |= WOS_SELECTED;
	else
		field->woStatus &= ~WOS_SELECTED;
}

void ZAF_LOCALE_WINDOW::SetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	field->Information(I_SET_TEXT, text);
}

void ZAF_LOCALE_WINDOW::SetValue(ZIL_NUMBERID fieldID, int value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	field->Information(I_SET_VALUE, &value);
}

EVENT_TYPE ZAF_LOCALE_WINDOW::CheckFlagSelection(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		// Turn off all other related flags.
		UI_WINDOW_OBJECT *sibling;
		ZIL_OBJECTID searchID = object->SearchID();
		object->woStatus |= WOS_SELECTED;
		for (sibling = object->Previous(); sibling && searchID == sibling->SearchID(); sibling = sibling->Previous())
			if (FlagSet(sibling->woStatus, WOS_SELECTED))
			{
				sibling->woStatus &= ~WOS_SELECTED;
				sibling->Information(I_CHANGED_STATUS, ZIL_NULLP(void));
				if (FlagSet(sibling->woStatus, WOS_REDISPLAY))
					sibling->Event(S_REDISPLAY);
				sibling->woStatus |= WOS_CHANGED;
			}
		for (sibling = object->Next(); sibling && searchID == sibling->SearchID(); sibling = sibling->Next())
			if (FlagSet(sibling->woStatus, WOS_SELECTED))
			{
				sibling->woStatus &= ~WOS_SELECTED;
				sibling->Information(I_CHANGED_STATUS, ZIL_NULLP(void));
				if (FlagSet(sibling->woStatus, WOS_REDISPLAY))
					sibling->Event(S_REDISPLAY);
				sibling->woStatus |= WOS_CHANGED;
			}
	}
	return (ccode);
}

EVENT_TYPE ZAF_LOCALE_WINDOW::StringCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		UI_WINDOW_OBJECT *stringEditor = _serviceManager->Get(ID_STRING_EDITOR);
		stringEditor->Information(I_SET_REQUESTOR, object);
		object->windowManager->Add(stringEditor);
	}
	return (ccode);
}

void ZAF_LOCALE_WINDOW::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *localeFile,
	ZIL_STORAGE_OBJECT_READ_ONLY *, UI_ITEM *, UI_ITEM *)
{
	// Get the locale name.
	if (!name)
		Information(I_GET_TEXT, (void *)&name);

	// Move to the locale directory.
	if (localeFile)
	{
		ZIL_I18N::Traverse(localeFile, ZAF_I18N_EDITOR::_localeDirectory);
		localeFile->ChDir(name);
	}
	else
		localeFile = ZAF_I18N_EDITOR::_storage;
	ZIL_I18N::Traverse(ZAF_I18N_EDITOR::_storage, ZAF_I18N_EDITOR::_localeDirectory);
	ZAF_I18N_EDITOR::_storage->ChDir(ZAF_I18N_EDITOR::_defaultLocale);

	// Create the table with default information.
	ZIL_STORAGE_READ_ONLY *file = localeFile;
	if (!file->FindFirstObject(_tableName))
		file = ZAF_I18N_EDITOR::_storage;
	ZIL_STORAGE_OBJECT_READ_ONLY loadObject(*file, _tableName, 0);

	// Load the locale data.
	if (!loadObject.objectError)
	{
		ZIL_LOCALE locale;
		locale.ClassLoadData(&loadObject);
		table = &locale.data;

		for (int i = 0; _processTable[i].className; i++)
		{
			// Load the locale data.
			notebook->Information(I_SET_SEARCH_PAGE, &i);
			DataLoad = _processTable[i].Load;
			(this->*DataLoad)();
		}
	}
}

void ZAF_LOCALE_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *localeFile,
	ZIL_STORAGE_OBJECT *, UI_ITEM *, UI_ITEM *)
{
	// Check for an invalid file name.
	if (name)
		Information(I_SET_TEXT, StringID(name));
	else
		name = StringID();

	// Move to the locale directory.
	ZIL_I18N::Traverse(localeFile, ZAF_I18N_EDITOR::_localeDirectory, TRUE);
	if (localeFile->ChDir(name))
	{
		localeFile->MkDir(name);
		localeFile->ChDir(name);
	}
	ZIL_I18N::Traverse(ZAF_I18N_EDITOR::_storage, ZAF_I18N_EDITOR::_localeDirectory);
	ZAF_I18N_EDITOR::_storage->ChDir(ZAF_I18N_EDITOR::_defaultLocale);

	// Create the table with default information.
	ZIL_STORAGE_READ_ONLY *file = localeFile;
	if (!file->FindFirstObject(_tableName))
		file = ZAF_I18N_EDITOR::_storage;
	ZIL_STORAGE_OBJECT_READ_ONLY *loadObject = new ZIL_STORAGE_OBJECT_READ_ONLY(*file, _tableName, 0);

	// Store the locale data.
	if (!loadObject->objectError)
	{
		ZIL_LOCALE locale;
		locale.ClassLoadData(loadObject);
		delete loadObject;
		table = &locale.data;

		// Store the locale information.
		for (int i = 0; _processTable[i].className; i++)
		{
			// Store the locale data.
			notebook->Information(I_SET_SEARCH_PAGE, &i);
			DataStore = _processTable[i].Store;
			(this->*DataStore)();
		}

		// Store the table entry.
		ZIL_STORAGE_OBJECT storeObject(*localeFile, _tableName, 0, UIS_CREATE | UIS_READWRITE);
		locale.ClassStoreData(&storeObject);
	}
	else
		delete loadObject;
}
