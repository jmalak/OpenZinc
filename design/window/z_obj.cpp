//	Zinc Interface Library - Z_OBJ.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UI_OBJECT
#define USE_MSG_TABLE
#include "p_window.hpp"

// ----- UIW_OBJECT_ITEM ----------------------------------------------------

class UIW_OBJECT_ITEM : public UIW_BUTTON
{
public:
	UIW_OBJECT_ITEM(UI_WINDOW_OBJECT *object, ZIL_ICHAR *bitmapName);
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ItemCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

UIW_OBJECT_ITEM::UIW_OBJECT_ITEM(UI_WINDOW_OBJECT *_object, ZIL_ICHAR *bitmapName) :
	UIW_BUTTON(0, 0, 50, ZIL_NULLP(ZIL_ICHAR), BTF_NO_3D | BTF_DOUBLE_CLICK,
	WOF_NO_FLAGS, ItemCallback, 0, bitmapName)
{
	userObject = _object;
	woStatus |= WOS_OWNERDRAW;
}

EVENT_TYPE UIW_OBJECT_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	const int FIELD1_OFFSET = 6;

	UI_REGION region = trueRegion;
	display->VirtualGet(screenID, region);
	lastPalette = LogicalPalette(ccode, ID_LIST_ITEM);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);

	if (bitmapArray || colorBitmap)
	{
		region.right = trueRegion.left + FIELD1_OFFSET * display->cellWidth - 1;
		region.left += (region.Width() - bitmapWidth) / 2;
		region.top += (region.Height() - bitmapHeight) / 2;
		display->Bitmap(screenID, region.left, region.top,
			bitmapWidth, bitmapHeight, bitmapArray, ZIL_NULLP(UI_PALETTE),
			&clip, &colorBitmap, &monoBitmap);
	}

	// Draw the item text.
	region = trueRegion;
	region.left += FIELD1_OFFSET * display->cellWidth;
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)userObject;
	DrawText(screenID, region, object->StringID(), lastPalette, FALSE, ccode);

	// Draw the focus rectangle.
	region = trueRegion;
	if (ccode == S_CURRENT)
		DrawFocus(screenID, region, ccode);

	woStatus &= ~WOS_REDISPLAY;
	display->VirtualPut(screenID);
	return (ccode);
}

EVENT_TYPE UIW_OBJECT_ITEM::ItemCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		UI_WINDOW_OBJECT *editObject = (UI_WINDOW_OBJECT *)object->userObject;
		if (editObject)
		{
			eventManager->DeviceState(E_MOUSE, DM_WAIT);
			ZIL_OBJECTID searchID = editObject->SearchID();
			UI_ITEM *table = ZAF_WINDOW::_objectTable;

			// Find a matching edit object.
			int i;
			for (i = 0; table[i].value != ID_END; i++)
				if (table[i].value == searchID)
					break;
			if (table[i].value != ID_END)
				*windowManager + new ZAF_OBJECT_EDITOR(table[i].text, editObject);
			else if (editObject->Inherited(ID_WINDOW))
				*windowManager + new ZAF_OBJECT_EDITOR(UIW_WINDOW::_className, editObject);
			else
				*windowManager + new ZAF_OBJECT_EDITOR(UI_WINDOW_OBJECT::_className, editObject);
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
	}
	return (ccode);
}

// ----- Directory Services -------------------------------------------------

ZIL_ICHAR *ZAF_OBJECT_EDITOR::BitmapName(UI_WINDOW_OBJECT *object)
{
	UI_ITEM *table = ZAF_WINDOW::_objectTable;
	ZIL_OBJECTID searchID = object->SearchID();
	if (searchID >= ID_DERIVED_START)
	{
		int i;
		for (i = 0; table[i].value != ID_END; i++)
			if (table[i].value == searchID)
				break;
		int j;
		for (j = 0; table[j].value != ID_END; j++)
			if (table[i].data == table[j].data)
				break;
		searchID = (ZIL_OBJECTID)table[j].value; // Reset value to the base class.
	}
	for (int i = 0; table[i].value != ID_END; i++)
		if (table[i].value == searchID)
			return (table[i].text);
	return (ZIL_NULLP(ZIL_ICHAR));
}

void ZAF_OBJECT_EDITOR::ListUpdate(void)
{
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = ZAF_WINDOW_EDITOR::_storage;

	// Clear the directory entries.
	dirList->Destroy();
	objList->Destroy();

	// Determine the parent objects.
	int indentation = 0;
	UI_LIST tlist;
	UI_WINDOW_OBJECT *item;
	for (UI_WINDOW_OBJECT *pObject = dirObject->parent; pObject; pObject = pObject->parent)
	{
		item = new UIW_BUTTON(0, 0, 50, pObject->StringID(),
			BTF_NO_3D | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
			ZIL_NULLF(ZIL_USER_FUNCTION), OPT_RESET_PARENT, BitmapName(pObject));
		item->userObject = pObject;
		tlist.Add(item);
	}
	for (item = (UI_WINDOW_OBJECT *)tlist.Last(); item; item = (UI_WINDOW_OBJECT *)tlist.Last())
	{
		tlist.Subtract(item);
		item->relative.left += indentation;
		item->relative.right += indentation;
		dirList->Add(item);
		indentation++;
	}

	// Determine the current object.
	UI_WINDOW_OBJECT *cItem = new UIW_BUTTON(indentation++, 0, 50,
		dirObject->StringID(), BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
		ZIL_NULLF(ZIL_USER_FUNCTION), OPT_RESET_OBJECT, BitmapName(dirObject));
	cItem->userObject = dirObject;
	dirList->Add(cItem);

	// Place the support-list name.
	UIW_STRING *sSeparator = new UIW_STRING(0, 0, 50,
		msgTable.GetMessage(ZMSG_SUPPORT_OBJECTS), -1, STF_NO_FLAGS,
		WOF_VIEW_ONLY);
	sSeparator->woAdvancedFlags |= WOAF_NON_CURRENT;
	objList->Add(sSeparator);
	UIW_STRING *oSeparator = new UIW_STRING(0, 0, 50,
		msgTable.GetMessage(ZMSG_NORMAL_OBJECTS), -1, STF_NO_FLAGS,
		WOF_VIEW_ONLY);
	oSeparator->woAdvancedFlags |= WOAF_NON_CURRENT;

	// Determine sub-objects.
	UI_WINDOW_OBJECT *cObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	dirObject->Information(I_GET_SUPPORT_FIRST, &cObject);
	if (!cObject)
	{
		dirObject->Information(I_GET_FIRST, &cObject);
		objList->Add(oSeparator);
	}
	while (cObject)
	{
		// Create the object item.
		ZIL_OBJECTID searchID = cObject->SearchID();
		ZIL_ICHAR *bitmapName = BitmapName(cObject);
		item = new UIW_OBJECT_ITEM(cObject, bitmapName);
		objList->Add(item);

		// Check for sub-objects.
		for (int i = 0; _classTable[i].objectID != ID_END; i++)
			if (_classTable[i].objectID == searchID)
			{
				if (_classTable[i].subObjectID)
				{
					item = new UIW_BUTTON(indentation, 0, 50, cObject->StringID(),
						BTF_NO_3D | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
						ZIL_NULLF(ZIL_USER_FUNCTION), OPT_RESET_CHILD, bitmapName);
					item->userObject = cObject;
					dirList->Add(item);
				}
				break;
			}

		// Get the next item in the list.
		if (cObject->Next())
			cObject = cObject->Next();
		else if (FlagSet(cObject->woFlags, WOF_SUPPORT_OBJECT))
		{
			dirObject->Information(I_GET_FIRST, &cObject);
			objList->Add(oSeparator);
		}
		else
			cObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	}

	// Update the list information.
	dirList->Add(cItem);
	if (screenID)
	{
		dirList->Event(S_REDISPLAY);
		objList->Event(S_REDISPLAY);
	}
	defaultStorage = saveStorage;
}

// ----- UI_OBJECT ----------------------------------------------------------

EVENT_TYPE ZAF_OBJECT_EDITOR::Event(const UI_EVENT &event)
{
	// Check for user controls.
	EVENT_TYPE ccode = event.type;
	if ((ccode >= D_DESIGNER_FIRST && ccode <= D_DESIGNER_LAST) ||
		(ccode <= -D_DESIGNER_FIRST && ccode >= -D_DESIGNER_LAST))
	{
		for (int i = 0; _processTable[i].option; i++)
			if (_processTable[i].option == ccode)
				return ((this->*(_processTable[i].Process))(event));
	}

	// Switch on the event type.
	switch (ccode)
	{
	case OPT_OK:
		{
		// Store the edit information.
		int page = 1;
		int changed = FALSE;
		WOF_FLAGS oldFlags = editObject->woFlags;

		// --- Level 2: Sub-object information.
		if (subWindow)
		{
			notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
			changed |= OBJ_SUBOBJECT_DataStore(editObject);
		}

		// --- Level 3: Position information.
		notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
		changed |= OBJ_POSITION_DataStore(editObject);

		// --- Level 4: Geometry management.
		if (geoWindow)
		{
			notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
			changed |= geoWindow->DataStore(editObject);
		}

		// --- Level 5: Advanced information.
		notebook->Information(I_SET_SEARCH_PAGE, &page); page++;
		changed |= OBJ_ADVANCED_DataStore(editObject);

		// --- Level 1: Object specific information (must be last to override).
		page = 0;
		notebook->Information(I_SET_SEARCH_PAGE, &page);
		if (DataStore != 0)
			changed |= (this->*DataStore)(editObject);

		oldFlags |= editObject->woFlags;
		if (changed)
			woStatus |= WOS_CHANGED;

		// Redisplay the object.
#if defined(ZIL_MOTIF)
		if (!FlagSet(woStatus, WOS_CHANGED))
			;
		else if (!editObject->parent ||
			editObject->parent->Inherited(ID_NOTEBOOK) ||
			editObject->parent->Inherited(ID_TOOL_BAR) ||
			editObject->parent->Inherited(ID_TABLE_RECORD))
		{
			UI_EVENT cEvent(S_CHANGED);
			cEvent.region = editObject->Root()->relative;
			editObject->Root()->Event(cEvent);
		}
		else if (FlagSet(oldFlags, WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT) ||
			(geoWindow && FlagSet(geoWindow->woStatus, WOS_CHANGED)))
		{
			UI_EVENT cEvent(S_CHANGED);
			cEvent.region = editObject->parent->relative;
			editObject->parent->Event(cEvent);
		}
		else
			editObject->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
#else
		if (!FlagSet(woStatus, WOS_CHANGED))
			;
		else if (!editObject->parent ||
			editObject->parent->Inherited(ID_NOTEBOOK) ||
			editObject->parent->Inherited(ID_TOOL_BAR) ||
			editObject->parent->Inherited(ID_TABLE_RECORD))
			editObject->Root()->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
		else if (FlagSet(oldFlags, WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT) ||
			(geoWindow && FlagSet(geoWindow->woStatus, WOS_CHANGED)) ||
			editObject->parent->Inherited(ID_SPIN_CONTROL))
			editObject->parent->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
		else
			editObject->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
#endif

		// Warn the other editors.
		if (FlagSet(woStatus, WOS_CHANGED))
		{
			for (UI_WINDOW_OBJECT *window = Next(); window; window = window->Next())
				if (window->SearchID() == ID_OBJECT_EDITOR)
					window->Information(I_CHECK_EDIT_NAMES, editObject);
		}
		}

		// Continue to OPT_CANCEL.
	case OPT_CANCEL:
		eventManager->Put(S_CLOSE); // Close the window.
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case OPT_EDIT:
		if (objList && objList->Current())
			objList->Current()->Event(L_SELECT);
		break;

	case OPT_EDIT_CHILD:
		// Special code for spin control.
		{
		// Store the object before entering the sub-editor.
		int page = 0;
		notebook->Information(I_SET_SEARCH_PAGE, &page);
		if ((this->*DataStore)(editObject))
		{
			editObject->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
			ListUpdate();
		}
		UI_WINDOW_OBJECT *object;
		editObject->Information(I_GET_FIRST, &object);
		if (object)
		{
			// Find a matching edit object.
			eventManager->DeviceState(E_MOUSE, DM_WAIT);
			ZIL_OBJECTID searchID = object->SearchID();
			UI_ITEM *table = ZAF_WINDOW::_objectTable;
			int i;
			for (i = 0; table[i].value != ID_END; i++)
				if (table[i].value == searchID)
					break;
			*windowManager + new ZAF_OBJECT_EDITOR(table[i].text, object);
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		}
		break;

	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
	case S_DROP_DEFAULT:
		ObjectAdd(event);
		break;

	case OPT_RESET_PARENT:
	case OPT_RESET_CHILD:
		{
		int page = 1;
		Information(I_SET_SEARCH_PAGE, &page);
		dirObject = (UI_WINDOW_OBJECT *)event.windowObject->userObject;
		ListUpdate();
		ZIL_OBJECTID searchID = dirObject->SearchID();
		for (int i = 0; _classTable[i].objectID != ID_END; i++)
			if (_classTable[i].objectID == searchID && _classTable[i].subObjectID)
			{
				subList->Information(I_SET_TEXT, nameTable.GetMessage(_classTable[i].subObjectID));
				break;
			}
		}
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(ZAF_WINDOW_EDITOR::_storage, FALSE);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- Object options -----------------------------------------------------

EVENT_TYPE ZAF_OBJECT_EDITOR::ObjectAdd(const UI_EVENT &event)
{
	// Check for a valid sub-list.
	if (!subList)
		return (S_ERROR);

	// Get the window editor.
	UI_WINDOW_OBJECT *windowEditor = ZIL_NULLP(UIW_WINDOW);
	windowEditor = _serviceManager->Get(ID_WINDOW_EDITOR);
	// Get the duplicate object.
	UI_WINDOW_OBJECT *sObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	subList->Information(I_GET_CURRENT, &sObject);
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)windowEditor->Information(I_GET_DUPLICATE_OBJECT, sObject->StringID());

	// Add the object to the window and object list.
	if (object)
	{
		// add the window object.
		UI_EVENT event(S_ADD_OBJECT);
		event.windowObject = object;
		if (dirObject->Event(event) != S_ERROR)
			ListUpdate();
		else
		{
			UI_ERROR_STUB::Beep();
			delete object;
		}
	}
	woStatus |= WOS_CHANGED;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_OBJECT_EDITOR::ObjectDelete(const UI_EVENT &event)
{
	// Check for a valid object-list.
	if (!objList->Current() || !objList->Current()->Inherited(ID_BUTTON))
	{
		UI_ERROR_STUB::Beep();
		return (S_ERROR);
	}

	// Get the affected list items.
	UI_WINDOW_OBJECT *cObject = objList->Current();
	UI_WINDOW_OBJECT *nObject = cObject->Next() ? cObject->Next() : cObject->Previous();

	// Check for special support objects (removed with flags).
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)cObject->userObject;
	if (object->parent->parent && object->NumberID() == NUMID_GEOMETRY)
		; // TEMPORARY patch (must delete window geometry from option list).
	else if (object->NumberID() >= (ZIL_NUMBERID)0xFFF0 ||
		object->SearchID() == ID_TABLE_RECORD ||
		object->SearchID() == ID_TABLE_HEADER)
		{
			UI_ERROR_STUB::Beep();
			return (S_ERROR);
		}

	// Check for parent deletion.
	UI_WINDOW_OBJECT *root = NULL;
	for (root = editObject; root; root = root->parent)
		if (root == object)
		{
			UI_ERROR_STUB::Beep();
			return (S_ERROR);
		}
	UI_WINDOW_OBJECT *window = NULL;
	for (window = Next(); window; window = window->Next())
		if (window->SearchID() == ID_OBJECT_EDITOR)
		{
			UI_WINDOW_OBJECT *tempObject = ZIL_NULLP(UI_WINDOW_OBJECT);
			window->Information(I_GET_EDIT_OBJECT, &tempObject);
			// Check for root matches
			for (root = tempObject; root; root = root->parent)
				if (root == object)
				{
					UI_ERROR_STUB::Beep();
					return (S_ERROR);
				}
		}

	// Remove the geometry management.
	UI_GEOMETRY_MANAGER *gmgr = (UI_GEOMETRY_MANAGER *)object->parent->Get(NUMID_GEOMETRY);
	if (gmgr)
	{
		for (UI_CONSTRAINT *constraint = gmgr->First(); constraint; )
		{
			UI_CONSTRAINT *temp = constraint->Next();
			if (object == (UI_WINDOW_OBJECT *)constraint->Information(I_GET_OBJECT, ZIL_NULLP(void)))
			{
				gmgr->Subtract(constraint);
				delete constraint;
			}
			constraint = temp;
		}
	}

	// Remove the specified object.
	UI_EVENT sEvent(S_SUBTRACT_OBJECT);
	sEvent.windowObject = object;
	UI_WINDOW_OBJECT *pObject = object->parent;
	if (pObject->Event(sEvent) == S_ERROR)
	{
		UI_ERROR_STUB::Beep();
		return (S_ERROR);
	}
#if defined(ZIL_MOTIF)
	UI_EVENT cEvent(S_CHANGED);
	cEvent.region = pObject->relative;
	pObject->Event(cEvent);
#else
	pObject->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
#endif

	// Notify any potentially affected editors (dirList & objList).
	for (window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_OBJECT_EDITOR)
			window->Information(I_CHECK_EDIT_DELETE, object);
	delete object;

	// Update the presentation list.
	objList->Add(nObject);
	UI_EVENT scrollEvent(S_VSCROLL_CHECK);
	objList->ScrollEvent(scrollEvent);
	woStatus |= WOS_CHANGED;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_OBJECT_EDITOR::ObjectMoveUp(const UI_EVENT &event)
{
	// Check for a valid object-list.
	if (!objList->Current() || objList->Current() == objList->First() ||
		!objList->Current()->Inherited(ID_BUTTON))
		return (S_ERROR);

	// Get the affected list items.
	UI_WINDOW_OBJECT *cObject = objList->Current();
	UI_WINDOW_OBJECT *pObject = cObject->Previous();

	// Move the specified object.
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)cObject->userObject;
	// Check for special support objects (removed with flags).
	if (!object->Previous() || object->NumberID() >= (ZIL_NUMBERID)0xFFF0 ||
		object->Previous()->NumberID() >= (ZIL_NUMBERID)0xFFF0)
		return (S_ERROR);
	UI_WINDOW_OBJECT *offset = object->Previous();
	UIW_WINDOW *window = (UIW_WINDOW *)object->parent;
	if (window->support.Index(object) >= 0)
	{
		window->support.UI_LIST::Subtract(object);
		window->support.UI_LIST::Add(offset, object);
	}
	else if (window->UI_LIST::Index(object) >= 0)
	{
		window->UI_LIST::Subtract(object);
		window->UI_LIST::Add(offset, object);
	}
	objList->UI_LIST::Subtract(cObject);
	objList->UI_LIST::Add(pObject, cObject);

	// Check the list.
	objList->Add(cObject);
	objList->Event(S_REDISPLAY);
	UI_EVENT scrollEvent(S_VSCROLL_CHECK);
	objList->ScrollEvent(scrollEvent);
	woStatus |= WOS_CHANGED;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_OBJECT_EDITOR::ObjectMoveDown(const UI_EVENT &event)
{
	// Check for a valid object-list.
	if (!objList->Current() || objList->Current() == objList->Last() ||
		!objList->Current()->Inherited(ID_BUTTON))
		return (S_ERROR);

	// Get the affected list items.
	UI_WINDOW_OBJECT *cObject = objList->Current();
	UI_WINDOW_OBJECT *nObject = cObject->Next()->Next();

	// Move the specified object.
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)cObject->userObject;
	// Check for special support objects (removed with flags).
	if (!object->Next() || object->NumberID() >= (ZIL_NUMBERID)0xFFF0 ||
		object->Next()->NumberID() >= (ZIL_NUMBERID)0xFFF0)
		return (S_ERROR);
	UI_WINDOW_OBJECT *offset = object->Next()->Next();
	UIW_WINDOW *window = (UIW_WINDOW *)object->parent;
	if (window->support.Index(object) >= 0)
	{
		window->support.UI_LIST::Subtract(object);
		window->support.UI_LIST::Add(offset, object);
	}
	else if (window->UI_LIST::Index(object) >= 0)
	{
		window->UI_LIST::Subtract(object);
		window->UI_LIST::Add(offset, object);
	}
	objList->UI_LIST::Subtract(cObject);
	objList->UI_LIST::Add(nObject, cObject);

	// Check the list.
	objList->Add(cObject);
	objList->Event(S_REDISPLAY);
	UI_EVENT scrollEvent(S_VSCROLL_CHECK);
	objList->ScrollEvent(scrollEvent);
	woStatus |= WOS_CHANGED;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

// ----- Special members ----------------------------------------------------

void ZAF_OBJECT_EDITOR::OBJ_SUBOBJECT_DataLoad(UI_WINDOW_OBJECT *)
{
	ListUpdate();
}

int ZAF_OBJECT_EDITOR::OBJ_SUBOBJECT_DataStore(UI_WINDOW_OBJECT *)
{
	// Return the status.
	return (FALSE);
}
