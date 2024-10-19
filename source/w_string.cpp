//	Zinc Interface Library - W_STRING.CPP
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

// ----- UIW_STRING ---------------------------------------------------------

#if defined(ZIL_MSWINDOWS_CTL3D)
extern "C" LRESULT CALLBACK EditWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

static int _stringOffset = -1;
static FARPROC _stringCallback = (FARPROC)DefWindowProc;
LRESULT FAR PASCAL _export StringJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
	// Get the object pointer and call the C++ class object.
	UIW_STRING *object = (UIW_STRING *)GetWindowLong(hWnd, _stringOffset);
	if (FlagSet(object->stFlags, STF_VARIABLE_NAME) && wMsg == WM_CHAR && wParam == ' ')
		wParam = '_';
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _stringJumpInstance = (FARPROC)StringJumpProcedure;

EVENT_TYPE UIW_STRING::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif
	const int STRING_OFFSET = Inherited(ID_LIST_ITEM) ? 4 :
		FlagSet(woFlags, WOF_BORDER) ? 2 : 0;

	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);
	lastPalette = LogicalPalette(ccode);

	// Draw the string information.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	UI_WINDOW_OBJECT *object = NULL;
	for (object = this; object && FlagSet(object->woStatus, WOS_CURRENT);
		object = object->parent)
			;
	if (!object && parent->Inherited(ID_LIST))
		DrawFocus(screenID, trueRegion, ccode);
	region.left += STRING_OFFSET;
	if (parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
		region.right -= STRING_OFFSET;
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_STRING::Event(const UI_EVENT &event)
{
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
#if defined(ZIL_MSWINDOWS_CTL3D)
		case WM_CTLCOLOR:
			{
			ZIL_SCREENID objectID = (HWND)LOWORD(lParam);
			HDC objectDC = (HDC)wParam;
			if (objectID == event.message.hwnd)
			{
				if (FlagSet(woFlags, WOF_VIEW_ONLY) && FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) &&
					!FlagSet(woStatus, WOS_OWNERDRAW))
				{
					UI_WINDOW_OBJECT *backgroundObject = NULL;
					for (backgroundObject = parent; backgroundObject &&
						display->MapColor(backgroundObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE) == BACKGROUND;
						backgroundObject = backgroundObject->parent)
						;
					SetBkColor (objectDC, !backgroundObject ? GetSysColor(COLOR_WINDOW) :
						display->MapColor(backgroundObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE));

					if (ctlHBrush)
						DeleteObject(ctlHBrush);
					ctlHBrush = CreateSolidBrush(!backgroundObject ? GetSysColor(COLOR_WINDOW) :
						display->MapColor(backgroundObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE));
					UnrealizeObject(ctlHBrush);
					POINT p = { 0, 0 };
					ClientToScreen(screenID, &p);
					SetBrushOrg(objectDC, p.x, p.y);
					ccode = (LRESULT)ctlHBrush;
				}
				else
					ccode = FALSE;
			}
			else
				ccode = SendMessage(objectID, message, wParam, lParam);
			}
			break;
#endif

		case WM_KILLFOCUS:
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) &&
				SendMessage(screenID, EM_GETMODIFY, 0, 0))
					DataGet();
			// Fall through to WM_SETFOCUS.

		case WM_SETFOCUS:
			if (FlagSet(woStatus, WOS_OWNERDRAW) && screenID)
				ccode = UI_WINDOW_OBJECT::Event(UI_EVENT(E_MSWINDOWS, screenID,
					WM_PAINT, wParam, lParam));
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		case WM_CHAR:
			if (parent && parent->Inherited(ID_LIST))
				processed = FALSE;
			else if (FlagSet(woFlags, WOF_VIEW_ONLY) && parent &&
				FlagSet(parent->woAdvancedFlags, WOAF_NORMAL_HOT_KEYS))
					ccode = parent->Event(UI_EVENT(E_MSWINDOWS, screenID,
						WM_SYSCHAR, wParam, lParam));
			else if (FlagSet(woFlags, WOF_VIEW_ONLY))
				break;
			else if (wParam < ' ')
			{
				if (wParam == VK_ESCAPE && Root()->IsTemporary())
					break;
				processed = FALSE;
			}
			else
			{
				UI_EVENT tEvent = event;
				if (HasVariableName()  && wParam == ' ')
					tEvent.message.wParam = '_';
				ccode = UI_WINDOW_OBJECT::Event(tEvent);
			}

			if (processed && SendMessage(screenID, EM_GETMODIFY, 0, 0))
				woStatus |= WOS_CHANGED;
			break;

		// Ambiguous cases.
		case WM_LBUTTONDOWN:
		case WM_KEYDOWN:
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
		ccode = LogicalEvent(event,
			parent->Inherited(ID_LIST) ? ID_LIST_ITEM : ID_STRING);
		switch (ccode)
		{
		case S_INITIALIZE:
			UI_WINDOW_OBJECT::Event(event);

			// Set the environment specific flag settings.
			dwStyle |= ES_AUTOHSCROLL;
			if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
				dwStyle |= ES_CENTER;
			else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				dwStyle |= ES_RIGHT;
			if (FlagSet(stFlags, STF_LOWER_CASE))
				dwStyle |= ES_LOWERCASE;
			else if (FlagSet(stFlags, STF_UPPER_CASE))
				dwStyle |= ES_UPPERCASE;
			if (FlagSet(stFlags, STF_PASSWORD))
				dwStyle |= ES_PASSWORD;
			dwStyle &= ~WS_DISABLED;

			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
			break;

		case S_CREATE:
			if (FlagSet(woStatus, WOS_UNANSWERED) && !Inherited(ID_FORMATTED_STRING))
				strcpy(text, _blankString);
			// Continue to S_CHANGED.

		case S_CHANGED:
			UI_WINDOW_OBJECT::Event(event);
			if (!FlagSet(woFlags, WOF_BORDER))
			{
				trueRegion.top += (display->preSpace + 1);
				trueRegion.bottom -= (display->postSpace + 1);
			}
			// Continue to S_REGISTER_OBJECT.

		case S_REGISTER_OBJECT:
			RegisterObject("UIW_STRING", "EDIT", &_stringOffset,
				&_stringJumpInstance, &_stringCallback, text);
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
				SendMessage(screenID, EM_LIMITTEXT, maxLength, FALSE);
			break;

		case L_BEGIN_SELECT:
		case L_BEGIN_COPY_DRAG:
		case L_BEGIN_MOVE_DRAG:
			if (FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woStatus, WOS_EDIT_MODE))
				break;
			else if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT) &&
				!Inherited(ID_LIST_ITEM))
			{
				CallWindowProc((WINDOWSPROC)defaultCallback, event.message.hwnd,
					WM_LBUTTONDOWN, event.message.wParam, event.message.lParam);
				CallWindowProc((WINDOWSPROC)defaultCallback, event.message.hwnd,
					WM_LBUTTONUP, event.message.wParam, event.message.lParam);

				SetCapture(screenID);

				if (ccode == L_BEGIN_SELECT)
					ccode = L_BEGIN_MOVE_DRAG;
				windowManager->dragObject = this;
				eventManager->DeviceImage(E_MOUSE,
					ccode == L_BEGIN_MOVE_DRAG ? DM_DRAG_MOVE : DM_DRAG_COPY);
			}
			else
				UI_WINDOW_OBJECT::Event(event);
			break;

		case L_END_SELECT:
			if (event.type == E_MSWINDOWS)
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				break;
			}
			// Else fall through;

		case L_SELECT:
			DataGet();
			ccode = UI_WINDOW_OBJECT::Event(UI_EVENT(ccode));
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) &&
				FlagSet(woFlags, WOF_AUTO_CLEAR))
				SendMessage(screenID, EM_SETSEL, 0, 0x7FFF0000L);
			break;

		case L_PREVIOUS:
		case L_NEXT:
		case L_UP:
		case L_DOWN:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		case L_LEFT:
		case L_RIGHT:
			{
			LRESULT r = SendMessage(screenID, EM_GETSEL, 0, 0);
			WORD start = LOWORD(r);
			WORD end = HIWORD(r);
			LRESULT pos;
			if (ccode == L_LEFT)
				pos = 0;
			else
				pos = SendMessage(screenID, WM_GETTEXTLENGTH, 0, 0);

			// If we are at a boundry allow movement off.
			if (start != pos || end != pos)
				woStatus |= WOS_WINDOWS_ACTION;

			ccode = UI_WINDOW_OBJECT::Event(event);
			woStatus &= ~WOS_WINDOWS_ACTION;
			}
			break;

		case L_BACKSPACE:
		case L_DELETE:
		case L_DELETE_WORD:
		case L_DELETE_EOL:
		case L_CUT:
		case L_PASTE:
			if (!FlagSet(woFlags, WOF_VIEW_ONLY))
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				woStatus |= WOS_CHANGED;
			}
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_STRING::OSDataGet(void)
{
	if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) && !FlagSet(woFlags, WOF_VIEW_ONLY) &&
		!FlagSet(woStatus, WOS_OWNERDRAW))
#if defined(ZIL_UNICODE) 
	{
		int length = (maxLength + 1) * sizeof(ZIL_ICHAR);
		char *tmpBuff = new char[length];
		SendMessage(screenID, WM_GETTEXT, length, (LONG)tmpBuff);
		UnMapText(tmpBuff, text);
		delete tmpBuff;
	}
#else
		SendMessage(screenID, WM_GETTEXT, maxLength + 1, (LONG)text);
#endif
}

void UIW_STRING::OSDataSet(void)
{
	// Redisplay the string field.
	if (parent && screenID)
	{
#if defined(ZIL_UNICODE) 
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
		if (parent->Inherited(ID_COMBO_BOX))
		{
			ZIL_SCREENID tScreenID = screenID;
			SendMessage(screenID, CB_DELETESTRING, ListIndex(), 0);
			SendMessage(screenID, CB_INSERTSTRING, ListIndex(),
				FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)TEXT_);
			if (FlagSet(woStatus, WOS_CURRENT))
				SendMessage(screenID, CB_SETCURSEL, ListIndex(), 0);
			screenID = tScreenID;
		}
		else if (parent->Inherited(ID_LIST))
		{
			int index = ListIndex();
	        if (!FlagSet(parent->woStatus, WOS_OWNERDRAW))
			{
				SendMessage(screenID, LB_DELETESTRING, index, 0);
				SendMessage(screenID, LB_INSERTSTRING, index, (LONG)TEXT_);
				if (FlagSet(woStatus, WOS_CURRENT))
					SendMessage(screenID, LB_SETCARETINDEX, index, 0);
			}
			else
				SendMessage(screenID, LB_SETITEMDATA, index, (LONG)this);
		}
		else if (FlagSet(woStatus, WOS_OWNERDRAW))
		{
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			{
				RECT rect = { 0, 0, trueRegion.Width(), trueRegion.Height() };
				InvalidateRect(screenID, &rect, FALSE);
			}
		}
		else
		{
			SendMessage(screenID, EM_LIMITTEXT, maxLength, FALSE);
			HideCaret(screenID);
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)TEXT_);
			ShowCaret(screenID);
			if (FlagSet(woFlags, WOF_AUTO_CLEAR))
			{
				SendMessage(screenID, EM_SETSEL, 0, 0x7FFF0000L);
			}
		}
	}
}

void UIW_STRING::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (Inherited(ID_LIST_ITEM))
		DataSet(text);

	// See if the field needs to be re-computed.
	if (objectID == ID_STRING && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

