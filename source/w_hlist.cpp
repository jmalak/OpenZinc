//	Zinc Interface Library - W_HLIST.CPP
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

// ----- UIW_HZ_LIST --------------------------------------------------------

#if defined(ZIL_MSWINDOWS_CTL3D)
extern "C" LRESULT CALLBACK ListWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

static int _hlistOffset = -1;
static FARPROC _hlistCallback = (FARPROC)DefWindowProc;
LRESULT FAR PASCAL _export HListJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
	UIW_HZ_LIST *object = (UIW_HZ_LIST *)GetWindowLong(hWnd, _hlistOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _hlistJumpInstance = (FARPROC)HListJumpProcedure;

EVENT_TYPE UIW_HZ_LIST::Event(const UI_EVENT &event)
{
	static int bState = 1;
	UI_WINDOW_OBJECT *object;
	int processed = FALSE;
	EVENT_TYPE ccode = S_UNKNOWN;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;
		WPARAM wParam = event.message.wParam;
		LPARAM lParam = event.message.lParam;


		processed = TRUE;

		switch (message)
		{
		case WM_COMMAND:
			{
			WORD command = HIWORD(lParam);
			ZIL_NUMBERID wID = LOWORD(wParam);
			switch (command)
			{
			case LBN_SELCHANGE:
				{
				LRESULT selection = SendMessage(screenID, LB_GETCURSEL, 0, 0);
				UIW_WINDOW::Add((selection >= 0) ? (UI_WINDOW_OBJECT *)UI_LIST::Get(LOWORD(selection)) : ZIL_NULLP(UI_WINDOW_OBJECT));
				if (FlagsSet(wnFlags, WNF_AUTO_SELECT | WNF_SELECT_MULTIPLE))
					Current()->Event(UI_EVENT(L_SELECT));
				}
				break;

			default:
				if (wID != numberID)			// If from child not parent.
					ccode = UI_WINDOW_OBJECT::Event(event);
				break;
			}
			}
			break;

		case WM_LBUTTONUP:
			if (windowManager->dragObject &&
				windowManager->dragObject != this)
					ccode = UI_WINDOW_OBJECT::Event(event);
			else if (GetCapture() == screenID)
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				if (Current())
				{
					if (bState)
						Current()->Event(UI_EVENT(L_END_SELECT, 0));
					else
						bState = 1;
				}
			}
			break;

		case WM_COMPAREITEM:
			ccode = UIW_WINDOW::Event(event);
			break;

		case WM_MEASUREITEM:
			{
			MEASUREITEMSTRUCT *measure = (MEASUREITEMSTRUCT *)lParam;
			measure->itemWidth = cellWidth * display->cellWidth;
			measure->itemHeight = cellHeight * (display->cellHeight - display->preSpace - display->postSpace);
			}
			break;

		case WM_LBUTTONDBLCLK:
			if (current && Current()->userFunction)
			{
				BTF_FLAGS btFlags = 0;
				Current()->Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
				UI_EVENT uEvent = event;
				if (FlagSet(btFlags, BTF_DOUBLE_CLICK))
				{
					ccode = UI_WINDOW_OBJECT::Event(event);
					ccode = (*Current()->userFunction)(Current(), uEvent, L_DOUBLE_CLICK);
					bState = 0;
				}
				else
				{
					uEvent.message.message = WM_LBUTTONDOWN;
					ccode = Event(uEvent);
				}
			}
			else
				processed = FALSE;
			break;

		case WM_DESTROY:
		case WM_SYSCHAR:
			ccode = UIW_WINDOW::Event(event);
			break;

		// Ambiguous cases.
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_CHAR:
			processed = FALSE;
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = LogicalEvent(event, ID_HZ_LIST);
		switch (ccode)
		{
		case S_INITIALIZE:
			UIW_WINDOW::Event(event);
			if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
				woStatus |= WOS_OWNERDRAW;
			else
				woStatus &= ~WOS_OWNERDRAW;

			// Check the environment specific flag settings.
			dwStyle |= LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | LBS_NOTIFY;
			if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
				dwStyle |= LBS_OWNERDRAWFIXED;
			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE) || !FlagSet(wnFlags, WNF_AUTO_SELECT))	
				dwStyle |= LBS_MULTIPLESEL;
			if (compareFunction)
				dwStyle |= LBS_SORT;
			if (FlagSet(woFlags, WOF_BORDER))
				dwStyle |= WS_BORDER;
			break;

		case S_CREATE:
			UI_WINDOW_OBJECT::Event(event);
			// Fall through to S_REGISTER_OBJECT.

		case S_REGISTER_OBJECT:
			if (FlagSet(woFlags, WOF_BORDER))
				--trueRegion;
			RegisterObject("UIW_HZ_LIST", "LISTBOX", &_hlistOffset,
				&_hlistJumpInstance, &_hlistCallback);
			if (FlagSet(woFlags, WOF_BORDER))
				++trueRegion;
			SendMessage(screenID, LB_SETCOLUMNWIDTH, cellWidth * display->cellWidth, 0);
			SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
			for (object = First(); object; object = object->Next())
				object->Event(event);
			SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
			break;

		case S_CURRENT:
		case S_REDISPLAY:
		{
			bState = 1;
			ccode = UIW_WINDOW::Event(event);
			if (screenID && current)
				if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					SendMessage(screenID, LB_SETCARETINDEX, Index(current), 0);
				else
					SendMessage(screenID, LB_SETCURSEL, Index(current), 0);
		}
			break;

		case S_NON_CURRENT:
			ccode = UIW_WINDOW::Event(event);
			break;

		case S_ADD_OBJECT:
			object = event.windowObject;
			if (UI_LIST::Index(object) == -1)
				Add(object);
			else if (object != current)
			{
				if (current)
					Current()->woStatus &= ~WOS_CURRENT;
				current = object;
				Current()->woStatus |= WOS_CURRENT;
				if (screenID && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					SendMessage(screenID, LB_SETCURSEL, Index(current), 0);
				if (FlagsSet(wnFlags, WNF_AUTO_SELECT | WNF_SELECT_MULTIPLE))
					Current()->Event(UI_EVENT(L_SELECT));
			}
			break;

		case S_SUBTRACT_OBJECT:
			object = event.windowObject;
			if (UI_LIST::Index(object) != -1)
			{
				if (screenID)
					SendMessage(screenID, LB_DELETESTRING, UI_LIST::Index(object), 0);
				UI_LIST::Subtract(object);
				object->screenID = 0;
				LRESULT selection = SendMessage(screenID, LB_GETCARETINDEX, 0, 0);
				Add((UI_WINDOW_OBJECT *)UI_LIST::Get(LOWORD(selection)));
			}
			break;

		case S_HSCROLL:
			{
			int height = trueRegion.Height();
			if (hScroll)
				height -= GetSystemMetrics(SM_CXHTHUMB);
			int rows;
			if (FlagSet(woStatus, WOS_OWNERDRAW))
				rows = height / (cellHeight * (display->cellHeight - display->preSpace - display->postSpace));
			else
			{
				ZIL_ICHAR string[] = { ' ', 0 };
				rows = height / display->TextHeight(string);
			}
			LRESULT pos = SendMessage(screenID, LB_GETTOPINDEX, 0, 0);
			SendMessage(screenID, LB_SETTOPINDEX, LOWORD(pos + event.scroll.delta * rows), 0);
			}
			break;

		case S_HSCROLL_CHECK:
			{
			LRESULT pos = SendMessage(screenID, LB_GETCARETINDEX, 0, 0);
			SendMessage(screenID, LB_SETCARETINDEX, LOWORD(pos), 0);
			}
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
						object->woStatus &= ~WOS_CURRENT;
						dEvent.type = S_ADD_OBJECT;
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
				dEvent.windowObject->woStatus &= ~WOS_CURRENT;
				Event(dEvent);
			}
			Event(S_REDISPLAY);
			break;

		case S_DROP_COPY_OBJECT:
			if (windowManager->dragObject == this)
				break;
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
						if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
							object->woStatus |= WOS_SELECTED;
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
				if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					dragObject->woStatus |= WOS_SELECTED;
				dragObject->relative.right = dragObject->relative.Width() - 1;
				dragObject->relative.left = 0;
				UI_EVENT dEvent(S_ADD_OBJECT);
				dEvent.windowObject = dragObject;
				Event(dEvent);
			}
			Event(S_REDISPLAY);
#endif
			break;

		case L_BEGIN_SELECT:
		case L_BEGIN_MOVE_DRAG:
		case L_BEGIN_COPY_DRAG:
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			LRESULT selection = SendMessage(screenID, LB_GETCARETINDEX, 0, 0);
			current = UI_LIST::Get(LOWORD(selection));
			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE) ||
				!FlagSet(wnFlags, WNF_AUTO_SELECT))
			{
				if (current)
					SendMessage(screenID, LB_SETSEL,
						FlagSet(Current()->woStatus, WOS_SELECTED), LOWORD(selection));
			}

			if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
			{
				int count = 0;
				for(UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
					if (FlagSet(object->woStatus, WOS_SELECTED))
						count++;

				if (!count)
				{
					windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
					if (!first)
						ReleaseCapture();
					eventManager->DeviceImage(E_MOUSE, DM_VIEW);
				}
				else
				{
					EVENT_TYPE tCode;
					if (FlagsSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT |
						WOAF_COPY_DRAG_OBJECT))
							tCode = (ccode == L_BEGIN_COPY_DRAG) ?
								(count > 1) ? DM_DRAG_MOVE_MULTIPLE : DM_DRAG_COPY :
								(count > 1) ? DM_DRAG_COPY_MULTIPLE : DM_DRAG_MOVE;
					else if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT))
						tCode = (count > 1) ? DM_DRAG_MOVE_MULTIPLE : DM_DRAG_MOVE;
					else
						tCode = (count > 1) ? DM_DRAG_COPY_MULTIPLE : DM_DRAG_COPY;
		
					eventManager->DeviceImage(E_MOUSE, tCode);
				}
			}
			else if (current && FlagSet(Current()->woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
			{
				SendMessage(screenID, WM_LBUTTONUP, event.message.wParam,
					event.message.lParam);
				Current()->Event(event);
			}
		}
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
					CallWindowProc((WINDOWSPROC)defaultCallback, event.message.hwnd,
						WM_LBUTTONUP, event.message.wParam, event.message.lParam);

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

		case L_PREVIOUS:
		case L_NEXT:
			ccode = S_UNKNOWN;
			break;

		case L_LEFT:
		case L_RIGHT:
		case L_UP:
		case L_DOWN:
			woStatus |= WOS_WINDOWS_ACTION;
			ccode = UI_WINDOW_OBJECT::Event(event);
			woStatus &= ~WOS_WINDOWS_ACTION;
			break;

		case L_SELECT:
			if (current)
				ccode = Current()->Event(event);
			if (ccode != S_ERROR)
				Root(TRUE)->Event(UI_EVENT(L_SELECT));
			break;

		default:
			if (event.type == E_MSWINDOWS)
				ccode = UI_WINDOW_OBJECT::Event(event);
			else
				ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_HZ_LIST::OSDestroy(void)
{
	// Destroy the list elements (high level).
	if (screenID)
	{
		SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
		SendMessage(screenID, LB_RESETCONTENT, 0, 0);
	}
}

void UIW_HZ_LIST::OSUpdateSettings(ZIL_OBJECTID objectID)
{
		// See if the field needs to be re-computed.
		if (objectID == ID_HZ_LIST && FlagSet(woStatus, WOS_REDISPLAY))
		{
			Event(UI_EVENT(S_INITIALIZE));
			Event(UI_EVENT(S_CREATE));
		}
}

void UIW_HZ_LIST::OSSort(void)
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

