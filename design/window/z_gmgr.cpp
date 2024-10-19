//	Zinc Interface Library - Z_GMGR.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UI_GEOMETRY_MANAGER
#include "p_window.hpp"

// ----- ZAF_CONSTRAINT_LIST ------------------------------------------------

class ZAF_CONSTRAINT_LIST : public UIW_VT_LIST
{
public:
	ZAF_CONSTRAINT_LIST(int left, int top, int right, int bottom);
	EVENT_TYPE Event(const UI_EVENT &event);
	EVENT_TYPE MoveUp(const UI_EVENT &event);
	EVENT_TYPE MoveDown(const UI_EVENT &event);
};

ZAF_CONSTRAINT_LIST::ZAF_CONSTRAINT_LIST(int left, int top, int width, int height) :
	UIW_VT_LIST(left, top, width, height, ZIL_NULLF(ZIL_COMPARE_FUNCTION),
	WNF_NO_WRAP | WNF_CONTINUE_SELECT | WNF_BITMAP_CHILDREN)
{
	Add(new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL));
}

EVENT_TYPE ZAF_CONSTRAINT_LIST::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	if (event.InputType() == E_KEY)
		ccode = UI_EVENT_MAP::MapEvent(_serviceManager->_eventTable, event, ID_WINDOW);
	if (ccode == OPT_MOVE_UP)
		ccode = MoveUp(event);
	else if (ccode == OPT_MOVE_DOWN)
		ccode = MoveDown(event);
	else
		ccode = UIW_VT_LIST::Event(event);
	return (ccode);
}

EVENT_TYPE ZAF_CONSTRAINT_LIST::MoveUp(const UI_EVENT &event)
{
	// Get the affected list items.
	UI_WINDOW_OBJECT *cObject = Current();
	if (!cObject->Previous())
		return (S_ERROR);
	UI_WINDOW_OBJECT *pObject = cObject->Previous();

	// Move the specified object.
	UI_LIST::Subtract(cObject);
	UI_LIST::Add(pObject, cObject);

	// Check the list.
	Add(cObject);
	Event(S_REDISPLAY);
	UI_EVENT scrollEvent(S_VSCROLL_CHECK);
	ScrollEvent(scrollEvent);
	parent->woStatus |= WOS_CHANGED;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_CONSTRAINT_LIST::MoveDown(const UI_EVENT &event)
{
	// Get the affected list items.
	UI_WINDOW_OBJECT *cObject = Current();
	if (!cObject->Next())
		return (S_ERROR);
	UI_WINDOW_OBJECT *nObject = cObject->Next()->Next();

	// Move the specified object.
	UI_LIST::Subtract(cObject);
	UI_LIST::Add(nObject, cObject);

	// Check the list.
	Add(cObject);
	Event(S_REDISPLAY);
	UI_EVENT scrollEvent(S_VSCROLL_CHECK);
	ScrollEvent(scrollEvent);
	parent->woStatus |= WOS_CHANGED;
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

// ----- UI_GEOMETRY_MANAGER ------------------------------------------------

void ZAF_OBJECT_EDITOR::UI_GEOMETRY_MANAGER_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UI_GEOMETRY_MANAGER *object = (UI_GEOMETRY_MANAGER *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_NO_FLAGS;
	woFlagsAllowed = WOF_SUPPORT_OBJECT;
	woAdvancedFlagsAllowed = WOAF_NO_FLAGS;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);

	// Replace sub-list with derived list.
	UI_WINDOW_OBJECT *replaceField = genWindow->Get(FIELD_CONSTRAINTS);
	ZAF_CONSTRAINT_LIST *list = new ZAF_CONSTRAINT_LIST(
		replaceField->relative.left, replaceField->relative.top,
		replaceField->relative.Width(), replaceField->relative.Height());
	list->NumberID(replaceField->NumberID());
	list->StringID(replaceField->StringID());
	genWindow->Subtract(replaceField);
	delete replaceField;
	genWindow->Add(list);
	genWindow->Add(list); // Make it the current object.

	// Add the constraints.
	UI_EVENT addEvent(S_ADD_OBJECT);
	ZIL_STORAGE_READ_ONLY *saveStorage = defaultStorage;
	defaultStorage = ZAF_WINDOW_EDITOR::_storage;
	for (UI_CONSTRAINT *constraint = object->First(); constraint; constraint = constraint->Next())
	{
		static ZIL_ICHAR _none[] = { 'a','t','t','a','c','h','_','n','o','n','e',0 };
		static ZIL_ICHAR _absolute[] = { 'a','t','t','a','c','h','_','a','b','s','o','l','u','t','e',0 };
		static ZIL_ICHAR _relative[] = { 'a','t','t','a','c','h','_','r','e','l','a','t','i','v','e',0 };
		static ZIL_ICHAR _dimension[] = { 'a','t','t','a','c','h','_','d','i','m','e','n','s','i','o','n',0 };
		ZIL_ICHAR *name = _none;
		if (constraint->SearchID() == ID_ATTACHMENT)
			name = _absolute;
		else if (constraint->SearchID() == ID_RELATIVE_CONSTRAINT)
			name = _relative;
		else if (constraint->SearchID() == ID_DIMENSION_CONSTRAINT)
			name = _dimension;
		addEvent.windowObject = new UIW_BUTTON(0, 0, 30,
			constraint->object->StringID(), BTF_NO_3D, WOF_NO_FLAGS,
			ZIL_NULLF(ZIL_USER_FUNCTION), 0, name);
		addEvent.windowObject->userObject = constraint;
		list->Event(addEvent);
	}
	defaultStorage = saveStorage;
}

int ZAF_OBJECT_EDITOR::UI_GEOMETRY_MANAGER_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UI_GEOMETRY_MANAGER *object = (UI_GEOMETRY_MANAGER *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	if (FlagSet(genWindow->woStatus, WOS_CHANGED))
	{
		changed |= TRUE;
		UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);

		// Remove the old constraints.
		UI_EVENT sEvent(S_SUBTRACT_OBJECT);
		UI_WINDOW_OBJECT *gmgr = editObject->parent->Get(NUMID_GEOMETRY);
		for (gmgr->Information(I_GET_FIRST, &object); object; gmgr->Information(I_GET_FIRST, &object))
		{
			sEvent.data = object;
			gmgr->Event(sEvent);
		}

		// Add the new constraints.
		UI_EVENT aEvent(S_ADD_OBJECT);
		UI_WINDOW_OBJECT *ctrlList = genWindow->Get(FIELD_CONSTRAINTS);
		for (ctrlList->Information(I_GET_FIRST, &object); object; object = object->Next())
		{
			aEvent.data = object->userObject;
			gmgr->Event(aEvent);
		}
	}

	// Return the status.
	return (changed);
}

