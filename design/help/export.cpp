//	Zinc Interface Library - EXPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "help.hpp"

extern ZIL_ICHAR *_helpDirectory;
const int LINE_LENGTH = 76;

// ----- ZAF_HELP_EDITOR ----------------------------------------------------

EVENT_TYPE ZAF_HELP_EDITOR::HelpExport(const UI_EVENT &event)
{
	static ZIL_STORAGE *helpFile;
	static ZIL_STORAGE *datFile = ZIL_NULLP(ZIL_STORAGE);
	static ZIL_FILE *osFile = ZIL_NULLP(ZIL_FILE);

	// Level 1-Select the export file.
	if (event.type == OPT_FILE_EXPORT)
	{
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &helpFile))
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
		if (strstr(compareName, _txtExtension))
			osFile = new ZIL_FILE(pathName, UIS_CREATE | UIS_READWRITE | UIS_BINARY);
		else
			datFile = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
		eventManager->Put(OPT_RESOURCE_EXPORT); // Wait till provider window is removed from screen.
	}

	// Level 3-Select the export resources.
	else if (event.type == OPT_RESOURCE_EXPORT)
		return (HelpRequest(event));

	// Level 4-Save the export resources.
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && osFile)
		OSExport(helpFile, osFile, event);
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && datFile)
		OSExport(helpFile, datFile, event);

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

EVENT_TYPE ZAF_HELP_EDITOR::OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *osFile, const UI_EVENT &event)
{
	// Traverse to the help directory.
	if (srcFile->ChDir(_helpDirectory))
		return (FALSE);

	// Store the help context.
	static ZIL_ICHAR prefix[] = { '-','-','-',' ', 0 };
	static ZIL_ICHAR cline[] = { '\\', 0 };
	ZIL_ICHAR *text = (ZIL_ICHAR *)event.data;
//	ZIL_ICHAR *mappedText = (ZIL_ICHAR *)event.data;
//	ZIL_ICHAR *text = MapText(mappedText);
	ZIL_STORAGE_OBJECT_READ_ONLY context(*srcFile, text, 0);

	// name
	osFile->Write(prefix);
	osFile->Write(text);
	osFile->Write(_serviceManager->ZMSG_newline());
	// title
	context.Load(&text);
//	context.Load(&mappedText);
//	text = MapText(mappedText);
	osFile->Write(text);
	osFile->Write(_serviceManager->ZMSG_newline());
	delete text;
	// context
	context.Load(&text);
//	context.Load(&mappedText);
//	text = MapText(mappedText);
	int i = 0, j = LINE_LENGTH, length = text ? strlen(text) : 0;
	while (i < length)
	{
		// look backward for spaces.
		if (j >= length)
			j = length - 1;
		else
		{
			for ( ; j > i; j--)
				if (text[j] == ' ')
					break;
		}
		// look forward for newline.
		int k;
		for (k = i; k < j; k++)
			if (text[k] == '\n')
				break;
		j = k;

		if (text[j] == ' ')
		{
			text[j] = '\0';
			osFile->Write(&text[i]);
			if (j < length)
				osFile->Write(_serviceManager->ZMSG_newline());
		}
		else if (text[j] == '\n')
		{
			text[j] = '\0';
			if (j > 0 && text[j-1] == '\r')
				text[j-1] = '\0';
			osFile->Write(&text[i]);
			if (j < length)
			{
				osFile->Write(cline);
				osFile->Write(_serviceManager->ZMSG_newline());
			}
		}
		else if (text[j+1] == '\0')
		{
			osFile->Write(&text[i]);
			osFile->Write(_serviceManager->ZMSG_newline());
		}
		i = j + 1;
		j += LINE_LENGTH;
	}
	if (!text)
		osFile->Write(_serviceManager->ZMSG_newline());
	else
		delete text;

	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *datFile, const UI_EVENT &event)
{
	// Traverse to the help directory.
	if (srcFile->ChDir(_helpDirectory))
		return (FALSE);
	if (datFile->ChDir(_helpDirectory))
	{
		datFile->MkDir(_helpDirectory);
		datFile->ChDir(_helpDirectory);
	}

	// Confirm the export of duplicate objects.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	if (datFile->FindFirstObject(pathName) &&
		storageService->Event(event) != event.type)
		return (FALSE);

	// Get the proper helpID.
	static UI_HELP_CONTEXT helpID = NO_HELP_CONTEXT;
	static ZIL_STORAGE *_datFile = ZIL_NULLP(ZIL_STORAGE);
	if (_datFile != datFile || helpID)
	{
		for (ZIL_ICHAR *name = datFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); name; name = datFile->FindNextObject())
		{
			ZIL_STORAGE_OBJECT_READ_ONLY hObject(*datFile, name, 0);
			if (hObject.objectID > helpID)
					helpID = hObject.objectID;
		}
	}

	// Store the help information.
	int length;
	char buffer[1024];
	ZIL_STORAGE_OBJECT_READ_ONLY srcObject(*srcFile, pathName, 0);
	ZIL_STORAGE_OBJECT dstObject(*datFile, pathName, ++helpID, UIS_OPENCREATE | UIS_READWRITE);
//	ZIL_ICHAR *mappedText;
	while ((length = srcObject.Load(buffer, 1024, 1)) > 0)
	{
//		mappedText = MapText(buffer);
//		dstObject.Store(mappedText, length, 1);
		dstObject.Store(buffer, length, 1);
//		delete mappedText;
	}

	return (event.type);
}

