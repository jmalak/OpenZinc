//	Zinc Interface Library - DERIVE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "window.hpp"
#define USE_ZAF_DERIVED_OBJECT
#include "p_window.hpp"

// --------------------------------------------------------------------------
// ----- ZAF_DERIVED_OBJECT -----------------------------------------------------
// --------------------------------------------------------------------------

ZIL_ICHAR ZAF_DERIVED_OBJECT::_className[] = { 'Z','A','F','_','D','E','R','I','V','E','D','_','O','B','J','E','C','T', 0 };

ZAF_DERIVED_OBJECT::ZAF_DERIVED_OBJECT(void) :
	UI_WINDOW_OBJECT(0, 0, 20, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS)
{
	// Initialize the class information.
	ZAF_DERIVED_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	baseSearchID = ID_WINDOW_OBJECT;

	// Set the associated edit flags.
	if (!ZAF_SERVICE_MANAGER::_testMode)
		woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

ZAF_DERIVED_OBJECT::~ZAF_DERIVED_OBJECT(void)
{
}

EVENT_TYPE ZAF_DERIVED_OBJECT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Draw the derived object.
	UI_REGION region = trueRegion;
	DrawBorder(screenID, region, FALSE, ccode);
	return (ccode);
}

EVENT_TYPE ZAF_DERIVED_OBJECT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		UI_WINDOW_OBJECT::Event(S_REGISTER_OBJECT);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *ZAF_DERIVED_OBJECT::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		searchID = windowID[0] = ID_DERIVED_OBJECT;
		woStatus |= WOS_OWNERDRAW;
		break;

	case I_CHANGED_FLAGS:
		UI_WINDOW_OBJECT::Information(request, data, ID_DERIVED_OBJECT);
		if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

ZAF_DERIVED_OBJECT::ZAF_DERIVED_OBJECT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NO_FLAGS, WOAF_NO_FLAGS)
{
	// Initialize the class information.
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	ZAF_DERIVED_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Set the associated edit flags.
	if (!ZAF_SERVICE_MANAGER::_testMode)
		woStatus |= WOS_EDIT_MODE;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS;
}

UI_WINDOW_OBJECT *ZAF_DERIVED_OBJECT::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// ----- Persistent Constructor -----
	if (file)
		return (new ZAF_DERIVED_OBJECT(name, file, object, objectTable, userTable));

	return (new ZAF_DERIVED_OBJECT);
}

// --------------------------------------------------------------------------

void ZAF_OBJECT_EDITOR::ZAF_DERIVED_OBJECT_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	ZAF_DERIVED_OBJECT *object = (ZAF_DERIVED_OBJECT *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT |
		WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION |
		WOF_NO_ALLOCATE_DATA | WOF_AUTO_CLEAR |
		WOF_SUPPORT_OBJECT |
		WOF_VIEW_ONLY | WOF_UNANSWERED | WOF_INVALID | WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_NON_CURRENT |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;

	// Load the object data.
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
}

int ZAF_OBJECT_EDITOR::ZAF_DERIVED_OBJECT_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	ZAF_DERIVED_OBJECT *object = (ZAF_DERIVED_OBJECT *)data;

	// Store the object data.
	int changed = FALSE;
	changed |= GetText(FIELD_STRINGID, object->stringID);
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);

	// Return the status.
	return (changed);
}
