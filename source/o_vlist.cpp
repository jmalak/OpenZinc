//	Zinc Interface Library - O_HLIST.CPP
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

// ----- UIW_VT_LIST --------------------------------------------------------

EVENT_TYPE UIW_VT_LIST::Event(const UI_EVENT &event)
{
	// OS/2 specific messages.
	EVENT_TYPE ccode = event.type;
	if (ccode == E_OS2)
	{
		int processed = TRUE;
		switch (event.message.msg)
		{
		case WM_BUTTON1DOWN:
		case WM_BUTTON1UP:
		case WM_BUTTON1DBLCLK:
		case WM_MOUSEMOVE:
			processed = FALSE;
			break;

		case WM_CHAR:
			if (Inherited(ID_COMBO_BOX))
				ccode = parent->Event(event);
			else
				processed = FALSE;
			break;

		case WM_PAINT:
			if (!FlagSet(woStatus, WOS_OWNERDRAW) || !DrawItem(event, FlagSet(woStatus, WOS_CURRENT) ?
					S_CURRENT : S_DISPLAY_ACTIVE))
			{
				RECTL windowRect;
				RECTL updateRect;
				WinQueryWindowRect(screenID, &windowRect);
				UI_OS2_DISPLAY::hps = WinBeginPaint(screenID, ZIL_NULLH(HPS), &updateRect);
				display->VirtualGet(ID_DIRECT, 0, 0, windowRect.xRight - windowRect.xLeft - 1,
					windowRect.yTop - windowRect.yBottom - 1);

				UI_REGION updateRegion;
				int windowHeight = windowRect.yTop - windowRect.yBottom;
				updateRegion.left = updateRect.xLeft;
				updateRegion.top = windowHeight - updateRect.yTop;
				updateRegion.right = updateRect.xRight;
				updateRegion.bottom = windowHeight - updateRect.yBottom;

				lastPalette = LogicalPalette(ccode, ID_LIST);
				if (topObject)
				{
					// Draw the Children.
					for (UI_WINDOW_OBJECT *object = topObject; object; object = object->Next())
					{
						// Calculate Regions.
						UI_REGION region;
						region.left = 0;
						region.top = scroll.top + object->relative.top;
						region.right = object->relative.right;
						region.bottom = scroll.top + object->relative.bottom;
						object->trueRegion = region;

						// Test for completion.
						if (region.top > updateRegion.bottom)
							break;
						else if (region.Overlap(updateRegion))
						{
							// Draw the child.
							if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
							{
  								UI_EVENT os2Event;
								os2Event.type = E_OS2;
								os2Event.message.hwnd = object->screenID;
								os2Event.message.msg = WM_DRAWITEM;
								os2Event.message.mp1 = this;
								os2Event.message.mp2 = object;
								object->Event(os2Event);
							}
							else
							{
								// Draw the text.
								UI_REGION drawRegion = clip = region;
								ZIL_ICHAR *text = (ZIL_ICHAR *)object->Information(I_GET_TEXT, ZIL_NULLP(void));
								DrawText(screenID, drawRegion, text, object->LogicalPalette(S_DISPLAY_ACTIVE),
									TRUE, S_DISPLAY_ACTIVE);

								// Draw the Focus Rectangle.
								if (FlagSet(woStatus, WOS_CURRENT) && FlagSet(object->woStatus, WOS_CURRENT))
									DrawFocus(screenID, region, S_CURRENT);
							}
						}
					}
					if (scroll.top + Last()->relative.bottom < updateRegion.bottom)
					{
						int top = scroll.top + Last()->relative.bottom + 1;
						if (updateRegion.top < top)
							updateRegion.top = top;
						display->Rectangle(screenID, updateRegion, lastPalette, 0, TRUE);
					}
				}
				else
				{
					display->Rectangle(screenID, updateRegion, lastPalette, 0, TRUE);
					if (FlagSet(woStatus, WOS_CURRENT))
					{
						UI_REGION focusRegion;
						focusRegion.left = focusRegion.top = 0;
						focusRegion.right = clipList.First()->region.Width() - 1;
						focusRegion.bottom = display->cellHeight;
						DrawFocus(screenID, focusRegion, S_CURRENT);
					}
				}

				WinEndPaint(UI_OS2_DISPLAY::hps);
				display->VirtualPut(ID_DIRECT);
			}
			return (0);

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}

		if (processed)
			return (ccode);
	}

	// Zinc specific messages.
	ccode = LogicalEvent(event, ID_VT_LIST);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		topObject = ZIL_NULLP(UI_WINDOW_OBJECT);
		break;

	case S_CREATE:
	case S_CHANGED:
	case S_DISPLAY_ACTIVE:
	case S_REDISPLAY:
		{
		if (ccode == S_CREATE)
			UIW_WINDOW::Event(event);
		else if (screenID)
		{
			UI_EVENT scrollEvent(S_SCROLLRANGE);
			ScrollEvent(scrollEvent);
			WinInvalidateRect(screenID, ZIL_NULLP(RECTL), TRUE);
		}
		else
			break;

		// Set the child object regions.
		int top = 0;
		int right = trueRegion.Width() - 1;
		if (FlagSet(woFlags, WOF_BORDER))
			right -= 2;
		if (vScroll)
			right -= vScroll->trueRegion.Width();
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
		{
			object->trueRegion.left = object->relative.left;
			object->trueRegion.top = object->relative.top = top;
			object->trueRegion.right = object->relative.right = right;
			top += cellHeight;
			object->trueRegion.bottom = object->relative.bottom = top - 1;
			object->trueRegion.top += scroll.top;
			object->trueRegion.bottom += scroll.top;
		}

		for (topObject = First(); topObject && topObject->Next(); topObject = topObject->Next())
			if (topObject->relative.top >= -scroll.top)
				break;

		if (!current && topObject)
			Add(topObject);

		if (ccode == S_REDISPLAY || ccode == S_DISPLAY_ACTIVE)
			UI_WINDOW_OBJECT::Event(event);
		}
		break;

	case S_REGISTER_OBJECT:
		{
		UIW_WINDOW::Event(event);
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
		{
			object->screenID = screenID;
			object->woStatus |= WOS_OWNERDRAW;
		}
		}
		break;

	case L_UP:
	case L_DOWN:
	case L_PGUP:
	case L_PGDN:
		if (current)
		{
			int lines = (ccode == L_UP || ccode == L_DOWN) ? 1 :
				clipList.First()->region.Height() / cellHeight - 1;
		
			UI_WINDOW_OBJECT *newCurrent = Current();
			for (int i = 0; i < lines && ((ccode == L_UP || ccode == L_PGUP) ?
				newCurrent->Previous() : newCurrent->Next()); i++)
			{
				newCurrent = (ccode == L_UP || ccode == L_PGUP) ? newCurrent->Previous() :
					newCurrent->Next();
			}
			Add(newCurrent);
			UI_EVENT scrollEvent(S_VSCROLL_CHECK);
			ScrollEvent(scrollEvent);
		}
		break;


	case L_TOP:
	case L_BOTTOM:
		if (First())
		{
			Add(ccode == L_TOP ? First() : Last());
			UI_EVENT scrollEvent(S_VSCROLL_CHECK);
			ScrollEvent(scrollEvent);
		}
		break;

	case L_SELECT:
		if (current && !FlagSet(Current()->woFlags, WOF_NON_SELECTABLE))
		{
			Current()->UserFunction(event, L_SELECT);
			Current()->Event(UI_EVENT(S_REDISPLAY, 0));
			if (Inherited(ID_COMBO_BOX))
				parent->Information(I_RESET_SELECTION, parent);
		}
		break;

	case L_BEGIN_SELECT:
	case L_BEGIN_ESCAPE:
	case L_CONTINUE_SELECT:
	case L_DOUBLE_CLICK:
	case L_END_SELECT:
		{
		if (!Inherited(ID_COMBO_BOX))
			UI_WINDOW_OBJECT::Event(event);
#if defined (ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			break;
#endif
		if (First())
		{
			UI_POSITION mousePos = event.position;
			mousePos.column -= trueRegion.left;
			mousePos.line -= trueRegion.top;

			UI_REGION clientRegion = clipList.First()->region;
			for (UI_WINDOW_OBJECT *object = topObject; object; object = object->Next())
			{
				if (object->trueRegion.Overlap(mousePos))
				{
					static int discardUpClick = TRUE;
					UI_WINDOW_OBJECT *oldCurrent = Current();
					BTF_FLAGS btFlags = 0;
					object->Information(I_GET_FLAGS, &btFlags, ID_BUTTON);

					if (ccode == L_BEGIN_SELECT || ccode == L_BEGIN_ESCAPE ||
						ccode == L_CONTINUE_SELECT)
					{
						if (ccode == L_BEGIN_SELECT)
							discardUpClick = FALSE;

						if (object != oldCurrent)
						{
							Add(object);

							if (Inherited(ID_COMBO_BOX))
								parent->Information(I_RESET_SELECTION, this);
						}

						if (ccode == L_BEGIN_SELECT && FlagSet(btFlags, BTF_DOWN_CLICK) &&
							!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
						{
							object->UserFunction(event, L_SELECT);
							object->woStatus |= WOS_REDISPLAY;
						}
					}
					else if (ccode == L_DOUBLE_CLICK)
					{
						if (FlagSet(btFlags, BTF_DOUBLE_CLICK) &&
							!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
						{
							discardUpClick = TRUE;
							object->UserFunction(event, L_DOUBLE_CLICK);
							object->woStatus |= WOS_REDISPLAY;
						}
						else if (FlagSet(btFlags, BTF_DOWN_CLICK) &&
							!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
						{
							object->UserFunction(event, L_SELECT);
							object->woStatus |= WOS_REDISPLAY;
						}
					}
					else
					{
						if (!discardUpClick && !FlagSet(btFlags, BTF_DOWN_CLICK) &&
							!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
						{
							object->UserFunction(event, L_SELECT);
							object->woStatus |= WOS_REDISPLAY;
						}
						if (Inherited(ID_COMBO_BOX) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
							parent->Information(I_RESET_SELECTION, parent);
						discardUpClick = TRUE;
					}

					if (FlagSet(object->woStatus, WOS_REDISPLAY))
						object->Event(UI_EVENT(S_REDISPLAY));
					break;
				}
				if (object->trueRegion.bottom > clientRegion.Height() - 1)
					break;
			}
		}
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		if (First())
		{
			if (ccode == S_CURRENT)
				woStatus |= WOS_CURRENT;
			else
				woStatus &= ~WOS_CURRENT;
		}
		UIW_WINDOW::Event(event);		
		break;

	case S_DRAG_DEFAULT:
	case S_DRAG_MOVE_OBJECT:
		if (!windowManager->dragObject->Inherited(ID_LIST) &&
			!(windowManager->dragObject->parent &&
			windowManager->dragObject->parent->Inherited(ID_LIST)))
		{
			if (ccode == S_DRAG_DEFAULT)
				ccode = S_DRAG_COPY_OBJECT;
			else
			 	return (S_ERROR);
		}
		// Continue.

	case S_DRAG_COPY_OBJECT:
		eventManager->DeviceState(E_MOUSE, ccode == S_DRAG_COPY_OBJECT ? DM_DRAG_COPY : DM_DRAG_MOVE);
		return (0);

	case S_DROP_DEFAULT:
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		if (ccode == S_DROP_DEFAULT)
		{
			if (!windowManager->dragObject->Inherited(ID_LIST) &&
				!(windowManager->dragObject->parent &&
				windowManager->dragObject->parent->Inherited(ID_LIST)))
				ccode = S_DROP_COPY_OBJECT;
			else
				ccode = S_DROP_MOVE_OBJECT;
		}
		if (ccode == S_DROP_MOVE_OBJECT)
		{
			if (windowManager->dragObject->parent &&
				windowManager->dragObject->parent == this)
					break;
			else if (windowManager->dragObject->Inherited(ID_LIST))
			{
				UI_WINDOW_OBJECT *object;
				windowManager->dragObject->Information(I_GET_FIRST, &object);
				while (object)
				{
					UI_WINDOW_OBJECT *nextObject = object->Next();
					if (FlagSet(object->woStatus, WOS_SELECTED))
					{
						UI_EVENT moveEvent(S_SUBTRACT_OBJECT);
						moveEvent.windowObject = object;
						windowManager->dragObject->Event(moveEvent);
						object->woStatus &= ~WOS_CURRENT;
						moveEvent.type = S_ADD_OBJECT;
						if (Inherited(ID_COMBO_BOX))
							parent->Event(moveEvent);
						else
							Event(moveEvent);
					}
					object = nextObject;
				}
				windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
			}
			else if (windowManager->dragObject->parent &&
				windowManager->dragObject->parent->Inherited(ID_LIST) &&
				windowManager->dragObject->parent != this)
			{
				UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
				UI_WINDOW_OBJECT *dragParent = dragObject->parent;
				UI_EVENT moveEvent(S_SUBTRACT_OBJECT);
				moveEvent.windowObject = dragObject;
				dragParent->Event(moveEvent);
				dragParent->Event(UI_EVENT(S_REDISPLAY));
				dragObject->relative.right = dragObject->relative.Width() - 1;
				dragObject->relative.left = 0;
				moveEvent.type = S_ADD_OBJECT;
				moveEvent.windowObject->woStatus &= ~WOS_CURRENT;
				if (Inherited(ID_COMBO_BOX))
					parent->Event(moveEvent);
				else
					Event(moveEvent);
			}
			if (!Inherited(ID_COMBO_BOX))
				Event(S_REDISPLAY);
		}
#if defined(ZIL_LOAD) && defined(ZIL_STORE)
		else
		{
			if (windowManager->dragObject->Inherited(ID_LIST))
			{
				UI_WINDOW_OBJECT *object;
				windowManager->dragObject->Information(I_GET_FIRST, &object);
				while (object)
				{
					UI_WINDOW_OBJECT *nextObject = object->Next();
					if (FlagSet(object->woStatus, WOS_SELECTED))
					{
						ZIL_ICHAR name[] = { 'd','r','a','g',0 };
						ZIL_NEW_FUNCTION newFunction = object->NewFunction();
						ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
						ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
						object->Store(name, &directory, &file);
						file.Seek(0);
						object = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
						UI_EVENT addEvent(S_ADD_OBJECT);
						addEvent.windowObject = object;
						if (Inherited(ID_COMBO_BOX))
							parent->Event(addEvent);
						else
							Event(addEvent);
					}
					object = nextObject;
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
				if (Inherited(ID_COMBO_BOX))
					parent->Event(dEvent);
				else
					Event(dEvent);
			}
			if (Inherited(ID_COMBO_BOX))
				Event(S_CHANGED);
			else
				Event(S_REDISPLAY);
		}
#endif
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_VT_LIST::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_SCROLLRANGE:
		{
		// Set the cell height.
		cellHeight = 1;
 		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		{
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				if (object->trueRegion.Height() > cellHeight)
					cellHeight = object->trueRegion.Height();
		}
		else if (screenID)
		{
			display->VirtualGet(screenID, trueRegion);
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			{
				ZIL_ICHAR *text;
				object->Information(I_GET_TEXT, &text);
				int height = display->TextHeight(text, screenID, font);
				if (height > cellHeight)
					cellHeight = height;
			}
			display->VirtualPut(screenID);
		}
		else
			cellHeight = display->cellHeight;

 		// Set the scroll bar information.
		int height = trueRegion.Height();
		if (FlagSet(woFlags, WOF_BORDER))
			height -= 2;
		int showing = MaxValue(height / cellHeight, 1);

		int bottom = scroll.top + Count() * cellHeight;
		if (scroll.top < 0 && height - bottom > cellHeight)
			scroll.top = MinValue(0, scroll.top + (height - bottom) / cellHeight * cellHeight);

		vScrollInfo.showing = showing * cellHeight;
		vScrollInfo.minimum = 0;
		vScrollInfo.maximum = MaxValue(Count() * cellHeight - vScrollInfo.showing, 0);
		vScrollInfo.current = -scroll.top;
		vScrollInfo.delta = cellHeight;

		if (vScroll)
		{
			UI_EVENT sEvent(S_VSCROLL_SET);
			sEvent.scroll = vScrollInfo;
			vScroll->Event(sEvent);
		}
		}
		break;

	case S_VSCROLL:
		{
		int delta = event.scroll.delta / cellHeight;
		event.scroll.delta = delta * cellHeight;
		if (delta)
		{
			if (delta > 0)
			{
				for (int i = 0; i < delta; i++)
					topObject = topObject->Next();
			}
			else
			{
				for (int i = 0; i < -delta; i++)
					topObject = topObject->Previous();
			}

			int clientHeight = clipList.First()->region.Height();
			for (UI_WINDOW_OBJECT *object = topObject; object; object = object->Next())
			{
				if (scroll.top - event.scroll.delta + object->relative.top > clientHeight)
					break;

				object->trueRegion.top = scroll.top - event.scroll.delta + object->relative.top;
				object->trueRegion.bottom = scroll.top - event.scroll.delta + object->relative.bottom;
			}
	  		UIW_WINDOW::ScrollEvent(event);
		}
		}
		break;

	case S_VSCROLL_CHECK:
		if (current)
		{
			int scrollDelta = 0;
			UI_REGION clientRegion = clipList.First()->region;
			if (Current()->relative.top + scroll.top < 0)
				scrollDelta = scroll.top + Current()->relative.top;
			else if (Current()->relative.bottom + scroll.top > clientRegion.Height() - 1)
				scrollDelta = (Current()->relative.bottom + scroll.top - clientRegion.Height() +
					cellHeight - 1) / cellHeight * cellHeight;

			if (scrollDelta)
			{
				UI_EVENT sEvent(S_VSCROLL);
				sEvent.scroll.delta = scrollDelta;
				ScrollEvent(sEvent);
			}
		}
		break;

	default:
		ccode = UIW_WINDOW::ScrollEvent(event);
	}
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_VT_LIST::OSDestroy(void)
{
	if (screenID)
	{
		WinEnableWindowUpdate(screenID, FALSE);
		UIW_WINDOW::Destroy();
		topObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	}
}

void UIW_VT_LIST::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_VT_LIST && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}

void UIW_VT_LIST::OSSort(void)
{
}
