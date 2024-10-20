//	Zinc Interface Library - W_TABLE1.CPP
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

EVENT_TYPE UIW_TABLE_RECORD::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	UI_REGION region = trueRegion;

	lastPalette = LogicalPalette(ccode);
	display->VirtualGet(screenID, region);

#if defined(ZIL_EDIT)
	if (FlagSet(woStatus, WOS_EDIT_MODE))
	{
		display->Rectangle(screenID, region, lastPalette, 1, TRUE);
		display->VirtualPut(screenID);
		return (ccode);
	}
#endif

	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (parent->Inherited(ID_TABLE_HEADER))
		display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	else
	{
		UI_PALETTE backgroundPalette = *lastPalette;
		backgroundPalette.colorForeground = backgroundPalette.colorBackground;
		display->Rectangle(screenID, region, &backgroundPalette, 1, FALSE);
		if (!virtualRecord && !FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			display->Rectangle(screenID, region, display->xorPalette, 1, FALSE, TRUE);
	}

	display->VirtualPut(screenID);
	return (ccode);
}

EVENT_TYPE UIW_TABLE_RECORD::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = S_UNKNOWN;

	//Process messages from system first.
	int processed = FALSE;
	if (event.type == E_MSWINDOWS)
	{
		processed = TRUE;
		UINT message = event.message.message;

		// Switch on the windows message.
		ccode = TRUE;
		switch (message)
		{
		case WM_SETFOCUS:
			if (editMode)
				ccode = UIW_WINDOW::Event(event);
			break;

		// Amibigous cases.
		case WM_KEYDOWN:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
			processed = FALSE;
			break;

		case WM_ERASEBKGND:
			return (UI_WINDOW_OBJECT::Event(event));

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	if (!processed)
	{
		ZIL_SCREENID frameID;
		Information(I_GET_FRAMEID, &frameID);

		// Switch on the event type.
		ccode = LogicalEvent(event, ID_TABLE_RECORD);
		switch (ccode)
		{
		case S_INITIALIZE:
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
			break;
	
		case S_CURRENT:
		case S_NON_CURRENT:
			{
			UI_EVENT tEvent = event;
			tEvent.rawCode = recordNum;
			tEvent.data = data;

			UIW_WINDOW::Event(tEvent);
			if (ccode == S_CURRENT)
			{
				UI_WINDOW_OBJECT *object = NULL;
				for (object = this;
					object && FlagSet(object->woStatus, WOS_CURRENT);
					object = object->parent)
					;
				if (!object)
					SetFocus(frameID);
			}
			}
			break;

		case S_SET_DATA:
			recordNum = event.rawCode;
			data = event.data;
			if (userFunction) {
				UI_EVENT tEvent = event;
				(*userFunction)(this, tEvent, ccode);
			}
			break;
	
		case L_BEGIN_SELECT:
			{
			UI_POSITION position =
				{ event.position.column - trueRegion.left, event.position.line - trueRegion.top };
			UI_WINDOW_OBJECT *object = NULL;
			for (object = First();
				object && !object->trueRegion.Overlap(position); object = object->Next())
				;
			editMode = FALSE;
			if (object)
			{
				Add(object);
				editMode = TRUE;
				UI_EVENT tEvent = event;
				tEvent.message.hwnd = object->screenID;
				tEvent.position = position;
				tEvent.message.lParam -= object->trueRegion.left | ((LPARAM)object->trueRegion.top << 16);
				ccode = object->Event(tEvent);
			}
#if defined(ZIL_EDIT)
			else if (FlagSet(woStatus, WOS_EDIT_MODE))
				ccode = UIW_WINDOW::Event(event);
#endif
			}
			break;
	
		case L_NEXT:
		case L_PREVIOUS:
			if ((current == last && ccode == L_NEXT) ||
				(current == first && ccode == L_PREVIOUS))
			{
				parent->Event(UI_EVENT(ccode));
				Add(ccode == L_NEXT ? First() : Last());
				editMode = TRUE;
			}
			else
				ccode = UIW_WINDOW::Event(UI_EVENT(ccode));
			break;

		case L_SELECT:
			{
			UI_EVENT tEvent = event;
			tEvent.rawCode = recordNum;
			tEvent.data = data;
			UserFunction(tEvent, L_SELECT);
			if (editMode)
			{
				if (current)
				{
					Current()->Event(UI_EVENT(S_NON_CURRENT));
					Current()->woStatus &= ~WOS_CURRENT;
				}
				editMode = FALSE;
				SetFocus(frameID);
			}
			else
			{
				editMode = TRUE;
				if (current)
					Add(Current());
				else if (first)
					Add(First());
				else
					editMode = FALSE;
			}
			Event(UI_EVENT(S_DISPLAY_ACTIVE, 0, trueRegion));
			}
			break;

		case L_PGUP:
		case L_PGDN:
			if (editMode)
			{
				ccode = parent->Event(event);
				Add(Current());
			}
			else
				ccode = parent->Event(event);
			break;

		case L_UP:
		case L_DOWN:
			if (editMode && event.type == E_MSWINDOWS)
				ccode = UIW_WINDOW::Event(event);
			else if (editMode)
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				Add(Current());
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		case L_LEFT:
		case L_RIGHT:
			if (editMode && (event.type == E_MSWINDOWS || first != last))
				ccode = UIW_WINDOW::Event(event);
			else if (editMode)
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				Add(Current());
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			break;
	
		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}
	return(ccode);
}

void UIW_TABLE_RECORD::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);

#if defined(ZIL_MSWINDOWS_CTL3D)
	int offset = FlagSet(woFlags, WOF_BORDER) ? 3 : 2;
#else
	int offset = FlagSet(woFlags, WOF_BORDER) ? 2 : 1;
#endif
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
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

