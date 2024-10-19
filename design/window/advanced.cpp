//	Zinc Interface Library - ADVANCED.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_OBJ_ADVANCED
#include "p_window.hpp"

// ----- OBJ_ADVANCED -------------------------------------------------------

void ZAF_OBJECT_EDITOR::OBJ_ADVANCED_DataLoad(UI_WINDOW_OBJECT *object)
{
	// Load the object data.
	int value = object->numberID;
	SetValue(FIELD_NUMBERID, value);
	if (object->numberID >= (ZIL_NUMBERID)0xFF00)
		notebook->Get(FIELD_NUMBERID)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FIELD_NUMBERID)->woFlags &= ~WOF_NON_SELECTABLE;
	SetText(FIELD_USER_OBJECT, object->userObjectName);
	value = object->userFlags;
	SetValue(FIELD_USER_FLAGS, value);
	value = object->userStatus;
	SetValue(FIELD_USER_STATUS, value);
	if (FlagSet(dsFlagsAllowed, DSF_CALLBACK))
	{
		notebook->Get(FIELD_CALLBACK)->woFlags &= ~WOF_NON_SELECTABLE;
		SetText(FIELD_CALLBACK, object->userFunctionName);
	}
	else
	{
		notebook->Get(FIELD_CALLBACK)->woFlags |= WOF_NON_SELECTABLE;
		SetText(FIELD_CALLBACK, _blankString);
	}

	// Load the object flags (group 1).
	WOAF_FLAGS advancedFlags = FlagSet(object->woStatus, WOS_EDIT_MODE) ? object->designerAdvancedFlags : object->woAdvancedFlags;
	SetFlag(FLAG_NO_FLAGS,
		FlagSet(object->woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE) |
		FlagSet(advancedFlags, WOAF_NON_CURRENT), 0);
	if (!FlagSet(woFlagsAllowed, WOF_VIEW_ONLY | WOF_NON_SELECTABLE) &&
		!FlagSet(woAdvancedFlagsAllowed, WOAF_NON_CURRENT))
		notebook->Get(FLAG_NO_FLAGS)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_NO_FLAGS)->woFlags &= ~WOF_NON_SELECTABLE;
	SetFlag(FLAG_NON_SELECTABLE_GRAY, object->woFlags, WOF_NON_SELECTABLE);
	if (!FlagSet(woFlagsAllowed, WOF_NON_SELECTABLE))
		notebook->Get(FLAG_NON_SELECTABLE_GRAY)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_NON_SELECTABLE_GRAY)->woFlags &= ~WOF_NON_SELECTABLE;

	// Special flags.
	UI_WINDOW_OBJECT *viewField = notebook->Get(FLAG_VIEW_ONLY);
	UI_WINDOW_OBJECT *currentField = notebook->Get(FLAG_NON_CURRENT);
	UI_WINDOW_OBJECT *bothField = notebook->Get(FLAG_NON_SELECTABLE_BLACK);
	if (FlagSet(object->woFlags, WOF_VIEW_ONLY) && FlagSet(advancedFlags, WOAF_NON_CURRENT))
	{
		bothField->woStatus |= WOS_SELECTED; // set the non-selectable black.
		viewField->woStatus &= ~WOS_SELECTED;
		currentField->woStatus &= ~WOS_SELECTED;
	}
	else if (FlagSet(object->woFlags, WOF_VIEW_ONLY))
	{
		bothField->woStatus &= ~WOS_SELECTED;
		viewField->woStatus |= WOS_SELECTED; // set the view-only.
		currentField->woStatus &= ~WOS_SELECTED;
	}
	else if (FlagSet(advancedFlags, WOAF_NON_CURRENT))
	{
		bothField->woStatus &= ~WOS_SELECTED;
		viewField->woStatus &= ~WOS_SELECTED;
		currentField->woStatus |= WOS_SELECTED; // set the non-current.
	}
	else
	{
		bothField->woStatus &= ~WOS_SELECTED;
		viewField->woStatus &= ~WOS_SELECTED;
		currentField->woStatus &= ~WOS_SELECTED;
	}
	if (!FlagSet(woFlagsAllowed, WOF_VIEW_ONLY))
	{
		viewField->woFlags |= WOF_NON_SELECTABLE;
		bothField->woFlags |= WOF_NON_SELECTABLE;
	}
	else
	{
		viewField->woFlags &= ~WOF_NON_SELECTABLE;
		bothField->woFlags &= ~WOF_NON_SELECTABLE;
	}
	if (!FlagSet(woAdvancedFlagsAllowed, WOAF_NON_CURRENT))
	{
		currentField->woFlags |= WOF_NON_SELECTABLE;
		bothField->woFlags |= WOF_NON_SELECTABLE;
	}
	else
	{
		currentField->woFlags &= ~WOF_NON_SELECTABLE;
		bothField->woFlags &= ~WOF_NON_SELECTABLE;
	}

	// Load the object flags (group 2).
	SetFlag(FLAG_INVALID, object->woFlags, WOF_INVALID);
	if (!FlagSet(woFlagsAllowed, WOF_INVALID))
		notebook->Get(FLAG_INVALID)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_INVALID)->woFlags &= ~WOF_NON_SELECTABLE;
	SetFlag(FLAG_UNANSWERED, object->woFlags, WOF_UNANSWERED);
	if (!FlagSet(woFlagsAllowed, WOF_UNANSWERED))
		notebook->Get(FLAG_UNANSWERED)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_UNANSWERED)->woFlags &= ~WOF_NON_SELECTABLE;

	// Load the object flags (group 3).
	SetFlag(FLAG_NO_DEALLOCATE, object->woFlags, WOF_NO_ALLOCATE_DATA);
	if (!FlagSet(woFlagsAllowed, WOF_NO_ALLOCATE_DATA))
		notebook->Get(FLAG_NO_DEALLOCATE)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_NO_DEALLOCATE)->woFlags &= ~WOF_NON_SELECTABLE;
	SetFlag(FLAG_MOVE_DRAG_OBJECT, advancedFlags, WOAF_MOVE_DRAG_OBJECT);
	if (!FlagSet(woAdvancedFlagsAllowed, WOAF_MOVE_DRAG_OBJECT))
		notebook->Get(FLAG_MOVE_DRAG_OBJECT)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_MOVE_DRAG_OBJECT)->woFlags &= ~WOF_NON_SELECTABLE;
	SetFlag(FLAG_COPY_DRAG_OBJECT, advancedFlags, WOAF_COPY_DRAG_OBJECT);
	if (!FlagSet(woAdvancedFlagsAllowed, WOAF_COPY_DRAG_OBJECT))
		notebook->Get(FLAG_COPY_DRAG_OBJECT)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_COPY_DRAG_OBJECT)->woFlags &= ~WOF_NON_SELECTABLE;
	SetFlag(FLAG_DROP_OBJECT, advancedFlags, WOAF_ACCEPTS_DROP);
	if (!FlagSet(woAdvancedFlagsAllowed, WOAF_ACCEPTS_DROP))
		notebook->Get(FLAG_DROP_OBJECT)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_DROP_OBJECT)->woFlags &= ~WOF_NON_SELECTABLE;

	// Special flags
	SetFlag(FLAG_SUPPORT_OBJECT, object->woFlags, WOF_SUPPORT_OBJECT);
	if (!FlagSet(woFlagsAllowed, WOF_SUPPORT_OBJECT))
		notebook->Get(FLAG_SUPPORT_OBJECT)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FLAG_SUPPORT_OBJECT)->woFlags &= ~WOF_NON_SELECTABLE;

	// Get the derive object name.
	if (!object->parent)
		notebook->Get(FIELD_DERIVED_OBJECT)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(FIELD_DERIVED_OBJECT)->woFlags &= ~WOF_NON_SELECTABLE;
	if (object->searchID >= ID_DERIVED_START)
	{
		UI_ITEM *deriveTable = ZAF_WINDOW::_deriveTable;
		for (int i = 0; deriveTable[i].value != ID_END; i++)
			if (object->searchID == deriveTable[i].value)
			{
				SetText(FIELD_DERIVED_OBJECT, deriveTable[i].text);
				break;
			}
	}
	else
		SetText(FIELD_DERIVED_OBJECT, _serviceManager->ZMSG_none());
}

int ZAF_OBJECT_EDITOR::OBJ_ADVANCED_DataStore(UI_WINDOW_OBJECT *object)
{
	// Store the object data.
	int changed = FALSE;
	int value, i, j;
	if (GetValue(FIELD_NUMBERID, &value) && value >= object->numberID)
	{
		changed |= TRUE;
		object->numberID = (NUMBERID)value;
	}
	changed |= GetText(FIELD_USER_OBJECT, &object->userObjectName);
	if (GetValue(FIELD_USER_FLAGS, &value))
	{
		changed |= TRUE;
		object->userFlags = (UIF_FLAGS)value;
	}
	if (GetValue(FIELD_USER_STATUS, &value))
	{
		changed |= TRUE;
		object->userStatus = (UIS_STATUS)value;
	}
	changed |= GetText(FIELD_CALLBACK, &object->userFunctionName);

	// Store the object flags (group 1).
	WOAF_FLAGS *advancedFlags = FlagSet(object->woStatus, WOS_EDIT_MODE) ? &object->designerAdvancedFlags : &object->woAdvancedFlags;
	changed |= GetFlag(FLAG_INVALID, &object->woFlags, WOF_INVALID);
	changed |= GetFlag(FLAG_UNANSWERED, &object->woFlags, WOF_UNANSWERED);
	// Store the object flags (group 2). Black field must be called first.
	changed |= GetFlag(FLAG_VIEW_ONLY, &object->woFlags, WOF_VIEW_ONLY);
	changed |= GetFlag(FLAG_NON_CURRENT, advancedFlags, WOAF_NON_CURRENT);
	changed |= GetFlag(FLAG_NON_SELECTABLE_GRAY, &object->woFlags, WOF_NON_SELECTABLE);
	UI_WINDOW_OBJECT *bothField = notebook->Get(FLAG_NON_SELECTABLE_BLACK);
	if (FlagSet(bothField->woStatus, WOS_SELECTED) &&
		(!FlagSet(object->woFlags, WOF_VIEW_ONLY) ||
		 !FlagSet(*advancedFlags, WOAF_NON_CURRENT)))
	{
		changed |= TRUE;
		object->woFlags |= WOF_VIEW_ONLY;
		*advancedFlags |= WOAF_NON_CURRENT;
	}
	// Store the object flags (group 3).
	changed |= GetFlag(FLAG_NO_DEALLOCATE, &object->woFlags, WOF_NO_ALLOCATE_DATA);
	changed |= GetFlag(FLAG_MOVE_DRAG_OBJECT, advancedFlags, WOAF_MOVE_DRAG_OBJECT);
	changed |= GetFlag(FLAG_COPY_DRAG_OBJECT, advancedFlags, WOAF_COPY_DRAG_OBJECT);
	changed |= GetFlag(FLAG_DROP_OBJECT, advancedFlags, WOAF_ACCEPTS_DROP);
	// Remove and re-add object to go to proper support/window list.
	if (object->parent && GetFlag(FLAG_SUPPORT_OBJECT, &object->woFlags, WOF_SUPPORT_OBJECT))
	{
		UI_EVENT event;
		changed |= TRUE;
		UI_WINDOW_OBJECT *temp = object->parent;
		event.type = S_SUBTRACT_OBJECT;
		event.windowObject = object;
		temp->Event(event);
		event.type = S_ADD_OBJECT;
		object->woStatus &= ~WOS_EDIT_MODE; // ensure proper placement.
		temp->Event(event);
		object->woStatus |= WOS_EDIT_MODE;
	}

	// Get the derive object type (must be the last check).
	UI_WINDOW_OBJECT *field = notebook->Get(FIELD_DERIVED_OBJECT);
	if (FlagSet(field->woStatus, WOS_CHANGED))
	{
		field->woStatus &= ~WOS_CHANGED;
		ZIL_ICHAR *text;
		field->Information(I_GET_TEXT, &text);
		int openSlot = -1;
		UI_ITEM *table = ZAF_WINDOW::_objectTable;
		UI_ITEM *deriveTable = ZAF_WINDOW::_deriveTable;

		// check for un-derived object.
		if (!text[0] || !stricmp(text, _serviceManager->ZMSG_none()))
		{
			// Check for un-derived object.
			if (object->searchID < ID_DERIVED_START)
				return (changed);

			// Find the matching derived object.
			for (i = 0; deriveTable[i].value != ID_END; i++)
				if (deriveTable[i].value == object->searchID)
					break;
			for (j = 0; table[j].value != ID_END; j++)
				if (deriveTable[i].data == table[j].data)
					break;
			object->searchID = (ZIL_OBJECTID)table[j].value; // Reset value to the base class.
			changed |= TRUE;
			return (changed);
		}

		// check for derived match.
		changed |= TRUE;
		for (i = 0; deriveTable[i].value != ID_END; i++)
			if (!stricmp(text, deriveTable[i].text))
			{
				if (object->searchID != (ZIL_OBJECTID)deriveTable[i].flags)
					openSlot = -1;
				else
					object->searchID = (ZIL_OBJECTID)deriveTable[i].value;
				break;
			}
			else if (openSlot == -1 && deriveTable[i].text == ZAF_DERIVED_OBJECT::_className)
				openSlot = i;
		// Add a new derived object entry, if so specified.
		if (openSlot != -1 && stricmp(text, _serviceManager->ZMSG_none()))
		{
			for (i = 0; table[i].value != ID_END; i++)
				if (table[i].value == object->searchID)
					break;
			// We store the base type of the derived object in the flags field. 
			// This eliminates different base types from having the same derived
			// name.
			deriveTable[openSlot].flags = object->searchID;
			object->searchID = (ZIL_OBJECTID)deriveTable[openSlot].value;
			deriveTable[openSlot].text = strdup(text);
			deriveTable[openSlot].data = table[i].data;
			
		}
	}

	return (changed);
}

