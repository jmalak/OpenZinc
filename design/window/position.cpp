//	Zinc Interface Library - POSITION.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_OBJ_POSITION
#include "p_window.hpp"

// ----- OBJ_POSITION ---------------------------------------------------------

void ZAF_OBJECT_EDITOR::OBJ_POSITION_DataLoad(UI_WINDOW_OBJECT *object)
{
	// Load the object data.
	// Set the border.
	SetFlag(FLAG_NO_BORDER,
		FlagSet(object->woFlags, WOF_BORDER), 0);
	SetFlag(FLAG_BORDER, object->woFlags, WOF_BORDER);
	if (!FlagSet(woFlagsAllowed, WOF_BORDER))
	{
		notebook->Get(GROUP_BORDER)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_NO_BORDER)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_BORDER)->woFlags |= WOF_NON_SELECTABLE;
	}
	else
	{
		notebook->Get(GROUP_BORDER)->woFlags &= ~WOF_NON_SELECTABLE;
		notebook->Get(FLAG_NO_BORDER)->woFlags &= ~WOF_NON_SELECTABLE;
		notebook->Get(FLAG_BORDER)->woFlags &= ~WOF_NON_SELECTABLE;
	}
	// Set the field region.
	SetFlag(FLAG_FIELD_REGION,
		FlagSet(object->woFlags, WOF_NON_FIELD_REGION), 0);
	SetFlag(FLAG_NON_FIELD_REGION, object->woFlags, WOF_NON_FIELD_REGION);
	if (!FlagSet(woFlagsAllowed, WOF_NON_FIELD_REGION))
	{
		notebook->Get(GROUP_REGION)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_FIELD_REGION)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_NON_FIELD_REGION)->woFlags |= WOF_NON_SELECTABLE;
	}
	else
	{
		notebook->Get(GROUP_REGION)->woFlags &= ~WOF_NON_SELECTABLE;
		notebook->Get(FLAG_FIELD_REGION)->woFlags &= ~WOF_NON_SELECTABLE;
		notebook->Get(FLAG_NON_FIELD_REGION)->woFlags &= ~WOF_NON_SELECTABLE;
	}
	// Set the justification.
	SetFlag(FLAG_LEFT_JUSTIFY,
		FlagSet(object->woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT), 0);
	SetFlag(FLAG_CENTER_JUSTIFY, object->woFlags, WOF_JUSTIFY_CENTER);
	SetFlag(FLAG_RIGHT_JUSTIFY, object->woFlags, WOF_JUSTIFY_RIGHT);
	if (!FlagSet(woFlagsAllowed, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))
	{
		notebook->Get(GROUP_ALIGNMENT)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_LEFT_JUSTIFY)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_CENTER_JUSTIFY)->woFlags |= WOF_NON_SELECTABLE;
		notebook->Get(FLAG_RIGHT_JUSTIFY)->woFlags |= WOF_NON_SELECTABLE;
	}
	else
	{
		notebook->Get(GROUP_ALIGNMENT)->woFlags &= ~WOF_NON_SELECTABLE;
		notebook->Get(FLAG_LEFT_JUSTIFY)->woFlags &= ~WOF_NON_SELECTABLE;
		notebook->Get(FLAG_CENTER_JUSTIFY)->woFlags &= ~WOF_NON_SELECTABLE;
		notebook->Get(FLAG_RIGHT_JUSTIFY)->woFlags &= ~WOF_NON_SELECTABLE;
	}
	// Set the cell, minicell and pixel coordinates.
	SetFlag(PIXEL_COORDINATE_FIELD, object->woFlags, WOF_PIXEL);
	if (!FlagSet(woFlagsAllowed, WOF_PIXEL))
		notebook->Get(PIXEL_COORDINATE_FIELD)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(PIXEL_COORDINATE_FIELD)->woFlags &= ~WOF_NON_SELECTABLE;
	SetFlag(MINICELL_COORDINATE_FIELD, object->woFlags, WOF_MINICELL);
	if (!FlagSet(woFlagsAllowed, WOF_MINICELL))
		notebook->Get(MINICELL_COORDINATE_FIELD)->woFlags |= WOF_NON_SELECTABLE;
	else
		notebook->Get(MINICELL_COORDINATE_FIELD)->woFlags &= ~WOF_NON_SELECTABLE;
	SetFlag(CELL_FIELD, object->woStatus, WOS_GRAPHICS);

	// Set proper group coordinates.
	UI_EVENT addEvent(S_ADD_OBJECT);
	UI_EVENT event(L_SELECT);
	UI_WINDOW_OBJECT *pixelField = notebook->Get(PIXEL_COORDINATE_FIELD);
	UI_WINDOW_OBJECT *minicellField = notebook->Get(MINICELL_COORDINATE_FIELD);
	UI_WINDOW_OBJECT *cellField = notebook->Get(CELL_FIELD);
	if (pixelField && FlagSet(pixelField->woStatus, WOS_SELECTED))
	{
		CheckCoordinates(pixelField, event, L_SELECT);
		addEvent.windowObject = pixelField;
		pixelField->parent->Event(addEvent); // make the pixel field current.
	}
	else if (minicellField && FlagSet(minicellField->woStatus, WOS_SELECTED))
	{
		CheckCoordinates(minicellField, event, L_SELECT);
		addEvent.windowObject = minicellField;
		minicellField->parent->Event(addEvent); // make the minicell field current.
	}
	else if (cellField)
	{
		CheckCoordinates(cellField, event, L_SELECT);
		addEvent.windowObject = cellField;
		cellField->parent->Event(addEvent); // make the cell field current.
	}

	// Get the cell, minicell or pixel coordinates.
	UI_REGION region;

	// Pixel
	NaturalCoordinate(editObject, region, WOF_PIXEL);
	SetValue(FIELD_PIXEL_COLUMN, region.left);
	SetValue(FIELD_PIXEL_LINE, region.top);
	SetValue(FIELD_PIXEL_WIDTH, region.Width());
	SetValue(FIELD_PIXEL_HEIGHT, region.Height());

	// Mini-cell
	NaturalCoordinate(editObject, region, WOF_MINICELL);
	SetValue(FIELD_MINICELL_COLUMN, region.left);
	SetValue(FIELD_MINICELL_LINE, region.top);
	SetValue(FIELD_MINICELL_WIDTH, region.Width());
	SetValue(FIELD_MINICELL_HEIGHT, region.Height());

	// Cell
	NaturalCoordinate(editObject, region, WOF_NO_FLAGS);
	SetValue(FIELD_CELL_COLUMN, region.left);
	SetValue(FIELD_CELL_LINE, region.top);
	SetValue(FIELD_CELL_WIDTH, region.Width());
	SetValue(FIELD_CELL_HEIGHT, region.Height());
}

int ZAF_OBJECT_EDITOR::OBJ_POSITION_DataStore(UI_WINDOW_OBJECT *object)
{
	// Store the object data.
	int changed = FALSE;
	// Get the border.
	changed |= GetFlag(FLAG_BORDER, &object->woFlags, WOF_BORDER);
	// Get the field region.
	changed |= GetFlag(FLAG_NON_FIELD_REGION, &object->woFlags, WOF_NON_FIELD_REGION);
	// Get the justification.
	changed |= GetFlag(FLAG_CENTER_JUSTIFY, &object->woFlags, WOF_JUSTIFY_CENTER);
	changed |= GetFlag(FLAG_RIGHT_JUSTIFY, &object->woFlags, WOF_JUSTIFY_RIGHT);

	int dummy;
	UI_WINDOW_OBJECT *field;
	int absolute = (object->parent && !FlagSet(object->woFlags, WOF_NON_FIELD_REGION)) ? FALSE : TRUE;
	// Get the cell, minicell or pixel coordinates.
	// Pixel
	field = notebook->Get(PIXEL_COORDINATE_FIELD);
	if (FlagSet(field->woStatus, WOS_SELECTED) &&
		(!FlagSet(object->woFlags, WOF_PIXEL) ||
		 GetValue(FIELD_PIXEL_COLUMN, &dummy) ||
		 GetValue(FIELD_PIXEL_LINE, &dummy) ||
		 GetValue(FIELD_PIXEL_WIDTH, &dummy) ||
		 GetValue(FIELD_PIXEL_HEIGHT, &dummy)))
	{
		UI_EVENT sizeEvent(S_SIZE, 0, object->relative);
		notebook->Get(FIELD_PIXEL_COLUMN)->Information(I_GET_VALUE, &sizeEvent.region.left);
		notebook->Get(FIELD_PIXEL_LINE)->Information(I_GET_VALUE, &sizeEvent.region.top);
		notebook->Get(FIELD_PIXEL_WIDTH)->Information(I_GET_VALUE, &sizeEvent.region.right);
		notebook->Get(FIELD_PIXEL_HEIGHT)->Information(I_GET_VALUE, &sizeEvent.region.bottom);
		sizeEvent.region.right += sizeEvent.region.left - 1;
		sizeEvent.region.bottom += sizeEvent.region.top - 1;
		// Check for valid object coordinates.
		if (sizeEvent.region.left >= 0 && sizeEvent.region.top >= 0 &&
			sizeEvent.region.right >= sizeEvent.region.left &&
			sizeEvent.region.bottom >= sizeEvent.region.top)
		{
			changed |= TRUE;
			object->woFlags &= ~WOF_MINICELL;
			object->woFlags |= WOF_PIXEL;
			object->woStatus |= WOS_GRAPHICS;
			// re-size the window.
			object->Event(sizeEvent);
		}
	}

	// Mini-cell
	field = notebook->Get(MINICELL_COORDINATE_FIELD);
	if (FlagSet(field->woStatus, WOS_SELECTED) &&
		(!FlagSet(object->woFlags, WOF_MINICELL) ||
		 GetValue(FIELD_MINICELL_COLUMN, &dummy) ||
		 GetValue(FIELD_MINICELL_LINE, &dummy) ||
		 GetValue(FIELD_MINICELL_WIDTH, &dummy) ||
		 GetValue(FIELD_MINICELL_HEIGHT, &dummy)))
	{
		UI_EVENT sizeEvent(S_SIZE, 0, object->relative);
		notebook->Get(FIELD_MINICELL_COLUMN)->Information(I_GET_VALUE, &sizeEvent.region.left);
		notebook->Get(FIELD_MINICELL_LINE)->Information(I_GET_VALUE, &sizeEvent.region.top);
		notebook->Get(FIELD_MINICELL_WIDTH)->Information(I_GET_VALUE, &sizeEvent.region.right);
		notebook->Get(FIELD_MINICELL_HEIGHT)->Information(I_GET_VALUE, &sizeEvent.region.bottom);
		sizeEvent.region.right += sizeEvent.region.left - 1;
		sizeEvent.region.bottom += sizeEvent.region.top - 1;
		// Check for valid object coordinates.
		if (sizeEvent.region.left >= 0 && sizeEvent.region.top >= 0 &&
			sizeEvent.region.right >= sizeEvent.region.left &&
			sizeEvent.region.bottom >= sizeEvent.region.top)
		{
			changed |= TRUE;
			object->woFlags &= ~WOF_PIXEL;
			object->woFlags |= WOF_MINICELL;
			object->woStatus &= ~WOS_GRAPHICS;
			object->RegionConvert(sizeEvent.region, absolute);
			// re-size the window.
			object->Event(sizeEvent);
		}
	}

	// Cell
	field = notebook->Get(CELL_FIELD);
	if (FlagSet(field->woStatus, WOS_SELECTED) &&
		(FlagSet(object->woFlags, WOF_PIXEL | WOF_MINICELL) ||
		 GetValue(FIELD_CELL_COLUMN, &dummy) ||
		 GetValue(FIELD_CELL_LINE, &dummy) ||
		 GetValue(FIELD_CELL_WIDTH, &dummy) ||
		 GetValue(FIELD_CELL_HEIGHT, &dummy)))
	{
		UI_EVENT sizeEvent(S_SIZE, 0, object->relative);
		notebook->Get(FIELD_CELL_COLUMN)->Information(I_GET_VALUE, &sizeEvent.region.left);
		notebook->Get(FIELD_CELL_LINE)->Information(I_GET_VALUE, &sizeEvent.region.top);
		notebook->Get(FIELD_CELL_WIDTH)->Information(I_GET_VALUE, &sizeEvent.region.right);
		notebook->Get(FIELD_CELL_HEIGHT)->Information(I_GET_VALUE, &sizeEvent.region.bottom);
		sizeEvent.region.right += sizeEvent.region.left - 1;
		sizeEvent.region.bottom += sizeEvent.region.top - 1;
		// Check for valid object coordinates.
		if (sizeEvent.region.left >= 0 && sizeEvent.region.top >= 0 &&
			sizeEvent.region.right >= sizeEvent.region.left &&
			sizeEvent.region.bottom >= sizeEvent.region.top)
		{
			changed |= TRUE;
			object->woFlags &= ~(WOF_MINICELL | WOF_PIXEL);
			object->woStatus &= ~WOS_GRAPHICS;
			object->RegionConvert(sizeEvent.region, absolute);
			// re-size the window.
			object->Event(sizeEvent);
		}
	}

	return (changed);
}

