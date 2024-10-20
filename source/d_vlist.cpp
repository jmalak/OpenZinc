//	Zinc Interface Library - D_VLIST.CPP
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
	// Check for current item removed from list;
	if (current && Index(current) == -1)
		SetCurrent(First());

 	UI_WINDOW_OBJECT *object;
	UI_WINDOW_OBJECT *oldCurrent = Current();

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_VT_LIST);
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
				region.bottom--;
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
		cellHeight = display->TextHeight(ZIL_NULLP(ZIL_ICHAR), screenID, font) + display->preSpace + display->postSpace;
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
		int lineWidth = client.Width();
		int top = 0;

		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !display->isText)
			for (object = First(); object; object = object->Next())
			{
				object->Event(event);
				cellHeight = MaxValue(cellHeight, object->trueRegion.Height());
			}

		for (object = First(); object; object = object->Next())
		{
			if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
				object->woFlags &= ~WOF_BORDER;
			object->woFlags |= WOF_VIEW_ONLY;
			object->relative.top = top;
			object->relative.right = lineWidth - 1;
			top += cellHeight;
			object->relative.bottom = top - 1;
			object->Event(event);
		}
		UI_EVENT sEvent(S_SCROLLRANGE);
		ScrollEvent(sEvent);

		// Scroll item into view if WOS_SELECTED set.
		if (oldCurrent && ccode == S_CREATE && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE) &&
			FlagSet(oldCurrent->woStatus, WOS_SELECTED))
		{
			sEvent.type = S_VSCROLL_CHECK;
			ScrollEvent(sEvent);
		}

		if (parent->Inherited(ID_COMBO_BOX) && ccode == S_CREATE && current)
		{
			// Force current item to the top of combobox list when displayed.
			sEvent.type = S_VSCROLL_CHECK;
			ScrollEvent(sEvent);
			if (current != last)
			{
				int count = 0;
				for (object = Current(); object; object = object->Previous())
				if (object->trueRegion.top > client.top)
					count++;
				if (count)
				{
					UI_EVENT tEvent(S_VSCROLL);
					tEvent.scroll.delta = count;
					Event(tEvent);
				}
			}
		}
		if (-scroll.top > (vScrollInfo.maximum - vScrollInfo.minimum) * cellHeight)
		{
			scroll.top += (-scroll.top - (vScrollInfo.maximum - vScrollInfo.minimum) * cellHeight);
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

		if	(!(parent->Inherited(ID_COMBO_BOX) && windowManager->Index(this) == -1))
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
				display->Rectangle(screenID, rect, currentBar, 0, TRUE, FALSE, &clip);
			}
			else
			{
#if !defined(ZIL_TEXT_ONLY)
				rect.bottom = rect.top + 14;
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

	case S_ADD_OBJECT:
		object = event.windowObject;
		if (UI_LIST::Index(object) == -1)
		{
			if (!parent || FlagSet(parent->woStatus, WOS_EDIT_MODE))
				object->relative.left = 0;
			object->relative.right = object->relative.left + trueRegion.right - trueRegion.left;
		}
		UIW_VT_LIST::Add(event.windowObject);
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
				{
					if (parent->Inherited(ID_COMBO_BOX))
					{
						if (clipList.First() && !clipList.First()->region.Overlap(event.position))
							break;
						// This allows WNF_CONTINUE_SELECT to work in combo list.
						object->woStatus |= WOS_INTERNAL_ACTION;
						UIW_WINDOW::Add(object);
						object->woStatus &= ~WOS_INTERNAL_ACTION;
					}
					else
						UIW_WINDOW::Add(object);
				}
				else if (ccode == L_END_SELECT)
				{
					if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY) && parent->Inherited(ID_COMBO_BOX) &&
						(!clipList.First() || clipList.First()->region.Overlap(event.position)))
						windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
				}
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
		if (parent->Inherited(ID_COMBO_BOX))
			return parent->Event(event);
		// Continue on.
	case L_UP:
	case L_DOWN:
		if (oldCurrent)
		{
			if (ccode == L_UP || ccode == L_LEFT)
				object = oldCurrent->Previous() ? oldCurrent->Previous() : First();
			else
				object = oldCurrent->Next() ? oldCurrent->Next() : Last();
			// Make the newOjbect current and scroll into view.
	 	 	UIW_WINDOW::Add(object);
			UI_EVENT sEvent(S_VSCROLL_CHECK);
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
				newIndex = MaxValue(0, Index(oldCurrent) - (vScrollInfo.showing - 1));
				object = (UI_WINDOW_OBJECT *)UI_LIST::Get(newIndex);
			}
			else
			{
				newIndex = MinValue(Index(Last()), Index(oldCurrent) + vScrollInfo.showing - 1);
				object = (UI_WINDOW_OBJECT *)UI_LIST::Get(newIndex);
			}
			// Make the newOjbect current and scroll into view.
			Add(object);
			UI_EVENT sEvent(S_VSCROLL_CHECK);
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
			UI_EVENT sEvent(S_VSCROLL_CHECK);
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
		if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
		{
			parent->Information(I_RESET_SELECTION, ZIL_NULLP(void));
			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY), Q_BEGIN);
		}
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
					object->relative.left = 0;
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
			dragObject->relative.left = 0;
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
					object->relative.left = 0;
					object->woStatus &= ~(WOS_CURRENT | WOS_SELECTED);
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
			ZIL_NEW_FUNCTION newFunction = dragObject->NewFunction();
			ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
			ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
			dragObject->Store(name, &directory, &file);
			file.Seek(0);
			dragObject = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
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
		if (event.type == E_KEY && parent->Inherited(ID_COMBO_BOX))
		{
			WNF_FLAGS comboFlags = 0;
			parent->Information(I_GET_FLAGS, &comboFlags, ID_WINDOW);
			if (!FlagSet(parent->woFlags, WOF_VIEW_ONLY) && !FlagSet(comboFlags, WNF_BITMAP_CHILDREN))
				return parent->Event(event);
		}
			
		// Try to do first character recognition.
		if (event.type == E_KEY && IsPrint(event.key.value) && Current())
		{
			ZIL_ICHAR buffer[2];
			ZIL_ICHAR *data;

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
				if (parent->Inherited(ID_COMBO_BOX) && windowManager->First() == this)
					Event(UI_EVENT(S_CREATE));
				else
				{
					UI_EVENT sEvent(S_VSCROLL_CHECK);
					ScrollEvent(sEvent);
				}
			}
		}
		break;
	}

	// Check for a special combo-box condition.
	if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY) && parent && (oldCurrent != current ||
		ccode == L_SELECT) && ccode != S_INITIALIZE)
		parent->Information(I_RESET_SELECTION, Current());

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_VT_LIST::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	UI_WINDOW_OBJECT *object = Current();
	switch(ccode)
	{

	case S_SCROLLRANGE:
		{
		// Set up clientArea, range, showing, and max scroll area.
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;

		vScrollInfo.delta = 1;
		vScrollInfo.showing = MaxValue(1,(client.bottom - client.top + 1) / cellHeight);
		vScrollInfo.maximum = MaxValue(0, Count() - vScrollInfo.showing);

		if (vScrollInfo.current > vScrollInfo.maximum)
			vScrollInfo.current = vScrollInfo.maximum;

		if (vScroll)
		{
			// Reset the scrollbar information.
			event.type = S_VSCROLL_SET;
			event.scroll = vScrollInfo;
			vScroll->Event(event);
		}
		}
		break;

	case S_VSCROLL_CHECK:
		if (object)
		{
		// Insure that object is scrolled into view.
		event.type = S_VSCROLL;
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
		if (object->trueRegion.top < client.top)
		{
			UI_WINDOW_OBJECT *top;
	 		for (top = First(); top &&
				top->trueRegion.top < client.top; top = top->Next())
 				;
			event.scroll.delta = Index(object) - Index(top);
			ccode = ScrollEvent(event);
		}
		else if (object->trueRegion.bottom > client.bottom)
		{
			UI_WINDOW_OBJECT *bottom;
	 		for (bottom = Last(); bottom && bottom->trueRegion.bottom > client.bottom;
	 			bottom = bottom->Previous())
				;
			event.scroll.delta = Index(object) - Index(bottom);
			ccode = ScrollEvent(event);
		}
		}
		break;

	case S_VSCROLL_WINDOW:
		{
		scroll.top -= event.scroll.delta * cellHeight;
		for (object = First(); object; object = object->Next())
			object->Event(UI_EVENT(S_CHANGED));

		if (parent && parent->Inherited(ID_COMBO_BOX) &&
			windowManager->Index(this) == -1)
			break;
		if (screenID)
		{
			UI_REGION region = trueRegion;
			UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
			UI_EVENT sEvent(S_REDISPLAY);
			display->VirtualGet(screenID, region);
			for (object = First(); object; object = object->Next())
				object->Event(sEvent);
			display->VirtualPut(screenID);
			if (Last() && Last()->trueRegion.bottom < client.bottom)
			{
				// Update remaining empty space in the list.
				region = client;
				region.top = Last()->trueRegion.bottom;
				region.right = Last()->trueRegion.right;
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

void UIW_VT_LIST::OSDestroy(void)
{
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
		event.type = S_REDISPLAY;
		Event(event);
	}
}

void UIW_VT_LIST::OSSort(void)
{
	Event(UI_EVENT(S_REDISPLAY));
}

