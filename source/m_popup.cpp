//	Zinc Interface Library - M_POPUP.CPP
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


extern "C"
{
#include <Xm/RowColumn.h>
}

extern ZIL_ICHAR _tabString[];

// ----- UIW_POP_UP_MENU ----------------------------------------------------

const int ITEM_OFFSET = 2;

UI_WINDOW_OBJECT *UIW_POP_UP_MENU::Add(UI_WINDOW_OBJECT *object)
{
	// Add the menu-item searchID.
	if (object)
		object->WindowID(ID_MENU_ITEM);

	// Add the item to the menu.
	UIW_WINDOW::Add(object);

	// Create the menu if it does not exist; otherwise, recalculate the
	// position and size of the menu.
	if (!screenID && parent && parent->screenID)
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
		XtVaSetValues(parent->screenID, XmNsubMenuId, screenID, 
			ZIL_NULLP(void));
	}
	else if (screenID) 
		Event(UI_EVENT(S_CHANGED));

	return (object);
}

EVENT_TYPE UIW_POP_UP_MENU::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;
	int width = 0, height = 0, top = 0, tabs = FALSE;
	int leftofTab = 0, rightofTab = 0, maxWidth = 0;

	if (event.type == E_MOTIF)
		if (event.message.type == FocusIn)
			return(event.type);

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_POP_UP_MENU);
	switch (ccode)
	{
	case S_CREATE:
	case S_CHANGED:
		// Compute the pop-up item positions.
		for (object = First(); object; object = object->Next())
		{
			height += (object->relative.bottom - object->relative.top + 1);
			ZIL_ICHAR *string =
				(ZIL_ICHAR *)object->Information(I_GET_TEXT, ZIL_NULLP(void));
			ZIL_ICHAR *tabBuffer = ZIL_NULLP(ZIL_ICHAR);
			if (string)
			{
				// Look for tab characters and figure tabColumn;
				int hasTab = FALSE;
				if (strchr(string, '\t'))
				{
					tabs = hasTab = TRUE;
					tabBuffer = strdup(string);
					string = strtok(tabBuffer, _tabString);
				}
				int leftPortion = strlen(string) + ITEM_OFFSET;
				if (display->isText)
					leftPortion -= 2;
				if (leftPortion > leftofTab)
					leftofTab = leftPortion;
				if (hasTab)
				{
					string = strtok(0, _blankString);
					int rightPortion = strlen(string) + ITEM_OFFSET;
					if (rightPortion > rightofTab)
						rightofTab = rightPortion;
				}
				if (tabBuffer)
					delete(tabBuffer);
				tabColumn = leftofTab;
			}
			width = (leftofTab + rightofTab - ITEM_OFFSET) * display->cellWidth;
		}

		if (!parent)
			parent = windowManager;
		ccode = UI_WINDOW_OBJECT::Event(event);

		trueRegion.right = trueRegion.left + width - 1;
		trueRegion.bottom = trueRegion.top + height - 1;
		
		if (ccode == S_CHANGED)
		{
			if (screenID)
				XtVaSetValues(screenID, XmNwidth, trueRegion.Width(), XmNheight, trueRegion.Height(),
					ZIL_NULLP(void));
			break;	
		}
		// Fall thru.

	case S_REGISTER_OBJECT:
		{
		if (ccode != S_CREATE)
			ccode = UI_WINDOW_OBJECT::Event(event);

		// Create the menu.
		if (!screenID)
		{
			if (parent == windowManager)
				RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreatePopupMenu,
					ccode, FALSE, FALSE);
			else
				RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreatePulldownMenu,
					ccode, FALSE, FALSE, parent->parent->screenID);
		}

		for (object = First(); object; object = object->Next())
		{
			height = (object->relative.bottom - object->relative.top + 1);
			object->relative.left = 0;
			object->relative.top = top;
			object->relative.right = width - 1;
			top += height;
			object->relative.bottom = top - 1;
			object->Event(event);
		}

		Dimension w, h;
		XtVaGetValues(screenID, XmNwidth, &w, XmNheight, &h, ZIL_NULLP(void));
		trueRegion.right = trueRegion.left + w - 1;
		trueRegion.bottom = trueRegion.top + h - 1;
		}

		if (parent == windowManager)
			XtManageChild(screenID);
		break;

	case S_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		// Try to find the hotkey menu item.
		if (ccode == E_MOTIF && event.message.type == KeyRelease &&
			!FlagSet(event.key.shiftState, S_ALT))
		{
			for (object = First(); object; object = object->Next())
			{
				ZIL_ICHAR hotKey = object->HotKey();
				if (hotKey && ToLower(hotKey) == event.key.value &&
					!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				{
					return (object->Event(UI_EVENT(L_END_SELECT)));
				}
			}
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------
