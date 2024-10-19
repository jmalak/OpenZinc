//	Zinc Interface Library - Z_GMGR1.CPP
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

// ----- UI_ATTACHMENT ------------------------------------------------------

UI_ATTACHMENT::UI_ATTACHMENT(UI_WINDOW_OBJECT *_object,
	ATCF_FLAGS _atcFlags, int _offset) :
	UI_CONSTRAINT(_object), atcFlags(_atcFlags), offset(_offset),
	reference(ZIL_NULLP(UI_WINDOW_OBJECT)), refNumberID(0)
{
	UI_ATTACHMENT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UI_ATTACHMENT::UI_ATTACHMENT(UI_WINDOW_OBJECT *_object,
	UI_WINDOW_OBJECT *_reference, ATCF_FLAGS _atcFlags, int _offset) :
	UI_CONSTRAINT(_object), atcFlags(_atcFlags), reference(_reference),
	offset(_offset), refNumberID(0)
{
	UI_ATTACHMENT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UI_ATTACHMENT::~UI_ATTACHMENT()
{
}

void *UI_ATTACHMENT::Information(ZIL_INFO_REQUEST request, void *data,  ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		UI_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		searchID = ID_ATTACHMENT;
		if (refNumberID && manager && !reference)
			reference = (UI_WINDOW_OBJECT *)manager->Information(I_GET_NUMBERID_OBJECT, (void *)&refNumberID);
		break;

	case I_GET_REFERENCE_OBJECT:
		if (!data)
			return reference;
		*(UI_WINDOW_OBJECT **)data = reference;
		break;

	case I_SET_REFERENCE_OBJECT:
		reference = (UI_WINDOW_OBJECT *)data;
		break;

	case I_GET_OFFSET:
		*(int *)data = offset;
		break;

	case I_SET_OFFSET:
		offset = *(int *)data;
		break;

	case I_GET_FLAGS:
		if (data)
			*(ATCF_FLAGS *)data = atcFlags;
		else
			data = &atcFlags;
		break;

	case I_SET_FLAGS:
		atcFlags |= *(ATCF_FLAGS *)data;
		break;

	case I_CLEAR_FLAGS:
		atcFlags &= ~(*(ATCF_FLAGS *)data);
		break;

	default:
		data = UI_CONSTRAINT::Information(request, data, objectID);
		break;
	}

	return data;
}

void UI_ATTACHMENT::Modify(void)
{
	// Determine support variables.
	int width = object->relative.Width();
	int height = object->relative.Height();

	// Get the calculation width.
	int calcWidth = offset * object->display->cellWidth;
	int calcHeight = offset * object->display->cellHeight;
	if (FlagSet(object->woFlags, WOF_MINICELL))
	{
		calcWidth = (int)(calcWidth * object->display->miniNumeratorX / object->display->miniDenominatorX);
		calcHeight = (int)(calcHeight * object->display->miniNumeratorY / object->display->miniDenominatorY);
	}
	else if (FlagSet(object->woFlags, WOF_PIXEL))
		calcWidth = calcHeight = offset;

	// Get the reference region.
	UI_REGION refRegion;
	if (reference)
		refRegion = reference->relative;
	else if (object->parent)
	{
		// Use the parent clip region.
		object->parent->Information(I_GET_CLIPREGION, &refRegion);
		refRegion.right -= refRegion.left;
		refRegion.bottom -= refRegion.top;
		refRegion.left = refRegion.top = 0;
	}
	else
	{
		// Use the screen coordinates.
		refRegion.left = refRegion.top = 0;
		refRegion.right = UI_WINDOW_OBJECT::display->lines - 1;
		refRegion.bottom = UI_WINDOW_OBJECT::display->columns - 1;
	}

	// Determine the object region based on the type of connection.
	UI_REGION newRegion = object->relative;
	if (FlagSet(atcFlags, ATCF_LEFT))
	{
		if (FlagSet(atcFlags, ATCF_OPPOSITE))
			newRegion.left = refRegion.right + (reference ? calcWidth : -calcWidth);
		else
			newRegion.left = refRegion.left + calcWidth;
		if (!FlagSet(atcFlags, ATCF_STRETCH))
			newRegion.right = newRegion.left + width - 1;
	}
	if (FlagSet(atcFlags, ATCF_TOP))
	{
		if (FlagSet(atcFlags, ATCF_OPPOSITE))
			newRegion.top = refRegion.bottom + (reference ? calcHeight : -calcHeight);
		else
			newRegion.top = refRegion.top + calcHeight;
		if (!FlagSet(atcFlags, ATCF_STRETCH))
			newRegion.bottom = newRegion.top + height - 1;
	}
	if (FlagSet(atcFlags, ATCF_RIGHT))
	{
		if (FlagSet(atcFlags, ATCF_OPPOSITE))
			newRegion.right = refRegion.left - (reference ? calcWidth : -calcWidth);
		else
			newRegion.right = refRegion.right - calcWidth;
		if (!FlagSet(atcFlags, ATCF_STRETCH))
			newRegion.left = newRegion.right - width + 1;
	}
	if (FlagSet(atcFlags, ATCF_BOTTOM))
	{
		if (FlagSet(atcFlags, ATCF_OPPOSITE))
			newRegion.bottom = refRegion.top - (reference ? calcHeight : -calcHeight);
		else
			newRegion.bottom = refRegion.bottom - calcHeight;
		if (!FlagSet(atcFlags, ATCF_STRETCH))
			newRegion.top = newRegion.bottom - height + 1;
	}
	object->relative = newRegion;
	object->woStatus |= WOS_REDISPLAY;
#if defined(ZIL_NEXTSTEP)
	object->Event(S_MOVE);
#endif
}

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UI_ATTACHMENT::NewFunction(void) { return (UI_ATTACHMENT::New); }

UI_ATTACHMENT::UI_ATTACHMENT(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_CONSTRAINT(ZIL_NULLP(UI_WINDOW_OBJECT)), atcFlags(ATCF_NO_FLAGS),
	offset(0), reference(ZIL_NULLP(UI_WINDOW_OBJECT)), refNumberID(0)
{
	// Initialize the constraint information.
	UI_ATTACHMENT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UI_ATTACHMENT::Load(name, directory, file, objectTable, userTable);
}

void UI_ATTACHMENT::Load(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the constraint information.
	UI_CONSTRAINT::Load(name, directory, file, objectTable, userTable);
	file->Load(&refNumberID);
	file->Load(&atcFlags);
	ZIL_INT16 value; file->Load(&value); offset = value;
}
#endif

#if defined(ZIL_STORE)
void UI_ATTACHMENT::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the constraint information.
	UI_CONSTRAINT::Store(name, directory, file, objectTable, userTable);
	ZIL_NUMBERID refObjectID = reference ? reference->NumberID() : 0;
	file->Store(refObjectID);
	file->Store(atcFlags);
	ZIL_INT16 value = (ZIL_INT16)offset; file->Store(value);
}
#endif
