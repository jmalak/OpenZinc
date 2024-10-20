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

static WNDPROC _statusbarCallback = ZIL_NULLF(WNDPROC);
static ZIL_UINT8 _sizeGripBitmap[] =
{
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x07,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x07,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07, 0x07,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x07,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x07,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07,
	0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07, 0x07,
	0xFF, 0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x07,
	0xFF, 0xFF, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x07,
	0xFF, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07,
	0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07, 0x0F, 0x08, 0x08, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
};

LRESULT CALLBACK SizeGripCallback(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wMsg)
	{
	case WM_PAINT:
		{
		UI_DISPLAY *display = UI_WINDOW_OBJECT::display;
		UI_MSWINDOWS_DISPLAY::hDC = BeginPaint(hWnd, &UI_MSWINDOWS_DISPLAY::paintStruct);
		display->VirtualGet(ID_DIRECT, 0, 0, 12, 12);
		display->Bitmap(hWnd, 0, 0, 13, 13, _sizeGripBitmap);
		display->VirtualPut(ID_DIRECT);
		EndPaint(hWnd, &UI_MSWINDOWS_DISPLAY::paintStruct);
		}
		return (0);

	case WM_LBUTTONDOWN:
		SendMessage(GetParent(GetParent(hWnd)), WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, 0);
		return (0);

	default:
		return (DefWindowProc(hWnd, wMsg, wParam, lParam));
	}
}

EVENT_TYPE UIW_STATUS_BAR::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event;

	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);
	lastPalette = LogicalPalette(ccode, ID_STATUS_BAR);
	display->Rectangle(screenID, trueRegion, lastPalette, 0, TRUE, FALSE);
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_STATUS_BAR::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STATUS_BAR);

	switch (ccode)
	{
	case S_CHANGED:
		if (!screenID)
			break;
		// Fall Through to S_CREATE.
	case S_CREATE:
		{
		UI_WINDOW_OBJECT::Event(event);
		clipList.Destroy();

		// Set top of status bar to be the programmer-specified height
		// plus a few pixels above and below.
		int _height = height;
		if (!FlagSet(woFlags, WOF_PIXEL))
		{
			_height *= display->cellHeight;
			if (FlagSet(woFlags, WOF_MINICELL))
				_height = height * display->miniNumeratorY / display->miniDenominatorY;
			else
				_height -= display->postSpace;
		}
		trueRegion.top = trueRegion.bottom - _height + 1;

		UI_REGION region;
		region.left = 0;
		region.right = trueRegion.Width() - 1;
		region.top = 0;
		region.bottom = trueRegion.Height() - 1;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));

		if (ccode == S_CREATE)
		{
			RegisterObject("UIW_STATUS_BAR", ZIL_NULLP(char), &_statusbarCallback);
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

			// Register the size grip class.
			WNDCLASS sizeGripClass;
#if defined(ZIL_UNICODE)
			ZIL_ICHAR *sizeGripClassName = L"SIZE_GRIP";
#else
			ZIL_ICHAR *sizeGripClassName = "SIZE_GRIP";
#endif
			if (!GetClassInfo(display->hInstance, sizeGripClassName, &sizeGripClass))
			{
				sizeGripClass.style = 0;
				sizeGripClass.lpfnWndProc = SizeGripCallback;
				sizeGripClass.cbClsExtra = 0;
				sizeGripClass.cbWndExtra = 0;
				sizeGripClass.hInstance = display->hInstance;
				sizeGripClass.hIcon = 0;
				sizeGripClass.hCursor = LoadCursor(0, IDC_SIZENWSE);
				sizeGripClass.hbrBackground = 0;
				sizeGripClass.lpszMenuName = ZIL_NULLP(ZIL_ICHAR);
				sizeGripClass.lpszClassName = sizeGripClassName;
				RegisterClass(&sizeGripClass);
			}

			RECT rect;
			GetClientRect(screenID, &rect);
			sizeGrip = CreateWindow(sizeGripClassName, ZIL_NULLP(ZIL_ICHAR),
				WS_CHILD | WS_VISIBLE, rect.right - 13, rect.bottom - 13,
				13, 13, screenID, 0, display->hInstance, ZIL_NULLP(void));
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

			RECT rect;
			GetClientRect(screenID, &rect);
			MoveWindow(sizeGrip, rect.right - 13, rect.bottom - 13, 13, 13, FALSE);
		}
		}
		break;

	case S_REGISTER_OBJECT:
		RegisterObject("UIW_STATUS_BAR", ZIL_NULLP(char), &_statusbarCallback);
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

void UIW_STATUS_BAR::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);
	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION) &&
		object->trueRegion.top == 0)
			object->trueRegion.top += display->preSpace;
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

