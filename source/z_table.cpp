//	Zinc Interface Library - Z_TABLE.CPP
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

// ----- UIW_TABLE ----------------------------------------------------------

UIW_TABLE::UIW_TABLE(int left, int top, int width, int height, int _columns,
	int _recordSize, int _maxRecords, void *_data, int _records,
	TBLF_FLAGS _tblFlags, WOF_FLAGS _woFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags, WOAF_NO_FLAGS),
	columns(_columns), recordSize(_recordSize), maxRecords(_maxRecords),
	records(_records), tblFlags(_tblFlags),
	tableRecord(ZIL_NULLP(UIW_TABLE_RECORD)),
	virtualRecord(ZIL_NULLP(UIW_TABLE_RECORD)),
	columnHeader(ZIL_NULLP(UI_WINDOW_OBJECT)),
	rowHeader(ZIL_NULLP(UI_WINDOW_OBJECT))
{
	// Initialize the table data.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA) && _maxRecords > 0)
	{
		data = new ZIL_INT8[_recordSize * _maxRecords];
		memset(data, 0, _recordSize * _maxRecords);
	}
	else
		data = ZIL_NULLP(void);
	DataSet(_data, _records, _maxRecords);

	UIW_TABLE::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_TABLE::~UIW_TABLE(void)
{
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete [] (ZIL_INT8 *)data;
	if (virtualRecord)
		delete virtualRecord;
}

void *UIW_TABLE::DataGet(int *_records)
{
	if (_records)
		*_records = records;
	return (data);
}

int UIW_TABLE::DataSet(void *_data, int _records, int _maxRecords)
{
	// Change the record and max-record count.
	if (_maxRecords >= 0)
	{
		if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA) &&
			_maxRecords > maxRecords)
		{
			void *newData = new ZIL_INT8[recordSize * _maxRecords];
			memset(newData, 0, recordSize * _maxRecords);
			if (data)
			{
				memcpy(newData, data, recordSize * maxRecords);
				delete [] (ZIL_INT8 *)data;
			}
			data = newData;
		}
		maxRecords = _maxRecords;
	}
	if (_records >= 0)
		records = _records;

	// Set the new data for the table.
	if (_data)
	{
		if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			data = _data;
		else
		{
			if (!data)
				data = new ZIL_INT8[recordSize * maxRecords];
			memcpy(data, _data, recordSize * records);
		}
	}

	if (currentRecord >= records)  // Preserve the current record if possible
		currentRecord = records - 1;
	else if (currentRecord == -1 && records)
		currentRecord = 0;

	if (screenID)
	{
		if (records > 0)
		{
			// Update the data in the current record.
			UI_EVENT dataEvent(S_SET_DATA);
			dataEvent.rawCode = currentRecord;
			dataEvent.data = GetRecord(currentRecord);
			tableRecord->Event(dataEvent);
			SetCurrent(currentRecord);
		}

		// Redisplay the table.
		UI_EVENT tEvent(S_SCROLLRANGE);
		ScrollEvent(tEvent);
		Event(UI_EVENT(S_REDISPLAY));
	}

	// Return the new record count.
	return (records);
}

void UIW_TABLE::InsertRecord(int recordNum, void *_data)
{
	// Make sure data gets saved.
	if (currentRecord >= 0)
		tableRecord->Event(UI_EVENT(S_NON_CURRENT));

 	if (data)
	{
		if (records < maxRecords)
		{
			memmove(GetRecord(recordNum + 1), GetRecord(recordNum),
				(records - recordNum) * recordSize);
			memcpy(GetRecord(recordNum), _data, recordSize);
			records++;
		}
		else
			return;
	}
	else
		records++;

	if (!Inherited(ID_TABLE_HEADER) && currentRecord != recordNum)
		SetCurrent(recordNum);
	else
	{
		UI_EVENT dataEvent(S_SET_DATA);
		dataEvent.rawCode = currentRecord;
		dataEvent.data = GetRecord(currentRecord);
		tableRecord->Event(dataEvent);
	}
}

void UIW_TABLE::DeleteRecord(int recordNum)
{
	if (data)
		memmove(GetRecord(recordNum), GetRecord(recordNum + 1),
			(records - recordNum - 1) * recordSize);

	records--;

	if (currentRecord >= recordNum)
	{
		if (currentRecord > recordNum || currentRecord > records - 1)
		{
			currentRecord--;
			if (currentRecord >= 0)
				SetCurrent(currentRecord);
		}
		else
		{
			UI_EVENT dataEvent(S_SET_DATA);
			dataEvent.rawCode = currentRecord;
			dataEvent.data = GetRecord(currentRecord);
			tableRecord->Event(dataEvent);
		}
	}
}

void *UIW_TABLE::Information(INFO_REQUEST request, void *_data,
	ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_TABLE;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_TABLE);
		woStatus |= WOS_OWNERDRAW;

		// Update the internal flag settings.
		UIW_TABLE::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, _data, ID_TABLE);

		// Update the internal flag settings.
		UIW_TABLE::UpdateSettings(objectID);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		if (objectID && objectID != ID_TABLE)
			_data = UIW_WINDOW::Information(request, _data, objectID);
		else if (request == I_GET_FLAGS && !_data)
			_data = &tblFlags;
		else if (request == I_GET_FLAGS)
			*(TBLF_FLAGS *)_data = tblFlags;
		else if (request == I_SET_FLAGS)
			tblFlags |= *(TBLF_FLAGS *)_data;
		else
			tblFlags &= ~(*(TBLF_FLAGS *)_data);
		break;

	case I_SET_TABLE_RECORD:
		tableRecord = (UIW_TABLE_RECORD *)_data;
		break;

	case I_SET_VIRTUAL_RECORD:
		if (virtualRecord && virtualRecord != (UI_WINDOW_OBJECT *)_data)
			delete virtualRecord;
		virtualRecord = (UI_WINDOW_OBJECT *)_data;
		break;

	case I_SET_COL_HEADER:
		columnHeader = (UI_WINDOW_OBJECT *)_data;
		break;

	case I_SET_ROW_HEADER:
		rowHeader = (UI_WINDOW_OBJECT *)_data;
		break;

	case I_GET_COLUMNS:
		*(int *)_data = columns;
		break;

	case I_GET_RECORDS:
		*(int *)_data = records;
		break;

	case I_GET_CORNER_WIDTH:
		*(int *)_data = 0;
		if (rowHeader)
			rowHeader->Information(I_GET_WIDTH, _data);
		break;

	case I_GET_CORNER_HEIGHT:
		*(int *)_data = 0;
		if (columnHeader)
			columnHeader->Information(I_GET_HEIGHT, _data);
		break;

	case I_GET_RECORD_HEIGHT:
		tableRecord->Information(I_GET_HEIGHT, _data);
		break;

	case I_GET_RECORD_WIDTH:
		tableRecord->Information(I_GET_WIDTH, _data);
		break;

	default:
		_data = UIW_WINDOW::Information(request, _data, objectID);
		break;
	}

	// Return the information.
	return (_data);
}

void UIW_TABLE::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_TABLE::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_TABLE::NewFunction(void) { return (UIW_TABLE::New); }

UIW_TABLE::UIW_TABLE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 0, 0, WOF_NO_FLAGS),
	tableRecord(ZIL_NULLP(UIW_TABLE_RECORD)),
	virtualRecord(ZIL_NULLP(UIW_TABLE_RECORD)),
	columnHeader(ZIL_NULLP(UI_WINDOW_OBJECT)),
	rowHeader(ZIL_NULLP(UI_WINDOW_OBJECT))
{
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TABLE::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_TABLE::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
	file->Load(&tblFlags);
	ZIL_INT16 value;
	file->Load(&value); columns = value;
	file->Load(&value); recordSize = value;
	records = 0;
	maxRecords = 0;
	data = ZIL_NULLP(void);
}
#endif

#if defined(ZIL_STORE)
void UIW_TABLE::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
	file->Store(tblFlags);
	ZIL_INT16 value = columns; file->Store(value);
	value = recordSize; file->Store(value);
}
#endif
