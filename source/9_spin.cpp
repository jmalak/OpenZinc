//	Zinc Interface Library - D_SPIN.CPP
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

extern ATOM zafProcL;
extern ATOM zafProcH;

#define CELL_RATIO(w) (w / 2 + 1)

#if defined(ZIL_EDIT)
LRESULT CALLBACK EditUpDownCallback(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wMsg)
	{
	case WM_NCHITTEST:
		return (HTTRANSPARENT);
	}

	WINDOWSPROC OldCallback = (WINDOWSPROC)GetWindowLong(hWnd, GWL_USERDATA);
	return (CallWindowProc(OldCallback, hWnd, wMsg, wParam, lParam));
}

LRESULT CALLBACK EditFieldObjectCallback(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wMsg)
	{
	case WM_NCHITTEST:
		return (HTTRANSPARENT);
	}

#if defined(ZIL_UNICODE)
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPWSTR)zafProcL), GetProp(hWnd, (LPWSTR)zafProcH));
#else
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPCSTR)zafProcL), GetProp(hWnd, (LPCSTR)zafProcH));
#endif
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
#endif

// ----- UIW_SPIN_CONTROL ------------------------------------------------------

EVENT_TYPE UIW_SPIN_CONTROL::Event(const UI_EVENT &event)
{

	EVENT_TYPE ccode = S_UNKNOWN;

	//Process messages from system first.
	int processed = FALSE;
	if (event.type == E_MSWINDOWS)
	{
		processed = TRUE;

		// Switch on the windows message.
		UINT message = event.message.message;
		ccode = TRUE;
		switch (message)
		{
#if defined(ZIL_EDIT)
		case WM_LBUTTONDOWN:
			if (FlagSet(woStatus, WOS_EDIT_MODE))
				UI_WINDOW_OBJECT::Event(event);
			break;
#endif

		case WM_NOTIFY:
			return (0);

		case WM_VSCROLL:
			{
			int pos = (int)HIWORD(event.message.wParam);
			if (pos < 1)
				Event(UI_EVENT(L_DOWN));
			else if (pos > 1)
				Event(UI_EVENT(L_UP));
			SendMessage(GetWindow(GetWindow(screenID, GW_CHILD), GW_HWNDLAST),
				UDM_SETPOS, 0, MAKELONG(1, 0));
			}
			return(0);

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = LogicalEvent(event, ID_SPIN_CONTROL);
		switch (ccode)
		{
		case S_INITIALIZE:
			UIW_WINDOW::Event(event);
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
#if defined(ZIL_MSWINDOWS_CTL3D)
			relative.top--;
			relative.bottom++;
#endif
			break;
	
		case S_CHANGED:
		case S_CREATE:
			{
			UI_WINDOW_OBJECT::Event(event);
#if defined(ZIL_MSWINDOWS_CTL3D)
			UI_REGION region = { 1, 1, trueRegion.Width() - 20, trueRegion.Height() - 2 };
#else
			UI_REGION region = { 0, 0, trueRegion.Width() - 19, trueRegion.Height() - 1 };
#endif
			fieldObject->Event(UI_EVENT(S_SIZE, 0, region));
			UIW_WINDOW::Event(event);

			if (ccode == S_CREATE)
			{
				upDownControl = CreateWindow(UPDOWN_CLASS, ZIL_NULLP(ZIL_ICHAR),
					UDS_AUTOBUDDY | WS_CHILD | WS_VISIBLE, trueRegion.Width() - 18, 0, 18,
					trueRegion.Height(), screenID, 0, display->hInstance, ZIL_NULLP(void));
				SendMessage(upDownControl, UDM_SETRANGE, 0, MAKELONG(2, 0));
				SendMessage(upDownControl, UDM_SETPOS, 0, MAKELONG(1, 0));
#if defined(ZIL_EDIT)
				if (FlagSet(woStatus, WOS_EDIT_MODE))
				{
#if defined(ZIL_UNICODE)
					SetProp(upDownControl, (LPWSTR)zafProcL, (HANDLE)LOWORD(this));
					SetProp(upDownControl, (LPWSTR)zafProcH, (HANDLE)HIWORD(this));
#else
					SetProp(upDownControl, (LPCSTR)zafProcL, (HANDLE)LOWORD(this));
					SetProp(upDownControl, (LPCSTR)zafProcH, (HANDLE)HIWORD(this));
#endif
					WNDPROC OldCallback = (WNDPROC)GetWindowLong(upDownControl, GWL_WNDPROC);
					SetWindowLong(upDownControl, GWL_USERDATA, (LONG)OldCallback);
					SetWindowLong(upDownControl, GWL_WNDPROC, (LONG)EditUpDownCallback);
					SetWindowLong(fieldObject->screenID, GWL_WNDPROC, (LONG)EditFieldObjectCallback);


				}
#endif
			}
			else
				MoveWindow(upDownControl, trueRegion.Width() - 18, 0, 18, trueRegion.Height(), FALSE);
			}
			break;
	
		case S_CURRENT:
		case S_NON_CURRENT:
		case S_DISPLAY_ACTIVE:
		case S_DISPLAY_INACTIVE:
			current = fieldObject;
			if (ccode == S_CURRENT)
				SetFocus(fieldObject->screenID);
			UIW_WINDOW::Event(event);
			break;
	
		case L_PREVIOUS:
		case L_NEXT:
			ccode = S_UNKNOWN;
			break;
	
		case L_SELECT:
			if (FlagSet(woStatus, WOS_EDIT_MODE))
				return (UI_WINDOW_OBJECT::Event(event));
			if (!FlagSet(woFlags, WOF_VIEW_ONLY) &&
				event.type == E_KEY && event.key.value == ' ')
				return (fieldObject->Event(event));
			{
				int width = relative.right - relative.left;
				if (!FlagSet(woFlags, WOF_VIEW_ONLY))
					width -= display->cellWidth;
				fieldObject->Event(L_SELECT);
			}
			break;
	
		case L_FIRST:
		case L_LAST:
			if (!FlagSet(woFlags, WOF_VIEW_ONLY))
				return (fieldObject->Event(event));
			// Continue to L_UP.
		case L_UP:
			fieldObject->Information(I_INCREMENT_VALUE, &delta);
			break;
	
		case L_DOWN:
			fieldObject->Information(I_DECREMENT_VALUE, &delta);
			break;
	
		case L_PGUP:
		case L_PGDN:
			break;
	
		case S_ADD_OBJECT:
		    if (fieldObject != event.windowObject)
		    {
			    Subtract(fieldObject);
			    delete fieldObject;
			    fieldObject = event.windowObject;
			    Add(fieldObject);
		    }
			break;

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

