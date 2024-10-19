//	Zinc Interface Library - Z_TABLE2.CPP
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

static ZIL_ICHAR _rowStringID[] = { 'N','U','M','I','D','_','R','O','W','_','H','E','A','D','E','R', 0 };
static ZIL_ICHAR _colStringID[] = { 'N','U','M','I','D','_','C','O','L','_','H','E','A','D','E','R', 0 };
static ZIL_ICHAR _cnrStringID[] = { 'N','U','M','I','D','_','C','N','R','_','H','E','A','D','E','R', 0 };

// ----- UIW_TABLE_HEADER ---------------------------------------------------

UIW_TABLE_HEADER::UIW_TABLE_HEADER(THF_FLAGS _thFlags, int _recordSize,
	int _maxRecords, void *_data, WOF_FLAGS _woFlags) :
	UIW_TABLE(0, 0, 0, 0, 0, _recordSize, _maxRecords, _data, 0,
	TBLF_NO_FLAGS, _woFlags | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION),
	thFlags(_thFlags)
{
	if (FlagSet(_thFlags, THF_GRID))
		tblFlags |= TBLF_GRID;
	woAdvancedFlags |= WOAF_NON_CURRENT;
	UIW_TABLE_HEADER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_TABLE_HEADER::~UIW_TABLE_HEADER(void)
{
}

void *UIW_TABLE_HEADER::Information(INFO_REQUEST request, void *_data,
	ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_TABLE_HEADER;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_TABLE_HEADER);

		// Update the internal flag settings.
		UIW_TABLE_HEADER::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the object and base class flag settings.
		{
		for (UI_WINDOW_OBJECT *table = parent; table; table = table->parent)
			if (table->Inherited(ID_TABLE))
				return (table->Information(request, _data, ID_TABLE));
		UIW_TABLE::Information(I_CHANGED_FLAGS, _data, objectID);
		}

		// Update the internal flag settings.
		UIW_TABLE_HEADER::UpdateSettings(objectID);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		if (objectID && objectID != ID_TABLE_HEADER)
			_data = UIW_TABLE::Information(request, _data, objectID);
		else if (request == I_GET_FLAGS && !_data)
			_data = &thFlags;
		else if (request == I_GET_FLAGS)
			*(THF_FLAGS *)_data = thFlags;
		else if (request == I_SET_FLAGS)
			thFlags |= *(THF_FLAGS *)_data;
		else
			thFlags &= ~(*(THF_FLAGS *)_data);
		break;

	case I_SET_VSCROLL:
		vScrollInfo = *(UI_SCROLL_INFORMATION *)_data;
		break;

	case I_SET_HSCROLL:
		hScrollInfo = *(UI_SCROLL_INFORMATION *)_data;
		break;

	case I_GET_WIDTH:
	case I_GET_HEIGHT:
		tableRecord->Information(request, _data);
		if (!display->isText)
    		*(int *)_data += 1;
		break;

	default:
		_data = UIW_TABLE::Information(request, _data, objectID);
		break;
	}

	// Return the information.
	return (_data);
}

void UIW_TABLE_HEADER::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the general flag settings.
	if (FlagSet(thFlags, THF_CORNER_HEADER))
	{
		numberID = NUMID_CNR_HEADER;
		strcpy(stringID, _cnrStringID);
	}
	else if (FlagSet(thFlags, THF_COLUMN_HEADER))
	{
		numberID = NUMID_COL_HEADER;
		strcpy(stringID, _colStringID);
	}
	else if (FlagSet(thFlags, THF_ROW_HEADER))
	{
		numberID = NUMID_ROW_HEADER;
		strcpy(stringID, _rowStringID);
	}

	// Check the environment specific flag settings.
	UIW_TABLE_HEADER::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_TABLE_HEADER::NewFunction(void) { return (UIW_TABLE_HEADER::New); }

UIW_TABLE_HEADER::UIW_TABLE_HEADER(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TABLE(0, 0, 0, 0)
{
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TABLE::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TABLE_HEADER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_TABLE_HEADER::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UIW_TABLE::Load(name, directory, file, objectTable, userTable);
	file->Load(&thFlags);
}
#endif

#if defined(ZIL_STORE)
void UIW_TABLE_HEADER::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UIW_TABLE::Store(name, directory, file, objectTable, userTable);
	file->Store(thFlags);
}
#endif
