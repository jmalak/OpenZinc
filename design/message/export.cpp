//	Zinc Interface Library - EXPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "message.hpp"

// ----- ZAF_MESSAGE_EDITOR -------------------------------------------------

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageExport(const UI_EVENT &event)
{
	static ZIL_STORAGE *messageFile;
	static ZIL_STORAGE *datFile = ZIL_NULLP(ZIL_STORAGE);
	static ZIL_FILE *osFile = ZIL_NULLP(ZIL_FILE);

	// Level 1-Select the export file.
	if (event.type == OPT_FILE_EXPORT)
	{
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &messageFile))
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
		return (MessageRequest(event));

	// Level 4-Save the export resources.
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && osFile)
		OSExport(messageFile, osFile, event);
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && datFile)
		OSExport(messageFile, datFile, event);

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

EVENT_TYPE ZAF_MESSAGE_EDITOR::OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *txtFile,
	const UI_EVENT &event)
{
	// Traverse to the window directory.
	extern ZIL_ICHAR *_messageDirectory;
	if (srcFile->ChDir(_messageDirectory))
		return (S_ERROR);

	// Store the window text information.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_LANGUAGE table(pathName, srcFile,
		ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY));

	// This line will be unnecessary if ZIL_LANGAUGE is enhanced to do it automatically.
	table.pathName = pathName;

	ZIL_ICHAR level[256]; level[0] = '\0';
	ExportTXT(&table, txtFile, level);
	return (event.type);
}

void ZAF_MESSAGE_EDITOR::ExportTXT(ZIL_LANGUAGE *table, ZIL_FILE *txtFile,
	ZIL_ICHAR *level)
{
	const int INDENT_SPACE = 4;

	// Store the table information.
	txtFile->Write(level);
	txtFile->Write((ZIL_ICHAR *)table->pathName);
	txtFile->Write(_serviceManager->ZMSG_newline());

	// Store begin of section.
	static ZIL_ICHAR _beginSection[] = { '{',0 };
	txtFile->Write(level);
	txtFile->Write(_beginSection);
	txtFile->Write(_serviceManager->ZMSG_newline());

	// increase level indentation.
	int length = strlen(level);
	for (int l = length; l < length + INDENT_SPACE; l++)
		level[l] = ' ';

//	memset(&level[length], ' ', INDENT_SPACE); // Can't memset a Unicode string.
	level[length + INDENT_SPACE] = '\0';

	// Store the messages.	(Do recursion here for heirarchical tables.)
	static ZIL_ICHAR _beginText[] = { ' ','=',' ','"',0 };
	static ZIL_ICHAR _endText[] = { '"',0 };
	for (int i = 0; i < table->noOfElements; i++)
	{
		txtFile->Write(level);
		txtFile->Write(table->data[i].stringID);
		txtFile->Write(_beginText);
		txtFile->Write(table->data[i].text);
		txtFile->Write(_endText);
		txtFile->Write(_serviceManager->ZMSG_newline());
	}

	// Decrease level indentation.
	length = strlen(level);
	level[length - INDENT_SPACE] = '\0';

	// Store end of section.
	static ZIL_ICHAR _endSection[] = { '}',0 };
	txtFile->Write(level);
	txtFile->Write(_endSection);
	txtFile->Write(_serviceManager->ZMSG_newline());

	// (If doing hierarchical tables, only do this line at root level.)
	txtFile->Write(_serviceManager->ZMSG_newline()); // end of table.
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::OSExport(ZIL_STORAGE *, ZIL_STORAGE *, const UI_EVENT &)
{
	return (S_ERROR);
}

