//	Zinc Interface Library - W_WIN.CPP
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
#if defined(ZIL_MSWINDOWS_CTL3D)
#	include <ctl3d.h>
#endif

// ----- UI_WINDOW_MANAGER --------------------------------------------------

UI_WINDOW_OBJECT *UI_WINDOW_MANAGER::Add(UI_WINDOW_OBJECT *object)
{
	// Check for a null object.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));

		UI_EVENT event;
		UI_WINDOW_OBJECT *firstObject = First();

	// Remove any temporary objects if we have a non-temporary window.
	if (!FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
	{
		while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			if (!FlagSet(firstObject->woStatus, WOS_WINDOWS_ACTION))
			{
				UI_WINDOW_MANAGER::Subtract(firstObject);
				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
					delete firstObject;
			}
			firstObject = nextObject;
		}
	}

	// Remove any temporary objects if the window is already in the list.
	else if (UI_LIST::Index(object) != -1)
	{
		while (firstObject && firstObject != object)
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			if (!FlagSet(firstObject->woStatus, WOS_WINDOWS_ACTION))
			{
				UI_WINDOW_MANAGER::Subtract(firstObject);
				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
					delete firstObject;
			}
			firstObject = nextObject;
		}
	}

	// See if the object is already the first on the list.
	if (object == First())
		return (object);

	// Find the maximum update region.
 	UI_REGION updateRegion = { 0x0FFF, 0x0FFF, 0x0000, 0x0000 };
	UI_WINDOW_OBJECT *tObject = NULL;
	for (tObject = First(); tObject && tObject != object;
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

		// Special case of adding UIW_POP_UP_MENU to the windowManager.
		if (object->Inherited(ID_POP_UP_MENU))
		{
			UINT flags = TPM_LEFTALIGN | TPM_LEFTBUTTON;

			if (object != firstObject)
			{
				if (UI_LIST::Index(object) != -1)
					UI_LIST::Subtract(object);
				UI_LIST::Add(firstObject, object);
			}

			TrackPopupMenu(object->menuID, flags, object->trueRegion.left,
				object->trueRegion.top, 0, taskID, ZIL_NULLP(RECT));

			// Make sure windows event is processed before the S_CLOSE_TEMPORARY.
			UI_EVENT tEvent;
			eventManager->Get(tEvent);
			eventManager->Put(tEvent, Q_BEGIN);
//			eventManager->Get(tEvent, Q_NO_DESTROY);
			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY));

			return (object);
		}
	}

	// Bring the object to the front of the object queue.
	if (object != firstObject)
	{
		if (UI_LIST::Index(object) != -1)
			UI_LIST::Subtract(object);
		else
			updateRegion = object->trueRegion;
		event.type = S_REGION_DEFINE;
		object->Event(event);
		UI_LIST::Add(firstObject, object);
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top = event.region.right = event.region.bottom = -1;
		if (firstObject && !FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject->Event(event);
	}
	event.type = S_CURRENT;
	event.region = updateRegion;
	object->Event(event);
		
	// See if the object needs to be re-displayed.
	if (!FlagSet(object->woStatus, WOS_WINDOWS_ACTION))
	{
		ZIL_SCREENID frameID;
		object->Information(I_GET_FRAMEID, &frameID);
		object->woStatus |= WOS_WINDOWS_ACTION;
		if (taskID == ID_SCREEN)
		{
			taskID = object->screenID;
			ShowWindow(object->screenID, display->nCmdShow);
		}
		else
		{
			int showState = SW_SHOWNORMAL;
			if (UI_LIST::Count() == 1)
				showState = display->nCmdShow;
			if (FlagSet(object->woStatus, WOS_MAXIMIZED))
				showState = SW_SHOWMAXIMIZED;
			else if (FlagSet(object->woStatus, WOS_MINIMIZED))
				showState = SW_SHOWMINIMIZED;
			ShowWindow(frameID, showState);
		}
		EnableWindow(frameID, TRUE);
		UpdateWindow(frameID);
		SetActiveWindow(frameID);
		object->woStatus &= ~WOS_WINDOWS_ACTION;
	}
	else
	{
		event.type = S_CURRENT;
		event.region = updateRegion;
		object->Event(event);
	}

	if (FlagSet(object->woAdvancedFlags, WOAF_MODAL))
	{
		for (tObject = object->Next(); tObject; tObject = tObject->Next())
		{
			ZIL_SCREENID frameID;
			tObject->Information(I_GET_FRAMEID, &frameID);
			EnableWindow(frameID, FALSE);
		}
	}

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UI_WINDOW_MANAGER::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *firstObject = First();

	int processed = FALSE;
	EVENT_TYPE ccode = S_UNKNOWN;

	if (event.type == E_MSWINDOWS)
	{
		processed = TRUE;

		MSG msg = event.message;
		WPARAM wParam = event.message.wParam;
		LPARAM lParam = event.message.lParam;

		if (msg.message == WM_QUIT)
			ccode = L_EXIT;
		else if (!IsWindow(msg.hwnd))
			;
		else if (msg.hwnd == taskID)
		{
			// Process messages for task window.
			TranslateMessage(&msg);

			switch (msg.message)
			{
			case WM_SETFOCUS:
				{
				UI_EVENT addEvent(S_ADD_OBJECT);
				addEvent.windowObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				eventManager->Put(addEvent);
				ccode = 0;
				}
				break;

			case WM_WINDOWPOSCHANGING:
				((WINDOWPOS*)event.message.lParam)->flags |= SWP_NOMOVE;
				ccode = DefWindowProc(msg.hwnd, msg.message,
					msg.wParam, msg.lParam);
				break;

			case WM_NCDESTROY:
				windowManager->taskID = 0;
				ccode = DefWindowProc(msg.hwnd, msg.message,
					msg.wParam, msg.lParam);
				break;

			case WM_QUERYENDSESSION:
			case WM_CLOSE:
				{
				if (Event(UI_EVENT(L_EXIT_FUNCTION)) == L_EXIT)
				{
					ccode = DefWindowProc(msg.hwnd, msg.message,
						msg.wParam, msg.lParam);
					PostQuitMessage(0);
				}
				else
					ccode = FALSE;
				}
				break;

			case WM_MENUSELECT:
				{
				WORD wID = LOWORD(wParam);
				WORD wMenuFlags = HIWORD(wParam);

				// Test if menu has been closed.
				if (wMenuFlags != 0xFFFF || wID != 0)
					First()->Event(event);
				}
				break;

			case WM_COMMAND:
				{
				ZIL_NUMBERID tNumberID = LOWORD(wParam);
				if (first)
				{
					UI_WINDOW_OBJECT *object =
						(UI_WINDOW_OBJECT *)First()->Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&tNumberID));
					if (object)
						ccode = object->Event(event);
				}
				}
				break;

#if defined(ZIL_MSWINDOWS_CTL3D)
			case WM_SYSCOLORCHANGE:
				ccode = Ctl3dColorChange();
				break;
#endif

			case ZM_HELPKEY:
				Event(L_HELP);
				break;

			default:
				ccode = DefWindowProc(msg.hwnd, msg.message,
					msg.wParam, msg.lParam);
			}
		}
		else	// Not for task window.
		{
			HWND mdiClient = 0;

			// See if it is not an MDI hot message.
			if (!First() || !FlagSet(First()->woAdvancedFlags, WOAF_MDI_OBJECT) ||
				!TranslateMDISysAccel(First()->screenID, &msg))
			{
				UINT message = msg.message;

				switch (message)
				{
					// Ambiguous messages.
					case WM_KEYDOWN:
					case WM_KEYUP:
					case WM_CHAR:
					case WM_SYSKEYDOWN:
					case WM_SYSKEYUP:
						processed = FALSE;
						break;

					default:
						ccode = DispatchMessage(&msg);
				}
			}
			else	// It is an MDI hot message.
			{
				HWND cWindowHwnd = (HWND)SendMessage(mdiClient, WM_MDIGETACTIVE, 0, 0);
				ZIL_SCREENID frameID;
				First()->Information(I_GET_FRAMEID, &frameID);
				if (frameID != cWindowHwnd)
				{
	                UI_WINDOW_OBJECT *cWindow = ((UIW_WINDOW *)First())->Last();
					do
					{
						if (!cWindow)
							break;
						ZIL_SCREENID frameID;
						cWindow->Information(I_GET_FRAMEID, &frameID);
						if (frameID == cWindowHwnd)
							break;
						cWindow = cWindow->Previous();
					} while (cWindow && frameID != cWindowHwnd);

					if (cWindow)
					{
						cWindow->woStatus |= WOS_WINDOWS_ACTION;
						cWindow->UI_WINDOW_OBJECT::Event(UI_EVENT(S_ADD_OBJECT));
						cWindow->woStatus &= ~WOS_WINDOWS_ACTION;
					}
				}
			}
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = LogicalEvent(event, ID_WINDOW_MANAGER);
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

		case S_ADD_OBJECT:
			if (event.windowObject)
			{
				// Add the window to the window manager.
				if (firstObject)
					firstObject->woStatus &= ~WOS_CURRENT;
				firstObject = Add(event.windowObject);
				firstObject->woStatus |= WOS_CURRENT;
			}
			else if (first)
			{
				ZIL_SCREENID frameID;
				First()->Information(I_GET_FRAMEID, &frameID);
				SetActiveWindow(frameID);
			}
			break;

		case S_SUBTRACT_OBJECT:
			// Subtract the window from the window manager.
			Subtract(event.windowObject);
			break;

		case S_REDISPLAY:
			{
			for (UI_WINDOW_OBJECT *object = Last(); object; object = object->Previous())
				object->Event(UI_EVENT(S_REDISPLAY));
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

		case L_PREVIOUS:
		case L_NEXT:
		case L_UP:
		case L_DOWN:
		case L_LEFT:
		case L_RIGHT:
			if (firstObject)
				ccode = firstObject->Event(event);
			break;

		case L_NEXT_WINDOW:
			while (firstObject && FlagsSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
				firstObject = firstObject->Next();
			if (firstObject && !FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL) &&
				firstObject != Last())
				UI_WINDOW_MANAGER::Add(Last());
			break;

		case L_HELP:
			// Only display help for the ZM_HELPKEY message which is translated to L_HELP
			if (event.type == E_MSWINDOWS) 
				break;
			// MapEvent returns E_KEY if help context is not general.
			if (UI_EVENT_MAP::MapEvent(eventMapTable, event) != L_HELP)
			{
				if (helpSystem)
					helpSystem->DisplayHelp(windowManager, NO_HELP_CONTEXT);
			}
			else
				ccode = firstObject->Event(UI_EVENT(ccode, 0));
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
			if (event.type == E_MSWINDOWS)
			{
				MSG msg = event.message;
				if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP ||
					msg.message == WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP)
						TranslateMessage(&msg);
				ccode = DispatchMessage(&msg);
			}
			else if (firstObject)
				ccode = firstObject->Event(event);
			else
				ccode = S_NO_OBJECT;
			break;
		}
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

	// Update any status minimized window information.
	if (FlagSet(object->woAdvancedFlags, WOAF_MODAL) && object == First())
	{
		for (UI_WINDOW_OBJECT *tObject = object->Next(); tObject; tObject = tObject->Next())
		{
			ZIL_SCREENID frameID;
			tObject->Information(I_GET_FRAMEID, &frameID);
			EnableWindow(frameID, TRUE);
			if (FlagSet(tObject->woAdvancedFlags, WOAF_MODAL))
				break;
		}
	}

	// Update the windows according to their new list positions.
	UI_EVENT event;
	UI_WINDOW_OBJECT *nextObject = object->Next();
	if (UI_LIST::Index(object) == -1)
		return (object);
	else if (FlagSet(object->woStatus, WOS_WINDOWS_ACTION))
	{
		UI_LIST::Subtract(object);
		if (object->screenID && object->screenID == screenID)
			eventManager->Put(UI_EVENT(L_EXIT, 0));
	}
	else
	{
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top = event.region.right = event.region.bottom = -1;
		object->Event(event);		// Call any user-functions.
		object->woStatus |= WOS_WINDOWS_ACTION;
		UI_WINDOW_OBJECT *nObject = first && first != object ? First() : object->Next();
		if (nObject)
		{
			ZIL_SCREENID frameID;
			nObject->Information(I_GET_FRAMEID, &frameID);
			SetFocus(frameID);
		}
		if (object->screenID)
		{
			object->Event(UI_EVENT(S_DEINITIALIZE));
			if (object->Inherited(ID_POP_UP_MENU))
			{
				DestroyMenu(object->menuID);
				UI_LIST::Subtract(object);
			}
			else
			{
				ZIL_SCREENID frameID;
				object->Information(I_GET_FRAMEID, &frameID);
				DestroyWindow(frameID);	// This re-calls Subtract Internally (see previous else).
			}
			object->woStatus &= ~WOS_SYSTEM_OBJECT;
		}
		object->woStatus &= ~WOS_WINDOWS_ACTION;
	}

	// Return a pointer to the next object.
	return (nextObject);
}

