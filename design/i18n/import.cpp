//	Zinc Interface Library - IMPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsImport(const UI_EVENT &event)
{
	static ZIL_STORAGE *i18nFile;
	static ZIL_STORAGE *importFile;
	static UI_WINDOW_OBJECT *iObject;

	// Level 1-Select the import file.
	if (event.type == OPT_FILE_IMPORT)
	{
		iObject = event.windowObject;
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &i18nFile))
			return (directoryService->Event(S_ERROR));
		// Set up the i18n directory.
		ZIL_ICHAR *name = iObject->StringID();
		if (!strcmp(_languageDirectory, name))
			_i18nDirectory = _languageDirectory;
		else
			_i18nDirectory = _localeDirectory;
		return (FileRequest(event));
	}

	// Level 2-Open the import file.
	else if (event.type == -OPT_FILE_IMPORT && !event.data)
		return (event.type); // end to service response has no data.
	else if (event.type == -OPT_FILE_IMPORT)
	{
		// Try to import a filter file.
		if (OSImport(i18nFile, event) != S_ERROR)
			return (event.type);

		// Import from a zinc file.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		importFile = new ZIL_STORAGE(pathName, UIS_READWRITE);
		if (importFile->storageError)
		{
			delete importFile;
			return (directoryService->Event(S_ERROR));
		}
		storageService->Information(I_SET_STORAGE, importFile);
		eventManager->Put(OPT_RESOURCE_IMPORT); // Wait till provider window is removed from screen.

		// Move to the proper directory.
		extern ZIL_ICHAR *_rootStorageDirectory;
		i18nFile->ChDir(_rootStorageDirectory);
		ZIL_I18N::Traverse(i18nFile, _i18nDirectory);
	}

	// Level 3-Select the import resources.
	else if (event.type == OPT_RESOURCE_IMPORT)
	{
		UI_EVENT iEvent = event;
		iEvent.windowObject = iObject;
		return (DefaultsRequest(iEvent));
	}

	// Level 4-Save the import resources.
	else if (event.type == -OPT_RESOURCE_IMPORT && event.data)
	{
		// Confirm the import of duplicate objects.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		if (i18nFile->FindFirstObject(pathName) &&
			storageService->Event(event) != event.type)
			return (S_ERROR);

		// Import the language/locale information.
		if (_i18nDirectory == _languageDirectory)
		{
			ZAF_LANGUAGE_WINDOW languageWindow(pathName);
			languageWindow.Store(ZIL_NULLP(ZIL_ICHAR), i18nFile);
		}
		else
		{
			ZAF_LOCALE_WINDOW localeWindow(pathName);
			localeWindow.Store(ZIL_NULLP(ZIL_ICHAR), i18nFile);
		}
	}

	// Level 5-Clean up the import services.
	else if (event.type == -OPT_RESOURCE_IMPORT)
	{
		storageService->Information(I_SET_STORAGE, i18nFile);
		delete importFile;
	}

	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::OSImport(ZIL_STORAGE *, const UI_EVENT &)
{
	return (S_ERROR);
}

