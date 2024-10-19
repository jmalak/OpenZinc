//	Zinc Interface Library - W_COMBO.CPP
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

// ----- UIW_COMBO_BOX ------------------------------------------------------

#if defined(ZIL_EDIT)
LRESULT CALLBACK EditChildCallback(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wMsg)
	{
	case WM_NCHITTEST:
		return (HTTRANSPARENT);
	}

	WINDOWSPROC OldCallback = (WINDOWSPROC)GetWindowLong(hWnd, GWL_USERDATA);
	return (CallWindowProc((WINDOWSPROC)OldCallback, hWnd, wMsg, wParam, lParam));
}
#endif

static WNDPROC _comboboxCallback = ZIL_NULLF(WNDPROC);
/* START BLOCK COMMENT
**	#if defined(ZIL_MSWINDOWS_CTL3D)
**	extern "C" LRESULT CALLBACK ComboWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
**	#endif
END BLOCK COMMENT */

EVENT_TYPE UIW_COMBO_BOX::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;
	EVENT_TYPE ccode = S_UNKNOWN;
	int processed = FALSE;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;
		WPARAM wParam = event.message.wParam;
		LPARAM lParam = event.message.lParam;

		processed = TRUE;

/* START BLOCK COMMENT
**			if (FlagSet(GetVersion(), 0x80000000) && message >= WM_USER) 		// Win32s Bug
**				switch (message)
**				{
**				case WM_USER+0:
**					message = CB_GETEDITSEL;
**					break;
**				case WM_USER+1:
**					message = CB_LIMITTEXT;
**					break;
**				case WM_USER+2:
**					message = CB_SETEDITSEL;
**					break;
**				case WM_USER+3:
**					message = CB_ADDSTRING;
**					break;
**				case WM_USER+4:
**					message = CB_DELETESTRING;
**					break;
**				case WM_USER+5:
**					message = CB_DIR;
**					break;
**				case WM_USER+6:
**					message = CB_GETCOUNT;
**					break;
**				case WM_USER+7:
**					message = CB_GETCURSEL;
**					break;
**				case WM_USER+8:
**					message = CB_GETLBTEXT;
**					break;
**				case WM_USER+9:
**					message = CB_GETLBTEXTLEN;
**					break;
**				case WM_USER+10:
**					message = CB_INSERTSTRING;
**					break;
**				case WM_USER+11:
**					message = CB_RESETCONTENT;
**					break;
**				case WM_USER+12:
**					message = CB_FINDSTRING;
**					break;
**				case WM_USER+13:
**					message = CB_SELECTSTRING;
**					break;
**				case WM_USER+14:
**					message = CB_SETCURSEL;
**					break;
**				case WM_USER+15:
**					message = CB_SHOWDROPDOWN;
**					break;
**				case WM_USER+16:
**					message = CB_GETITEMDATA;
**					break;
**				case WM_USER+17:
**					message = CB_SETITEMDATA;
**					break;
**				case WM_USER+18:
**					message = CB_GETDROPPEDCONTROLRECT;
**					break;
**				case WM_USER+19:
**					message = CB_SETITEMHEIGHT;
**					break;
**				case WM_USER+20:
**					message = CB_GETITEMHEIGHT;
**					break;
**				case WM_USER+21:
**					message = CB_SETEXTENDEDUI;
**					break;
**				case WM_USER+22:
**					message = CB_GETEXTENDEDUI;
**					break;
**				case WM_USER+23:
**					message = CB_GETDROPPEDSTATE;
**					break;
**				case WM_USER+24:
**					message = CB_FINDSTRINGEXACT;
**					break;
**				}
END BLOCK COMMENT */

		switch (message)
		{
		case WM_SIZE:
			{
			ccode = UI_WINDOW_OBJECT::Event(event);
			RECT rect;
			GetWindowRect(screenID, &rect);
			relative.bottom = relative.top + rect.bottom - rect.top - 1;
			trueRegion.bottom = trueRegion.top + rect.bottom - rect.top - 1;
			}
			break;

		case WM_KILLFOCUS:
			if (FlagSet(woStatus, WOS_CHANGED))
				Information(I_GET_TEXT, ZIL_NULLP(void));
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		case WM_COMMAND:
			{
			WORD command = HIWORD(wParam);
			WORD wID = LOWORD(wParam);
			switch (command)
			{
				case CBN_SELCHANGE:
					if (wID != numberID)			// If from child not parent.
					{
						woStatus &= ~WOS_UNANSWERED;
						woStatus |= WOS_CHANGED;
						ccode = UI_WINDOW_OBJECT::Event(event);
						LRESULT selection = SendMessage(screenID, CB_GETCURSEL, 0, 0);
						object = (selection >= 0) ? (UI_WINDOW_OBJECT *)list.UI_LIST::Get(LOWORD(selection)) : ZIL_NULLP(UI_WINDOW_OBJECT);
						if (object)
							list.Add(object);
						if (!FlagSet(woFlags, WOF_AUTO_CLEAR))
							SendMessage(screenID, CB_SETEDITSEL, 0, 0x00000000L);
					}
					break;

				case CBN_SETFOCUS:
					if (wID != numberID)			// If from child not parent.
						ccode = UI_WINDOW_OBJECT::Event(event);
					if (FlagSet(woStatus, WOS_UNANSWERED))
						SendMessage(screenID, WM_SETTEXT, 0, (LPARAM)_blankString);
					else if (!FlagSet(woFlags, WOF_AUTO_CLEAR))
						SendMessage(screenID, CB_SETEDITSEL, 0, 0x00000000L);
					break;

				case CBN_EDITCHANGE:
					if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
					{
						woStatus &= ~WOS_UNANSWERED;
						woStatus |= WOS_CHANGED;
						if (wID != numberID)			// If from child not parent.
							UI_WINDOW_OBJECT::Event(event);
			
						int eLen = (int)SendMessage(screenID, WM_GETTEXTLENGTH, 0, 0) + 1;
						ZIL_ICHAR *eBuffer = new ZIL_ICHAR[eLen];
						SendMessage(screenID, WM_GETTEXT, eLen, (LPARAM)eBuffer);
						int index = (int)SendMessage(screenID, CB_FINDSTRING, -1, (LPARAM)eBuffer);
						object = (index >= 0) ? (UI_WINDOW_OBJECT *)list.UI_LIST::Get(index) : ZIL_NULLP(UI_WINDOW_OBJECT);
						if (object)
							list.current = object;
						delete eBuffer;
					}
					break;

				default:
					if (!FlagSet(woStatus, WOS_CURRENT) &&
						HIWORD(lParam) == WM_KEYDOWN)
					{
						woStatus |= WOS_WINDOWS_ACTION;
						UI_WINDOW_OBJECT::Event(UI_EVENT(S_ADD_OBJECT));
						woStatus &= ~WOS_WINDOWS_ACTION;
					}
					else if (wID != numberID)			// If from child not parent.
						ccode = UI_WINDOW_OBJECT::Event(event);
					break;
			}

			}
			break;

		case WM_COMPAREITEM:
			ccode = list.Event(event);
			break;

		case WM_MEASUREITEM:
			{
			MEASUREITEMSTRUCT *measure = (MEASUREITEMSTRUCT *)lParam;
			if (measure->itemID == (unsigned)-1)
			{
				measure->itemWidth = relative.right - relative.left - 2;
				measure->itemHeight = display->cellHeight - display->preSpace - display->postSpace;
				for (object = list.First(); object; object = object->Next())
					if (measure->itemHeight < (unsigned)object->relative.Height())
						measure->itemHeight = object->relative.Height();
			}
			else
			{
				object = (UI_WINDOW_OBJECT *)measure->itemData;
				object->Event(event);
			}
			ccode = TRUE;
			}
			break;

		case WM_PAINT:
			ccode = UI_WINDOW_OBJECT::Event(event);
			if (FlagSet(woStatus, WOS_UNANSWERED))
				SendMessage(screenID, WM_SETTEXT, 0, (LPARAM)_blankString);
			else if (!FlagSet(woFlags, WOF_AUTO_CLEAR))
				SendMessage(screenID, CB_SETEDITSEL, 0, 0x00000000L);
			break;

		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			{
			ZIL_SCREENID objectID = (HWND)lParam;
			if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) ||
				FlagSet(woFlags, WOF_VIEW_ONLY))
					ccode = UI_WINDOW_OBJECT::Event(event);
			else if (objectID == event.message.hwnd)
			{
				HDC objectDC = (HDC)wParam;
				UI_WINDOW_OBJECT *bObject = NULL;
				for (bObject = parent; bObject &&
					(bObject->LogicalPalette(S_DISPLAY_ACTIVE)->colorBackground == BACKGROUND ||
					bObject->LogicalPalette(S_DISPLAY_ACTIVE)->fillPattern == PTN_RGB_COLOR);
					bObject = bObject->parent)
					;
				SetBkColor (objectDC, !bObject ? GetSysColor(COLOR_WINDOW) :
					display->MapColor(bObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE));

				if (ctlHBrush)
				{
					HGDIOBJ oldBrush = SelectObject(objectDC, ctlHBrush);
					if (oldBrush != ctlHBrush)
					{
						SelectObject(objectDC, oldBrush);
						DeleteObject(ctlHBrush);
						ctlHBrush = 0;
					}
				}

				if (!ctlHBrush)
				{
					ctlHBrush = CreateSolidBrush(!bObject ? GetSysColor(COLOR_WINDOW) :
						display->MapColor(bObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE));
					UnrealizeObject(ctlHBrush);
					POINT p = { 0, 0 };
					ClientToScreen(screenID, &p);
					POINT oldP;
					SetBrushOrgEx(objectDC, p.x, p.y, &oldP);
				}

				ccode = (LRESULT)ctlHBrush;
			}
			else
			{
				UI_EVENT tEvent = event;
				tEvent.message.hwnd = objectID;
				ccode = UI_WINDOW_OBJECT::Event(tEvent);
			}
			}
			break;

		case WM_DESTROY:
			{
			ccode = UIW_WINDOW::Event(event);
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				if (!FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
					object->screenID = 0;
			}
			break;

		case WM_SYSCHAR:
			ccode = UIW_WINDOW::Event(event);
			break;

		// Ambiguous cases.
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			processed = FALSE;
			break;

		case WM_DRAWITEM:
			ccode = UIW_WINDOW::Event(event);
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = LogicalEvent(event, ID_COMBO_BOX);
		switch (ccode)
		{
		case S_INITIALIZE:
			UIW_WINDOW::Event(event);

			// Check the environment specific flag settings.
			if (compareFunction)
				dwStyle |= CBS_SORT;
			if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(list.wnFlags, WNF_BITMAP_CHILDREN))
				dwStyle |= CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE;
			else if (FlagSet(woFlags, WOF_VIEW_ONLY))
				dwStyle |= CBS_DROPDOWNLIST;
			else
				dwStyle |= CBS_DROPDOWN | CBS_AUTOHSCROLL;
			if (FlagSet(woFlags, WOF_BORDER))
				dwStyle |= WS_BORDER;
			list.Event(event);
			relative.bottom = MaxValue(relative.bottom, relative.top + display->cellHeight - display->preSpace - display->postSpace - 1);
			break;

		case S_CREATE:
			{
			if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
			{
				int height = display->cellHeight - display->preSpace - display->postSpace;
				for (object = list.First(); object; object = object->Next())
					if (height < object->relative.Height())
						height = object->relative.Height();
				relative.bottom = relative.top + height + display->preSpace + display->postSpace + 1;
			}

			UI_WINDOW_OBJECT::Event(event);
			dwStyle |= (list.dwStyle & (WS_VSCROLL | WS_HSCROLL));

			trueRegion.bottom += list.relative.bottom - list.relative.top;
			RegisterObject("UIW_COMBO_BOX", "COMBOBOX", &_comboboxCallback);
#if defined(ZIL_EDIT)
			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				ZIL_SCREENID childID = GetWindow(screenID, GW_CHILD);
				WNDPROC OldCallback = (WNDPROC)GetWindowLong(childID, GWL_WNDPROC);
				SetWindowLong(childID, GWL_USERDATA, (LONG)OldCallback);
				SetWindowLong(childID, GWL_WNDPROC, (LONG)EditChildCallback);
			}
#endif


			RECT rect;
			GetClientRect(screenID, &rect);
			trueRegion.bottom = trueRegion.top + rect.bottom - 1;
			relative.bottom = relative.top + rect.bottom - 1;
			list.screenID = screenID;
			for (object = list.First(); object; object = object->Next())
				object->Event(event);

			if (list.current &&
				SendMessage(screenID, CB_GETCURSEL, 0, 0) != list.Index(list.current))
				SendMessage(screenID, CB_SETCURSEL, list.Index(list.current), 0);

			if (text)
			{
				SendMessage(screenID, WM_SETTEXT, 0, (LPARAM)text);
				delete text;
				text = ZIL_NULLP(ZIL_ICHAR);
			}
			}
			break;

		case S_REGISTER_OBJECT:
			{
			UI_REGION region = trueRegion;
			trueRegion.bottom += list.relative.bottom - list.relative.top;
			RegisterObject("UIW_COMBO_BOX", "COMBOBOX", &_comboboxCallback, text);
			trueRegion = region;
			if (text)
				delete text;
			text = ZIL_NULLP(ZIL_ICHAR);
			}
			break;

		case S_ADD_OBJECT:
			object = event.windowObject;
			if (list.Index(object) == -1)
			{
#if defined(ZIL_EDIT)
				if (FlagSet(woStatus, WOS_EDIT_MODE))
					object->relative.left = 0;
#endif
				list.Event(event);
			}
			else if (object != list.current)
			{
				if (list.current)
					list.Current()->woStatus &= ~WOS_CURRENT;
				list.current = object;
				list.Current()->woStatus |= WOS_CURRENT;
				if (screenID)
					SendMessage(screenID, CB_SETCURSEL, list.Index(list.current), 0);
			}
			break;		// Stop at the combo-box level.

		case S_SUBTRACT_OBJECT:
			if (list.Index(event.windowObject) != -1)
				Subtract(event.windowObject);
			break;

		case S_DEINITIALIZE:
			list.Event(event);
			break;

		case S_DROP_DEFAULT:
		case S_DROP_MOVE_OBJECT:
			if (windowManager->dragObject == this ||
				windowManager->dragObject->parent == this)
					break;
			else if (windowManager->dragObject->Inherited(ID_LIST))
			{
				UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
				UI_WINDOW_OBJECT *nextObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				WNF_FLAGS twnFlags;
				windowManager->dragObject->Information(I_GET_FLAGS, &twnFlags, ID_WINDOW);
				for (windowManager->dragObject->Information(I_GET_FIRST, &object);
					object; object = nextObject)
				{
					nextObject = object->Next();
					if (FlagSet(object->woStatus, WOS_SELECTED))
					{
						UI_EVENT dEvent(S_SUBTRACT_OBJECT);
						dEvent.windowObject = object;
						windowManager->dragObject->Event(dEvent);
						dEvent.type = S_ADD_OBJECT;
						dEvent.windowObject->woStatus &= ~(WOS_CURRENT | WOS_SELECTED);
						Event(dEvent);
						if (!FlagSet(twnFlags, WNF_SELECT_MULTIPLE))
							break;
					}
				}
				windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
			}
			else if (windowManager->dragObject->parent &&
				windowManager->dragObject->parent->Inherited(ID_LIST) &&
				windowManager->dragObject->parent != this)
			{
				UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
				UI_WINDOW_OBJECT *dragParent = dragObject->parent;
				UI_EVENT dEvent(S_SUBTRACT_OBJECT);
				dEvent.windowObject = dragObject;
				dragParent->Event(dEvent);
				dragParent->Event(UI_EVENT(S_REDISPLAY));
				dragObject->relative.right = dragObject->relative.Width() - 1;
				dragObject->relative.left = 0;
				dEvent.type = S_ADD_OBJECT;
				dEvent.windowObject->woStatus &= ~(WOS_CURRENT | WOS_SELECTED);
				Event(dEvent);
			}
			Event(S_REDISPLAY);
			break;

		case S_DROP_COPY_OBJECT:
#if defined(ZIL_LOAD) && defined(ZIL_STORE)
			if (windowManager->dragObject->Inherited(ID_LIST))
			{
				UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
				UI_WINDOW_OBJECT *nextObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				for (windowManager->dragObject->Information(I_GET_FIRST, &object);
					object; object = nextObject)
				{
					nextObject = object->Next();
					if (FlagSet(object->woStatus, WOS_SELECTED))
					{
						ZIL_ICHAR name[] = { 'd','r','a','g',0 };
						ZIL_NEW_FUNCTION newFunction = object->NewFunction();
						ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
						ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
						object->Store(name, &directory, &file);
						file.Seek(0);
						object = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
						UI_EVENT dEvent(S_ADD_OBJECT);
						dEvent.windowObject = object;
						Event(dEvent);
					}
				}
				windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
			}
			else
			{
				ZIL_ICHAR name[] = { 'd','r','a','g',0 };
				UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
				ZIL_NEW_FUNCTION newFunction = dragObject->NewFunction();
				ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
				ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
				dragObject->Store(name, &directory, &file);
				file.Seek(0);
				dragObject = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
				dragObject->relative.right = dragObject->relative.Width() - 1;
				dragObject->relative.left = 0;
				UI_EVENT dEvent(S_ADD_OBJECT);
				dEvent.windowObject = dragObject;
				Event(dEvent);
			}
			Event(S_REDISPLAY);
#endif
			break;

		case L_CONTINUE_SELECT:
		case L_CONTINUE_COPY_DRAG:
		case L_CONTINUE_MOVE_DRAG:
			if (windowManager->dragObject)
			{
				// Find who the object overlaps and send them the message.
				POINT p = { LOWORD(event.message.lParam), HIWORD(event.message.lParam) };
				ClientToScreen(screenID, &p);
				HWND hWin = WindowFromPoint(p);

				if (!hWin || hWin == GetDesktopWindow())
					eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
				else if (hWin == screenID)
				{
					CallWindowProc((WINDOWSPROC)defaultCallback, event.message.hwnd,
						event.message.message, event.message.wParam, event.message.lParam);

					if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP) ||
						windowManager->dragObject == this)
					{
						EVENT_TYPE tCode;
						if (FlagsSet(windowManager->dragObject->woAdvancedFlags,
							WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
						{
							tCode = (ccode == L_CONTINUE_MOVE_DRAG) ?
								S_DRAG_MOVE_OBJECT :
								ccode == L_CONTINUE_COPY_DRAG ?
								S_DRAG_COPY_OBJECT : S_DRAG_DEFAULT;
						}
						else if (FlagSet(windowManager->dragObject->woAdvancedFlags,
							WOAF_MOVE_DRAG_OBJECT))
								tCode = S_DRAG_MOVE_OBJECT;
						else
								tCode = S_DRAG_COPY_OBJECT;
	
						if (windowManager->dragObject == this || !userFunction ||
							UserFunction(event, tCode) == S_UNKNOWN)
								Event(UI_EVENT(tCode, 0, event.position));
					}
					else
						eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
				}
				else if (GetWindowTask(hWin) == GetWindowTask(screenID) &&
					IsWindowEnabled(hWin))
				{
					ScreenToClient(hWin, &p);
					LPARAM tLParam = p.x | ((LPARAM)p.y << 16);
					SendMessage(hWin, event.message.message, event.message.wParam,
						tLParam);
				}
				else
					eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		case L_BEGIN_SELECT:
		case L_BEGIN_MOVE_DRAG:
		case L_BEGIN_COPY_DRAG:
			if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				if (!list.current)
				{
					windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
					eventManager->DeviceImage(E_MOUSE, DM_VIEW);
				}
			}
			else
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE) ||
					!FlagSet(wnFlags, WNF_AUTO_SELECT))
				{
					LRESULT selection = SendMessage(screenID, LB_GETCARETINDEX, 0, 0);
					current = UI_LIST::Get(LOWORD(selection));
					if (current)
						SendMessage(screenID, LB_SETSEL,
							FlagSet(Current()->woStatus, WOS_SELECTED), LOWORD(selection));
				}
				if (current && FlagSet(Current()->woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
				{
					SendMessage(screenID, WM_LBUTTONUP, event.message.wParam,
						event.message.lParam);
					Current()->Event(event);
				}
			}
			break;

		case L_PREVIOUS:
		case L_NEXT:
			ccode = S_UNKNOWN;
			break;

		case L_LEFT:
		case L_RIGHT:
			if (FlagSet(woFlags, WOF_VIEW_ONLY) || FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				break;
			}
			// Else fall throught to L_LEFT.

		case L_UP:
		case L_DOWN:
			woStatus |= WOS_WINDOWS_ACTION;
			ccode = UI_WINDOW_OBJECT::Event(event);
			woStatus |= WOS_CHANGED;
			woStatus &= ~WOS_WINDOWS_ACTION;
			break;

		case L_SELECT:
			if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) &&
			 	!FlagSet(woFlags, WOF_VIEW_ONLY))
			{
				woStatus &= ~WOS_UNANSWERED;
				ccode = UI_WINDOW_OBJECT::Event(event);
				LRESULT selection = SendMessage(screenID, CB_GETCURSEL, 0, 0);
				object = (selection >= 0) ? (UI_WINDOW_OBJECT *)list.UI_LIST::Get(LOWORD(selection)) : ZIL_NULLP(UI_WINDOW_OBJECT);
				if (object)
					list.Add(object);
				if (!FlagSet(woFlags, WOF_AUTO_CLEAR))
					SendMessage(screenID, CB_SETEDITSEL, 0, 0x00000000L);
			}
			else
				ccode = UIW_WINDOW::Event(event);
			woStatus |= WOS_CHANGED;
			break;

		default:
			if (event.type == E_MSWINDOWS)
				ccode = UI_WINDOW_OBJECT::Event(event);
			else
				ccode = UIW_WINDOW::Event(event);
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_COMBO_BOX::OSDataGet(void)
{
	if (!screenID || FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		return;

	if (!text)
		text = new ZIL_ICHAR[256];

	if (screenID)
		SendMessage(screenID, WM_GETTEXT, 255, (LONG)text);
}

void UIW_COMBO_BOX::OSDataSet(void)
{
	// Try to find a matching element.
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
	{
		ZIL_ICHAR *ctext = ZIL_NULLP(ZIL_ICHAR);
		object->Information(I_GET_TEXT, &ctext);
		if (ctext && !stricmp(text, ctext))
		{
			Add(object);
			Event(S_REDISPLAY);
			break;
		}
	}
	if (screenID && !FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
	{
		InvalidateRect(screenID, ZIL_NULLP(RECT), TRUE);
		SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
	}
}

void UIW_COMBO_BOX::OSDestroy(void)
{
	// No code necessary.
}

void UIW_COMBO_BOX::OSResetSelection(UI_WINDOW_OBJECT *object)
{
}

void UIW_COMBO_BOX::OSMatchInput(void)
{
	// No code necessary.
}

void UIW_COMBO_BOX::OSSort(void)
{
	if (screenID)
	{
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		{
			for (UI_WINDOW_OBJECT *tObject = First(); tObject; tObject = tObject->Next())
				SendMessage(screenID, LB_SETITEMDATA, tObject->ListIndex(), (LPARAM)tObject);
			Event(UI_EVENT(S_REDISPLAY));
		}
		else
			for (UI_WINDOW_OBJECT *tObject = First(); tObject; tObject = tObject->Next())
				tObject->Information(I_SET_TEXT, ZIL_NULLP(void));
	}
}

void UIW_COMBO_BOX::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_COMBO_BOX && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

