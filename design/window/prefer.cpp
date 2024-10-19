//	Zinc Interface Library - PREFER.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_ZAF_WINDOW_PREFERENCES
#include "p_window.hpp"

// --------------------------------------------------------------------------
// ----- ZAF_WINDOW_PREFERENCES ---------------------------------------------
// --------------------------------------------------------------------------

static ZIL_ICHAR _windowName[] = { 'Z','A','F','_','W','I','N','D','O','W','_','P','R','E','F','E','R','E','N','C','E','S',0 };

ZAF_WINDOW_PREFERENCES::ZAF_WINDOW_PREFERENCES(void) :
	UIW_WINDOW(_windowName, ZAF_WINDOW_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT), ZAF_WINDOW_EDITOR::_objectTable,
	ZAF_WINDOW_EDITOR::_userTable)
{
	// Load the default information.
	SetOptions();

	// Center the window on the screen.
	windowManager->Center(this);
}

ZAF_WINDOW_PREFERENCES::~ZAF_WINDOW_PREFERENCES(void)
{
}

EVENT_TYPE ZAF_WINDOW_PREFERENCES::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_OK:
		// Save the options.
		GetOptions();
		StoreOptions();
		// Continue to OPT_CANCEL.
	case OPT_CANCEL:
		// Remove the window from the window manager
		eventManager->Put(S_CLOSE, Q_BEGIN);
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case OPT_FILE_OPEN:
	case OPT_FILE_CLOSE:
		{
		ZIL_STORAGE *_deltaFile = ZAF_WINDOW_EDITOR::_deltaFile;
		if (_deltaFile)
		{
			delete _deltaFile;
			_deltaFile = ZIL_NULLP(ZIL_STORAGE);
		}
		if (ccode == OPT_FILE_OPEN)
		{
			ZIL_ICHAR *pathName;
			Get(FIELD_DELTA_PATHNAME)->Information(I_GET_TEXT, &pathName);
			_deltaFile = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
			if (_deltaFile->storageError)
			{
				delete _deltaFile;
				_deltaFile = ZIL_NULLP(ZIL_STORAGE);
			}
		}
		ZAF_WINDOW_EDITOR::_deltaFile = _deltaFile;
		if (_deltaFile)
		{
			ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
			_deltaFile->StorageName(pathName);
			Get(FIELD_DELTA_CURRENT)->Information(I_SET_TEXT, pathName);
		}
		else
			Get(FIELD_DELTA_CURRENT)->Information(I_SET_TEXT, _serviceManager->ZMSG_noFile());

		// Update the window editor's title (for delta storage).
		UI_WINDOW_OBJECT *windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
		windowEditor->Information(I_SET_TEXT, ZIL_NULLP(void));
		}
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(ZAF_WINDOW_EDITOR::_storage, FALSE);
		break;

	default:
		// Default to the base window event.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void ZAF_WINDOW_PREFERENCES::GetOptions(void)
{
	int value = 0;
	ZIL_ICHAR *string;

	// storage options
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	Get(FIELD_BACKUPS)->Information(I_GET_VALUE, &value);
	storageService->Information(I_SET_REVISIONS, &value);
	Get(FIELD_DEFAULT_EXTENSION)->Information(I_GET_TEXT, &string);
	storageService->Information(I_SET_DEFAULT_EXTENSION, string);
	value = FlagSet(Get(OPTION_WRITE_HPP)->woStatus, WOS_SELECTED) ? TRUE : FALSE;
	storageService->Information(I_SET_WRITE_HPP, &value);
	value = FlagSet(Get(OPTION_WRITE_CPP)->woStatus, WOS_SELECTED) ? TRUE : FALSE;
	storageService->Information(I_SET_WRITE_CPP, &value);

	// mini-cell values
	Get(FIELD_NUMX)->Information(I_GET_VALUE, &value);
	display->miniNumeratorX = value;
	Get(FIELD_DENX)->Information(I_GET_VALUE, &value);
	display->miniDenominatorX = value;
	Get(FIELD_NUMY)->Information(I_GET_VALUE, &value);
	display->miniNumeratorY = value;
	Get(FIELD_DENY)->Information(I_GET_VALUE, &value);
	display->miniDenominatorY = value;
}

void ZAF_WINDOW_PREFERENCES::LoadOptions(void)
{
	// Open the config file.
	ZIL_STORAGE_READ_ONLY configFile(_serviceManager->ZMSG_configName());
	if (configFile.storageError ||
		configFile.ChDir(_serviceManager->ZMSG_configPath()) ||
		!configFile.FindFirstObject(ZAF_WINDOW_EDITOR::_className))
		return;

	// Load the window options.
	ZIL_STORAGE_OBJECT_READ_ONLY file(configFile, ZAF_WINDOW_EDITOR::_className, 0);

	// storage options
	int value = 0;
	ZIL_INT16 iData;
	ZIL_ICHAR *sData;
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);

	// Store the storage options.
	file.Load(&iData); value = iData;
	storageService->Information(I_SET_REVISIONS, &value);
	file.Load(&iData); value = iData;
	storageService->Information(I_SET_WRITE_HPP, &value);
	file.Load(&iData); value = iData;
	storageService->Information(I_SET_WRITE_CPP, &value);

	file.Load(&sData);
	storageService->Information(I_SET_DEFAULT_EXTENSION, sData);
	delete sData;

	// Store the window preferences.
	file.Load(&iData);
	display->miniNumeratorX = iData;
	file.Load(&iData);
	display->miniDenominatorX = iData;
	file.Load(&iData);
	display->miniNumeratorY = iData;
	file.Load(&iData);
	display->miniDenominatorY = iData;
}

void ZAF_WINDOW_PREFERENCES::SetOptions(void)
{
	int value = 0;
	ZIL_ICHAR *string;

	// storage options
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	storageService->Information(I_GET_REVISIONS, &value);
	Get(FIELD_BACKUPS)->Information(I_SET_VALUE, &value);
	storageService->Information(I_GET_DEFAULT_EXTENSION, &string);
	Get(FIELD_DEFAULT_EXTENSION)->Information(I_SET_TEXT, string);
	storageService->Information(I_GET_WRITE_HPP, &value);
	if (value)
		Get(OPTION_WRITE_HPP)->woStatus |= WOS_SELECTED;
	storageService->Information(I_GET_WRITE_CPP, &value);
	if (value)
		Get(OPTION_WRITE_CPP)->woStatus |= WOS_SELECTED;

	// mini-cell values
	value = (int)display->miniNumeratorX;
	Get(FIELD_NUMX)->Information(I_SET_VALUE, &value);
	value = display->miniDenominatorX ? (int)display->miniDenominatorX : 1;
	Get(FIELD_DENX)->Information(I_SET_VALUE, &value);
	value = (int)display->miniNumeratorY;
	Get(FIELD_NUMY)->Information(I_SET_VALUE, &value);
	value = display->miniDenominatorY ? (int)display->miniDenominatorY : 1;
	Get(FIELD_DENY)->Information(I_SET_VALUE, &value);

	// delta storage
	ZIL_STORAGE *_deltaFile = ZAF_WINDOW_EDITOR::_deltaFile;
	if (_deltaFile)
	{
		ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
		_deltaFile->StorageName(pathName);
		Get(FIELD_DELTA_CURRENT)->Information(I_SET_TEXT, pathName);
	}
	else
		Get(FIELD_DELTA_CURRENT)->Information(I_SET_TEXT, _serviceManager->ZMSG_noFile());
}

void ZAF_WINDOW_PREFERENCES::StoreOptions(void)
{
	// Open, or create the config file.
	ZIL_STORAGE *configFile = new ZIL_STORAGE(_serviceManager->ZMSG_configName(), UIS_READWRITE);
	if (configFile->storageError) // try opening in designer path.
	{
		delete configFile;
		ZIL_ICHAR pathName[ZIL_MAXPATHLEN];
		UI_PATH *path = ZIL_STORAGE_READ_ONLY::searchPath;
		// Add path. (designer path is second in the list, first is current working directory).
		if (path && path->FirstPathName())
			ZIL_STORAGE_READ_ONLY::AppendFullPath(pathName,
				path->NextPathName(),
				_serviceManager->ZMSG_configName());
		else
			strcpy(pathName, _serviceManager->ZMSG_configName());
		configFile = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
	}
	if (configFile->storageError) // try opening in current path.
	{
		delete configFile;
		configFile = new ZIL_STORAGE(_serviceManager->ZMSG_configName(), UIS_OPENCREATE | UIS_READWRITE);
	}
	if (configFile->storageError) // give up.
	{
		delete configFile;
		return;
	}

	// Store the configuration information.
	if (configFile->ChDir(_serviceManager->ZMSG_configPath()))
	{
		configFile->MkDir(_serviceManager->ZMSG_configPath());
		configFile->ChDir(_serviceManager->ZMSG_configPath());
	}

	// Store the window options.
	ZIL_STORAGE_OBJECT *file = new ZIL_STORAGE_OBJECT(*configFile, ZAF_WINDOW_EDITOR::_className, 0, UIS_CREATE | UIS_READWRITE);

	// storage options
	int value = 0;
	ZIL_INT16 iData;
	ZIL_ICHAR *sData;
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);

	// Store the storage options.
	storageService->Information(I_GET_REVISIONS, &value);
	iData = value; file->Store(iData);
	storageService->Information(I_GET_WRITE_HPP, &value);
	iData = value; file->Store(iData);
	storageService->Information(I_GET_WRITE_CPP, &value);
	iData = value; file->Store(iData);

	storageService->Information(I_GET_DEFAULT_EXTENSION, &sData);
	file->Store(sData);

	// Store the display preferences.
	iData = (int)display->miniNumeratorX;
	file->Store(iData);
	iData = (int)display->miniDenominatorX;
	file->Store(iData);
	iData = (int)display->miniNumeratorY;
	file->Store(iData);
	iData = (int)display->miniDenominatorY;
	file->Store(iData);

	// Store the data.
	delete file;
	configFile->Save();
	delete configFile;
}

