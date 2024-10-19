//	Zinc Interface Library - LANGUAGE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_ZAF_LANGUAGE_WINDOW
#include "p_i18n.hpp"

// ----- ZAF_LANGUAGE_WINDOW ------------------------------------------------

static ZIL_ICHAR _languageWindow[] = { 'Z','A','F','_','L','A','N','G','U','A','G','E','_','W','I','N','D','O','W',0 };

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_LANGUAGE_WINDOW::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_LANGUAGE_WINDOW::PROCESS ZAF_LANGUAGE_WINDOW::_processTable[] =
{
	{ ZIL_UTIME::_dayName, 				ZIL_PROCESS_REFERENCE(ZIL_DAY_DataLoad), 			ZIL_PROCESS_REFERENCE(ZIL_DAY_DataStore) },
	{ ZIL_UTIME::_monthName, 			ZIL_PROCESS_REFERENCE(ZIL_MONTH_DataLoad), 			ZIL_PROCESS_REFERENCE(ZIL_MONTH_DataStore) },
	{ ZIL_UTIME::_timeName, 			ZIL_PROCESS_REFERENCE(ZIL_TIME_DataLoad), 			ZIL_PROCESS_REFERENCE(ZIL_TIME_DataStore) },

	{ UIW_DATE::_className, 			ZIL_PROCESS_REFERENCE(UIW_DATE_DataLoad), 			ZIL_PROCESS_REFERENCE(UIW_DATE_DataStore) },
	{ UIW_INTEGER::_className, 			ZIL_PROCESS_REFERENCE(UIW_INTEGER_DataLoad), 		ZIL_PROCESS_REFERENCE(UIW_INTEGER_DataStore) },
	{ UIW_BIGNUM::_className, 			ZIL_PROCESS_REFERENCE(UIW_BIGNUM_DataLoad), 		ZIL_PROCESS_REFERENCE(UIW_BIGNUM_DataStore) },
	{ UIW_REAL::_className, 			ZIL_PROCESS_REFERENCE(UIW_REAL_DataLoad), 			ZIL_PROCESS_REFERENCE(UIW_REAL_DataStore) },
	{ UIW_SYSTEM_BUTTON::_className,	ZIL_PROCESS_REFERENCE(UIW_SYSTEM_BUTTON_DataLoad),	ZIL_PROCESS_REFERENCE(UIW_SYSTEM_BUTTON_DataStore) },
	{ UIW_TIME::_className, 			ZIL_PROCESS_REFERENCE(UIW_TIME_DataLoad), 			ZIL_PROCESS_REFERENCE(UIW_TIME_DataStore) },
	{ UIW_WINDOW::_className, 			ZIL_PROCESS_REFERENCE(UIW_WINDOW_DataLoad),			ZIL_PROCESS_REFERENCE(UIW_WINDOW_DataStore) },

	{ UI_ERROR_SYSTEM::_className,		ZIL_PROCESS_REFERENCE(UI_ERROR_SYSTEM_DataLoad),	ZIL_PROCESS_REFERENCE(UI_ERROR_SYSTEM_DataStore) },
	{ UI_HELP_SYSTEM::_className, 		ZIL_PROCESS_REFERENCE(UI_HELP_SYSTEM_DataLoad), 	ZIL_PROCESS_REFERENCE(UI_HELP_SYSTEM_DataStore) },

	{ ZAF_MESSAGE_WINDOW::_className, 	ZIL_PROCESS_REFERENCE(ZAF_MESSAGE_WINDOW_DataLoad),	ZIL_PROCESS_REFERENCE(ZAF_MESSAGE_WINDOW_DataStore) },

	{ ZIL_NULLP(ZIL_ICHAR), 0, 0 }
};

ZAF_LANGUAGE_WINDOW::ZAF_LANGUAGE_WINDOW(ZIL_ICHAR *name) :
	UIW_WINDOW(_languageWindow, ZAF_I18N_EDITOR::_storage,
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
	ZIL_STORAGE *languageFile = ZIL_NULLP(ZIL_STORAGE);
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	storageService->Information(I_GET_STORAGE, &languageFile);
	Load(name, languageFile);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}

ZAF_LANGUAGE_WINDOW::~ZAF_LANGUAGE_WINDOW(void)
{
}

EVENT_TYPE ZAF_LANGUAGE_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_STORE:
		// Let the i18n editor handle the store request.
		ZAF_I18N_EDITOR::_i18nDirectory = ZAF_I18N_EDITOR::_languageDirectory;
		_serviceManager->Get(ID_I18N_EDITOR)->Event(OPT_RESOURCE_STORE);
		break;

	case OPT_STORE_AS:
		// Let the i18n editor handle the store-as request.
		ZAF_I18N_EDITOR::_i18nDirectory = ZAF_I18N_EDITOR::_languageDirectory;
		_serviceManager->Get(ID_I18N_EDITOR)->Event(OPT_RESOURCE_STOREAS);
		break;

	case OPT_CLOSE:
		// Close the window.
		eventManager->Put(S_CLOSE);
		break;

	case OPT_DEFAULT:
		{
		Load(ZAF_I18N_EDITOR::_defaultLanguage, ZAF_I18N_EDITOR::_storage);
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

void ZAF_LANGUAGE_WINDOW::GetList(ZIL_NUMBERID fieldID, ZIL_NUMBERID *numberID)
{
	// Check for an empty table.
	if (!table)
		return;
	// Load the list.
	UI_WINDOW_OBJECT *list = notebook->Get(fieldID);
	UI_WINDOW_OBJECT *field = ZIL_NULLP(UI_WINDOW_OBJECT);
	list->Information(I_GET_FIRST, &field);
	for (int i = 0; numberID[i] && field; i++)
	{
		for (int j = 0; j < tableLength; j++)
			if (table[j].numberID == numberID[i])
			{
				if (table[j].text)
					delete table[j].text;
				field->Information(I_GET_TEXT, &table[j].text);
				table[j].text = strdup(table[j].text); // string must be duplicated.
				break;
			}
		field = field->Next();
	}
}

void ZAF_LANGUAGE_WINDOW::GetText(ZIL_NUMBERID fieldID, ZIL_NUMBERID numberID)
{
	// Check for an empty table.
	if (!table)
		return;
	// Load the text.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	for (int i = 0; i < tableLength; i++)
		if (numberID == table[i].numberID)
		{
			if (table[i].text)
				delete table[i].text;
			field->Information(I_GET_TEXT, &table[i].text);
			table[i].text = strdup(table[i].text); // string must be duplicated.
			break;
		}
}

void *ZAF_LANGUAGE_WINDOW::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	case I_GET_FLAGS:
		// Get the i18n flag settings.
		*(I18_FLAGS *)data = I18_LANGUAGE_SET;
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}
	return (data);
}

void ZAF_LANGUAGE_WINDOW::SetList(ZIL_NUMBERID fieldID, ZIL_NUMBERID *numberID)
{
	// Check for an empty table.
	if (!table)
		return;
	// Store the list.
	UI_EVENT event(S_ADD_OBJECT);
	UI_WINDOW_OBJECT *list = notebook->Get(fieldID);
	if (list->screenID)
		list->Information(I_DESTROY_LIST, ZIL_NULLP(void));
	for (int i = 0; numberID[i]; i++)
	{
		for (int j = 0; j < tableLength; j++)
			if (table[j].numberID == numberID[i])
			{
				event.windowObject = new UIW_BUTTON(0, 0, 0,
					table[j].text, BTF_NO_3D | BTF_DOUBLE_CLICK,
					WOF_NO_FLAGS, StringCallback);
				list->Event(event);
				break;
			}
	}
	if (list->screenID)
		list->Event(S_REDISPLAY);
}

void ZAF_LANGUAGE_WINDOW::SetText(ZIL_NUMBERID fieldID, ZIL_NUMBERID numberID)
{
	// Check for an empty table.
	if (!table)
		return;
	// Set the text.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	for (int i = 0; i < tableLength; i++)
		if (numberID == table[i].numberID)
		{
			field->Information(I_SET_TEXT, table[i].text);
			break;
		}
}

EVENT_TYPE ZAF_LANGUAGE_WINDOW::StringCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		UI_WINDOW_OBJECT *stringEditor = _serviceManager->Get(ID_STRING_EDITOR);
		stringEditor->Information(I_SET_REQUESTOR, object);
		stringEditor->Information(I_ACTIVATE_SERVICE, ZIL_NULLP(void));
	}
	return (ccode);
}

void ZAF_LANGUAGE_WINDOW::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *languageFile,
	ZIL_STORAGE_OBJECT_READ_ONLY *, UI_ITEM *, UI_ITEM *)
{
	// Get the language name.
	if (!name)
		Information(I_GET_TEXT, (void *)&name);

	// Move to the language directory.
	if (languageFile)
	{
		ZIL_I18N::Traverse(languageFile, ZAF_I18N_EDITOR::_languageDirectory);
		languageFile->ChDir(name);
	}
	else
		languageFile = ZAF_I18N_EDITOR::_storage;
	ZIL_I18N::Traverse(ZAF_I18N_EDITOR::_storage, ZAF_I18N_EDITOR::_languageDirectory);
	ZAF_I18N_EDITOR::_storage->ChDir(ZAF_I18N_EDITOR::_defaultLanguage);

	// Initialize the language information.
	for (int i = 0; _processTable[i].className; i++)
	{
		notebook->Information(I_SET_SEARCH_PAGE, &i);
		DataLoad = _processTable[i].Load;

		// Create the table with default information.
		tableLength = -1;
		table = ZIL_NULLP(ZIL_LANGUAGE_ELEMENT);
		ZIL_LANGUAGE language;
		ZIL_STORAGE_READ_ONLY *file = languageFile;
		if (!file->FindFirstObject(_processTable[i].className))
			file = ZAF_I18N_EDITOR::_storage;
		ZIL_STORAGE_OBJECT_READ_ONLY loadObject(*file, _processTable[i].className, 0);
		if (loadObject.objectError)
			continue; // error loading the data.

		// Load the language data.
		language.Load(ZIL_NULLP(ZIL_ICHAR), ZIL_NULLP(ZIL_STORAGE_READ_ONLY), &loadObject);
		table = language.data;
		tableLength = language.noOfElements;
		(this->*DataLoad)();
	}
}

void ZAF_LANGUAGE_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *languageFile,
	ZIL_STORAGE_OBJECT *, UI_ITEM *, UI_ITEM *)
{
	// Check for an invalid file name.
	if (name)
		Information(I_SET_TEXT, StringID(name));
	else
		name = StringID();

	// Move to the language directory.
	ZIL_I18N::Traverse(languageFile, ZAF_I18N_EDITOR::_languageDirectory, TRUE);
	if (languageFile->ChDir(name))
	{
		languageFile->MkDir(name);
		languageFile->ChDir(name);
	}
	ZIL_I18N::Traverse(ZAF_I18N_EDITOR::_storage, ZAF_I18N_EDITOR::_languageDirectory);
	ZAF_I18N_EDITOR::_storage->ChDir(ZAF_I18N_EDITOR::_defaultLanguage);

	// Store the language information.
	for (int i = 0; _processTable[i].className; i++)
	{
		notebook->Information(I_SET_SEARCH_PAGE, &i);
		DataStore = _processTable[i].Store;

		// Load a template of the table.
		tableLength = -1;
		table = ZIL_NULLP(ZIL_LANGUAGE_ELEMENT);
		ZIL_LANGUAGE language;
		ZIL_STORAGE_READ_ONLY *file = languageFile;
		if (!file->FindFirstObject(_processTable[i].className))
			file = ZAF_I18N_EDITOR::_storage;
		ZIL_STORAGE_OBJECT_READ_ONLY *loadObject = new ZIL_STORAGE_OBJECT_READ_ONLY(*file, _processTable[i].className, 0);
		if (loadObject->objectError)
		{
			delete loadObject;
			continue; // error loading the data.
		}

		// Load the default language data.
		language.Load(ZIL_NULLP(ZIL_ICHAR), ZIL_NULLP(ZIL_STORAGE_READ_ONLY), loadObject);
		delete loadObject;
		table = language.data;
		tableLength = language.noOfElements;

		// Store the new data to the table.
		(this->*DataStore)();

		// Store the object.
		ZIL_STORAGE_OBJECT storeObject(*languageFile, _processTable[i].className, 0, UIS_CREATE | UIS_READWRITE);
		ZIL_INT16 value = tableLength; storeObject.Store(value);
		for (int j = 0; j < tableLength; j++)
		{
			storeObject.Store(table[j].numberID);
			storeObject.Store(table[j].stringID);
			storeObject.Store(table[j].text);
		}
	}
}
