//	Zinc Interface Library - EXPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"

EVENT_TYPE ZAF_I18N_EDITOR::DefaultsExport(const UI_EVENT &event)
{
	static ZIL_STORAGE *defaultsFile;
	static UI_WINDOW_OBJECT *iObject;
	static ZIL_STORAGE *datFile = ZIL_NULLP(ZIL_STORAGE);
	static ZIL_FILE *osFile = ZIL_NULLP(ZIL_FILE);

	// Level 1-Select the export file.
	if (event.type == OPT_FILE_EXPORT)
	{
		iObject = event.windowObject;
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &defaultsFile))
			return (directoryService->Event(S_ERROR));
		return (FileRequest(event));
	}

	// Level 2-Open the export file.
	else if (event.type == -OPT_FILE_EXPORT && !event.data)
		return (event.type); // end to service response has no data.
	else if (event.type == -OPT_FILE_EXPORT)
	{
		// Open the export file.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		if (strstr(pathName, _txtExtension))
			osFile = new ZIL_FILE(pathName, UIS_CREATE | UIS_READWRITE);
		else
			datFile = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
		eventManager->Put(OPT_RESOURCE_EXPORT); // Wait till provider window is removed from screen.
	}

	// Level 3-Select the export resources.
	else if (event.type == OPT_RESOURCE_EXPORT)
	{
		UI_EVENT iEvent = event;
		iEvent.windowObject = iObject;
		return (DefaultsRequest(iEvent));
	}

	// Level 4-Save the export resources.
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && osFile)
		OSExport(defaultsFile, osFile, event);
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && datFile)
		OSExport(defaultsFile, datFile, event);

	// Level 5-Clean up the export services.
	else if (event.type == -OPT_RESOURCE_EXPORT && datFile)
	{
		datFile->Save();
		delete datFile;
		datFile = ZIL_NULLP(ZIL_STORAGE);
	}
	else if (event.type == -OPT_RESOURCE_EXPORT && osFile)
	{
		delete osFile;
		osFile = ZIL_NULLP(ZIL_FILE);
	}

	return (event.type);
}

EVENT_TYPE ZAF_I18N_EDITOR::OSExport(ZIL_STORAGE *, ZIL_FILE *, const UI_EVENT &)
{
	return (S_ERROR);
}

EVENT_TYPE ZAF_I18N_EDITOR::OSExport(ZIL_STORAGE *, ZIL_STORAGE *, const UI_EVENT &)
{
	return (S_ERROR);
}

