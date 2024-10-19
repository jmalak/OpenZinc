//	Zinc Interface Library - M_TABLE1.CPP
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


#include <ui_win.hpp>


EVENT_TYPE UIW_TABLE_RECORD::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_REGION region = trueRegion;
	UI_PALETTE *lastPalette = parent->LogicalPalette(ccode);

	// Handle case of parent also owner draw.
	ZIL_SCREENID tScreenID = screenID;
	UI_WINDOW_OBJECT *tParent = parent;
	while (!tScreenID && tParent)
	{
		tScreenID = tParent->screenID;
		tParent = tParent->parent;
	}
	if (!tScreenID)
		return ccode;

	UIW_WINDOW::DrawItem(event, ccode);
	if (FlagSet(woFlags, WOF_BORDER) || FlagSet(woStatus, WOS_EDIT_MODE))
	{
		DrawBorder(tScreenID, region, FALSE, ccode);
	}

 	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
	{
		if (virtualRecord)
		{
			object->woStatus |= WOS_REDISPLAY;
			UI_EVENT updateEvent;
			updateEvent.type = S_DISPLAY_ACTIVE;
			updateEvent.region = event.region;
			object->Event(updateEvent);
		}
	}

	return (ccode);
}

EVENT_TYPE UIW_TABLE_RECORD::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TABLE_RECORD);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (!virtualRecord)
		{
			parent->Information(I_SET_TABLE_RECORD, this);
			parent->Information(I_SET_VIRTUAL_RECORD, VirtualRecord());
		}

		TBLF_FLAGS tblFlags;
		parent->Information(I_GET_FLAGS, &tblFlags);
		if (FlagSet(tblFlags, TBLF_GRID))
			woFlags |= WOF_BORDER;
		else
			woFlags &= ~WOF_BORDER;

		ccode = UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
		if (virtualRecord)
		{
			UI_WINDOW_OBJECT::Event(event);
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				object->UI_WINDOW_OBJECT::Event(event);
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;


	case S_SET_DATA:
		recordNum = (int)event.rawCode;
		data = event.data;
		if (userFunction)
		{
			UI_EVENT tEvent = event;
			(*userFunction)(this, tEvent, ccode);
		}
		break;

	case L_SELECT:
		UserFunction(event, L_SELECT);
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		{
		UI_EVENT tEvent = event;
		tEvent.rawCode = recordNum;
		tEvent.data = data;
		UIW_WINDOW::Event(tEvent);
		}
		break;

	case L_UP:
	case L_DOWN:
	case L_LEFT:
	case L_RIGHT:
	case L_PGUP:
	case L_PGDN:
		ccode = parent ? parent->Event(event) : S_UNKNOWN;
		break;

	case S_CHANGED:
		if (screenID)
			ccode = UIW_WINDOW::Event(event); // Don't pass for virtual records.
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}
	return(ccode);
}

void UIW_TABLE_RECORD::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);

	int offset = FlagSet(woFlags, WOF_BORDER) ? 2 : 1;
	object->trueRegion.left += offset;
	object->trueRegion.top += offset;
	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
	{
		object->trueRegion.right -= offset;
		object->trueRegion.bottom -= offset;
	}
	else
	{
 		object->trueRegion.top -= display->preSpace;
 		object->trueRegion.bottom -= display->preSpace;
		object->trueRegion.right += offset;
		object->trueRegion.bottom += offset;
	}
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TABLE_RECORD::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TABLE_RECORD && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
