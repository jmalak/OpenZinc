//	Zinc Interface Library - Z_TABLE1.CPP
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

UIW_TABLE_RECORD::UIW_TABLE_RECORD(int width, int height,
	ZIL_USER_FUNCTION _userFunction, WOF_FLAGS _woFlags) :
	UIW_WINDOW(0, 0, width, height, _woFlags, WOAF_NO_FLAGS)
{
	userFunction = _userFunction;
	UIW_TABLE_RECORD::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_TABLE_RECORD::~UIW_TABLE_RECORD(void)
{
}

void *UIW_TABLE_RECORD::Information(INFO_REQUEST request, void *_data,
	ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_TABLE_RECORD;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_TABLE_RECORD);
		virtualRecord = FALSE;
		editMode = FALSE;
		woStatus |= WOS_OWNERDRAW;

		// Update the internal flag settings.
		UIW_TABLE_RECORD::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the object and base class flag settings.
		{
		for (UI_WINDOW_OBJECT *table = parent; table; table = table->parent)
			if (table->Inherited(ID_TABLE))
				return (table->Information(request, _data, ID_TABLE));
		UIW_WINDOW::Information(I_CHANGED_FLAGS, _data, objectID);
		}

		// Update the internal flag settings.
		UIW_TABLE_RECORD::UpdateSettings(objectID);
		break;

	case I_GET_VALUE:
		*(void **)_data = data;
		break;

	case I_GET_WIDTH:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_NEXTSTEP)
		*(int *)_data = relative.Width();
#elif defined(ZIL_MACINTOSH)
		*(int *)_data = relative.Width() + (FlagSet(woFlags, WOF_BORDER) ? 6 : 4);
#else
		*(int *)_data = relative.Width() + (FlagSet(woFlags, WOF_BORDER) ? 4 : 2);
#endif
		break;

	case I_GET_HEIGHT:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_NEXTSTEP)
		*(int *)_data = relative.Height();
#elif defined(ZIL_MACINTOSH)
		*(int *)_data = relative.Height() + (FlagSet(woFlags, WOF_BORDER) ? 6 : 4);
#elif defined(ZIL_MSWINDOWS) && defined(ZIL_MSWINDOWS_CTL3D)
		*(int *)_data = relative.Height() + (FlagSet(woFlags, WOF_BORDER) ? 6 : 4);
#else
		*(int *)_data = relative.Height() + (FlagSet(woFlags, WOF_BORDER) ? 4 : 2);
#endif
		break;

	default:
		_data = UIW_WINDOW::Information(request, _data, objectID);
		break;
	}

	// Return the information.
	return (_data);
}

void UIW_TABLE_RECORD::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_TABLE_RECORD::OSUpdateSettings(objectID);
}

UIW_TABLE_RECORD *UIW_TABLE_RECORD::VirtualRecord(void)
{
#if defined(ZIL_LOAD) && defined(ZIL_STORE)
	static ZIL_ICHAR name[] = { 'c','o','p','y', 0 };
	ZIL_NEW_FUNCTION newFunction = NewFunction();
	ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
	ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
	UI_WINDOW_OBJECT *storeObject = this;
	storeObject->Store(name, &directory, &file);
	file.Seek(0);
	UIW_TABLE_RECORD *virtualRecord = (UIW_TABLE_RECORD *)(newFunction)
		(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
	virtualRecord->userFunction = userFunction;
	virtualRecord->virtualRecord = TRUE;
#	if !defined(ZIL_OS2) && !defined(ZIL_MSWINDOWS)
	for (UI_WINDOW_OBJECT *object = virtualRecord->First(); object; object = object->Next())
		object->woStatus |= WOS_OWNERDRAW;
#	endif
	return (virtualRecord);
#else
	return ZIL_NULLP(UIW_TABLE_RECORD);
#endif
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_TABLE_RECORD::NewFunction(void) { return (UIW_TABLE_RECORD::New); }

UIW_TABLE_RECORD::UIW_TABLE_RECORD(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 0, 0, WOF_NO_FLAGS)
{
	data = ZIL_NULLP(void);
	recordNum = 0;
	virtualRecord = FALSE;

	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TABLE_RECORD::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_TABLE_RECORD::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
}
#endif

#if defined(ZIL_STORE)
void UIW_TABLE_RECORD::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
}
#endif
