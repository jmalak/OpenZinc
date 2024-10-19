//	Zinc Interface Library - O_SPIN.CPP
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


#define INCL_WINSTDSPIN         // OS/2 spin button control class
#define INCL_WINSTDDRAG			// OS/2 drag messages and flags.
#include "ui_win.hpp"

#if defined(ZIL_EDIT)
static MRESULT EXPENTRY EditCallback(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch (msg)
	{
	case WM_BUTTON1DOWN:
	case WM_BUTTON1DBLCLK:
	case WM_MOUSEMOVE:
		{
		POINTL point;
		point.x = LOWORD(mp1);
		point.y = HIWORD(mp1);
		HWND hwndParent = WinQueryWindow(hwnd, QW_PARENT);
		WinMapWindowPoints(hwnd, hwndParent, &point, 1);
		WinPostMsg(hwndParent, msg, MPFROM2SHORT(point.x, point.y), mp2);
		}
		return ((void *)TRUE);

	case DM_DRAGOVER:
	case DM_DROP:
		{
		HWND hwndParent = WinQueryWindow(hwnd, QW_PARENT);
		return (WinSendMsg(hwndParent, msg, mp1, mp2));
		}
	}
	FNWP *callback = (FNWP *)WinQueryWindowULong(hwnd, QWL_USER);
	return callback(hwnd, msg, mp1, mp2);
}
#endif

static MRESULT EXPENTRY ChildCallback(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch (msg)
	{
	case DM_DRAGOVER:
	case DM_DROP:
		{
		HWND hwndParent = WinQueryWindow(hwnd, QW_PARENT);
		return (WinSendMsg(hwndParent, msg, mp1, mp2));
		}
	}
	FNWP *callback = (FNWP *)WinQueryWindowULong(hwnd, QWL_USER);
	return callback(hwnd, msg, mp1, mp2);
}


EVENT_TYPE UIW_SPIN_CONTROL::Event(const UI_EVENT &event)
{
	static PFNWP _spinCallback = ZIL_NULLP(FNWP);
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_CONTROL:
			{
				USHORT control = HIWORD(event.message.mp1);
				if (control == SPBN_UPARROW || control == SPBN_DOWNARROW)
				{
					// Increment/Decrement value.
					fieldObject->Information(control == SPBN_UPARROW ?
						I_INCREMENT_VALUE : I_DECREMENT_VALUE, &delta);

					// Move fielObject text into spin control.
					ZIL_ICHAR *fieldText;
					fieldObject->Information(I_GET_TEXT, &fieldText);
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
					char *TEXT_ = MapText(fieldText, ZIL_NULLP(char), FALSE);
#else
#					define TEXT_ fieldText
#endif
					WinSendMsg(screenID, SPBM_SETARRAY, (MPARAM)&TEXT_, (MPARAM)1);
				}
			}
			return (0);

		case WM_CHAR:
			{
			ccode = UI_WINDOW_OBJECT::Event(event);

			// Move spin control text to field object.
			ZIL_ICHAR *fieldText;
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
			char os2Text[128];
			WinSendMsg(screenID, SPBM_QUERYVALUE, os2Text, MPFROM2SHORT(128, SPBQ_DONOTUPDATE));
			fieldText = UnMapText(os2Text);
			fieldObject->Information(I_SET_TEXT, fieldText);
			delete fieldText;
#else
			WinSendMsg(screenID, SPBM_QUERYVALUE, fieldText, MPFROM2SHORT(255, SPBQ_DONOTUPDATE));
			fieldObject->Information(I_SET_TEXT, fieldText);
#endif
			}
			return (ccode);
		}

		// Return the control code.
		if (event.InputType() != E_KEY)
			return (UI_WINDOW_OBJECT::Event(event));
	}

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_SPIN_CONTROL);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		flStyle |= SPBS_MASTER;
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			flStyle |= SPBS_READONLY;
		else
			flStyle &= ~SPBS_READONLY;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			flStyle |= WS_DISABLED;
		break;

	case S_REGISTER_OBJECT:
		{
		RegisterObject("UIW_SPIN_CONTROL", WC_SPINBUTTON, &_spinCallback, ZIL_NULLP(ZIL_ICHAR));
		ZIL_ICHAR *fieldText;
		fieldObject->Information(I_GET_TEXT, &fieldText);
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(fieldText, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ fieldText
#endif
		WinSendMsg(screenID, SPBM_SETARRAY, (MPARAM)&TEXT_, (MPARAM)1);

		ZIL_SCREENID childID = WinQueryWindow(screenID, QW_TOP);
		while (childID)
		{
#if defined(ZIL_EDIT)
			if (FlagSet(woStatus, WOS_EDIT_MODE))
				WinSetWindowULong(childID, QWL_USER, (ULONG)WinSubclassWindow(childID, EditCallback));
			else
#endif
				WinSetWindowULong(childID, QWL_USER, (ULONG)WinSubclassWindow(childID, ChildCallback));
			childID = WinQueryWindow(childID, QW_NEXT);
		}		
		}
		break;

	case L_SELECT:
	case S_NON_CURRENT:
		{
		// Set text for fieldObject.
		ZIL_ICHAR *fieldText;
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char os2Text[128];
		WinSendMsg(screenID, SPBM_QUERYVALUE, os2Text, MPFROM2SHORT(128, SPBQ_DONOTUPDATE));
		fieldText = UnMapText(os2Text);
		fieldObject->Information(I_SET_TEXT, fieldText);
		delete fieldText;
#else
		WinSendMsg(screenID, SPBM_QUERYVALUE, fieldText, MPFROM2SHORT(255, SPBQ_DONOTUPDATE));
		fieldObject->Information(I_SET_TEXT, fieldText);
#endif
		fieldObject->Event(S_NON_CURRENT);
		
		// Update spin control with formatted value from fieldObject.
		fieldObject->Information(I_GET_TEXT, &fieldText);
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(fieldText, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ fieldText
#endif
		WinSendMsg(screenID, SPBM_SETARRAY, (MPARAM)&TEXT_, (MPARAM)1);
		}
		// Call user function.
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_ADD_OBJECT:
		if (fieldObject != event.windowObject)
        {
            if (fieldObject)
    			delete fieldObject;
		    fieldObject = event.windowObject;
		    fieldObject->parent = this;
		    Information(I_SET_TEXT, fieldObject->Information(I_GET_TEXT, ZIL_NULLP(void)));
        }
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}
