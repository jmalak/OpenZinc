//	Zinc Interface Library - D_GROUP.CPP
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

// ----- UIW_GROUP ----------------------------------------------------------

EVENT_TYPE UIW_GROUP::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	return UIW_WINDOW::DrawItem(event, ccode);
}

EVENT_TYPE UIW_GROUP::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_GROUP);
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
		clipList.Destroy();
		UIW_WINDOW::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		{
		int width = (text && text[0]) ?
			display->TextWidth(text, screenID, font) + 2 * display->cellWidth : 0;
		if (display->isText)
		{
			UI_WINDOW_OBJECT::Event(event); // Get the proper palette entry.
			display->VirtualGet(screenID, trueRegion);
#if defined(ZIL_3D_BORDER)
			UI_REGION region = trueRegion;
			DrawBorder(screenID, --region, TRUE, ccode);
#else
			UI_REGION overlap;
			trueRegion.Overlap(event.region, overlap);
			display->Rectangle(screenID, overlap, lastPalette, 0, TRUE, FALSE, &clip);
#endif
			ccode = UIW_WINDOW::Event(event); // Draw the sub-items.
			display->Text(screenID, trueRegion.left + 1, trueRegion.top, text,
				lastPalette, -1, FALSE, FALSE, &clip);
			display->VirtualPut(screenID);
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
			ccode = UIW_WINDOW::Event(event);
			lastPalette = FlagSet(woFlags, WOF_NON_SELECTABLE) ?
				LogicalPalette(ccode, ID_GROUP) : LogicalPalette(ccode, ID_PROMPT);
			int polygon[12];
			polygon[10] = trueRegion.left + 2 * display->cellWidth;
			polygon[0] = polygon[10] + width;
			polygon[1] = polygon[3] = polygon[9] = polygon[11] = trueRegion.top + display->cellHeight / 2;
			polygon[2] = polygon[4] = trueRegion.right;
			polygon[5] = polygon[7] = trueRegion.bottom;
			polygon[6] = polygon[8] = trueRegion.left;
			if (text && text[0])
				display->Text(screenID, polygon[10] + display->cellWidth,
					trueRegion.top + display->cellHeight / 4, text, lastPalette, -1,
					FALSE, FALSE, &clip);
#	if defined(ZIL_MSWINDOWS_STYLE)
			display->Polygon(screenID, 6, polygon, lastPalette, FALSE, FALSE, &clip);
#	elif defined(ZIL_MOTIF_STYLE) || defined(ZIL_OS2_STYLE)
			UI_PALETTE *darkShadow = &display->colorMap[DARKGRAY];
			display->Polygon(screenID, 6, polygon, darkShadow, FALSE, FALSE, &clip);
			UI_PALETTE *lightShadow = &display->colorMap[WHITE];
			display->Line(screenID, trueRegion.left + 1, polygon[1] + 1,
				polygon[10], polygon[1] + 1, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, polygon[0], polygon[1] + 1,
				trueRegion.right - 2, polygon[1] + 1, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, trueRegion.left + 1, polygon[1] + 1,
				trueRegion.left + 1, trueRegion.bottom - 2, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, trueRegion.right, polygon[1], trueRegion.right,
				trueRegion.bottom, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, trueRegion.left, trueRegion.bottom, trueRegion.right - 1,
				trueRegion.bottom, lightShadow, 1, FALSE, &clip);
#	endif
#endif
		}
		}	
		break;

	case S_REDISPLAY:
			if (screenID)
			{
				UIW_WINDOW::DrawItem(event, ccode);
				Event(UI_EVENT(RedisplayType(), 0, trueRegion));
			}
			break;

	case L_PREVIOUS:
	case L_NEXT:
			//??? Leave code for semismart tabbing of groups out until further
			//??? notice.  Possible improvement is to have part of the S_CURRENT
			//??? event tell which direction the focus was moving if it was the
			//??? result of a tab.
/* START BLOCK COMMENT
**			if (!HasWrappedData())
**			{
**				EVENT_TYPE tCode = ccode;
**				UI_WINDOW_OBJECT *object = Current();
**				ccode = UIW_WINDOW::Event(event);
**				if (tCode != ccode)
**				{
**					object = (tCode == L_NEXT) ? First() : Last();
**					while (object && (object->IsNonselectable() || object->IsNoncurrent()))
**						object = (tCode == L_NEXT) ? object->Next() : object->Previous();
**					if (object)
**					{
**						Current()->Event(S_NON_CURRENT);
**						SetCurrent(object);
**					}
**					ccode = S_UNKNOWN;
**				}
**			}
**			else
END BLOCK COMMENT */
			ccode = S_UNKNOWN;
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_GROUP::OSDataSet(void)
{
	if (screenID)
	{
		// Redraw background of previous string.
		display->VirtualGet(screenID, trueRegion);
		display->Rectangle(screenID, trueRegion.left, trueRegion.top,
			trueRegion.right, trueRegion.top + display->cellHeight, lastPalette, 0, TRUE, FALSE, &clip);
		display->VirtualPut(screenID);
		Event(UI_EVENT(S_REDISPLAY));
	}
}

void UIW_GROUP::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_GROUP && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}

