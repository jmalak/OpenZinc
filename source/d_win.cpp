//	Zinc Interface Library - D_WIN.CPP
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


#define USE_RAW_KEYS
#include "ui_win.hpp"

// ----- UI_WINDOW_MANAGER --------------------------------------------------

UI_WINDOW_OBJECT *UI_WINDOW_MANAGER::Add(UI_WINDOW_OBJECT *object)
{
	// Check for a null object.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));

	// Remove any temporary objects if we have a non-temporary window.
	UI_EVENT event;
	UI_WINDOW_OBJECT *firstObject = First();
	if (!FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
	{
		while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
	}
	// Remove any temporary objects if the window is already in the list.
	else if (UI_LIST::Index(object) != -1)
	{
		while (firstObject && firstObject != object)
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
	}
	// See if the object is already the first on the list.
	if (object == First())
		return (object);

	// Find the maximum update region.
	UI_REGION updateRegion;
	updateRegion.left = updateRegion.top = 0x0FFF;
	updateRegion.right = updateRegion.bottom = 0x0000;
	for (UI_WINDOW_OBJECT *tObject = First(); tObject && tObject != object;
		tObject = tObject->Next())
		if (object->trueRegion.Overlap(tObject->trueRegion))
		{
			updateRegion.left = MinValue(updateRegion.left, tObject->trueRegion.left);
			updateRegion.top = MinValue(updateRegion.top, tObject->trueRegion.top);
			updateRegion.right = MaxValue(updateRegion.right, tObject->trueRegion.right);
			updateRegion.bottom = MaxValue(updateRegion.bottom, tObject->trueRegion.bottom);
		}

	// Initialize the object region.
	if (!object->screenID || FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
	{
		event.type = S_INITIALIZE;
		object->Event(event);
		event.type = S_CREATE;
		if (!object->screenID)
		{
			object->screenID = ++currentScreenID;
			object->Event(event);
		}
		else
			object->Event(event);

		// Make sure the temporary window is visible on the screen.
		if (FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
		{
			if (object->relative.left < 0)
			{
				object->relative.right -= object->relative.left;
				object->relative.left = 0;
			}
			else if (object->relative.right >= display->columns)
			{
				object->relative.left += (display->columns - 1 - object->relative.right);
				object->relative.right = display->columns - 1;
			}
			if (object->relative.bottom >= display->lines && object->parent)
			{
				int height = object->relative.bottom - object->relative.top;
				object->relative.bottom = object->parent->trueRegion.top - 1;
				object->relative.top = object->relative.bottom - height;
				if (object->relative.top < 0)
				{
					object->relative.top = 0;
					object->relative.bottom = height;
				}
			}
			event.type = S_CREATE;
			object->Event(event);
		}
		updateRegion = object->trueRegion;
	}

	// Bring the object to the front of the object queue.
	eventManager->DeviceState(E_CURSOR, D_OFF);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	if (object != firstObject)
	{
		if (UI_LIST::Index(object) != -1)
			UI_LIST::Subtract(object);
		else
			updateRegion = object->trueRegion;
		UI_LIST::Add(firstObject, object);
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top =
			event.region.right = event.region.bottom = -1;
		if (firstObject && !FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject->Event(event);
		event.type = S_REGION_DEFINE;
		object->Event(event);
	}
	event.type = S_CURRENT;
	event.region = updateRegion;
	object->Event(event);

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UI_WINDOW_MANAGER::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Check for an empty list.
	UI_WINDOW_OBJECT *firstObject = First();
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_MANAGER);
	if (!firstObject && ccode == L_EXIT)
		return (L_EXIT);
	else if (!firstObject && (ccode != L_EXIT_FUNCTION && ccode != S_ADD_OBJECT))
		return (S_NO_OBJECT);

	// Switch on the event type.
	switch (ccode)
	{
	case S_CASCADE:
		{
		UI_EVENT event;
		int column = 0, line = 0;
		display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
		// Recompute the object regions.
		for (object = Last(); object; object = object->Previous())
		{
			int width = object->relative.right - object->relative.left + 1;
			int height = object->relative.bottom - object->relative.top + 1;
			if (line + height >= display->lines)
				line = 0;
			object->relative.left = column;
			object->relative.top = line;
			object->relative.right = column + width - 1;
			object->relative.bottom = line + height - 1;
			event.type = S_CREATE;
			object->Event(event);
			event.type = S_REGION_DEFINE;
			object->Event(event);
			column += display->cellWidth * 3;
			line += display->cellHeight * 3 / 2;
		}
		}
		// Continue to S_REDISPLAY.
	case S_REDISPLAY:
		// Redisplay the objects on the screen.
		display->VirtualGet(ID_SCREEN, 0, 0, display->columns, display->lines);
		display->Rectangle(ID_SCREEN, trueRegion, display->backgroundPalette, 0, TRUE);
		for (object = Last(); object; object = object->Previous())
			object->Event(UI_EVENT((object == First()) ? S_CURRENT : S_DISPLAY_INACTIVE, 0, object->trueRegion));
		display->VirtualPut(ID_SCREEN);
		break;

#if !defined(ZIL_GRAPHICS_ONLY)
	case S_RESET_DISPLAY:
		{
		static int usesMouse;
		static UI_DEVICE *mouse;

		UI_WINDOW_OBJECT *tCurrent = ZIL_NULLP(UI_WINDOW_OBJECT);
		if (screenID != ID_SCREEN)
			for (tCurrent = First(); tCurrent && tCurrent->screenID != screenID; tCurrent = tCurrent->Next())
				;	// NOTHING

		while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
		if (!event.data)
		{
			for (mouse = eventManager->First(); mouse && mouse->type != E_MOUSE; mouse = mouse->Next())
				;
			if (mouse)
			{
				usesMouse = TRUE;
				eventManager->Subtract(mouse);
				delete mouse;
			}
			else
				usesMouse = FALSE;

			woStatus &= ~WOS_GRAPHICS;

			// Convert each object's coordinates from graphics.
			UIW_WINDOW::Event(event);

			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				object->Event(event);
			break;
		}
		display = (UI_DISPLAY *)event.data;
		if (usesMouse)
			eventManager->Add(new UID_MOUSE);
#if !defined(ZIL_TEXT_ONLY)
		if (!display->isText)
			woStatus |= WOS_GRAPHICS;
#endif
		UI_WINDOW_OBJECT *object1 = Last();
		UI_WINDOW_OBJECT *object2;
		first = current = last = ZIL_NULLP(UI_ELEMENT);
		currentScreenID = screenID = ID_SCREEN;
		while (object1)
		{
			object2 = object1;
			object1 = object2->Previous();
			UI_WINDOW_MANAGER::Add(object2);
		}
		if (tCurrent)
			screenID = tCurrent->screenID;
		eventManager->Put(UI_EVENT(S_REDISPLAY));
		}
		break;
#endif

	case S_CLOSE_TEMPORARY:
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			if(event.type != S_CLOSE_TEMPORARY && First() &&
				!FlagSet(First()->woAdvancedFlags, WOAF_TEMPORARY))
				First()->Event(UI_EVENT(L_CANCEL));
		}
		else if (event.type != S_CLOSE_TEMPORARY)
			ccode = firstObject->Event(event);
		break;

	case S_CLOSE:
	case L_MOVE:
	case L_SIZE:
	case L_MINIMIZE:
	case L_MAXIMIZE:
	case L_RESTORE:
		while (firstObject && firstObject->parent &&
			FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
		if (!firstObject)
			return (S_NO_OBJECT);
		else if (ccode != S_CLOSE)
			return (firstObject->Event(event));
		else if (screenID != firstObject->screenID)
		{
			// Make sure we can remove the window from the display.
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_LOCKED))
			{
				UI_WINDOW_MANAGER::Subtract(firstObject);
				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
					delete firstObject;
				if (!First())
					eventManager->Put(UI_EVENT(S_NO_OBJECT));
			}
			break;
		}
		// Continue to L_EXIT_FUNCTION.
	case L_EXIT_FUNCTION:
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL))
			break;
		else if (exitFunction && (*exitFunction)(display, eventManager, this) != L_EXIT)
			break;
		// Continue to L_EXIT.
	case L_EXIT:
		ccode = L_EXIT;
		break;

	case L_NEXT_WINDOW:
		while (firstObject && FlagsSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject = firstObject->Next();
		if (firstObject && !FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL) &&
			!FlagSet(Last()->woFlags, WOF_NON_SELECTABLE) && firstObject != Last())
			UI_WINDOW_MANAGER::Add(Last());
		break;

	case L_CONTINUE_MOVE_DRAG:
	case L_CONTINUE_COPY_DRAG:
	case L_END_MOVE_DRAG:
	case L_END_COPY_DRAG:
		if (dragObject)
		{
			for (object = First(); object; object = object->Next())
				if (object->trueRegion.Overlap(event.position))
				{
					if (FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL) &&
						firstObject != object)
						eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
					else
					{
						if (object != firstObject &&
							eventManager->DeviceState(E_MOUSE, D_STATE) != DM_CANCEL &&
							(ccode == L_END_MOVE_DRAG || ccode == L_END_COPY_DRAG))
							Add(object);
						object->Event(event);
					}
					break;
				}
			if (ccode == L_END_MOVE_DRAG || ccode == L_END_COPY_DRAG)
			{
				dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				if (ccode == L_END_MOVE_DRAG)
					firstObject->Event(event);
				Event(UI_EVENT(L_VIEW, 0, event.position));
			}
			else if (!object)
				eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
  		}
		else
			ccode = firstObject ? firstObject->Event(event) : S_UNKNOWN;
		break;

	case L_BEGIN_SELECT:
	case L_VIEW:
		{
		EVENT_TYPE tCode = S_UNKNOWN;
		if (ccode == L_BEGIN_SELECT && firstObject &&
			firstObject->trueRegion.Overlap(event.position))
			return firstObject->Event(event);

		while (firstObject && tCode == S_UNKNOWN)
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			if (!FlagSet(firstObject->woFlags, WOF_NON_SELECTABLE) &&
				firstObject->trueRegion.Overlap(event.position))
			{
				// Bring the new object to the front.
				if (firstObject != first && ccode != L_VIEW)
				{
					// Check for an overlapping parent.
					if (!FlagSet(firstObject->woStatus, WOS_MINIMIZED) &&
						FlagSet(First()->woAdvancedFlags, WOAF_TEMPORARY) &&
						First()->Next() == firstObject)
					{
						for (UI_WINDOW_OBJECT *object = First()->parent; object;
							object = object->parent)
							if (object == firstObject || !object->trueRegion.Overlap(event.position))
								break;
						// Temporary object is direct child of firstObject.
						if (object == firstObject)
							return (firstObject->Event(event));
					}
					UI_WINDOW_MANAGER::Add(firstObject);
				}
				tCode = firstObject->Event(event);
			}
			else if (FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL))
			{
				if (ccode != L_VIEW)
					UI_ERROR_SYSTEM::Beep();
				break;
			}
			firstObject = nextObject;
		}
		if (tCode == S_UNKNOWN)
		{
			if (ccode == L_BEGIN_SELECT)
				while (First() && FlagSet(First()->woAdvancedFlags, WOAF_TEMPORARY) &&
					!FlagSet(First()->woAdvancedFlags, WOAF_LOCKED))
					Event(UI_EVENT(S_CLOSE_TEMPORARY));
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}

		ccode = tCode;
		}
		break;

	case L_HELP:
		// MapEvent returns E_KEY if help context is not general.
		if (UI_EVENT_MAP::MapEvent(eventMapTable, event) != L_HELP)
		{
			if (helpSystem)
				helpSystem->DisplayHelp(windowManager, NO_HELP_CONTEXT);
			break;
		}
		else
			ccode = firstObject->Event(UI_EVENT(ccode, 0));
		break;

	case S_ALT_KEY:
		{
		int closedMenu = FALSE;
		while (firstObject && firstObject->Inherited(ID_POP_UP_MENU) &&
			FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY) &&
			!FlagSet(firstObject->woAdvancedFlags, WOAF_LOCKED))
		{
			Event(UI_EVENT(S_CLOSE_TEMPORARY));
			firstObject = First();
			closedMenu = TRUE;
		}
		if (closedMenu && firstObject)
			return firstObject->Event(UI_EVENT(L_CANCEL));
		else if (firstObject)
			return firstObject->Event(event);
		}

		// Continue.

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		UIW_WINDOW::Event(event);
		break;

	case S_OPEN_DOCUMENT:
	case S_PRINT_DOCUMENT:
		// If these are not processed, we must delete event.text.
		ccode = firstObject->Event(event);
		if (ccode == S_UNKNOWN && event.text)
			delete event.text;
		break;

	default:
		ccode = firstObject->Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

UI_WINDOW_OBJECT *UI_WINDOW_MANAGER::Subtract(UI_WINDOW_OBJECT *object)
{
	// Make sure there is an object to subtract.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
	object->woStatus &= ~WOS_CURRENT;

	// Update the windows according to their new list positions.
	UI_EVENT event;
	UI_WINDOW_OBJECT *nextObject = object->Next();
	UI_WINDOW_OBJECT *newCurrent = First();
	if (newCurrent == object)
		newCurrent = newCurrent->Next();
	if (UI_LIST::Index(object) == -1)
		return (object);
	else
	{
		// De-activate the specified object.
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top =
			event.region.right = event.region.bottom = -1;
		object->Event(event);		// Call any user-functions.

		event.type = S_REGION_DEFINE;
		event.region = object->trueRegion;
#if defined(ZIL_3D_BORDER)
		if (FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
		{
			event.region.right++;
			event.region.bottom++;
		}
#endif
		// Reset the device states.
		eventManager->DeviceState(E_CURSOR, D_OFF);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);

		// Re-define the window regions on the screen.
		display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
		UI_WINDOW_OBJECT *tObject = NULL;
		for (tObject = Last(); tObject; tObject = tObject->Previous())
			if (tObject != object)
				tObject->Event(event);

		// Refresh the screen and windows affected by the object removal.
		display->VirtualGet(ID_SCREEN, 0, 0, display->columns, display->lines);
		display->Rectangle(ID_SCREEN, event.region, display->backgroundPalette, 0, TRUE);
		for (tObject = Last(); tObject; tObject = tObject->Previous())
		{
			int updateRegion = FALSE;
			if (tObject == object)
				;
			else if (tObject == newCurrent)
			{
				event.type = S_CURRENT;
				tObject->Event(event);
				updateRegion = TRUE;
				break;
			}
			else if (tObject->trueRegion.Overlap(object->trueRegion))
			{
				if (FlagSet(tObject->woAdvancedFlags, WOAF_TEMPORARY) ||
					(tObject->Previous() && FlagSet(tObject->Previous()->woAdvancedFlags, WOAF_TEMPORARY)))
					event.type = S_CURRENT;
				else
					event.type = S_DISPLAY_INACTIVE;
				tObject->Event(event);
				updateRegion = TRUE;
			}
			if (updateRegion && display->isText) // Fix for shadow border problem.
			{
#if !defined(ZIL_GRAPHICS_ONLY)
				event.region.left = MinValue(event.region.left, tObject->trueRegion.left);
				event.region.top = MinValue(event.region.top, tObject->trueRegion.top);
				event.region.right = MaxValue(event.region.right, tObject->trueRegion.right);
				event.region.bottom = MaxValue(event.region.bottom, tObject->trueRegion.bottom);
				updateRegion = FALSE;
#endif
			}
		}
		display->VirtualPut(ID_SCREEN);

  		// Remove the window from the manager's list.
		UI_LIST::Subtract(object);
		event.type = S_DEINITIALIZE;
		object->Event(event);

	}

	// Return a pointer to the next object.
	return (nextObject);
}
