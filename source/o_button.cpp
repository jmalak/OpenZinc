//	Zinc Interface Library - O_BUTTON.CPP
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


#define INCL_DOSRESOURCES		// OS/2 DosGetResource, DosFreeResource
#if !defined(INCL_GPIBITMAPS)
	#define INCL_GPIBITMAPS		// OS/2 GpiQueryBitmapParameters
#endif
#define INCL_WINPOINTERS		// OS/2 WinGetSysBitmap
#define INCL_WINMENUS			// OS/2 OWNERITEM definition
#define INCL_WINBUTTONS			// OS/2 button flags
#define INCL_WINLISTBOXES		// OS/2 list flags
#define USE_RAW_KEYS
#include "ui_win.hpp"

// ----- UIW_BUTTON ---------------------------------------------------------

EVENT_TYPE UIW_BUTTON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif
	const int BUTTON_OFFSET = 4;
	UI_REGION region = trueRegion;
	int listItem = parent->Inherited(ID_LIST);

	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);

	//Get the color palette.
	if (!FlagSet(btFlags, BTF_NO_3D))
		lastPalette = LogicalPalette(ccode, ID_BUTTON);
	else
	{
		WOS_STATUS saveStatus = woStatus;
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
			woStatus &= ~WOS_SELECTED;
		if (listItem)
			lastPalette = LogicalPalette(ccode);
		else
		  	lastPalette = parent->LogicalPalette(ccode);
		woStatus = saveStatus;
	}

	// Draw the background.
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE);

	// Draw Default Button indicator.
	if (!listItem)
	{
		if (FlagSet(btStatus, BTS_DEFAULT) || FlagSet(woStatus, WOS_CURRENT))
			DrawBorder(screenID, region, FALSE, ccode);
		else
			--region;
	}

	// Draw the shadow or border.
	if (!FlagSet(btFlags, BTF_NO_3D))
		DrawShadow(screenID, region, (FlagSet(btStatus, BTS_DEPRESSED) ||
			FlagSet(woStatus, WOS_SELECTED)) ? -depth : depth, FALSE, ccode);
	else if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);

	if (depth && FlagSet(btStatus, BTS_DEPRESSED))
	{
		region.left += depth;
		region.top++;
	}

	// Compute the draw region.
	region.left += BUTTON_OFFSET;
	if (listItem && parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
		region.right -= BUTTON_OFFSET;
	int fieldWidth = region.Width();
	int fieldHeight = region.Height();
	int left = region.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		left = region.right - bitmapWidth + 1;
	else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		left = region.left + (fieldWidth - bitmapWidth) / 2;
	int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && text && *text) ?
		region.top + 2 : region.top + (fieldHeight - bitmapHeight) / 2;

	// Draw the bitmap.
	if (FlagSet(btFlags, BTF_RADIO_BUTTON))
	{
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			myDecorations->GetBitmap(FlagSet(woStatus, WOS_SELECTED) ?
			(bitmapWidth >= 16 ? ZIL_RadioSelectedHiRes : ZIL_RadioSelected) :
			(bitmapWidth >= 16 ? ZIL_RadioNormalHiRes : ZIL_RadioNormal)),
			ZIL_NULLP(UI_PALETTE), ZIL_NULLP(UI_REGION), ZIL_NULLH(ZIL_BITMAP_HANDLE), ZIL_NULLH(ZIL_BITMAP_HANDLE));
	}
	else if (FlagSet(btFlags, BTF_CHECK_BOX))
	{
		RECTL srcRegion;
		srcRegion.xLeft = FlagSet(woStatus, WOS_SELECTED) ? bitmapWidth : 0;
		if (FlagSet(btStatus, BTS_DEPRESSED))
			srcRegion.xLeft += 2 * bitmapWidth;
		srcRegion.yBottom = FlagSet(btFlags, BTF_CHECK_BOX) ? 2 * bitmapHeight : bitmapHeight;
		srcRegion.xRight = srcRegion.xLeft + bitmapWidth;
		srcRegion.yTop = srcRegion.yBottom + bitmapHeight;

		SWP swp;
		WinQueryWindowPos(screenID, &swp);
		POINTL dstPosition;
		dstPosition.x = left - trueRegion.left;
		dstPosition.y = swp.cy - bitmapHeight - top - 1;
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			dstPosition.y += region.top;

		ZIL_BITMAP_HANDLE bitmap = WinGetSysBitmap(HWND_DESKTOP, SBMP_CHECKBOXES);
		WinDrawBitmap(UI_OS2_DISPLAY::hps, bitmap, &srcRegion, &dstPosition,
			0, 0, DBM_NORMAL);
		GpiDeleteBitmap(bitmap);
	}
	else if (colorBitmap && colorBitmap == monoBitmap)
	{
		POINTL dstPosition;
		dstPosition.x = left - trueRegion.left;
		dstPosition.y = trueRegion.Height() - top - bitmapHeight;
		ZIL_BITMAP_HANDLE bitmap = GpiLoadBitmap(UI_OS2_DISPLAY::hps, ZIL_NULLH(HMODULE), colorBitmap, 0, 0);
		WinDrawBitmap(UI_OS2_DISPLAY::hps, bitmap, ZIL_NULLP(RECTL), &dstPosition, 0, 0, DBM_NORMAL);
		GpiDeleteBitmap(bitmap);
	}	
	else if (bitmapArray)
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			bitmapArray, ZIL_NULLP(UI_PALETTE), ZIL_NULLP(UI_REGION), &colorBitmap, &monoBitmap);

	// Draw the text.
	if (text && *text)
	{
		WOF_FLAGS flags = woFlags;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && (colorBitmap || bitmapArray))
			region.top = region.bottom - display->cellHeight + 4;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		{
			region.right -= bitmapWidth - BUTTON_OFFSET;
			woFlags &= ~(WOF_JUSTIFY_RIGHT);
		}
		else
			region.left += bitmapWidth + BUTTON_OFFSET;
		DrawText(screenID, region, text, lastPalette, FALSE, ccode);
		woFlags = flags;

		// Draw the focus rectangle.
		if (FlagSet(woStatus, WOS_CURRENT) && FlagSet(parent->woStatus, WOS_CURRENT))
			DrawFocus(screenID, listItem ? trueRegion : region, ccode);
	}
	else if (listItem && FlagSet(woStatus, WOS_CURRENT))
		DrawFocus(screenID, trueRegion, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_BUTTON::Event(const UI_EVENT &event)
{
	static PFNWP _buttonCallback = ZIL_NULLP(FNWP);
	static int repeat = FALSE;

	// Check for OS/2 specific messages.
	EVENT_TYPE ccode = event.type;
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case BM_SETHILITE:
			if (event.message.mp1)
				btStatus |= BTS_DEPRESSED;
			else
				btStatus &= ~BTS_DEPRESSED;
			if (FlagSet(woStatus, WOS_OWNERDRAW))
			{
				WinEnableWindowUpdate(screenID, FALSE);
				ccode = DefaultCallback(event);
				WinEnableWindowUpdate(screenID, TRUE);
				DrawItem(event, S_DISPLAY_ACTIVE);
				WinUpdateWindow(screenID);
			}
			else
				ccode = DefaultCallback(event);
			break;

		case WM_BUTTON1DOWN:
			if (!FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
				WinSetFocus(HWND_DESKTOP, screenID);
			ccode = UI_WINDOW_OBJECT::Event(event);
			UIW_BUTTON::Event(UI_EVENT(L_BEGIN_SELECT));
			break;

		case WM_BUTTON1DBLCLK:
			if (FlagSet(btFlags, BTF_DOUBLE_CLICK))
			{
				UserFunction(event, L_DOUBLE_CLICK);
				ccode = UI_WINDOW_OBJECT::Event(event);
			}
#if defined(ZIL_EDIT)
			else if (FlagSet(woStatus, WOS_EDIT_MODE))
				ccode = UI_WINDOW_OBJECT::Event(event);
#endif
			else
				ccode = (EVENT_TYPE)WinSendMsg(screenID, WM_BUTTON1DOWN, event.message.mp1, event.message.mp2);
			break;

		case WM_BUTTON1UP:
			UIW_BUTTON::Event(UI_EVENT(L_END_SELECT));
			ccode = DefaultCallback(event);
			break;

		case WM_CHAR:
			ccode = LogicalEvent(event);
			if ((event.rawCode == ENTER || event.rawCode == GRAY_ENTER) &&
				!FlagSet(event.modifiers, KC_KEYUP))
			{
				Event(UI_EVENT(L_SELECT));
				return (0);
			}
			else if (event.rawCode == SPACE)
			{
				if (FlagSet(event.modifiers, KC_KEYUP))
					UIW_BUTTON::Event(UI_EVENT(L_END_SELECT));
				else if (!FlagSet(btStatus, BTS_DEPRESSED))
					UIW_BUTTON::Event(UI_EVENT(L_BEGIN_SELECT));
				ccode = DefaultCallback(event);
			}
			else
			{
				for (int i = 0; hotKeyMapTable[i].objectID != ID_END; i++)
				{
					if (hotKeyMapTable[i].logicalValue == event.key.value)
					{
						MPARAM mp1 = (MPARAM)((hotKeyMapTable[i].rawCode & 0xFF000000) | KC_CHAR | KC_ALT |
							(FlagSet(hotKeyMapTable[i].modifiers, S_SHIFT) ? KC_SHIFT : 0));
						WinPostMsg(parent->screenID, WM_CHAR, mp1, MPFROMSHORT(event.key.value));
						return (0);
					}
				}

				if (ccode == L_LEFT || ccode == L_RIGHT || ccode == L_UP || ccode == L_DOWN)
				{
					parent->Event(UI_EVENT(ccode));
					return (0);
				}

				ccode = UI_WINDOW_OBJECT::Event(event);
			}
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}

		// Return the control code.
		return (ccode);
	}

	// Check for Zinc specific messages.
	UI_EVENT tEvent = event;
	ccode = LogicalEvent(event, ID_BUTTON);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(btFlags, BTF_CHECK_BOX))
			flStyle |= BS_CHECKBOX;
		else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
			flStyle |= BS_RADIOBUTTON;
		else if (!FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
			(colorBitmap || bitmapArray || !FlagSet(btFlags, BTF_NO_TOGGLE) ||
			FlagSet(btFlags, BTF_NO_3D)))
		{
			flStyle |= BS_USERBUTTON;
			woStatus |= WOS_OWNERDRAW;
		}
		else
			flStyle |= BS_PUSHBUTTON;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woStatus, WOS_EDIT_MODE))
			flStyle |= WS_DISABLED;
		if (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			flStyle |= BS_NOPOINTERFOCUS;

		if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent)
			parent->Information(I_SET_DEFAULT_OBJECT, this);

		// See if the bitmap is system specified.
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
		{
			ZIL_BITMAP_HANDLE bitmap = WinGetSysBitmap(HWND_DESKTOP, SBMP_CHECKBOXES);
			BITMAPINFOHEADER info;
			GpiQueryBitmapParameters(bitmap, &info);
			bitmapWidth = info.cx / 4;
			bitmapHeight = info.cy / 3;
			GpiDeleteBitmap(bitmap);
		}
		else if (bitmapName && !colorBitmap)
		{
			// See if it is an application contained bitmap image.
			void *data;
			if (!DosGetResource(ZIL_NULLH(HMODULE), RT_RCDATA, 1000, &data))
			{
				char *buffer = (char *)data;
				while (*buffer && !colorBitmap)
				{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
					ZIL_ICHAR *mappedStr = UnMapText(buffer);
#					define BUFFER mappedStr
#else
#					define BUFFER buffer
#endif
					if (stricmp(BUFFER, bitmapName) == 0)
					{
						colorBitmap = monoBitmap = *(ULONG *)(buffer + ::strlen(buffer) + 1);
						HPS desktop = WinGetPS(HWND_DESKTOP);
						ZIL_BITMAP_HANDLE bitmap = GpiLoadBitmap(desktop, ZIL_NULLH(HMODULE), colorBitmap, 0, 0);
						BITMAPINFOHEADER info;
						GpiQueryBitmapParameters(bitmap, &info);
						bitmapWidth = info.cx;
						bitmapHeight = info.cy;
						GpiDeleteBitmap(bitmap);
						WinReleasePS(desktop);
					}
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
					delete mappedStr;
#endif
					buffer += ::strlen(buffer) + 1 + sizeof(ULONG);
				}
				DosFreeResource(data);
			}
		}
		if (!FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
			(colorBitmap || bitmapArray || !FlagSet(btFlags, BTF_NO_TOGGLE) ||
			FlagSet(btFlags, BTF_NO_3D)))
		{
			flStyle &= ~BS_PUSHBUTTON;
			flStyle |= BS_USERBUTTON;
			woStatus |= WOS_OWNERDRAW;
		}
		if (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			flStyle |= BS_NOPOINTERFOCUS;

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
			}
			height = MaxValue(height, display->cellHeight * 10 / 9);
			relative.top = relative.bottom - height;
		}

		trueRegion = relative;
		break;

	case S_CHANGED:
		trueRegion.bottom = MaxValue(trueRegion.bottom, trueRegion.top + display->cellHeight -
			display->preSpace - display->postSpace - 1);
		break;

	case S_REGISTER_OBJECT:
		RegisterObject("UIW_BUTTON", WC_BUTTON, &_buttonCallback, text);
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && !FlagSet(woStatus, WOS_OWNERDRAW))
		{
			WinSendMsg(screenID, BM_SETCHECK, (MPARAM)(FlagSet(woStatus, WOS_SELECTED) ? 1 : 0), 0);
			ULONG color;
			if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
				ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
				WinSetPresParam(screenID, PP_DISABLEDBACKGROUNDCOLORINDEX, sizeof(color), &color);
		}
		break;

	case S_REDISPLAY:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && !FlagSet(woStatus, WOS_OWNERDRAW))
			WinSendMsg(screenID, BM_SETCHECK, (MPARAM)(FlagSet(woStatus, WOS_SELECTED) ? 1 : 0), 0);
		break;

	case L_BEGIN_SELECT:
		if (FlagSet(btFlags, BTF_REPEAT))
		{
			UserFunction(event, L_SELECT);
			WinSetCapture(HWND_DESKTOP, screenID);
			repeat = TRUE;
			ZIL_TIME lastTime;
			do
			{
				UI_EVENT loopEvent;
				if (!eventManager->Get(loopEvent, Q_NO_BLOCK))
					windowManager->Event(loopEvent);

				if (FlagSet(btStatus, BTS_DEPRESSED) && ZIL_TIME() - lastTime >= repeatRate)
				{
					UserFunction(event, L_SELECT);
					lastTime.Import();
				}
			} while (repeat);
			WinSetCapture(HWND_DESKTOP, ZIL_NULLH(ZIL_SCREENID));
		}
		else if (FlagSet(btFlags, BTF_DOWN_CLICK))
		{
			UserFunction(event, L_SELECT);
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && !FlagSet(woStatus, WOS_OWNERDRAW))
				WinSendMsg(screenID, BM_SETCHECK, (MPARAM)(FlagSet(woStatus, WOS_SELECTED) ? 1 : 0), 0);
		}
		break;

	case L_END_SELECT:
		repeat = FALSE;
		if (FlagSet(btStatus, BTS_DEPRESSED) &&
			!FlagSet(btFlags, BTF_DOWN_CLICK | BTF_DOUBLE_CLICK))
		{
			UserFunction(event, L_SELECT);
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && !FlagSet(woStatus, WOS_OWNERDRAW))
				WinSendMsg(screenID, BM_SETCHECK, (MPARAM)(FlagSet(woStatus, WOS_SELECTED) ? 1 : 0), 0);
		}
		break;

	case L_SELECT:
		UserFunction(event, L_SELECT);
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && !FlagSet(woStatus, WOS_OWNERDRAW))
			WinSendMsg(screenID, BM_SETCHECK, (MPARAM)(FlagSet(woStatus, WOS_SELECTED) ? 1 : 0), 0);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		UI_WINDOW_OBJECT::Event(event);
		if (!FlagSet(btFlags, BTF_RADIO_BUTTON | BTF_CHECK_BOX) && FlagSet(woStatus, WOS_SYSTEM_OBJECT))
		{
			UI_WINDOW_OBJECT *object;
			Root(TRUE)->Information(I_GET_DEFAULT_OBJECT, &object);
			if (object && ccode == S_CURRENT)
				object->Event(UI_EVENT(S_HIDE_DEFAULT));
			WinSendMsg(screenID, BM_SETDEFAULT, (MPARAM)((ccode == S_CURRENT) ? TRUE : FALSE), 0);
			if (object && ccode == S_NON_CURRENT)
				object->Event(UI_EVENT(S_SHOW_DEFAULT));
		}
		break;

	case S_SHOW_DEFAULT:
	case S_HIDE_DEFAULT:
		if (ccode == S_SHOW_DEFAULT)
			btStatus |= BTS_DEFAULT;
		else
			btStatus &= ~BTS_DEFAULT;
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			WinSendMsg(screenID, BM_SETDEFAULT, (MPARAM)((ccode == S_SHOW_DEFAULT) ? TRUE : FALSE), 0);
		break;

	case E_KEY:
		if (event.rawCode == 0xFFFFFFFF && hotKey == event.key.value)
			Event(UI_EVENT(L_SELECT));
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_BUTTON::OSDataSet(void)
{
	if (screenID)
	{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
		if (parent->Inherited(ID_COMBO_BOX) && !FlagSet(woStatus, WOS_OWNERDRAW))
			WinSendMsg(screenID, LM_SETITEMTEXT, (MPARAM)ListIndex(), (MPARAM)TEXT_);
		else if (parent->Inherited(ID_MENU))
		{
			WinSendMsg(screenID, MM_SETITEMTEXT, (MPARAM)numberID, (MPARAM)TEXT_);
			parent->Event(UI_EVENT(S_REDISPLAY, 0));
		}
		else
		{
			if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
				WinSetWindowText(screenID, (PSZ)TEXT_);
			else
				Event(UI_EVENT(S_REDISPLAY));
		}
	}
}

void UIW_BUTTON::OSImageInitialize(void)
{
}

void UIW_BUTTON::OSSystemImage(void)
{
}

void UIW_BUTTON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_BUTTON && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
