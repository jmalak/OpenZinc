//	Zinc Interface Library - OBJECT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_OBJ_SUBOBJECT
#include "p_window.hpp"

extern ZIL_ICHAR *_bitmapDirectory;
extern ZIL_ICHAR *_iconDirectory;
extern ZIL_ICHAR *_helpDirectory;
static ZIL_ICHAR _format[] = { '%','s',' ','-',' ','%','s',0 };

const int MAX_STATIC_LEVELS = 2;

// --------------------------------------------------------------------------
// ----- ZAF_OBJECT_EDITOR --------------------------------------------------
// --------------------------------------------------------------------------

ZAF_OBJECT_EDITOR::ZAF_OBJECT_EDITOR(ZIL_ICHAR *name, UI_WINDOW_OBJECT *_editObject) :
	// Initialize the base class.
	UIW_WINDOW(_className, ZAF_WINDOW_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), ZAF_WINDOW_EDITOR::_objectTable,
	ZAF_WINDOW_EDITOR::_userTable),
	// Initialize the class members.
	DataLoad(0), DataStore(0), subObjectID(0),
	editObject(_editObject), dirObject(_editObject),
	nameTable(ZAF_WINDOW_EDITOR::_nameTableName, ZAF_WINDOW_EDITOR::_storage),
	msgTable(_serviceManager->ZMSG_msgTableName(), ZAF_WINDOW_EDITOR::_storage)
{
	// Determine the window position.
	searchID = ID_OBJECT_EDITOR;
	windowManager->Center(this);
	relative.top += _editLevel * display->cellHeight;
	relative.bottom += _editLevel * display->cellHeight;

	InitializeStaticMembers(); // Load the static windows
	int offset = _editLevel % MAX_STATIC_LEVELS;
	_editLevel++; // increment the level count.

	// Reset the title.
	int i, j;
	ZIL_ICHAR title[64];
	sprintf(title, _format, name, editObject->StringID());
	Information(I_SET_TEXT, title);

	// Set the pointers to member functions to later load the data.
	ZIL_OBJECTID objectID = editObject->SearchID();

	// Check for derived objects.
	UI_ITEM *table = ZAF_WINDOW::_objectTable;
	if (objectID >= ID_DERIVED_START)
	{
		for (i = 0; table[i].value != ID_END; i++)
			if (table[i].value == objectID)
				break;
		for (j = 0; table[j].value != ID_END; j++)
			if (table[i].data == table[j].data)
				break;
		objectID = (ZIL_OBJECTID)table[j].value; // Reset value & name to the base class.
		name = table[j].text;
	}

	// Find a matching edit object.
	for (i = 0; _classTable[i].objectID != ID_END; i++)
		if (_classTable[i].objectID == objectID)
		{
			DataLoad = _classTable[i].Load;
			DataStore = _classTable[i].Store;
			subObjectID = _classTable[i].subObjectID;
			break;
		}

	// ----- Create the notebook and add the controls -----
	int page = 0;
	ZIL_STORAGE_READ_ONLY *storage = ZAF_WINDOW_EDITOR::_storage;
	ZIL_STORAGE_OBJECT_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	UI_ITEM *objectTable = ZAF_WINDOW_EDITOR::_objectTable;
	UI_ITEM *userTable = ZAF_WINDOW_EDITOR::_userTable;

	notebook = new UIW_NOTEBOOK;
	Add(notebook);

	// --- Level 1: Object specific information.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_NO_FLAGS;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;
	genWindow = new UIW_WINDOW(name, storage, file, objectTable, userTable);
	*notebook + genWindow;
	notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
	(this->*DataLoad)(editObject);

	// --- Level 2: Sub-object information.
	if (subObjectID)
	{
		//subWindow = _subWindow[offset];
		//dirList = _dirList[offset];
		//objList = _objList[offset];
		//subList = _subList[offset];
		
//code above causes segmentation fault when switching to subobject
//window under Motif. Code segment below by jdh avoids this 		
		
		subWindow = new UIW_WINDOW("OBJ_SUBOBJECT", storage, file, objectTable, userTable);
		dirList = (UIW_VT_LIST *)subWindow->Get("LIST_DIRECTORIES");
		objList = (UIW_VT_LIST *)subWindow->Get("LIST_OBJECTS");
		subList = (UIW_COMBO_BOX *)subWindow->Get("FIELD_ADD_OBJECT");
		
		subWindow->woStatus &= ~WOS_CHANGED;
		
		*notebook + subWindow;
		notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
		OBJ_SUBOBJECT_DataLoad(editObject);

		// Load the sub-objects in the list.
		if (!subList->First())
		{
			ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
			defaultStorage = ZAF_WINDOW_EDITOR::_storage;
			for (int i = 0; _classTable[i].objectID != ID_END; i++)
				if (_classTable[i].New)
				{
					name = nameTable.GetMessage(_classTable[i].objectID);
					UIW_BUTTON *button = new UIW_BUTTON(0, 0, 20, name,
						BTF_NO_3D, WOF_NO_FLAGS, ZIL_NULLF(ZIL_USER_FUNCTION),
						0, _classTable[i].className);
					button->StringID(_classTable[i].className);
					*subList + button;
				}
			defaultStorage = saveStorage;
		}
		subList->Information(I_SET_TEXT, nameTable.GetMessage(subObjectID));
	}
	else
	{
		subWindow = ZIL_NULLP(UIW_WINDOW);
		dirList = ZIL_NULLP(UIW_VT_LIST);
		objList = ZIL_NULLP(UIW_VT_LIST);
		subList = ZIL_NULLP(UIW_COMBO_BOX);
	}

	// --- Level 3: Position information.
	posWindow = _posWindow[offset];
	posWindow->woStatus &= ~WOS_CHANGED;
	if (editObject->Inherited(ID_GEOMETRY_MANAGER) ||
		editObject->Inherited(ID_MENU_ITEM))
		posWindow->woFlags |= WOF_NON_SELECTABLE;
	else
		posWindow->woFlags &= ~WOF_NON_SELECTABLE;
	*notebook + posWindow;
	notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
	OBJ_POSITION_DataLoad(editObject);

	// --- Level 4: Geometry management.
	if (editObject->parent)
	{
		geoWindow = _geoWindow[offset];
		geoWindow->woStatus &= ~WOS_CHANGED;

		// Check for valid flags.
		if (!FlagSet(dsFlagsAllowed, DSF_GEOMETRY) ||
			FlagSet(editObject->woFlags, WOF_NON_FIELD_REGION) ||
			editObject->Inherited(ID_GEOMETRY_MANAGER) ||
			editObject->Inherited(ID_MENU) ||
			editObject->Inherited(ID_MENU_ITEM) ||
			editObject->Inherited(ID_LIST_ITEM))
			geoWindow->woFlags |= WOF_NON_SELECTABLE;
		else
			geoWindow->woFlags &= ~WOF_NON_SELECTABLE;
		*notebook + geoWindow;
		notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
		geoWindow->DataLoad(editObject);
	}
	else
		geoWindow = ZIL_NULLP(ZAF_GEOMETRY_MANAGER);

	// --- Level 5: Advanced information.
	advWindow = _advWindow[offset];
	advWindow->woStatus &= ~WOS_CHANGED;
	*notebook + advWindow;
	notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
	OBJ_ADVANCED_DataLoad(editObject);
}

ZAF_OBJECT_EDITOR::~ZAF_OBJECT_EDITOR(void)
{
	_editLevel--; // decrement the level count.
	int offset = _editLevel % 2;

	// Restore the static windows.
	if (_subWindow[offset] == subWindow)
		*notebook - subWindow;
	if (_posWindow[offset] == posWindow)
		*notebook - posWindow;
	if (_geoWindow[offset] == geoWindow)
		*notebook - geoWindow;
	if (_advWindow[offset] == advWindow)
		*notebook - advWindow;
}

EVENT_TYPE ZAF_OBJECT_EDITOR::CheckCoordinates(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	// Check for selection of the object.
	if (ccode != L_SELECT)
		return (0);
	UI_WINDOW_OBJECT *sibling;
	object->woStatus |= WOS_SELECTED;

	// Objects in the position section are grouped according to cell,
	// mini-cell, and pixel coordinates.  This function sets/clears the
	// selectability of items (buttons, integers) in a related group.
	// Beginning/Ending of group is delimited by a non-integer field.

	// Mark all non related objects (previous) as non-selectable.
	for (sibling = object->Previous(); sibling; sibling = sibling->Previous())
		if (sibling->Inherited(ID_INTEGER) && !FlagSet(sibling->woFlags, WOF_NON_SELECTABLE))
		{
			// Mark the sibling as non selectable.
			sibling->woFlags |= WOF_NON_SELECTABLE;
			sibling->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
			sibling->Event(S_REDISPLAY);
		}
		else if (FlagSet(sibling->woStatus, WOS_SELECTED))
		{
			// Mark the sibling as not selected.
			sibling->woStatus &= ~WOS_SELECTED;
			sibling->Information(I_CHANGED_STATUS, ZIL_NULLP(void));
			sibling->Event(S_REDISPLAY);
		}

	// Allow edit of associated objects.
	for (sibling = object->Next(); sibling && sibling->Inherited(ID_INTEGER); sibling = sibling->Next())
		if (FlagSet(sibling->woFlags, WOF_NON_SELECTABLE))
		{
			// Mark all related siblings as selectable.
			sibling->woFlags &= ~WOF_NON_SELECTABLE;
			sibling->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
			sibling->Event(S_REDISPLAY);
		}

	// Mark all remaining related objects (next) as non-selectable.
	for ( ; sibling; sibling = sibling->Next())
		if (sibling->Inherited(ID_INTEGER) && !FlagSet(sibling->woFlags, WOF_NON_SELECTABLE))
		{
			// Mark the sibling as non selectable.
			sibling->woFlags |= WOF_NON_SELECTABLE;
			sibling->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
			sibling->Event(S_REDISPLAY);
		}
		else if (FlagSet(sibling->woStatus, WOS_SELECTED))
		{
			// Mark the sibling as not selected.
			sibling->woStatus &= ~WOS_SELECTED;
			sibling->Information(I_CHANGED_STATUS, ZIL_NULLP(void));
			sibling->Event(S_REDISPLAY);
		}

	// Return.
	return (0);
}

EVENT_TYPE ZAF_OBJECT_EDITOR::CheckFlagSelection(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	// Check for selection of the object.
	if (ccode != L_SELECT)
		return (0);
	UI_WINDOW_OBJECT *sibling;
	ZIL_OBJECTID searchID = object->SearchID();
	object->woStatus |= WOS_SELECTED;

	// Objects in this section are grouped according to their
	// selectability.  Radio-buttons specify this callback function
	// to turn on/off the selectability of a particular selection group.
	// Beginning/Ending of group is delimited by a non-radio-button field.

	// Mark all previous radio-buttons as non-selectable.
	for (sibling = object->Previous(); sibling && searchID == sibling->SearchID(); sibling = sibling->Previous())
		if (FlagSet(sibling->woStatus, WOS_SELECTED))
		{
			sibling->woStatus &= ~WOS_SELECTED;
			sibling->Information(I_CHANGED_STATUS, ZIL_NULLP(void));
			sibling->Event(S_REDISPLAY);
			sibling->woStatus |= WOS_CHANGED;
		}

	// Mark all remaining radio-buttons as non-selectable.
	for (sibling = object->Next(); sibling && searchID == sibling->SearchID(); sibling = sibling->Next())
		if (FlagSet(sibling->woStatus, WOS_SELECTED))
		{
			sibling->woStatus &= ~WOS_SELECTED;
			sibling->Information(I_CHANGED_STATUS, ZIL_NULLP(void));
			sibling->Event(S_REDISPLAY);
			sibling->woStatus |= WOS_CHANGED;
		}

	// Return.
	return (0);
}

void *ZAF_OBJECT_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_SET_SEARCH_PAGE:
	case I_GET_NUMBERID_OBJECT:
	case I_GET_STRINGID_OBJECT:
		data = notebook->Information(request, data, objectID);
		break;

	case I_GET_GEOMETRY:
		*(ZAF_GEOMETRY_MANAGER **)data = geoWindow;
		break;

	case I_GET_EDIT_OBJECT:
		*(UI_WINDOW_OBJECT **)data = editObject;
		break;

	case I_CHECK_EDIT_NAMES:
		{
		UI_WINDOW_OBJECT *match, *object;
		object = (UI_WINDOW_OBJECT *)data;
		match = dirList ? dirList->First() : ZIL_NULLP(UI_WINDOW_OBJECT);
		for ( ; match; match = match->Next())
			if (match->userObject == object)
				match->Information(I_SET_TEXT, object->StringID());
		match = objList ? objList->First() : ZIL_NULLP(UI_WINDOW_OBJECT);
		for ( ; match; match = match->Next())
			if (match->userObject == object)
				match->Event(S_REDISPLAY);
		}
		break;

	case I_CHECK_EDIT_DELETE:
		{
		UI_WINDOW_OBJECT *match, *object;
		object = (UI_WINDOW_OBJECT *)data;
		match = dirList ? dirList->First() : ZIL_NULLP(UI_WINDOW_OBJECT);
		while (match)
		{
			UI_WINDOW_OBJECT *temp = match->Next();
			if (match->userObject == object)
			{
				dirList->Subtract(match);
				dirList->Event(S_REDISPLAY);
				delete match;
				break;
			}
			match = temp;
		}
		match = objList ? objList->First() : ZIL_NULLP(UI_WINDOW_OBJECT);
		while (match)
		{
			UI_WINDOW_OBJECT *temp = match->Next();
			if (match->userObject == object)
			{
				objList->Subtract(match);
				objList->Event(S_REDISPLAY);
				delete match;
				break;
			}
			match = temp;
		}
		}
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int ZAF_OBJECT_EDITOR::InitializeStaticMembers(void)
{
	ZIL_ICHAR windowName[32];
	ZIL_STORAGE_READ_ONLY *storage = ZAF_WINDOW_EDITOR::_storage;
	ZIL_STORAGE_OBJECT_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	UI_ITEM *objectTable = ZAF_WINDOW_EDITOR::_objectTable;
	UI_ITEM *userTable = ZAF_WINDOW_EDITOR::_userTable;

	// This function creates an additional layer of notebook pages, based
	// on the offset.  In addition, a new level is automatically created 
	// if the _editLevel is above, or equal to MAX_STATIC_LEVELS and the
	// object is currently in use (signified by the screenID).
	int offset = _editLevel % MAX_STATIC_LEVELS;

	// sub-window page.
	if (!_subWindow[offset] ||
		(_editLevel >= MAX_STATIC_LEVELS && _subWindow[offset]->screenID))
	{
		NormalizeString(windowName, "OBJ_SUBOBJECT");
		_subWindow[offset] = new UIW_WINDOW(windowName, storage, file, objectTable, userTable);
		_dirList[offset] = (UIW_VT_LIST *)_subWindow[offset]->Get(LIST_DIRECTORIES);
		_objList[offset] = _serviceManager->CtrlList(_subWindow[offset], LIST_OBJECTS);
		_subList[offset] = (UIW_COMBO_BOX *)_subWindow[offset]->Get(FIELD_ADD_OBJECT);
	}

	// position page.
	if (!_posWindow[offset] ||
		(_editLevel >= MAX_STATIC_LEVELS && _posWindow[offset]->screenID))
	{
		NormalizeString(windowName, "OBJ_POSITION");
		_posWindow[offset] = new UIW_WINDOW(windowName, storage, file, objectTable, userTable);
	}

	// geometry management page.
	if (!_geoWindow[offset] ||
		(_editLevel >= MAX_STATIC_LEVELS && _geoWindow[offset]->screenID))
		_geoWindow[offset] = new ZAF_GEOMETRY_MANAGER;

	// advanced options page.
	if (!_advWindow[offset] ||
		(_editLevel >= MAX_STATIC_LEVELS && _advWindow[offset]->screenID))
	{
		NormalizeString(windowName, "OBJ_ADVANCED");
		_advWindow[offset] = new UIW_WINDOW(windowName, storage, file, objectTable, userTable);
	}

	return (TRUE);
}

void ZAF_OBJECT_EDITOR::NaturalCoordinate(UI_WINDOW_OBJECT *object,
	UI_REGION &region, WOF_FLAGS newFlags)
{
	// Prepare the coordinate.
	int cellWidth = display->cellWidth;
	int cellHeight = display->cellHeight;
	if (object->parent)
	{
		region = object->relative;
		if (FlagSet(object->woStatus, WOS_GRAPHICS) &&
			!FlagSet(object->woFlags, WOF_NON_FIELD_REGION) &&
			!FlagSet(newFlags, WOF_PIXEL) &&
			!FlagSet(newFlags, WOF_MINICELL))
		{
			region.top -= display->preSpace;
			region.bottom += display->postSpace;
		}
	}
	else
		region = object->trueRegion;

	// Convert to natural coordinates.
	if (!display->isText && FlagSet(object->woStatus, WOS_GRAPHICS) && FlagSet(newFlags, WOF_MINICELL))
	{
		long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
		long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

		int width = (int)(((miniDX * region.Width()) / cellWidth + miniNX / 2) / miniNX);
		region.left = (int)(((miniDX * region.left) / cellWidth + miniNX / 2) / miniNX);
		region.right = region.left + width - 1;
		int height = (int)(((miniDY * region.Height()) / cellHeight + miniNY / 2) / miniNY);
		region.bottom = (int)(((miniDY * region.bottom) / cellHeight + miniNY / 2) / miniNY);
		region.top = region.bottom - height + 1;
	}
	else if (!display->isText && FlagSet(object->woStatus, WOS_GRAPHICS) && !FlagSet(newFlags, WOF_PIXEL))
	{
#		if defined(ZIL_OS2)
		int width = MaxValue(region.Width() / cellWidth, 1);
		int height = MaxValue(region.Height() / cellHeight, 1);
#		else
		int width = region.Width() / cellWidth;
		int height = region.Height() / cellHeight;
#		endif
		region.left /= cellWidth;
		region.bottom++;
		region.bottom /= cellHeight;
		region.bottom--;
		region.right = region.left + width - 1;
		region.top = region.bottom - height + 1;
	}
}

int ZAF_OBJECT_EDITOR::RestoreStaticMembers(void)
{
	// Deallocate the static members.
	for (int i = 0; i < MAX_STATIC_LEVELS; i++)
	{
		if (_subWindow[i])
			delete _subWindow[i];
		_subWindow[i] = ZIL_NULLP(UIW_WINDOW);
		if (_posWindow[i])
			delete _posWindow[i];
		_posWindow[i] = ZIL_NULLP(UIW_WINDOW);
		if (_geoWindow[i])
			delete _geoWindow[i];
		_geoWindow[i] = ZIL_NULLP(ZAF_GEOMETRY_MANAGER);
		if (_advWindow[i])
			delete _advWindow[i];
		_advWindow[i] = ZIL_NULLP(UIW_WINDOW);
	}

	return (TRUE);
}

// ----- Get routines -------------------------------------------------------

int ZAF_OBJECT_EDITOR::GetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS *currentFlags, UIF_FLAGS flag)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field)
		return (FALSE);
	else if (!FlagSet(*currentFlags, flag) && FlagSet(field->woStatus, WOS_SELECTED))
		*currentFlags |= flag;
	else if (FlagSet(*currentFlags, flag) && !FlagSet(field->woStatus, WOS_SELECTED))
		*currentFlags &= ~flag;
	else
		return (FALSE);
	return (TRUE);
}

int ZAF_OBJECT_EDITOR::GetHelp(ZIL_NUMBERID fieldID, UI_HELP_CONTEXT *helpContext)
{
	// Check to see if the field has changed.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field || !FlagSet(field->woStatus, WOS_CHANGED))
		return (FALSE);
	field->woStatus &= ~WOS_CHANGED;

	// Make a request to the directory service.
	UI_WINDOW_OBJECT *windowEditor = ZIL_NULLP(UIW_WINDOW);
	windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
	if (!windowEditor)
		return (FALSE);
	ZIL_STORAGE_READ_ONLY *editFile = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
	if (!windowEditor->Information(I_GET_STORAGE, &editFile))
		return (FALSE);

	ZIL_ICHAR *helpContextName;
	field->Information(I_GET_TEXT, &helpContextName);
	editFile->ChDir(_helpDirectory);
	ZIL_STORAGE_OBJECT_READ_ONLY hObject(*editFile, helpContextName, 0);
	*helpContext = (UI_HELP_CONTEXT)hObject.objectID;
	return (TRUE);
}

int ZAF_OBJECT_EDITOR::GetImage(ZIL_NUMBERID fieldID, ZIL_ICHAR **name, ZIL_OBJECTID imageType)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field || !FlagSet(field->woStatus, WOS_CHANGED))
		return (FALSE);
	field->woStatus &= ~WOS_CHANGED;

	// Get the image name.
	ZIL_ICHAR *temp;
	field->Information(I_GET_TEXT, &temp);
	if (*name)
		delete *name;
	if (temp && stricmp(temp, _serviceManager->ZMSG_none()))
		*name = strdup(temp);
	else
	{
		*name = ZIL_NULLP(ZIL_ICHAR);
		int size = 0;
		editObject->Information(I_SET_BITMAP_ARRAY, ZIL_NULLP(void));
		editObject->Information(I_SET_BITMAP_WIDTH, &size);
		editObject->Information(I_SET_BITMAP_HEIGHT, &size);
		return (TRUE);
	}

	// Make a request to the directory service.
	UI_WINDOW_OBJECT *windowEditor = ZIL_NULLP(UIW_WINDOW);
	windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
	if (!windowEditor)
		return (TRUE);

	// Set the specified image.
	ZIL_STORAGE *editFile;
	if (!windowEditor->Information(I_GET_STORAGE, &editFile) || !editFile)
		return (TRUE);

	editFile->ChDir((imageType == ID_BITMAP_IMAGE) ? _bitmapDirectory : _iconDirectory);
	ZIL_STORAGE_OBJECT_READ_ONLY bFile(*editFile, *name, 0);
	if (!bFile.objectError)
	{
		ZIL_UINT16 value;
		bFile.Load(&value); int bitmapWidth = value;
		bFile.Load(&value); int bitmapHeight = value;
		ZIL_UINT8 *bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
		bFile.Load(bitmapArray, bitmapWidth, bitmapHeight);
		if (imageType == ID_BITMAP_IMAGE)
		{
			editObject->Information(I_SET_BITMAP_WIDTH, &bitmapWidth);
			editObject->Information(I_SET_BITMAP_HEIGHT, &bitmapHeight);
		}
		editObject->Information(I_SET_BITMAP_ARRAY, bitmapArray);
		delete bitmapArray;
	}
	return (TRUE);
}

int ZAF_OBJECT_EDITOR::GetScroll(ZIL_NUMBERID fieldID, UIW_WINDOW *window, SBF_FLAGS sbFlags)
{
	// Try to find the matching object.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field)
		return (FALSE);

	UI_WINDOW_OBJECT *object = NULL;
	for (object = (UI_WINDOW_OBJECT *)window->support.First();
		object; object = object->Next())
		if (object->Inherited(ID_SCROLL_BAR))
		{
			SBF_FLAGS flags = SBF_NO_FLAGS;
			object->Information(I_GET_FLAGS, &flags, ID_SCROLL_BAR);
			if (FlagSet(flags, sbFlags))
				break;
		}

	if (object && !FlagSet(field->woStatus, WOS_SELECTED))
	{
		// Object needs to be removed from the window.
		window->Subtract(object);
		delete object;
		return (TRUE); // Object deleted.
	}
	else if (!object && FlagSet(field->woStatus, WOS_SELECTED))
	{
		// Object needs to added to the window.
		object = new UIW_SCROLL_BAR(0, 0, 0, 0, sbFlags);
		window->Add(object);
		return (TRUE); // Object added.
	}
	return (FALSE); // Object is ok.
}

int ZAF_OBJECT_EDITOR::GetSupport(ZIL_NUMBERID fieldID, UIW_WINDOW *window, NUMBERID numberID)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field)
		return (FALSE);

	UI_WINDOW_OBJECT *object = window->Get(numberID);
	if (object && !FlagSet(field->woStatus, WOS_SELECTED))
	{
		// Object needs to be removed from the window.
		window->Subtract(object);
		delete object;
		return (TRUE); // Object deleted.
	}
	else if (!object && FlagSet(field->woStatus, WOS_SELECTED))
	{
		// Object needs to added to the window.
		if (numberID == NUMID_TITLE)
			window->Add(new ZAF_TITLE(_blankString));
		else if (numberID == NUMID_BORDER)
			window->Add(new ZAF_BORDER);
		else if (numberID == NUMID_MAXIMIZE)
			window->Add(new ZAF_MAXIMIZE_BUTTON);
		else if (numberID == NUMID_MINIMIZE)
			window->Add(new ZAF_MINIMIZE_BUTTON);
		else if (numberID == NUMID_SYSTEM)
			window->Add(new ZAF_SYSTEM_BUTTON(SYF_GENERIC));
		else if (numberID == NUMID_TITLE)
			window->Add(new ZAF_TITLE(_serviceManager->ZMSG_untitled()));
		else if (numberID == NUMID_C_SCROLL)
			window->Add(new ZAF_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER));
		else if (numberID == NUMID_VT_SCROLL)
			window->Add(new ZAF_SCROLL_BAR(0, 0, 0, 5, SBF_VERTICAL));
		else if (numberID == NUMID_HZ_SCROLL)
			window->Add(new ZAF_SCROLL_BAR(0, 0, 15, 0, SBF_HORIZONTAL));
		else if (numberID == NUMID_GEOMETRY)
			window->Add(new UI_GEOMETRY_MANAGER);
		else
			return (FALSE); // Object cannot be found.
		return (TRUE);
	}
	return (FALSE); // Object is ok.
}

int ZAF_OBJECT_EDITOR::GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field && FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		field->Information(I_COPY_TEXT, text);
		return (TRUE);
	}
	return (FALSE);
}

int ZAF_OBJECT_EDITOR::GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR **text)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field && FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		ZIL_ICHAR *temp;
		field->Information(I_GET_TEXT, &temp);
		if (*text)
			delete *text;
		*text = temp ? strdup(temp) : ZIL_NULLP(ZIL_ICHAR);
		return (TRUE);
	}
	return (FALSE);
}

int ZAF_OBJECT_EDITOR::GetValue(ZIL_NUMBERID fieldID, int *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field && FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		field->Information(I_GET_VALUE, value);
		return (TRUE);
	}
	return (FALSE);
}

int ZAF_OBJECT_EDITOR::GetValue(ZIL_NUMBERID fieldID, double *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field && FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		field->Information(I_GET_VALUE, value);
		return (TRUE);
	}
	return (FALSE);
}

int ZAF_OBJECT_EDITOR::GetValue(ZIL_NUMBERID fieldID, ZIL_BIGNUM *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field && FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		field->Information(I_GET_VALUE, value);
		return (TRUE);
	}
	return (FALSE);
}

int ZAF_OBJECT_EDITOR::GetValue(ZIL_NUMBERID fieldID, ZIL_DATE *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field && FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		field->Information(I_GET_VALUE, value);
		return (TRUE);
	}
	return (FALSE);
}

int ZAF_OBJECT_EDITOR::GetValue(ZIL_NUMBERID fieldID, ZIL_TIME *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field && FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		field->Information(I_GET_VALUE, value);
		return (TRUE);
	}
	return (FALSE);
}

// ----- Set routines -------------------------------------------------------

void ZAF_OBJECT_EDITOR::SetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS currentFlags, UIF_FLAGS flag)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field)
		;
	else if (FlagSet(currentFlags, flag) || (!currentFlags && !flag))
		field->woStatus |= WOS_SELECTED;
	else
		field->woStatus &= ~WOS_SELECTED;
}

void ZAF_OBJECT_EDITOR::SetHelp(ZIL_NUMBERID fieldID, UI_HELP_CONTEXT helpContext)
{
	// Make a request to the directory service.
	UI_WINDOW_OBJECT *windowEditor = ZIL_NULLP(UIW_WINDOW);
	windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
	if (!windowEditor)
		return;

	// Find the help field and set the default help context.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field)
		return;

	// Set the specified default help context.
	ZIL_STORAGE_READ_ONLY *editFile = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
	UI_EVENT event(S_ADD_OBJECT);
	event.windowObject = new UIW_BUTTON(0, 0, 50, _serviceManager->ZMSG_none(), BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS);
	field->Event(event);
	if (!windowEditor->Information(I_GET_STORAGE, &editFile))
		return;

	// Add all help context options.
	UI_HELP_CONTEXT index = NO_HELP_CONTEXT;
	if (editFile->ChDir(_helpDirectory))
		return;
	static ZIL_ICHAR _allFiles[] = { '*', 0 };
	for (ZIL_ICHAR *helpName = editFile->FindFirstObject(_allFiles); helpName; helpName = editFile->FindNextObject())
		if (helpName[0] != '.')
		{
			event.windowObject = new UIW_BUTTON(0, 0, 50, helpName,
				BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
				ZIL_NULLF(ZIL_USER_FUNCTION), index);
			field->Event(event);
		}

	// Set the current selection.
	if (helpContext)
		field->Information(I_SET_TEXT, editFile->FindFirstID(helpContext));
}

void ZAF_OBJECT_EDITOR::SetImage(ZIL_NUMBERID fieldID, ZIL_ICHAR *name, ZIL_OBJECTID imageType)
{
	// Make a request to the directory service.
	UI_WINDOW_OBJECT *windowEditor = ZIL_NULLP(UIW_WINDOW);
	windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
	if (!windowEditor)
		return;

	// Find the image field and set the default image.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (!field)
		return;
	UI_EVENT event(S_ADD_OBJECT);
	event.windowObject = new UIW_BUTTON(0, 0, 50, _serviceManager->ZMSG_none(), BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS);
	field->Event(event);

	// Set the specified image.
	ZIL_STORAGE *editFile;
	if (!windowEditor->Information(I_GET_STORAGE, &editFile) || !editFile)
		return;

	if (editFile->ChDir((imageType == ID_BITMAP_IMAGE) ? _bitmapDirectory : _iconDirectory))
		return;
	static ZIL_ICHAR _allFiles[] = { '*', 0 };
	ZIL_STORAGE_READ_ONLY *tempStorage = defaultStorage;
	defaultStorage = editFile;
	for (ZIL_ICHAR *imageName = editFile->FindFirstObject(_allFiles); imageName; imageName = editFile->FindNextObject())
		if (imageName[0] != '.')
		{
			if (imageType == ID_BITMAP_IMAGE)
				event.windowObject = new UIW_BUTTON(0, 0, 50, imageName,
					BTF_AUTO_SIZE | BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
					ZIL_NULLF(ZIL_USER_FUNCTION), 0, imageName);
			else
				event.windowObject = new UIW_ICON(0, 0, imageName,
					imageName, ICF_DOUBLE_CLICK, WOF_NO_FLAGS,
					ZIL_NULLF(ZIL_USER_FUNCTION));
			field->Event(event);
		}
	defaultStorage = tempStorage;

	// Set the current image.
	if (name)
		field->Information(I_SET_TEXT, name);
}

void ZAF_OBJECT_EDITOR::SetScroll(ZIL_NUMBERID fieldID, UIW_WINDOW *window, SBF_FLAGS sbFlags)
{
	// Try to find the specified scroll objects.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
	{
		field->woStatus &= ~WOS_SELECTED;
		for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)window->support.First();
			object; object = object->Next())
			if (object->Inherited(ID_SCROLL_BAR))
			{
				SBF_FLAGS flags = SBF_NO_FLAGS;
				object->Information(I_GET_FLAGS, &flags, ID_SCROLL_BAR);
				if (FlagSet(flags, sbFlags))
				{
					field->woStatus |= WOS_SELECTED; // Object found.
					break;
				}
			}
	}
}

void ZAF_OBJECT_EDITOR::SetSupport(ZIL_NUMBERID fieldID, UIW_WINDOW *window, NUMBERID numberID)
{
	// Try to find the specified support object.
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
	{
		field->woStatus &= ~WOS_SELECTED;
		if (window->Get(numberID))
			field->woStatus |= WOS_SELECTED; // object found.
		if (numberID == NUMID_TITLE && !window->parent)
			field->woFlags |= WOF_NON_SELECTABLE;
	}
}

void ZAF_OBJECT_EDITOR::SetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
		field->Information(I_SET_TEXT, text ? text : _blankString);
}

void ZAF_OBJECT_EDITOR::SetValue(ZIL_NUMBERID fieldID, int value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
		field->Information(I_SET_VALUE, &value);
}

void ZAF_OBJECT_EDITOR::SetValue(ZIL_NUMBERID fieldID, double value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
		field->Information(I_SET_VALUE, &value);
}

void ZAF_OBJECT_EDITOR::SetValue(ZIL_NUMBERID fieldID, ZIL_BIGNUM *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
		field->Information(I_SET_VALUE, value);
}

void ZAF_OBJECT_EDITOR::SetValue(ZIL_NUMBERID fieldID, ZIL_DATE *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
		field->Information(I_SET_VALUE, value);
}

void ZAF_OBJECT_EDITOR::SetValue(ZIL_NUMBERID fieldID, ZIL_TIME *value)
{
	UI_WINDOW_OBJECT *field = notebook->Get(fieldID);
	if (field)
		field->Information(I_SET_VALUE, value);
}

// ----- Drag & Drop --------------------------------------------------------

EVENT_TYPE ZAF_OBJECT_EDITOR::ObjectDrag(UI_WINDOW_OBJECT *, ZIL_OBJECTID *idTable, const UI_EVENT &)
{
	// Validate object dragging.
	ZIL_ICHAR *name = windowManager->dragObject->StringID();
	ZIL_OBJECTID objectID = 0;
	int i;
	for (i = 0; _classTable[i].objectID != ID_END; i++)
		if (!stricmp(ZAF_OBJECT_EDITOR::_classTable[i].className, name))
			objectID = _classTable[i].objectID;
	if (!objectID)
	{
		eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
		return (S_ERROR);
	}
	for (i = 0; idTable[i] != ID_END; i++)
		if (idTable[i] == objectID)
		{
			eventManager->DeviceImage(E_MOUSE, DM_DRAG_MOVE);
			return (0);
		}
	eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
	return (S_ERROR);
}

EVENT_TYPE ZAF_OBJECT_EDITOR::ObjectDrop(UI_WINDOW_OBJECT *editObject, ZIL_OBJECTID *idTable, const UI_EVENT &event)
{
	// Validate object dropping.
	ZIL_ICHAR *name = windowManager->dragObject->StringID();
	ZIL_OBJECTID objectID = 0;
	int i;
	for (i = 0; _classTable[i].objectID != ID_END; i++)
		if (!stricmp(ZAF_OBJECT_EDITOR::_classTable[i].className, name))
			objectID = _classTable[i].objectID;
	if (!objectID)
		return (S_ERROR);

	// Add the object to the edit window.
	for (i = 0; idTable[i] != ID_END; i++)
		if (idTable[i] == objectID)
		{
			// Set-up object placement.
			UI_WINDOW_OBJECT *object;
			UI_WINDOW_OBJECT *windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
			UI_EVENT addEvent(D_CREATE_OBJECT);
			addEvent.windowObject = windowManager->dragObject;
			windowEditor->Event(addEvent);
			windowEditor->Information(I_GET_CREATE_OBJECT, &object);

			// Compute the object position within its parent window.
			addEvent.type = S_ADD_OBJECT;
			TruetoRelative(editObject, event.position, addEvent.position);
			int width = object->relative.Width();
			int height = object->relative.Height();
			object->relative.left = addEvent.position.column / display->cellWidth;
			object->relative.top = addEvent.position.line / display->cellHeight;
			object->relative.right = object->relative.left + width - 1;
			object->relative.bottom = object->relative.top + height - 1;

			// Place the object.
			addEvent.windowObject = object;
			if (editObject->Event(addEvent) == S_ERROR)
			{
				delete object;
				return (S_ERROR);
			}

			// Update the edit status.
			addEvent.type = D_SET_OBJECT;
			object->Root()->Event(addEvent);
			return (event.type);
		}
	return (S_ERROR);
}

// ----- Static tables ------------------------------------------------------

int ZAF_OBJECT_EDITOR::_editLevel = 0;
ZIL_ICHAR ZAF_OBJECT_EDITOR::_className[] = { 'Z','A','F','_','O','B','J','E','C','T','_','E','D','I','T','O','R',0 };

UIW_WINDOW *ZAF_OBJECT_EDITOR::_subWindow[2] = { ZIL_NULLP(UIW_WINDOW), ZIL_NULLP(UIW_WINDOW) };
UIW_VT_LIST *ZAF_OBJECT_EDITOR::_dirList[2] = { ZIL_NULLP(UIW_VT_LIST), ZIL_NULLP(UIW_VT_LIST) };
UIW_VT_LIST *ZAF_OBJECT_EDITOR::_objList[2] = { ZIL_NULLP(UIW_VT_LIST), ZIL_NULLP(UIW_VT_LIST) };
UIW_COMBO_BOX *ZAF_OBJECT_EDITOR::_subList[2] = { ZIL_NULLP(UIW_COMBO_BOX), ZIL_NULLP(UIW_COMBO_BOX) };
UIW_WINDOW *ZAF_OBJECT_EDITOR::_posWindow[2] = { ZIL_NULLP(UIW_WINDOW), ZIL_NULLP(UIW_WINDOW) };
ZAF_GEOMETRY_MANAGER *ZAF_OBJECT_EDITOR::_geoWindow[2] = { ZIL_NULLP(ZAF_GEOMETRY_MANAGER), ZIL_NULLP(ZAF_GEOMETRY_MANAGER) };
UIW_WINDOW *ZAF_OBJECT_EDITOR::_advWindow[2] = { ZIL_NULLP(UIW_WINDOW), ZIL_NULLP(UIW_WINDOW) };

ZIL_ICHAR _radioName[] = { 'U','I','W','_','R','A','D','I','O','_','B','U','T','T','O','N', 0 };
ZIL_ICHAR _checkName[] = { 'U','I','W','_','C','H','E','C','K','_','B','O','X', 0 };
ZIL_ICHAR _hzScrollName[] = { 'U','I','W','_','H','Z','_','S','C','R','O','L','L','_','B','A','R', 0 };
ZIL_ICHAR _vtScrollName[] = { 'U','I','W','_','V','T','_','S','C','R','O','L','L','_','B','A','R', 0 };

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_OBJECT_EDITOR::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_OBJECT_EDITOR::TABLE ZAF_OBJECT_EDITOR::_classTable[] =
{
	// ----- Input Objects -----
	{ ID_STRING, UIW_STRING::_className,
		TRUE, 0,
		ZAF_STRING::New, ZIL_PROCESS_REFERENCE(UIW_STRING_DataLoad), ZIL_PROCESS_REFERENCE(UIW_STRING_DataStore) },
	{ ID_FORMATTED_STRING, UIW_FORMATTED_STRING::_className,
		TRUE, 0,
		ZAF_FORMATTED_STRING::New, ZIL_PROCESS_REFERENCE(UIW_FORMATTED_STRING_DataLoad), ZIL_PROCESS_REFERENCE(UIW_FORMATTED_STRING_DataStore) },
	{ ID_TEXT, UIW_TEXT::_className,
		TRUE, 0,
		ZAF_TEXT::New, ZIL_PROCESS_REFERENCE(UIW_TEXT_DataLoad), ZIL_PROCESS_REFERENCE(UIW_TEXT_DataStore) },
	{ ID_DATE, UIW_DATE::_className,
		TRUE, 0,
		ZAF_DATE::New, ZIL_PROCESS_REFERENCE(UIW_DATE_DataLoad), ZIL_PROCESS_REFERENCE(UIW_DATE_DataStore) },
	{ ID_TIME, UIW_TIME::_className,
		TRUE, 0,
		ZAF_TIME::New, ZIL_PROCESS_REFERENCE(UIW_TIME_DataLoad), ZIL_PROCESS_REFERENCE(UIW_TIME_DataStore) },
	{ ID_BIGNUM, UIW_BIGNUM::_className,
		TRUE, 0,
		ZAF_BIGNUM::New, ZIL_PROCESS_REFERENCE(UIW_BIGNUM_DataLoad), ZIL_PROCESS_REFERENCE(UIW_BIGNUM_DataStore) },
	{ ID_INTEGER, UIW_INTEGER::_className,
		TRUE, 0,
		ZAF_INTEGER::New, ZIL_PROCESS_REFERENCE(UIW_INTEGER_DataLoad), ZIL_PROCESS_REFERENCE(UIW_INTEGER_DataStore) },
	{ ID_REAL, UIW_REAL::_className,
		TRUE, 0,
		ZAF_REAL::New, ZIL_PROCESS_REFERENCE(UIW_REAL_DataLoad), ZIL_PROCESS_REFERENCE(UIW_REAL_DataStore) },
	// ----- Control Objects -----
	{ ID_BUTTON, UIW_BUTTON::_className,
		TRUE, 0,
		ZAF_BUTTON::New, ZIL_PROCESS_REFERENCE(UIW_BUTTON_DataLoad), ZIL_PROCESS_REFERENCE(UIW_BUTTON_DataStore) },
	{ ID_RADIO_BUTTON, _radioName,
		TRUE, 0,
		ZAF_BUTTON::RadioButtonNew, ZIL_PROCESS_REFERENCE(UIW_BUTTON_DataLoad), ZIL_PROCESS_REFERENCE(UIW_BUTTON_DataStore) },
	{ ID_CHECK_BOX, _checkName,
		TRUE, 0,
		ZAF_BUTTON::CheckBoxNew, ZIL_PROCESS_REFERENCE(UIW_BUTTON_DataLoad), ZIL_PROCESS_REFERENCE(UIW_BUTTON_DataStore) },
	{ ID_SCROLL_BAR, UIW_SCROLL_BAR::_className,
		FALSE, 0,
		0, ZIL_PROCESS_REFERENCE(UIW_SCROLL_BAR_DataLoad), ZIL_PROCESS_REFERENCE(UIW_SCROLL_BAR_DataStore) },
	{ ID_HZ_SCROLL, _hzScrollName,
		TRUE, 0,
		ZAF_SCROLL_BAR::HzSlideNew, ZIL_PROCESS_REFERENCE(UIW_SCROLL_BAR_DataLoad), ZIL_PROCESS_REFERENCE(UIW_SCROLL_BAR_DataStore) },
	{ ID_VT_SCROLL, _vtScrollName,
		TRUE, 0,
		ZAF_SCROLL_BAR::VtSlideNew, ZIL_PROCESS_REFERENCE(UIW_SCROLL_BAR_DataLoad), ZIL_PROCESS_REFERENCE(UIW_SCROLL_BAR_DataStore) },
	{ ID_COMBO_BOX, UIW_COMBO_BOX::_className,
		TRUE, ID_BUTTON,
		ZAF_COMBO_BOX::New, ZIL_PROCESS_REFERENCE(UIW_COMBO_BOX_DataLoad), ZIL_PROCESS_REFERENCE(UIW_COMBO_BOX_DataStore) },
	{ ID_SPIN_CONTROL, UIW_SPIN_CONTROL::_className,
		TRUE, ID_INTEGER,
		ZAF_SPIN_CONTROL::New, ZIL_PROCESS_REFERENCE(UIW_SPIN_CONTROL_DataLoad), ZIL_PROCESS_REFERENCE(UIW_SPIN_CONTROL_DataStore) },
	// ----- Select Objects -----
	{ ID_HZ_LIST, UIW_HZ_LIST::_className,
		TRUE, ID_BUTTON,
		ZAF_HZ_LIST::New, ZIL_PROCESS_REFERENCE(UIW_HZ_LIST_DataLoad), ZIL_PROCESS_REFERENCE(UIW_HZ_LIST_DataStore) },
	{ ID_VT_LIST, UIW_VT_LIST::_className,
		TRUE, ID_BUTTON,
		ZAF_VT_LIST::New, ZIL_PROCESS_REFERENCE(UIW_VT_LIST_DataLoad), ZIL_PROCESS_REFERENCE(UIW_VT_LIST_DataStore) },
	{ ID_TOOL_BAR, UIW_TOOL_BAR::_className,
		TRUE, ID_BUTTON,
		ZAF_TOOL_BAR::New, ZIL_PROCESS_REFERENCE(UIW_TOOL_BAR_DataLoad), ZIL_PROCESS_REFERENCE(UIW_TOOL_BAR_DataStore) },
	{ ID_PULL_DOWN_MENU, UIW_PULL_DOWN_MENU::_className,
		TRUE, ID_PULL_DOWN_ITEM,
		ZAF_PULL_DOWN_MENU::New, ZIL_PROCESS_REFERENCE(UIW_PULL_DOWN_MENU_DataLoad), ZIL_PROCESS_REFERENCE(UIW_PULL_DOWN_MENU_DataStore) },
	{ ID_PULL_DOWN_ITEM, UIW_PULL_DOWN_ITEM::_className,
		TRUE, ID_POP_UP_ITEM,
		ZAF_PULL_DOWN_ITEM::New, ZIL_PROCESS_REFERENCE(UIW_PULL_DOWN_ITEM_DataLoad), ZIL_PROCESS_REFERENCE(UIW_PULL_DOWN_ITEM_DataStore) },
	{ ID_POP_UP_MENU, UIW_POP_UP_MENU::_className,
		FALSE, ID_POP_UP_ITEM,
		ZAF_POP_UP_MENU::New, ZIL_PROCESS_REFERENCE(UIW_POP_UP_MENU_DataLoad), ZIL_PROCESS_REFERENCE(UIW_POP_UP_MENU_DataStore) },
	{ ID_POP_UP_ITEM, UIW_POP_UP_ITEM::_className,
		FALSE, ID_POP_UP_ITEM,
		ZAF_POP_UP_ITEM::New, ZIL_PROCESS_REFERENCE(UIW_POP_UP_ITEM_DataLoad), ZIL_PROCESS_REFERENCE(UIW_POP_UP_ITEM_DataStore) },
	// ----- Static Objects -----
	{ ID_PROMPT, UIW_PROMPT::_className,
		TRUE, 0,
		ZAF_PROMPT::New, ZIL_PROCESS_REFERENCE(UIW_PROMPT_DataLoad), ZIL_PROCESS_REFERENCE(UIW_PROMPT_DataStore) },
	{ ID_GROUP, UIW_GROUP::_className,
		TRUE, ID_BUTTON,
		ZAF_GROUP::New, ZIL_PROCESS_REFERENCE(UIW_GROUP_DataLoad), ZIL_PROCESS_REFERENCE(UIW_GROUP_DataStore) },
	{ ID_ICON, UIW_ICON::_className,
		TRUE, 0,
		ZAF_ICON::New, ZIL_PROCESS_REFERENCE(UIW_ICON_DataLoad), ZIL_PROCESS_REFERENCE(UIW_ICON_DataStore) },
	{ ID_STATUS_BAR, UIW_STATUS_BAR::_className,
		TRUE, ID_STRING,
		ZAF_STATUS_BAR::New, ZIL_PROCESS_REFERENCE(UIW_STATUS_BAR_DataLoad), ZIL_PROCESS_REFERENCE(UIW_STATUS_BAR_DataStore) },
	{ ID_NOTEBOOK, UIW_NOTEBOOK::_className,
		TRUE, ID_WINDOW,
		ZAF_NOTEBOOK::New, ZIL_PROCESS_REFERENCE(UIW_NOTEBOOK_DataLoad), ZIL_PROCESS_REFERENCE(UIW_NOTEBOOK_DataStore) },
	{ ID_TABLE, UIW_TABLE::_className,
		TRUE, ID_STRING,
		ZAF_TABLE::New, ZIL_PROCESS_REFERENCE(UIW_TABLE_DataLoad), ZIL_PROCESS_REFERENCE(UIW_TABLE_DataStore) },
	{ ID_TABLE_HEADER, UIW_TABLE_HEADER::_className,
		FALSE, ID_PROMPT,
		0, ZIL_PROCESS_REFERENCE(UIW_TABLE_HEADER_DataLoad), ZIL_PROCESS_REFERENCE(UIW_TABLE_HEADER_DataStore) },
	{ ID_TABLE_RECORD, UIW_TABLE_RECORD::_className,
		FALSE, ID_STRING,
		0, ZIL_PROCESS_REFERENCE(UIW_TABLE_RECORD_DataLoad), ZIL_PROCESS_REFERENCE(UIW_TABLE_RECORD_DataStore) },
	{ ID_WINDOW, UIW_WINDOW::_className,
		TRUE, ID_STRING,
		ZAF_WINDOW::New, ZIL_PROCESS_REFERENCE(UIW_WINDOW_DataLoad), ZIL_PROCESS_REFERENCE(UIW_WINDOW_DataStore) },
	// ----- Support Objects -----
	{ ID_BORDER, UIW_BORDER::_className,
		FALSE, 0,
		0, ZIL_PROCESS_REFERENCE(UIW_BORDER_DataLoad), ZIL_PROCESS_REFERENCE(UIW_BORDER_DataStore) },
	{ ID_MAXIMIZE_BUTTON, UIW_MAXIMIZE_BUTTON::_className,
		FALSE, 0,
		0, ZIL_PROCESS_REFERENCE(UIW_MAXIMIZE_BUTTON_DataLoad), ZIL_PROCESS_REFERENCE(UIW_MAXIMIZE_BUTTON_DataStore) },
	{ ID_MINIMIZE_BUTTON, UIW_MINIMIZE_BUTTON::_className,
		FALSE, 0,
		0, ZIL_PROCESS_REFERENCE(UIW_MINIMIZE_BUTTON_DataLoad), ZIL_PROCESS_REFERENCE(UIW_MINIMIZE_BUTTON_DataStore) },
	{ ID_SYSTEM_BUTTON, UIW_SYSTEM_BUTTON::_className,
		FALSE, ID_POP_UP_ITEM,
		0, ZIL_PROCESS_REFERENCE(UIW_SYSTEM_BUTTON_DataLoad), ZIL_PROCESS_REFERENCE(UIW_SYSTEM_BUTTON_DataStore) },
	{ ID_TITLE, UIW_TITLE::_className,
		FALSE, 0,
		0, ZIL_PROCESS_REFERENCE(UIW_TITLE_DataLoad), ZIL_PROCESS_REFERENCE(UIW_TITLE_DataStore) },
	{ ID_GEOMETRY_MANAGER, UI_GEOMETRY_MANAGER::_className,
		FALSE, 0,
		0, ZIL_PROCESS_REFERENCE(UI_GEOMETRY_MANAGER_DataLoad), ZIL_PROCESS_REFERENCE(UI_GEOMETRY_MANAGER_DataStore) },
	// ----- Derived Objects -----
	{ ID_DERIVED_OBJECT, ZAF_DERIVED_OBJECT::_className,
		TRUE, 0,
		ZAF_DERIVED_OBJECT::New, ZIL_PROCESS_REFERENCE(ZAF_DERIVED_OBJECT_DataLoad), ZIL_PROCESS_REFERENCE(ZAF_DERIVED_OBJECT_DataStore) },
	{ ID_EDIT_GROUP, ZAF_EDIT_GROUP::_className,
		FALSE, 0,
		0, ZIL_PROCESS_REFERENCE(ZAF_EDIT_GROUP_DataLoad), ZIL_PROCESS_REFERENCE(ZAF_EDIT_GROUP_DataStore) },

	{ ID_END, 0, 0, 0, 0, 0 }
};

ZAF_OBJECT_EDITOR::PROCESS ZAF_OBJECT_EDITOR::_processTable[] =
{
	{ OPT_ADD, 						ZIL_PROCESS_REFERENCE(ObjectAdd) },
	{ OPT_DELETE, 					ZIL_PROCESS_REFERENCE(ObjectDelete) },
	{ OPT_MOVE_UP, 					ZIL_PROCESS_REFERENCE(ObjectMoveUp) },
	{ OPT_MOVE_DOWN, 				ZIL_PROCESS_REFERENCE(ObjectMoveDown) },
	{ 0, 0 }
};
