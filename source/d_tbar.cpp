//	Zinc Interface Library - D_TBAR.CPP
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


#include <stdio.h>
#include "ui_win.hpp"

// ----- UIW_TOOL_BAR -------------------------------------------------------

EVENT_TYPE UIW_TOOL_BAR::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TOOL_BAR);
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
		{
		clipList.Destroy();
		WOF_FLAGS _woFlags = woFlags;		// Get the remaining window region.
		woFlags |= WOF_NON_FIELD_REGION;
		UI_WINDOW_OBJECT::Event(event);
		woFlags = _woFlags;
		if (display->isText)
		{
			woFlags &= ~WOF_BORDER;
			trueRegion.bottom = trueRegion.top + relative.bottom - relative.top;
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
			woFlags |= WOF_BORDER;
			trueRegion.left--; trueRegion.right++;
			trueRegion.bottom = --trueRegion.top + relative.bottom - relative.top + 2 * ZIL_BORDER_WIDTH;
#endif
		}
		if (!FlagSet(wnFlags, WNF_NO_WRAP))
		{
			relative.left = 0;
			int height = display->cellHeight;
			for (object = First(); object; object = object->Next())
			{
				object->Event(event);
				if (height < object->relative.bottom - object->relative.top)
					height = object->relative.bottom - object->relative.top + 1;
			}
			trueRegion.bottom = trueRegion.top + height - 1;
			relative.right = relative.left + trueRegion.right - trueRegion.left + 1;
			relative.bottom = relative.top + trueRegion.bottom - trueRegion.top;
			int left = 0, top = 0;
			for (object = First(); object; object = object->Next())
			{
				int width = object->relative.right - object->relative.left + 1;
				if (left > 0 && left + width > relative.right)
				{
					left = 0;
					top += height;
					trueRegion.bottom += height;
				}
				object->relative.left = left;
				object->relative.top = top;
				left += width;
				object->relative.right = left - 1;
				object->relative.bottom = top + height - 1;
			}
#if !defined(ZIL_TEXT_ONLY)
			if (!display->isText)
				trueRegion.bottom += 2 * ZIL_BORDER_WIDTH;
#endif
		}
		UI_REGION region = trueRegion;
		if (FlagSet(woFlags, WOF_BORDER))
			region -= ZIL_BORDER_WIDTH;
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(UI_EVENT(S_CHANGED));
		}
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case S_ADD_OBJECT:
		{
		UI_REGION updateRegion = trueRegion;
		UIW_WINDOW::Event(event);
		parent->Event(UI_EVENT(S_CHANGED));
		if (screenID && trueRegion.bottom < updateRegion.bottom)
		{
			updateRegion.top = trueRegion.bottom;
			parent->Event(UI_EVENT(S_CURRENT, 0, updateRegion));
		}
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TOOL_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TOOL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
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
