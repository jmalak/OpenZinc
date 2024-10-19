//	Zinc Interface Library - D_PLLDN.CPP
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

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_MENU::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_PULL_DOWN_MENU);
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		clipList.Destroy();
		{
		int left = 0, top = 0, height = display->cellHeight;
		if (display->isText)
		{
#if defined(ZIL_3D_BORDER)
			trueRegion.bottom = FlagSet(woFlags, WOF_BORDER) ? trueRegion.top + 2 * ZIL_BORDER_WIDTH : trueRegion.top;
#else
			woFlags &= ~WOF_BORDER;
			trueRegion.bottom = trueRegion.top;
#endif
			relative.right = trueRegion.Width();
			relative.bottom = trueRegion.Height();
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
			woFlags |= WOF_BORDER;
#if !defined(ZIL_OS2_STYLE)
			if (ccode != S_REDISPLAY)
				trueRegion.left--, trueRegion.right++, trueRegion.top--;
#endif
#if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
			height -= (display->preSpace + display->postSpace);
#endif
			trueRegion.bottom = trueRegion.top + height + 1;
			relative.right = trueRegion.right - trueRegion.left - 1;
			relative.bottom = trueRegion.bottom - trueRegion.top - 1;
#endif
		}
		for (object = First(); object; object = object->Next())
		{
			int width = object->relative.Width();
			if (left + width > relative.right)
			{
				left = 0;
				top += relative.bottom;
				trueRegion.bottom += height;
			}
			object->relative.left = left;
			object->relative.top = top;
			left += width;
			object->relative.right = left - 1;
			object->relative.bottom = top + height - 1;
		}

		// Compute the actual regions.
		UI_REGION region = trueRegion;
		if (FlagSet(woFlags, WOF_BORDER))
			region -= ZIL_BORDER_WIDTH;
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
		UI_EVENT tEvent = event;
		for (object = First(); object; object = object->Next())
			object->Event(tEvent);
		}
		break;

	case L_UP:
	case L_DOWN:
		if (current)
			ccode = Current()->Event(event);
		break;

	case L_LEFT:
	case L_RIGHT:
		ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_LEFT) ? L_PREVIOUS : L_NEXT));
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_BEGIN_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return UI_WINDOW_OBJECT::Event(event);
		if (trueRegion.Overlap(event.position))
			ccode = UIW_WINDOW::Event(event);

		// Check for a new window added to windowManager.
		if (!FlagSet(Root()->woStatus, WOS_CURRENT))
			break;
		{
		UI_EVENT sEvent;
		UI_WINDOW_OBJECT *popMenu, *oldCurrent = Current();

		// Process mouse drag events.
		do
		{
			eventManager->Get(sEvent);
			ccode = LogicalEvent(sEvent, ID_MENU);

			// Send all user messages to the window manager.
			if (sEvent.type > 9999)
				windowManager->Event(sEvent);
			else if (sEvent.type == E_MOUSE)
			{
				// First check to see if event overlaps any existing popups.
				for (popMenu = windowManager->First();	popMenu &&
					FlagSet(popMenu->woAdvancedFlags, WOAF_TEMPORARY); popMenu = popMenu->Next())
					if (popMenu->trueRegion.Overlap(sEvent.position))
					{
						popMenu->Event(sEvent);
						break;
					}

				// Check for event overlapping pulldown menu.
				if (trueRegion.Overlap(sEvent.position))
					for (object = First(); object; object = object->Next())
						if (object->trueRegion.Overlap(sEvent.position) && object != current &&
							!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
						{
							// Close existing popups.
							for (popMenu = windowManager->First();	popMenu &&
								FlagSet(popMenu->woAdvancedFlags, WOAF_TEMPORARY); popMenu = windowManager->First())
								windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
								// Select the new pulldown item and show its menu.
							Add(object);
							object->Event(UI_EVENT(L_DOWN));
						}
			}
		} while (ccode != L_END_SELECT);

		if (current && !Current()->Information(I_GET_FIRST, ZIL_NULLP(void)) &&
			current != oldCurrent)
			Current()->Event(UI_EVENT(L_SELECT));
		}
		break;

	default:
		// Try to find the hotkey menu item.
		if (ccode == E_KEY && !FlagSet(event.key.shiftState, S_ALT))
		{
			for (object = First(); object; object = object->Next())
				if (object->HotKey() == ToLower(event.key.value) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				{
					UIW_WINDOW::Add(object);
					return (object->Event(UI_EVENT(L_SELECT)));
				}
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PULL_DOWN_MENU::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (screenID && objectID == ID_PULL_DOWN_MENU)
		parent->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
	else
		UIW_WINDOW::Information(I_CHANGED_FLAGS, ZIL_NULLP(void), objectID);
}
