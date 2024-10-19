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

#define CELL_RATIO(w) (w / 2 + 1)

// ----- UIW_SPIN_CONTROL ------------------------------------------------------

EVENT_TYPE UIW_SPIN_CONTROL::Event(const UI_EVENT &event)
{

	EVENT_TYPE ccode = S_UNKNOWN;

	//Process messages from system first.
	int processed = FALSE;
	if (event.type == E_MSWINDOWS)
	{
		processed = TRUE;

		// Switch on the windows message.
		UINT message = event.message.message;
		ccode = TRUE;
		switch (message)
		{
#if defined(ZIL_EDIT)
		case WM_LBUTTONDOWN:
			if (FlagSet(woStatus, WOS_EDIT_MODE))
				UI_WINDOW_OBJECT::Event(event);
			break;
#endif

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = LogicalEvent(event, ID_SPIN_CONTROL);
		switch (ccode)
		{
		case S_INITIALIZE:
			UIW_WINDOW::Event(event);
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
#if defined(ZIL_MSWINDOWS_CTL3D)
			relative.bottom += 2;
#endif
			break;
	
		case S_CHANGED:
		case S_CREATE:
			{
			UI_WINDOW_OBJECT::Event(event);
			UI_REGION region = { trueRegion.Width() - CELL_RATIO(display->cellHeight) -
				((CELL_RATIO(display->cellHeight) - 1) % 2 ? 1 : 0), 0,
				trueRegion.Width() - 1, trueRegion.Height() - 1 };
#if defined(ZIL_MSWINDOWS_CTL3D)
			region.top++;
			region.bottom--;
#endif
			spinButton->Event(UI_EVENT(S_SIZE, 0, region));
			region.right = region.left;
#if defined(ZIL_MSWINDOWS_CTL3D)
			region.right -= 2;
#endif
			region.left = 0;
			fieldObject->Event(UI_EVENT(S_SIZE, 0, region));
			UIW_WINDOW::Event(event);
			}
			break;
	
		case S_CURRENT:
		case S_NON_CURRENT:
		case S_DISPLAY_ACTIVE:
		case S_DISPLAY_INACTIVE:
			current = fieldObject;
			if (ccode == S_CURRENT)
				SetFocus(fieldObject->screenID);
			UIW_WINDOW::Event(event);
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
	
		case S_ADD_OBJECT:
		    if (fieldObject != event.windowObject)
		    {
			    Subtract(fieldObject);
			    delete fieldObject;
			    fieldObject = event.windowObject;
			    Add(fieldObject);
		    }
			break;

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::UIW_SPIN_BUTTON() :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_TOGGLE, WOF_BORDER)
{
	direction = 0;
	woAdvancedFlags |= WOAF_NON_CURRENT;
	woStatus |= WOS_OWNERDRAW;
}						

UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::~UIW_SPIN_BUTTON(void)
{
	text = ZIL_NULLP(ZIL_ICHAR);
	bitmapArray = ZIL_NULLP(ZIL_UINT8);
}

EVENT_TYPE UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif
	UI_REGION region = trueRegion;

	// Virtualize the display.
	display->VirtualGet(screenID, region);
	lastPalette = LogicalPalette(ccode, windowID[0]);

	// Draw the button faces.
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
#if defined(ZIL_MSWINDOWS_CTL3D)
	DrawShadow(screenID, region, 1, FALSE, ccode);
#else
	display->Rectangle(screenID, region, outline, 1, FALSE);
	--region;
#endif
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	int middle = region.top + region.Height() / 2;
	display->Line(screenID,  region.left, middle,
		region.right, middle, outline);

	// Draw the arrows.
	int arrowHeight = region.bottom - middle - 3;
	int upPoints[10] = {	region.left + 2, region.top + 1 + arrowHeight,
							region.left + region.Width() / 2, region.top + 1,
							region.right - region.Width() / 2, region.top + 1,
							region.right - 2, region.top + 1 + arrowHeight,
							region.left + 2, region.top + 1 + arrowHeight		};
	int downPoints[10] = {	region.left + 2, region.bottom - 1 - arrowHeight,
							region.left + region.Width() / 2, region.bottom - 1,
							region.right - region.Width() / 2, region.bottom - 1,
							region.right - 2, region.bottom - 1 - arrowHeight,
							region.left + 2, region.bottom - 1 - arrowHeight	};
	display->Polygon(screenID, 5, upPoints, outline, TRUE);
	display->Polygon(screenID, 5, downPoints, outline, TRUE);

	// Highlight selected area.
	if (direction == L_UP)
		region.bottom = middle;
	else if (direction == L_DOWN)
		region.top = middle + 2;
	if (direction)
	{
		region.right--;
		region.bottom--;
		UI_PALETTE *highlight = LogicalPalette(ccode, ID_WHITE_SHADOW);
		display->Rectangle(screenID, region, highlight, 0, TRUE, TRUE);
	}

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = S_UNKNOWN;
	int processed = FALSE;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;
		LPARAM lParam = event.message.lParam;

		processed = TRUE;

		switch (message)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			{
#if defined(ZIL_EDIT)
			if (FlagSet(parent->woStatus, WOS_EDIT_MODE))
			{
				POINT p = MAKEPOINT(lParam);
				ClientToScreen(screenID, &p);
				ZIL_SCREENID pFrameID;
				parent->Information(I_GET_FRAMEID, &pFrameID);
				ScreenToClient(pFrameID, &p);
				LPARAM tLParam = ((long)p.y << 16) | p.x;
				parent->Event(UI_EVENT(E_MSWINDOWS, pFrameID,
					WM_LBUTTONDOWN, 0, tLParam));
				break;
			}
#endif
			if (!parent->IsCurrent())
				parent->Event(UI_EVENT(S_CURRENT));
			
			UI_EVENT tEvent = event;
			tEvent.region = trueRegion;

			ZIL_TIME currentTime, lastTime;
			lastTime+= delayLength;
			lastPalette = LogicalPalette(ccode);
			while (message != WM_LBUTTONUP)
			{
				if (!tEvent.message.hwnd)
					;
				else if (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK ||
					(message == WM_MOUSEMOVE && tEvent.message.hwnd == screenID))
				{
					int middle = trueRegion.top + trueRegion.Height() / 2;
					if (tEvent.position.line == middle && direction)
					{
						direction = 0;
						DrawItem(tEvent, direction);
					}
					else if (tEvent.position.line < middle && direction != L_UP)
					{
						direction = L_UP;
						DrawItem(tEvent, direction);
					}
					else if (tEvent.position.line > middle && direction != L_DOWN)
					{
						direction = L_DOWN;
						DrawItem(tEvent, direction);
					}
					SendMessage(screenID, BM_SETSTATE, 1, 0);
				}
				else if (message == WM_MOUSEMOVE && tEvent.message.hwnd != screenID)
				{
					direction = 0;
					SendMessage(screenID, BM_SETSTATE, 0, 0);
				}
				else if (tEvent.message.hwnd == screenID)
					ccode = UI_WINDOW_OBJECT::Event(tEvent);
				else
					SendMessage(tEvent.message.hwnd, message,
						tEvent.message.wParam, tEvent.message.lParam);

				currentTime.Import();
				long elapsedTime = currentTime - lastTime;
				if (eventManager->Get(tEvent, Q_NO_BLOCK) != -2)
				{
					// Send all user messages to the window manager.
					if (tEvent.type > 9999)
						windowManager->Event(tEvent);
					if (tEvent.type == E_MSWINDOWS)
						message = tEvent.message.message;
				}
				else if (FlagSet(btStatus, BTS_DEPRESSED) &&
					elapsedTime > repeatRate)
				{
					if (direction)
						parent->Event(UI_EVENT(direction));
					lastTime = currentTime;
				}
			}
			if (direction)
				parent->Event(UI_EVENT(direction));
			direction = 0;
			UI_WINDOW_OBJECT::Event(tEvent);
			SendMessage(screenID, BM_SETSTATE, 0, 0);
			}
			break;

		default:
			processed = FALSE;
			break;
		}
	}

	if (!processed)
		ccode = UIW_BUTTON::Event(event);

	// Return the control code.
	return (ccode);
}

