//	Zinc Interface Library - Z_GMGR2.CPP
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

// ----- UI_DIMENSION_CONSTRAINT ----------------------------------------------

UI_DIMENSION_CONSTRAINT::UI_DIMENSION_CONSTRAINT(UI_WINDOW_OBJECT *_object,
	DNCF_FLAGS _dncFlags, int _minimum, int _maximum) :
	UI_CONSTRAINT(_object), dncFlags(_dncFlags), maximum(_maximum), minimum(_minimum)
{
	UI_DIMENSION_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UI_DIMENSION_CONSTRAINT::~UI_DIMENSION_CONSTRAINT(void)
{
}

void *UI_DIMENSION_CONSTRAINT::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		UI_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		searchID = ID_DIMENSION_CONSTRAINT;
		break;

	case I_GET_MAXDIMENSION:
		*(int *)data = maximum;
		break;

	case I_SET_MAXDIMENSION:
		maximum = *(int *)data;
		break;

	case I_GET_MINDIMENSION:
		*(int *)data = minimum;
		break;

	case I_SET_MINDIMENSION:
		minimum = *(int *)data;
		break;

	case I_GET_FLAGS:
		if (data)
			*(DNCF_FLAGS *)data = dncFlags;
		else
			data = &dncFlags;
		break;

	case I_SET_FLAGS:
		dncFlags |= *(DNCF_FLAGS *)data;
		break;

	case I_CLEAR_FLAGS:
		dncFlags &= ~(*(DNCF_FLAGS *)data);
		break;

	default:
		data = UI_CONSTRAINT::Information(request, data, objectID);
		break;
	}

	return data;
}

void UI_DIMENSION_CONSTRAINT::Modify(void)
{
	if (FlagSet(dncFlags, DNCF_WIDTH))
	{
		int min = minimum * object->display->cellWidth;
		int max = maximum * object->display->cellWidth;
		if (FlagSet(object->woFlags, WOF_MINICELL))
		{
			min = (int)(min * object->display->miniNumeratorX / object->display->miniDenominatorX);
			max = (int)(max * object->display->miniNumeratorX / object->display->miniDenominatorX);
		}
		else if (FlagSet(object->woFlags, WOF_PIXEL))
		{
			min = minimum;
			max = maximum;
		}
		if (min && object->relative.Width() < min)
			object->relative.right = object->relative.left + min;
		if (max && object->relative.Width() > max)
			object->relative.right = object->relative.left + max;
	}
	if (FlagSet(dncFlags, DNCF_HEIGHT))
	{
		int min = minimum * object->display->cellHeight;
		int max = maximum * object->display->cellHeight;
		if (FlagSet(object->woFlags, WOF_MINICELL))
		{
			min = (int)(min * object->display->miniNumeratorY / object->display->miniDenominatorY);
			max = (int)(max * object->display->miniNumeratorY / object->display->miniDenominatorY);
		}
		else if (FlagSet(object->woFlags, WOF_PIXEL))
		{
			min = minimum;
			max = maximum;
		}
		if (min && object->relative.Height() < min)
			object->relative.bottom = object->relative.top + min;
		if (max && object->relative.Height() > max)
			object->relative.bottom = object->relative.top + max;
	}
	object->woStatus |= WOS_REDISPLAY;
}

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UI_DIMENSION_CONSTRAINT::NewFunction(void) { return (UI_DIMENSION_CONSTRAINT::New); }

UI_DIMENSION_CONSTRAINT::UI_DIMENSION_CONSTRAINT(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_CONSTRAINT(ZIL_NULLP(UI_WINDOW_OBJECT)), dncFlags(DNCF_NO_FLAGS),
	maximum(0), minimum(0)
{
	// Initialize the constraint information.
	UI_DIMENSION_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UI_DIMENSION_CONSTRAINT::Load(name, directory, file, objectTable, userTable);
}

void UI_DIMENSION_CONSTRAINT::Load(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the constraint information.
	UI_CONSTRAINT::Load(name, directory, file, objectTable, userTable);
	file->Load(&dncFlags);
	ZIL_INT16 value; file->Load(&value); maximum = value;
	file->Load(&value); minimum = value;
}
#endif

#if defined(ZIL_STORE)
void UI_DIMENSION_CONSTRAINT::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the constraint information.
	UI_CONSTRAINT::Store(name, directory, file, objectTable, userTable);
	file->Store(dncFlags);
	ZIL_INT16 value = (ZIL_INT16)maximum; file->Store(value);
	value = (ZIL_INT16)minimum; file->Store(value);
}
#endif
