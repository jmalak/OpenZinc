//	Zinc Interface Library - W_GROUP.CPP
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

// ----- UIW_GROUP ---------------------------------------------------------

#if defined(ZIL_MSWINDOWS_CTL3D)
extern "C" LRESULT CALLBACK BtnWndProc3d(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

static int _groupOffset = -1;
static FARPROC _groupCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export GroupJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_GROUP *object = (UIW_GROUP *)GetWindowLong(hWnd, _groupOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _groupJumpInstance = (FARPROC)GroupJumpProcedure;

EVENT_TYPE UIW_GROUP::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{

	display->VirtualGet(screenID, trueRegion);
	UIW_WINDOW::DrawItem(event, ccode);

	int gap = display->TextHeight(text, screenID, font) / 2 + 1;

	display->Rectangle(screenID, trueRegion.left + 1, trueRegion.top + gap,
		trueRegion.right - 2, trueRegion.bottom - 2, LogicalPalette(ccode, ID_DARK_SHADOW));
	display->Rectangle(screenID, trueRegion.left + 2, trueRegion.top + gap + 1,
		trueRegion.right - 1, trueRegion.bottom - 1, LogicalPalette(ccode, ID_WHITE_SHADOW));

	display->Text(screenID, trueRegion.left + 5, trueRegion.top + 1, text, lastPalette, -1,
		TRUE, FALSE, ZIL_NULLP(UI_REGION), font);

	display->VirtualPut(screenID);

	return (TRUE);
}

EVENT_TYPE UIW_GROUP::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	int processed = FALSE;
	EVENT_TYPE ccode = S_UNKNOWN;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;
		WPARAM wParam = event.message.wParam;

		processed = TRUE;

		switch (message)
		{

			case WM_ERASEBKGND:
				if (!FlagSet(woStatus, WOS_OWNERDRAW))
				{
					UI_WINDOW_OBJECT *oObject = NULL;
					for (oObject = parent; oObject &&
						(oObject->LogicalPalette(S_DISPLAY_ACTIVE)->colorBackground == BACKGROUND ||
						oObject->LogicalPalette(S_DISPLAY_ACTIVE)->fillPattern == PTN_RGB_COLOR);
						oObject = oObject->parent)
						;
					UI_PALETTE *outPalette = oObject->LogicalPalette(S_DISPLAY_ACTIVE);

					UI_WINDOW_OBJECT *bObject = NULL;
					for (bObject = this; bObject &&
						(bObject->LogicalPalette(S_DISPLAY_ACTIVE)->colorBackground == BACKGROUND ||
						bObject->LogicalPalette(S_DISPLAY_ACTIVE)->fillPattern == PTN_RGB_COLOR);
						bObject = bObject->parent)
						;
					lastPalette = bObject->LogicalPalette(ccode);

					int outsideTop = 8;
					UI_MSWINDOWS_DISPLAY::hDC = (HDC)wParam;
					display->VirtualGet(ID_DIRECT, trueRegion);
					display->Rectangle(screenID, trueRegion.left, trueRegion.top,
						trueRegion.right, trueRegion.top + outsideTop, outPalette, 0, TRUE);
					display->Rectangle(screenID, trueRegion.left, trueRegion.top + outsideTop,
						trueRegion.right, trueRegion.bottom, lastPalette, 0, TRUE);
					display->VirtualPut(ID_DIRECT);
					UI_MSWINDOWS_DISPLAY::hDC = 0;
				}
				ccode = TRUE;
				break;

			// Ambiguous cases.
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
				processed = FALSE;
				break;

			default:
				ccode = UIW_WINDOW::Event(event);
				break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = LogicalEvent(event, ID_GROUP);
		switch (ccode)
		{
		case S_INITIALIZE:
			UIW_WINDOW::Event(event);
			dwStyle |= BS_GROUPBOX;
			break;

		case S_CREATE:
			UI_WINDOW_OBJECT::Event(event);
			// Fall through to S_REGISTER_OBJECT.

		case S_REGISTER_OBJECT:
			{
			RegisterObject("UIW_GROUP", "BUTTON", &_groupOffset,
				&_groupJumpInstance, &_groupCallback, text);

			clipList.Destroy();

			RECT cRect;
			GetClientRect(screenID, &cRect);
			UI_REGION region;
			region.Assign(cRect);
			clipList.Add(new UI_REGION_ELEMENT(screenID, region));

			if (ccode == S_CREATE)
			{
				// Compute the list object regions.
				for (object = First(); object; object = object->Next())
					object->Event(event);
			}
			}
			break;

		case L_PREVIOUS:
		case L_NEXT:
/* START BLOCK COMMENT
**			if (!HasWrappedData())
**			{
**				EVENT_TYPE tCode = ccode;
**				UI_WINDOW_OBJECT *object = Current();
**				ccode = UIW_WINDOW::Event(event);
**				if (tCode != ccode)
**				{
**					object = (tCode == L_NEXT) ? First() : Last();
**					while (object && (object->IsNonselectable() || object->IsNoncurrent()))
**						object = (tCode == L_NEXT) ? object->Next() : object->Previous();
**					if (object)
**						SetCurrent(object);
**					ccode = S_UNKNOWN;
**				}
**			}
**			else
END BLOCK COMMENT */
			ccode = S_UNKNOWN;
		break;

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_GROUP::OSDataSet(void)
{
	// Redisplay the string.
	if (screenID)
	{
		InvalidateRect(screenID, ZIL_NULLP(RECT), TRUE);
#if defined(ZIL_UNICODE) 
		char *TEXT_ = MapText(text, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ text
#endif
		SendMessage(screenID, WM_SETTEXT, 0, (LONG)TEXT_);
	}
}

void UIW_GROUP::OSUpdateSettings(ZIL_OBJECTID objectID)
{
		// See if the field needs to be re-computed.
		if (objectID == ID_GROUP && FlagSet(woStatus, WOS_REDISPLAY))
		{
			Event(UI_EVENT(S_INITIALIZE));
			Event(UI_EVENT(S_CREATE));
		}
}

