//	Zinc Interface Library - Z_I18N.CPP
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


#include <errno.h>
#include "ui_gen.hpp"
#if defined(__MWERKS__) && !defined(ENOENT)
#	define ENOENT (-43)
#endif

#if defined(ZIL_LOAD) || defined(ZIL_STORE)
static ZIL_ICHAR _pathRoot[] =
{
	ZIL_DIRECTORY_SEPARATOR,
	'Z','I','L','_','I','N','T','E','R','N','A','T','I','O','N','A','L',
	0
};
#endif

ZIL_I18N::ZIL_I18N(void)
{
	className = ZIL_NULLP(ZIL_ICHAR);
	name[0] = 0;
	useCount = 0;
	error = 0;
	allocated = FALSE;
	defaults = FALSE;
}

ZIL_I18N::~ZIL_I18N(void)
{
}

#if defined(ZIL_LOAD)
int ZIL_I18N::Traverse(ZIL_STORAGE_READ_ONLY *storage, const ZIL_ICHAR *_path)
{
	// Change to the locale directory.
	if (!storage)
		return (TRUE);
	storage->ChDir(_pathRoot);
	return (storage->ChDir(_path) != 0);
}

void ZIL_I18N::Load(ZIL_STORAGE_READ_ONLY *storage, ZIL_STORAGE_OBJECT_READ_ONLY *file)
{
	// Load the default information.
	if (storage == ZIL_NULLP(ZIL_STORAGE_READ_ONLY))
		storage = defaultStorage;
	if (Traverse(storage, pathName) || storage->ChDir(name) != 0)
	{
		error = ENOENT;
		return;
	}
	int allocate = FALSE;
	if (!file)
	{
		file = new ZIL_STORAGE_OBJECT_READ_ONLY(*storage, className, 0);
		if ((error = file->objectError) != 0)
		{
			delete file;
			return;
		}
		allocate = TRUE;
	}

	// Load in data from the current directory.
	allocated = TRUE;
	ClassLoadData(file);
	if (allocate)
		delete file;
}
#endif

#if defined(ZIL_STORE)
// ---------------------------------------------------------------------------
int ZIL_I18N::Traverse(ZIL_STORAGE *storage, const ZIL_ICHAR *_path, int create)
{
	// Change to the locale directory.
	if (!storage)
		return (TRUE);
	if (create)
	{
		storage->MkDir(_pathRoot);
		storage->ChDir(_pathRoot);
		storage->MkDir(_path);
		storage->storageError = 0;
		return (storage->ChDir(_path) != 0);
	}
	storage->ChDir(_pathRoot);
	return (storage->ChDir(_path) != 0);
}

void ZIL_I18N::Store(ZIL_STORAGE *storage, ZIL_STORAGE_OBJECT *file)
{
	// Store the default information.
	if (storage == ZIL_NULLP(ZIL_STORAGE))
		storage = (ZIL_STORAGE *)defaultStorage;
	if (Traverse(storage, pathName, TRUE))
		return;
	storage->MkDir(name);
	if (storage->ChDir(name) != 0)
		return;
	int allocate = FALSE;
	if (!file)
	{
		file = new ZIL_STORAGE_OBJECT(*storage, className, 0, UIS_CREATE | UIS_READWRITE);
		allocate = TRUE;		
	}

	// Store data to the current directory.
	ClassStoreData(file);
	if (allocate)
		delete file;
}
#endif

ZIL_I18N_MANAGER::ZIL_I18N_MANAGER(void)
{
}

ZIL_I18N_MANAGER::~ZIL_I18N_MANAGER(void)
{
}

void ZIL_I18N_MANAGER::FreeI18N(const ZIL_I18N *_i18n)
{
	for (UI_ELEMENT *tmp = First(); tmp; tmp = tmp->Next())
	{
		if ((ZIL_I18N *)tmp == _i18n)
		{
			ZIL_I18N *i18n = (ZIL_I18N *)_i18n;
			// If it goes to zero, delete it if not default.
			if (--i18n->useCount == 0 && !i18n->defaults)
			{
				Subtract(i18n);
				delete i18n;
			}
			return;
		}
	}
}

void ZIL_I18N_MANAGER::LoadDefaultI18N(const ZIL_ICHAR *i18nName)
{
#if defined(ZIL_LOAD)
	if (ZIL_INTERNATIONAL::streq(defaultName, i18nName) == 0)
		return;
	UI_ELEMENT *tmp = First();
	if (tmp == ZIL_NULLP(UI_ELEMENT))
	{
		ZIL_I18N *myType = CreateData();
		if (!myType->Traverse(ZIL_I18N::defaultStorage, myType->pathName) && ZIL_I18N::defaultStorage->ChDir(i18nName) == 0)
			ZIL_INTERNATIONAL::strcpy(defaultName, i18nName);
		delete myType;
		return;
	}
	for (; tmp; tmp = tmp->Next())
	{
		ZIL_I18N *def = (ZIL_I18N *)tmp;
		if (def->defaults)
		{
			ZIL_I18N *tmpI18N = CreateData();
			tmpI18N->className = def->className;
			ZIL_INTERNATIONAL::strcpy(tmpI18N->name, i18nName);
			tmpI18N->Load(ZIL_NULLP(ZIL_STORAGE_READ_ONLY), ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY));
			if (tmpI18N->error == 0)
			{
				def->DeleteData();
				ZIL_INTERNATIONAL::strcpy(def->name, i18nName);
				def->allocated = TRUE;
				def->AssignData(tmpI18N);
				ZIL_INTERNATIONAL::strcpy(defaultName, i18nName);
			}
			tmpI18N->allocated = FALSE;
			delete tmpI18N;
		}
	}
#endif
}

ZIL_I18N *ZIL_I18N_MANAGER::UseI18N(const ZIL_I18N *i18n)
{
	for (UI_ELEMENT *tmp = First(); tmp; tmp = tmp->Next())
	{
		if ((ZIL_I18N *)tmp == i18n)
		{
			((ZIL_I18N *)tmp)->useCount++;
			return ((ZIL_I18N *)tmp);
		}
	}
	return (ZIL_NULLP(ZIL_I18N));
}

ZIL_I18N *ZIL_I18N_MANAGER::UseI18N(const ZIL_ICHAR *className, const ZIL_ICHAR *i18nName)
{
	if (!initialized)
		return ZIL_NULLP(ZIL_I18N);
#if defined(ZIL_LOAD)
	ZIL_I18N *useDefault = ZIL_NULLP(ZIL_I18N);
	for (UI_ELEMENT *tmp = First(); tmp; tmp = tmp->Next())
	{
		ZIL_I18N *def = (ZIL_I18N *)tmp;
		if (ZIL_INTERNATIONAL::streq(def->className, className) == 0)
		{
			if (def->defaults)
				useDefault = def;
			if  ((i18nName == ZIL_NULLP(ZIL_ICHAR) && def->defaults) ||
			     (i18nName != ZIL_NULLP(ZIL_ICHAR) && ZIL_INTERNATIONAL::streq(def->name, i18nName) == 0))
			{
				def->useCount++;
				return (def);
			}
		}
	}
	ZIL_I18N *def = CreateData();
	def->className = className;
	ZIL_INTERNATIONAL::strcpy(def->name, i18nName);
	def->Load(ZIL_NULLP(ZIL_STORAGE_READ_ONLY), ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY));
	if (def->error != 0)
	{
		delete def;
		useDefault->useCount++;
		return (useDefault);
	}
	def->className = className;
	ZIL_INTERNATIONAL::strcpy(def->name, i18nName);
	def->allocated = TRUE;
	def->defaults = ZIL_INTERNATIONAL::streq(i18nName, defaultName) == 0;
	def->useCount = 1;
	Add(def);
	return (def);
#else
	for (UI_ELEMENT *tmp = First(); tmp; tmp = tmp->Next())
	{
		ZIL_I18N *def = (ZIL_I18N *)tmp;
		if (ZIL_INTERNATIONAL::streq(def->className, className) == 0 &&
		    def->defaults)
		{
			def->useCount++;
			return (def);
		}
	}
	return (ZIL_NULLP(ZIL_I18N));
#endif
}

void ZIL_I18N::AssignData(const ZIL_I18N *) {}
void ZIL_I18N::DeleteData() {}
#if defined(ZIL_LOAD)
	void ZIL_I18N::ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *) {}
#endif
#if defined(ZIL_STORE)
	void ZIL_I18N::ClassStoreData(ZIL_STORAGE_OBJECT *) {}
#endif
