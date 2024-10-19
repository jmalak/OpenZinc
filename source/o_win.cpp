//	Zinc Interface Library - O_WIN.CPP
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


#define INCL_WINMENUS
#include "ui_win.hpp"

// ----- UI_WINDOW_MANAGER --------------------------------------------------

UI_WINDOW_OBJECT *UI_WINDOW_MANAGER::Add(UI_WINDOW_OBJECT *object)
{
	UI_EVENT event;
	UI_WINDOW_OBJECT *firstObject = First();

	// Bring the object to the front of the object queue.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
	else if (object == firstObject)
	{
		WinSetActiveWindow(HWND_DESKTOP, object->screenID);
		return (object);
	}

	// Remove any temporary objects if we have a non-temporary window.
	if (!FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
	{
		while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = nextObject;
		}
	}
	else if (UI_LIST::Index(object) != -1)
	{
		// Remove any temporary objects after the added temporary object.
		while (firstObject && firstObject != object)
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = nextObject;
		}
	}

	if (firstObject != object)
	{
		if (UI_LIST::Index(object) != -1)
			UI_LIST::Subtract(object);

		if (firstObject)
		{
			event.type = S_NON_CURRENT;
			firstObject->woStatus &= ~WOS_CURRENT;
			firstObject->Event(event);
			object->woStatus |= WOS_CURRENT;
		}

		UI_LIST::Add(firstObject, object);
	}

	// Initialize object.
	if (!object->screenID)
	{
		event.type = S_INITIALIZE;
		object->Event(event);
		object->trueRegion = object->clip = object->relative;
		event.type = S_CREATE;
		object->Event(event);
	}
	event.type = S_CURRENT;
	object->Event(event);

	// Activate the object.
	if (object->Inherited(ID_POP_UP_MENU))
	{
		WinPopupMenu(taskID, taskID, object->screenID,
			0, 0, 0, PU_NONE | PU_KEYBOARD | PU_MOUSEBUTTON1 |
			PU_HCONSTRAIN | PU_VCONSTRAIN);
	}
	else
	{
		ZIL_SCREENID frameID;
		object->Information(I_GET_FRAMEID, &frameID);
		WinShowWindow(frameID, TRUE);
	}

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UI_WINDOW_MANAGER::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *firstObject = First();
	EVENT_TYPE ccode = event.type;
	UI_WINDOW_OBJECT *object;

	// Dispatch the message through the OS/2 window manager.
	if (ccode == E_OS2)
	{
		QMSG message = event.message;
		switch (message.msg)
		{
		case WM_QUIT:
			if (exitFunction && (*exitFunction)(display, eventManager, this) != L_EXIT)
				return (S_CONTINUE);
			return (L_EXIT);

		case WM_CHAR:
			if (LogicalEvent(event, ID_WINDOW_MANAGER) != E_OS2)
				break;
			// Continue to default.
   		default:
			if (message.msg >= WM_MOUSEFIRST && message.msg <= WM_MOUSELAST)
			{
				// Trap Mouse Events for Modal Windows.
				UI_WINDOW_OBJECT *modalWindow = First();
				while (modalWindow && !FlagSet(modalWindow->woAdvancedFlags, WOAF_MODAL))
					modalWindow = modalWindow->Next();
				if (modalWindow)
				{
					POINTL mousePos;
					mousePos.x = LOWORD(message.mp1);
					mousePos.y = HIWORD(message.mp1);
					WinMapWindowPoints(message.hwnd, HWND_DESKTOP, &mousePos, 1);
					HWND mouseWindowID = WinWindowFromPoint(HWND_DESKTOP, &mousePos, FALSE);

					UI_WINDOW_OBJECT *destWindow = First();
					ZIL_SCREENID destWindowID = 0;
					while (destWindow && destWindowID != mouseWindowID)
					{
						destWindow->Information(I_GET_FRAMEID, &destWindowID);
						if (destWindowID != mouseWindowID)
							destWindow = destWindow->Next();
					}

					if (destWindow && UI_LIST::Index(destWindow) > UI_LIST::Index(modalWindow))
					{
						if (FlagsSet(event.rawCode, M_LEFT | M_LEFT_CHANGE) ||
							FlagsSet(event.rawCode, M_RIGHT | M_RIGHT_CHANGE))
						{
							ZIL_SCREENID firstWindowID;
							First()->Information(I_GET_FRAMEID, &firstWindowID);

							if (WinQueryActiveWindow(HWND_DESKTOP) != firstWindowID)
								WinSetActiveWindow(HWND_DESKTOP, firstWindowID);
							else
								object->errorSystem->Beep();
							return (ccode);
						}
					}
				}
			}
			return ((EVENT_TYPE)WinDispatchMsg(display->hab, &message));
		}
	}

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_WINDOW_MANAGER);
	switch (ccode)
	{
	case S_CLOSE:
	case S_CLOSE_TEMPORARY:
		// Check the validity of removing the window object.
		if (!firstObject)
			return (S_NO_OBJECT);
		else if (screenID == firstObject->screenID && exitFunction)
		{
			ccode = Event(UI_EVENT(L_EXIT_FUNCTION, 0));
			break;
		}
		else if (screenID == firstObject->screenID)
			ccode = L_EXIT;
		else if (FlagSet(firstObject->woAdvancedFlags, WOAF_LOCKED))
			break;
		else if (!FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY) &&
			ccode == S_CLOSE_TEMPORARY)
			break;

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
			return (S_NO_OBJECT);
		break;

	case L_EXIT_FUNCTION:
		if (exitFunction && (*exitFunction)(display, eventManager, this) != L_EXIT)
			break;
		// Continue to L_EXIT.
	case L_EXIT:
		ccode = L_EXIT;
		break;

	case L_HELP:
		// MapEvent returns E_KEY if help context is not general.
		if (UI_EVENT_MAP::MapEvent(eventMapTable, event) != L_HELP)
		{
			if (helpSystem)
				helpSystem->DisplayHelp(windowManager, NO_HELP_CONTEXT);
		}
		else
			ccode = firstObject->Event(UI_EVENT(ccode, 0));
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		UIW_WINDOW::Event(event);
		break;

	case S_OPEN_DOCUMENT:
	case S_PRINT_DOCUMENT:
		// If these are not processed, we must delete event.text.
		ccode = firstObject ? firstObject->Event(event) : S_NO_OBJECT;
		if ((ccode == S_UNKNOWN || ccode == S_NO_OBJECT) && event.text)
			delete event.text;
		break;

	default:
		if (event.type != E_OS2)
			ccode = firstObject ? firstObject->Event(event) : S_NO_OBJECT;
		else
		{
			QMSG message = event.message;
			ccode = (EVENT_TYPE)WinDispatchMsg(display->hab, &message);
		}
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
	else if (UI_LIST::Index(object) == -1)
		return (object);

	object->Event(UI_EVENT(S_NON_CURRENT));

	// Remove the object from the display.
	UI_EVENT event;
	if (object->screenID)
	{
		event.type = S_DEINITIALIZE;
		event.windowObject = object;
		object->Event(event);
	}

	// Remove the object from the object queue.
	UI_LIST::Subtract(object);

	UI_WINDOW_OBJECT *topObject = First();
	if (topObject)
	{
		WinSetActiveWindow(HWND_DESKTOP, topObject->screenID);
		if (!FlagSet(topObject->woStatus, WOS_CURRENT))
			topObject->Event(UI_EVENT(S_CURRENT));
	}

	// Return a pointer to the top object.
	return (topObject);
}

