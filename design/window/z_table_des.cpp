//	Zinc Interface Library - Z_TABLE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_TABLE
#define USE_UIW_TABLE_RECORD
#define USE_UIW_TABLE_HEADER
#include "p_window.hpp"

// ----- UIW_TABLE ----------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_TABLE_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TABLE *object = (UIW_TABLE *)data;

	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_VIEW_ONLY | WOF_NON_SELECTABLE | WOF_NO_ALLOCATE_DATA;
	woAdvancedFlagsAllowed = WOAF_NON_CURRENT;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	SetFlag(FLAG_GRID, object->tblFlags, TBLF_GRID);
	SetValue(FIELD_COLUMNS, object->columns);
	SetValue(FIELD_REC_SIZE, object->recordSize);

	// Get the support objects.
	for (UI_WINDOW_OBJECT *field = (UI_WINDOW_OBJECT *)object->support.First(); field; field = field->Next())
	{
		SBF_FLAGS sbFlags = SBF_NO_FLAGS;
		THF_FLAGS thFlags = THF_NO_FLAGS;
		if (field->Inherited(ID_SCROLL_BAR) && field->Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR))
		{
			if (FlagSet(sbFlags, SBF_VERTICAL))
				notebook->Get(FLAG_VT_SCROLL)->woStatus |= WOS_SELECTED;
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				notebook->Get(FLAG_HZ_SCROLL)->woStatus |= WOS_SELECTED;
		}
		else if (field->Inherited(ID_TABLE_HEADER) && field->Information(I_GET_FLAGS, &thFlags, ID_TABLE_HEADER))
		{
			if (FlagSet(thFlags, THF_COLUMN_HEADER))
				notebook->Get(FLAG_COL_HEADER)->woStatus |= WOS_SELECTED;
			else if (FlagSet(thFlags, THF_ROW_HEADER))
				notebook->Get(FLAG_ROW_HEADER)->woStatus |= WOS_SELECTED;
		}
	}
}

int ZAF_OBJECT_EDITOR::UIW_TABLE_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TABLE *object = (UIW_TABLE *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);
	changed |= GetFlag(FLAG_GRID, &object->tblFlags, TBLF_GRID);
	changed |= GetValue(FIELD_COLUMNS, &object->columns);
	if (!object->columns)
		object->columns = 1; // minimum value is 1.
	changed |= GetValue(FIELD_REC_SIZE, &object->recordSize);

	// Set the support objects.
	UI_WINDOW_OBJECT *vScroll, *hScroll, *cScroll, *corHeader, *colHeader, *rowHeader, *vRecord;
	vScroll = hScroll = cScroll = corHeader = colHeader = rowHeader = vRecord = ZIL_NULLP(UI_WINDOW_OBJECT);
	UI_WINDOW_OBJECT *field = NULL;
	for (field = (UI_WINDOW_OBJECT *)object->support.First(); field; field = field->Next())
	{
		SBF_FLAGS sbFlags = SBF_NO_FLAGS;
		THF_FLAGS thFlags = THF_NO_FLAGS;
		if (field->Inherited(ID_SCROLL_BAR) && field->Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR))
		{
			if (FlagSet(sbFlags, SBF_VERTICAL))
				vScroll = field;
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				hScroll = field;
			else if (FlagSet(sbFlags, SBF_CORNER))
				cScroll = field;
		}
		else if (field->Inherited(ID_TABLE_HEADER) && field->Information(I_GET_FLAGS, &thFlags, ID_TABLE_HEADER))
		{
			if (FlagSet(thFlags, THF_COLUMN_HEADER))
				colHeader = field;
			else if (FlagSet(thFlags, THF_ROW_HEADER))
				rowHeader = field;
			else if (FlagSet(thFlags, THF_CORNER_HEADER))
				corHeader = field;
		}
	}
	for (field = object->First(); field && !vRecord; field = field->Next())
		if (field->Inherited(ID_TABLE_RECORD))
			vRecord = field;

	// Subtract the support objects.
	UI_EVENT addEvent(S_ADD_OBJECT);
#if defined(ZIL_MOTIF)
	object->Event(UI_EVENT(S_DEINITIALIZE));
#endif
	*object - corHeader - colHeader - rowHeader - cScroll - vScroll - hScroll;
	object->columnHeader = object->rowHeader = ZIL_NULLP(UI_WINDOW_OBJECT);

	// Check scroll options.
	if (!FlagSet(notebook->Get(FLAG_VT_SCROLL)->woStatus, WOS_SELECTED))
	{
		if (vScroll)
			changed |= TRUE, delete vScroll;
		vScroll = ZIL_NULLP(UIW_SCROLL_BAR);
	}
	else if (!vScroll)
		changed |= TRUE, vScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL);
	if (!FlagSet(notebook->Get(FLAG_HZ_SCROLL)->woStatus, WOS_SELECTED))
	{
		if (hScroll)
			changed |= TRUE, delete hScroll;
		hScroll = ZIL_NULLP(UIW_SCROLL_BAR);
	}
	else if (!hScroll)
		changed |= TRUE, hScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL);
	if (!vScroll || !hScroll)
	{
		if (cScroll)
			changed |= TRUE, delete cScroll;
		cScroll = ZIL_NULLP(UIW_SCROLL_BAR);
	}
	else if (!cScroll) // Match up the corner scroll.
		changed |= TRUE, cScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER);

	// Check table header options.
	static ZIL_ICHAR _title[] = { 'x','x','x',0 };
	if (!FlagSet(notebook->Get(FLAG_COL_HEADER)->woStatus, WOS_SELECTED))
	{
		if (colHeader)
			changed |= TRUE, delete colHeader;
		colHeader = ZIL_NULLP(UIW_TABLE_HEADER);
	}
	else if (!colHeader)
	{
		changed |= TRUE;
		colHeader =
			&(*new ZAF_TABLE_HEADER(THF_COLUMN_HEADER)
				+ &(*new ZAF_TABLE_RECORD(20, 1)
					+ new ZAF_PROMPT(0, 0, _title)));
	}
	if (!FlagSet(notebook->Get(FLAG_ROW_HEADER)->woStatus, WOS_SELECTED))
	{
		if (rowHeader)
			changed |= TRUE, delete rowHeader;
		rowHeader = ZIL_NULLP(UIW_TABLE_HEADER);
	}
	else if (!rowHeader)
	{
		changed |= TRUE;
		rowHeader =
			&(*new ZAF_TABLE_HEADER(THF_ROW_HEADER)
				+ &(*new ZAF_TABLE_RECORD(4, 1)
					+ new ZAF_PROMPT(0, 0, _title)));
	}
	if (!colHeader || !rowHeader)
	{
		if (corHeader)
			changed |= TRUE, delete corHeader;
		corHeader = ZIL_NULLP(UIW_TABLE_HEADER);
	}
	else if (!corHeader) // Match up the corner header.
	{
		changed |= TRUE;
		corHeader =
			&(*new ZAF_TABLE_HEADER(THF_CORNER_HEADER)
				+ new ZAF_TABLE_RECORD(0, 0));
	}

	// Add the requested support objects.
	if (FlagSet(object->tblFlags, TBLF_GRID))
	{
		if (colHeader)
		{
			((UIW_TABLE_HEADER *)colHeader)->thFlags |= THF_GRID;
			((UIW_TABLE_HEADER *)colHeader)->tblFlags |= TBLF_GRID;
		}
		if (rowHeader)
		{
			((UIW_TABLE_HEADER *)rowHeader)->thFlags |= THF_GRID;
			((UIW_TABLE_HEADER *)rowHeader)->tblFlags |= TBLF_GRID;
		}
	}
	else
	{
		if (colHeader)
		{
			((UIW_TABLE_HEADER *)colHeader)->thFlags &= ~THF_GRID;
			((UIW_TABLE_HEADER *)colHeader)->tblFlags &= ~TBLF_GRID;
		}
		if (rowHeader)
		{
			((UIW_TABLE_HEADER *)rowHeader)->thFlags &= ~THF_GRID;
			((UIW_TABLE_HEADER *)rowHeader)->tblFlags &= ~TBLF_GRID;
		}
	}

#if defined(ZIL_MOTIF)
	object->Event(UI_EVENT(S_CREATE));
#endif
	*object + corHeader + colHeader + rowHeader + cScroll + vScroll + hScroll;

	// Return the status.
//	return (changed);  // Temporary Fix.  Table must be regenerated because
	return (TRUE);	   // headers and scroll bars were subtracted and added.
}

// ----- UIW_TABLE_HEADER ---------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_TABLE_HEADER_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TABLE_HEADER *object = (UIW_TABLE_HEADER *)data;

	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_CALLBACK;
	woFlagsAllowed = WOF_PIXEL | WOF_MINICELL;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_HSTRINGID, object->stringID);
}

int ZAF_OBJECT_EDITOR::UIW_TABLE_HEADER_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TABLE_HEADER *object = (UIW_TABLE_HEADER *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_HSTRINGID, object->stringID);

	// Return the status.
	return (changed);
}

// ----- UIW_TABLE_RECORD ---------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_TABLE_RECORD_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TABLE_RECORD *object = (UIW_TABLE_RECORD *)data;

	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_CALLBACK;
	woFlagsAllowed = WOF_PIXEL | WOF_MINICELL;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_RSTRINGID, object->stringID);
	// Load the object flags.
	SetFlag(FLAG_AUTO_SELECT, object->wnFlags, WNF_AUTO_SELECT);
	SetFlag(FLAG_SELECT_MULTIPLE, object->wnFlags, WNF_SELECT_MULTIPLE);
	SetFlag(FLAG_NO_WRAP, object->wnFlags, WNF_NO_WRAP);
}

int ZAF_OBJECT_EDITOR::UIW_TABLE_RECORD_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_TABLE_RECORD *object = (UIW_TABLE_RECORD *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_RSTRINGID, object->stringID);
	// Store the object flags.
	changed |= GetFlag(FLAG_AUTO_SELECT, &object->wnFlags, WNF_AUTO_SELECT);
	changed |= GetFlag(FLAG_SELECT_MULTIPLE, &object->wnFlags, WNF_SELECT_MULTIPLE);
	changed |= GetFlag(FLAG_NO_WRAP, &object->wnFlags, WNF_NO_WRAP);

	// Return the status.
	return (changed);
}

// ----- ZAF_TABLE ----------------------------------------------------------

static ZIL_OBJECTID _idTable[] =
{
	ID_TABLE_HEADER, ID_TABLE_RECORD, // special table
	ID_END // end
};

ZAF_TABLE::ZAF_TABLE(int left, int top, int width, int height, int _columns,
	int _recordSize, int _maxRecords, void *_data, int _records,
	TBLF_FLAGS _tblFlags, WOF_FLAGS _woFlags) :
	UIW_TABLE(left, top, width, height, _columns, _recordSize, _maxRecords, _data, _records, _tblFlags, _woFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_TABLE::ZAF_TABLE(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TABLE(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_TABLE::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idTable[i] != ID_END; i++)
			if (_idTable[i] == objectID)
			{
				ccode = UIW_TABLE::Event(event);
				Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_INITIALIZE)
	{
		// Set up for the virtual table.
		UI_ITEM *saveObjectTable = UI_WINDOW_OBJECT::objectTable;
		UI_ITEM *saveUserTable = UI_WINDOW_OBJECT::userTable;
		UI_WINDOW_OBJECT::objectTable = ZAF_WINDOW::_objectTable;
		UI_WINDOW_OBJECT::userTable = ZAF_WINDOW::_userTable;
		ccode = UIW_TABLE::Event(event);
		UI_WINDOW_OBJECT::objectTable = saveObjectTable;
		UI_WINDOW_OBJECT::userTable = saveUserTable;
	}
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idTable, event);
	else
		ccode = UIW_TABLE::Event(event);
	return (ccode);
}

void *ZAF_TABLE::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idTable;
	else
		data = UIW_TABLE::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_TABLE::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	UIW_TABLE *table;
	if (ZAF_SERVICE_MANAGER::_testMode)
	{
		table = new UIW_TABLE(name, file, object, objectTable, userTable);
		table->DataSet(ZIL_NULLP(void), 10);
		return (table);
	}

	// -- edit mode constructors ---
	if (file)
		table = new ZAF_TABLE(name, file, object, objectTable, userTable);
	else
	{
		ZIL_ICHAR _text1[32];
		NormalizeString(_text1, "xxx");
		table = new ZAF_TABLE(0, 0, 30, 5, 1, 0, 1, ZIL_NULLP(void), 0);
		*table
			+ &(*new ZAF_TABLE_HEADER(THF_CORNER_HEADER)
				+ new ZAF_TABLE_RECORD(0, 0))
			+ &(*new ZAF_TABLE_HEADER(THF_COLUMN_HEADER)
				+ &(*new ZAF_TABLE_RECORD(20, 1)
					+ new ZAF_PROMPT(0, 0, _text1)))
			+ &(*new ZAF_TABLE_HEADER(THF_ROW_HEADER)
				+ &(*new ZAF_TABLE_RECORD(4, 1)
					+ new ZAF_PROMPT(0, 0, _text1)))
			+ new ZAF_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER)
			+ new ZAF_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL)
			+ new ZAF_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL)
			+ new ZAF_TABLE_RECORD(20, 1);
	}
	return (table);
}

// ----- ZAF_TABLE_HEADER ---------------------------------------------------

static ZIL_OBJECTID _idHTable[] =
{
	ID_END // end
};

ZAF_TABLE_HEADER::ZAF_TABLE_HEADER(THF_FLAGS _thFlags, int _recordSize,
	int _maxRecords, void *_data, WOF_FLAGS _woFlags) :
	UIW_TABLE_HEADER(_thFlags, _recordSize, _maxRecords, _data, _woFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_TABLE_HEADER::ZAF_TABLE_HEADER(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TABLE_HEADER(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_TABLE_HEADER::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idHTable[i] != ID_END; i++)
			if (_idHTable[i] == objectID)
			{
				ccode = UIW_TABLE_HEADER::Event(event);
				Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_INITIALIZE)
	{
		// Set up for the virtual table.
		UI_ITEM *saveObjectTable = UI_WINDOW_OBJECT::objectTable;
		UI_ITEM *saveUserTable = UI_WINDOW_OBJECT::userTable;
		UI_WINDOW_OBJECT::objectTable = ZAF_WINDOW::_objectTable;
		UI_WINDOW_OBJECT::userTable = ZAF_WINDOW::_userTable;
		ccode = UIW_TABLE_HEADER::Event(event);
		UI_WINDOW_OBJECT::objectTable = saveObjectTable;
		UI_WINDOW_OBJECT::userTable = saveUserTable;
	}
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idHTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idHTable, event);
	else
		ccode = UIW_TABLE_HEADER::Event(event);
	return (ccode);
}

void *ZAF_TABLE_HEADER::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idHTable;
	else
		data = UIW_TABLE_HEADER::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_TABLE_HEADER::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_TABLE_HEADER(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_TABLE_HEADER *tHeader;
	if (file)
		tHeader = new ZAF_TABLE_HEADER(name, file, object, objectTable, userTable);
	else
		tHeader = new UIW_TABLE_HEADER(THF_COLUMN_HEADER);
	return (tHeader);
}

// ----- ZAF_TABLE_RECORD ---------------------------------------------------

static ZIL_OBJECTID _idRTable[] =
{
	ID_STRING, ID_FORMATTED_STRING, ID_TEXT, ID_DATE, ID_TIME, ID_BIGNUM, ID_INTEGER, ID_REAL, // input
	ID_BUTTON, ID_RADIO_BUTTON, ID_CHECK_BOX, ID_HZ_SCROLL, ID_VT_SCROLL, ID_COMBO_BOX, ID_SPIN_CONTROL, // control
	ID_PROMPT, ID_ICON, // static
	ID_END // end
};

ZAF_TABLE_RECORD::ZAF_TABLE_RECORD(int width, int height,
	ZIL_USER_FUNCTION _userFunction, WOF_FLAGS _woFlags) :
	UIW_TABLE_RECORD(width, height, _userFunction, _woFlags)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

ZAF_TABLE_RECORD::ZAF_TABLE_RECORD(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_TABLE_RECORD(name, file, object, objectTable, userTable)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
	woAdvancedFlags |= WOAF_ACCEPTS_DROP;
}

EVENT_TYPE ZAF_TABLE_RECORD::Event(const UI_EVENT &event)
{
	// Check for special edit requests.
	EVENT_TYPE ccode = event.type;
	if (ccode == S_ADD_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
	{
		ZIL_OBJECTID objectID = event.windowObject->SearchID();
		for (int i = 0; _idRTable[i] != ID_END; i++)
			if (_idRTable[i] == objectID)
			{
				ccode = UIW_TABLE_RECORD::Event(event);
				UIW_TABLE_RECORD::Event(UI_EVENT(S_REDISPLAY));
				return (ccode);
			}
		ccode = S_ERROR;
	}
	else if (ccode == S_SUBTRACT_OBJECT &&
		support.Index(event.windowObject) == -1 &&
		UIW_WINDOW::Index(event.windowObject) == -1)
		ccode = S_ERROR;
	else if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrag(this, _idRTable, event);
	else if (ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
		ccode = ZAF_OBJECT_EDITOR::ObjectDrop(this, _idRTable, event);
	else
		ccode = UIW_TABLE_RECORD::Event(event);
	return (ccode);
}

void *ZAF_TABLE_RECORD::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	if (request == I_GET_ADDTABLE)
		*(ZIL_OBJECTID **)data = _idRTable;
	else
		data = UIW_TABLE_RECORD::Information(request, data, objectID);
	return (data);
}

UI_WINDOW_OBJECT *ZAF_TABLE_RECORD::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// --- test mode construction ---
	if (ZAF_SERVICE_MANAGER::_testMode)
		return (new UIW_TABLE_RECORD(name, file, object, objectTable, userTable));

	// -- edit mode constructors ---
	UIW_TABLE_RECORD *tRecord;
	if (file)
		tRecord = new ZAF_TABLE_RECORD(name, file, object, objectTable, userTable);
	else
		tRecord = new UIW_TABLE_RECORD(4, 1);
	return (tRecord);
}

