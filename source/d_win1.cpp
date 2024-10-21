//	Zinc Interface Library - D_WIN1.CPP
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

// ----- UIW_WINDOW ---------------------------------------------------------

UI_WINDOW_OBJECT *UIW_WINDOW::Add(UI_WINDOW_OBJECT *object)
{
	int newObject = FALSE;

	// See if the new object exists or is already current.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
 	// Add the object to the list.
	if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		support.Index(object) == -1)
	{
		newObject = TRUE;
		support.Add(object);
		object->parent = this;
	}
	else if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		((!object->Previous() && !object->Next() && object != First()) ||
		 UI_LIST::Index(object) == -1))
	{
		newObject = TRUE;
		UI_LIST::Add(object);
		object->parent = this;
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			if (current)
				Current()->woStatus &= ~WOS_CURRENT;
			if (RedisplayType() == S_CURRENT)
				object->woStatus |= WOS_CURRENT;
			current = object;
		}
	}
	else
	{
		UI_WINDOW_OBJECT *tObject = Current();
		UI_REGION updateRegion;
		if (tObject)
			updateRegion = tObject->trueRegion;
		else
			updateRegion = object->trueRegion;

		// Place MDI objects at the end of the list.
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			UI_LIST::Subtract(object);
			UI_LIST::Add(object);
			object->Event(UI_EVENT(S_REGION_DEFINE));
			updateRegion = object->trueRegion;
		}

		// Make the old current field non-current.
		if (tObject && tObject != object)
		{
			if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
				tObject->woStatus &= ~WOS_CURRENT;
			if (!object->Inherited(ID_PULL_DOWN_MENU))
				tObject->Event(UI_EVENT(S_NON_CURRENT, 0, object->trueRegion));
			if (tObject->Inherited(ID_PULL_DOWN_MENU))
				for (UI_WINDOW_OBJECT *oldCurrent = First(); oldCurrent;
					oldCurrent = oldCurrent->Next())
					if (FlagSet(oldCurrent->woStatus, WOS_CURRENT) && oldCurrent != object)
						oldCurrent->Event(UI_EVENT(S_NON_CURRENT, 0, object->trueRegion));
		}

		if (Inherited(ID_LIST) || (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
			!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
			current = object;

		// Update the new current field.
		if (tObject != object)
		{
			object->woStatus |= WOS_REDISPLAY;

			// Make sure the base window is current on the screen.
			if (object->screenID && !FlagSet(woStatus, WOS_INTERNAL_ACTION))
			{
				eventManager->DeviceState(E_CURSOR, D_OFF);
				object->Event(UI_EVENT(RedisplayType(), 0, updateRegion));
			}
			if (object->screenID && FlagSet(wnFlags, WNF_AUTO_SELECT))
				object->Event(L_SELECT);
			else if (FlagSet(wnFlags, WNF_AUTO_SELECT))
				object->woStatus |= WOS_SELECTED;
		}
	}

	// Initialize the object if its parent is already shown on the screen.
	if (screenID && !object->screenID && newObject)
	{
		object->Event(UI_EVENT(S_INITIALIZE));
		object->Event(UI_EVENT(S_CREATE));
		// Define a screen region for MDI objects.
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			object->Event(UI_EVENT(S_REGION_DEFINE));
			if (object->Previous() && FlagSet(object->Previous()->woAdvancedFlags,
				WOAF_MDI_OBJECT))
				object->Previous()->Event(UI_EVENT(S_NON_CURRENT, 0, object->trueRegion));
		}
		if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			clipList.Split(screenID, object->trueRegion, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);
		if (!current && !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
			!FlagSet(object->woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT))
			current = object;
	}
	else if (screenID && parent && parent->Inherited(ID_COMBO_BOX) && newObject)
	{
		object->Event(UI_EVENT(S_INITIALIZE));
		object->Event(UI_EVENT(S_CREATE));
	}

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UIW_WINDOW::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	display->VirtualGet(screenID, trueRegion);
	UI_REGION region = trueRegion;

#if defined (ZIL_MOTIF_STYLE) && !defined(ZIL_TEXT_ONLY)
	// Don't allow borders to write over support objects.
	if (!display->isText && clipList.First())
	{
		region = clipList.First()->region;
		region += ZIL_BORDER_WIDTH;
	}
#endif
#if defined(ZIL_3D_BORDER) && !defined(ZIL_GRAPHICS_ONLY)
	if (display->isText && Inherited(ID_PULL_DOWN_MENU))
		DrawBorder(screenID, region, FALSE, ccode);
#endif
	if (FlagSet(woFlags, WOF_BORDER) && trueRegion.Overlap(event.region) &&	(!display->isText ||
		Inherited(ID_POP_UP_MENU) || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
		DrawBorder(screenID, region, FALSE, ccode);

	if (Inherited(ID_GROUP))
	{
		UI_WINDOW_OBJECT *bObject = NULL;
		for (bObject = this; bObject &&
			display->MapColor(bObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE) == BACKGROUND;
			bObject = bObject->parent)
			;
		lastPalette = bObject->LogicalPalette(ccode);
	}

	UI_REGION tRegion;
	for (UI_REGION_ELEMENT *element = clipList.First(); element; element = element->Next())
		if (element->region.Overlap(event.region, tRegion))
			display->Rectangle(screenID, tRegion, lastPalette, 0, TRUE, FALSE, &clip);

	display->VirtualPut(screenID);
	return (FALSE);	// Draw operation is not complete until children are drawn.
}

EVENT_TYPE UIW_WINDOW::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Check for a minimized object.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);
	if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
		switch(ccode)
			{
			case S_CHANGED:
			case S_CREATE:
			case L_MOVE:
			case L_RESTORE:
			case L_MAXIMIZE:
			case L_MINIMIZE:
			case S_DEINITIALIZE:
			case S_RESET_DISPLAY:
			case S_REGION_DEFINE:
			case S_INITIALIZE:
			case E_KEY:
			case S_ALT_KEY:
				break;
			default:
				return icon->Event(event);
			}

	// Switch on the event type.
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		WOS_STATUS oldStatus = woStatus;
		UI_WINDOW_OBJECT::Event(event);
		UI_WINDOW_OBJECT *minButton = Get(NUMID_MINIMIZE);
		UI_WINDOW_OBJECT *maxButton = Get(NUMID_MAXIMIZE);

		// See if initializing window minimized or maximized.
		if (FlagSet(woStatus, WOS_MINIMIZED) && !parent && minButton &&
			!FlagSet(minButton->woStatus, WOS_MINIMIZED))
		{
			for (object = (UI_WINDOW_OBJECT *)support.First(); object &&
				!object->Inherited(ID_ICON); object = object->Next())
				;
			if (object)
				icon = object;

			woStatus &= ~WOS_MINIMIZED;
			UI_EVENT tEvent(L_MINIMIZE);
			tEvent.data = this;

			// Prevent the S_CHANGED message from being sent.
			woStatus |= WOS_INTERNAL_ACTION;
			Event(tEvent);
			woStatus &= ~WOS_INTERNAL_ACTION;
		}
		else if (FlagSet(woStatus, WOS_MAXIMIZED) && !parent && maxButton &&
			!FlagSet(maxButton->woStatus, WOS_MAXIMIZED))
		{
			woStatus &= ~WOS_MAXIMIZED;
			UI_EVENT tEvent(L_MAXIMIZE);
			tEvent.data = this;

			// Prevent the S_CHANGED message from being sent.
			woStatus |= WOS_INTERNAL_ACTION;
			Event(tEvent);
			woStatus &= ~WOS_INTERNAL_ACTION;
		}
		else if (FlagSet(woStatus, WOS_GRAPHICS) &&
			FlagSet(woStatus, WOS_MINIMIZED | WOS_MAXIMIZED) && oldStatus != woStatus)
		{
			restore.left *= display->cellWidth;
			restore.top *= display->cellHeight;
			restore.right *= display->cellWidth;
			restore.bottom *= display->cellHeight;
		}
		}
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
				if (FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					object->woStatus |= WOS_SELECTED;
			}
			else
				object->woStatus &= ~WOS_CURRENT;
			object->Event(event);
		}

		// Try to make a support object current.
		if (!current)
			for (object = (UI_WINDOW_OBJECT *)support.Last(); object;
				object = object->Previous())
				if ((object->Inherited(ID_TOOL_BAR) ||
					object->Inherited(ID_PULL_DOWN_MENU)) &&
					!FlagSet(woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
				{
				 	current = object;
					break;
			  	}

		hScrollInfo.delta = vScrollInfo.delta = display->cellHeight;
		break;

	case S_DEINITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		break;

	case S_CHANGED:
	case S_CREATE:
		if (FlagSet(woStatus, WOS_MINIMIZED))
		{
			object = Get(NUMID_MINIMIZE);
			if (parent && (!icon || (icon && !icon->screenID)) && object &&
				!FlagSet(object->woStatus, WOS_MINIMIZED))
			{
				woStatus &= ~WOS_MINIMIZED;
				UI_EVENT tEvent(L_MINIMIZE);
				tEvent.data = this;
				Event(tEvent);
			}
			if (icon)
			{
				UI_WINDOW_OBJECT::Event(event);
				if (parent)
				{
					icon->clip = clip;
					icon->relative = relative;
				}
				else
					icon->clip.left = icon->clip.top = 0, icon->clip.right =
					display->columns - 1, icon->clip.bottom = display->lines - 1;
				icon->Event(event);
			}

			// Set minimized window clipList so no child objects can show.
			clipList.Destroy();
			UI_REGION region;
			region.left = region.top = 2000;
			region.right = region.left + restore.Width() - 1;
			region.bottom = region.top + restore.Height() - 1;
			clipList.Add(new UI_REGION_ELEMENT(screenID, region));
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
				object->Event(event);
			for (object = First(); object; object = object->Next())
				object->Event(event);
			break;
		}

		// Compute the object region.
		clipList.Destroy();
		// Specify an absolute minimum size for non-parent and MDI windows.
		if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			int minWidth = 12 * display->cellWidth;
			if (relative.Width() < minWidth)
				relative.right = relative.left + minWidth - 1;
			int minHeight = display->cellHeight;
			if (display->isText)
				minHeight *= 3;
			else
				minHeight += 2 * UIW_BORDER::width;
			if (relative.Height() < minHeight)
				relative.bottom = relative.top + minHeight - 1;
		}

		if (FlagSet(woStatus, WOS_MAXIMIZED) && !parent)
		{
			relative.left = relative.top = 0;
			relative.right = display->columns - 1;
			relative.bottom = display->lines - 1;
#	if defined(ZIL_SHADOW_BORDER) && !defined(ZIL_GRAPHICS_ONLY)
			NUMBERID borderID = NUMID_BORDER;
			if (display->isText && Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&borderID)))
				relative.right++, relative.bottom++;
#	endif
		}
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_MAXIMIZED) && parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			trueRegion = clip;
			object = Get(NUMID_MAXIMIZE);
			if (object && !FlagSet(object->woStatus, WOS_MAXIMIZED))
			{
				woStatus &= ~WOS_MAXIMIZED;
				UI_EVENT tEvent(L_MAXIMIZE);
				tEvent.data = this;
				Event(tEvent);
			}
		}
		{
		UI_REGION region = trueRegion;
		if (!display->isText && Inherited(ID_TABLE_RECORD) && FlagSet(woFlags, WOF_BORDER))
		{
			if (Inherited(ID_TABLE_HEADER))
				--region;
#if defined (ZIL_MSWINDOWS_STYLE)
			else
				region.right--, region.bottom--;
#endif
		}
		else if (FlagSet(woFlags, WOF_BORDER) && (!display->isText || Inherited(ID_POP_UP_MENU) ||
			FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
		{
			region -= (display->isText ? 1 : ZIL_BORDER_WIDTH);
#	if defined(ZIL_OS2_STYLE)
			if (!display->isText && FlagSet(woFlags, WOAF_TEMPORARY) && !Inherited(ID_LIST))
				region.left++;
#	endif
		}
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));

		// Compute the support object regions.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION))
			{
				UI_REGION region = object->trueRegion;
				clipList.Destroy();
#if defined(ZIL_SHADOW_BORDER) && !defined(ZIL_GRAPHICS_ONLY)
				if (display->isText)
					clipList.Add(new UI_REGION_ELEMENT(screenID, region));
				else
					clipList.Add(new UI_REGION_ELEMENT(screenID, --region));
#else
				clipList.Add(new UI_REGION_ELEMENT(screenID, --region));
#endif
			}
			else if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);
		}

		// Compute the remaining object regions.
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);
		}

		// Let the window set its scrolling information.
		if (!FlagSet(wnFlags, WNF_NO_SCROLL))
		{
			UI_EVENT sEvent(S_SCROLLRANGE);
			ScrollEvent(sEvent);
		}
		}
		break;

	case S_SCROLLRANGE:
		{
		UI_EVENT sEvent(ccode);
		ScrollEvent(sEvent);
		if (vScroll)
			vScroll->Event(UI_EVENT(S_REDISPLAY));
		if (hScroll)
			hScroll->Event(UI_EVENT(S_REDISPLAY));
		}
		break;

	case S_VSCROLL:
	case S_HSCROLL:
	case S_VSCROLL_CHECK:
	case S_HSCROLL_CHECK:
		{
		UI_EVENT sEvent = event;
		ccode = ScrollEvent(sEvent);
		}
		break;

	case S_SIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

#if !defined(ZIL_GRAPHICS_ONLY)
	case S_RESET_DISPLAY:
		{
		if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS))
		{
			restore.left /= display->cellWidth;
			restore.top /= display->cellHeight;
			restore.right /= display->cellWidth;
			restore.bottom /= display->cellHeight;
		}

		if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
			icon->Event(event);
		UI_WINDOW_OBJECT::Event(event);

		// Convert each object's coordinates from graphics.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (!(FlagSet(woStatus, WOS_MINIMIZED) && object == icon))
				object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		}
		break;
#endif

	case S_REGION_DEFINE:
		{
		UI_REGION region = trueRegion;
		if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
		{
			if (parent)
				icon->clip = clip;
			else
				icon->clip.left = icon->clip.right = 0, icon->clip.right =
				display->columns - 1, icon->clip.bottom = display->lines - 1;
			ccode = icon->Event(event);
			trueRegion = icon->trueRegion; relative = icon->relative;
			break;
		}
		if (parent)
		{
			UI_REGION clipRegion = clip;
#if (defined(ZIL_SHADOW_BORDER) || defined(ZIL_3D_BORDER)) && !defined(ZIL_GRAPHICS_ONLY)
			if (display->isText)
				clipRegion.right++, clipRegion.bottom++;
#endif
			region.left = MaxValue(region.left, clipRegion.left);
			region.top = MaxValue(region.top, clipRegion.top);
			region.right = MinValue(region.right, clipRegion.right);
			region.bottom = MinValue(region.bottom, clipRegion.bottom);
		}
#if (defined(ZIL_SHADOW_BORDER) || defined(ZIL_3D_BORDER)) && !defined(ZIL_GRAPHICS_ONLY)
		if (display->isText)
		{
			NUMBERID borderID = NUMID_BORDER;
			if (Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&borderID)) && !FlagSet(woStatus, WOS_MAXIMIZED))
			{
				region.right--;
				region.bottom--;
			}
		}
#endif
		display->RegionDefine(screenID, region);
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			for (object = First(); object; object = object->Next())
				if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
					object->Event(event);
		}
		}
		break;

	case L_MOVE:
	case L_SIZE:
		if (event.type != E_KEY && !event.rawCode && event.data != this)
			Current()->Event(event);
		else if ((ccode == L_MOVE && !FlagSet(woStatus, WOS_MAXIMIZED) &&
			!FlagSet(woAdvancedFlags, WOAF_NO_MOVE)) || (ccode == L_SIZE &&
			!FlagsSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED) &&
			!FlagSet(woAdvancedFlags, WOAF_NO_SIZE)))
		{
			UI_EVENT sEvent;
			sEvent = event;
			if (ccode != sEvent.type)
			{
				sEvent.type = ccode;
				sEvent.rawCode = 0;
			}
			Modify(sEvent);

			if (parent && FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT))
				parent->Event(UI_EVENT(S_SCROLLRANGE));

#if defined(ZIL_EDIT)
			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				UI_EVENT dEvent(D_SET_OBJECT);
				dEvent.rawCode = searchID;
				dEvent.data = this;
				eventManager->Put(dEvent);
			}
#endif
		}
		break;

	case S_MDICHILD_EVENT + S_CLOSE:
		if (!parent && last && FlagSet(Last()->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			object = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? Last() : Current();
			if (!object)
				break;

 			UIW_WINDOW::Subtract(object);

			// Redefine and redisplay the MDI children.
			if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
				Event(UI_EVENT(S_REGION_DEFINE));
			Event(UI_EVENT(RedisplayType(), 0, object->trueRegion));
			if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
				delete object;
		}
		break;

	case L_MDICHILD_EVENT + L_MOVE:
	case L_MDICHILD_EVENT + L_SIZE:
	case S_MDICHILD_EVENT + S_RESTORE:
	case S_MDICHILD_EVENT + S_MINIMIZE:
	case S_MDICHILD_EVENT + S_MAXIMIZE:
		{
		UI_EVENT tEvent = event;
		tEvent.rawCode = 0;
		if (ccode == L_MDICHILD_EVENT + L_MOVE ||
			ccode == L_MDICHILD_EVENT + L_SIZE)
			tEvent.type = ccode - L_MDICHILD_EVENT;
		else
			tEvent.type = ccode - S_MDICHILD_EVENT;
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && last)
		{
			tEvent.data = Last();
			Last()->Event(tEvent);
		}
		else if (parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			tEvent.data = this;
			Event(tEvent);
		}
		else
			ccode = S_UNKNOWN;
		}
		break;

	case L_RESTORE:
	case L_MINIMIZE:
	case L_MAXIMIZE:
		if (event.type != E_KEY && !event.rawCode && event.data != this)
			return Current()->Event(event);
		{
		UI_REGION region = trueRegion;
		if (ccode == L_MINIMIZE)
		{
			object = Get(NUMID_MINIMIZE);
			if (!object || FlagSet(woStatus, WOS_MINIMIZED))
				return ccode;
			woStatus |= WOS_MINIMIZED;
			object->woStatus |= WOS_MINIMIZED;

			if (!FlagsSet(woStatus, WOS_MAXIMIZED))
				restore = relative;
			woStatus &= ~WOS_MAXIMIZED;
			if (!icon)
			{
				ZIL_ICHAR *title;
				Information(I_GET_TEXT, &title);
				*this + (icon = new UIW_ICON(0, 0, ZIL_NULLP(ZIL_ICHAR), title,
					ICF_MINIMIZE_OBJECT));
			}
			else
			{
				ZIL_ICHAR *title;
				Information(I_GET_TEXT, &title);
				icon->Information(I_SET_TEXT, title);
			}
			if (!icon->screenID)
			{
				icon->Event(UI_EVENT(S_INITIALIZE));
				icon->Event(UI_EVENT(S_CREATE));
			}
			UI_REGION clipRegion = region;
			if (parent)
				parent->Information(I_GET_CLIPREGION, &clipRegion);
			else
				clipRegion.left = clipRegion.top = 0, clipRegion.right =
				display->columns - 1, clipRegion.bottom = display->lines - 1;

			// Do icon placement if first time minimized
			if (!FlagSet(icon->woStatus, WOS_MINIMIZED))
			{
				icon->woStatus |= WOS_MINIMIZED;
				int width = icon->relative.Width() - 1;
				int height = icon->relative.Height() - 1;
				int left = display->cellWidth;
				int top = clipRegion.Height() - (height + display->cellWidth);
				UI_REGION iconRegion;
				iconRegion.left = left;	iconRegion.top = top;
				iconRegion.right = left + width;	iconRegion.bottom = top + height;
				object = parent ? this : windowManager->Last();
				int overLapping = FALSE;
				while (object)
				{
					if (FlagSet(object->woStatus, WOS_MINIMIZED) && object != this &&
						iconRegion.Overlap(object->relative))
						overLapping = TRUE;
					if (overLapping)
					{
						left = iconRegion.left = object->relative.right + 2 * display->cellWidth;
						if (left + width > clipRegion.right)
						{
							overLapping = TRUE;
							top -= (height + display->cellWidth);
							iconRegion.top = top;
							left = iconRegion.left = display->cellWidth;
						}
						iconRegion.right = iconRegion.left + width;
						iconRegion.bottom = top + height;
						overLapping = FALSE;
						object = parent ? this : windowManager->Last();
					}
					object = object->Previous();
				}
				relative = iconRegion;
				icon->relative = relative;
			}
			else
				relative = icon->relative;
			if (!screenID)
				return ccode;
		}
		else if (ccode == L_MAXIMIZE)
		{
			object = Get(NUMID_MAXIMIZE);
			if (!object || FlagSet(woStatus, WOS_MAXIMIZED))
				return ccode;

			woStatus |= WOS_MAXIMIZED;
			object->woStatus |= WOS_MAXIMIZED;

			if (!FlagSet(woStatus, WOS_MINIMIZED))
				restore = relative;
			woStatus &= ~WOS_MINIMIZED;
			relative.left = relative.top = 0;
			if (parent)
			{
				relative.right = parent->relative.right - parent->relative.left;
				relative.bottom = parent->relative.bottom - parent->relative.top;
			}
			else
			{
				relative.right = display->columns - 1;
				relative.bottom = display->lines - 1;
#if defined(ZIL_SHADOW_BORDER) && !defined(ZIL_GRAPHICS_ONLY)
				NUMBERID borderID = NUMID_BORDER;
				if (display->isText && Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&borderID)))
					relative.right++, relative.bottom++;
#endif
			}
		}
		else if (ccode == L_RESTORE)
		{
			if (!FlagSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
				return ccode;
			relative = restore;
			woStatus &= ~(WOS_MINIMIZED | WOS_MAXIMIZED);
		}
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
			break;

		Event(UI_EVENT(S_CHANGED));
		eventManager->DeviceState(E_CURSOR, D_OFF);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);

		if (!parent)
		{
			display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
			for (object = windowManager->Last(); object; object = object->Previous())
				object->Event(UI_EVENT(S_REGION_DEFINE));
			display->Rectangle(ID_SCREEN, region, display->backgroundPalette, 0, TRUE);
			for (object = windowManager->Last(); object; object = object->Previous())
				if (object == this)
					object->Event(UI_EVENT(S_CURRENT, 0, trueRegion));
				else if (object->trueRegion.Overlap(region))
					object->Event(UI_EVENT(S_DISPLAY_INACTIVE, 0, region));
		}
		else
		{
			if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
				parent->Event(UI_EVENT(S_REGION_DEFINE));
			region.left = MinValue(region.left, trueRegion.left);
			region.top = MinValue(region.top, trueRegion.top);
			region.right = MaxValue(region.right, trueRegion.right);
			region.bottom = MaxValue(region.bottom, trueRegion.bottom);
			parent->Event(UI_EVENT(S_CURRENT, 0, region));
		}

		if (ccode == L_MINIMIZE)
		{
			// Try to make another window active.
			UI_WINDOW_OBJECT *makeCurrent = this;
			while (makeCurrent)
			{
				makeCurrent = (parent && FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) ?
					makeCurrent->Previous() : makeCurrent->Next());
				if (makeCurrent && 
					!FlagSet(makeCurrent->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(makeCurrent->woStatus, WOS_MINIMIZED) &&
					!FlagSet(makeCurrent->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					UI_EVENT addEvent(S_ADD_OBJECT);
					addEvent.windowObject = makeCurrent; 
					if (parent)
						parent->Event(addEvent);
					else
						windowManager->Event(addEvent);
					break;
				}
			}
		}
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (!screenID)
			break;
		{

		UI_WINDOW_OBJECT::Event(event);
		display->VirtualGet(screenID, trueRegion);
		UI_EVENT event1 = event;

		// Check the status of the currently selected object.
		object = Current();
		if (ccode == S_CURRENT)
			eventManager->DeviceState(E_CURSOR, D_OFF);

		if (!parent && object && object->Inherited(ID_PULL_DOWN_MENU))
		{
 			UI_WINDOW_OBJECT *oldCurrent = (UI_WINDOW_OBJECT *)object->userObject;
 			if (oldCurrent && ccode == S_NON_CURRENT && !oldCurrent->Inherited(ID_PULL_DOWN_ITEM))
			{
				object->userObject = ZIL_NULLP(void);
				SetCurrent(oldCurrent);
				if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && oldCurrent)
					oldCurrent->Event(UI_EVENT(S_NON_CURRENT, 0, oldCurrent->trueRegion));
				else if (oldCurrent)
	 				UIW_WINDOW::Add(oldCurrent);
			}
		}

		// Draw the window's border and fill the background.
		if (FlagSet(woStatus, WOS_REDISPLAY) && searchID != ID_COMBO_BOX &&
			!(searchID == ID_GROUP && !FlagSet(woStatus, WOS_EDIT_MODE) &&
			!FlagSet(woFlags, WOF_NON_FIELD_REGION)))
			DrawItem(event1, ccode);

		// Update the children.
#if defined(ZIL_3D_BORDER)
		++event1.region;
#endif
		UI_EVENT aEvent = event1;
		if (ccode == S_CURRENT)
			aEvent.type = S_DISPLAY_ACTIVE;

		else if (ccode == S_NON_CURRENT ||
			(ccode == S_DISPLAY_ACTIVE && parent &&
			parent->Information(I_GET_CURRENT, ZIL_NULLP(void)) != this))
			aEvent.type = S_DISPLAY_INACTIVE;

		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (object == current || FlagSet(object->woStatus, WOS_CURRENT))
				object->Event(event1);
			else
				object->Event(aEvent);
		for (object = First(); object; object = object->Next())
			if (object == current && (ccode == S_CURRENT || ccode == S_NON_CURRENT))
				object->Event(event1);
			else if (object->trueRegion.Overlap(event1.region))
				object->Event(FlagSet(object->woStatus, WOS_CURRENT) ? event1 : aEvent);

		woStatus &= ~WOS_REDISPLAY;
		display->VirtualPut(screenID);
		}
		break;

	case S_REDISPLAY:
		if (screenID && FlagSet(woFlags, WOF_NON_FIELD_REGION) && parent &&
			!Inherited(ID_SCROLL_BAR))
			parent->Event(UI_EVENT(S_CHANGED));
		else if (screenID && !Inherited(ID_SCROLL_BAR))
			Event(UI_EVENT(S_CHANGED));
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_ADD_OBJECT:
		// Add the window object to the window.
		Add(event.windowObject);
		break;

	case S_SUBTRACT_OBJECT:
		// Subtract the window object from the window.
		Subtract(event.windowObject);
		break;

	case L_HELP:
		// See if the low level object processes the help.
		if (!FlagSet(woStatus, WOS_MINIMIZED) &&
			current && Current()->Event(event) != S_UNKNOWN)
			break;
		else if (parent && helpContext == NO_HELP_CONTEXT)
		{
			if (Inherited(ID_MENU) && helpSystem)
				helpSystem->DisplayHelp(windowManager, helpContext);
			else
				ccode = S_UNKNOWN;
		}
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case L_FIRST:
		if (FlagSet(woStatus, WOS_MINIMIZED))
			return (S_UNKNOWN);
		ccode = current ? Current()->Event(event) : S_UNKNOWN;
		for (object = First(); object && ccode == S_UNKNOWN; object = object->Next())
			if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
			{
				wnFlags |= WNF_MOVE_REVERSE;
				UIW_WINDOW::Add(object);
				wnFlags &= ~WNF_MOVE_REVERSE;
				ccode = L_FIRST;
			}
		break;

	case L_LAST:
		if (FlagSet(woStatus, WOS_MINIMIZED))
			return (S_UNKNOWN);
		ccode = current ? Current()->Event(event) : S_UNKNOWN;
		for (object = Last(); object && ccode == S_UNKNOWN; object = object->Previous())
			if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
			{
				wnFlags |= WNF_MOVE_FORWARD;
				UIW_WINDOW::Add(object);
				wnFlags &= ~WNF_MOVE_FORWARD;
				ccode = L_LAST;
			}
		break;

	case L_PREVIOUS:
		if (FlagSet(woStatus, WOS_MINIMIZED))
			return (S_UNKNOWN);
		{
		EVENT_TYPE tcode = ccode;
		ccode = S_UNKNOWN;
		// Make sure there is a current object.
		if (!current)
			UIW_WINDOW::Add(Last());
		// See if the current object processes the message.
		else if (Current()->Event(event) != S_UNKNOWN)
			ccode = tcode;
		// Don't move if on a support object.
		else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
			UI_ERROR_SYSTEM::Beep();
		// Don't move if on the first field of a non-wrapping window.
		else if (!Current()->Previous() && FlagSet(wnFlags, WNF_NO_WRAP))
			break;
		else if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			ccode = tcode;
		// Go to the previous field in the window.
		else
		{
			object = Current()->Previous() ? Current()->Previous() : Last();
			while (object && object != current && ccode == S_UNKNOWN)
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					wnFlags |= WNF_MOVE_REVERSE;
					UIW_WINDOW::Add(object);
					wnFlags &= ~WNF_MOVE_REVERSE;
					ccode = tcode;
				}
				else if (object->Previous())
					object = object->Previous();
				else if (FlagSet(wnFlags, WNF_NO_WRAP))
					break;
				else
					object = Last();
			if (FlagSet(Current()->woFlags, WOF_NON_SELECTABLE))
				current = ZIL_NULLP(UI_ELEMENT);
		}
		}
		break;

	case L_MDICHILD_EVENT + L_NEXT_WINDOW:
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT)	&&
			last != first)
			Add(First());
		break;

	case L_NEXT:
		if (FlagSet(woStatus, WOS_MINIMIZED))
			return (S_UNKNOWN);
		{
		EVENT_TYPE tcode = ccode;
		ccode = S_UNKNOWN;
		// Make sure there is a current object.
		if (!current)
		{
			for (object = First(); object && FlagSet(object->woFlags, WOF_NON_SELECTABLE); object = object->Next())
				;
			if (object)
				UIW_WINDOW::Add(object);
		}
		// See if the current object processes the message.
		else if (Current()->Event(event) != S_UNKNOWN)
			ccode = tcode;
		// Don't move if on a support object.
		else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
			UI_ERROR_SYSTEM::Beep();
		// Don't move if on the last field of a non-wrapping window.
		else if (!Current()->Next() && FlagSet(wnFlags, WNF_NO_WRAP))
			;
		else if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			ccode = tcode;

		// Go to the next field in the window.
		else
		{
			object = Current()->Next() ? Current()->Next() : First();
			while (object && object != current && ccode == S_UNKNOWN)
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					wnFlags |= WNF_MOVE_FORWARD;
					UIW_WINDOW::Add(object);
					wnFlags &= ~WNF_MOVE_FORWARD;
					ccode = tcode;
				}
				else if (object->Next())
					object = object->Next();
				else if (FlagSet(wnFlags, WNF_NO_WRAP))
					break;
				else
					object = First();
			if (FlagSet(Current()->woFlags, WOF_NON_SELECTABLE))
				current = ZIL_NULLP(UI_ELEMENT);
		}
		}
		break;

	case L_SELECT:
		{
		if (current)
			ccode = Current()->Event(event);
		if ((!current || (ccode != S_UNKNOWN && ccode != S_ERROR)) && defaultObject)
			ccode = defaultObject->Event(UI_EVENT(L_SELECT));
		}
		break;

	case L_CANCEL:
	case S_ALT_KEY:
		// Bring up or take down the pull-down menu (if any).
		for (object = (UI_WINDOW_OBJECT *)support.Last(); object; object = object->Previous())
			if (object->Inherited(ID_PULL_DOWN_MENU) && !FlagSet(woStatus, WOS_MINIMIZED))
			{
				// Restore the old selected window object.
				if (FlagSet(object->woStatus, WOS_CURRENT))
				{
					current = (UI_WINDOW_OBJECT *)object->userObject;
					object->Event(UI_EVENT(S_NON_CURRENT, 0, object->trueRegion));

					if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
						current = Last();
					else if (current)
					{
						Add(Current());
						UI_REGION region;
						region.left = region.top = region.right = region.bottom = -1;
						Current()->Event(UI_EVENT(S_CURRENT, 0, region));
					}
				}
				// Activate the pull-down menu.
				else if (ccode == S_ALT_KEY  && !FlagSet(object->woFlags,
					WOF_NON_SELECTABLE))
				{
					object->userObject = current;
					UIW_WINDOW::Add(object);
				}
				return (ccode);
			}
			else if (ccode == S_ALT_KEY && object->Inherited(ID_SYSTEM_BUTTON))
			{
				object->Event(UI_EVENT(L_SELECT));
				return (ccode);
			}
		ccode = S_UNKNOWN;
		break;

	case S_DRAG_DEFAULT:
		ccode = S_DRAG_MOVE_OBJECT;
		// Fall through to S_DRAG_MOVE_OBJECT.
	case S_DRAG_MOVE_OBJECT:
	case S_DRAG_COPY_OBJECT:
		if (!Inherited(ID_LIST))
		{
			for (object = First(); object; object = object->Next())
				if (object->trueRegion.Overlap(event.position))
					return (object->Event(event));
			ccode = UI_WINDOW_OBJECT::Event(event);
		}
		else if (!FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP))
		{
			if (Index(windowManager->dragObject) == -1 &&
				windowManager->dragObject != this)
					eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
			else
				eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
					DM_DRAG_MOVE : DM_DRAG_COPY);
		}
		else if (windowManager->dragObject->Inherited(ID_LIST))
		{
			int count = 0;
			UI_WINDOW_OBJECT *object;
			for (windowManager->dragObject->Information(I_GET_FIRST, &object);
				object; object = object->Next())
					if (FlagSet(object->woStatus, WOS_SELECTED))
						count++;
			if (count > 1)
				eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
					DM_DRAG_MOVE_MULTIPLE : DM_DRAG_COPY_MULTIPLE);
			else
				eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
					DM_DRAG_MOVE : DM_DRAG_COPY);
		}
		else if (ccode == S_DRAG_COPY_OBJECT || (windowManager->dragObject->parent &&
			windowManager->dragObject->parent->Inherited(ID_LIST)))
				eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
					DM_DRAG_MOVE : DM_DRAG_COPY);
		else
			eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
		break;

	case S_DROP_DEFAULT:
		ccode = S_DROP_MOVE_OBJECT;
		// Fall through to S_DROP_MOVE_OBJECT.
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_BEGIN_ESCAPE:
	case L_CONTINUE_ESCAPE:
	case L_END_ESCAPE:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		// Continue to E_MOUSE.
	case E_MOUSE:
	case L_VIEW:
	case L_CONTINUE_COPY_DRAG:
	case L_END_COPY_DRAG:
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		if (windowManager->dragObject)
		{
			for (object = First(); object; object = object->Next())
				if (object->trueRegion.Overlap(event.position))
				{
					object->Event(event);
					break;
				}
			if (!object)
				for (object = (UI_WINDOW_OBJECT *)support.Last(); object; object = object->Previous())
					if (object->trueRegion.Overlap(event.position) && !object->Inherited(ID_BORDER))
					{
						object->Event(event);
						break;
					}

			if (!object)
				UI_WINDOW_OBJECT::Event(event);
			break;
		}

		// Check for mouse out-of-range.
		if (!trueRegion.Overlap(event.position))
		{
			if (current)
				return (Current()->Event(event));
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
			return (S_UNKNOWN);
		}

		// Check for an active support object.
		object = (UI_WINDOW_OBJECT *)support.Current();
		if (object && !object->trueRegion.Overlap(event.position))
		{
#if defined(ZIL_EDIT)
			if (!FlagSet(woStatus, WOS_EDIT_MODE))
#endif
				object->Event(event);
			support.SetCurrent(ZIL_NULLP(UI_ELEMENT));
		}

		// Let object unactivate its region.
		if ((ccode == L_END_SELECT || ccode == L_END_ESCAPE ||
		 	ccode == L_CONTINUE_SELECT || ccode == L_CONTINUE_ESCAPE) &&
			current && !Current()->trueRegion.Overlap(event.position))
		{
			Current()->Event(event);
			if (FlagSet(Current()->woAdvancedFlags, WOAF_NON_CURRENT))
			{
				for (object = First(); object; object = object->Next())
					if (FlagSet(object->woStatus, WOS_CURRENT))
					{
						current = object;
						break;
					}
			}
		}
#if defined(ZIL_SHADOW_BORDER)
		object = support.First() ? (UI_WINDOW_OBJECT *)support.First() : Last();
		while (object)
		{
			if (!(FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) && object->Inherited(ID_ICON)) &&
				((!FlagSet(object->woFlags, WOF_NON_SELECTABLE) || FlagSet(woStatus, WOS_EDIT_MODE)) &&
				((FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
				trueRegion.Overlap(event.position) && !object->trueRegion.Overlap(event.position)) ||
				(!FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
				object->trueRegion.Overlap(event.position)))))
#elif defined(ZIL_STANDARD_BORDER) || defined(ZIL_3D_BORDER)
		object = support.Last() ? (UI_WINDOW_OBJECT *)support.Last() : Last();
		while (object)
		{
			if (!(FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) && object->Inherited(ID_ICON)) &&
				((!FlagSet(object->woFlags, WOF_NON_SELECTABLE) || FlagSet(woStatus, WOS_EDIT_MODE)) &&
				((display->isText && FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
				object->trueRegion.Touching(event.position)) ||
				(!display->isText && (FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
				trueRegion.Overlap(event.position) && !object->trueRegion.Overlap(event.position))) ||
				(!FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
				object->trueRegion.Overlap(event.position)))))
#else
		{
#endif
			{
				if (FlagSet(woStatus, WOS_MINIMIZED))
					return (S_UNKNOWN);
				else if (FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					if (ccode == L_BEGIN_SELECT && current && current != object &&
						Current()->Inherited(ID_PULL_DOWN_MENU) && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
					{
						UI_WINDOW_OBJECT *windowCurrent = (UI_WINDOW_OBJECT *)Current()->userObject;
						Current()->Event(UI_EVENT(S_NON_CURRENT));
						if (windowCurrent &&	windowCurrent->screenID &&
							FlagSet(windowCurrent->woStatus, WOS_CURRENT) &&
							!windowCurrent->Inherited(ID_PULL_DOWN_ITEM))
							SetCurrent(windowCurrent);
						else
							SetCurrent(ZIL_NULLP(UI_ELEMENT));
					}
	 				return object->Event(event);
				}
				else if (ccode == L_BEGIN_SELECT || (ccode == L_CONTINUE_SELECT &&
					FlagSet(wnFlags, WNF_CONTINUE_SELECT)))
				{
					if (object != current && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
						object->userObject = current;
					UI_WINDOW_OBJECT *oldCurrent = Current();
					WOF_FLAGS flags = object->woFlags;
					object->woFlags &= ~WOF_AUTO_CLEAR;
					UIW_WINDOW::Add(object);
					object->woFlags = flags;
					if (oldCurrent && FlagSet(oldCurrent->woStatus, WOS_INVALID))
						return ccode;
				}
				else if ((ccode == L_END_SELECT || ccode == L_END_ESCAPE ||
					ccode == L_CONTINUE_SELECT || ccode == L_CONTINUE_ESCAPE) &&
					object != current && (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) ||
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
					return ccode;
				if (event.rawCode && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
				{
					support.SetCurrent(object);
					object->Event(event);
				}
				else
					object->Event(event);
				return (ccode);
			}

#if defined(ZIL_SHADOW_BORDER)
			if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
				object = object->Next() ? object->Next() : Last();
			else
				object = object->Previous() ? object->Previous() : ZIL_NULLP(UI_WINDOW_OBJECT);
#elif defined(ZIL_STANDARD_BORDER) || defined(ZIL_3D_BORDER)
			if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
				object = object->Previous() ? object->Previous() : Last();
			else
				object = object->Previous() ? object->Previous() : ZIL_NULLP(UI_WINDOW_OBJECT);
#endif
		}

		// Object does not overlap any regions.
		if (ccode == L_CONTINUE_SELECT || ccode == L_CONTINUE_ESCAPE)
			return ccode;

		if (ccode == L_BEGIN_SELECT || ccode == L_BEGIN_ESCAPE)
			Event(UI_EVENT(L_CANCEL));

		woStatus &= ~WOS_AUTO_MARK;
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		if (ccode == E_KEY && (FlagSet(event.key.shiftState, S_ALT) ||
			FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS)))
		{
			object = Current();
			if (object && (object->Inherited(ID_COMBO_BOX) && event.rawCode == 0xA000) ||  // Alt-Down
				(FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS) &&
				 (object->Inherited(ID_STRING) || object->Inherited(ID_TEXT)) &&
				 !FlagSet(object->woFlags, WOF_VIEW_ONLY)))
				return (object->Event(event));

			ZIL_UINT8 tHotKey;
			if (FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS) &&
				!FlagSet(event.key.shiftState, S_ALT))
				tHotKey = event.key.value;
			else
				tHotKey = UI_EVENT_MAP::MapEvent(hotKeyMapTable, event);
			tHotKey = ToLower(tHotKey);

			// Find a matching hotkey.
			if (!FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS) || FlagSet(woStatus, WOS_MINIMIZED) ||
				event.key.value == ' ')
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
				if (object->hotKey == HOT_KEY_SUB_WINDOW &&
					!FlagSet(woStatus, WOS_MINIMIZED) &&
					object->Event(event) != S_UNKNOWN)
				{
					if (object != current)
					{
						object->userObject = current;
						UIW_WINDOW::Add(object);
					}
					return (ccode);
				}
				else if (object->hotKey == tHotKey &&
					!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					(FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) ||
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
				{
					if (object->Inherited(ID_BUTTON) || object->Inherited(ID_ICON))
						ccode = object->Event(UI_EVENT(L_SELECT));
					return (ccode);
				}
			if (FlagSet(woStatus, WOS_MINIMIZED))
				return (S_UNKNOWN);
			else if (Inherited(ID_NOTEBOOK) && current)
				return Current()->Event(event);

			for (object = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? Last() : First();
				object; object = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? object->Previous() : object->Next())  // BUG 2971
				if (object->hotKey == HOT_KEY_SUB_WINDOW && object->Event(event) != S_UNKNOWN &&
					!FlagSet(object->woFlags, WOF_NON_SELECTABLE) && !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					UIW_WINDOW::Add(object);
					return (ccode);
				}
				else if (object->hotKey == tHotKey && !FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					if (searchID == ID_PULL_DOWN_MENU && current == object)
						current = ZIL_NULLP(UI_ELEMENT);
					if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
						UIW_WINDOW::Add(object);
					if (object->Inherited(ID_BUTTON) || object->Inherited(ID_ICON))
						ccode = object->Event(UI_EVENT(L_SELECT));
					return (ccode);
				}
			ccode = S_UNKNOWN;
		}
		else if (ccode == E_KEY && event.key.value == ' ' &&
			FlagSet(event.key.shiftState, S_CTRL))
		{
			if (!parent && Last() && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
				!FlagSet(woStatus, WOS_MINIMIZED))
				ccode = Last()->Event(event);
			else if (parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			{
				for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
					if (object->Inherited(ID_SYSTEM_BUTTON))
						return object->Event(UI_EVENT(L_SELECT));
			}
			else ccode = S_UNKNOWN;
		}
		else if (FlagSet(woStatus, WOS_MINIMIZED))
			ccode = S_UNKNOWN;
		else
		{
			object = (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent && ccode > 9999) ? Last() : Current();
			ccode = object ? object->Event(event) : S_UNKNOWN;
		}
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_WINDOW::RegionMax(UI_WINDOW_OBJECT *object)
{
	// Check to see if it is the top level object.
	if (object == this && (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
	{
		clip = trueRegion = relative;
		clipList.Destroy();
		return;
	}

	// Find the appropriate region element.
	UI_REGION_ELEMENT *element = clipList.First();
	UI_REGION region;
	if (element)						// Get first available region.
		region = element->region;
	else								// Determine the update region.
	{
		region = trueRegion;
		if (FlagSet(woFlags, WOF_BORDER) && (!display->isText || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
			region -= (display->isText ? 1 : ZIL_BORDER_WIDTH);
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
	}
	int bottomRegion = FlagsSet(object->woFlags, WOF_NON_FIELD_REGION | WOF_JUSTIFY_RIGHT);
	for ( ; element; element = element->Next())
		if (!bottomRegion && region.top > element->region.top)
			region = element->region;
		else if (bottomRegion && region.bottom < element->region.bottom)
			region = element->region;

	// Compute the actual field region.
	object->clip = object->trueRegion = region;
	if (object->clip.left < object->parent->clip.left)
		object->clip.left = object->parent->clip.left;
	if (object->clip.top < object->parent->clip.top)
		object->clip.top = object->parent->clip.top;
	if (object->clip.right > object->parent->clip.right)
		object->clip.right = object->parent->clip.right;
	if (object->clip.bottom > object->parent->clip.bottom)
		object->clip.bottom = object->parent->clip.bottom;
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
	UI_WINDOW_OBJECT *object;
	EVENT_TYPE ccode = event.type;
	switch(ccode)
	{

	case S_SCROLLRANGE:
		{
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;

		// Find minimum xy and maximum xy values.
		int minx = 32767, miny = 32767, maxx = 0, maxy = 0;
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
		{
			if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			{
				minx = MinValue(minx, object->relative.left);
				miny = MinValue(miny, object->relative.top);
				maxx = MaxValue(maxx, object->relative.right);
				maxy = MaxValue(maxy, object->relative.bottom);
			}
		}

		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			if (minx < 0)
				minx -= display->cellWidth;
			if (miny < 0)
				miny -= display->cellWidth;
			if (maxx > client.Width())
				maxx += display->cellWidth;
			if (maxy > client.Height())
				maxy += display->cellWidth;
		}

		hScrollInfo.minimum = MinValue(0, minx);
		vScrollInfo.minimum = MinValue(0, miny);
		hScrollInfo.showing = client.Width();
		vScrollInfo.showing = client.Height();

		// Adjust maximum values for MDI child window scrolling.
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			if (hScrollInfo.showing > maxx && hScrollInfo.current != hScrollInfo.minimum)
				maxx = hScrollInfo.showing;
			if (vScrollInfo.showing > maxy && vScrollInfo.current != vScrollInfo.minimum)
				maxy = vScrollInfo.showing;
		}

		scroll.right = scroll.left + MaxValue(0, minx) + (maxx - minx);
		scroll.bottom = scroll.top + MaxValue(0, miny) + (maxy - miny);

		if (vScrollInfo.current < vScrollInfo.minimum)
			vScrollInfo.current = vScrollInfo.minimum;
		if (hScrollInfo.current < hScrollInfo.minimum)
			hScrollInfo.current = hScrollInfo.minimum;

		// Calculate scroll maximum.
		if (scroll.Height() < client.Height())
			vScrollInfo.maximum = (vScrollInfo.current != vScrollInfo.minimum ?
			vScrollInfo.minimum + (vScrollInfo.current - vScrollInfo.minimum) : vScrollInfo.minimum);
		else if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			vScrollInfo.maximum = MaxValue(vScrollInfo.minimum, vScrollInfo.minimum +
				scroll.Height() - client.Height());
		else
			vScrollInfo.maximum = MaxValue(vScrollInfo.current - vScrollInfo.minimum, vScrollInfo.minimum +
				scroll.Height() - client.Height() + display->cellWidth);

		if (scroll.Width() < client.Width())
			hScrollInfo.maximum = (hScrollInfo.current != hScrollInfo.minimum ?
			hScrollInfo.minimum + (hScrollInfo.current - hScrollInfo.minimum) : hScrollInfo.minimum);
		else if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			hScrollInfo.maximum = MaxValue(hScrollInfo.minimum, hScrollInfo.minimum +
				scroll.Width() - client.Width());
		else
			hScrollInfo.maximum = MaxValue(hScrollInfo.current - hScrollInfo.minimum, hScrollInfo.minimum +
				scroll.Width() - client.Width() + display->cellWidth);

		if (vScrollInfo.current > vScrollInfo.maximum)
			vScrollInfo.current = vScrollInfo.maximum;
		if (hScrollInfo.current > hScrollInfo.maximum)
			hScrollInfo.current = hScrollInfo.maximum;

		event.type = S_VSCROLL_SET;
		event.scroll = vScrollInfo;
		ScrollEvent(event);
		event.type = S_HSCROLL_SET;
		event.scroll = hScrollInfo;
		ScrollEvent(event);
		}
		break;

	case S_VSCROLL_SET:
		vScrollInfo = event.scroll;
		if (vScroll)
			vScroll->Event(event);
		break;

	case S_HSCROLL_SET:
		hScrollInfo = event.scroll;
		if (hScroll)
			hScroll->Event(event);
		break;

	case S_VSCROLL:
	case S_HSCROLL:
		{
		int delta = event.scroll.delta;
		UI_SCROLL_INFORMATION sInfo = ccode == S_VSCROLL ? vScrollInfo : hScrollInfo;
		if ((sInfo.current == sInfo.minimum && delta <= 0) ||
			(sInfo.current == sInfo.maximum && delta >= 0) ||
			FlagSet(wnFlags, WNF_NO_SCROLL))
			return ccode;
		sInfo.current += delta;
		if (sInfo.current < sInfo.minimum)
		{
			delta -= (sInfo.current - sInfo.minimum);
			sInfo.current = sInfo.minimum;
		}
		else if (sInfo.current > sInfo.maximum)
		{
			delta -= (sInfo.current - sInfo.maximum);
			sInfo.current = sInfo.maximum;
		}

		// Update scroll information.
		event.scroll = sInfo;
		event.scroll.delta = delta;
		if (ccode == S_VSCROLL && delta)
		{
			vScrollInfo = sInfo;
			if (vScroll)
					vScroll->Event(event);
			event.type = S_VSCROLL_WINDOW;
			ScrollEvent(event);
		}
		else if (ccode == S_HSCROLL && delta)
		{
			hScrollInfo = sInfo;
			if (hScroll)
				hScroll->Event(event);
			event.type = S_HSCROLL_WINDOW;
			ScrollEvent(event);
		}
		}
		break;

	case S_VSCROLL_WINDOW:
	case S_HSCROLL_WINDOW:
		if (ccode == S_VSCROLL_WINDOW)
		{
 			if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
	 			for (object = First(); object; object = object->Next())
				{
					object->relative.top -= event.scroll.delta;
					object->relative.bottom -= event.scroll.delta;
				}
			else
			{
				scroll.top -= event.scroll.delta;
				scroll.bottom -= event.scroll.delta;
			}
		}
		else
		{
 			if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
	 			for (object = First(); object; object = object->Next())
				{
					object->relative.left -= event.scroll.delta;
					object->relative.right -= event.scroll.delta;
				}
			else
			{
				scroll.left -= event.scroll.delta;
				scroll.right -= event.scroll.delta;
			}
		}
 	 	for (object = First(); object; object = object->Next())
	 		object->Event(UI_EVENT(S_CHANGED));
	 	if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
	 		Event(UI_EVENT(S_REGION_DEFINE));
		{
	 	// Update the scrolled window.
	 	UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
		eventManager->DeviceState(E_CURSOR, D_OFF);
	 	Event(UI_EVENT(RedisplayType(), 0, client));
		}

		// See if scrollrange should be recomputed.
		if ((ccode == S_VSCROLL_WINDOW && (vScrollInfo.current == vScrollInfo.minimum ||
			(!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && vScrollInfo.current == vScrollInfo.maximum))) ||
			(ccode == S_HSCROLL_WINDOW && (hScrollInfo.current == hScrollInfo.minimum ||
			(!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && hScrollInfo.current == hScrollInfo.maximum))))
		{
			event.type = S_SCROLLRANGE;
			ScrollEvent(event);
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
	else if (UI_LIST::Index(object) != -1)
	{
		if (Inherited(ID_COMBO_BOX) && object == current)
			current = object->Next() ? object->Next() : object->Previous();

		UI_LIST::Subtract(object);
		if (object->screenID)
			object->Event(UI_EVENT(S_DEINITIALIZE));

		// Check for active menu object.
		if (current && Current()->userObject == object)
		{
			int mdiObject = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent;
			Current()->userObject = ZIL_NULLP(void);
			for (UI_WINDOW_OBJECT *tObject = mdiObject ? Last() : First();
				tObject; tObject = mdiObject ? tObject->Previous() : tObject->Next())
				if (!FlagSet(tObject->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(tObject->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					tObject->woStatus |= WOS_CURRENT;
					Current()->userObject = tObject;
					break;
				}
		}

		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
			FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
			current = last;
	}
	else if (support.Index(object) != -1)
	{
		support.Subtract(object);
		object->screenID = 0;
		if (object == current)	// Check for a current support object.
			current = First();
		if (object == vScroll)
			vScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
		else if (object == hScroll)
			hScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
		else if (object == icon)
			icon = ZIL_NULLP(UI_WINDOW_OBJECT);
	}
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
	// Destroy the list elements.
	UI_EVENT event(S_DEINITIALIZE);
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
		object->Event(event);
}

void UIW_WINDOW::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the window needs to be re-computed.
	if (objectID == ID_WINDOW && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}

void UIW_WINDOW::OSInitialize(void)
{
	vScroll = hScroll = icon = ZIL_NULLP(UI_WINDOW_OBJECT);
}
