//	Zinc Interface Library - EXPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowExport(const UI_EVENT &event)
{
	static ZIL_STORAGE *editFile;
	static ZIL_STORAGE *datFile = ZIL_NULLP(ZIL_STORAGE);
	static ZIL_FILE *osFile = ZIL_NULLP(ZIL_FILE);

	// Level 1-Select the export file.
	if (event.type == OPT_FILE_EXPORT)
	{
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &editFile))
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
		ZIL_ICHAR compareName[ZIL_MAXPATHLEN];
		strcpy(compareName, pathName);
		strlwr(compareName);

		if (strstr(compareName, _txtExtension)) // check for TXT file.
			osFile = new ZIL_FILE(pathName, UIS_CREATE | UIS_READWRITE | UIS_BINARY);
		else // open DAT file.
		{
			// Export to a zinc file.
			datFile = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
			if (datFile->storageError)
			{
				delete datFile;
				datFile = ZIL_NULLP(ZIL_STORAGE);
				return (storageService->Event(S_ERROR));
			}
		}
		eventManager->Put(OPT_RESOURCE_EXPORT); // Wait till provider window is removed from screen.
	}

	// Level 3-Select the export resources.
	else if (event.type == OPT_RESOURCE_EXPORT)
		return (WindowRequest(event));

	// Level 4-Save the export resources.
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && osFile)
		OSExport(editFile, osFile, event);
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && datFile)
		OSExport(editFile, datFile, event);

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

EVENT_TYPE ZAF_WINDOW_EDITOR::OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *dstFile, const UI_EVENT &event)
{
	// Traverse to the window directory.
	extern ZIL_ICHAR *_windowDirectory;
	if (srcFile->ChDir(_windowDirectory))
		return (S_ERROR);
	if (dstFile->ChDir(_windowDirectory))
	{
		dstFile->MkDir(_windowDirectory);
		dstFile->ChDir(_windowDirectory);
	}

	// Confirm the export of duplicate objects.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	if (dstFile->FindFirstObject(pathName) &&
		storageService->Event(event) != event.type)
		return (FALSE);

	// Read the edit window information.
	ZAF_WINDOW window(pathName, srcFile,
		ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);

	// Handle the modified window information.
	ShiftHelpIDs(srcFile, dstFile, &window);

	// Store the edit window information.
	window.Store(pathName, dstFile, ZIL_NULLP(ZIL_STORAGE_OBJECT),
		ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);
	return (event.type);
}

// --- Export for TXT files -------------------------------------------------

void ZAF_WINDOW_EDITOR::ExportTXT(UI_WINDOW_OBJECT *window, ZIL_FILE *txtFile, ZIL_ICHAR *level)
{
	const int INDENT_SPACE = 4;

	// Store the window information.
	ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
	window->Information(I_GET_TEXT, &text);
	txtFile->Write(level);
	txtFile->Write(window->stringID);
	if (text)
	{
		static ZIL_ICHAR _beginText[] = { ' ','=',' ','"',0 };
		static ZIL_ICHAR _endText[] = { '"',0 };
		txtFile->Write(_beginText);
		txtFile->Write(text);
		txtFile->Write(_endText);
	}
	txtFile->Write(_serviceManager->ZMSG_newline());

	// Store the sub-object identifications.
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUPPORT_FIRST, &object);
	if (!object)
		window->Information(I_GET_FIRST, &object);
	int subObject = object ? TRUE : FALSE;

	if (subObject || !window->parent) // begin new section.
	{
		// store begin of section.
		static ZIL_ICHAR _beginSection[] = { '{',0 };
		txtFile->Write(level);
		txtFile->Write(_beginSection);
		txtFile->Write(_serviceManager->ZMSG_newline());
		// increase level indentation.
		int length = strlen(level);
		memset(&level[length], ' ', INDENT_SPACE);
		level[length + INDENT_SPACE] = '\0';
	}
	while (object) // write sub-object information.
	{
		if (object->NumberID() < (ZIL_NUMBERID)0xFF00)
			ExportTXT(object, txtFile, level);
		if (!object->Next() && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
			window->Information(I_GET_FIRST, &object);
		else
			object = object->Next();
	}
	if (subObject || !window->parent) // end section.
	{
		// decrease level indentation.
		int length = strlen(level);
		level[length - INDENT_SPACE] = '\0';
		// store end of section.
		static ZIL_ICHAR _endSection[] = { '}',0 };
		txtFile->Write(level);
		txtFile->Write(_endSection);
		txtFile->Write(_serviceManager->ZMSG_newline());
	}
	if (!window->parent)
		txtFile->Write(_serviceManager->ZMSG_newline()); // end of window.
}

EVENT_TYPE ZAF_WINDOW_EDITOR::OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *txtFile, const UI_EVENT &event)
{
	// Traverse to the window directory.
	extern ZIL_ICHAR *_windowDirectory;
	if (srcFile->ChDir(_windowDirectory))
		return (S_ERROR);

	// Store the window text information.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZAF_WINDOW window(pathName, srcFile,
		ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);

	ZIL_ICHAR level[256]; level[0] = '\0';
	ExportTXT(&window, txtFile, level);
	return (event.type);
}

