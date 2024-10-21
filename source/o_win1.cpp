//	Zinc Interface Library - O_WIN1.CPP
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


#define INCL_WINMENUS			// OS/2 menu messages and flags.
#define INCL_WINSCROLLBARS		// OS/2 scroll messages and flags.
#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#define INCL_WIN
#include "ui_win.hpp"

// ----- UIW_WINDOW ---------------------------------------------------------

static PFNWP _frameCallback = ZIL_NULLP(FNWP);
MRESULT EXPENTRY FrameJumpProcedure(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	UIW_WINDOW *window = (UIW_WINDOW *)WinQueryWindowULong(hwnd, QWL_USER);
	UI_EVENT event(E_OS2, hwnd, msg, mp1, mp2);
	return ((MRESULT)window->FrameEvent(event));
}

UI_WINDOW_OBJECT *UIW_WINDOW::Add(UI_WINDOW_OBJECT *object)
{
	// See if the new object exists or is already current.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
	else if (object == current)
		return (object);

	// Add the object to the list.
	object->parent = this;
	if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
	{
		if (support.Index(object) == -1)
			support.Add(object);
	}
	else if ((!object->Previous() && !object->Next() && object != First()) ||
		 UI_LIST::Index(object) == -1)
	{
		UI_LIST::Add(object);

		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT) ||
			FlagSet(object->woStatus, WOS_CURRENT))
		{
			if (FlagSet(woStatus, WOS_CURRENT))
			{
				if (current)
					Current()->woStatus &= ~WOS_CURRENT;
				object->woStatus |= WOS_CURRENT;
			}
			else
				object->woStatus &= ~WOS_CURRENT;
			current = object;
		}
	}
	else if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
		!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
	{
		// Place MDI objects at the end of the list.
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			UI_LIST::Subtract(object);
			UI_LIST::Add(object);
		}

		// Make object current.
		if (FlagSet(woStatus, WOS_CURRENT))
		{
			UI_EVENT event;
			event.type = S_NON_CURRENT;
			if (current)
				Current()->Event(event);
			current = object;
			event.type = S_CURRENT;
			object->Event(event);
		}
		else
		{
			object->woStatus &= ~WOS_CURRENT;
			current = object;
		}

		// Update object selection.
		if (screenID)
		{
			if (FlagSet(wnFlags, WNF_AUTO_SELECT))
			{
				object->UserFunction(UI_EVENT(S_CURRENT, 0), L_SELECT);
				object->Event(UI_EVENT(S_REDISPLAY, 0));
			}

			if (Inherited(ID_COMBO_BOX) && !FlagSet(woStatus, WOS_OWNERDRAW))
				WinSendMsg(screenID, LM_SELECTITEM, (MPARAM)Index(object), (MPARAM)TRUE);
		}
		return (object);
	}
	else if (Inherited(ID_LIST))
	{
		UI_EVENT event;
		event.type = S_NON_CURRENT;
		if (current)
			Current()->Event(event);
		current = object;
		event.type = S_CURRENT;
		object->Event(event);
	}

	// Draw the object on the screen.
	if (screenID && !object->screenID)
	{
		if (Inherited(ID_LIST))
		{
			object->screenID = screenID;
			object->woStatus |= WOS_OWNERDRAW;
		}

		// Disable updates until redisplay.
		WinEnableWindowUpdate(screenID, FALSE);

		// Create the object.
		object->Event(UI_EVENT(S_INITIALIZE));
		object->Event(UI_EVENT(S_CREATE));
		if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);

		// Updated Selection.
		if (FlagSet(object->woStatus, WOS_SELECTED))
		{
			if (FlagSet(wnFlags, WNF_AUTO_SELECT))
				object->woStatus |= WOS_CURRENT;
			CheckSelection(object);
		}

		// Update Current.
		if (FlagSet(object->woStatus, WOS_CURRENT) || (!current &&
			!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
			!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT)))
		{
			current = object;
			if (FlagSet(woStatus, WOS_CURRENT))
				object->Event(UI_EVENT(S_CURRENT));
			else
				object->woStatus &= ~WOS_CURRENT;
		}
	}

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UIW_WINDOW::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	if (!FlagSet(woStatus, WOS_OWNERDRAW) || Inherited(ID_LIST))
		return (FALSE);

	if (!Inherited(ID_LIST))
	{
		display->VirtualGet(screenID, trueRegion);
		lastPalette = LogicalPalette(ccode);
		display->Rectangle(screenID, trueRegion, lastPalette, 0, TRUE);
		display->VirtualPut(screenID);
	}

	return (FALSE);	// Draw operation is not complete until children are drawn.
}

EVENT_TYPE UIW_WINDOW::Event(const UI_EVENT &event)
{
	static PFNWP _windowCallback = ZIL_NULLP(FNWP);
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	UI_EVENT tEvent = event;
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		switch (event.message.msg)
		{
		case WM_WINDOWPOSCHANGED:
			if (frameID)
			{
				// Temporary fix for help system geometry.
				if (!parent)
					Event(S_CHANGED);

				SWP swp;
				WinQueryWindowPos(frameID, &swp);
				if (!FlagSet(swp.fl, SWP_MINIMIZE))
					Event(UI_EVENT(S_SIZE, TRUE, relative));

				if (FlagSet(swp.fl, SWP_MINIMIZE))
					woStatus |= WOS_MINIMIZED;
				else if (FlagSet(swp.fl, SWP_MAXIMIZE))
					woStatus |= WOS_MAXIMIZED;

				return (DefaultCallback(event));
			}
			else
			{
				clipList.Destroy();
				clipList.Add(new UI_REGION_ELEMENT(screenID, 0, 0,
					trueRegion.Width() - 1, trueRegion.Height() - 1));
				for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
					object->Event(UI_EVENT(S_CHANGED));
			}
			break;

		case WM_CONTROL:
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
		case WM_VSCROLL:
		case WM_HSCROLL:
		case WM_INITMENU:
		case WM_MENUEND:
		case WM_MENUSELECT:
			{
			ZIL_NUMBERID tNumberID = LOWORD(event.message.mp1);
			if (tNumberID != numberID)
				object = Get(tNumberID);
			if (object)
				return (object->Event(event));
			}
			return (0);

		case WM_CREATE:
			if ((!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) || Inherited(ID_LIST) ||
				(Inherited(ID_TABLE) && !Inherited(ID_TABLE_HEADER)) || SearchID() == ID_WINDOW) &&
				!frameID)
			{
				WinSetWindowPtr(screenID, QWP_PFNWP, (PVOID)FrameJumpProcedure);
				return (FrameEvent(event));
			}
			else
				return (DefaultCallback(event));

		case WM_PAINT:
			if (FlagSet(woStatus, WOS_OWNERDRAW))
			{
				RECTL windowRect;
				WinQueryWindowRect(screenID, &windowRect);
				int windowHeight = windowRect.yTop - windowRect.yBottom;

				RECTL updateRect;
				WinQueryUpdateRect(screenID, &updateRect);

				UI_REGION updateRegion;
				updateRegion.left = trueRegion.left + updateRect.xLeft;
				updateRegion.top = trueRegion.top + windowHeight - updateRect.yTop;
				updateRegion.right = trueRegion.left + updateRect.xRight;
				updateRegion.bottom = trueRegion.top + windowHeight - updateRect.yBottom;

				UI_EVENT tEvent = event;
				tEvent.region = updateRegion;

				RECTL rect, rect2;
				UI_OS2_DISPLAY::hps = WinBeginPaint(screenID, ZIL_NULLH(HPS), &rect2);
				WinQueryWindowRect(screenID, &rect);
				display->VirtualGet(ID_DIRECT, trueRegion.left, trueRegion.top,
					trueRegion.right, trueRegion.top + rect.yTop - rect.yBottom - 1);
				ccode = DrawItem(tEvent, S_CURRENT);
				WinEndPaint(UI_OS2_DISPLAY::hps);
  				display->VirtualPut(ID_DIRECT);
			}
			else
			{
				ULONG colorIndex;
				if (!WinQueryPresParam(screenID, PP_BACKGROUNDCOLORINDEX, 0,
					ZIL_NULLP(ULONG), sizeof(colorIndex), &colorIndex, 0))
					colorIndex = FlagSet(woAdvancedFlags, WOAF_DIALOG_OBJECT) ?
						SYSCLR_DIALOGBACKGROUND : SYSCLR_WINDOW;

				RECTL updateRect;
				HPS hps = WinBeginPaint(screenID, NULLHANDLE, &updateRect);
				WinFillRect(hps, &updateRect, colorIndex);
				WinEndPaint(hps);
			}
			return (0);


#if defined(ZIL_EDIT)
		case WM_BUTTON1DOWN:
		case WM_BUTTON1DBLCLK:
		case WM_BUTTON1UP:
		case WM_BUTTON2DOWN:
		case WM_BUTTON2DBLCLK:
		case WM_BUTTON2UP:
			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				UI_POSITION mousePos = event.position;
				mousePos.column -= trueRegion.left;
				mousePos.line -= trueRegion.top;

				for (object = First(); object; object = object->Next())
					if (object->trueRegion.Overlap(mousePos))
					{
						tEvent.position = mousePos;
						break;
					}
			}
			return (object ? object->Event(tEvent) : UI_WINDOW_OBJECT::Event(event));
#endif

		case WM_CHAR:
			{
			EVENT_TYPE tcode = LogicalEvent(event);
			switch (tcode)
			{
			case L_NEXT:
			case L_PREVIOUS:
				if (Inherited(ID_LIST) || //Inherited(ID_GROUP) ||
					Inherited(ID_TOOL_BAR))
					return parent->Event(event);
				else
				{
					Event(UI_EVENT(tcode, 0));
					return (0);
				}

			case L_UP:
			case L_DOWN:
			case L_RIGHT:
			case L_LEFT:
				return (0);

			case L_MDICHILD_EVENT + L_NEXT:
			case S_MDICHILD_EVENT + S_CLOSE:
				eventManager->Put(UI_EVENT(tcode), Q_BEGIN);
				return (0);

			default:
				{
				ZIL_RAW_CODE hotKey = 0;
				USHORT keyFlags = SHORT1FROMMP(event.message.mp1);
				if (!FlagSet(keyFlags, KC_KEYUP))
				{
					if (FlagSet(event.key.shiftState, S_ALT))
					{
						for (int i = 0; hotKeyMapTable[i].objectID != ID_END; i++)
						{
							if (hotKeyMapTable[i].rawCode == event.rawCode &&
								(hotKeyMapTable[i].modifiers & S_SHIFT) ==
								(event.key.shiftState & S_SHIFT))
							{
								hotKey = hotKeyMapTable[i].logicalValue;
								break;
							}
						}
					}
					else if (FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS))
						hotKey = event.key.value;
				}

				if (hotKey)
				{
					UI_EVENT hotKeyEvent(E_KEY, 0xFFFFFFFF);
					hotKeyEvent.key.value = hotKey;
					if (parent && parent->Inherited(ID_NOTEBOOK))
						Event(hotKeyEvent);
					else
						Root(TRUE)->Event(hotKeyEvent);
					return (0);
				}
				else
					return (DefaultCallback(event));
				}
			}
			}
		}
		return (UI_WINDOW_OBJECT::Event(event));
	}

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_WINDOW);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		flFlag |= FCF_NOBYTEALIGN;
		if (FlagSet(woFlags, WOF_BORDER))
			flFlag |= FCF_BORDER;

		title = ZIL_NULLP(UI_WINDOW_OBJECT);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
		{
			if (FlagSet(object->woStatus, WOS_CURRENT) ||
				(!current && !FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
				 !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
			{
				if (current)
					Current()->woStatus &= ~WOS_CURRENT;
				if (current && FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					Current()->woStatus &= ~WOS_SELECTED;
				current = object;
				if (FlagSet(woStatus, WOS_CURRENT))
					object->woStatus |= WOS_CURRENT;
				if (FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					object->woStatus |= WOS_SELECTED;
			}
			else
				object->woStatus &= ~WOS_CURRENT;
			object->Event(event);
		}
		hScrollInfo.delta = vScrollInfo.delta = display->cellHeight;
		break;

	case S_DEINITIALIZE:
		if (frameID)
		{
			if (FlagSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
			{
				WOS_STATUS saveStatus = woStatus;
				WinShowWindow(frameID, FALSE);
				WinSetWindowPos(frameID, 0, 0, 0, 0, 0, SWP_RESTORE);
				woStatus = saveStatus;
			}
			screenID = frameID;
			frameID = 0;
		}
		UI_WINDOW_OBJECT::Event(event);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		break;

	case S_CREATE:
		{
		// Remove standard support objects from notebook pages.
		if (parent && parent->Inherited(ID_NOTEBOOK))
		{
			flFlag = 0;
			woAdvancedFlags |= WOAF_DIALOG_OBJECT;
		}

		UI_WINDOW_OBJECT::Event(event);

		// Create the support objects.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);

		if (frameID)
			WinSendMsg(frameID, WM_UPDATEFRAME, (MPARAM)flFlag, (MPARAM)0);

		// Initialize the clip list.
		RECTL rect;
		WinQueryWindowRect(screenID, &rect);
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, 0, 0,
			rect.xRight - rect.xLeft - 1, rect.yTop - rect.yBottom - 1));

		// Create the field regions.
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
		}

		if (FlagSet(woStatus, WOS_MAXIMIZED))
		{
			woStatus &= ~WOS_MAXIMIZED;
			Event(UI_EVENT(S_MAXIMIZE));
		}
		else if (FlagSet(woStatus, WOS_MINIMIZED))
		{
			woStatus &= ~WOS_MINIMIZED;
			Event(UI_EVENT(S_MINIMIZE));
		}

		if (!FlagSet(wnFlags, WNF_NO_SCROLL))
		{
			UI_EVENT scrollEvent(S_SCROLLRANGE);
			ScrollEvent(scrollEvent);
		}
		}
		break;

	case S_CHANGED:
		if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			// Specify a minimum size.
			int minWidth = 10 * display->cellWidth;
			if (trueRegion.Width() < minWidth)
				trueRegion.right = trueRegion.left + minWidth - 1;
			int minHeight = display->cellHeight;
			minHeight += 2 * UIW_BORDER::width;
			if (trueRegion.Height() < minHeight)
				trueRegion.bottom = trueRegion.top + minHeight - 1;
		}
		break;

	case S_SIZE:
		if (event.rawCode)
		{
			if (parent && parent->Inherited(ID_NOTEBOOK))
				parent->Information(I_GET_CLIPREGION, &trueRegion);

			for (object = First(); object; object = object->Next())
			{
				object->Event(UI_EVENT(S_SIZE, 0, object->relative));
				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
			}

			if (!FlagSet(wnFlags, WNF_NO_SCROLL))
			{
				UI_EVENT scrollEvent(S_SCROLLRANGE);
				ScrollEvent(scrollEvent);
			}
		}
		else if (FlagSet(woStatus, WOS_MAXIMIZED) && parent)
		{
			WinEnableWindowUpdate(frameID, FALSE);
			UI_WINDOW_OBJECT::Event(event);
			WinSetWindowUShort(frameID, QWS_YRESTORE, clip.bottom - restore.bottom - ((UIW_WINDOW *)parent)->scroll.top);
			WinSetWindowPos(frameID, 0, 0, 0, 0, 0, SWP_RESTORE);
			WinSetWindowPos(frameID, 0, 0, 0, 0, 0, SWP_MAXIMIZE);
			WinEnableWindowUpdate(frameID, TRUE);
		}
		else
			UI_WINDOW_OBJECT::Event(event);
		break;

	case S_REGISTER_OBJECT:
		if (!screenID)
		{
			if (parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
				!Inherited(ID_LIST) && !Inherited(ID_TABLE) && SearchID() != ID_WINDOW)
				return (UI_WINDOW_OBJECT::Event(event));

			// Ajust Size for notebooks.
			if (first && First()->Inherited(ID_NOTEBOOK))
			{
				int deltaWidth = (relative.Width() + 2 * display->cellWidth - trueRegion.Width()) / 2;
				int deltaHeight = (relative.Height() + 2 * display->cellHeight - trueRegion.Height()) / 2;
				trueRegion.left -= deltaWidth;
				trueRegion.right += deltaWidth;
				trueRegion.top -= deltaHeight;
				trueRegion.bottom += deltaHeight;
			}

			// Register the frame.
			RegisterObject("UIW_WINDOW", WC_FRAME, &_frameCallback,
				(title ? (ZIL_ICHAR *)title->Information(I_GET_TEXT, ZIL_NULLP(void)) : ZIL_NULLP(ZIL_ICHAR)));
			frameID = screenID;
			screenID = 0;

			// Register the client.
			ZIL_NUMBERID saveID = numberID;
			numberID = FID_CLIENT;
			RegisterObject("UI_WINDOW_OBJECT", 0, &_windowCallback,
				ZIL_NULLP(ZIL_ICHAR), ZIL_NULLP(void), frameID);
			numberID = saveID;

			ULONG value = FlagSet(woAdvancedFlags, WOAF_DIALOG_OBJECT) ?
				SYSCLR_DIALOGBACKGROUND : SYSCLR_WINDOW;
				WinSetPresParam(frameID, PP_BACKGROUNDCOLORINDEX, sizeof(value), &value);
		}
		break;

	case S_VSCROLL:
	case S_HSCROLL:
	case S_VSCROLL_CHECK:
	case S_HSCROLL_CHECK:
	case S_SCROLLRANGE:
		{
		UI_EVENT scrollEvent = event;
		ccode = ScrollEvent(scrollEvent);
		}
		break;

	case S_ADD_OBJECT:
		Add(event.windowObject);
		break;

	case S_SUBTRACT_OBJECT:
		Subtract(event.windowObject);
		break;

	case S_MDICHILD_EVENT + S_CLOSE:
		{
		UI_WINDOW_OBJECT *object = Current();
	 	if (object && object->Inherited(ID_PULL_DOWN_MENU))
	 		object = (UI_WINDOW_OBJECT *)object->userObject;
		if (object && FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			Subtract(object);
			if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
				delete object;
		}
		}
		break;

	case S_MDICHILD_EVENT + S_RESTORE:
	case S_MDICHILD_EVENT + S_MAXIMIZE:
	case S_MDICHILD_EVENT + S_MINIMIZE:
		{
		UI_WINDOW_OBJECT *object = Current();
	 	if (object && object->Inherited(ID_PULL_DOWN_MENU))
	 		object = (UI_WINDOW_OBJECT *)object->userObject;
		if (object && FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
			object->Event(UI_EVENT(ccode - S_MDICHILD_EVENT, 0));
		}
		break;

	case L_MDICHILD_EVENT + L_NEXT:
		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			UIW_WINDOW::Add(First());
		break;

	case L_NEXT:
		if (current)
		{
			// Don't move if on a support object.
			if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
				UI_ERROR_SYSTEM::Beep();
			else
			{
				// Go to the next field in the window.
				object = Current();
				do
				{
					if (object->Next())
						object = object->Next();
					else if (!FlagSet(wnFlags, WNF_NO_WRAP))
						object = First();
					else
						return (parent ? parent->Event(event) : S_UNKNOWN);

					if (object != current &&
						!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
						!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
					{
						UIW_WINDOW::Add(object);
						return (ccode);
					}
				} while (object != current);
			}
		}
		return (S_UNKNOWN);

	case L_PREVIOUS:
		if (current)
		{
			// Don't move if on a support object.
			if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
				UI_ERROR_SYSTEM::Beep();
			else
			{
				// Go to the previous field in the window.
				object = Current();
				do
				{
					if (object->Previous())
						object = object->Previous();
					else if (!FlagSet(wnFlags, WNF_NO_WRAP))
						object = Last();
					else
						return (parent ? parent->Event(event) : S_UNKNOWN);

					if (object != current &&
						!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
						!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
					{
						UIW_WINDOW::Add(object);
						return (ccode);
					}
				} while (object != current);
			}
		}
		return (S_UNKNOWN);

	case L_UP:
	case L_DOWN:
	case L_LEFT:
	case L_RIGHT:
		if (current)
		{
			if (event.type == E_OS2)
				ccode = Current()->Event(event);
			else
			{
				UI_WINDOW_OBJECT *nextObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				UI_WINDOW_OBJECT *wrapObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				{
					// Is it an object that can get focus.
					if (object == Current() ||
						FlagSet(object->woFlags, WOF_NON_SELECTABLE) ||
						FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
							continue;

					if (ccode == L_LEFT && !(object->trueRegion.top > Current()->trueRegion.bottom ||
						object->trueRegion.bottom < Current()->trueRegion.top))
					{
						if (object->trueRegion.right < Current()->trueRegion.right)
						{
							if (!nextObject || object->trueRegion.right > nextObject->trueRegion.right)
								nextObject = object;
						}
						else if (!wrapObject || object->trueRegion.right > wrapObject->trueRegion.right)
							wrapObject = object;
					}
					else if (ccode == L_RIGHT && !(object->trueRegion.top > Current()->trueRegion.bottom ||
						object->trueRegion.bottom < Current()->trueRegion.top))
					{
						if (object->trueRegion.left > Current()->trueRegion.left)
						{
							if (!nextObject || object->trueRegion.left < nextObject->trueRegion.left)
								nextObject = object;
						}
						else if (!wrapObject || object->trueRegion.left < wrapObject->trueRegion.left)
							wrapObject = object;
					}
					else if (ccode == L_UP && !(object->trueRegion.left > Current()->trueRegion.right ||
						object->trueRegion.right < Current()->trueRegion.left))
					{
						if (object->trueRegion.bottom < Current()->trueRegion.bottom)
						{
							if (!nextObject || object->trueRegion.bottom > nextObject->trueRegion.bottom)
								nextObject = object;
						}
						else if (!wrapObject || object->trueRegion.bottom > wrapObject->trueRegion.bottom)
							wrapObject = object;
					}
					else if (ccode == L_DOWN && !(object->trueRegion.left > Current()->trueRegion.right ||
						object->trueRegion.right < Current()->trueRegion.left))
					{
						if (object->trueRegion.top > Current()->trueRegion.top)
						{
							if (!nextObject || object->trueRegion.top < nextObject->trueRegion.top)
								nextObject = object;
						}
						else if (!wrapObject || object->trueRegion.top < wrapObject->trueRegion.top)
							wrapObject = object;
					}
				}

				if (nextObject || (!FlagSet(wnFlags, WNF_NO_WRAP) && wrapObject))
					Add(nextObject ? nextObject : wrapObject);
			}
		}
		break;

	case S_RESTORE:
	case S_MAXIMIZE:
	case S_MINIMIZE:
		if (ccode == S_MAXIMIZE)
			WinSetWindowPos(frameID, 0, 0, 0, 0, 0, SWP_MAXIMIZE);
		else if (ccode == S_MINIMIZE)
		{
			WinSetWindowPos(frameID, 0, 0, 0, 0, 0, SWP_MINIMIZE);
			if (!parent && next)
				windowManager->Add(Next());
		}
		else
		{
			if (parent)
				WinSetWindowUShort(frameID, QWS_YRESTORE, clip.bottom - restore.bottom - ((UIW_WINDOW *)parent)->scroll.top);
			WinSetWindowPos(frameID, 0, 0, 0, 0, 0, SWP_RESTORE);
		}
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (defaultObject)
			defaultObject->Event(UI_EVENT(ccode == S_CURRENT ? S_SHOW_DEFAULT : S_HIDE_DEFAULT));
		if (current)
			Current()->Event(event);
		break;

	case S_REDISPLAY:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_HELP:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case E_KEY:
		// Test for hot-key event.
		if (event.rawCode == 0xFFFFFFFF)
		{
			object = support.First() ? (UI_WINDOW_OBJECT *)support.First() : First();
			while (object)
			{
				if (object->hotKey == event.key.value)
				{
					Add(object);
					object->Event(event);
					return (ccode);
				}
				else if (object->hotKey == HOT_KEY_SUB_WINDOW)
				{
					EVENT_TYPE test = object->Event(event);
					if (test == ccode)
					{
						Add(object);
						return (ccode);
					}
				}
				if (object == support.Last())
					object = First();
				else
					object = object->Next();
			}
			return (0);
		}
		else
			ccode = current ? Current()->Event(event) : UI_WINDOW_OBJECT::Event(event);
		break;

	case S_DROP_DEFAULT:
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_SELECT:
		if (defaultObject)
			ccode = defaultObject->Event(UI_EVENT(L_SELECT));
		else
			ccode = S_UNKNOWN;
		break;

	default:
		ccode = current ? Current()->Event(event) : UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_WINDOW::FrameEvent(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	UI_EVENT tEvent = event;

	EVENT_TYPE ccode = event.type;

	switch (event.message.msg)
	{
	case WM_PAINT:
		if (FlagSet(woStatus, WOS_MINIMIZED) && !WinSendMsg(frameID, WM_QUERYICON, (MPARAM)0, (MPARAM)0))
		{
			HPOINTER tempIcon = WinQuerySysPointer(HWND_DESKTOP, SPTR_APPICON, FALSE);
			WinSendMsg(frameID, WM_SETICON, (MPARAM)tempIcon, (MPARAM)0);
			ccode = (EVENT_TYPE)_frameCallback(event.message.hwnd,
				event.message.msg, event.message.mp1, event.message.mp2);
			WinSendMsg(frameID, WM_SETICON, (MPARAM)0, (MPARAM)0);
			return (ccode);
		}
		break;

	case WM_CONTROL:
		{
		ZIL_NUMBERID tNumberID = LOWORD(event.message.mp1);
		if (tNumberID != numberID)
			object = (UI_WINDOW_OBJECT *)Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&tNumberID));
		if (object)
			return (object->Event(event));
		}
		break;

	case WM_DRAWITEM:
	case WM_MEASUREITEM:
		{
		ZIL_NUMBERID tNumberID = LOWORD(event.message.mp1);
		if (tNumberID != numberID)
			object = (UI_WINDOW_OBJECT *)Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&tNumberID));
		if (object)
			return (object->Event(event));
		}
		break;

	case WM_WINDOWPOSCHANGED:
		{
		SWP *swp = (SWP *)event.message.mp1;
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			if (FlagSet(swp[0].fl, SWP_MAXIMIZE | SWP_MINIMIZE) &&
				!FlagSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
					restore = trueRegion;
			if (FlagSet(swp[0].fl, SWP_MOVE))
			{
				int xDelta = (short)swp[0].x - (short)swp[1].x;
				int yDelta = (short)swp[1].y - (short)swp[0].y;
				trueRegion.left += xDelta;
				trueRegion.top += yDelta;
				trueRegion.right += xDelta;
				trueRegion.bottom += yDelta;
				relative.left += xDelta;
				relative.top += yDelta;
				relative.right += xDelta;
				relative.bottom += yDelta;
			}
			if (FlagSet(swp[0].fl, SWP_SIZE))
			{
				trueRegion.right = trueRegion.left + swp[0].cx - 1;
				trueRegion.top = trueRegion.bottom - swp[0].cy + 1;
				relative.right = relative.left + swp[0].cx - 1;
				relative.top = relative.bottom - swp[0].cy + 1;

				// Ajust Size for notebooks.
				if (first && First()->Inherited(ID_NOTEBOOK))
				{
					relative.right -= 2 * display->cellWidth;
					relative.top += 2 * display->cellHeight;
				}
			}

			if (parent)
			{
				UI_EVENT scrollEvent(S_SCROLLRANGE);
				parent->Event(scrollEvent);
			}
		}

		if (FlagSet(swp[0].fl, SWP_MINIMIZE))
			woStatus |= WOS_MINIMIZED;
		else if (FlagSet(swp[0].fl, SWP_MAXIMIZE))
			woStatus |= WOS_MAXIMIZED;
		else if (FlagSet(swp[0].fl, SWP_RESTORE))
			woStatus &= ~WOS_MINIMIZED & ~WOS_MAXIMIZED;
		}
		break;

	case WM_CALCVALIDRECTS:
		return (CVR_REDRAW);

	case WM_QUERYFRAMECTLCOUNT:
		{
		int count = SHORT1FROMMP(_frameCallback(event.message.hwnd, event.message.msg,
			event.message.mp1, event.message.mp2));
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
				count++;
		return (count);
		}

	case WM_FORMATFRAME:
		{
		int count = SHORT1FROMMP(_frameCallback(event.message.hwnd, event.message.msg,
			event.message.mp1, event.message.mp2));

		SWP *swp = (SWP *)event.message.mp1;
		SWP *clientSwp = &swp[count - 1];

		int borderWidth = (FlagSet(woFlags, WOF_BORDER)) ? WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER) : 0;
		int borderHeight = (FlagSet(woFlags, WOF_BORDER)) ? WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER) : 0;
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, borderWidth, borderHeight,
			trueRegion.Width() - borderWidth - 1, trueRegion.Height() - borderHeight - 1));

		UI_WINDOW_OBJECT *geometryManager = ZIL_NULLP(UI_WINDOW_OBJECT);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (parent && parent->Inherited(ID_NOTEBOOK) && object->NumberID() >= (ZIL_NUMBERID)0xFF00)
				continue;
			else if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			{
				RegionMax(object);
				object->Event(UI_EVENT(S_CHANGED));
				UI_REGION region;
				if (FlagSet(woStatus, WOS_MINIMIZED))
					region.left = region.top = region.right = region.bottom = -1;
				else
					region = object->trueRegion;

				if (FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION))
				{
					clipList.Destroy();
					clipList.Add(new UI_REGION_ELEMENT(screenID, region));
				}
				else
					clipList.Split(screenID, region, parent ? FALSE : TRUE);

				int swpIndex = -1;
				for (int i = 0; i < count; i++)
					if (swp[i].hwnd == object->screenID)
					{
						swpIndex = i;
						break;
					}

				if (swpIndex == -1)
					if (FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
					{
						swpIndex = count++;
						swp[swpIndex].hwnd = object->screenID;
						swp[swpIndex].fl = SWP_MOVE | SWP_SIZE;
						swp[swpIndex].ulReserved1 = 0;
						swp[swpIndex].ulReserved2 = 0;
					}
					else
						continue;

				swp[swpIndex].x = region.left;
				swp[swpIndex].y = trueRegion.Height() - region.bottom - 1;
				swp[swpIndex].cx = region.Width();
				swp[swpIndex].cy = region.Height();
			}
			else if (object->Inherited(ID_GEOMETRY_MANAGER))
				geometryManager = object;

		UI_REGION clientRegion;
		if (clipList.First())
			clientRegion = clipList.First()->region;
		else
		{
			clientRegion.left = clientRegion.top = 0;
			clientRegion.right = clientRegion.bottom = -1;
		}
		clientSwp->x = clientRegion.left;
		clientSwp->y = trueRegion.Height() - clientRegion.bottom - 1;
		clientSwp->cx = clientRegion.Width();
		clientSwp->cy = clientRegion.Height();

		// Reset clip list.
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, 0, 0,
			clientRegion.Width() - 1, clientRegion.Height() - 1));

		if (geometryManager)
			geometryManager->Event(UI_EVENT(S_CHANGED));
		return (count);
		}

	case WM_COMMAND:
	case WM_SYSCOMMAND:
		{
		if (parent && !FlagSet(flFlag, FCF_SYSMENU | FCF_MINMAX))
		{
			object = parent;
			while (object->parent && !FlagSet(flFlag, FCF_SYSMENU | FCF_MINMAX))
				object = object->parent;
			ZIL_SCREENID windowFrameID;
			object->Information(I_GET_FRAMEID, &windowFrameID);
			WinPostMsg(windowFrameID, event.message.msg, event.message.mp1, event.message.mp2);
			return (0);
		}

		USHORT command = LOWORD(event.message.mp1);
		if ((command == SC_SIZE && FlagSet(woAdvancedFlags, WOAF_NO_SIZE)) ||
			(command == SC_MOVE && FlagSet(woAdvancedFlags, WOAF_NO_MOVE)))
			return (0);
		else if (command == SC_CLOSE)
		{
			if (!FlagSet(woAdvancedFlags, WOAF_LOCKED))
			{
				USHORT source = LOWORD(event.message.mp2);
				if (parent && source == CMDSRC_MENU)
					eventManager->Put(UI_EVENT(S_MDICHILD_EVENT + S_CLOSE));
				else
					eventManager->Put(UI_EVENT(S_CLOSE));
			}
			else if (windowManager->screenID == screenID)
				eventManager->Put(UI_EVENT(L_EXIT_FUNCTION));
			return (0);
		}
		else if (command == SC_NEXTFRAME)
		{
			// Don't allow windows behind a modal window to move to the front.
			if (!FlagSet(woAdvancedFlags, WOAF_MODAL) && next)
			{
				UI_WINDOW_OBJECT *nextWindow = Next();
				while (!FlagSet(nextWindow->woAdvancedFlags, WOAF_MODAL) &&
					nextWindow->Next())
						nextWindow = nextWindow->Next();
				windowManager->Add(nextWindow);
			}
			else
				errorSystem->Beep();
			return (0);
		}
		else if (command == SC_RESTORE && parent)
			WinSetWindowUShort(frameID, QWS_YRESTORE, clip.bottom - restore.bottom - ((UIW_WINDOW *)parent)->scroll.top);
		}
		break;

#if defined(ZIL_EDIT)
	case WM_ADJUSTWINDOWPOS:
		if (!parent && FlagSet(woStatus, WOS_EDIT_MODE))
		{
			SWP *swp = (SWP *)event.message.mp1;
			if ((short)swp->x < 0)  // Must type cast to get negative value.
				swp->x = 0;
			if ((short)swp->y + swp->cy > display->lines)
				swp->y = display->lines - swp->cy;
			if (!FlagSet(woFlags, WOF_PIXEL))
			{
				int cellWidth = display->cellWidth;
				int cellHeight = display->cellHeight;
				int left = swp->x;
				int top = display->lines - swp->y - swp->cy;
				int width = swp->cx;
				int height = swp->cy;
				if (FlagSet(woFlags,WOF_MINICELL))
				{
					long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
					long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
					left = (left + cellWidth * miniNX / miniDX / 2) * miniDX /
						(miniNX * cellWidth) * miniNX * cellWidth / miniDX;
					top = (top + cellHeight * miniNY / miniDY / 2) * miniDY /
						(miniNY * cellHeight) * miniNY * cellHeight / miniDY;
					width = (width + cellWidth * miniNX / miniDX / 2) * miniDX /
						(miniNX * cellWidth) * miniNX * cellWidth / miniDX;
					height = (height + cellHeight * miniNY / miniDY / 2) * miniDY /
						(miniNY * cellHeight) * miniNY * cellHeight / miniDY;
				}
				else
				{
					left = (left + cellWidth / 2) / cellWidth * cellWidth;
					top = (top + cellHeight / 2) / cellHeight * cellHeight;
					width = (width + cellWidth / 2) / cellWidth * cellWidth;
					height = (height + cellHeight / 2) / cellHeight * cellHeight;
				}
				swp->x = left;
				swp->y = display->lines - top - height;
				swp->cx = width;
				swp->cy = height;
			}
		}
		else
			return ((EVENT_TYPE)_frameCallback(event.message.hwnd,
				event.message.msg, event.message.mp1, event.message.mp2));
		break;
#endif

	case WM_INITMENU:
		// Configure size, move and close system menu options.
		if (LOWORD(event.message.mp1) == SC_SYSMENU)
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			HWND sysMenu = (HWND)event.message.mp2;
			if (sysMenu)
			{
				if (FlagSet(woAdvancedFlags, WOAF_NO_SIZE))
					WinSendMsg(sysMenu, MM_SETITEMATTR, MPFROM2SHORT(SC_SIZE, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
				if (FlagSet(woAdvancedFlags, WOAF_NO_MOVE))
					WinSendMsg(sysMenu, MM_SETITEMATTR, MPFROM2SHORT(SC_MOVE, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
				if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
					WinSendMsg(sysMenu, MM_SETITEMATTR, MPFROM2SHORT(SC_CLOSE, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
			}
			return ccode;
		}
		break;

	case WM_FOCUSCHANGE:
		// Prevent focus change if there is a modal window.
	 	if (!parent && LOWORD(event.message.mp2) &&
			!FlagSet(HIWORD(event.message.mp2), FC_NOSETFOCUS))
		{
			UI_WINDOW_OBJECT *firstObject = windowManager->First();
			if (firstObject != this &&
				FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL) &&
				FlagSet(firstObject->woStatus, WOS_SYSTEM_OBJECT))
			{
				UI_EVENT addEvent(S_ADD_OBJECT);
				addEvent.windowObject = firstObject;
				eventManager->Put(addEvent);
				return 0;
			}
		}

		ccode = (EVENT_TYPE)_frameCallback(event.message.hwnd,
			event.message.msg, event.message.mp1, event.message.mp2);
		if (parent)
			WinDefWindowProc(event.message.hwnd, event.message.msg,
				event.message.mp1, event.message.mp2);

		if (LOWORD(event.message.mp2) &&
			!FlagSet(HIWORD(event.message.mp2), FC_NOSETFOCUS))
		{
			tEvent.type = S_ADD_OBJECT;
			tEvent.windowObject = this;
			if (parent)
				parent->Event(tEvent);
			else
				windowManager->Event(tEvent);
		}
		return (ccode);

	case WM_VSCROLL:
	case WM_HSCROLL:
		{
		UI_EVENT sEvent(event.message.msg == WM_VSCROLL ? S_VSCROLL : S_HSCROLL);
		UI_SCROLL_INFORMATION sInfo = (sEvent.type == S_VSCROLL) ?
			vScrollInfo : hScrollInfo;
		int delta = 0;
		switch(HIWORD(event.message.mp2))
		{
		case SB_LINEUP:
			delta = -sInfo.delta;
			break;
		case SB_LINEDOWN:
			delta = sInfo.delta;
			break;
		case SB_PAGEUP:
			delta = -sInfo.showing;
			break;
		case SB_PAGEDOWN:
			delta = sInfo.showing;
			break;
		case SB_SLIDERTRACK:
			delta = sInfo.minimum + LOWORD(event.message.mp2) - sInfo.current;
			break;
		default:
			break;
		}

		if (delta)
		{
			if (sInfo.current + delta < sInfo.minimum)
				delta = sInfo.minimum - sInfo.current;
			else if (sInfo.current + delta > sInfo.maximum)
				delta = sInfo.maximum - sInfo.current;
			sEvent.scroll.delta = delta;
			Event(sEvent);
		}

		// See if scrollrange should be recomputed.
		USHORT scrollCode = HIWORD(event.message.mp2);
		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
			scrollCode == SB_ENDSCROLL || scrollCode == SB_SLIDERPOSITION)
		{
			UI_EVENT scrollEvent(S_SCROLLRANGE);
			ScrollEvent(scrollEvent);
		}
		}
		return(0);

#if defined(ZIL_EDIT)
	case WM_BUTTON1DOWN:
	case WM_BUTTON1DBLCLK:
	case WM_MOUSEMOVE:
		if (FlagSet(woStatus, WOS_EDIT_MODE) && searchID != ID_WINDOW)
			return(UIW_WINDOW::Event(event));
		break;
#endif

	case WM_QUERYTRACKINFO:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			// Set the new current object.
			UI_EVENT setEvent(D_SET_OBJECT);
			setEvent.rawCode = searchID;
			setEvent.windowObject = this;
			eventManager->Put(setEvent);
			break;
		}
#endif
		if (event.message.hwnd == frameID &&
			(FlagSet(woAdvancedFlags, WOAF_NO_MOVE) || FlagSet(woStatus, WOS_MAXIMIZED)))
				return FALSE;
		break;
	}
	return ((EVENT_TYPE)_frameCallback(event.message.hwnd,
		event.message.msg, event.message.mp1, event.message.mp2));
}

void UIW_WINDOW::RegionMax(UI_WINDOW_OBJECT *object)
{
	// Check to see if it is the top level object.
	if (!object->parent)
	{
		clipList.Destroy();
		clip = trueRegion = relative;
		return;
	}

	// Find the appropriate region element.
	UI_REGION region;
	UI_REGION_ELEMENT *element = clipList.First();
	if (element)						// Get first available region.
		region = element->region;
	else								// Determine the update region.
	{
		region.left = region.top = 0;
		region.right = trueRegion.right - trueRegion.left;
		region.bottom = trueRegion.bottom - trueRegion.top;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
	}
	int bottomRegion = FlagsSet(object->woFlags, WOF_NON_FIELD_REGION | WOF_JUSTIFY_RIGHT);
	for ( ; element; element = element->Next())
		if (!bottomRegion && region.top > element->region.top)
			region = element->region;
		else if (bottomRegion && region.bottom < element->region.bottom)
			region = element->region;

	// Compute the actual field region.
	object->clip = region;
	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
		object->trueRegion = region;
	else
	{
		object->trueRegion.left = region.left + scroll.left + object->relative.left;
		object->trueRegion.top = region.top + scroll.top + object->relative.top;
		object->trueRegion.right = region.left + scroll.left + object->relative.right;
		object->trueRegion.bottom = region.top + scroll.top + object->relative.bottom;
	}
}

EVENT_TYPE UIW_WINDOW::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch(ccode)
	{
	case S_SCROLLRANGE:
		{
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
		int minx = 32767, miny = 32767, maxx = 0, maxy = 0;
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			{
				minx = MinValue(minx, object->relative.left);
				miny = MinValue(miny, object->relative.top);
				maxx = MaxValue(maxx, object->relative.right);
				maxy = MaxValue(maxy, object->relative.bottom);
			}

		if (minx < 0)
			minx -= display->cellWidth;
		if (miny < 0)
			miny -= display->cellWidth;
		if (maxx > client.Width())
			maxx += display->cellWidth;
		if (maxy > client.Height())
			maxy += display->cellWidth;

		hScrollInfo.showing = client.Width();
		vScrollInfo.showing = client.Height();
		hScrollInfo.current = -scroll.left;
		vScrollInfo.current = -scroll.top;
		hScrollInfo.minimum = hScrollInfo.current + MinValue(scroll.left + minx, 0);
		vScrollInfo.minimum = vScrollInfo.current + MinValue(scroll.top + miny, 0);
		hScrollInfo.maximum = hScrollInfo.current +
			MaxValue(scroll.left + maxx - hScrollInfo.showing, 0);
		vScrollInfo.maximum = vScrollInfo.current +
			MaxValue(scroll.top + maxy - vScrollInfo.showing, 0);

		scroll.right = scroll.left + MaxValue(0, minx) + (maxx - minx);
		scroll.bottom = scroll.top + MaxValue(0, miny) + (maxy - miny);

		event.type = S_VSCROLL_SET;
		event.scroll = vScrollInfo;
		ScrollEvent(event);
		event.type = S_HSCROLL_SET;
		event.scroll = hScrollInfo;
		ScrollEvent(event);
		}
		break;

	case S_VSCROLL_SET:
		// Set the scroll range and thumb position.
		vScrollInfo = event.scroll;
		if (vScroll)
			vScroll->Event(event);
		break;

	case S_HSCROLL_SET:
		// Set the scroll range and thumb position.
		hScrollInfo = event.scroll;
		if (hScroll)
			hScroll->Event(event);
		break;

	case S_VSCROLL:
	case S_HSCROLL:
		{
		int delta = event.scroll.delta;
		UI_SCROLL_INFORMATION sInfo = ccode == S_VSCROLL ? vScrollInfo : hScrollInfo;

		if (sInfo.current + delta < sInfo.minimum)
			delta = sInfo.minimum - sInfo.current;
		else if (sInfo.current + delta > sInfo.maximum)
			delta = sInfo.maximum - sInfo.current;

		if (delta)
		{
			sInfo.current += delta;

			// Update scroll information and thumb button.
			event.scroll = sInfo;
			event.scroll.delta = delta;
			if (ccode == S_VSCROLL)
			{
				vScrollInfo = sInfo;
				if (vScroll)
					vScroll->Event(event);
				event.type = S_VSCROLL_WINDOW;
				ScrollEvent(event);
			}
			else if (ccode == S_HSCROLL)
			{
				hScrollInfo = sInfo;
				if (hScroll)
					hScroll->Event(event);
				event.type = S_HSCROLL_WINDOW;
				ScrollEvent(event);
			}
//			if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
//				WinUpdateWindow(screenID);
		}
		}
		break;

	case S_VSCROLL_WINDOW:
	case S_HSCROLL_WINDOW:
		{
//		RECTL scrollRect;
//		WinQueryWindowRect(screenID, &scrollRect);
//		RECTL invalidRect = scrollRect;
		if (ccode == S_VSCROLL_WINDOW)
		{
//			RECTL updateRect;
//			if (WinQueryUpdateRect(screenID, &updateRect))
//			{
//				if (event.scroll.delta > 0)
//					scrollRect.yBottom = invalidRect.yTop = updateRect.yTop;
//				else
//					scrollRect.yTop = invalidRect.yBottom = updateRect.yBottom;
//				WinInvalidateRect(screenID, &invalidRect, TRUE);
//			}

			scroll.top -= event.scroll.delta;
			scroll.bottom -= event.scroll.delta;
//			WinScrollWindow(screenID, 0, (LONG)event.scroll.delta, &scrollRect,
			WinScrollWindow(screenID, 0, (LONG)event.scroll.delta, ZIL_NULLP(RECTL),
				ZIL_NULLP(RECTL), ZIL_NULLH(HRGN), ZIL_NULLP(RECTL),
				SW_SCROLLCHILDREN | SW_INVALIDATERGN);
		}
		else
		{
//			RECTL updateRect;
//			if (WinQueryUpdateRect(screenID, &updateRect))
//			{
//				if (event.scroll.delta > 0)
//					scrollRect.xRight = invalidRect.xLeft = updateRect.xLeft;
//				else
//					scrollRect.xLeft = invalidRect.xRight = updateRect.xRight;
//				WinInvalidateRect(screenID, &invalidRect, TRUE);
//			}

			scroll.left -= event.scroll.delta;
			scroll.right -= event.scroll.delta;
//			WinScrollWindow(screenID, -(LONG)event.scroll.delta, 0, &scrollRect,
			WinScrollWindow(screenID, -(LONG)event.scroll.delta, 0, ZIL_NULLP(RECTL),
				ZIL_NULLP(RECTL), ZIL_NULLH(HRGN), ZIL_NULLP(RECTL),
				SW_SCROLLCHILDREN | SW_INVALIDATERGN);
		}
		WinUpdateWindow(screenID);
		}
		break;

	default:
		ccode = S_UNKNOWN;
	}
	return ccode;
}

UI_WINDOW_OBJECT *UIW_WINDOW::Subtract(UI_WINDOW_OBJECT *object)
{
	UI_WINDOW_OBJECT *nextObject = object ? object->Next() : ZIL_NULLP(UI_WINDOW_OBJECT);

	// Remove the object from the list.
	if (!object)
		return (nextObject);

	if (UI_LIST::Index(object) != -1)
	{
		UI_EVENT event(S_DEINITIALIZE);
		event.windowObject = object;
		object->Event(event);

		// Remove the object from the window.
   		UI_LIST::Subtract(object);

		if (object == current)
			current = ZIL_NULLP(UI_WINDOW_OBJECT);
		else if (current && Current()->Inherited(ID_PULL_DOWN_MENU)
			&& object == Current()->userObject)
				Current()->userObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	}
	else if (support.Index(object) != -1)
	{
		UI_EVENT event(S_DEINITIALIZE);
		event.windowObject = object;
		object->Event(event);

		support.Subtract(object);

		if (object == current)
			current = ZIL_NULLP(UI_WINDOW_OBJECT);
		if (object == title)
			title = ZIL_NULLP(UI_WINDOW_OBJECT);
		else if (object == vScroll)
			vScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
		else if (object == hScroll)
			hScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
	}
	else
		return (nextObject);

	// Return a pointer to the next object.
	return (nextObject);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_WINDOW::CheckSelection(UI_WINDOW_OBJECT *selectedObject)
{
	// Check for proper single-select status.
	if (!selectedObject)
		selectedObject = Current();
	if (!selectedObject || !FlagSet(selectedObject->woStatus, WOS_SELECTED) ||
		FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
		return;

	// Make sure other items are not selected.
	int position = 0;
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next(), position++)
	{
		if (object != selectedObject && FlagSet(object->woStatus, WOS_SELECTED))
		{
			object->woStatus &= ~WOS_SELECTED;
			object->Event(UI_EVENT(S_REDISPLAY));
		}
	}
}

void UIW_WINDOW::OSDestroy(void)
{
	UI_WINDOW_OBJECT *object;

	// Destroy the list elements.
	UI_EVENT event(S_DEINITIALIZE);
	WinEnableWindowUpdate(screenID, FALSE);
	for (object = First(); object; object = object->Next())
	{
		event.windowObject = object;
		object->Event(event);
	}
}

void UIW_WINDOW::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the window needs to be re-computed.
	if (objectID == ID_WINDOW && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
		if (FlagSet(woStatus, WOS_CURRENT))
			WinSetActiveWindow(HWND_DESKTOP, frameID ? frameID : screenID);
		WinShowWindow(frameID, TRUE);
	}
}

void UIW_WINDOW::OSInitialize(void)
{
		frameID = 0;
		vScroll = hScroll = title = ZIL_NULLP(UI_WINDOW_OBJECT);
}
