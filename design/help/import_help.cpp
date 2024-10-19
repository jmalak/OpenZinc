//	Zinc Interface Library - IMPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "help.hpp"
#define USE_MSG_TABLE
#include "p_help.hpp"

extern ZIL_ICHAR *_helpDirectory;

// ----- ZIL_HELP_FILE ------------------------------------------------------

class ZIL_HELP_FILE : public ZIL_FILE
{
public:
	ZIL_HELP_FILE(const ZIL_ICHAR *pathName);
	int LoadLine(ZIL_ICHAR *line, int maxLength);
	void WriteHelpMessage(ZIL_STORAGE_OBJECT *element, long beginOffset, long endOffset);
};

ZIL_HELP_FILE::ZIL_HELP_FILE(const ZIL_ICHAR *pathName) :
	ZIL_FILE(pathName, UIS_READ | UIS_BINARY)
{
}

int ZIL_HELP_FILE::LoadLine(ZIL_ICHAR *line, int maxLength)
{
	// Load the data.
	int bytesRead = Read(line, maxLength);
	if (bytesRead == 0)
		return (-1); // eof.

	// Clean up the line.
	ZIL_ICHAR *endOfLine = line;
	for (int length = 0; length < maxLength; endOfLine++)
		if (*endOfLine == '\r' || *endOfLine == '\n')
		{
			endOfLine++;
			int eol = (*endOfLine == '\r' || *endOfLine == '\n') ? 2 : 1;
			Seek(length + eol - bytesRead, SEEK_FROM_CURRENT);
			endOfLine += 1 - eol;
			break;
		}
		else
			length += ::strlen(MapChar(*endOfLine)); // Get the length of the character.
	// Return to the next line.
	*endOfLine = 0;
	return ((int)(endOfLine - line));
}

void ZIL_HELP_FILE::WriteHelpMessage(ZIL_STORAGE_OBJECT *element,
	long beginOffset, long endOffset)
{
	// Write out the help message.
	long currentOffset = Tell();
	Seek(beginOffset, SEEK_FROM_START);
	if (beginOffset >= endOffset)
	{
		static ZIL_ICHAR tmp[1] = { 0 };
		element->Store(tmp);
		return;
	}
	else
	{
		// Import the buffer contents.
		ZIL_UINT16 wLength = 0;
		long offset = element->Tell();
		element->Seek(offset+sizeof(wLength));
		while (beginOffset < endOffset)
		{
			ZIL_ICHAR buffer[256];
			int length = LoadLine(buffer, 256);
			beginOffset = Tell();
			if (length == 0)
			{
				strcpy(buffer, _serviceManager->ZMSG_newline());
				length = strlen(buffer);
			}
#if defined(ZIL_UNICODE)
			else if (buffer[length-1] == '\\' ||
				 buffer[length-1] == 0xa5 ||
				 buffer[length-1] == 0x20a9)
#else
			else if (buffer[length-1] == '\\')
#endif
			{
				strcpy(&buffer[length-1], _serviceManager->ZMSG_newline());
				length = strlen(buffer);
			}
			else if (beginOffset < endOffset && buffer[length-1] != ' ')
				buffer[length++] = ' ';
			wLength += length;
#if defined(ZIL_UNICODE)
			for (int i=0; i < length; i++)
			{
				ZIL_UINT16 c = buffer[i];
				element->Store(c);
			}
#else
//			ZIL_ICHAR *unmappedText = UnMapText(buffer);
//			element->Store(unmappedText, length, sizeof(ZIL_ICHAR));
			element->Store(buffer, length, sizeof(ZIL_ICHAR));
//			delete unmappedText;
			
#endif
		}
		element->Seek(offset);
#if defined(ZIL_UNICODE)
		element->Store((ZIL_UINT16)(wLength|ZIL_UNICODE_FLAG));
#else
		element->Store(wLength);
#endif
	}
	Seek(currentOffset, SEEK_FROM_START);
}

// ----- ZAF_HELP_EDITOR ----------------------------------------------------

EVENT_TYPE ZAF_HELP_EDITOR::HelpImport(const UI_EVENT &event)
{
	static ZIL_STORAGE *helpFile;
	static ZIL_STORAGE *importFile;

	// Level 1-Select the import file.
	if (event.type == OPT_FILE_IMPORT)
	{
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &helpFile))
			return (directoryService->Event(S_ERROR));
		return (FileRequest(event));
	}

	// Level 2-Open the import file.
	else if (event.type == -OPT_FILE_IMPORT && !event.data)
		return (event.type); // end to service response has no data.
	else if (event.type == -OPT_FILE_IMPORT)
	{
		// Try to import a filter file.
		if (OSImport(helpFile, event) != S_ERROR)
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
		extern ZIL_ICHAR *_helpDirectory;
		helpFile->ChDir(_rootStorageDirectory);
		helpFile->ChDir(_helpDirectory);
	}

	// Level 3-Select the import resources.
	else if (event.type == OPT_RESOURCE_IMPORT)
		return (HelpRequest(event));

	// Level 4-Save the import resources.
	else if (event.type == -OPT_RESOURCE_IMPORT && event.data)
	{
		// Confirm the import of duplicate objects.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		if (helpFile->FindFirstObject(pathName) &&
			storageService->Event(event) != event.type)
			return (S_ERROR);

		// Import the resource.
		UIW_HELP_WINDOW helpWindow(pathName);
		helpWindow.Load(pathName, importFile);
		if (helpID == NO_HELP_CONTEXT)
		{
			for (ZIL_ICHAR *name = helpFile->FindFirstObject(_serviceManager->ZMSG_allObjects()); name; name = helpFile->FindNextObject())
			{
				ZIL_STORAGE_OBJECT_READ_ONLY hObject(*helpFile, name, 0);
				if (hObject.objectID > helpID)
 					helpID = hObject.objectID;
			}
		}
		helpWindow.helpID = ++helpID;
		helpWindow.Store(pathName, helpFile);
	}

	// Level 5-Clean up the import services.
	else if (event.type == -OPT_RESOURCE_IMPORT)
	{
		storageService->Information(I_SET_STORAGE, helpFile);
		delete importFile;
	}

	return (event.type);
}

EVENT_TYPE ZAF_HELP_EDITOR::OSImport(ZIL_STORAGE *helpFile, const UI_EVENT &event)
{
	// Check for a valid file (lower-case comparision).
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_ICHAR compareName[ZIL_MAXPATHLEN];
	strcpy(compareName, pathName);
	strlwr(compareName);
	if (!strstr(compareName, _txtExtension))
		return (S_ERROR);

	// Traverse to the help directory.
	if (helpFile->ChDir(_helpDirectory))
	{
		helpFile->MkDir(_helpDirectory);
		helpFile->ChDir(_helpDirectory);
	}

	ZIL_ICHAR *sTitle = msgTable.GetMessage(ZMSG_IMPORT_TITLE);
	ZIL_ICHAR *sFormat = msgTable.GetMessage(ZMSG_IMPORT_MESSAGE);
	static ZIL_ICHAR sLine[] = { '-','-','-',' ','%','[','^','\n','\r',']',0 };

	// Write the help context entries.
	ZIL_HELP_FILE rFile(pathName);

	long beginOffset = 0L, endOffset = 0L;
	ZIL_ICHAR line[256], helpName[256], title[256];
	ZIL_STORAGE_OBJECT *helpElement = ZIL_NULLP(ZIL_STORAGE_OBJECT);
	_serviceManager->StatusUpdate(sTitle);
	while (rFile.LoadLine(line, sizeof(line)/sizeof(ZIL_ICHAR)) != -1)
	{
		// Check for a valid help context.
		if (sscanf(line, sLine, helpName) == 1)
		{
			// Save the previous help message.
			if (helpElement)
			{
				rFile.WriteHelpMessage(helpElement, beginOffset, endOffset);
				delete helpElement;
			}
			// Determine the help identification.
			ZIL_OBJECTID helpID;
			if (helpFile->FindFirstObject(helpName))
			{
				strupr(helpName); // convert name to upper-case.
				ZIL_STORAGE_OBJECT_READ_ONLY sObject(*helpFile, helpName, 0);
				helpID = sObject.objectID;
			}
			else
				Information(GET_HELP_IDENTIFICATION, &helpID);
			// Get the new help storage element.
			helpElement = new ZIL_STORAGE_OBJECT(*helpFile, helpName, helpID, UIS_CREATE | UIS_READWRITE);
			_serviceManager->StatusUpdate(ZIL_NULLP(ZIL_ICHAR), sFormat, helpName);
			// Import the new help title.
			if (rFile.LoadLine(title, sizeof(title)/sizeof(ZIL_ICHAR)) == -1)
				title[0] = '\0';
			helpElement->Store(title);
			// Get the starting position of the help message.
			beginOffset = endOffset = rFile.Tell();
		}
		else
			endOffset = rFile.Tell();
	}
	_serviceManager->StatusUpdate();

	// Save the last help message.
	if (helpElement)
	{
		endOffset = rFile.Tell();
		rFile.WriteHelpMessage(helpElement, beginOffset, endOffset);
		delete helpElement;
	}

	return (event.type);
}
