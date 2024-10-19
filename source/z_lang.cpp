//	Zinc Interface Library - Z_LANG.CPP
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


#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#	include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_gen.hpp"

#define CP_ISO8859_1	1
#define CP_UNICODE	2
#define CP_OTHER	2

static ZIL_ICHAR _pathLanguage[] =
{
	'L','A','N','G','U','A','G','E',
	0
};
ZIL_LANGUAGE_MANAGER languageManager;
static ZIL_ICHAR _defaultName[12] = { 0 };
static ZIL_ICHAR _defaultOSName[12] = { 0 };

#define ZIL_LANGUAGE_DATA
#include "lang_def.cpp"

extern ZIL_ICHAR *_zilExtName;
static ZIL_ICHAR _messageDirectory[]	= { '~','U','I','_','M','E','S','S','A','G','E', 0 };

// ----- ZIL_LANGUAGE_ELEMENT --------------------------------------------------------

/* START BLOCK COMMENT
**	ZIL_LANGUAGE_ELEMENT::ZIL_LANGUAGE_ELEMENT()
**	{
**		text = ZIL_NULLP(ZIL_ICHAR);
**		numberID = 0;
**		*stringID = 0;
**	}
END BLOCK COMMENT */

void ZIL_LANGUAGE_ELEMENT::SwapData(ZIL_LANGUAGE_ELEMENT &language)
{
	int _numberID = numberID;
	numberID = language.numberID;
	language.numberID = _numberID;

	ZIL_ICHAR _stringID[ZIL_STRINGID_LEN];
	ZIL_INTERNATIONAL::strcpy(_stringID, stringID);
	ZIL_INTERNATIONAL::strcpy(stringID, language.stringID);
	ZIL_INTERNATIONAL::strcpy(language.stringID, _stringID);

	ZIL_ICHAR *_text = ZIL_LANGUAGE_ELEMENT::text;
#if defined(ZIL_NEXTSTEP)
	(ZIL_ICHAR *)ZIL_LANGUAGE_ELEMENT::text = language.text;
#else
	ZIL_LANGUAGE_ELEMENT::text = language.text;
#endif
	language.text = _text;
}

// ----- ZIL_LANGUAGE ---------------------------------------------------

ZIL_LANGUAGE::ZIL_LANGUAGE(void) : ZIL_I18N()
{
	noOfElements = 0;
	data = ZIL_NULLP(ZIL_LANGUAGE_ELEMENT);
	pathName = _pathLanguage;
}

ZIL_LANGUAGE::~ZIL_LANGUAGE(void)
{
	ZIL_LANGUAGE::DeleteData();
}

void ZIL_LANGUAGE::AssignData(const ZIL_I18N *_data)
{
	noOfElements = ((ZIL_LANGUAGE *)_data)->noOfElements;
	data = ((ZIL_LANGUAGE *)_data)->data;
}

void ZIL_LANGUAGE::DeleteData(void)
{
	if (allocated)
	{
		for (int i=0; i < noOfElements; i++)
			delete data[i].text;
		delete []data;
	}
}

ZIL_ICHAR *ZIL_LANGUAGE::GetMessage(ZIL_NUMBERID numberID, int useDefault) const
{
	for (int i = 0; i < noOfElements; i++)
		if (data[i].numberID == numberID)
			return (data[i].text);
	return (useDefault ? data[0].text : ZIL_NULLP(ZIL_ICHAR));
}

#if defined(ZIL_LOAD)
ZIL_LANGUAGE::ZIL_LANGUAGE(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file) : ZIL_I18N()
{
	noOfElements = 0;
	data = ZIL_NULLP(ZIL_LANGUAGE_ELEMENT);
	ZIL_LANGUAGE::Load(name, directory, file);
}

void ZIL_LANGUAGE::Load(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file)
{
	// Check for a valid directory and file.
	int tempDirectory = FALSE, tempFile = FALSE;
	if (!file)
	{
		// Check for a valid name.
		if (!name)
			goto LOAD_ERROR;
		ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
		ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);

		// Check for a valid directory.
		if (!directory)
		{
			tempDirectory = TRUE;
			ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
			ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
			directory = new ZIL_STORAGE_READ_ONLY(pathName);
			if (directory->storageError)
				goto LOAD_ERROR;
		}

		// Check for a valid file.
		tempFile = TRUE;
		if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
			;
		else
			directory->ChDir(_messageDirectory);
		if (objectName[0] == '\0')
			ZIL_INTERNATIONAL::strcpy(objectName, fileName);
		file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, objectName, ID_MESSAGE);
		if (file->objectError)
			goto LOAD_ERROR;
	}

	// Allocate and read the message block.
	{
	DeleteData();
	ZIL_INT16 total; file->Load(&total); noOfElements = total;
	data = new ZIL_LANGUAGE_ELEMENT[total];
	for (int i = 0; i < noOfElements; i++)
	{
		file->Load(&data[i].numberID);
		file->Load(data[i].stringID, ZIL_STRINGID_LEN);
		file->Load(&data[i].text);
	}
	allocated = TRUE;
	}

	// Delete the temporary objects.
LOAD_ERROR:
	if (tempFile)
		delete file;
	if (tempDirectory)
		delete directory;
}
#endif

#if defined(ZIL_STORE)
void ZIL_LANGUAGE::Store(const ZIL_ICHAR *name,
	ZIL_STORAGE *directory, ZIL_STORAGE_OBJECT *file)
{
	// Check for a valid directory and file.
	int tempDirectory = FALSE, tempFile = FALSE;
	if (!file)
	{
		// Check for a valid name.
		if (!name)
			goto STORE_ERROR;
		ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
		ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);

		// Check for a valid directory.
		if (!directory)
		{
			ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
			ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
			directory = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
			if (directory->storageError)
				goto STORE_ERROR;
			tempDirectory = TRUE;
		}

		// Check for a valid file.
		if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
			;
		else if (directory->ChDir(_messageDirectory))
		{
			directory->MkDir(_messageDirectory);
			directory->ChDir(_messageDirectory);
		}
		if (objectName[0] == '\0')
			ZIL_INTERNATIONAL::strcpy(objectName, fileName);
		file = new ZIL_STORAGE_OBJECT(*directory, objectName, ID_MESSAGE, UIS_CREATE | UIS_READWRITE);
		tempFile = TRUE;
	}

	// Allocate and read the message block.
	{
	ZIL_INT16 value = noOfElements; file->Store(value);
	for (int i = 0; i < noOfElements; i++)
	{
		file->Store(data[i].numberID);
		file->Store(data[i].stringID);
		file->Store(data[i].text);
	}
	}

	// Delete the temporary objects.
STORE_ERROR:
	if (tempFile)
		delete file;
	if (tempDirectory)
		delete directory;
}
#endif

//--- Language Setup ---------------------------------------------------------

ZIL_LANGUAGE_MANAGER::ZIL_LANGUAGE_MANAGER(void)
{
	initialized = TRUE;
	defaultName = _defaultName;
	defaultOSName = _defaultOSName;
}

void ZIL_LANGUAGE_MANAGER::SetLanguage(const ZIL_ICHAR *className,
		ZIL_PRIVATE_LANGUAGE_ELEMENT *defaultLanguage)
{
	if (!languageManager.initialized)	// patch
		return;				// patch
	if (_defaultName[0] == 0)
	{
		ZIL_INTERNATIONAL::strcpy(_defaultName, isoLanguageName);
	}
	ZIL_LANGUAGE *def = new ZIL_LANGUAGE;
	def->className = className;
	ZIL_INTERNATIONAL::strcpy(def->name, isoLanguageName);
	int len = 0;
	for (; defaultLanguage[len].text; len++)
		;
	def->noOfElements = len;
#if (defined(ZIL_UNICODE) && (langCodeset == CP_ISO8859_1))
	def->data = new ZIL_LANGUAGE_ELEMENT[len];
	for (int i = 0; i < len; i++)
	{
		def->data[i].text = ZIL_INTERNATIONAL::ISOtoUNICODE(defaultLanguage[i].text);
		def->data[i].numberID = defaultLanguage[i].numberID;
		ZIL_INTERNATIONAL::ISOtoUNICODE(defaultLanguage[i].stringID, def->data[i].stringID);
	}
	def->allocated = TRUE;
#elif (!defined(ZIL_UNICODE) && (langCodeset == CP_UNICODE))
	????;
#elif (langCodeset == CP_ISO8859_1 || langCodeset == CP_UNICODE)
	def->data = defaultLanguage;
	def->allocated = FALSE;
#else
	def->data = new ZIL_LANGUAGE_ELEMENT[len];
	ZIL_MAP_CHARS *map = new ZIL_MAP_CHARS(langCodesetName, ZIL_INTERNATIONAL::machineName);
	for (int i = 0; i < len; i++)
	{
		def->data[i].text = map->UnMapText(defaultLanguage[i].text);
		def->data[i].numberID = defaultLanguage[i].numberID;
		map->UnMapText(defaultLanguage[i].stringID, def->data[i].stringID);
	}
	delete map;
	def->allocated = TRUE;
#endif
	def->defaults = TRUE;
	def->useCount = 0;
	languageManager.Add(def);
	if (ZIL_INTERNATIONAL::streq(_defaultName, isoLanguageName) != 0)
	{
		ZIL_LANGUAGE *tmp = (ZIL_LANGUAGE *)UseLanguage(className, _defaultName);
		if (tmp != def)
		{
			def->defaults = FALSE;
			tmp->defaults = TRUE;
		}
	}
}

//----------------------------------------------------------------------------

#if defined(ZIL_LOAD)
void ZIL_LANGUAGE::ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *file)
{
	// Load in messages from the current directory.
	ZIL_UINT16 count; file->Load(&count); noOfElements = count;
	data = new ZIL_LANGUAGE_ELEMENT[noOfElements];
	for (int i = 0; i < noOfElements && !file->objectError; i++)
	{
		file->Load(&data[i].numberID);
		file->Load(data[i].stringID, ZIL_STRINGID_LEN);
		file->Load(&data[i].text);
		if (!data[i].text)
			data[i].text = ZIL_INTERNATIONAL::strdup(ZIL_INTERNATIONAL::_blankString);
	}
	error = file->objectError;
}
#endif

#if defined(ZIL_STORE)
void ZIL_LANGUAGE::ClassStoreData(ZIL_STORAGE_OBJECT *file)
{
	// Store language to the current directory.
	ZIL_UINT16 count = noOfElements; file->Store(count);
	for (int i = 0; i < noOfElements && !file->objectError; i++)
	{
		file->Store(data[i].numberID);
		file->Store(data[i].stringID);
		file->Store(data[i].text);
	}
	error = file->objectError;
}
#endif
