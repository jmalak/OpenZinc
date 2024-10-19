//	Zinc Interface Library - GEOMETRY.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "window.hpp"
#define USE_ZAF_GEOMETRY_MANAGER
#include "p_window.hpp"

// ----- ZAF_GEOMETRY_MANAGER -----------------------------------------------

ZAF_GEOMETRY_MANAGER::ZAF_GEOMETRY_MANAGER(void) :
	UIW_WINDOW(_className, ZAF_WINDOW_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), ZAF_WINDOW_EDITOR::_objectTable,
	ZAF_WINDOW_EDITOR::_userTable)
{
	// Initialize the constraint fields.
	field[FLD_SIDE_LEFT] = Get(FIELD_SIDE_LEFT);
	field[FLD_SIDE_TOP] = Get(FIELD_SIDE_TOP);
	field[FLD_SIDE_RIGHT] = Get(FIELD_SIDE_RIGHT);
	field[FLD_SIDE_BOTTOM] = Get(FIELD_SIDE_BOTTOM);
	field[FLD_WIDTH] = field[FLD_HEIGHT] = ZIL_NULLP(UI_WINDOW_OBJECT);
	field[FLD_TYPE_NONE] = Get(FIELD_TYPE_NONE);
	field[FLD_TYPE_ABSOLUTE] = Get(FIELD_TYPE_ABSOLUTE);
	field[FLD_TYPE_RELATIVE] = Get(FIELD_TYPE_RELATIVE);

	for (int i = FLD_SIDE_LEFT; i <= FLD_SIDE_BOTTOM; i++)
		field[i]->userFunction = UpdateSettings;

	// Initialize the general constraint options.
	anchorField = Get(FIELD_ANCHOR);
	offsetField = Get(FIELD_OFFSET);
	stretchField = Get(FIELD_STRETCH);
	oppositeField = Get(FIELD_OPPOSITE);
	vtCenterField = Get(FIELD_VT_CENTER);
	hzCenterField = Get(FIELD_HZ_CENTER);

	// Initialize the size constraints.
	widthField = Get(FIELD_WIDTH);
	minXField = Get(FIELD_MINX);
	maxXField = Get(FIELD_MAXX);
	heightField = Get(FIELD_HEIGHT);
	minYField = Get(FIELD_MINY);
	maxYField = Get(FIELD_MAXY);
}

ZAF_GEOMETRY_MANAGER::~ZAF_GEOMETRY_MANAGER(void)
{
}

void ZAF_GEOMETRY_MANAGER::UpdateCurrent(UI_WINDOW_OBJECT *requestor)
{
	// Check for a matching constraint field.
	for (int i = FLD_SIDE_LEFT; i <= FLD_SIDE_BOTTOM; i++)
		if (requestor == field[i]) // field matches a side field.
		{
			INFO_REQUEST request;
			WOS_STATUS selected = WOS_SELECTED;

			// Check for an up-to-date field.
			if (lastSide == field[i])
				return;
			UpdateNonCurrent(lastSide); // save the old information.
			lastSide = field[i]; // reset the last side.

			// side
			if (!FlagSet(field[i]->woStatus, WOS_SELECTED))
			{
				UI_EVENT addEvent(S_ADD_OBJECT);
				addEvent.windowObject = field[i];
				field[i]->parent->Event(addEvent);
				field[i]->woStatus |= WOS_SELECTED; // redundant for OS bugs.
			}

			// type
			request = (constraint[i].type == FLD_TYPE_NONE) ? I_SET_STATUS : I_CLEAR_STATUS;
			if ((request == I_SET_STATUS && !FlagSet(field[FLD_TYPE_NONE]->woStatus, selected)) ||
				(request != I_SET_STATUS && FlagSet(field[FLD_TYPE_NONE]->woStatus, selected)))
			{
				field[FLD_TYPE_NONE]->Information(request, &selected, ID_WINDOW_OBJECT);
				field[FLD_TYPE_NONE]->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
			}
			request = (constraint[i].type == FLD_TYPE_ABSOLUTE) ? I_SET_STATUS : I_CLEAR_STATUS;
			if ((request == I_SET_STATUS && !FlagSet(field[FLD_TYPE_ABSOLUTE]->woStatus, selected)) ||
				(request != I_SET_STATUS && FlagSet(field[FLD_TYPE_ABSOLUTE]->woStatus, selected)))
			{
				field[FLD_TYPE_ABSOLUTE]->Information(request, &selected, ID_WINDOW_OBJECT);
				field[FLD_TYPE_ABSOLUTE]->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
			}
			request = (constraint[i].type == FLD_TYPE_RELATIVE) ? I_SET_STATUS : I_CLEAR_STATUS;
			if ((request == I_SET_STATUS && !FlagSet(field[FLD_TYPE_RELATIVE]->woStatus, selected)) ||
				(request != I_SET_STATUS && FlagSet(field[FLD_TYPE_RELATIVE]->woStatus, selected)))
			{
				field[FLD_TYPE_RELATIVE]->Information(request, &selected, ID_WINDOW_OBJECT);
				field[FLD_TYPE_RELATIVE]->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
			}
			requestor = field[constraint[i].type]; // set for next section.
			// anchor
			anchorField->Information(I_SET_TEXT, constraint[i].anchor ? constraint[i].anchor->StringID() : editObject->parent->StringID());
			// offset
			offsetField->Information(I_SET_VALUE, &constraint[i].offset);
			// stretch
			request = constraint[i].stretch ? I_SET_STATUS : I_CLEAR_STATUS;
			if ((request == I_SET_STATUS && !FlagSet(stretchField->woStatus, selected)) ||
				(request != I_SET_STATUS && FlagSet(stretchField->woStatus, selected)))
			{
				stretchField->Information(request, &selected, ID_WINDOW_OBJECT);
				stretchField->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
			}
			// opposite
			request = constraint[i].opposite ? I_SET_STATUS : I_CLEAR_STATUS;
			if ((request == I_SET_STATUS && !FlagSet(oppositeField->woStatus, selected)) ||
				(request != I_SET_STATUS && FlagSet(oppositeField->woStatus, selected)))
			{
				oppositeField->Information(request, &selected, ID_WINDOW_OBJECT);
				oppositeField->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
			}
			// vt-center
			request = constraint[i].vtCenter ? I_SET_STATUS : I_CLEAR_STATUS;
			if ((request == I_SET_STATUS && !FlagSet(vtCenterField->woStatus, selected)) ||
				(request != I_SET_STATUS && FlagSet(vtCenterField->woStatus, selected)))
			{
				vtCenterField->Information(request, &selected, ID_WINDOW_OBJECT);
				vtCenterField->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
			}
			// hz-center
			request = constraint[i].hzCenter ? I_SET_STATUS : I_CLEAR_STATUS;
			if ((request == I_SET_STATUS && !FlagSet(hzCenterField->woStatus, selected)) ||
				(request != I_SET_STATUS && FlagSet(hzCenterField->woStatus, selected)))
			{
				hzCenterField->Information(request, &selected, ID_WINDOW_OBJECT);
				hzCenterField->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
			}
			break;
		}
}

void ZAF_GEOMETRY_MANAGER::UpdateNonCurrent(UI_WINDOW_OBJECT *requestor)
{
	// Check for a matching constraint field.
	for (int i = FLD_SIDE_LEFT; i <= FLD_SIDE_BOTTOM; i++)
		if (requestor == field[i]) // field matches a side field.
		{
			// type
			for (int j = FLD_TYPE_NONE; j <= FLD_TYPE_RELATIVE; j++)
				if (FlagSet(field[j]->woStatus, WOS_SELECTED))
					constraint[i].type = j;
			// anchor
			ZIL_ICHAR *anchor = _blankString;
			anchorField->Information(I_GET_TEXT, &anchor);
			constraint[i].anchor = ZIL_NULLP(UI_WINDOW_OBJECT);
			UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
			for (editObject->parent->Information(I_GET_FIRST, &object); object; object = object->Next())
				if (!stricmp(anchor, object->StringID()))
				{
					constraint[i].anchor = object;
					break;
				}
			// offset
			offsetField->Information(I_GET_VALUE, &constraint[i].offset);
			// stretch
			constraint[i].stretch = FlagSet(stretchField->woStatus, WOS_SELECTED) ? TRUE : FALSE;
			// opposite
			constraint[i].opposite = FlagSet(oppositeField->woStatus, WOS_SELECTED) ? TRUE : FALSE;
			// vt-center
			constraint[i].vtCenter = FlagSet(vtCenterField->woStatus, WOS_SELECTED) ? TRUE : FALSE;
			// hz-center
			constraint[i].hzCenter = FlagSet(hzCenterField->woStatus, WOS_SELECTED) ? TRUE : FALSE;
			break;
		}
}

EVENT_TYPE ZAF_GEOMETRY_MANAGER::UpdateSettings(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	// Notify the geometry manager of an update request.
	if (ccode == L_SELECT)
	{
		ZAF_GEOMETRY_MANAGER *manager = ZIL_NULLP(ZAF_GEOMETRY_MANAGER);
		object->Root()->Information(I_GET_GEOMETRY, &manager);
		manager->UpdateCurrent(object);
	}
	return (ccode);
}

ZIL_ICHAR ZAF_GEOMETRY_MANAGER::_className[] = { 'Z','A','F','_','G','E','O','M','E','T','R','Y','_','M','A','N','A','G','E','R',0 };

// ----- ZAF_OBJECT_EDITOR --------------------------------------------------

void ZAF_GEOMETRY_MANAGER::DataLoad(UI_WINDOW_OBJECT *object)
{
	// Initialize the edit object.
	editObject = object;
	if (!editObject || !editObject->parent)
		return;
	lastSide = ZIL_NULLP(UI_WINDOW_OBJECT);

	// Initialize the anchor field.
	anchorField->Information(I_DESTROY_LIST, ZIL_NULLP(void));
	UI_EVENT addEvent(S_ADD_OBJECT);
	// Add the parent object.
	addEvent.windowObject = new UIW_STRING(0, 0, 30, editObject->parent->StringID());
	anchorField->Event(addEvent);
	// Add the sibling objects.
	object = ZIL_NULLP(UI_WINDOW_OBJECT);
	for (editObject->parent->Information(I_GET_FIRST, &object); object; object = object->Next())
		if (object != editObject)
		{
			addEvent.windowObject = new UIW_STRING(2, 0, 30, object->StringID());
			anchorField->Event(addEvent);
		}

	// Reset the constraint information.
	for (int i = FLD_SIDE_LEFT; i <= FLD_HEIGHT; i++)
	{
		constraint[i].origin = ZIL_NULLP(UI_CONSTRAINT);
		constraint[i].type = FLD_TYPE_NONE;
		constraint[i].anchor = ZIL_NULLP(UI_WINDOW_OBJECT);
		constraint[i].offset = 0;
		constraint[i].stretch = FALSE;
		constraint[i].opposite = FALSE;
		constraint[i].vtCenter = FALSE;
		constraint[i].hzCenter = FALSE;
		constraint[i].min = 0;
		constraint[i].max = 0;
	}
	constraint[FLD_WIDTH].min = constraint[FLD_WIDTH].max =
		editObject->relative.Width() / display->cellWidth;
	constraint[FLD_HEIGHT].min = constraint[FLD_HEIGHT].max =
		(editObject->relative.Height() + display->preSpace + display->postSpace) / display->cellHeight;

	// Determine the load constraints.
	UI_WINDOW_OBJECT *gmgr = editObject->parent->Get(NUMID_GEOMETRY);
	UI_CONSTRAINT *element = ZIL_NULLP(UI_CONSTRAINT);
	if (gmgr)
		gmgr->Information(I_GET_FIRST, &element);
	for (; element; element = element->Next())
	{
		// Find the matching constraint.
		int i;
		if (element->object != editObject)
		 	continue; // no match.
		else if (element->searchID == ID_ATTACHMENT)
		{
			ATCF_FLAGS atcFlags;
			element->Information(I_GET_FLAGS, &atcFlags, ID_ATTACHMENT);
			if (FlagSet(atcFlags, ATCF_LEFT))
				i = FLD_SIDE_LEFT;
			else if (FlagSet(atcFlags, ATCF_TOP))
				i = FLD_SIDE_TOP;
			else if (FlagSet(atcFlags, ATCF_RIGHT))
				i = FLD_SIDE_RIGHT;
			else if (FlagSet(atcFlags, ATCF_BOTTOM))
				i = FLD_SIDE_BOTTOM;
			else
				continue; // no match.
			constraint[i].origin = element;
			constraint[i].type = FLD_TYPE_ABSOLUTE;
			element->Information(I_GET_REFERENCE_OBJECT, &constraint[i].anchor);
			element->Information(I_GET_OFFSET, &constraint[i].offset);
			constraint[i].stretch = FlagSet(atcFlags, ATCF_STRETCH) ? TRUE : FALSE;
			constraint[i].opposite = FlagSet(atcFlags, ATCF_OPPOSITE) ? TRUE : FALSE;
		}
		else if (element->searchID == ID_RELATIVE_CONSTRAINT)
		{
			RLCF_FLAGS rlcFlags;
			element->Information(I_GET_FLAGS, &rlcFlags, ID_RELATIVE_CONSTRAINT);
			if (FlagSet(rlcFlags, RLCF_LEFT))
				i = FLD_SIDE_LEFT;
			else if (FlagSet(rlcFlags, RLCF_TOP))
				i = FLD_SIDE_TOP;
			else if (FlagSet(rlcFlags, RLCF_RIGHT))
				i = FLD_SIDE_RIGHT;
			else if (FlagSet(rlcFlags, RLCF_BOTTOM))
				i = FLD_SIDE_BOTTOM;
			else
				continue; // no match.
			constraint[i].origin = element;
			constraint[i].type = FLD_TYPE_RELATIVE;
			int numerator = 1, denominator = 1;
			element->Information(I_GET_NUMERATOR, &numerator);
			element->Information(I_GET_DENOMINATOR, &denominator);
			constraint[i].offset = numerator * 100 / denominator;
			constraint[i].stretch = FlagSet(rlcFlags, ATCF_STRETCH) ? TRUE : FALSE;
			constraint[i].opposite = FlagSet(rlcFlags, ATCF_OPPOSITE) ? TRUE : FALSE;
			constraint[i].hzCenter = FlagSet(rlcFlags, RLCF_HORIZONTAL_CENTER) ? TRUE : FALSE;
			constraint[i].vtCenter = FlagSet(rlcFlags, RLCF_VERTICAL_CENTER) ? TRUE : FALSE;
		}
		else if (element->searchID == ID_DIMENSION_CONSTRAINT)
		{
			DNCF_FLAGS dncFlags;
			element->Information(I_GET_FLAGS, &dncFlags, ID_DIMENSION_CONSTRAINT);
			if (FlagSet(dncFlags, DNCF_WIDTH))
				i = FLD_WIDTH;
			else if (FlagSet(dncFlags, DNCF_HEIGHT))
				i = FLD_HEIGHT;
			else
				continue; // no match.
			constraint[i].origin = element;
			constraint[i].type = FLD_TYPE_DIMENSION;
			element->Information(I_GET_MINDIMENSION, &constraint[i].min);
			element->Information(I_GET_MAXDIMENSION, &constraint[i].max);
		}
		else
			continue; // no match.
	}

	// Initialize the constraint group.
	addEvent.windowObject = field[FLD_SIDE_LEFT];
	Get(FIELD_SIDE)->Event(addEvent);
	UpdateCurrent(field[FLD_SIDE_LEFT]);

	// Initialize the size group.
	WOS_STATUS selected = WOS_SELECTED;
	INFO_REQUEST request = (constraint[FLD_WIDTH].type == FLD_TYPE_DIMENSION) ? I_SET_STATUS : I_CLEAR_STATUS;
	if ((request == I_SET_STATUS && !FlagSet(widthField->woStatus, selected)) ||
		(request != I_SET_STATUS && FlagSet(widthField->woStatus, selected)))
	{
		widthField->Information(request, &selected, ID_WINDOW_OBJECT);
		widthField->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
	}
	minXField->Information(I_SET_VALUE, &constraint[FLD_WIDTH].min);
	maxXField->Information(I_SET_VALUE, &constraint[FLD_WIDTH].max);
	request = (constraint[FLD_HEIGHT].type == FLD_TYPE_DIMENSION) ? I_SET_STATUS : I_CLEAR_STATUS;
	if ((request == I_SET_STATUS && !FlagSet(heightField->woStatus, selected)) ||
		(request != I_SET_STATUS && FlagSet(heightField->woStatus, selected)))
	{
		heightField->Information(request, &selected, ID_WINDOW_OBJECT);
		heightField->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_WINDOW_OBJECT);
	}
	minYField->Information(I_SET_VALUE, &constraint[FLD_HEIGHT].min);
	maxYField->Information(I_SET_VALUE, &constraint[FLD_HEIGHT].max);
}

int ZAF_GEOMETRY_MANAGER::DataStore(UI_WINDOW_OBJECT *)
{
	UI_EVENT addEvent(S_ADD_OBJECT);
	UI_EVENT subEvent(S_SUBTRACT_OBJECT);

	// Uninitialize the constraint and size fields.
	UpdateNonCurrent(lastSide);
	constraint[FLD_WIDTH].type = FlagSet(widthField->woStatus, WOS_SELECTED) ? FLD_TYPE_DIMENSION : FLD_TYPE_NONE;
	constraint[FLD_HEIGHT].type = FlagSet(heightField->woStatus, WOS_SELECTED) ? FLD_TYPE_DIMENSION : FLD_TYPE_NONE;
	minXField->Information(I_GET_VALUE, &constraint[FLD_WIDTH].min);
	maxXField->Information(I_GET_VALUE, &constraint[FLD_WIDTH].max);
	minYField->Information(I_GET_VALUE, &constraint[FLD_HEIGHT].min);
	maxYField->Information(I_GET_VALUE, &constraint[FLD_HEIGHT].max);

	// Determine the constraints.
	UI_WINDOW_OBJECT *gmgr = editObject->parent->Get(NUMID_GEOMETRY);
	for (int i = FLD_SIDE_LEFT; i <= FLD_HEIGHT; i++)
	{
		UI_CONSTRAINT *element = constraint[i].origin;

		// Check to remove an old constraint.
		if (element &&
			(constraint[i].type == FLD_TYPE_NONE ||
			(constraint[i].type == FLD_TYPE_ABSOLUTE && element->searchID != ID_ATTACHMENT) ||
			(constraint[i].type == FLD_TYPE_RELATIVE && element->searchID != ID_RELATIVE_CONSTRAINT) ||
			(constraint[i].type == FLD_TYPE_DIMENSION && element->searchID != ID_DIMENSION_CONSTRAINT)))
		{
			woStatus |= WOS_CHANGED;
			subEvent.data = element;
			gmgr->Event(subEvent);
			delete element;
			element = ZIL_NULLP(UI_CONSTRAINT);
		}

		// Check for a new constraint.
		if (!element && constraint[i].type != FLD_TYPE_NONE)
		{
			woStatus |= WOS_CHANGED;
			if (!gmgr)
			{
				addEvent.windowObject = gmgr = new UI_GEOMETRY_MANAGER;
				editObject->parent->Event(addEvent);
			}
			if (constraint[i].type == FLD_TYPE_ABSOLUTE)
				element = new UI_ATTACHMENT(editObject, ATCF_NO_FLAGS);
			else if (constraint[i].type == FLD_TYPE_RELATIVE)
				element = new UI_RELATIVE_CONSTRAINT(editObject, RLCF_NO_FLAGS);
			else // FLD_TYPE_DIMENSION
				element = new UI_DIMENSION_CONSTRAINT(editObject, DNCF_NO_FLAGS);
			addEvent.data = element;
			gmgr->Event(addEvent);
		}

		// Update constraint parameters.
		if (!element)
			continue;
		else if (constraint[i].type == FLD_TYPE_ABSOLUTE)
		{
			ATCF_FLAGS newFlags = ATCF_NO_FLAGS;
			if (i == FLD_SIDE_LEFT)
				newFlags = ATCF_LEFT;
			else if (i == FLD_SIDE_TOP)
				newFlags = ATCF_TOP;
			else if (i == FLD_SIDE_RIGHT)
				newFlags = ATCF_RIGHT;
			else if (i == FLD_SIDE_BOTTOM)
				newFlags = ATCF_BOTTOM;
			if (constraint[i].stretch)
				newFlags |= ATCF_STRETCH;
			if (constraint[i].opposite)
				newFlags |= ATCF_OPPOSITE;

			ATCF_FLAGS oldFlags = ATCF_NO_FLAGS;
			element->Information(I_GET_FLAGS, &oldFlags, ID_ATTACHMENT);
			UI_WINDOW_OBJECT *anchor = ZIL_NULLP(UI_WINDOW_OBJECT);
			element->Information(I_GET_REFERENCE_OBJECT, &anchor);
			int offset = 0;
			element->Information(I_GET_OFFSET, &offset);

			if (oldFlags != newFlags ||
				anchor != constraint[i].anchor ||
				offset != constraint[i].offset)
			{
				woStatus |= WOS_CHANGED;
				element->Information(I_CLEAR_FLAGS, &oldFlags, ID_ATTACHMENT);
				element->Information(I_SET_FLAGS, &newFlags, ID_ATTACHMENT);
				element->Information(I_SET_REFERENCE_OBJECT, constraint[i].anchor);
				element->Information(I_SET_OFFSET, &constraint[i].offset);
			}
		}
		else if (constraint[i].type == FLD_TYPE_RELATIVE)
		{
			RLCF_FLAGS newFlags = RLCF_NO_FLAGS;
			if (i == FLD_SIDE_LEFT)
				newFlags = RLCF_LEFT;
			else if (i == FLD_SIDE_TOP)
				newFlags = RLCF_TOP;
			else if (i == FLD_SIDE_RIGHT)
				newFlags = RLCF_RIGHT;
			else if (i == FLD_SIDE_BOTTOM)
				newFlags = RLCF_BOTTOM;
			if (constraint[i].stretch)
				newFlags |= RLCF_STRETCH;
			if (constraint[i].opposite)
				newFlags |= RLCF_OPPOSITE;
			if (constraint[i].hzCenter)
				newFlags |= RLCF_HORIZONTAL_CENTER;
			if (constraint[i].vtCenter)
				newFlags |= RLCF_VERTICAL_CENTER;

			RLCF_FLAGS oldFlags = RLCF_NO_FLAGS;
			element->Information(I_GET_FLAGS, &oldFlags, ID_RELATIVE_CONSTRAINT);
			int numerator = 1; element->Information(I_GET_NUMERATOR, &numerator);
			int denominator = 100; element->Information(I_GET_DENOMINATOR, &denominator);

			if (oldFlags != newFlags ||
				numerator != constraint[i].offset ||
				denominator != 100)
			{
				woStatus |= WOS_CHANGED;
				element->Information(I_CLEAR_FLAGS, &oldFlags, ID_RELATIVE_CONSTRAINT);
				element->Information(I_SET_FLAGS, &newFlags, ID_RELATIVE_CONSTRAINT);
				numerator = constraint[i].offset, denominator = 100;
				element->Information(I_SET_NUMERATOR, &numerator);
				element->Information(I_SET_DENOMINATOR, &denominator);
			}
		}
		else if (constraint[i].type == FLD_TYPE_DIMENSION)
		{
			DNCF_FLAGS newFlags = DNCF_NO_FLAGS;
			if (i == FLD_WIDTH)
				newFlags = DNCF_WIDTH;
			else if (i == FLD_HEIGHT)
				newFlags = DNCF_HEIGHT;

			RLCF_FLAGS oldFlags = DNCF_NO_FLAGS;
			element->Information(I_GET_FLAGS, &oldFlags, ID_DIMENSION_CONSTRAINT);
			int oldMin; element->Information(I_GET_MINDIMENSION, &oldMin);
			int oldMax; element->Information(I_GET_MAXDIMENSION, &oldMax);

			if (oldFlags != newFlags ||
				oldMin != constraint[i].min ||
				oldMax != constraint[i].max)
			{
				woStatus |= WOS_CHANGED;
				element->Information(I_CLEAR_FLAGS, &oldFlags, ID_DIMENSION_CONSTRAINT);
				element->Information(I_SET_FLAGS, &newFlags, ID_DIMENSION_CONSTRAINT);
				element->Information(I_SET_MINDIMENSION, &constraint[i].min);
				element->Information(I_SET_MAXDIMENSION, &constraint[i].max);
			}
		}
	}

	// Return the changed status.
	return (FlagSet(woStatus, WOS_CHANGED) ? TRUE : FALSE);
}

