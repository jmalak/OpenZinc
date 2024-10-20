//	Zinc Interface Library - Z_NOTEBK.CPP
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

EVENT_TYPE UIW_NOTEBOOK::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event;

	// Check for non-current messages.
	if (ccode == S_NON_CURRENT)
		return (TRUE);

	UI_REGION region = trueRegion;
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	UI_PALETTE *lightShadow = outline, *darkShadow = outline;

	// Clear the window region.
	display->VirtualGet(screenID, region);

	lastPalette = &display->colorMap[LIGHTGRAY];
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	if (!display->isText)
	{
		region -= borderWidth;
		region.top += tabHeight;
		UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.Last();
		for (int count = 0; count < shadowWidth; count++)
		{
			if (object && object->trueRegion.right < region.right)
				display->Line(screenID, object->trueRegion.right - count, region.top, region.right, region.top, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top, region.left, region.bottom, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.bottom, region.right, region.bottom, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.right, region.top, region.right, region.bottom, darkShadow, 1, FALSE, &clip);
			if (!count)
			{
				lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
				darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
			}
			--region;
		}
	}
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_NOTEBOOK::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = S_UNKNOWN;
	int processed = FALSE;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;
		WPARAM wParam = event.message.wParam;
		LPARAM lParam = event.message.lParam;

		processed = TRUE;

		// Switch on the windows message.
		switch (message)
		{
		case WM_SYSCHAR:
		case WM_MENUCHAR:
			{
			woStatus |= WOS_WINDOWS_ACTION;
			processed = FALSE;

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
			if (!hotChar)
			{
				processed = TRUE;
				ccode = CallWindowProc((WINDOWSPROC)defaultCallback,
					event.message.hwnd, event.message.message, wParam, lParam);
				woStatus &= ~WOS_WINDOWS_ACTION;
				break;
			}

			for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First(); object && !processed; object = object->Next())
				if (object->HotKey() == HOT_KEY_SUB_WINDOW &&
					!FlagSet(object->woStatus, WOS_WINDOWS_ACTION))
				{
					ccode = object->Event(event);
					if (ccode == 0x00010000L)
						processed = TRUE;
				}
				else if (hotChar == (ZIL_LOGICAL_EVENT)object->HotKey() &&
					!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				{
					object->Event(UI_EVENT(L_SELECT, 0));
					processed = TRUE;
					ccode = 0x00010000L;
					woStatus &= ~WOS_WINDOWS_ACTION;
					break;
				}

			if (processed)
			{
				woStatus &= ~WOS_WINDOWS_ACTION;
				break;
			}

			if (!FlagSet(Current()->woStatus, WOS_WINDOWS_ACTION))
			{
				ccode = Current()->Event(event);
				if (ccode == 0x00010000L)
					processed = TRUE;
			}

			if (processed)
			{
				woStatus &= ~WOS_WINDOWS_ACTION;
				break;
			}
			if (!FlagSet(parent->woStatus, WOS_WINDOWS_ACTION))
				UI_WINDOW_OBJECT::Event(event);
			processed = TRUE;
			woStatus &= ~WOS_WINDOWS_ACTION;
			}
			break;

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Check the event type.
		ccode = LogicalEvent(event, ID_NOTEBOOK);
		switch (ccode)
		{
		case S_INITIALIZE:
			if (!FlagSet(woStatus, WOS_EDIT_MODE))
				Root()->NumberID(0x1000);
			ccode = UIW_WINDOW::Event(event);
			break;
	
		case S_CREATE:
		case S_CHANGED:
		case S_SIZE:
			{
			UI_REGION tRegion = trueRegion;
			UI_WINDOW_OBJECT::Event(event);
			if (display->isText)
				borderWidth = 0, shadowWidth = 1;
			else
				borderWidth = 2, shadowWidth = 3;
			tabWidth = (trueRegion.Width() - 2 * borderWidth - 1) / totalPages;
	
			// Compute the tab areas.
			UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First();
			int count;
			for (count = 0; object; count++, object = object->Next())
			{
				object->relative.left = borderWidth + count * tabWidth;
				object->relative.top = borderWidth;
				object->relative.right = object->relative.left + tabWidth - 1;
				object->relative.bottom = display->isText ? borderWidth :
					object->relative.top + tabHeight + shadowWidth - 1;
				if (count == currentPage)
					object->woStatus |= WOS_SELECTED;
				else
					object->woStatus &= ~WOS_SELECTED;
			}
	
			// Set the page area.
			int offset = display->isText ? 0 : borderWidth + shadowWidth;
			for (count = 0, object = First(); object; count++, object = object->Next())
				if (count == currentPage)
				{
					object->relative.left = offset;
					object->relative.top = offset + tabHeight;
					object->relative.right = trueRegion.Width() - offset - 1;
					object->relative.bottom = trueRegion.Height() - offset - 1;
					object->woStatus |= WOS_CURRENT;
					current = object;
				}
				else
				{
					object->relative.left = object->relative.top = 10000;
					object->relative.right = object->relative.bottom = 11000;
					object->woStatus &= ~WOS_CURRENT;
				}
	
			trueRegion = tRegion;
			UIW_WINDOW::Event(event);
			}
			break;
	
		case S_ADD_OBJECT:
			if (Index(event.windowObject) != -1)
				UIW_WINDOW::Event(event);
			else if (event.windowObject->SearchID() == ID_WINDOW)
				Add(event.windowObject);
			else
				ccode = S_ERROR;
			break;
	
		case S_SUBTRACT_OBJECT:
			if (Index(event.windowObject) != -1 &&
				event.windowObject->SearchID() == ID_WINDOW)
				Subtract(event.windowObject);
			else
				ccode = S_ERROR; // cannot subtract tabs.
			break;
	
		case L_NEXT:
		case L_PREVIOUS:
			ccode = Current()->Event(event);
			break;

		default:
			if (ccode <= S_CHANGE_PAGE)
			{
				// Determine the new page.
				UI_WINDOW_OBJECT *object;
				int newPage = (int)(-event.type + S_CHANGE_PAGE);
				if (newPage == currentPage)
					break;
				eventManager->DeviceState(E_CURSOR, D_OFF);
	
				// Swap out the old and new tabs.
				object = (UI_WINDOW_OBJECT *)support.Get(currentPage);
				object->woStatus &= ~WOS_SELECTED;
				object->Event(S_REDISPLAY);
				object = (UI_WINDOW_OBJECT *)support.Get(newPage);
				object->woStatus |= WOS_SELECTED;
				object->Event(S_REDISPLAY);

				// Swap out the old and new pages.
				object = (UI_WINDOW_OBJECT *)UI_LIST::Get(currentPage);
				UI_REGION temp = object->relative;
				object->relative.left = object->relative.top = 10000;
				object->relative.right = object->relative.bottom = 11000;
				object->Event(S_CHANGED);
				object = (UI_WINDOW_OBJECT *)UI_LIST::Get(newPage);
				object->relative = temp;
				object->Event(S_CHANGED);
				object->Event(S_REDISPLAY);
				UIW_WINDOW::Add(object);
	
				// Update the current page.
				currentPage = newPage;
				return (ccode);
			}
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	return (ccode);
}

