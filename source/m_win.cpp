//	Zinc Interface Library - M_WIN.CPP
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

// ----- UI_WINDOW_MANAGER --------------------------------------------------

UI_WINDOW_OBJECT *UI_WINDOW_MANAGER::Add(UI_WINDOW_OBJECT *object)
{
	// Check for a null object.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));

	// Remove any temporary objects if we have a non-temporary window.
	UI_EVENT event;
	UI_WINDOW_OBJECT *firstObject = First();

	// Remove all temporary windows so things work better with Implicit Focus.
	while (firstObject)
	{
		UI_WINDOW_OBJECT *nextObject = firstObject->Next();
		if (!FlagSet(firstObject->woStatus, WOS_WINDOWS_ACTION) &&
			FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY) &&
			firstObject != object)
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
		}
		firstObject = nextObject;
	}
	firstObject = First();

	// See if the object is already the first on the list.
	if (object == First())
		return (object);

	// Find the maximum update region.
	UI_REGION updateRegion;
	updateRegion.left = 0x0FFF;
	updateRegion.top = 0x0FFF;
	updateRegion.right = 0x0000;
	updateRegion.bottom = 0x0000;
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
	if (!object->screenID)
	{
		event.type = S_INITIALIZE;
		object->Event(event);
		event.type = S_CREATE;
		object->Event(event);
	}

	// Bring the object to the front of the object queue.
	if (object != firstObject && UI_LIST::Index(object) != -1)
	{
		if (UI_LIST::Index(object) != -1)
			UI_LIST::Subtract(object);
		else
			updateRegion = object->trueRegion;
		event.type = S_REGION_DEFINE;
		object->Event(event);
		UI_LIST::Add(firstObject, object);
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top =
			event.region.right = event.region.bottom = -1;
		if (firstObject && !FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject->Event(event);

		if (screenID && !FlagSet(object->woStatus, WOS_INTERNAL_ACTION))
		{
			// Pop the window to the top of the window stack.
			XRaiseWindow(display->xDisplay, XtWindow(object->shell));
		}
		event.type = S_CURRENT;
		event.region = updateRegion;
		object->Event(event);
	}
	else
	{
		UI_LIST::Add(object);
	}
		
	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UI_WINDOW_MANAGER::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;
	UI_WINDOW_OBJECT *firstObject = First();
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_MANAGER);
	if (!firstObject && ccode != S_ADD_OBJECT)
		return (ccode);

	if (event.type == E_MOTIF)
	{
		XtDispatchEvent((XEvent*)&event.message);
		return(ccode);
	}

	switch (ccode)
	{
	case S_CLOSE:
	case S_CLOSE_TEMPORARY:
		// Check the validity of removing the window object.
		if (!firstObject)
			return (S_NO_OBJECT);
		
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_LOCKED))
			break;
		else if (!FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY) &&
			ccode == S_CLOSE_TEMPORARY)
			break;
		else if (screenID == firstObject->screenID)
			return (Event(UI_EVENT(L_EXIT_FUNCTION)));

		// Remove all affected window objects.
		while (firstObject)
		{
			UI_WINDOW_OBJECT *nextObject = (ccode == S_CLOSE &&
				FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY)) ?
				firstObject->Next() : ZIL_NULLP(UI_WINDOW_OBJECT);
			Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = nextObject;
		}
		if (!first)
			ccode = S_NO_OBJECT;
		break;

	case S_REDISPLAY:
		// Redisplay the objects on the screen.
		for (object = Last(); object; object = object->Previous())
		{
			XtUnmanageChild(object->screenID);
			XtManageChild(object->screenID);
		}
		break;

	case L_EXIT_FUNCTION:
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL))
			break;
		else if (exitFunction && (*exitFunction)(display, eventManager, this) != L_EXIT)
			break;
		// Continue to L_EXIT.
	case L_EXIT:
		ccode = L_EXIT;
		break;

	case S_ROUTE_EVENT:
		{
		UI_EVENT *rEvent = event.event;
		if (event.routeTarget && event.routeTarget->screenID)
			event.routeTarget->Event(*rEvent);
		delete rEvent;
		}
		break;

	case S_ADD_OBJECT:
		Add(event.windowObject);
		break;

	case S_SUBTRACT_OBJECT:
		Subtract(event.windowObject);
		break;

	case S_OPEN_DOCUMENT:
	case S_PRINT_DOCUMENT:
		if (firstObject)
			ccode = firstObject->Event(event);
		else
			ccode = S_NO_OBJECT;

		// If these are not processed, we must delete event.text.
		if ((ccode == S_UNKNOWN || ccode == S_NO_OBJECT) && event.text)
			delete event.text;
		break;

	default:
		if (firstObject)
			ccode = firstObject->Event(event);
		else
			ccode = S_NO_OBJECT;
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
	if (UI_LIST::Index(object) == -1)
		return (object);
	else
	{
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top = event.region.right = event.region.bottom = -1;
		object->Event(event);		// Call any user-functions.
		UI_LIST::Subtract(object);
		if (object->screenID)
		{
			if (object->screenID == screenID)
				eventManager->Put(UI_EVENT(L_EXIT, 0));
			else if (!First())
				eventManager->Put(UI_EVENT(S_NO_OBJECT, 0));
			event.type = S_DEINITIALIZE;
			object->Event(event);
		}
	}

	// Return a pointer to the next object.
	return (nextObject);
}
