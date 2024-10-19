//	Zinc Interface Library - Z_GMGR3.CPP
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

// ----- UI_RELATIVE_CONSTRAINT ----------------------------------------------

UI_RELATIVE_CONSTRAINT::UI_RELATIVE_CONSTRAINT(UI_WINDOW_OBJECT *_object,
	RLCF_FLAGS _rlcFlags, int _numerator, int _denominator, int _offset) :
	UI_CONSTRAINT(_object), rlcFlags(_rlcFlags), offset(_offset),
	numerator(_numerator), denominator(_denominator)
{
	UI_RELATIVE_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UI_RELATIVE_CONSTRAINT::~UI_RELATIVE_CONSTRAINT(void)
{
}

void *UI_RELATIVE_CONSTRAINT::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		UI_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		searchID = ID_RELATIVE_CONSTRAINT;
		break;

	case I_GET_NUMERATOR:
		*(int *)data = numerator;
		break;

	case I_SET_NUMERATOR:
		numerator = *(int *)data;
		break;

	case I_GET_DENOMINATOR:
		*(int *)data = denominator;
		break;

	case I_SET_DENOMINATOR:
		denominator = *(int *)data;
		break;

	case I_GET_FLAGS:
		if (data)
			*(RLCF_FLAGS *)data = rlcFlags;
		else
			data = &rlcFlags;
		break;

	case I_SET_FLAGS:
		rlcFlags |= *(RLCF_FLAGS *)data;
		break;

	case I_CLEAR_FLAGS:
		rlcFlags &= ~(*(RLCF_FLAGS *)data);
		break;

	default:
		data = UI_CONSTRAINT::Information(request, data, objectID);
		break;
	}

	return data;
}

void UI_RELATIVE_CONSTRAINT::Modify(void)
{
	// Determine support variables.
	int width = object->relative.Width();
	int height = object->relative.Height();
	if (height == 0)
		height = object->display->cellHeight;

	int calcWidth = object->display->cellWidth * offset;
	int calcHeight = object->display->cellHeight * offset;

	// Get the reference region.
	UI_REGION refRegion;
	object->parent->Information(I_GET_CLIPREGION, &refRegion);
	refRegion.right -= refRegion.left;
	refRegion.bottom -= refRegion.top;
	refRegion.left = refRegion.top = 0;

	// Determine the object region based on the type of connection.
	UI_REGION newRegion = object->relative;
	if (FlagSet(rlcFlags, RLCF_LEFT))
	{
		if (FlagSet(rlcFlags, RLCF_OPPOSITE))
			newRegion.left = refRegion.Width() -
				(int)((long)refRegion.Width() * numerator / denominator) + calcWidth;
		else
			newRegion.left = (int)((long)refRegion.Width() *
				numerator / denominator + calcWidth);
		if (!FlagSet(rlcFlags, RLCF_STRETCH))
			newRegion.right = newRegion.left + width - 1;
	}
	if (FlagSet(rlcFlags, RLCF_TOP))
	{
		if (FlagSet(rlcFlags, RLCF_OPPOSITE))
			newRegion.top = refRegion.Height() -
				(int)((long)refRegion.Height() * numerator / denominator) + calcHeight;
		else
			newRegion.top = (int)((long)refRegion.Height() *
				numerator / denominator + calcHeight);
		if (!FlagSet(rlcFlags, RLCF_STRETCH))
			newRegion.bottom = newRegion.top + height - 1;
	}
	if (FlagSet(rlcFlags, RLCF_RIGHT))
	{
		if (FlagSet(rlcFlags, RLCF_OPPOSITE))
			newRegion.right = refRegion.Width() -
				(int)((long)refRegion.Width() * numerator / denominator) + calcWidth;
		else
			newRegion.right = (int)((long)refRegion.Width() *
				numerator / denominator + calcWidth);
		if (!FlagSet(rlcFlags, RLCF_STRETCH))
			newRegion.left = newRegion.right - width + 1;
	}
	if (FlagSet(rlcFlags, RLCF_BOTTOM))
	{
		if (FlagSet(rlcFlags, RLCF_OPPOSITE))
			newRegion.bottom = refRegion.Height() -
				(int)((long)refRegion.Height() * numerator / denominator) + calcWidth;
		else
			newRegion.bottom = (int)((long)refRegion.Height() *
				numerator / denominator + calcWidth);
		if (!FlagSet(rlcFlags, RLCF_STRETCH))
			newRegion.top = newRegion.bottom - height + 1;
	}
	if (FlagSet(rlcFlags, RLCF_HORIZONTAL_CENTER))
	{
		// Don't use STRETCH or OPPOSITE with CENTER.
		newRegion.left = (int)(-width/2 +
			(long)refRegion.Width() * numerator / denominator + calcWidth);
		newRegion.right = newRegion.left + width - 1;
	}
	if (FlagSet(rlcFlags, RLCF_VERTICAL_CENTER))
	{
		// Don't use STRETCH or OPPOSITE with CENTER.
		newRegion.top = (int)(-height/2 +
			(long)refRegion.Height() * numerator / denominator + calcHeight);
		newRegion.bottom = newRegion.top + height - 1;
	}
	object->relative = newRegion;
	object->woStatus |= WOS_REDISPLAY;
#if defined(ZIL_NEXTSTEP)
	object->Event(S_MOVE);
#endif
}

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UI_RELATIVE_CONSTRAINT::NewFunction(void) { return (UI_RELATIVE_CONSTRAINT::New); }

UI_RELATIVE_CONSTRAINT::UI_RELATIVE_CONSTRAINT(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_CONSTRAINT(ZIL_NULLP(UI_WINDOW_OBJECT)), rlcFlags(RLCF_NO_FLAGS),
	offset(0), numerator(50), denominator(100)
{
	// Initialize the constraint information.
	UI_RELATIVE_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UI_RELATIVE_CONSTRAINT::Load(name, directory, file, objectTable, userTable);
}

void UI_RELATIVE_CONSTRAINT::Load(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the constraint information.
	UI_CONSTRAINT::Load(name, directory, file, objectTable, userTable);
	file->Load(&rlcFlags);
	ZIL_INT16 value; file->Load(&value); numerator = value;
	file->Load(&value); denominator = value;
	file->Load(&value); offset = value;
}
#endif

#if defined(ZIL_STORE)
void UI_RELATIVE_CONSTRAINT::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the constraint information.
	UI_CONSTRAINT::Store(name, directory, file, objectTable, userTable);
	file->Store(rlcFlags);
	ZIL_INT16 value = (ZIL_INT16)numerator; file->Store(value);
	value = (ZIL_INT16)denominator; file->Store(value);
	value = (ZIL_INT16)offset; file->Store(value);
}
#endif
