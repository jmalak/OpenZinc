//	Zinc Interface Library - D_SPIN.CPP
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

		if (!display->isText)
		{
#if !defined(ZIL_TEXT_ONLY)
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace;
#endif
		}
		else
			relative.bottom = relative.top;
		break;

	case S_CHANGED:
	case S_CREATE:
		UIW_WINDOW::Event(event);
		upButton->trueRegion = trueRegion;
		upButton->trueRegion.left = display->isText ? trueRegion.right :
#if defined(ZIL_OS2_STYLE)
			trueRegion.right - 18;
#else
			trueRegion.right - display->cellHeight * CELL_RATIO;
#endif
		if (!display->isText)
		{
#if !defined(ZIL_TEXT_ONLY)
			upButton->trueRegion.bottom -= upButton->trueRegion.Height() / 2;
#endif
		}
		
		downButton->trueRegion = trueRegion;
		downButton->trueRegion.left = display->isText ? trueRegion.right - 1 :
#if defined(ZIL_OS2_STYLE)
			trueRegion.right - 18;
#else
			trueRegion.right - display->cellHeight * CELL_RATIO;
#endif
		if (display->isText)
			downButton->trueRegion.right = trueRegion.right - 1;

		if (!display->isText)
		{
#if !defined(ZIL_TEXT_ONLY)
			downButton->trueRegion.top += downButton->trueRegion.Height() / 2;
#endif
		}

		fieldObject->trueRegion = trueRegion;
		if (!display->isText)
		{
#if !defined(ZIL_TEXT_ONLY)
#	if defined(ZIL_OS2_STYLE)
		fieldObject->trueRegion.right = upButton->trueRegion.left - display->cellWidth;
#	else
		fieldObject->trueRegion.right = upButton->trueRegion.left;
#	endif
#endif
		}
		else
			fieldObject->trueRegion.right = trueRegion.right - 2;
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

	case S_RESET_DISPLAY:
		UIW_WINDOW::Event(event);
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_VIEW:
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		{
		UIW_BUTTON *selected = ZIL_NULLP(UIW_BUTTON);
		if (upButton->trueRegion.Overlap(event.position))
			selected = upButton;
		else if (downButton->trueRegion.Overlap(event.position))
			selected = downButton;

		if (selected && ccode != L_VIEW)
		{
			if (ccode != L_BEGIN_SELECT)
				break;
			selected->Event(event);
			UI_EVENT tEvent = event;
			ZIL_TIME currentTime, lastTime;
			lastTime+= delayLength;

			while (ccode != L_END_SELECT)
			{
				currentTime.Import();
				long elapsedTime = currentTime - lastTime;
				if (eventManager->Get(tEvent, Q_NO_BLOCK) != -2)
				{		
					ccode = LogicalEvent(tEvent, ID_SPIN_CONTROL);
					// Send all user messages to the window manager.
					if (tEvent.type > 9999)
						windowManager->Event(tEvent);
				}
				else if (tEvent.type == E_MOUSE)
				{
					// Redraw button unpressed when mouse doesn't overlap.
					if (selected && !selected->trueRegion.Overlap(tEvent.position))
					{
						selected->UIW_BUTTON::Event(tEvent);
						selected = ZIL_NULLP(UIW_BUTTON);
					}
					// Send spin button events repeatedly.
					if (elapsedTime > repeatRate && upButton->trueRegion.Overlap(tEvent.position))
					{
						if (selected == downButton)
						{
							UI_EVENT uEvent = tEvent;
							uEvent.position.line = upButton->trueRegion.top;
							selected->Event(uEvent);
						}
						selected = upButton;
						selected->Event(tEvent);
						lastTime = currentTime;
					}
					else if (elapsedTime > repeatRate && downButton->trueRegion.Overlap(tEvent.position))
					{
						if (selected == upButton)
						{
							UI_EVENT uEvent = tEvent;
							uEvent.position.line = downButton->trueRegion.bottom;
							selected->Event(uEvent);
						}
						selected = downButton;
						selected->Event(tEvent);
						lastTime = currentTime;
					}
				}
			}
			if (FlagSet(upButton->btStatus, BTS_DEPRESSED))
			{
				upButton->btStatus &= ~BTS_DEPRESSED;
				upButton->Event(UI_EVENT(S_REDISPLAY));
			}
			if (FlagSet(downButton->btStatus, BTS_DEPRESSED))
			{
				downButton->btStatus &= ~BTS_DEPRESSED;
				downButton->Event(UI_EVENT(S_REDISPLAY));
			}
		}
		else
			ccode = UIW_WINDOW::Event(event);
		}
		break;

	case L_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));

		ccode = UserFunction(event, L_SELECT);
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
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		current = fieldObject;
		UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::UIW_SPIN_BUTTON(EVENT_TYPE _direction) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_TOGGLE | BTF_STATIC_BITMAPARRAY,
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
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
			parent->Event(UI_EVENT(direction));
			UIW_BUTTON::Event(event);
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}
