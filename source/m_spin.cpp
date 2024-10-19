//	Zinc Interface Library - M_SPIN.CPP
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


#define CELL_RATIO	1 / 2

// ----- UIW_SPIN_CONTROL ------------------------------------------------------

EVENT_TYPE UIW_SPIN_CONTROL::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_SPIN_CONTROL);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		relative.bottom = relative.top + display->cellHeight;
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		if (!FlagSet(woFlags, WOF_VIEW_ONLY) &&
			event.type == E_KEY && event.key.value == ' ')
			return (fieldObject->Event(event));
		{
			int width = relative.right - relative.left;
			if (!FlagSet(woFlags, WOF_VIEW_ONLY))
				width -= display->cellWidth;
			fieldObject->Event(L_SELECT);
		}
		break;

	case L_FIRST:
	case L_LAST:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
			return (fieldObject->Event(event));
		// Continue to L_UP.
	case L_UP:
		fieldObject->Information(I_INCREMENT_VALUE, &delta);
		break;
	case L_DOWN:
		fieldObject->Information(I_DECREMENT_VALUE, &delta);
		break;
	case L_PGUP:
	case L_PGDN:
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		// Call user function.
		ccode = UI_WINDOW_OBJECT::Event(event);
		// Fall through.

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		current = fieldObject;
		UIW_WINDOW::Event(event);
		break;

	case S_ADD_OBJECT:
		if (fieldObject != event.windowObject)
		{
			Subtract(fieldObject);
			delete fieldObject;
			fieldObject = event.windowObject;
			Add(fieldObject);
		}
		break;

#if defined(ZIL_EDIT)
	case L_VIEW:
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_SPIN_CONTROL::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);
	if (object == upButton)
	{
		upButton->trueRegion.left = trueRegion.Width() - display->cellHeight * CELL_RATIO;
		upButton->trueRegion.top = 0;
		upButton->trueRegion.right = trueRegion.Width();
		upButton->trueRegion.bottom = (trueRegion.Height() - 1) / 2;
	}
	else if (object == downButton)
	{
		downButton->trueRegion.left = trueRegion.Width() - display->cellHeight * CELL_RATIO;
		downButton->trueRegion.top = (trueRegion.Height() - 1) / 2 + 1;
		downButton->trueRegion.right = trueRegion.Width();
		downButton->trueRegion.bottom = trueRegion.Height() - 2;
	}
	else if (object == fieldObject)
	{
		fieldObject->trueRegion.left = 0;
		fieldObject->trueRegion.top = 0;
		fieldObject->trueRegion.right = trueRegion.Width() - display->cellHeight * CELL_RATIO - 1;
		fieldObject->trueRegion.bottom = trueRegion.Height() - 2;
	}

}


UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::UIW_SPIN_BUTTON(EVENT_TYPE _direction) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_TOGGLE | BTF_STATIC_BITMAPARRAY | BTF_REPEAT,
		WOF_JUSTIFY_CENTER | WOF_BORDER), direction(_direction)
{
	woAdvancedFlags |= WOAF_NON_CURRENT;
	depth = 1;
	value = direction;
}						

UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::~UIW_SPIN_BUTTON(void)
{
	ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);
	text = ZIL_NULLP(ZIL_ICHAR);
	bitmapArray = ZIL_NULLP(ZIL_UINT8);
}

EVENT_TYPE UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UIW_BUTTON::DrawItem(event, ccode);
	return (TRUE);
}

EVENT_TYPE UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::Event(const UI_EVENT &event)
{
 	// Switch on the event type.
#if defined(ZIL_EDIT)
	if (FlagSet(parent->woStatus, WOS_EDIT_MODE) &&
        (event.message.type == ButtonPress ||
        event.message.type == ButtonRelease ||
        event.message.type == MotionNotify))
	{
		if (event.type == E_MOTIF)
			parent->Event(event);
	}
#endif

	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case L_SELECT:
	case L_BEGIN_SELECT:
		{
			UI_EVENT tEvent(S_ADD_OBJECT);
			tEvent.data = parent;
			parent->parent->Event(tEvent);
		}
		if (!FlagSet(parent->woStatus, WOS_EDIT_MODE))
			parent->Event(UI_EVENT(direction));
		// Fall thru.
	case L_VIEW:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		if (!FlagSet(parent->woStatus, WOS_EDIT_MODE))
			ccode = UIW_BUTTON::Event(event);
		break;

	case S_CHANGED:
	case S_CREATE:
		ccode = UIW_BUTTON::Event(event);
		XtVaSetValues(screenID, XmNhighlightThickness, (Dimension)0, ZIL_NULLP(void));
		XtVaSetValues(screenID, XmNlabelInsensitivePixmap, colorBitmap, ZIL_NULLP(void));
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}
