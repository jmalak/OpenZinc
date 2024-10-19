//	Zinc Interface Library - Z_STORED.CPP
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


extern "C"
{
#	include <errno.h>
}
#include "ui_gen.hpp"

ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::ZIL_DELTA_STORAGE_OBJECT_READ_ONLY
	(ZIL_STORAGE_OBJECT_READ_ONLY *_object,
	 ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name, ZIL_OBJECTID nObjectID) :
	ZIL_STORAGE_OBJECT_READ_ONLY(file, name, nObjectID), object(_object),
	current(0)
{
	if (objectError == 0)
	{
		ZIL_UINT32 boolPosition;
		ZIL_STORAGE_OBJECT_READ_ONLY::Load(&boolPosition);
		ZIL_UINT32 startPosition = Tell();
		Seek(boolPosition);
		bools = new ZIL_BIT_VECTOR(this);
		Seek(startPosition);
		if (object)
			object->Seek(0);
	}
	else
		bools = ZIL_NULLP(ZIL_BIT_VECTOR);
}

ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::~ZIL_DELTA_STORAGE_OBJECT_READ_ONLY(void)
{
	object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	if (bools)
		delete bools;
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_INT16 *value)
{
	int i = sizeof(*value);
	if (object)
		i = object->Load(value);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(value);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_UINT16 *value)
{
	int i = sizeof(*value);
	if (object)
		i = object->Load(value);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(value);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_INT32 *value)
{
	int i = sizeof(*value);
	if (object)
		i = object->Load(value);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(value);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_UINT32 *value)
{
	int i = sizeof(*value);
	if (object)
		i = object->Load(value);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(value);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_UINT8 *value)
{
	int i = sizeof(*value);
	if (object)
		i = object->Load(value);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(value);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_INT8 *value)
{
	int i = sizeof(*value);
	if (object)
		i = object->Load(value);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(value);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(void *buff, int size, int length)
{
	int i = size * length;
	if (object)
		i = object->Load(buff, size, length);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(buff, size, length);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_ICHAR *string, int length)
{
	int i = 0;
	if (object)
		i = object->Load(string, length);
	if (bools->Test(current++))
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(string, length);
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT_READ_ONLY::Load(ZIL_ICHAR **string)
{
	int i = 0;
	if (object)
		i = object->Load(string);
	if (bools->Test(current++))
	{
		delete *string;
		i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(string);
	}
	return (i);
}

#if defined(ZIL_STORE)
ZIL_DELTA_STORAGE_OBJECT::ZIL_DELTA_STORAGE_OBJECT
	(ZIL_STORAGE_OBJECT_READ_ONLY *_object,
	 ZIL_STORAGE &file, const ZIL_ICHAR *name,
	 ZIL_OBJECTID nObjectID, UIS_FLAGS pFlags) :
	ZIL_STORAGE_OBJECT(file, name, nObjectID, pFlags), object(_object),
	current(0)
{
	if (!object)
#if defined(ZIL_MACINTOSH)
		objectError = rfNumErr;
#else
		objectError = EINVAL;
#endif
	if (objectError == 0)
	{
		ZIL_UINT32 boolPosition = 0;
		ZIL_STORAGE_OBJECT::Store(boolPosition);
		object->Seek(0);
		bools = new ZIL_BIT_VECTOR();
	}
}

ZIL_DELTA_STORAGE_OBJECT::~ZIL_DELTA_STORAGE_OBJECT(void)
{
	object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	ZIL_UINT32 tmpPosition = Tell();
	bools->Store(this);
	Seek(0);
	ZIL_STORAGE_OBJECT::Store(tmpPosition);
	delete bools;
}

int ZIL_DELTA_STORAGE_OBJECT::Store(ZIL_INT16 value)
{
	ZIL_INT16 oldValue;
	int i = object->Load(&oldValue);
	if (value != oldValue)
	{
		i = ZIL_STORAGE_OBJECT::Store(value);
		bools->Set(current);
	}
	current++;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(ZIL_UINT16 value)
{
	ZIL_UINT16 oldValue;
	int i = object->Load(&oldValue);
	if (value != oldValue)
	{
		i = ZIL_STORAGE_OBJECT::Store(value);
		bools->Set(current);
	}
	current++;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(ZIL_INT32 value)
{
	ZIL_INT32 oldValue;
	int i = object->Load(&oldValue);
	if (value != oldValue)
	{
		i = ZIL_STORAGE_OBJECT::Store(value);
		bools->Set(current);
	}
	current++;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(ZIL_UINT32 value)
{
	ZIL_UINT32 oldValue;
	int i = object->Load(&oldValue);
	if (value != oldValue)
	{
		i = ZIL_STORAGE_OBJECT::Store(value);
		bools->Set(current);
	}
	current++;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(ZIL_INT8 value)
{
	ZIL_INT8 oldValue;
	int i = object->Load(&oldValue);
	if (value != oldValue)
	{
		i = ZIL_STORAGE_OBJECT::Store(value);
		bools->Set(current);
	}
	current++;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(ZIL_UINT8 value)
{
	ZIL_UINT8 oldValue;
	int i = object->Load(&oldValue);
	if (value != oldValue)
	{
		i = ZIL_STORAGE_OBJECT::Store(value);
		bools->Set(current);
	}
	current++;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(void *buff, int size, int length)
{
	char *oldBuff = new char[size*length];
	int i = object->Load(oldBuff, size, length);
	if (memcmp(buff, oldBuff, size*length) != 0)
	{
		i = ZIL_STORAGE_OBJECT::Store(buff, size, length);
		bools->Set(current);
	}
	current++;
	delete oldBuff;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(const ZIL_ICHAR *string)
{
	ZIL_ICHAR *oldString;
	int i = object->Load(&oldString);
	if ((oldString != string) &&
	    (oldString == ZIL_NULLP(ZIL_ICHAR) || string == ZIL_NULLP(ZIL_ICHAR) || strcmp(string, oldString) != 0))
	{
		i = ZIL_STORAGE_OBJECT::Store(string);
		bools->Set(current);
	}
	current++;
	delete oldString;
	return (i);
}

int ZIL_DELTA_STORAGE_OBJECT::Store(const directoryEntry *dirent)
{
	return (ZIL_STORAGE_OBJECT::Store(dirent));
}
#endif

