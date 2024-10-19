//	Zinc Interface Library - W_TEXT.CPP
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
const int STRING_OFFSET = 5;

// ----- UIW_TEXT -----------------------------------------------------------

#if defined(ZIL_MSWINDOWS_CTL3D)
extern "C" LRESULT CALLBACK EditWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

static int _textOffset = -1;
static FARPROC _textCallback = (FARPROC)DefWindowProc;
LRESULT FAR PASCAL _export TextJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _textOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _textJumpInstance = (FARPROC)TextJumpProcedure;

EVENT_TYPE UIW_TEXT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);
	lastPalette = LogicalPalette(ccode);

	// Draw the text information.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	HDC hDC = UI_MSWINDOWS_DISPLAY::hDC;
	COLORREF oldForeground, oldBackground;
	if (lastPalette)
	{
		oldForeground = SetTextColor(hDC, display->MapColor(lastPalette, TRUE));
		oldBackground = SetBkColor(hDC, display->MapColor(lastPalette, FALSE));
	}
	RECT rect;
	rect.left = FlagSet(woStatus, WOS_SYSTEM_OBJECT) ? region.left - trueRegion.left : region.left;
	rect.top = FlagSet(woStatus, WOS_SYSTEM_OBJECT) ? region.top - trueRegion.top : region.top;
	rect.right = FlagSet(woStatus, WOS_SYSTEM_OBJECT) ? region.right - trueRegion.left : region.right;
	rect.bottom = FlagSet(woStatus, WOS_SYSTEM_OBJECT) ? region.bottom - trueRegion.top : region.bottom;
	ZIL_LOGICAL_FONT useFont = font ^ FNT_IGNORE_UNDERSCORE;
	HGDIOBJ oldFont = SelectObject(hDC, UI_MSWINDOWS_DISPLAY::fontTable[useFont]);

#if defined(ZIL_UNICODE) 
	char *TEXT_ = MapText(text);
	::DrawText(hDC, TEXT_, -1, &rect, DT_WORDBREAK);
	delete TEXT_;
#else
	::DrawText(hDC, text, -1, &rect, DT_WORDBREAK);
#endif

	if (lastPalette)
	{
		SetTextColor(hDC, oldForeground);
		SetBkColor(hDC, oldBackground);
	}
	SelectObject(hDC, oldFont);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_TEXT::Event(const UI_EVENT &event)
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

		case ZM_CONVERTNCPOINT:
			ccode = UIW_WINDOW::Event(event);
			break;

		case WM_KILLFOCUS:
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) &&
				SendMessage(screenID, EM_GETMODIFY, 0, 0))
					DataGet();
			ccode = UI_WINDOW_OBJECT::Event(event);
			DataGet();
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
			else
				ccode = UI_WINDOW_OBJECT::Event(event);

			if (processed && SendMessage(screenID, EM_GETMODIFY, 0, 0))
				woStatus |= WOS_CHANGED;
			break;

		case WM_COMMAND:
			{
			int notifyCode = HIWORD(event.message.wParam);
			if (notifyCode == EN_CHANGE)
				woStatus |= WOS_CHANGED;
			}
			break;

		case WM_NCDESTROY:
			ccode = UI_WINDOW_OBJECT::Event(event);
			if (ghEditDS)
			{
				GlobalUnlock(ghEditDS);
				GlobalFree(ghEditDS);
			}
			break;

		// Ambiguous cases.
		case WM_LBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_KEYUP:
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
		ccode = LogicalEvent(event, ID_TEXT);
		switch (ccode)
		{
		case S_INITIALIZE:
			UIW_WINDOW::Event(event);
			dwStyle |= ES_AUTOVSCROLL | ES_MULTILINE;
			if (FlagSet(wnFlags, WNF_NO_WRAP))
				dwStyle |= ES_AUTOHSCROLL;
			if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
				dwStyle |= ES_CENTER;
			else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				dwStyle |= ES_RIGHT;
			dwStyle &= ~WS_DISABLED;
			break;

		case S_CREATE:
			UI_WINDOW_OBJECT::Event(event);
			// Continue to S_REGISTER_OBJECT.

		case S_REGISTER_OBJECT:
			{
			if (trueRegion.Height() < display->cellHeight)
			{
				dwStyle &= ~(ES_AUTOVSCROLL | ES_MULTILINE);
				dwStyle |= ES_AUTOHSCROLL;
			}
			HINSTANCE hInstance;
			ZIL_UINT16 mL = maxLength;

			// This algorithm for increasing data space from MS Developer's Network.
			ghEditDS = mL < 0x0100L ? 0 :
				GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, mL + 16);

			if (ghEditDS)
			{
				LPVOID lpPtr = GlobalLock(ghEditDS);
				hInstance = (HINSTANCE)HIWORD((LONG)lpPtr);
				LocalInit((UINT)hInstance, 0, mL);
				UnlockSegment((UINT)hInstance);
			}
			else
				hInstance = display->hInstance;

			RegisterObject("UIW_TEXT", "EDIT", &_textOffset, &_textJumpInstance,
				&_textCallback, 0, (HMENU)hInstance);
#if defined(ZIL_UNICODE)
			char *TEXT_ = MapText(text);
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)TEXT_);
			delete TEXT_;
#else
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
#endif
			SendMessage(screenID, EM_LIMITTEXT, maxLength, 0L);
			}
			break;

		case S_SUBTRACT_OBJECT:
		case S_ADD_OBJECT:
			UIW_WINDOW::Event(event);
			break;

		case S_VSCROLL:
			{
			LPARAM delta = event.scroll.delta;
			SendMessage(screenID, EM_LINESCROLL, 0, delta);
			}
			break;

		case S_HSCROLL:
			{
			LPARAM delta = event.scroll.delta << 16;
			SendMessage(screenID, EM_LINESCROLL, 0, delta);
			}
			break;

		case L_BEGIN_SELECT:
		case L_BEGIN_COPY_DRAG:
			if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
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

		case L_PGUP:
		case L_PGDN:
		case L_LEFT:
		case L_RIGHT:
		case L_UP:
		case L_DOWN:
			woStatus |= WOS_WINDOWS_ACTION;
			ccode = UI_WINDOW_OBJECT::Event(event);
			woStatus &= ~WOS_WINDOWS_ACTION;
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
//				woStatus |= WOS_CHANGED;
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

void UIW_TEXT::SetCursorPos(const UI_POSITION &position)
{
	int lines = (int)SendMessage(screenID, EM_GETLINECOUNT, 0, 0);
	int offset;
	if (position.line > lines)
		offset = (int)SendMessage(screenID, WM_GETTEXTLENGTH, 0, 0);
	else
	{
		offset = (int)SendMessage(screenID, EM_LINEINDEX, (WPARAM)position.line, 0);
		int columns = (int)SendMessage(screenID, EM_LINELENGTH, (WPARAM)position.line, 0);
		if (position.column > columns)
			offset += columns;
		else
			offset += position.column;
	}
	CursorOffset(offset);
}

void UIW_TEXT::GetCursorPos(UI_POSITION *position)
{
	int offset;
	offset = HIWORD(SendMessage(screenID, EM_GETSEL, 0, 0));
	position->line = LOWORD(SendMessage(screenID, EM_LINEFROMCHAR, (WPARAM)offset, 0));
	position->column = offset - LOWORD(SendMessage(screenID, EM_LINEINDEX, (WPARAM)position->line, 0));
}

int UIW_TEXT::CursorOffset(int offset)
{
	LPARAM pos = MAKELONG(offset, offset);
	SendMessage(screenID, EM_SETSEL, 0, pos);
	offset = HIWORD(SendMessage(screenID, EM_GETSEL, 0, 0));
	return (offset);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TEXT::OSDataGet(void)
{
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
#if defined(ZIL_UNICODE) 
	{
		int length = (maxLength + 1 ) * sizeof(ZIL_ICHAR);
		char *tmpBuff = new char[length];
		SendMessage(screenID, WM_GETTEXT, length, (LONG)tmpBuff);
		UnMapText(tmpBuff, text);
		delete tmpBuff;
	}
#else
		SendMessage(screenID, WM_GETTEXT, maxLength + 1, (LONG)text);
#endif
}

void UIW_TEXT::OSDataSet(void)
{
	// Redisplay the text field.
	if (screenID)
	{
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
		{
#if defined(ZIL_UNICODE) 
			char *TEXT_ = MapText(text);
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)TEXT_);
			delete TEXT_;
#else
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
#endif
		}
	}
}

void UIW_TEXT::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TEXT && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

