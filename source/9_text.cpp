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

static WNDPROC _textCallback = ZIL_NULLF(WNDPROC);
/* START BLOCK COMMENT
**	#if defined(ZIL_MSWINDOWS_CTL3D)
**	extern "C" LRESULT CALLBACK EditWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
**	#endif
END BLOCK COMMENT */

EVENT_TYPE UIW_TEXT::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event;

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

	::DrawText(hDC, text, -1, &rect, DT_WORDBREAK);

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

/* START BLOCK COMMENT
**			if (FlagSet(GetVersion(), 0x80000000) && message >= WM_USER) 		// Win32s Bug
**				switch (message)
**				{
**				case WM_USER+0:
**					message = EM_GETSEL;
**					break;
**				case WM_USER+1:
**					message = EM_SETSEL;
**					break;
**				case WM_USER+2:
**					message = EM_GETRECT;
**					break;
**				case WM_USER+3:
**					message = EM_SETRECT;
**					break;
**				case WM_USER+4:
**					message = EM_SETRECTNP;
**					break;
**				case WM_USER+6:
**					message = EM_LINESCROLL;
**					break;
**				case WM_USER+8:
**					message = EM_GETMODIFY;
**					break;
**				case WM_USER+9:
**					message = EM_SETMODIFY;
**					break;
**				case WM_USER+10:
**					message = EM_GETLINECOUNT;
**					break;
**				case WM_USER+11:
**					message = EM_LINEINDEX;
**					break;
**				case WM_USER+12:
**					message = EM_SETHANDLE;
**					break;
**				case WM_USER+13:
**					message = EM_GETHANDLE;
**					break;
**				case WM_USER+17:
**					message = EM_LINELENGTH;
**					break;
**				case WM_USER+18:
**					message = EM_REPLACESEL;
**					break;
**	//			case WM_USER+19:
**	//				message = EM_SETFONT;
**	//				break;
**				case WM_USER+20:
**					message = EM_GETLINE;
**					break;
**				case WM_USER+21:
**					message = EM_LIMITTEXT;
**					break;
**				case WM_USER+22:
**					message = EM_CANUNDO;
**					break;
**				case WM_USER+23:
**					message = EM_UNDO;
**					break;
**				case WM_USER+24:
**					message = EM_FMTLINES;
**					break;
**				case WM_USER+25:
**					message = EM_LINEFROMCHAR;
**					break;
**	//			case WM_USER+26:
**	//				message = EM_SETWORDBREAK;
**	//				break;
**				case WM_USER+27:
**					message = EM_SETTABSTOPS;
**					break;
**				case WM_USER+28:
**					message = EM_SETPASSWORDCHAR;
**					break;
**				case WM_USER+29:
**					message = EM_EMPTYUNDOBUFFER;
**					break;
**				case WM_USER+30:
**					message = EM_GETFIRSTVISIBLELINE;
**					break;
**				case WM_USER+31:
**					message = EM_SETREADONLY;
**					break;
**				case WM_USER+32:
**					message = EM_SETWORDBREAKPROC;
**					break;
**				case WM_USER+33:
**					message = EM_GETWORDBREAKPROC;
**					break;
**				case WM_USER+34:
**					message = EM_GETPASSWORDCHAR;
**					break;
**				}
END BLOCK COMMENT */

		processed = TRUE;

		switch (message)
		{
		case WM_CTLCOLOREDIT:
			{
			ZIL_SCREENID objectID = (HWND)lParam;
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
					POINT oldP;
					SetBrushOrgEx(objectDC, p.x, p.y, &oldP);
					ccode = (LRESULT)ctlHBrush;
				}
				else
					ccode = FALSE;
			}
			else
				ccode = SendMessage(objectID, message, wParam, lParam);
			}
			break;

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
			else if (wParam < ' ')
				processed = FALSE;
			else
				ccode = UI_WINDOW_OBJECT::Event(event);

			if (processed && SendMessage(screenID, EM_GETMODIFY, 0, 0))
				woStatus |= WOS_CHANGED;
			break;

		case WM_NCDESTROY:
			ccode = UI_WINDOW_OBJECT::Event(event);
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
			RegisterObject("UIW_TEXT", "EDIT", &_textCallback, text);
/* START BLOCK COMMENT
**	#if defined(ZIL_MSWINDOWS_CTL3D)
**				if (FlagSet(woFlags, WOF_BORDER))
**					defaultCallback = (WNDPROC)EditWndProc3d;
**	#endif
END BLOCK COMMENT */
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
			WPARAM delta = event.scroll.delta;
			SendMessage(screenID, EM_LINESCROLL, delta, 0);
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
	int dummy;
	SendMessage(screenID, EM_GETSEL, (WPARAM)&dummy, (LPARAM)&offset);
	position->line = LOWORD(SendMessage(screenID, EM_LINEFROMCHAR, (WPARAM)offset, 0));
	position->column = offset - LOWORD(SendMessage(screenID, EM_LINEINDEX, (WPARAM)position->line, 0));
}

int UIW_TEXT::CursorOffset(int offset)
{
	int dummy;
	SendMessage(screenID, EM_SETSEL, (WPARAM)offset, (LPARAM)offset);
	SendMessage(screenID, EM_GETSEL, (WPARAM)&dummy, (LPARAM)&offset);
	return (offset);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TEXT::OSDataGet(void)
{
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
		SendMessage(screenID, WM_GETTEXT, maxLength + 1, (LONG)text);
}

void UIW_TEXT::OSDataSet(void)
{
	// Redisplay the text field.
	if (screenID)
	{
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
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

