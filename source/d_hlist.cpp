//	Zinc Interface Library - D_HLIST.CPP
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

// ----- UIW_HZ_LIST --------------------------------------------------------

EVENT_TYPE UIW_HZ_LIST::Event(const UI_EVENT &event)
{
	// Check for current item removed from list;
	if (current && Index(current) == -1)
		SetCurrent(First());

	UI_WINDOW_OBJECT *object;
	UI_WINDOW_OBJECT *oldCurrent = Current();

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_HZ_LIST);
	switch (ccode)
	{

	case S_INITIALIZE:
		if (first && !current)
			SetCurrent(first);
		if (current && FlagSet(wnFlags, WNF_AUTO_SELECT))
			Current()->woStatus |= WOS_SELECTED;
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_CHANGED:
	case S_CREATE:

		// Compute the object regions.
		clipList.Destroy();
		UI_WINDOW_OBJECT::Event(event);
		{
		UI_REGION region = trueRegion;
		if (FlagSet(woFlags, WOF_BORDER) && (!parent || !display->isText ||
			FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
#if defined(ZIL_MOTIF_STYLE)
			region -= (display->isText ? 1 : 2);
#elif defined(ZIL_OS2_STYLE)
		{
			--region;
			if (!display->isText)
				region.right--;
		}
#else
			--region;
#endif
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));

		// Compute the support object regions.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, FALSE);
		}

		// Compute the list object regions.
		int top = 0, left = hScrollInfo.current && First() ? First()->relative.left : 0;
		UI_REGION client = clipList.First() ?	clipList.First()->region : trueRegion;
		int itemWidth = cellWidth * display->cellWidth;
		int listCellHeight = display->TextHeight(ZIL_NULLP(ZIL_ICHAR), screenID, font) + display->preSpace + display->postSpace;
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !display->isText)
			for (object = First(); object; object = object->Next())
			{
				object->Event(event);
				listCellHeight = MaxValue(listCellHeight, object->trueRegion.Height());
			}
		rows = 0; columns = 1;
		int setRows = FALSE;
		for (object = First(); object; object = object->Next())
		{
			if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
				object->woFlags &= ~WOF_BORDER;
			if (top + listCellHeight > client.Height() && object != First())
			{
				columns++;
				if (!setRows)
				{
					rows = Index(object);
					setRows = TRUE;
				}
				top = 0;
				left += itemWidth;
			}
			object->relative.left = left;
			object->relative.top = top;
			object->relative.right = left + itemWidth - 1;
			top += listCellHeight;
			object->relative.bottom = top - 1;
			object->woFlags |= WOF_VIEW_ONLY;
			object->Event(event);
		}
		UI_EVENT sEvent(S_SCROLLRANGE);
		ScrollEvent(sEvent);

		if (oldCurrent && ccode == S_CREATE && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE) &&
			FlagSet(oldCurrent->woStatus, WOS_SELECTED))
		{
			sEvent.type = S_HSCROLL_CHECK;
			ScrollEvent(sEvent);
		}

		if (-scroll.left > (hScrollInfo.maximum - hScrollInfo.minimum) * itemWidth)
		{
			scroll.left += (-scroll.left - (hScrollInfo.maximum - hScrollInfo.minimum) * itemWidth);
			Event(event);
		}
		}
		break;

	case S_DISPLAY_INACTIVE:
	case S_DISPLAY_ACTIVE:
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
			parent->Event(UI_EVENT(S_CHANGED));
		else
			Event(UI_EVENT(S_CHANGED));
		UIW_WINDOW::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		if (ccode == S_CURRENT)
		{
			for (object = First(); object; object = object->Next())
				if (FlagSet(object->woStatus, WOS_CURRENT))
				{
					if (current && current != object)
						Current()->Event(UI_EVENT(S_NON_CURRENT, 0, object->trueRegion));
					current = object;
					break;
				}
			if (!current)
				current = first;
			if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
				parent->Event(UI_EVENT(S_CHANGED));
			else
				Event(UI_EVENT(S_CHANGED));
		}
		ccode = UIW_WINDOW::Event(event);
		if (!first)
		{
			// Draw or hide a focus bar if no list items.
			UI_REGION rect = clipList.First() ? clipList.First()->region : trueRegion;
			UI_PALETTE *currentBar = LogicalPalette(ccode, ID_LIST_ITEM);

			display->VirtualGet(screenID, rect);
			if (display->isText)
			{
				rect.bottom = rect.top;
				rect.right = rect.left + cellWidth;
				display->Rectangle(screenID, rect, currentBar, 0, TRUE, FALSE, &clip);
			}
			else
			{
#if !defined(ZIL_TEXT_ONLY)
				rect.bottom = rect.top + 14;
				rect.right = rect.left + cellWidth * display->cellWidth;
				rect.left = MaxValue(clip.left, rect.left);
				rect.top = MaxValue(clip.top, rect.top);
				rect.right = MinValue(clip.right, rect.right);
				rect.bottom = MinValue(clip.bottom, rect.bottom);
				if (ccode == S_CURRENT)
					display->Rectangle(screenID, rect, currentBar, -1, FALSE);
				else
					display->Rectangle(screenID, rect, currentBar, 0, TRUE);
#endif
			}
			display->VirtualPut(screenID);
		}
		break;

	case L_VIEW:
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
	case L_CONTINUE_COPY_DRAG:
	case L_END_COPY_DRAG:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		if (windowManager->dragObject)
		{
		  	if (ccode == L_END_SELECT && (windowManager->dragObject == this ||
				Index(windowManager->dragObject) != -1))
				windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
			else
				UI_WINDOW_OBJECT::Event(event);
			break;
		}
		// Let object unactivate its region.
		if (ccode == L_END_SELECT && current && !Current()->trueRegion.Overlap(event.position) ||
			!trueRegion.Overlap(event.position))
			return UIW_WINDOW::Event(event);
		if (ccode == L_BEGIN_SELECT)
		{
			eventManager->DeviceState(E_CURSOR, D_OFF);
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
  		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		   if (object->trueRegion.Overlap(event.position))
				return UIW_WINDOW::Event(event);
		for (object = First(); object; object = object->Next())
			if	(object->trueRegion.Overlap(event.position))
			{
				if (ccode == L_BEGIN_SELECT || (ccode == L_CONTINUE_SELECT &&
					FlagSet(woStatus, WOS_CURRENT)))
  					UIW_WINDOW::Add(object);
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!(ccode == L_CONTINUE_SELECT &&
					FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT)))
					object->Event(event);
			}

		if (ccode == L_BEGIN_SELECT &&
			FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
		{
			int count = 0;
			for(object = First(); object; object = object->Next())
				if (FlagSet(object->woStatus, WOS_SELECTED))
					count++;
			if (count)
				windowManager->dragObject = this;
			else
				windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
		}
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_LEFT:
	case L_RIGHT:
		if (oldCurrent)
		{
		int currentTop = Current()->trueRegion.top;
		object = (ccode == L_LEFT) ? Current()->Previous() : Current()->Next();
		while (object && object != current)
		{
			if (object->trueRegion.top == currentTop)
				UIW_WINDOW::Add(object);
			else
				object = (ccode == L_LEFT) ? object->Previous() : object->Next();
		}
		UI_EVENT sEvent(S_HSCROLL_CHECK);
		ScrollEvent(sEvent);
		}
		break;

	case L_UP:
	case L_DOWN:
		if (oldCurrent)
		{
			if (ccode == L_UP)
				object = oldCurrent->Previous() ? oldCurrent->Previous() : First();
			else
				object = oldCurrent->Next() ? oldCurrent->Next() : Last();

			// Make the newOjbect current and scroll into view.
	 	 	UIW_WINDOW::Add(object);
			UI_EVENT sEvent(S_HSCROLL_CHECK);
			ScrollEvent(sEvent);
		}
		break;

	case L_PGUP:
	case L_PGDN:
		if (oldCurrent)
		{
			int newIndex;
			if (ccode == L_PGUP)
			{
				newIndex = MaxValue(0, Index(oldCurrent) - (rows * hScrollInfo.showing - 1));
			    object = (UI_WINDOW_OBJECT *)UI_LIST::Get(newIndex);
			}
			else
			{
				newIndex = MinValue(Index(Last()), Index(oldCurrent) + rows * hScrollInfo.showing - 1);
			    object = (UI_WINDOW_OBJECT *)UI_LIST::Get(newIndex);
			}
			// Make the newOjbect current and scroll into view.
			Add(object);
			UI_EVENT sEvent(S_HSCROLL_CHECK);
			ScrollEvent(sEvent);
		}
		break;

	case L_TOP:
	case L_BOTTOM:
		if (oldCurrent)
		{
			object = (ccode == L_TOP) ? First() : Last();

			// Make the newOjbect current and scroll into view.
			Add(object);
			UI_EVENT sEvent(S_HSCROLL_CHECK);
			ScrollEvent(sEvent);
		}
		break;

	case L_SELECT:
		{
		UI_WINDOW_OBJECT *dObject;
		parent->Information(I_GET_DEFAULT_OBJECT, &dObject);
		if (dObject)
			break;
		else	if (oldCurrent && Index(oldCurrent) != -1 &&
			!FlagSet(oldCurrent->woFlags, WOF_NON_SELECTABLE))
			ccode = oldCurrent->Event(UI_EVENT(L_SELECT));
		}	
		break;

	case S_DROP_DEFAULT:
	case S_DROP_MOVE_OBJECT:
		{
		UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
		UI_WINDOW_OBJECT *dragParent = dragObject->parent;
		object = ZIL_NULLP(UI_WINDOW_OBJECT);

		if (dragObject == this || dragParent == this)
			break;

		if (dragObject->Inherited(ID_LIST))
		{
			UI_WINDOW_OBJECT *nextObject = ZIL_NULLP(UI_WINDOW_OBJECT);
			for (dragObject->Information(I_GET_FIRST, &object);
				object; object = nextObject)
			{
				nextObject = object->Next();
				if (FlagSet(object->woStatus, WOS_SELECTED))
				{
					UI_EVENT dEvent(S_SUBTRACT_OBJECT);
					dEvent.windowObject = object;
					dragObject->Event(dEvent);
					object->woStatus &= ~(WOS_CURRENT | WOS_SELECTED);
					dEvent.type = S_ADD_OBJECT;
					Event(dEvent);
				}
			}
			dragObject->Event(UI_EVENT(S_REDISPLAY));
		}
		else if (dragParent && dragParent->Inherited(ID_LIST))
		{
			UI_EVENT dEvent(S_SUBTRACT_OBJECT);
			dEvent.windowObject = dragObject;
			dragParent->Event(dEvent);
			dragObject->woStatus &= ~(WOS_SELECTED | WOS_CURRENT);
			dragParent->Event(UI_EVENT(S_REDISPLAY));
			dEvent.type = S_ADD_OBJECT;
			Event(dEvent);
		}
		Event(S_REDISPLAY);
		}
		break;

	case S_DROP_COPY_OBJECT:
#if defined(ZIL_LOAD) && defined(ZIL_STORE)
		{
		UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
		if (dragObject == this || dragObject->parent == this)
			break;

		if (dragObject->Inherited(ID_LIST))
		{
			UI_WINDOW_OBJECT *nextObject = ZIL_NULLP(UI_WINDOW_OBJECT);
			for (dragObject->Information(I_GET_FIRST, &object);
				object; object = nextObject)
			{
				nextObject = object->Next();
				if (FlagSet(object->woStatus, WOS_SELECTED))
				{
					static ZIL_ICHAR name[] = { 'd','r','a','g',0 };
					ZIL_NEW_FUNCTION newFunction = object->NewFunction();
					ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
					ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
					object->Store(name, &directory, &file);
					file.Seek(0);
					object = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
					UI_EVENT dEvent(S_ADD_OBJECT);
					dEvent.windowObject = object;
					Event(dEvent);
				}
			}
			dragObject->Event(UI_EVENT(S_REDISPLAY));
		}
		else
		{
			static ZIL_ICHAR name[] = { 'd','r','a','g',0 };
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
			Event(dEvent);
		}
		Event(S_REDISPLAY);
		}
#endif
		break;

	default:
		ccode = UIW_WINDOW::Event(event);

		// Try to do first character recognition.
		if (event.type == E_KEY && IsPrint(event.key.value) && Current())
		{
			ZIL_ICHAR buffer[2], *data;
			buffer[0] = event.key.value;
			buffer[1] = 0;
			for (object = Current()->Next(); object; object = object->Next())
			{
				data = ZIL_NULLP(ZIL_ICHAR);
				object->Information(I_GET_TEXT, &data);
				if (data && !strnicmp(buffer, data, 1))
					break;
			}
			if (!object)
			{
				for (object = First(); object != Current(); object = object->Next())
				{
					data = ZIL_NULLP(ZIL_ICHAR);
					object->Information(I_GET_TEXT, &data);
					if (data && !strnicmp(buffer, data, 1))
						break;
				}
			}
			if (object && object != Current())
			{
				Add(object);
				UI_EVENT sEvent(S_HSCROLL_CHECK);
				ScrollEvent(sEvent);
			}
		}
		break;
	}

	// Return the control code.
	return (ccode);
}


EVENT_TYPE UIW_HZ_LIST::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	UI_WINDOW_OBJECT *object = Current();
	switch(ccode)
	{

	case S_SCROLLRANGE:
		{
		// Set up clientArea, range, showing, and max scroll area.
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;

		hScrollInfo.delta = 1;
		hScrollInfo.showing = MaxValue(1,client.Width() / (cellWidth * display->cellWidth));
		hScrollInfo.maximum = MaxValue(0, columns - hScrollInfo.showing);

		if (hScrollInfo.current > hScrollInfo.maximum)
			hScrollInfo.current = hScrollInfo.maximum;

		if (hScroll)
		{
			// Reset the scrollbar information.
			event.type = S_HSCROLL_SET;
			event.scroll = hScrollInfo;
			hScroll->Event(event);
		}
		}
		break;

	case S_HSCROLL_CHECK:
		if (object)
		{
		// Insure that object is scrolled into view.
		event.type = S_HSCROLL;
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
		if (object->trueRegion.left < client.left)
		{
			UI_WINDOW_OBJECT *left;
	 		for (left = First(); left && left->trueRegion.left < client.left;
				left = left->Next())
 				;
			while (left && left->trueRegion.top != object->trueRegion.top)
				left = left->Next();
			if (First() == object)
				event.scroll.delta = -hScrollInfo.maximum;
			else
				event.scroll.delta = (Index(object) - Index(left)) / MaxValue(1, rows);
			ScrollEvent(event);
		}
		else if (object->trueRegion.right > client.right)
		{
			UI_WINDOW_OBJECT *right;
	 		for (right = Last(); right && right->trueRegion.right > client.right;
				right = right->Previous())
				;
			while (right && right->trueRegion.top != object->trueRegion.top)
				right = right->Previous();
			if (Last() == object)
				event.scroll.delta = hScrollInfo.maximum;
			else
				event.scroll.delta = (Index(object) - Index(right)) / MaxValue(1, rows);
			ScrollEvent(event);
		}
		}
		break;

	case S_HSCROLL_WINDOW:
		{
		scroll.left -= event.scroll.delta * cellWidth * display->cellWidth;
		scroll.right -= event.scroll.delta * cellWidth * display->cellWidth;
		for (object = First(); object; object = object->Next())
			object->Event(UI_EVENT(S_CHANGED));

		if (screenID)
		{
			UI_EVENT sEvent(S_REDISPLAY);
			UI_REGION region = trueRegion;
			UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
			display->VirtualGet(screenID, region);
			for (object = First(); object; object = object->Next())
				object->Event(sEvent);
			display->VirtualPut(screenID);
			if (Last() && Last()->trueRegion.left < client.right)
			{
				// Update remaining empty space in the list.
				region = client;
				region.left = !((Index(Last()) + 1) % rows) ?
					Last()->trueRegion.right : Last()->trueRegion.left;
				region.bottom = trueRegion.bottom;
				UIW_WINDOW::Event(UI_EVENT(RedisplayType(), 0, region));
			}
		}
		}
		break;

	default:
		ccode = UIW_WINDOW::ScrollEvent(event);
	}
	return ccode;
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_HZ_LIST::OSDestroy(void)
{
}

void UIW_HZ_LIST::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_HZ_LIST && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}

void UIW_HZ_LIST::OSSort(void)
{
	Event(UI_EVENT(S_REDISPLAY));
}

