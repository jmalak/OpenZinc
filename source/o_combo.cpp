//	Zinc Interface Library - O_COMBO.CPP
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


#define INCL_GPI
#define INCL_WINENTRYFIELDS		// OS2/ entry fields.
#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#define INCL_WINPOINTERS		// OS/2 combo-box down arrow.
#define INCL_WINSTDDRAG			// OS/2 drag messages and flags.
#define USE_RAW_KEYS
#include "ui_win.hpp"

static int _buttonWidth = 0;

static MRESULT EXPENTRY ChildCallback(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch (msg)
	{
	case WM_BEGINDRAG:
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

#if defined(ZIL_EDIT)
static MRESULT EXPENTRY EditChildCallback(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
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

EVENT_TYPE UIW_COMBO_BOX::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_REGION region = trueRegion;
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
	UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);

	display->VirtualGet(screenID, trueRegion);

	// Draw the outer shadow region.
	display->Line(screenID, region.right, region.top,
		region.right, region.bottom, lightShadow, 1, FALSE);
	region.right--;
	display->Line(screenID, region.left, region.bottom,
		region.right, region.bottom, lightShadow, 1);
	region.bottom--;
	display->Rectangle(screenID, region, outline, 1);
	--region;

	// Draw the current item information.
	if (list.current)
	{
		UI_WINDOW_OBJECT *object = list.Current();
		ZIL_SCREENID _screenID = object->screenID;
		object->screenID = screenID;
		UI_REGION _trueRegion = object->trueRegion;
		object->trueRegion = region;
		object->trueRegion.right -= _buttonWidth - 2;
		UI_REGION _clip = object->clip;
		object->clip = object->trueRegion;
		WOS_STATUS saveStatus = object->woStatus;
		object->woStatus |= WOS_SYSTEM_OBJECT;
		if (FlagSet(list.flStyle, WS_VISIBLE))
			object->woStatus &= ~WOS_CURRENT;
		object->woStatus &= ~WOS_SELECTED;
		object->DrawItem(event, ccode);
		object->woStatus = saveStatus;
		object->clip = _clip;
		object->trueRegion = _trueRegion;
		object->screenID = _screenID;
	}

	// Draw the button shadow region.
	region.left = region.right - _buttonWidth + 3;
	lastPalette = LogicalPalette(ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	display->Line(screenID, region.left, region.top,
		region.left, region.bottom, outline, 1);
	if (FlagSet(list.flStyle, WS_VISIBLE))
	{
		display->Line(screenID, region.left + 1, region.top,
			region.right + 1, region.top, darkShadow, 1);
		display->Line(screenID, region.left + 1, region.top,
			region.left + 1, region.bottom, darkShadow, 1);
		display->Line(screenID, region.right + 1, region.top + 1,
			region.right + 1, region.bottom, lightShadow, 1);
		region.left += 2;
		region.top += 2;
	}
	else
	{
		display->Line(screenID, region.left + 1, region.top,
			region.right + 1, region.top, lightShadow, 1);
		display->Line(screenID, region.left + 1, region.top,
			region.left + 1, region.bottom, lightShadow, 1);
	}

	// Draw the button information.
	ZIL_BITMAP_HANDLE bitmap = WinGetSysBitmap(HWND_DESKTOP, SBMP_COMBODOWN);
	BITMAPINFOHEADER info;
	GpiQueryBitmapParameters(bitmap, &info);
	POINTL dstPosition;
	dstPosition.x = region.left - trueRegion.left + (region.Width() - info.cx + 1) / 2;
	dstPosition.y = trueRegion.bottom - region.bottom + (region.Height() - info.cy + 1) / 2;
	WinDrawBitmap(UI_OS2_DISPLAY::hps, bitmap, ZIL_NULLP(RECTL), &dstPosition, 0, 0, DBM_NORMAL);
	GpiDeleteBitmap(bitmap);

	display->VirtualPut(screenID);
	return (0);
}

EVENT_TYPE UIW_COMBO_BOX::Event(const UI_EVENT &event)
{
	static PFNWP _comboCallback = ZIL_NULLP(FNWP);
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	EVENT_TYPE ccode = event.type;

	// ----- Check for owner-draw combo-box. -----
	if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		goto OWNERDRAW_COMBOBOX;

	// ----- Code for normal combo-box. -----

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_MOVE:
			WinSendMsg(screenID, CBM_SHOWLIST, (MPARAM)FALSE, (MPARAM)0);
			break;

		case WM_CONTROL:
			{
			ccode = DefaultCallback(event);
			int selection = LOWORD(WinSendMsg(screenID, LM_QUERYSELECTION, (MPARAM)LIT_FIRST, (MPARAM)0));
			USHORT control = HIWORD(event.message.mp1);
			switch (control)
			{
			case CBN_ENTER:
				{
				if (!text)
					text = new ZIL_ICHAR[256];
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
				char tmpBuff[512];
				WinSendMsg(screenID, LM_QUERYITEMTEXT, (MPARAM)((255 << 16) | selection), (MPARAM)tmpBuff);
				WinSetWindowText(screenID, (PSZ)tmpBuff);
				UnMapText(tmpBuff, text);
#else
				WinSendMsg(screenID, LM_QUERYITEMTEXT, (MPARAM)((255 << 16) | selection), (MPARAM)text);
				WinSetWindowText(screenID, (PSZ)text);
#endif
				if (list.Current())
					list.Current()->Event(UI_EVENT(L_SELECT, 0));
				ccode = 0;
				}
				break;

			case CBN_LBSELECT:
				{
				object = (selection >= 0) ? (UI_WINDOW_OBJECT *)list.UI_LIST::Get(selection) : ZIL_NULLP(UI_WINDOW_OBJECT);
				if (object && object != list.Current())
					list.Add(object);
				ccode = 0;
				}
				break;

			case CBN_EFCHANGE:
				woStatus |= WOS_CHANGED;
				ccode = 0;
				break;
			}
			}
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}

		// Return the control code.
		return (ccode);
	}

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_COMBO_BOX);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			flStyle |= CBS_DROPDOWNLIST;
		else
			flStyle |= CBS_DROPDOWN;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woStatus, WOS_EDIT_MODE))
			flStyle |= WS_DISABLED;
		list.Event(event);
		break;

	case S_DEINITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		list.Event(event);
		break;

	case S_CHANGED:
		if (screenID)
			WinSendMsg(screenID, CBM_SHOWLIST, (MPARAM)FALSE, 0);
		break;

	case S_SIZE:
		if (!event.rawCode)
		{
			UI_EVENT tEvent = event;
			int height = tEvent.region.Height();
			tEvent.region.bottom += list.relative.Height();
			UI_WINDOW_OBJECT::Event(tEvent);
			relative.bottom = relative.top + height - 1;
			trueRegion.bottom = trueRegion.top + height - 1;
		}
		break;

	case S_REGISTER_OBJECT:
		{
		int height = trueRegion.Height();
		trueRegion.bottom += list.relative.Height();
		RegisterObject("UIW_COMBO_BOX(string)", WC_COMBOBOX, &_comboCallback, text);
		trueRegion.bottom = trueRegion.top + height - 1;
		list.screenID = screenID;
		for (UI_WINDOW_OBJECT *object = list.First(); object; object = object->Next())
		{
			object->screenID = screenID;
			ZIL_ICHAR *text;
			object->Information(I_GET_TEXT, &text);
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
			char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#			define TEXT_ text
#endif
			WinSendMsg(screenID, LM_INSERTITEM, (MPARAM)LIT_END, (MPARAM)TEXT_);
			if (object == list.Current())
				WinSendMsg(screenID, LM_SELECTITEM, (MPARAM)list.Index(object), (MPARAM)TRUE);
		}

		ZIL_SCREENID childID = WinQueryWindow(screenID, QW_TOP);
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			WinSetWindowULong(childID, QWL_USER, (ULONG)WinSubclassWindow(childID, EditChildCallback));
		else
#endif
			WinSetWindowULong(childID, QWL_USER, (ULONG)WinSubclassWindow(childID, ChildCallback));
		woStatus &= ~WOS_CHANGED;
		}
		break;

	case S_ADD_OBJECT:
		if (event.windowObject != &list)
		{
			list.Event(event);
			if (screenID)
			{
				ZIL_ICHAR *text;
				event.windowObject->Information(I_GET_TEXT, &text);
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
				char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#				define TEXT_ text
#endif
				WinSendMsg(screenID, LM_INSERTITEM, (MPARAM)LIT_END, (MPARAM)TEXT_);
				if (event.windowObject == list.Current())
					WinSendMsg(screenID, LM_SELECTITEM, (MPARAM)list.Index(event.windowObject), (MPARAM)TRUE);
			}
		}
		break;

	case S_SUBTRACT_OBJECT:
	case S_DRAG_DEFAULT:
	case S_DRAG_MOVE_OBJECT:
	case S_DRAG_COPY_OBJECT:
	case S_DROP_DEFAULT:
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		ccode = list.Event(event);
		if (ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_COPY_OBJECT)
			Event(S_REDISPLAY);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);

	// ----- Code for owner-draw combo-box. -----
OWNERDRAW_COMBOBOX:
	// Check for OS/2 specific messages.
	ccode = LogicalEvent(event, ID_COMBO_BOX);
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_FOCUSCHANGE:
			{
			ccode = UI_WINDOW_OBJECT::Event(event);
			ZIL_SCREENID listFrameID;
			list.Information(I_GET_FRAMEID, &listFrameID);
			if (FlagSet(list.flStyle, WS_VISIBLE) && !LOWORD(event.message.mp2) &&
				(HWND)event.message.mp1 != listFrameID &&
				!WinIsChild((HWND)event.message.mp1, listFrameID) &&
				!WinIsChild((HWND)event.message.mp1, screenID))
			{
				list.flStyle &= ~WS_VISIBLE;
				WinShowWindow(listFrameID, FALSE);
			}
			}
			break;

		case WM_CHAR:
			if (event.rawCode == F4 && !FlagSet(LOWORD(event.message.mp1), KC_KEYUP))
			{
				if (!FlagSet(list.flStyle, WS_VISIBLE))
				{
					SWP swp;
					RECTL rect;
					WinQueryWindowPos(screenID, &swp);
					rect.xLeft = swp.x;
					rect.yBottom = swp.y - list.relative.Height();
					rect.xRight = swp.x + swp.cx;
					rect.yTop = swp.y;
					WinMapWindowPoints(parent->screenID, HWND_DESKTOP, (PPOINTL)&rect, 2);

					ZIL_SCREENID listFrameID;
					list.Information(I_GET_FRAMEID, &listFrameID);
					WinSetWindowPos(listFrameID, 0, rect.xLeft, rect.yBottom,
						rect.xRight - rect.xLeft, rect.yTop - rect.yBottom,
						SWP_SIZE | SWP_MOVE);

					list.flStyle |= WS_VISIBLE;
					WinInvalidateRect(screenID, ZIL_NULLP(RECTL), TRUE);
					list.Event(UI_EVENT(S_REDISPLAY));
				}
				else
					Information(I_RESET_SELECTION, this);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		case WM_MOVE:
			ccode = UI_WINDOW_OBJECT::Event(event);
			ZIL_SCREENID listFrameID;
			list.Information(I_GET_FRAMEID, &listFrameID);
			list.flStyle &= ~WS_VISIBLE;
			WinShowWindow(listFrameID, FALSE);
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}

		// Return the control code.
		return (ccode);
	}

	// Switch on the event type.
	switch (ccode)
	{
	case S_INITIALIZE:
		flFlag |= FCF_BORDER;

		_buttonWidth = WinQuerySysValue(HWND_DESKTOP, SV_CXVSCROLL);
		ccode = UI_WINDOW_OBJECT::Event(event);
		list.woFlags |= WOF_BORDER;
		list.Event(event);
		woStatus |= WOS_OWNERDRAW;
		break;

	case S_DEINITIALIZE:
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			UI_EVENT tEvent = event;
			tEvent.windowObject = &list;
			list.Event(tEvent);
		}
		break;

	case S_CREATE:
		{
		list.flStyle |= LS_NOADJUSTPOS;
		list.flStyle &= ~WS_VISIBLE;
		list.parent = this;
		list.Event(event);

		ccode = UI_WINDOW_OBJECT::Event(event);

		ZIL_SCREENID frameID, listFrameID;
		list.Information(I_GET_FRAMEID, &listFrameID);
		Information(I_GET_FRAMEID, &frameID);
		WinSetOwner(listFrameID, frameID);
		WinSetParent(listFrameID, HWND_DESKTOP, FALSE);

		UI_REGION region = trueRegion;
		--region;
		region.right -= _buttonWidth - 2;
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, region.left - trueRegion.left,
			region.top - trueRegion.top, region.right - trueRegion.left, region.bottom - trueRegion.top));
		}
		break;

	case S_CHANGED:
		{
		int height = display->cellHeight - display->preSpace - display->postSpace;
		for (object = list.First(); object; object = object->Next())
			if (object->relative.Height() + 3 > height)
				height = object->relative.Height() + 3;
		trueRegion.bottom = trueRegion.top + height - 1;
		list.trueRegion = list.relative;
		}
		break;

	case S_REGISTER_OBJECT:
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_ADD_OBJECT:
		if (event.windowObject != &list)
			list.Event(event);
		break;

	case S_SUBTRACT_OBJECT:
		list.Event(event);
		break;

	case L_BEGIN_SELECT:
		ccode = UI_WINDOW_OBJECT::Event(event);

		if (WinQueryFocus(HWND_DESKTOP) != screenID)
			WinSetFocus(HWND_DESKTOP, screenID);

		if (!FlagSet(woStatus, WOS_EDIT_MODE))
		{
			UI_POSITION position = event.position;
			position.column -= trueRegion.left;
			position.line -= trueRegion.top;
			if (position.column <= relative.Width() - _buttonWidth)
				break;
			else if (!FlagSet(list.flStyle, WS_VISIBLE))
			{
				SWP swp;
				RECTL rect;
				WinQueryWindowPos(screenID, &swp);
				rect.xLeft = swp.x;
				rect.yBottom = swp.y - list.relative.Height();
				rect.xRight = swp.x + swp.cx;
				rect.yTop = swp.y;
				WinMapWindowPoints(parent->screenID, HWND_DESKTOP, (PPOINTL)&rect, 2);

				ZIL_SCREENID listFrameID;
				list.Information(I_GET_FRAMEID, &listFrameID);
				WinSetWindowPos(listFrameID, 0, rect.xLeft, rect.yBottom,
					rect.xRight - rect.xLeft, rect.yTop - rect.yBottom,
					SWP_SIZE | SWP_MOVE);
				list.flStyle |= WS_VISIBLE;
				WinInvalidateRect(screenID, ZIL_NULLP(RECTL), TRUE);
				list.Event(UI_EVENT(S_REDISPLAY));
			}
			else
			{
				while (ccode != L_END_SELECT)
				{
					UI_EVENT event;
					eventManager->Get(event);
					ccode = LogicalEvent(event, ID_COMBO_BOX);
					windowManager->Event(event);
				}
				Information(I_RESET_SELECTION, this);
			}
		}
		break;

	case L_SELECT:
		if (FlagSet(list.flStyle, WS_VISIBLE))
			Information(I_RESET_SELECTION, this);
		break;

	case L_UP:
	case L_DOWN:
	case L_PGUP:
	case L_PGDN:
		object = list.Current();
		list.Event(UI_EVENT(ccode));
		if (list.Current() != object)
			UI_WINDOW_OBJECT::Event(UI_EVENT(S_REDISPLAY));
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		if (ccode == S_CURRENT)
			list.woStatus |= WOS_CURRENT;
		else
			list.woStatus &= ~WOS_CURRENT;
		list.woStatus &= ~WOS_SYSTEM_OBJECT;
		list.Event(event);
		list.woStatus |= WOS_SYSTEM_OBJECT;
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_DRAG_DEFAULT:
	case S_DRAG_MOVE_OBJECT:
	case S_DRAG_COPY_OBJECT:
	case S_DROP_DEFAULT:
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		ccode = list.Event(event);
		if (ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_COPY_OBJECT)
		{
			list.Event(UI_EVENT(S_CHANGED));
			Event(S_REDISPLAY);
		}
		break;

	case S_REDISPLAY:
		list.Event(UI_EVENT(S_CHANGED));
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_SIZE:
		Information(I_RESET_SELECTION, this);
		UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_COMBO_BOX::OSDataGet(void)
{
	if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
	{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *tmpBuff = new char[256 * sizeof(ZIL_ICHAR)];
		WinQueryWindowText(screenID, 255 * sizeof(ZIL_ICHAR), (PSZ)tmpBuff);
		UnMapText(tmpBuff, text);
		delete tmpBuff;
#else
		WinQueryWindowText(screenID, 255, (PSZ)text);
#endif
	}
}

void UIW_COMBO_BOX::OSDataSet(void)
{
	if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) &&
		!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) &&
		!FlagSet(woFlags, WOF_VIEW_ONLY))
	{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
 		WinSetWindowText(screenID, (PSZ)TEXT_);
 	}
}

void UIW_COMBO_BOX::OSDestroy(void)
{
}

void UIW_COMBO_BOX::OSMatchInput(void)
{
}

void UIW_COMBO_BOX::OSResetSelection(UI_WINDOW_OBJECT *object)
{
	if (screenID && list.screenID && FlagSet(list.flStyle, WS_VISIBLE))
	{
		if (object == this)
		{
			ZIL_SCREENID listFrameID;
			list.Information(I_GET_FRAMEID, &listFrameID);
			list.flStyle &= ~WS_VISIBLE;
			WinShowWindow(listFrameID, FALSE);
			WinInvalidateRect(screenID, ZIL_NULLP(RECTL), TRUE);
		}
		else
		{
			RECTL os2Region;
			os2Region.xLeft = os2Region.yBottom = 0;
			os2Region.yTop = trueRegion.bottom - trueRegion.top;
			os2Region.xRight = trueRegion.right - trueRegion.left - WinQuerySysValue(HWND_DESKTOP, SV_CXVSCROLL);
			WinInvalidateRect(screenID, &os2Region, TRUE);
		}
		woStatus |= WOS_CHANGED;
	}
}

void UIW_COMBO_BOX::OSSort(void)
{
}

void UIW_COMBO_BOX::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		list.Information(I_CHANGED_FLAGS, ZIL_NULLP(void));

	// See if the field needs to be re-computed.
	if (objectID == ID_COMBO_BOX && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
