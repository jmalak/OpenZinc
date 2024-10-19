//	Zinc Interface Library - D_SBAR.CPP (STATUSBAR)
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

EVENT_TYPE UIW_STATUS_BAR::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Virtualize the display.
	UI_REGION region = trueRegion;
	display->VirtualGet(screenID, region);

	UI_PALETTE *palette = LogicalPalette(ccode, ID_STATUS_BAR);
	if (display->isText)
		display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &event.region);
	else
	{
		if (FlagSet(woFlags, WOF_BORDER))
			DrawBorder(screenID, region, FALSE, ccode);
		display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &event.region);
		woStatus &= ~WOS_REDISPLAY;
	}

	// Update the display.
	display->VirtualPut(screenID);
	return (FALSE);
}

EVENT_TYPE UIW_STATUS_BAR::Event(const UI_EVENT &event)
{
	const int STATUS_OFFSET = 4;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STATUS_BAR);
	switch (ccode)
	{
	case S_CREATE:
	case S_CHANGED:
		{
		UI_WINDOW_OBJECT *object;
		clipList.Destroy();
		// Compute the object region.
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
			trueRegion.top = (int)(trueRegion.bottom - height /
				(FlagSet(woFlags, WOF_MINICELL) ? display->miniDenominatorY : 1) + 1);
		else
		{
			// Increase size so don't get two-pixel border on left, right, bottom.
			if (FlagSet(woFlags, WOF_BORDER))
			{
				trueRegion.left--;
				trueRegion.right++;
				trueRegion.bottom++;
			}
			// Set top of status bar to be the programmer-specified height
			// plus a few pixels above and below.
			trueRegion.top = (int)(trueRegion.bottom - height * display->cellHeight /
				(FlagSet(woFlags, WOF_MINICELL) ? display->miniDenominatorY : 1) -
				2 * STATUS_OFFSET);
		}
		
		// Set clip region so status bar objects don't overwrite thick "border" of
		// status bar.
		UI_REGION region = trueRegion;
		if (display->isText)
			;
		else
		{
			region.left += display->cellWidth;
			region.right -= (display->cellWidth - 1);
			region.top += STATUS_OFFSET;
			region.bottom -= STATUS_OFFSET;
		}
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));

		// Compute the support object regions.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);
		}

		// Compute the remaining object regions.
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);
			object->woFlags |= WOF_VIEW_ONLY;
			object->woAdvancedFlags |= WOAF_NON_CURRENT;
		}
		}
		break;

	case S_SIZE:
		height = event.region.Height() / display->cellHeight;
		parent->Event(UI_EVENT(S_CHANGED));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_STATUS_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_STATUS_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION) && parent)
			parent->Event(event);
		else
			Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}
