//	Zinc Interface Library - GROUP.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "window.hpp"
#define USE_ZAF_EDIT_GROUP
#include "p_window.hpp"

const int GROUP_OFFSET = 2;

// --------------------------------------------------------------------------
// ----- ZAF_EDIT_GROUP -----------------------------------------------------
// --------------------------------------------------------------------------

ZIL_ICHAR ZAF_EDIT_GROUP::_className[] = { 'Z','A','F','_','E','D','I','T','_','G','R','O','U','P', 0 };

ZAF_EDIT_GROUP::ZAF_EDIT_GROUP(void) :
	UIW_WINDOW(0, 0, 0, 0)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE | WOS_GRAPHICS | WOS_OWNERDRAW;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS | WOAF_NO_SIZE;

	// Initialize the class information.
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	ZAF_EDIT_GROUP::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

ZAF_EDIT_GROUP::~ZAF_EDIT_GROUP(void)
{
}

EVENT_TYPE ZAF_EDIT_GROUP::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	static UI_PALETTE shadowPalette =
	{
		'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
		PTN_INTERLEAVE_FILL, LIGHTGRAY, WHITE, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
	};

	// Draw the edit area as a shaded region.
	display->VirtualGet(screenID, trueRegion);
#if defined(ZIL_MSDOS)
	UI_REGION clipRegion = trueRegion;
	clip.Overlap(event.region, clipRegion);
	display->Rectangle(screenID, trueRegion, &shadowPalette, 1, TRUE, FALSE, &clipRegion);
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
	display->Rectangle(screenID, trueRegion, &shadowPalette, 1, TRUE);
#elif defined(ZIL_MACINTOSH)
	if (ccode != S_CURRENT && ccode != S_NON_CURRENT)
		display->Rectangle(screenID, trueRegion, &shadowPalette, 1, TRUE);
#endif
	display->VirtualPut(screenID);

	return (FALSE); // draw operation is not complete until children are drawn.
}

UI_WINDOW_OBJECT *ZAF_EDIT_GROUP::Add(UI_WINDOW_OBJECT *object)
{
	// Check for coordinate variations.
	object->RegionConvert(object->relative, FALSE); // make sure coordinate systems match.
	if (First())
	{
		// Check the left position of the group.
		if (relative.left >= object->relative.left)
		{
			int deltaX = relative.left - object->relative.left + GROUP_OFFSET;
			relative.left = object->relative.left - GROUP_OFFSET;
			// Adjust all the other group items.
			for (UI_WINDOW_OBJECT *temp = First(); temp; temp = temp->Next())
			{
				temp->relative.left += deltaX;
				temp->relative.right += deltaX;
			}
		}
		// Check the top position of the group.
		if (relative.top >= object->relative.top)
		{
			int deltaY = relative.top - object->relative.top + GROUP_OFFSET;
			relative.top = object->relative.top - GROUP_OFFSET;
			// Adjust all the other group items.
			for (UI_WINDOW_OBJECT *temp = First(); temp; temp = temp->Next())
			{
				temp->relative.top += deltaY;
				temp->relative.bottom += deltaY;
			}
		}
		// Check the right position of the group.
		if (relative.right <= object->relative.right)
			relative.right = object->relative.right + GROUP_OFFSET;
		// Check the bottom position of the group.
		if (relative.bottom <= object->relative.bottom)
			relative.bottom = object->relative.bottom + GROUP_OFFSET;
	}
	else
	{
		// Group assumes the object coordinates.
		relative = object->relative;
		relative += GROUP_OFFSET;
	}

	// Modify the object coordinates for placement in the group.
	object->relative.left -= relative.left;
	object->relative.top -= relative.top;
	object->relative.right -= relative.left;
	object->relative.bottom -= relative.top;
	return (UIW_WINDOW::Add(object));
}

EVENT_TYPE ZAF_EDIT_GROUP::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
	case L_BEGIN_ESCAPE:
	case L_CONTINUE_ESCAPE:
	case L_END_ESCAPE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_ADD_OBJECT:
		if (support.Index(event.windowObject) == -1 &&
			UIW_WINDOW::Index(event.windowObject) == -1)
			Add(event.windowObject);
		else
			ccode = UIW_WINDOW::Event(event);
		break;

	case S_SUBTRACT_OBJECT:
		if (support.Index(event.windowObject) != -1 ||
			UIW_WINDOW::Index(event.windowObject) != -1)
			Subtract(event.windowObject);
		else
			ccode = UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *ZAF_EDIT_GROUP::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		searchID = windowID[0] = ID_EDIT_GROUP;
		windowID[1] = ID_WINDOW;
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

UI_WINDOW_OBJECT *ZAF_EDIT_GROUP::Subtract(UI_WINDOW_OBJECT *object)
{
	// Modify the object coordinates for placement in the window.
	object->relative.left += relative.left;
	object->relative.top += relative.top;
	object->relative.right += relative.left;
	object->relative.bottom += relative.top;
	return (UIW_WINDOW::Subtract(object));
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

ZAF_EDIT_GROUP::ZAF_EDIT_GROUP(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS)
{
	// Set the associated edit flags.
	woStatus |= WOS_EDIT_MODE | WOS_GRAPHICS | WOS_OWNERDRAW;
	designerAdvancedFlags = woAdvancedFlags;
	woAdvancedFlags = WOAF_NO_FLAGS | WOAF_NO_SIZE;

	// Initialize the class information.
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	ZAF_EDIT_GROUP::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void ZAF_EDIT_GROUP::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the edit group information.
	UIW_WINDOW::Load(name, directory, file, objectTable, userTable);
}

UI_WINDOW_OBJECT *ZAF_EDIT_GROUP::New(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// ----- Persistent Constructor -----
	if (file)
		return (new ZAF_EDIT_GROUP(name, file, object, objectTable, userTable));

	return (new ZAF_EDIT_GROUP);
}

void ZAF_EDIT_GROUP::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the edit group information.
	UIW_WINDOW::Store(name, directory, file, objectTable, userTable);
}

// --------------------------------------------------------------------------

int ZAF_OBJECT_EDITOR::LeftTopCompare(void *data1, void *data2)
{
	UI_WINDOW_OBJECT *object1 = (UI_WINDOW_OBJECT *)data1;
	UI_WINDOW_OBJECT *object2 = (UI_WINDOW_OBJECT *)data2;
	if (object1->relative.top < object2->relative.top ||
		(object1->relative.top == object2->relative.top &&
		 object1->relative.left < object2->relative.left))
		return (-1);
	return (1);
}

void ZAF_OBJECT_EDITOR::ZAF_EDIT_GROUP_DataLoad(UI_WINDOW_OBJECT *)
{
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION;
	woFlagsAllowed = WOF_PIXEL | WOF_MINICELL | WOF_NON_FIELD_REGION;
	woAdvancedFlagsAllowed = 0;

	// Set the flags.
	SetFlag(FLAG_JUSTIFY_NONE, 1, 1);
}

int ZAF_OBJECT_EDITOR::ZAF_EDIT_GROUP_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	ZAF_EDIT_GROUP *object = (ZAF_EDIT_GROUP *)data;

	// Check for changed object information.
	int changed = FALSE;
	if (FlagSet(Get(FLAG_POSITION_SORT)->woStatus, WOS_SELECTED))
	{
		object->compareFunction = LeftTopCompare;
		UI_LIST temp;
		UI_WINDOW_OBJECT *field = NULL;
		for (field = object->First(); field; field = object->First())
		{
			object->Subtract(field);
			temp.Add(field);
		}
		for (field = (UI_WINDOW_OBJECT *)temp.First(); field; field = (UI_WINDOW_OBJECT *)temp.First())
		{
			temp.Subtract(field);
			object->Add(field);
		}
		object->compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION);
		changed |= TRUE;
	}
	// Left justify the group objects.
	if (FlagSet(Get(FLAG_JUSTIFY_LEFT)->woStatus, WOS_SELECTED))
	{
		for (UI_WINDOW_OBJECT *field = object->First(); field; field = field->Next())
		{
			field->relative.right -= field->relative.left - GROUP_OFFSET;
			field->relative.left = GROUP_OFFSET;
		}
		changed |= TRUE;
	}
	// Center justify the group objects.
	if (FlagSet(Get(FLAG_JUSTIFY_CENTER)->woStatus, WOS_SELECTED))
	{
		for (UI_WINDOW_OBJECT *field = object->First(); field; field = field->Next())
		{
			int offset = (object->relative.Width() - field->relative.Width()) / 2 - field->relative.left;
			field->relative.left += offset;
			field->relative.right += offset;
		}
		changed |= TRUE;
	}
	// Right justify the group objects.
	if (FlagSet(Get(FLAG_JUSTIFY_RIGHT)->woStatus, WOS_SELECTED))
	{
		int offset = object->relative.Width() - GROUP_OFFSET - 1;
		for (UI_WINDOW_OBJECT *field = object->First(); field; field = field->Next())
		{
			field->relative.left += offset - field->relative.right;
			field->relative.right = offset;
		}
		changed |= TRUE;
	}

	// Return the status.
	return (changed);
}
