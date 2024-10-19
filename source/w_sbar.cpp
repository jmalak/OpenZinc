//	W_SBAR.CPP (STATUSBAR) - This file contains the status bar class.
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


#include <ui_win.hpp>

EVENT_TYPE UIW_STATUS_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);

	lastPalette = LogicalPalette(ccode, ID_STATUS_BAR);

	display->Rectangle(screenID, trueRegion, lastPalette, 0, TRUE, FALSE);
	if (FlagSet(woFlags, WOF_BORDER))
		display->Line(screenID, trueRegion.left, trueRegion.top, trueRegion.right,
			trueRegion.top,	lastPalette, 1, FALSE, &clip);
	woStatus &= ~WOS_REDISPLAY;

	// Update the display.
	display->VirtualPut(screenID);
	return (TRUE);
}


static int _statusbarOffset = -1;
static FARPROC _statusbarCallback = (FARPROC)DefWindowProc;
LRESULT FAR PASCAL _export StatusbarJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
	UIW_STATUS_BAR *object = (UIW_STATUS_BAR *)GetWindowLong(hWnd, _statusbarOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _statusbarJumpInstance = (FARPROC)StatusbarJumpProcedure;

EVENT_TYPE UIW_STATUS_BAR::Event(const UI_EVENT &event)
{
	const int STATUS_OFFSET = 4;

	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STATUS_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		break;

	case S_CHANGED:
		if (!screenID)
			break;
		// Fall Through to S_CREATE.
	case S_CREATE:
		{
		UI_WINDOW_OBJECT::Event(event);
		clipList.Destroy();
  		// Increase size so don't get two-pixel border on left, right, bottom.
		if (FlagSet(woFlags, WOF_BORDER))
		{
			trueRegion.left--;
			trueRegion.right++;
			trueRegion.bottom++;
		}

		// Set top of status bar to be the programmer-specified height
		// plus a few pixels above and below.
		trueRegion.top = trueRegion.bottom - height * display->cellHeight /
			(FlagSet(woFlags, WOF_MINICELL) ? display->miniDenominatorY : 1) -
			2 * STATUS_OFFSET;

		UI_REGION region;
		region.left = display->cellWidth;
		region.right = trueRegion.right - (display->cellWidth - 1);
		region.top = STATUS_OFFSET;
		region.bottom = trueRegion.Height() - STATUS_OFFSET;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));

		if (ccode == S_CREATE)
		{
			RegisterObject("UIW_STATUS_BAR", ZIL_NULLP(char), &_statusbarOffset,
				&_statusbarJumpInstance, &_statusbarCallback);
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			{
				object->woFlags |= WOF_VIEW_ONLY;
				object->woAdvancedFlags |= WOAF_NON_CURRENT;
				object->Event(event);
			}
			for (object = First(); object; object = object->Next())
			{
				object->woFlags |= WOF_VIEW_ONLY;
				object->woAdvancedFlags |= WOAF_NON_CURRENT;
				object->Event(event);
			}
		}
		else
		{
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			{
				UI_REGION region = object->trueRegion;
				object->Event(event);

				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
				if (object->trueRegion != region && FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
				{
					ZIL_SCREENID objectID;
					object->Information(I_GET_FRAMEID, &objectID);
					MoveWindow(objectID, object->trueRegion.left, object->trueRegion.top,
						object->trueRegion.Width(), object->trueRegion.Height(), FALSE);

					// Invalidate the old and new object regions.
					woStatus |= WOS_REDISPLAY;
					RECT winRegion;
					winRegion.left = region.left;
					winRegion.top = region.top;
					winRegion.right = region.right + 1;
					winRegion.bottom = region.bottom + 1;
					InvalidateRect(screenID, &winRegion, TRUE);
					InvalidateRect(objectID, ZIL_NULLP(RECT), TRUE);
				}
			}
			for (object = First(); object; object = object->Next())
			{
				UI_REGION region = object->trueRegion;
				object->Event(event);

				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
				if (object->trueRegion != region && FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
				{
					ZIL_SCREENID objectID;
					object->Information(I_GET_FRAMEID, &objectID);
					MoveWindow(objectID, object->trueRegion.left, object->trueRegion.top,
						object->trueRegion.Width(), object->trueRegion.Height(), FALSE);

					// Invalidate the old and new object regions.
					woStatus |= WOS_REDISPLAY;
					RECT winRegion;
					winRegion.left = region.left;
					winRegion.top = region.top;
					winRegion.right = region.right + 1;
					winRegion.bottom = region.bottom + 1;
					InvalidateRect(screenID, &winRegion, TRUE);
					InvalidateRect(objectID, ZIL_NULLP(RECT), TRUE);
				}
			}
		}
		}
		break;

	case S_REGISTER_OBJECT:
   		RegisterObject("UIW_STATUS_BAR", ZIL_NULLP(char), &_statusbarOffset,
			&_statusbarJumpInstance, &_statusbarCallback);
		break;

	case S_SIZE:
		height = event.region.Height() / display->cellHeight;
		parent->Event(UI_EVENT(S_CHANGED));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_STATUS_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_STATUS_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
		parent->Event(UI_EVENT(S_CHANGED));
		Event(UI_EVENT(S_REDISPLAY));
	}
}

