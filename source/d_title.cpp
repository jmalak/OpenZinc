//	Zinc Interface Library - D_TITLE.CPP
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

// ----- UIW_TITLE ----------------------------------------------------------

EVENT_TYPE UIW_TITLE::Event(const UI_EVENT &event)
{
	static ZIL_UTIME lastTime, currentTime;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TITLE);
	switch (ccode)
	{

	case S_CHANGED:
	case S_CREATE:
		value = FlagSet(parent->woStatus, WOS_MAXIMIZED) ? L_RESTORE : L_MAXIMIZE;
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
#if !defined(ZIL_GRAPHICS_ONLY)
#	if defined(ZIL_SHADOW_BORDER)
			trueRegion.bottom = trueRegion.top;
#	elif defined(ZIL_STANDARD_BORDER)
//			int width = (parent && parent->HotKey() &&     // BUG ROLM
//				parent->HotKey() != HOT_KEY_SUB_WINDOW) ? strlen(text) - 1 : strlen(text);
			int width = display->TextWidth(text, screenID, font);
			if (FlagSet(woFlags, WOF_BORDER))
				width += 2;
			trueRegion.top = trueRegion.bottom = parent->trueRegion.top;
			trueRegion.left = parent->trueRegion.left;
			trueRegion.right = parent->trueRegion.right;
			int border = 0;
			for (UI_WINDOW_OBJECT *object = Previous(); object; object = object->Previous())
			{
				if (object->Inherited(ID_SYSTEM_BUTTON))
					trueRegion.left += 3;
				else if (object->Inherited(ID_MAXIMIZE_BUTTON))
					trueRegion.right -= 3;
				else if (object->Inherited(ID_MINIMIZE_BUTTON))
					trueRegion.right -= 3;
				else if (object->Inherited(ID_BORDER))
				{
					border = TRUE;
					trueRegion.left++, trueRegion.right--;
				}
			}
			if (width < trueRegion.right - trueRegion.left && border)
			{
				if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
					trueRegion.left += (trueRegion.right - trueRegion.left - width + 1) / 2;
				else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
					trueRegion.left = trueRegion.right - width + 1;
				trueRegion.right = trueRegion.left + width - 1;
			}
			clip = trueRegion;
#	elif defined(ZIL_3D_BORDER)
			trueRegion.top = trueRegion.bottom = parent->trueRegion.top;
			int count = 0;
			for (UI_WINDOW_OBJECT *object = Previous(); object; object = object->Previous())
				if (object->trueRegion.top == parent->trueRegion.top)
					count++;
			if (count > 1)
			{
				trueRegion.left = parent->trueRegion.left + 4;
				trueRegion.right = parent->trueRegion.right - 8;
			}
			else
			{
				trueRegion.left = parent->trueRegion.left + 1;
				trueRegion.right = parent->trueRegion.right - 1;
			}
			clip = trueRegion;
#	endif
#endif
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
#	if defined(ZIL_MSWINDOWS_STYLE)
			trueRegion.left--; trueRegion.right++;
			trueRegion.bottom = --trueRegion.top + (display->cellHeight - display->preSpace - display->postSpace);
#	elif defined(ZIL_OS2_STYLE)
			// OS/2 style does not overlap parent region.
			trueRegion.bottom = trueRegion.top - 1 + (display->cellHeight - display->preSpace - display->postSpace);
#	elif defined(ZIL_MOTIF_STYLE)
			// Motif clip region overlaps parent for shadowing.
			NUMBERID maxID = NUMID_MAXIMIZE, minID = NUMID_MINIMIZE, sysID = NUMID_SYSTEM;
			if (!parent->Information(I_GET_NUMBERID_OBJECT, &sysID))
				trueRegion.left--;
			if (!parent->Information(I_GET_NUMBERID_OBJECT, &maxID) &&
				!parent->Information(I_GET_NUMBERID_OBJECT, &minID))
				trueRegion.right++;
			trueRegion.bottom = --trueRegion.top + (display->cellHeight - display->preSpace - display->postSpace);
			clip = trueRegion;
#	endif
#endif
		}
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (display->isText)
			lastPalette = ZIL_NULLP(UI_PALETTE);
			UI_WINDOW_OBJECT::Event(event);
		// Continue to S_REDISPLAY.
	case S_REDISPLAY:
		if (FlagSet(woStatus, WOS_REDISPLAY) || ccode == S_REDISPLAY)
		{
			UI_REGION region = trueRegion;
			if (!display->isText)
			{
#if defined(ZIL_MSWINDOWS_STYLE)
				if (FlagSet(woFlags, WOF_BORDER))
					DrawBorder(screenID, region, FALSE, ccode);
#elif defined(ZIL_OS2_STYLE)
				DrawShadow(screenID, region, 1, FALSE, ccode);
#elif defined(ZIL_OS2_STYLE) || defined(ZIL_MOTIF_STYLE)
				region.bottom--;
				DrawShadow(screenID, region, 1, FALSE, ccode);
				UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
				display->Line(screenID, trueRegion.left, trueRegion.bottom, trueRegion.right,
				trueRegion.bottom, darkShadow, 1, FALSE, &clip);
#endif
			}
			else if (display->isText && FlagSet(woFlags, WOF_BORDER))
				DrawBorder(screenID, region, FALSE, ccode);
			DrawText(screenID, region, text, lastPalette, TRUE, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case L_BEGIN_SELECT:
		{
		currentTime.Import();
		NUMBERID maxNumberID = NUMID_MAXIMIZE;
		if (currentTime.Difference(lastTime) > doubleClickRate &&
			!FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE) &&
			!FlagSet(parent->woStatus, WOS_MAXIMIZED))
		{
			parent->Event(UI_EVENT(L_MOVE, 0x0001, event.position));
		}
		else if (currentTime.Difference(lastTime) < doubleClickRate &&
			parent->Information(I_GET_NUMBERID_OBJECT, &maxNumberID))
		{
			UI_EVENT event(value);
			event.rawCode = 0;
			event.data = parent;
			eventManager->Put(event);
		}
		lastTime = currentTime;
		}
		break;

	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TITLE::OSDataSet(void)
{
	// Redisplay the string.
	if (parent && parent->screenID)
	{
#	if defined(ZIL_STANDARD_BORDER)
		if (display->isText)
			parent->Event(UI_EVENT(S_CREATE));
		for (UI_WINDOW_OBJECT *object = this; object && object->Previous();
			object = object->Previous())
			;
		for (;object != this; object = object->Next())
			object->Event(UI_EVENT(S_REDISPLAY));
#	endif
		Event(UI_EVENT(S_REDISPLAY));
	}
}

void UIW_TITLE::OSUpdateSettings(ZIL_OBJECTID)
{
	// No code necessary.
}
