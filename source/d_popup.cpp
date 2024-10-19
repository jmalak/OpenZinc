//	Zinc Interface Library - D_POPUP.CPP
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

extern ZIL_ICHAR _tabString[];

// ----- UIW_POP_UP_MENU ----------------------------------------------------

const int ITEM_OFFSET = 4;

UI_WINDOW_OBJECT *UIW_POP_UP_MENU::Add(UI_WINDOW_OBJECT *object)
{
	// Add the menu-item searchID.
	if (object)
		object->WindowID(ID_MENU_ITEM);
	// Add the item to the menu.
	return (UIW_WINDOW::Add(object));
}

EVENT_TYPE UIW_POP_UP_MENU::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_POP_UP_MENU);
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
		{
		// Compute the pop-up item positions.
		int width = 0, height = 0, top = 0, tabs = FALSE;
		int leftofTab = 0, rightofTab = 0;
		UI_WINDOW_OBJECT *tempParent = 0;
		if (parent && parent->Inherited(ID_SYSTEM_BUTTON) &&
			parent->parent && FlagSet(parent->parent->woStatus, WOS_MINIMIZED))
		{
			tempParent = parent;
			parent = parent->parent;
		}
		if (parent && parent->Inherited(ID_POP_UP_ITEM))
		{
			relative.left = parent->trueRegion.right + 1;
			relative.top = MaxValue(0, parent->trueRegion.top - 1);
		}
		else if (parent)
		{
			relative.left = MaxValue(0, parent->trueRegion.left);
			relative.top = parent->trueRegion.bottom + 1;
		}
		ZIL_ICHAR tabBuffer[256];
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
			int tWidth = object->relative.right - object->relative.left + 1;
			if (tWidth > width)
				width = tWidth;
			height += (object->relative.bottom - object->relative.top + 1);
			ZIL_ICHAR *string = (ZIL_ICHAR *)object->Information(I_GET_TEXT, ZIL_NULLP(void));
			if (string)
			{
				// Look for tab characters and figure tabColumn;
				int hasTab = FALSE;
				if (strchr(string, '\t'))
				{
					tabs = hasTab = TRUE;
					strcpy(tabBuffer, string);
					string = strtok(tabBuffer, _tabString);
				}
				int leftPortion = ITEM_OFFSET + display->TextWidth(string,
					ID_SCREEN, font) + display->cellWidth;
#if !defined(ZIL_GRAPHICS_ONLY)
				if (display->isText)
					leftPortion -= 2;
#endif
				if (leftPortion > leftofTab)
					leftofTab = leftPortion;
				if (hasTab)
				{
					string = strtok(0, _blankString);
					int rightPortion = display->TextWidth(string, ID_SCREEN, font)
						+ (ITEM_OFFSET + 1) * display->cellWidth;
#if !defined(ZIL_GRAPHICS_ONLY)
					if (display->isText)
						rightPortion--;
#endif
					if (rightPortion > rightofTab)
						rightofTab = rightPortion;
				}
				tabColumn = leftofTab;
			}
		}
		if (tabs)
			width = leftofTab + rightofTab;
		if (FlagSet(woFlags, WOF_BORDER))
		{
			width += (display->isText ? 2 : 2 * ZIL_BORDER_WIDTH);
			height += (display->isText ? 1 : ZIL_BORDER_WIDTH);
		}

		// Check for menus off the screen.
		int maxX = display->columns - 1;
		int maxY = display->lines - 1;
		if (parent)
		{
			if (!parent->Inherited(ID_POP_UP_ITEM) &&
				parent->trueRegion.left + width >= maxX)
				relative.left = MaxValue(0, maxX - width);

			if (parent->trueRegion.bottom + height >= maxY)
			{
				if (parent->Inherited(ID_POP_UP_ITEM))
					relative.top = MaxValue(0, maxY - height);
				else
					relative.top = (height <= parent->trueRegion.top) ?
						parent->trueRegion.top - height - 1 : MaxValue(0, maxY - height);
			}
			// One final check to make sure we don't cover the parent item.
			if ((relative.top < parent->trueRegion.top && relative.top + height > parent->trueRegion.top) ||
				(relative.top < parent->trueRegion.bottom && relative.top + height > parent->trueRegion.bottom) ||
				(relative.left + width > maxX))
				relative.left = (parent->trueRegion.right + width) > maxX ?
					MaxValue(0, parent->trueRegion.left - width) : MaxValue(0, parent->trueRegion.right);
			relative.left = MaxValue(0, relative.left);
			relative.top = MaxValue(0, relative.top);
		}
		relative.right = relative.left + width;
		relative.bottom = relative.top + height;

		for (object = First(); object; object = object->Next())
		{
			height = (object->relative.bottom - object->relative.top + 1);
			object->relative.left = 0;
			object->relative.top = top;
			object->relative.right = width - 1;
			top += height;
			object->relative.bottom = top - 1;
		}
		if (tempParent)
			parent = tempParent;
		UIW_WINDOW::Event(event);
		}
		break;

	case L_UP:
	case L_DOWN:
		ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_UP) ? L_PREVIOUS : L_NEXT));
		break;

	case L_LEFT:
	case L_RIGHT:
		if (parent && parent->Inherited(ID_PULL_DOWN_ITEM))
		{
			UI_WINDOW_OBJECT *menu = parent->parent;
			UI_WINDOW_OBJECT *menuCurrent;

			windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
	   	menu->Event(event);
			menu->Information(I_GET_CURRENT, &menuCurrent);
			if (menuCurrent)
				menuCurrent->Event(UI_EVENT(L_DOWN));
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_END_SELECT:
		if (!trueRegion.Overlap(event.position) &&
			parent && parent->trueRegion.Overlap(event.position))
			break;
		ccode = UIW_WINDOW::Event(event);
		break;

#if defined(ZIL_EDIT)
	case L_VIEW:
	case L_BEGIN_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		// Continue to default.
#endif
	default:
		// Try to find the hotkey menu item.
		if (ccode == E_KEY && !FlagSet(event.key.shiftState, S_ALT))
		{
			for (object = First(); object; object = object->Next())
				if (object->HotKey() == ToLower(event.key.value) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				{
					UIW_WINDOW::Add(object);
					return (object->Event(UI_EVENT(L_SELECT)));
				}
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}
