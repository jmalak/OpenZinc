//	Zinc Interface Library - D_TABLE1.CPP
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
	WOF_FLAGS saveFlags = woFlags;
	woFlags &= ~WOF_BORDER;

	display->VirtualGet(screenID, region);

	UIW_WINDOW::DrawItem(event, ccode);
	woFlags = saveFlags;

	if (FlagSet(woFlags, WOF_BORDER) && !display->isText)
#if defined(ZIL_MSWINDOWS_STYLE)
	{
		if (!Inherited(ID_TABLE_HEADER))
		{
			display->Line(screenID, region.right, region.top, region.right, region.bottom,
				lastPalette, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.bottom, region.right, region.bottom,
				lastPalette, 1, FALSE, &clip);
			region.right--; region.bottom--;
		}
		else
			DrawBorder(screenID, region, FALSE, ccode);
	}
#else
		DrawBorder(screenID, region, FALSE, ccode);
#endif

 	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
	{
		if (virtualRecord)
		{
			object->woStatus |= WOS_REDISPLAY;
			UI_EVENT updateEvent;
			updateEvent.type = RedisplayType();
			updateEvent.region = event.region;
			object->Event(updateEvent);
		}
	}
	if (FlagSet(woStatus, WOS_EDIT_MODE) && !display->isText)
	{
		if (Inherited(ID_TABLE_HEADER))
			DrawBorder(screenID, region, FALSE, ccode);
		else
			display->Rectangle(screenID, region, lastPalette, 1, FALSE, FALSE, &clip);
	}
	else if (!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) &&
		FlagSet(woStatus, WOS_CURRENT) && !display->isText)
	{
#if !defined(ZIL_MSWINDOWS_STYLE)
		if (FlagSet(woFlags, WOF_BORDER))
			region.left--;
#endif
		DrawFocus(screenID, region, ccode);
	}

	display->VirtualPut(screenID);
	return (ccode);
}

EVENT_TYPE UIW_TABLE_RECORD::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TABLE_RECORD);
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		if (!virtualRecord)
		{
			parent->Information(I_SET_TABLE_RECORD, this);
			parent->Information(I_SET_VIRTUAL_RECORD, VirtualRecord());
		}
		if (parent->Inherited(ID_TABLE_HEADER))
		{
			woAdvancedFlags |= WOAF_NON_CURRENT;
			windowID[1] = ID_TABLE_HEADER;
			windowID[2] = ID_WINDOW;
		}
		TBLF_FLAGS tblFlags;
		parent->Information(I_GET_FLAGS, &tblFlags);
		if (FlagSet(tblFlags, TBLF_GRID))
			woFlags |= WOF_BORDER;
		else
			woFlags &= ~WOF_BORDER;
		ccode = UIW_WINDOW::Event(event);
		}
		break;

	case S_SET_DATA:
		recordNum = event.rawCode;
		data = event.data;
		if (userFunction)
		{
			UI_EVENT uEvent = event;
			(*userFunction)(this, uEvent, ccode);
		}
		break;

	case L_UP:
	case L_DOWN:
	case L_PGUP:
	case L_PGDN:
	case L_LEFT:
	case L_RIGHT:
		if (editMode && current)
			ccode = Current()->Event(event);
		else
			ccode = S_UNKNOWN;
		break;

	case L_NEXT:
	case L_PREVIOUS:
		{
		editMode = TRUE;
		EVENT_TYPE eventCode = ccode;
		ccode = UIW_WINDOW::Event(event);

		int records = 0;
		parent->Information(I_GET_RECORDS, &records);

		if (ccode != S_UNKNOWN ||
			(eventCode == L_NEXT &&	recordNum == records - 1) ||
			(eventCode == L_PREVIOUS && recordNum == 0))
			break;

		for (UI_WINDOW_OBJECT *object = eventCode == L_NEXT ? First() : Last();
			object; object = eventCode == L_NEXT ? object->Next() : object->Previous())
			if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
				!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
			{
				if (current && current != object)
					Current()->woStatus &= ~WOS_CURRENT;
				current = object;
				break;
			}
		}
		break;

	case L_BEGIN_SELECT:
		{
		UI_WINDOW_OBJECT *object = NULL;
		for (object = Last(); object; object = object->Previous())
			if (object->trueRegion.Overlap(event.position) &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
				!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
			{
				object->woStatus |= WOS_CURRENT;
				editMode = TRUE;
				break;
			}
		if (FlagSet(parent->woStatus, WOS_INTERNAL_ACTION))
		{
			if (!object)
				editMode = FALSE;
			break;
		}
 		ccode = UIW_WINDOW::Event(event);
		}		
		break;

	case L_SELECT:
		if (current && editMode && Current()->Inherited(ID_TEXT) &&
			!(event.modifiers & S_CTRL))
			return Current()->Event(event);
		{
		UI_EVENT tEvent = event;
		tEvent.rawCode = recordNum;
		tEvent.data = data;
		UserFunction(tEvent, L_SELECT);

		if (current)
		{
			editMode = !editMode;
			Current()->Event(UI_EVENT(editMode ? S_CURRENT : S_NON_CURRENT, 0, trueRegion));
			if (!editMode)
				eventManager->DeviceState(E_CURSOR, D_OFF);
		}
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		{
		ZIL_USER_FUNCTION saveFunction = userFunction;
		userFunction = ZIL_NULLF(ZIL_USER_FUNCTION);
		UIW_WINDOW::Event(event);
		userFunction = saveFunction;

		UI_EVENT tEvent = event;
		tEvent.rawCode = recordNum;
		tEvent.data = data;
		UserFunction(tEvent, ccode);

		if (ccode == S_CURRENT && !editMode && current &&
			FlagSet(Current()->woStatus, WOS_CURRENT))
		{
			Current()->Event(UI_EVENT(S_NON_CURRENT, 0, trueRegion));
			eventManager->DeviceState(E_CURSOR, D_OFF);
		}
		}
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

	int offset = 0;
	if (!display->isText)
		offset = FlagSet(woFlags, WOF_BORDER) ? 2 : 1;
	object->trueRegion.left += offset;
	object->trueRegion.top += offset;

	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
	{
		object->trueRegion.right -= offset;
		object->trueRegion.bottom -= offset;
	}
	else
	{
		if (!FlagSet(object->woFlags, WOF_MINICELL | WOF_PIXEL))
		{
 			object->trueRegion.top -= display->preSpace;
 			object->trueRegion.bottom -= display->preSpace;
		}
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
		event.type = S_REDISPLAY;
		Event(event);
	}
}
