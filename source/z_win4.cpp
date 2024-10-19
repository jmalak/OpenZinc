//	Zinc Interface Library - Z_WIN4.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

/*       This file is a part of OpenZinc

          OpenZinc is free software; you can redistribute it and/or modify it under
          the terms of the GNU Lessor General Public License as published by
          the Free Software Foundation, either version 3 of the License, or (at
          your option) any later version

	OpenZinc is distributed in the hope that it will be useful, but WITHOUT
          ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
          or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
          General Public License for more details.

          You should have received a copy of the GNU Lessor General Public License
	 along with OpenZinc. If not, see <http://www.gnu.org/licenses/>                          */


#include "ui_win.hpp"

extern ZIL_ICHAR *_windowDirectory;
extern ZIL_ICHAR *_zilExtName;
static ZIL_ICHAR _deltaWindowDirectory[] = { 'U','I','W','_','W','I','N','D','O','W',0 };
static ZIL_ICHAR _deltaDirectory[] = { '~','Z','I','L','_','D','E','L','T','A',0 };

#if defined(ZIL_LOAD)
UIW_WINDOW::UIW_WINDOW(const ZIL_ICHAR *name, const ZIL_ICHAR *deltaName,
	const ZIL_ICHAR *deltaPathName,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_READ_ONLY *deltaDirectory,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 40, 10, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	UI_LIST(), support(), clipList(), defaultObject(ZIL_NULLP(UI_WINDOW_OBJECT)),
	myLanguage(ZIL_NULLP(ZIL_LANGUAGE))
{
	compareFunctionName = ZIL_NULLP(ZIL_ICHAR);
	ZIL_INT32 miniNumeratorX = display->miniNumeratorX;
	ZIL_INT32 miniDenominatorX = display->miniDenominatorX;
	ZIL_INT32 miniNumeratorY = display->miniNumeratorY;
	ZIL_INT32 miniDenominatorY = display->miniDenominatorY;
	UIW_WINDOW::Information(I_INITIALIZE_CLASS_FIRST, ZIL_NULLP(void));
	DeltaLoad(name, deltaName, deltaPathName, directory, deltaDirectory, objectTable, userTable);
	if (!FlagSet(woStatus, WOS_READ_ERROR))
	{
		UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	}
	display->miniNumeratorX = miniNumeratorX;
	display->miniDenominatorX = miniDenominatorX;
	display->miniNumeratorY = miniNumeratorY;
	display->miniDenominatorY = miniDenominatorY;
}

void UIW_WINDOW::DeltaLoad(const ZIL_ICHAR *name, const ZIL_ICHAR *deltaName,
	const ZIL_ICHAR *deltaPathName,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_READ_ONLY *deltaDirectory,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
	ZIL_STORAGE_OBJECT_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	ZIL_DELTA_STORAGE_OBJECT_READ_ONLY *deltaFile = ZIL_NULLP(ZIL_DELTA_STORAGE_OBJECT_READ_ONLY);
	int tempDirectory = !directory;
	int tempDeltaDirectory = !deltaDirectory;

	ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
	if (tempDirectory)
	{
		ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
		ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
		directory = new ZIL_STORAGE_READ_ONLY(pathName);
		if (directory->storageError)
		{
			UIW_WINDOW::Error(WOS_READ_ERROR, myLanguage->GetMessage(ZIL_NO_FILE), fileName);
			goto GENERAL_LOAD_ERROR;
		}
	}
	if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
		;
	else
		directory->ChDir(_windowDirectory);
	if (objectName[0] == '\0')
		strcpy(objectName, fileName);
	file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, objectName, ID_WINDOW);
	if (file->objectError)
	{
		Error(WOS_READ_ERROR, myLanguage->GetMessage(ZIL_NO_OBJ), objectName);
		goto GENERAL_LOAD_ERROR;
	}

	ZIL_STORAGE::StripFullPath(deltaName, pathName, fileName, objectName, objectPathName);
	if (tempDeltaDirectory)
	{
		ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
		ZIL_ICHAR tmp[20];
		tmp[0] = '.';
		strcpy(&tmp[1], deltaPathName ? deltaPathName : languageManager.defaultName);
		ZIL_STORAGE::ChangeExtension(pathName, tmp);
		deltaDirectory = new ZIL_STORAGE_READ_ONLY(pathName);
		if (deltaDirectory->storageError)
			goto DELTA_LOAD_ERROR;
	}
	else
	{
		deltaDirectory->ChDir(_deltaDirectory);
		if (deltaPathName)
			deltaDirectory->ChDir(deltaPathName);
		else
		{
			// If there is a special language_locale entry, use it
			ZIL_ICHAR specialName[40];
			strcpy(specialName, languageManager.defaultName);
			int i = strlen(languageManager.defaultName);
			specialName[i] = '_';
			strcpy(&specialName[i+1], localeManager.defaultName);
			if (deltaDirectory->ChDir(specialName) != 0)
				deltaDirectory->ChDir(languageManager.defaultName);
		}
	}
	deltaDirectory->ChDir(_deltaWindowDirectory);
	if (objectName[0] == '\0')
		strcpy(objectName, fileName);
	deltaFile = new ZIL_DELTA_STORAGE_OBJECT_READ_ONLY(file, *deltaDirectory, objectName, ID_WINDOW);
	if (deltaFile->objectError)
		goto DELTA_LOAD_ERROR;

DELTA_LOAD_ERROR:
	if (deltaFile && !deltaFile->objectError) // delta loading.
	{
		deltaFile->Load(&display->miniNumeratorX);		// miniNumeratorX
		deltaFile->Load(&display->miniDenominatorX);	// miniDenominatorX
		deltaFile->Load(&display->miniNumeratorY);		// miniNumeratorY
		deltaFile->Load(&display->miniDenominatorY);	// miniDenominatorY
		UIW_WINDOW::Load(deltaName, deltaDirectory, deltaFile, objectTable, userTable);
	}
	else // conventional loading.
	{
		file->Load(&display->miniNumeratorX);	// miniNumeratorX
		file->Load(&display->miniDenominatorX);	// miniDenominatorX
		file->Load(&display->miniNumeratorY);	// miniNumeratorY
		file->Load(&display->miniDenominatorY);	// miniDenominatorY
		UIW_WINDOW::Load(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
	}

GENERAL_LOAD_ERROR:
	// Clean up the file and storage.
	delete file;
	if (deltaFile)
		delete deltaFile;
	if (tempDeltaDirectory)
		delete deltaDirectory;
	if (tempDirectory)
		delete directory;
}
#endif

#if defined(ZIL_STORE)
void UIW_WINDOW::DeltaStore(const ZIL_ICHAR *name, const ZIL_ICHAR *deltaName,
	const ZIL_ICHAR *deltaPathName,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE *deltaDirectory,
	UI_ITEM *objectTable, UI_ITEM *userTable, int appendNames)
{
	ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
	ZIL_STORAGE_OBJECT_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	ZIL_DELTA_STORAGE_OBJECT *deltaFile = ZIL_NULLP(ZIL_DELTA_STORAGE_OBJECT);
	int tempDirectory = !directory;
	int tempDeltaDirectory = !deltaDirectory;

	ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
	if (tempDirectory)
	{
		ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
		ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
		directory = new ZIL_STORAGE_READ_ONLY(pathName);
		if (directory->storageError)
			goto STORE_ERROR;
	}
	if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
		;
	else
		directory->ChDir(_windowDirectory);
	if (objectName[0] == '\0')
		strcpy(objectName, fileName);
	file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, objectName, ID_WINDOW);
	if (file->objectError)
		goto STORE_ERROR;
	
	ZIL_STORAGE::StripFullPath(deltaName, pathName, fileName, objectName, objectPathName);
	if (tempDeltaDirectory)
	{
		ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
		ZIL_ICHAR tmp[20];
		tmp[0] = '.';
		strcpy(&tmp[1], deltaPathName ? deltaPathName : languageManager.defaultName);
		ZIL_STORAGE::ChangeExtension(pathName, tmp);
		deltaDirectory = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
		if (deltaDirectory->storageError)
			goto STORE_ERROR;
	}
	else
	{
		if (deltaDirectory->ChDir(_deltaDirectory) != 0)
		{
			deltaDirectory->MkDir(_deltaDirectory);
			deltaDirectory->ChDir(_deltaDirectory);
		}
		if (deltaPathName)
		{
			ZIL_ICHAR tmpPathName[100];
			strcpy(tmpPathName, deltaPathName);
			tmpPathName[strlen(tmpPathName)+1] = 0;
			ZIL_ICHAR *strt = tmpPathName, *stop = tmpPathName;
			while (*stop)
			{
				while (*stop && *stop != ZIL_DIRECTORY_SEPARATOR)
					stop++;
				*stop = 0;
				if (deltaDirectory->ChDir(strt) != 0)
				{
					deltaDirectory->MkDir(strt);
					deltaDirectory->ChDir(strt);
				}
				strt = ++stop;
			}
			
		}
		else
		{
			ZIL_ICHAR specialName[40];
			strcpy(specialName, languageManager.defaultName);
			if (appendNames)
			{
				int i = strlen(languageManager.defaultName);
				specialName[i] = '_';
				strcpy(&specialName[i+1], localeManager.defaultName);
			}
			// Create the special language_locale entry.
			if (deltaDirectory->ChDir(specialName) != 0)
			{
				deltaDirectory->MkDir(specialName);
				deltaDirectory->ChDir(specialName);
			}
		}
	}
	if (deltaDirectory->ChDir(_deltaWindowDirectory) != 0)
	{
		deltaDirectory->MkDir(_deltaWindowDirectory);
		deltaDirectory->ChDir(_deltaWindowDirectory);
	}
	if (objectName[0] == '\0')
		strcpy(objectName, fileName);
	StringID(objectName);
	deltaFile = new ZIL_DELTA_STORAGE_OBJECT(file, *deltaDirectory, objectName, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
	if (deltaFile->objectError)
		goto STORE_ERROR;
	deltaFile->Store(display->miniNumeratorX);	// miniNumeratorX
	deltaFile->Store(display->miniDenominatorX);	// miniDenominatorX
	deltaFile->Store(display->miniNumeratorY);	// miniNumeratorY
	deltaFile->Store(display->miniDenominatorY);	// miniDenominatorY
	UIW_WINDOW::Store(deltaName, deltaDirectory, deltaFile, objectTable, userTable);

STORE_ERROR:
	// Clean up the file and storage.
	delete file;
	delete deltaFile;
	if (tempDeltaDirectory)
	{
		deltaDirectory->Save();
		delete deltaDirectory;
	}
	if (tempDirectory)
		delete directory;
}
#endif
