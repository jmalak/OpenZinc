//	Zinc Interface Library - W_PROMPT.CPP
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

// ----- UIW_PROMPT ---------------------------------------------------------

#if defined(ZIL_MSWINDOWS_CTL3D)
extern "C" LRESULT CALLBACK StaticWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

static WNDPROC _promptCallback = ZIL_NULLF(WNDPROC);

EVENT_TYPE UIW_PROMPT::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	RECT r;
	GetWindowRect(screenID, &r);

	/* unused parameters */ (void)event;

	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);

	// Draw the prompt information.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);

	UI_WINDOW_OBJECT *bObject = NULL;
	for (bObject = this; bObject->parent &&
		(bObject->LogicalPalette(S_DISPLAY_ACTIVE)->colorBackground == BACKGROUND ||
		bObject->LogicalPalette(S_DISPLAY_ACTIVE)->fillPattern == PTN_RGB_COLOR);
		bObject = bObject->parent)
		;
	UI_PALETTE *bPalette = bObject->LogicalPalette(ccode);
	
	display->Rectangle(screenID, region, bPalette, 0, TRUE);

	lastPalette = LogicalPalette(ccode, ID_PROMPT);
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	if (parent->Inherited(ID_LIST))
	{
		// Draw the focus Rect.
		UI_WINDOW_OBJECT *object = NULL;
		for (object = this;
			object && FlagSet(object->woStatus, WOS_CURRENT); object = object->parent)
			;
		if (!object)
			DrawFocus(screenID, trueRegion, ccode);
	}

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_PROMPT::Event(const UI_EVENT &event)
{
	int processed = FALSE;
	EVENT_TYPE ccode = S_UNKNOWN;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;

		processed = TRUE;

		switch (message)
		{
		case WM_PAINT:
			{
			lastPalette = LogicalPalette(ccode, searchID);
			RECT winRegion;
			if (GetUpdateRect(screenID, &winRegion, FALSE) ||
				!FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			{
				UI_EVENT tEvent = event;
				tEvent.region.left = winRegion.left + trueRegion.left;
				tEvent.region.top = winRegion.top + trueRegion.top;
				tEvent.region.right = winRegion.right + 1 + trueRegion.left;
				tEvent.region.bottom = winRegion.bottom + 1 + trueRegion.top;
				lastPalette = LogicalPalette(ccode);

				UI_MSWINDOWS_DISPLAY::hDC = BeginPaint(screenID, &UI_MSWINDOWS_DISPLAY::paintStruct);
				display->VirtualGet(ID_DIRECT, trueRegion.left, trueRegion.top,
					trueRegion.right, trueRegion.bottom);
				ccode = DrawItem(tEvent, S_DISPLAY_ACTIVE);
				display->VirtualPut(ID_DIRECT);
			 	EndPaint(screenID, &UI_MSWINDOWS_DISPLAY::paintStruct);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			}
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = event.type;
		switch (ccode)
		{
		case S_INITIALIZE:
			UI_WINDOW_OBJECT::Event(event);
			if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))
			{
				relative.right = relative.left + display->TextWidth(text, screenID, font) + 1;
				if (FlagSet(woFlags, WOF_BORDER))
					relative.right += display->cellWidth;
			}
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
			if (hotKey && Next())
				Next()->HotKey(hotKey);
			break;

		case S_CHANGED:
		case S_CREATE:
			if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))
			{
				relative.right = relative.left + display->TextWidth(text, screenID, font) + 1;
				if (FlagSet(woFlags, WOF_BORDER))
					relative.right += display->cellWidth;
			}
			UI_WINDOW_OBJECT::Event(event);
			dwStyle &= ~WS_BORDER;
			// Fall through to S_REGISTER_OBJECT.

		case S_REGISTER_OBJECT:
			RegisterObject("UIW_PROMPT", "STATIC", &_promptCallback, text ? text : _blankString);
			if (ccode == S_CHANGED && FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			{
				RECT winRegion;
				winRegion.left = trueRegion.left;
				winRegion.top = trueRegion.top;
				winRegion.right = trueRegion.right + 1;
				winRegion.bottom = trueRegion.bottom + 1;
				RECT rect;
				GetWindowRect(screenID, &rect);
				POINT p = { rect.left, rect.top };
				ScreenToClient(parent->screenID, &p);
				rect.right -= rect.left - p.x;
				rect.left = p.x;
				rect.bottom -= rect.top - p.y;
				rect.top = p.y;

				if (rect.left != winRegion.left || rect.top != winRegion.top ||
					rect.right != winRegion.right)
				{
					InvalidateRect(parent->screenID, &winRegion, TRUE);
					MoveWindow(screenID, trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(), FALSE);
				}
			}
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PROMPT::OSDataSet(void)
{
	// Redisplay the prompt.
	if (screenID)
	{
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
		{
			InvalidateRect(screenID, ZIL_NULLP(RECT), TRUE);
			UpdateWindow(screenID);
		}
		Event(UI_EVENT(S_CHANGED));
		Event(UI_EVENT(S_REDISPLAY));
	}
}

void UIW_PROMPT::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-displayed.
	if (objectID == ID_PROMPT && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

