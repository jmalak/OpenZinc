//	Zinc Interface Library - O_TABLE.CPP
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
	lastPalette = parent->LogicalPalette(ccode);

	/* unused parameters */ (void)event;

#if defined(ZIL_EDIT)
	if (FlagSet(woStatus, WOS_EDIT_MODE))
	{
		display->Rectangle(screenID, region, lastPalette, 1, TRUE);
		return (ccode);
	}
#endif
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (!virtualRecord && !parent->Inherited(ID_TABLE_HEADER))
		DrawFocus(screenID, region, ccode);
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

		UIW_WINDOW::Event(event);
		flStyle |= WS_CLIPCHILDREN;
		break;

	case S_CREATE:
		UIW_WINDOW::Event(event);
		break;

	case S_REGISTER_OBJECT:
		{
		UI_WINDOW_OBJECT::Event(event);
		ULONG color;
		if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
			ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
			WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(color), &color);
		}
		break;

	case S_SET_DATA:
		recordNum = event.rawCode;
		data = event.data;
		if (userFunction)
		{
			UI_EVENT tEvent = event;
			(*userFunction)(this, tEvent, ccode);
		}
		break;

	case L_SELECT:
		{
		if (editMode)
		{
			if (current)
				Current()->Event(UI_EVENT(S_NON_CURRENT));
			WinSetFocus(HWND_DESKTOP, screenID);
			editMode = FALSE;
		}
		else if (first)
		{
			if (!current)
			{
				for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
					if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
						!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
					{
						current = object;
						break;
					}
			}
			if (current)
			{
				Current()->Event(UI_EVENT(S_CURRENT));
				editMode = TRUE;
			}
		}

		UI_EVENT tEvent = event;
		tEvent.rawCode = recordNum;
		tEvent.data = data;
		UserFunction(tEvent, L_SELECT);
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		{
		UI_EVENT tEvent = event;
		tEvent.rawCode = recordNum;
		tEvent.data = data;
		UI_WINDOW_OBJECT::Event(tEvent);
		}
		break;

	case L_NEXT:
	case L_PREVIOUS:
		if (current)
		{
			UI_WINDOW_OBJECT *lastFirst = ccode == L_NEXT ? Last() : First();
			while (FlagSet(lastFirst->woFlags, WOF_NON_SELECTABLE) ||
				FlagSet(lastFirst->woAdvancedFlags, WOAF_NON_CURRENT))
					lastFirst = ccode == L_NEXT ? lastFirst->Previous() :
						lastFirst->Next();

			if (current == lastFirst)
			{
				if (parent->Event(event) == S_UNKNOWN)
					UIW_WINDOW::Event(event);
				else
				{
					Add(ccode == L_NEXT ? First() : Last());
					Current()->Event(UI_EVENT(S_CURRENT));
				}
			}
			else
				UIW_WINDOW::Event(event);
			editMode = TRUE;
		}
		break;

	case L_LEFT:
	case L_RIGHT:
	case L_UP:
	case L_DOWN:
		parent->Event(event);
		if (current && FlagSet(Current()->woStatus, WOS_CURRENT))
			Current()->Event(UI_EVENT(S_CURRENT));
		break;

	case L_BEGIN_SELECT:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UIW_WINDOW::Event(event));
#endif
		if (WinQueryFocus(HWND_DESKTOP) == screenID)
			editMode = FALSE;
		else
			editMode = TRUE;
		return (0);

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
	}
}
