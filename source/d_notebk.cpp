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
	// Check for non-current messages.
	if (ccode == S_NON_CURRENT)
		return (TRUE);

	UI_REGION region = trueRegion;
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	UI_PALETTE *lightShadow = outline, *darkShadow = outline;

	// Clear the window region.
	display->VirtualGet(screenID, region);

	lastPalette = &display->colorMap[LIGHTGRAY];
	UI_REGION clipRegion = event.region;
	clipRegion.left = MaxValue(clipRegion.left, clip.left);
	clipRegion.top = MaxValue(clipRegion.top, clip.top);
	clipRegion.right = MinValue(clipRegion.right, clip.right);
	clipRegion.bottom = MinValue(clipRegion.bottom, clip.bottom);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clipRegion);
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
	// Check the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_NOTEBOOK);
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
				object->relative.left = object->relative.right = 10000;
				object->relative.top = object->relative.bottom = 10000;
				object->woStatus &= ~WOS_CURRENT;
			}
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
			object = (UI_WINDOW_OBJECT *)support.Get(newPage);
			
			// Make sure we have a valid page to redisplay.
			if (!object)
				return (ccode);
			object->woStatus |= WOS_SELECTED;
			object->Event(S_REDISPLAY);
			object = (UI_WINDOW_OBJECT *)support.Get(currentPage);
			object->woStatus &= ~WOS_SELECTED;
			object->Event(S_REDISPLAY);
			
			// Swap out the old page.
			object = (UI_WINDOW_OBJECT *)UI_LIST::Get(currentPage);
			UI_REGION temp = object->relative;
			object->relative.left = object->relative.right = 10000;
			object->relative.top = object->relative.bottom = 10000;
			object->Event(S_CHANGED);

			// Remove default object in case the new page has one.
			Root(TRUE)->Information(I_GET_DEFAULT_OBJECT, &object);
			if (object)
			{
				// Make sure the default object is one of my children.
				UI_WINDOW_OBJECT *oParent = object->parent;
				for (; oParent; oParent = oParent->parent)
				{
					if (oParent == this)
						break;
				}
				if (oParent)
				{
					BTS_STATUS _btStatus = BTS_DEFAULT;
					object->Information(I_CLEAR_STATUS, &_btStatus, ID_BUTTON);
					Root(TRUE)->Information(I_SET_DEFAULT_OBJECT, ZIL_NULLP(void));
				}
			}

			// Swap in the new page.
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

	return (ccode);
}
