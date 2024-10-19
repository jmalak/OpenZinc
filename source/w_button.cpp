//	Zinc Interface Library - W_BUTTON.CPP
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


#define OEMRESOURCE				// Windows button messages and flags.
#include "ui_win.hpp"
const int BUTTON_OFFSET = 4;

// ----- UIW_BUTTON ---------------------------------------------------------

#if defined(ZIL_MSWINDOWS_CTL3D)
extern "C" LRESULT CALLBACK BtnWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

static int _buttonOffset = -1;
static FARPROC _buttonCallback = (FARPROC)DefWindowProc;
LRESULT FAR PASCAL _export ButtonJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _buttonOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _buttonJumpInstance = (FARPROC)ButtonJumpProcedure;

EVENT_TYPE UIW_BUTTON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif
	const int BITMAP_OFFSET = 2;
	const int BUTTON_OFFSET = Inherited(ID_LIST_ITEM) ? 4 :
		(!FlagSet(btFlags, BTF_NO_3D) || FlagSet(woFlags, WOF_BORDER)) ? 2 : 0;
	UI_REGION region = trueRegion;

	// Find out if it needs focus rect.
	UI_WINDOW_OBJECT *noFocus = NULL;
	for (noFocus = this;
		noFocus && FlagSet(noFocus->woStatus, WOS_CURRENT); noFocus = noFocus->parent)
		;

	// Virtualize the display.
	display->VirtualGet(screenID, region);
	if (FlagSet(btFlags, BTF_NO_3D) && windowID[0] == ID_BUTTON)
		lastPalette = parent->LogicalPalette(ccode);
	else if (Inherited(ID_RADIO_BUTTON))
		lastPalette = LogicalPalette(ccode, ID_RADIO_BUTTON);
	else if (Inherited(ID_CHECK_BOX))
		lastPalette = LogicalPalette(ccode, ID_CHECK_BOX);
	else if (FlagSet(btFlags, BTF_NO_3D))
		lastPalette = LogicalPalette(ccode);
	else
		lastPalette = LogicalPalette(ccode, ID_BUTTON);

	UI_PALETTE palette = *lastPalette;

	// Find out what color to use if the user specified BACKGROUND
	if (lastPalette->colorForeground == BACKGROUND ||
		lastPalette->colorBackground == BACKGROUND)
	{
		palette.fillPattern = PTN_RGB_COLOR;
		if (lastPalette->colorForeground == BACKGROUND)
		{
			UI_WINDOW_OBJECT *fObject = NULL;
			for (fObject = parent; fObject &&
				display->MapColor(fObject->LogicalPalette(ccode), TRUE) == BACKGROUND;
				fObject = fObject->parent)
				;
			palette.colorForeground = !fObject ? GetSysColor(COLOR_WINDOWTEXT) :
				display->MapColor(fObject->LogicalPalette(ccode), TRUE);
		}
		else
			palette.colorForeground = display->MapColor(lastPalette, TRUE);

		if (lastPalette->colorBackground == BACKGROUND)
		{
			UI_WINDOW_OBJECT *bObject = NULL;
			for (bObject = parent; bObject &&
				display->MapColor(bObject->LogicalPalette(ccode), FALSE) == BACKGROUND;
				bObject = bObject->parent)
				;
			palette.colorBackground = !bObject ? GetSysColor(COLOR_WINDOW) :
				display->MapColor(bObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE);
		}
		else
			palette.colorBackground = display->MapColor(lastPalette, FALSE);
	}

	// Draw the heavy border if necessary.
	if (FlagSet(woFlags, WOF_BORDER) || FlagSet(btStatus, BTS_DEFAULT))
		DrawBorder(screenID, region, FALSE, ccode);

	// Draw the object shadow and fill it's region.
	if (!FlagSet(btFlags, BTF_NO_3D))
	{
		DrawBorder(screenID, region, FALSE, ccode);
		if ((FlagSet(woFlags, WOF_BORDER) || FlagSet(btStatus, BTS_DEFAULT)) &&
			(FlagSet(btStatus, BTS_DEPRESSED) ||
			(!FlagSet(btFlags, BTF_NO_TOGGLE) && FlagSet(woStatus, WOS_SELECTED))))
		{
			UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
			display->Line(screenID, region.left, region.top,
				region.right, region.top, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top + 1,
				region.left, region.bottom, darkShadow, 1, FALSE, &clip);
			region.left++;
			region.top++;
		}
		else
			DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ||
				(!FlagSet(btFlags, BTF_NO_TOGGLE) && FlagSet(woStatus, WOS_SELECTED)) ?
				-depth : depth, FALSE, ccode);
	}
	display->Rectangle(screenID, region, &palette, 0, TRUE);
	if ((depth && FlagSet(btStatus, BTS_DEPRESSED)) ||
		(!FlagSet(btFlags, BTF_NO_TOGGLE | BTF_NO_3D) && FlagSet(woStatus, WOS_SELECTED)))
	{
		region.left += depth;
		region.top += depth;
		region.bottom--;
	}

	// Compute the draw region.
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		region.right -= BUTTON_OFFSET;
	if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		region.left += BUTTON_OFFSET;
	if (parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	int left = region.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		left = region.right - bitmapWidth + 1;
	else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		left = region.left + (region.Width() - bitmapWidth) / 2;
	int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && text && *text) ?
		region.top + 2 : region.top + (region.Height() - bitmapHeight) / 2;

	// Draw the bitmap.
#if defined(ZIL_MSWINDOWS_CTL3D)
	if (FlagSet(btFlags, BTF_RADIO_BUTTON))
	{
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			myDecorations->GetBitmap(FlagSet(woStatus, WOS_SELECTED) ?
			ZIL_RadioSelected : ZIL_RadioNormal),
			ZIL_NULLP(UI_PALETTE), ZIL_NULLP(UI_REGION), ZIL_NULLP(ZIL_BITMAP_HANDLE), ZIL_NULLP(ZIL_BITMAP_HANDLE));
	}
	else if (FlagSet(btFlags, BTF_CHECK_BOX))
	{
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			myDecorations->GetBitmap(FlagSet(woStatus, WOS_SELECTED) ?
			ZIL_CheckSelected : ZIL_CheckNormal),
			ZIL_NULLP(UI_PALETTE), ZIL_NULLP(UI_REGION), ZIL_NULLP(ZIL_BITMAP_HANDLE), ZIL_NULLP(ZIL_BITMAP_HANDLE));
	}
#else
	if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
		int xOffset = 0, yOffset = 0;
		if (FlagSet(woStatus, WOS_SELECTED))
			xOffset = bitmapWidth;
		if (FlagSet(btFlags, BTF_RADIO_BUTTON))
			yOffset = bitmapHeight;
		HDC hDC = UI_MSWINDOWS_DISPLAY::hDC;
		HDC hMemDC = CreateCompatibleDC(hDC);
		COLORREF oldForeground = SetTextColor (hDC, display->MapColor(&palette, TRUE));
		COLORREF oldBackground = SetBkColor (hDC, display->MapColor(&palette, FALSE));

		SelectObject(hMemDC, colorBitmap);
		int bLeft = FlagSet(woStatus, WOS_SYSTEM_OBJECT) ? left - trueRegion.left : left;
		int bTop = FlagSet(woStatus, WOS_SYSTEM_OBJECT) ? top - trueRegion.top : top;
		BitBlt(UI_MSWINDOWS_DISPLAY::hDC, bLeft, bTop, bitmapWidth, bitmapHeight,
			hMemDC, xOffset, yOffset, SRCCOPY);
		SetTextColor(hDC, oldForeground);
		SetBkColor(hDC, oldBackground);
		DeleteDC(hMemDC);
	}
#endif
	else if (bitmapArray || colorBitmap)
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			bitmapArray, ZIL_NULLP(UI_PALETTE), &region, &colorBitmap, &monoBitmap);

	// Draw the text.
	if (text && *text)
	{
		UI_REGION maxRegion = region;
		WOF_FLAGS flags = woFlags;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && (bitmapArray || colorBitmap))
			region.top = top + bitmapHeight + BITMAP_OFFSET;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		{
			if (bitmapArray || colorBitmap)
			{
				region.right -= bitmapWidth + BITMAP_OFFSET;
				woFlags &= ~(WOF_JUSTIFY_RIGHT);
			}
			else
			{
				region.right -= BUTTON_OFFSET,
				region.left =
					region.right - display->TextWidth(text, screenID, font);
			}
		}
		else if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.left += bitmapWidth + BITMAP_OFFSET;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) &&
			display->MapColor(&palette, TRUE) == display->MapColor(&palette, FALSE))
		{
			HDC hDC = GetWindowDC(screenID);

			int textWidth = display->TextWidth(text, screenID, font);
			int textHeight = display->TextHeight(text, screenID, font);

			int left = region.left - trueRegion.left;
			if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				left = region.right - textWidth - trueRegion.left;
			else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
				left += (region.Width() - textWidth) / 2;
			region.left = MaxValue(left, region.left - trueRegion.left);
			region.top += (region.Height() - textHeight) / 2;

#if defined(ZIL_UNICODE) 
			char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
			int length = ::strlen(TEXT_);
#else
#			define TEXT_ text
			int length = strlen(TEXT_);
#endif
			SelectObject(hDC, UI_MSWINDOWS_DISPLAY::fontTable[font]);
			::GrayString(hDC, ZIL_NULLH(HBRUSH), ZIL_NULLF(GRAYSTRINGPROC), (DWORD)TEXT_,
				length, left, region.top - trueRegion.top, 0, 0);
			ReleaseDC(screenID, hDC);
			if (!noFocus)
			{
				if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
				{
					region += 2;
					maxRegion.Overlap(region, region);
					display->Rectangle(screenID, region, &palette, 1, FALSE, TRUE);
				}
				else
					DrawFocus(screenID, trueRegion, ccode);
			}
		}
		else
		{
			DrawText(screenID, region, text, &palette, FALSE, ccode);

			// Draw the focus rectangle.
			if (!noFocus)
			{
				if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
				{
					region += 2;
					maxRegion.Overlap(region, region);
					display->Rectangle(screenID, region, &palette, 1, FALSE, TRUE);
				}
				else
					DrawFocus(screenID, trueRegion, ccode);
			}
		}
		woFlags = flags;
	}
	else if (!FlagSet(woStatus, WOS_SYSTEM_OBJECT) && !noFocus)
		DrawFocus(screenID, trueRegion, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_BUTTON::Event(const UI_EVENT &event)
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
		case BM_SETSTATE:
			{
			BTS_STATUS oldStatus = btStatus;
			if (wParam)
				btStatus |= BTS_DEPRESSED;
			else
				btStatus &= ~BTS_DEPRESSED;

			// See if anything changed.
			if (btStatus ^ oldStatus && FlagSet(woStatus, WOS_OWNERDRAW))
			{
				SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
				ccode = UI_WINDOW_OBJECT::Event(event);
				SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
				UI_EVENT tEvent = event;
				tEvent.region = trueRegion;
				lastPalette = LogicalPalette(ccode);
				UI_MSWINDOWS_DISPLAY::hDC = GetWindowDC(screenID);
				display->VirtualGet(ID_DIRECT, trueRegion);
				DrawItem(tEvent, S_DISPLAY_ACTIVE);
				ReleaseDC(screenID, UI_MSWINDOWS_DISPLAY::hDC);
				display->VirtualPut(ID_DIRECT);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			}
			break;

		case BM_SETCHECK:
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
				ccode = UI_WINDOW_OBJECT::Event(event);
			else if (!FlagSet(btFlags, BTF_NO_TOGGLE))
			{
				UI_EVENT tEvent = event;
				tEvent.region = trueRegion;
				lastPalette = LogicalPalette(ccode);
				DrawItem(tEvent, S_DISPLAY_ACTIVE);
			}
			break;

		case WM_SYSCHAR:
			{
			ZIL_RAW_CODE hotCharKey = wParam;
			hotCharKey = ToLower(hotCharKey);
			hotCharKey += (ZIL_RAW_CODE)((lParam & 0x20000000L) >> 18);	// Add the ALT key

			ZIL_LOGICAL_EVENT hotChar = 0;
			if (hotCharKey & 0x800)
				for (int i = 0; hotKeyMapTable[i].objectID != ID_END; i++)
				{
					if (hotKeyMapTable[i].rawCode == hotCharKey)
						hotChar = (unsigned char)hotKeyMapTable[i].logicalValue;
				}
			else if (FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS))				
				hotChar = hotCharKey;
			if (hotChar && hotChar == (ZIL_LOGICAL_EVENT)hotKey)
			{
				ccode = UserFunction(UI_EVENT(E_KEY), L_SELECT);
				if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
					FlagSet(woStatus, WOS_SYSTEM_OBJECT))
					SendMessage(screenID, BM_SETCHECK, FlagSet(woStatus, WOS_SELECTED), 0);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			}
			break;

		case WM_LBUTTONDOWN:
			{
			if (FlagSet(btFlags, BTF_DOWN_CLICK))
				ccode = UserFunction(UI_EVENT(L_BEGIN_SELECT), L_SELECT);
			else if (FlagSet(btFlags, BTF_REPEAT) ||
				FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			{
				UI_EVENT tEvent = event;
				ZIL_TIME currentTime, lastTime;
				lastTime += delayLength;
				while (message != WM_LBUTTONUP)
				{
					if (!tEvent.message.hwnd)
						;
					else if (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
					{
						if (message == WM_LBUTTONDOWN ||
							(message == WM_MOUSEMOVE && tEvent.message.hwnd == screenID))
								SendMessage(screenID, BM_SETSTATE, 1, 0);
						else if (message == WM_MOUSEMOVE && tEvent.message.hwnd != screenID)
							SendMessage(screenID, BM_SETSTATE, 0, 0);
						else if (tEvent.message.hwnd == screenID)
							ccode = UI_WINDOW_OBJECT::Event(tEvent);
						else
							SendMessage(tEvent.message.hwnd, message,
								tEvent.message.wParam, tEvent.message.lParam);
					}
					else if (tEvent.message.hwnd == screenID)
						ccode = UI_WINDOW_OBJECT::Event(tEvent);
					else
						SendMessage(tEvent.message.hwnd, message,
							tEvent.message.wParam, tEvent.message.lParam);
					currentTime.Import();
					long elapsedTime = currentTime - lastTime;
					if (eventManager->Get(tEvent, Q_NO_BLOCK) != -2)
					{
						if (tEvent.type == E_MSWINDOWS)
						    message = tEvent.message.message;

						// Send all user messages to the window manager.
						if (tEvent.type > 9999 || (!FlagSet(btFlags, BTF_REPEAT) &&
							message != WM_LBUTTONUP))
							windowManager->Event(tEvent);
					}
					else if (FlagSet(btFlags, BTF_REPEAT) &&
						FlagSet(btStatus, BTS_DEPRESSED) &&
						elapsedTime > repeatRate)
					{
						UserFunction(UI_EVENT(L_CONTINUE_SELECT), L_SELECT);
						lastTime = currentTime;
					}
				}
				if (FlagSet(btFlags, BTF_REPEAT))
					UserFunction(UI_EVENT(L_END_SELECT), L_SELECT);
				UI_WINDOW_OBJECT::Event(tEvent);
				if (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
				{
					if (FlagSet(btStatus, BTS_DEPRESSED))
						UIW_BUTTON::Event(UI_EVENT(L_END_SELECT));
					SendMessage(screenID, BM_SETSTATE, 0, 0);
				}
			}
			else
				processed = FALSE;
			}
			break;

		case WM_LBUTTONUP:
			if (!FlagSet(woStatus, WOS_EDIT_MODE))
			{
				if (FlagSet(btStatus, BTS_DEPRESSED) &&
					!FlagSet(btFlags, BTF_DOWN_CLICK))
				{
					UIW_BUTTON::Event(UI_EVENT(L_END_SELECT));
					ccode = UI_WINDOW_OBJECT::Event(event);
				}
				else
					ccode = UI_WINDOW_OBJECT::Event(event);
			}
			break;

		case WM_LBUTTONDBLCLK:
			if (!FlagSet(woStatus, WOS_EDIT_MODE) && userFunction)
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				UI_EVENT uEvent = event;
				if (FlagSet(btFlags, BTF_DOUBLE_CLICK))
				{
					ccode = (*userFunction)(this, uEvent, L_DOUBLE_CLICK);
					btStatus &= ~BTS_DEPRESSED;
				}
				else
				{
					uEvent.message.message = WM_LBUTTONDOWN;
					Event(uEvent);
				}
			}
			else
				processed = FALSE;
			break;

		// Ambiguous cases.
		case WM_KEYDOWN:
		case WM_KEYUP:
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
		// Switch on the logical event type.
		ccode = LogicalEvent(event);

		switch (ccode)
		{
		case S_INITIALIZE:
			if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent)
				parent->Information(I_SET_DEFAULT_OBJECT, this);
			if (FlagSet(btFlags, BTF_SEND_MESSAGE) && !userFunction)
				userFunction = UIW_BUTTON::Message;
#if defined(ZIL_MSWINDOWS_CTL3D)
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
			{
				bitmapWidth = 13;
				bitmapHeight = 13;
			}
#endif
			UI_WINDOW_OBJECT::Event(event);
			if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				dwStyle |= BS_LEFTTEXT;
			if (FlagSet(btFlags, BTF_CHECK_BOX))
				dwStyle |= BS_CHECKBOX;
			else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
				dwStyle |= BS_AUTORADIOBUTTON;
			else if ((colorBitmap || bitmapArray || !FlagSet(woFlags, WOF_JUSTIFY_CENTER) ||
				FlagSet(btFlags, BTF_NO_3D) || !FlagSet(btFlags, BTF_NO_TOGGLE)) &&
				!Inherited(ID_LIST_ITEM))
			{
				dwStyle |= BS_OWNERDRAW;
				dwStyle &= ~WS_BORDER;
				woStatus |= WOS_OWNERDRAW;
			}

			// Fall through to S_CREATE.

		case S_CREATE:
			if (ccode == S_CREATE)
				UI_WINDOW_OBJECT::Event(event);
			if (FlagSet(btFlags, BTF_AUTO_SIZE))
			{
				int offset = 2;
				if (FlagSet(woFlags, WOF_BORDER))
					offset += 2;
				if (!FlagSet(btFlags, BTF_NO_3D))
					offset += 6;
				int height = 0;
				if (colorBitmap || bitmapHeight)
				{
					height = bitmapHeight + offset;
					if (text && *text && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
						height += display->cellHeight;
					height = MaxValue(height, display->cellHeight - display->postSpace * 2);
				}
				else
					height = display->cellHeight * 10 / 9;
				trueRegion.top = relative.top = relative.bottom - height;
				if (parent && parent->Inherited(ID_GROUP))
					trueRegion.top -= display->cellHeight / 4;
			}
			if (ccode == S_INITIALIZE)
			{
				relative.bottom = MaxValue(relative.bottom, relative.top + display->cellHeight - display->preSpace - display->postSpace - 1);
				break;
			}

			// Convert the bitmap array to a handle if possible.
			if (bitmapArray && !colorBitmap)
			{
				display->BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight,
					bitmapArray, ZIL_NULLP(UI_PALETTE), &colorBitmap, &monoBitmap);
				if (colorBitmap && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
				{
					delete bitmapArray;
					bitmapArray = ZIL_NULLP(ZIL_UINT8);
				}
			}

			if ((colorBitmap || bitmapArray || !FlagSet(btFlags, BTF_NO_TOGGLE) ||
				FlagSet(btFlags, BTF_NO_3D) || !FlagSet(woFlags, WOF_JUSTIFY_CENTER)) &&
				(!FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && !Inherited(ID_LIST_ITEM)))
			{
				woStatus |= WOS_OWNERDRAW;
				dwStyle |= BS_OWNERDRAW;
			}
			// Continue to S_REGISTER_OBJECT.
		case S_REGISTER_OBJECT:
			{
			ZIL_ICHAR *_text = text && (!FlagSet(woStatus, WOS_OWNERDRAW) ||
				Inherited(ID_LIST_ITEM)) ? text : _blankString;
			RegisterObject("UIW_BUTTON", "BUTTON", &_buttonOffset,
				&_buttonJumpInstance, &_buttonCallback, _text);
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) &&
				FlagSet(woStatus, WOS_SELECTED) &&
				FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
					SendMessage(screenID, BM_SETCHECK, 1, 0);
			}
			break;

		case S_CURRENT:
		case S_NON_CURRENT:
			{
			UI_WINDOW_OBJECT::Event(event);
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) ||
				!FlagSet(woStatus, WOS_SYSTEM_OBJECT))
					break;
			UI_WINDOW_OBJECT *object;
			Root(TRUE)->Information(I_GET_DEFAULT_OBJECT, &object);
			if (object && ccode == S_CURRENT)
			{
				object->Event(UI_EVENT(S_HIDE_DEFAULT));
				object->Event(UI_EVENT(S_REDISPLAY));
			}
			Event(UI_EVENT(ccode == S_CURRENT ? S_SHOW_DEFAULT : S_HIDE_DEFAULT));
			Event(UI_EVENT(S_REDISPLAY));
			if (object && ccode == S_NON_CURRENT)
			{
				object->Event(UI_EVENT(S_SHOW_DEFAULT));
				object->Event(UI_EVENT(S_REDISPLAY));
			}
			}
			break;

		case S_SHOW_DEFAULT:
		case S_HIDE_DEFAULT:
			if (ccode == S_SHOW_DEFAULT)
				btStatus |= BTS_DEFAULT;
			else
				btStatus &= ~BTS_DEFAULT;
			if (!FlagSet(woStatus, WOS_SYSTEM_OBJECT) ||
				FlagSet(btFlags, BTF_NO_3D))
				break;
			dwStyle &= 0xFFFFFFF0L;
			dwStyle |= ccode == S_SHOW_DEFAULT ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
			SendMessage(screenID, BM_SETSTYLE, LOWORD(dwStyle), MAKELPARAM(FALSE, 0));
			break;

		case S_DEINITIALIZE:
			if (FlagSet(btFlags, BTF_DEFAULT_BUTTON))
			{
				UI_WINDOW_OBJECT *object;
				parent->Information(I_GET_DEFAULT_OBJECT, &object);
				if (object == this)
					parent->Information(I_SET_DEFAULT_OBJECT, ZIL_NULLP(UI_WINDOW_OBJECT));
			}
			UI_WINDOW_OBJECT::Event(event);
			break;

		case L_END_SELECT:
		case L_SELECT:
			UserFunction(event, L_SELECT);
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
				FlagSet(woStatus, WOS_SYSTEM_OBJECT))
					SendMessage(screenID, BM_SETCHECK, FlagSet(woStatus, WOS_SELECTED), 0);
			else if (screenID && Inherited(ID_LIST_ITEM))
				SendMessage(screenID, LB_SETSEL, FlagSet(woStatus, WOS_SELECTED), ListIndex());
			else if (FlagsSet(woStatus, WOS_OWNERDRAW | WOS_SYSTEM_OBJECT))
				InvalidateRect(screenID, ZIL_NULLP(RECT), FALSE);
			ccode = -1;
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

void UIW_BUTTON::OSDataSet(void)
{
	if (!parent || !screenID)
		return;

#if defined(ZIL_UNICODE) 
	char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#	define TEXT_ text
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
	else if (parent->Inherited(ID_MENU))
	{
		WORD wFlags = MF_STRING;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			wFlags |= MF_GRAYED;
		MNIF_FLAGS mniFlags = MNIF_NO_FLAGS;
		Information(I_GET_FLAGS, &mniFlags, ID_POP_UP_ITEM);
		if (FlagSet(mniFlags, MNIF_CHECK_MARK) && FlagSet(woStatus, WOS_SELECTED))
			wFlags |= MF_CHECKED;
		UI_WINDOW_OBJECT *first;
		Information(I_GET_FIRST, &first);
		if (first)
			ModifyMenu(menuID, ListIndex(), MF_BYPOSITION | MF_POPUP| wFlags,
				(WORD)first->parent->screenID, TEXT_);
		else
			ModifyMenu(menuID, ListIndex(), MF_BYPOSITION | wFlags, numberID, TEXT_);
		parent->Event(UI_EVENT(S_REDISPLAY, 0));
	}
	else
	{
		if (!parent->Inherited(ID_TABLE_RECORD))
			InvalidateRect(screenID, ZIL_NULLP(RECT), TRUE);
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && screenID)
			SendMessage(screenID, BM_SETCHECK, FlagSet(woStatus, WOS_SELECTED), 0);
		SendMessage(screenID, WM_SETTEXT, 0, (LONG)TEXT_);
	}
}

void UIW_BUTTON::OSImageInitialize(void)
{
}

void UIW_BUTTON::OSSystemImage(void)
{
	if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
#if defined(ZIL_UNICODE) 
		colorBitmap = LoadBitmap(0, (char *)OBM_CHECKBOXES);
#else
		colorBitmap = LoadBitmap(0, (ZIL_ICHAR *)OBM_CHECKBOXES);
#endif
		BITMAP bm;
		GetObject(colorBitmap, sizeof(BITMAP), (LPSTR)&bm);
		bitmapHeight = bm.bmHeight / 3;
		bitmapWidth = bm.bmWidth / 4;
	}
	else if (bitmapName && !colorBitmap)
	{
#if defined(ZIL_UNICODE)
		char tempName[32];
		wcstombs(&tempName[1], bitmapName);
#else
		char tempName[32];
		strcpy(&tempName[1], bitmapName);
#endif
		tempName[0] = '_';
		colorBitmap = LoadBitmap(display->hInstance, &tempName[1]);
		monoBitmap = LoadBitmap(display->hInstance, tempName);
		if (colorBitmap)
		{
			BITMAP bm;
			GetObject(colorBitmap, sizeof(BITMAP), (LPSTR)&bm);
			bitmapHeight = bm.bmHeight;
			bitmapWidth = bm.bmWidth;
		}
	}
}

void UIW_BUTTON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (Inherited(ID_LIST_ITEM))
		DataSet(text);

	if (objectID == ID_BUTTON && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

