//	Zinc Interface Library - IMPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"

// ----- ZIL_TXT_FILE -------------------------------------------------------

const int MAX_TOKENS 		= 5;
const int MAX_LINE_LENGTH 	= 256;

class ZIL_TXT_FILE : public ZIL_FILE
{
public:
	ZIL_TXT_FILE(const ZIL_ICHAR *pathName);
	int LoadLine(ZIL_ICHAR *line, int maxLength);
	int ParseTokens(ZIL_ICHAR *line, ZIL_ICHAR **token, int maxTokens);
	void ImportTXT(ZIL_STORAGE *editFile, UI_WINDOW_OBJECT *window,
		UI_WINDOW_OBJECT *object, ZIL_ICHAR *line, ZIL_ICHAR **token,
		int level);
};

ZIL_TXT_FILE::ZIL_TXT_FILE(const ZIL_ICHAR *pathName) :
	ZIL_FILE(pathName, UIS_READ | UIS_BINARY)
{
}

int ZIL_TXT_FILE::LoadLine(ZIL_ICHAR *line, int maxLength)
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
	*endOfLine = 0;

	// Return to the next line.
	return ((int)(endOfLine - line));
}

int ZIL_TXT_FILE::ParseTokens(ZIL_ICHAR *line, ZIL_ICHAR **token, int maxTokens)
{
	int currentToken = 0;
	for (ZIL_ICHAR *data = line; *data; data++)
		if (!IsSpace(*data))
		{
			if (*data == '"') // quote delimited token.
			{
				token[currentToken++] = ++data; // move past " in string.
				while (*data && *data != '"')
					data++;
			}
			else // space delimited token.
			{
				token[currentToken++] = data;
				for (data++; *data && !IsSpace(*data); data++)
					;
			}
			if (*data && currentToken < maxTokens)
				*data = '\0';
			else
				break;
		}
	return (currentToken);
};

void ZIL_TXT_FILE::ImportTXT(ZIL_STORAGE *editFile, UI_WINDOW_OBJECT *window,
	UI_WINDOW_OBJECT *object, ZIL_ICHAR *line, ZIL_ICHAR **token, int level)
{
	// Load the window information.
	if (!window)
	{
		while (LoadLine(line, MAX_LINE_LENGTH) != -1)
		{
			int tokenCount = ParseTokens(line, token, MAX_TOKENS);
			if (!tokenCount)
				continue;
			UIW_WINDOW window(token[0], editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT),
				ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);

			// Change the window's text, then store the window.
			if (FlagSet(window.woStatus, WOS_READ_ERROR))
				continue;

			if (tokenCount == 3 && *token[1] == '=')
				window.Information(I_SET_TEXT, token[2]);
			ImportTXT(editFile, &window, ZIL_NULLP(UI_WINDOW_OBJECT), line, token, 0);

			window.Store(window.StringID(), editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT),
				ZAF_WINDOW::_objectTable, ZAF_WINDOW::_userTable);
		}
		return;
	}

	// Parse the sub-object lines.
	while (LoadLine(line, MAX_LINE_LENGTH) != -1)
	{
		int tokenCount = ParseTokens(line, token, MAX_TOKENS);
		if (tokenCount == 1 && *token[0] == '{') // begin section
		{
			ImportTXT(editFile, window, object, line, token, level + 1);
			if (!level)
				return; // automatically return if at root level.
		}
		else if (tokenCount == 3 && *token[1] == '=') // identification and text
		{
			if (object && object->Next() && !strcmp(token[0], object->Next()->StringID()))
				object = object->Next();
			else
				object = window->Get(token[0]);
			if (object)
				object->Information(I_SET_TEXT, token[2]);
		}
		else if (tokenCount == 1 && *token[0] == '}') // end section
			return;
	}
}

// ----- ZAF_WINDOW_EDITOR --------------------------------------------------

EVENT_TYPE ZAF_WINDOW_EDITOR::WindowImport(const UI_EVENT &event)
{
	static ZIL_STORAGE *editFile;
	static ZIL_STORAGE *importFile = ZIL_NULLP(ZIL_STORAGE);
	static UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM);
	static UI_ITEM *deriveTable = ZIL_NULLP(UI_ITEM);
	static ZIL_OBJECTID *mergeTable = ZIL_NULLP(ZIL_OBJECTID);

	// Level 1-Select the import file.
	if (event.type == OPT_FILE_IMPORT)
	{
		if (!storageService->Information(I_GET_STORAGE, &editFile))
			return (directoryService->Event(S_ERROR));
		return (FileRequest(event));
	}

	// Level 2-Open the import file.
	else if (event.type == -OPT_FILE_IMPORT && !event.data)
		return (event.type); // end service response has no data.
	else if (event.type == -OPT_FILE_IMPORT)
	{
		// Try to import a filter file.
		if (OSImport(editFile, event) != S_ERROR)
			return (event.type);

		// Import from a zinc file.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		importFile = new ZIL_STORAGE(pathName, UIS_READWRITE);
		if (importFile->storageError)
		{
			delete importFile;
			importFile = ZIL_NULLP(ZIL_STORAGE);
			return (directoryService->Event(S_ERROR));
		}
		storageService->Information(I_SET_STORAGE, importFile);
		objectTable = DuplicateObjectTable(ZAF_WINDOW::_objectTable);
		deriveTable = ResetDeriveTable(importFile, objectTable);
		mergeTable = MergeDeriveTable(deriveTable);
		eventManager->Put(OPT_RESOURCE_IMPORT); // Wait till provider window is removed from screen.

		// Move to the proper directory.
		extern ZIL_ICHAR *_rootStorageDirectory;
		extern ZIL_ICHAR *_windowDirectory;
		editFile->ChDir(_rootStorageDirectory);
		editFile->ChDir(_windowDirectory);
	}

	// Level 3-Select the import resources.
	else if (event.type == OPT_RESOURCE_IMPORT)
		return (WindowRequest(event));

	// Level 4-Save the import resources.
	else if (event.type == -OPT_RESOURCE_IMPORT && event.data)
	{
		// Confirm the import of duplicate objects.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		if (editFile->FindFirstObject(pathName) &&
			storageService->Event(event) != event.type)
			return (S_ERROR);

		// Import the window.
		defaultStatus |= WOS_INTERNAL_ACTION; // causes bitmaps and icons to be written.
		UIW_WINDOW window(pathName, importFile,
			ZIL_NULLP(ZIL_STORAGE_OBJECT), objectTable,
			ZAF_WINDOW::_userTable);
		defaultStatus &= ~WOS_INTERNAL_ACTION;
		if (FlagSet(window.woStatus, WOS_READ_ERROR))
			return (storageService->Event(S_ERROR));

		// Handle the modified window information.
		ShiftHelpIDs(importFile, editFile, &window);
		ShiftDeriveIDs(mergeTable, &window);

		// Store the window.
		window.Store(window.StringID(), editFile,
			ZIL_NULLP(ZIL_STORAGE_OBJECT), objectTable,
			ZAF_WINDOW::_userTable);
		ZAF_SERVICE_MANAGER::_changedData = TRUE;
	}

	// Level 5-Clean up the import services.
	else if (event.type == -OPT_RESOURCE_IMPORT)
	{
		storageService->Information(I_SET_STORAGE, editFile);
		delete importFile;
		if (objectTable)
			DestroyObjectTable(objectTable);
		if (mergeTable)
			delete mergeTable;
	}

	return (event.type);
}

EVENT_TYPE ZAF_WINDOW_EDITOR::OSImport(ZIL_STORAGE *editFile, const UI_EVENT &event)
{
	// Check for a valid file (lower-case comparision).
	extern ZIL_ICHAR *_windowDirectory;
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_ICHAR compareName[ZIL_MAXPATHLEN];
	strcpy(compareName, pathName);
	strlwr(compareName);
	if (!strstr(compareName, _txtExtension) ||
		editFile->ChDir(_windowDirectory))
		return (S_ERROR);

	// Find the matching window entry.
	ZIL_ICHAR line[MAX_LINE_LENGTH], *token[MAX_TOKENS];

	ZIL_TXT_FILE txtFile(pathName);
	txtFile.ImportTXT(editFile, ZIL_NULLP(UI_WINDOW_OBJECT),
		ZIL_NULLP(UI_WINDOW_OBJECT), line, token, 0);

	return (event.type);
}

// --------------------------------------------------------------------------

void ZAF_WINDOW_EDITOR::DestroyObjectTable(UI_ITEM *objectTable)
{
	// Destroy the derived object text.
	for (int i = 0; objectTable[i].value != ID_END; i++)
		if (objectTable[i].value >= ID_DERIVED_START &&
			objectTable[i].text != ZAF_DERIVED_OBJECT::_className)
			delete objectTable[i].text;
	// Destroy the object table.
	delete objectTable;
}

UI_ITEM *ZAF_WINDOW_EDITOR::DuplicateObjectTable(UI_ITEM *objectTable)
{
	// Allocate the object table.
	int count = 0;
	while (objectTable[count].value != ID_END)
		count++;
	UI_ITEM *duplicateTable = new UI_ITEM[++count];
	memcpy(duplicateTable, objectTable, count * sizeof(UI_ITEM));

	// Reset the derive table entries.
	for (int i = 0; i < count; i++)
		if (duplicateTable[i].value >= ID_DERIVED_START)
		{
			duplicateTable[i].data = (UI_WINDOW_OBJECT*)ZAF_DERIVED_OBJECT::New;
			duplicateTable[i].text = ZAF_DERIVED_OBJECT::_className;
			duplicateTable[i].flags = 0;
		}
	return (duplicateTable);
}

ZIL_OBJECTID *ZAF_WINDOW_EDITOR::MergeDeriveTable(UI_ITEM *srcTable)
{
	// Determine the merge table sizes.
	UI_ITEM *dstTable = ZAF_WINDOW::_deriveTable;
	int srcTotal;
	for (srcTotal = 0; srcTable[srcTotal].value != ID_END; )
		srcTotal++;

	// Allocate the duplicate table.
	ZIL_OBJECTID *mergeTable = new ZIL_OBJECTID[srcTotal+1];
	memset(mergeTable, 0, (srcTotal + 1) * sizeof(ZIL_OBJECTID));

	// Reset the duplicate table.
	int openSlot = 0;
	for (int i = 0; i <= srcTotal; i++)
		if (srcTable[i].value >= ID_DERIVED_START &&
			srcTable[i].text != ZAF_DERIVED_OBJECT::_className)
		{
			// Check for a matching entry.
			for (int j = 0; dstTable[j].value != ID_END; j++)
				if (!stricmp(srcTable[i].text, dstTable[j].text))
				{
					mergeTable[i] = dstTable[j].value;
					break;
				}
			// Merge the derive table entry.
			if (!mergeTable[i])
			{
				while (dstTable[openSlot].text != ZAF_DERIVED_OBJECT::_className)
					openSlot++;
				mergeTable[i] = dstTable[openSlot].value;
				dstTable[openSlot].text = strdup(srcTable[i].text);
				dstTable[openSlot].data = srcTable[i].data;
				openSlot++;
			}
		}
		else
			mergeTable[i] = srcTable[i].value;

	// Return a pointer to the merge table.
	return (mergeTable);
}

void ZAF_WINDOW_EDITOR::ShiftDeriveIDs(ZIL_OBJECTID *mergeTable, UI_WINDOW_OBJECT *window)
{
	// Check for an affected help context.
	if (window->searchID >= ID_DERIVED_START)
		window->searchID = mergeTable[window->searchID - ID_DERIVED_START];

	// Check the sub-objects (if any).
	if (window->NumberID() == NUMID_GEOMETRY)
		return; // Constraints can't be derived.
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUPPORT_FIRST, &object);
	if (!object)
		window->Information(I_GET_FIRST, &object);
	while (object)
	{
		// Check the sub-item.
		ShiftDeriveIDs(mergeTable, object);
		if (!object->Next() && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
			window->Information(I_GET_FIRST, &object);
		else
			object = object->Next();
	}
}

