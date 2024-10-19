//	Zinc Interface Library - IMPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "message.hpp"

// ----- ZIL_TEXT_FILE -------------------------------------------------------

const int MAX_TOKENS 		= 5;
const int MAX_LINE_LENGTH 	= 256;

class ZIL_TEXT_FILE : public ZIL_FILE
{
public:
	ZIL_TEXT_FILE(const ZIL_ICHAR *pathName);
	int LoadLine(ZIL_ICHAR *line, int maxLength);
	int ParseTokens(ZIL_ICHAR *line, ZIL_ICHAR **token, int maxTokens);
	void ImportTXT(ZIL_STORAGE *editFile, ZIL_LANGUAGE *table,
		ZIL_ICHAR *line, ZIL_ICHAR **token, int level);
};

ZIL_TEXT_FILE::ZIL_TEXT_FILE(const ZIL_ICHAR *pathName) :
	ZIL_FILE(pathName, UIS_READ | UIS_BINARY)
{
}

int ZIL_TEXT_FILE::LoadLine(ZIL_ICHAR *line, int maxLength)
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

int ZIL_TEXT_FILE::ParseTokens(ZIL_ICHAR *line, ZIL_ICHAR **token, int maxTokens)
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

void ZIL_TEXT_FILE::ImportTXT(ZIL_STORAGE *editFile, ZIL_LANGUAGE *table,
	ZIL_ICHAR *line, ZIL_ICHAR **token, int level)
{
	// Load the window information.
	if (!table)
	{
		while (LoadLine(line, MAX_LINE_LENGTH) != -1)
		{
			int tokenCount = ParseTokens(line, token, MAX_TOKENS);
			if (!tokenCount)
				continue;

			ZIL_ICHAR *tableName = strdup(token[0]);
			ZIL_LANGUAGE table(tableName, editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));

			ImportTXT(editFile, &table, line, token, 0);

			table.Store(tableName, editFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
			delete tableName;
		}
		return;
	}

	// Parse the sub-object lines.
	while (LoadLine(line, MAX_LINE_LENGTH) != -1)
	{
		int tokenCount = ParseTokens(line, token, MAX_TOKENS);
		if (tokenCount == 1 && *token[0] == '{') // begin section
		{
			ImportTXT(editFile, table, line, token, level + 1);
			if (!level)
				return; // automatically return if at root level.
		}
		else if (tokenCount == 3 && *token[1] == '=') // identification and text
		{
			int i;
			for (i = 0; i < table->noOfElements; i++)
				if (!strcmp(token[0], table->data[i].stringID))
				{
					delete table->data[i].text;
					table->data[i].text = strdup(token[2]);
					break;
				}
			if (i >= table->noOfElements)	// New entry.
			{
				// Reallocate the message table.
				ZIL_LANGUAGE_ELEMENT *oldTable = table->data;
				table->data = new ZIL_LANGUAGE_ELEMENT[table->noOfElements + 1];
				ZIL_NUMBERID highID = 0;
				for (int i = 0; i < table->noOfElements; i++)
				{
					// Null out the entry in new table and then swap data.
					table->data[i].text = ZIL_NULLP(ZIL_ICHAR);
					table->data[i].numberID = 0;
					table->data[i].stringID[0] = '\0';
					table->data[i].SwapData(oldTable[i]);
					if (table->data[i].numberID > highID)
						highID = table->data[i].numberID;
				}
				if (oldTable)
					delete oldTable;

				// Insert the new entry
				table->data[table->noOfElements].numberID = highID + 1;
				strcpy(table->data[table->noOfElements].stringID, token[0]);
				table->data[table->noOfElements].text = strdup(token[2]);
				table->noOfElements++;
			}
		}
		else if (tokenCount == 1 && *token[0] == '}') // end section
			return;
	}
}

// ----- ZAF_MESSAGE_EDITOR --------------------------------------------------

EVENT_TYPE ZAF_MESSAGE_EDITOR::MessageImport(const UI_EVENT &event)
{
	static ZIL_STORAGE *messageFile;
	static ZIL_STORAGE *importFile;

	// Level 1-Select the import file.
	if (event.type == OPT_FILE_IMPORT)
	{
		if (!storageService->Information(I_GET_STORAGE, &messageFile))
			return (directoryService->Event(S_ERROR));
		return (FileRequest(event));
	}

	// Level 2-Open the import file.
	else if (event.type == -OPT_FILE_IMPORT && !event.data)
		return (event.type); // end to service response has no data.
	else if (event.type == -OPT_FILE_IMPORT)
	{
		// Try to import a filter file.
		if (OSImport(messageFile, event) != S_ERROR)
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
		extern ZIL_ICHAR *_messageDirectory;
		messageFile->ChDir(_rootStorageDirectory);
		messageFile->ChDir(_messageDirectory);
	}

	// Level 3-Select the import resources.
	else if (event.type == OPT_RESOURCE_IMPORT)
		return (MessageRequest(event));

	// Level 4-Save the import resources.
	else if (event.type == -OPT_RESOURCE_IMPORT && event.data)
	{
		// Try to import a filter file.
		if (OSImport(messageFile, event) != S_ERROR)
			return (event.type);

		// Confirm the import of duplicate objects.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		if (messageFile->FindFirstObject(pathName) &&
			storageService->Event(event) != event.type)
			return (S_ERROR);

		// Import the message table.
		ZIL_LANGUAGE message(pathName, importFile);
		message.Store(pathName, messageFile);
	}

	// Level 5-Clean up the import services.
	else if (event.type == -OPT_RESOURCE_IMPORT)
	{
		storageService->Information(I_SET_STORAGE, messageFile);
		delete importFile;
	}

	return (event.type);
}

EVENT_TYPE ZAF_MESSAGE_EDITOR::OSImport(ZIL_STORAGE *editFile, const UI_EVENT &event)
{
	// Check for a valid file (lower-case comparision).
	extern ZIL_ICHAR *_messageDirectory;
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_ICHAR compareName[ZIL_MAXPATHLEN];
	strcpy(compareName, pathName);
	strlwr(compareName);
	if (!strstr(compareName, _txtExtension) ||
		editFile->ChDir(_messageDirectory))
		return (S_ERROR);

	// Import the message table(s).
	ZIL_ICHAR line[MAX_LINE_LENGTH], *token[MAX_TOKENS];

	ZIL_TEXT_FILE txtFile(pathName);
	txtFile.ImportTXT(editFile, ZIL_NULLP(ZIL_LANGUAGE), line, token, 0);

	return (event.type);
}

