//	Zinc Interface Library - D_MIN.CPP
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

// ----- UIW_MINIMIZE_BUTTON ------------------------------------------------

EVENT_TYPE UIW_MINIMIZE_BUTTON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_MINIMIZE_BUTTON);
	switch (ccode)
	{

	case S_CHANGED:
	case S_CREATE:
		value = L_MINIMIZE;
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
#if !defined(ZIL_GRAPHICS_ONLY)
			bitmapArray = ZIL_NULLP(ZIL_UINT8);
			bitmapWidth = bitmapHeight = 0;
			text = myDecorations->GetText(FlagSet(parent->woStatus, WOS_MINIMIZED) ? ZIL_Minimized : ZIL_MinNormal);
#	if defined(ZIL_SHADOW_BORDER)
			trueRegion.bottom = trueRegion.top;
			trueRegion.left = trueRegion.right - display->TextWidth(text, screenID, font) + 1;
#	elif defined(ZIL_STANDARD_BORDER)
			trueRegion.top = trueRegion.bottom = parent->trueRegion.top;
			trueRegion.left = parent->trueRegion.right - 2;
			for (UI_WINDOW_OBJECT *object = Previous(); object; object = object->Previous())
				if (object->Inherited(ID_BORDER))
					trueRegion.left--;
				else if (object->Inherited(ID_MAXIMIZE_BUTTON))
					trueRegion.left -= 3;
			trueRegion.right = trueRegion.left + 2;
			clip = trueRegion;
#	elif defined(ZIL_3D_BORDER)
			trueRegion.top = trueRegion.bottom = parent->trueRegion.top;
			trueRegion.left = parent->trueRegion.right - 7;
			trueRegion.right = trueRegion.left + 2;
			clip = trueRegion;
#	endif
#endif
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
			text = ZIL_NULLP(ZIL_ICHAR);
			ZIL_UINT8 *bitmap = myDecorations->GetBitmap(FlagSet(parent->woStatus, WOS_MINIMIZED) ? ZIL_Minimized : ZIL_MinNormal);
			bitmapWidth = bitmap[0];
			bitmapHeight = bitmap[1];
			bitmapArray = &bitmap[2];
#	if defined(ZIL_MSWINDOWS_STYLE)
			trueRegion.bottom = --trueRegion.top + (display->cellHeight - display->preSpace - display->postSpace);
			trueRegion.left = ++trueRegion.right - (display->cellHeight - display->preSpace - display->postSpace);
#	elif defined(ZIL_OS2_STYLE)
			// OS/2 style does not overlap parent region.
			trueRegion.bottom = trueRegion.top - 1 + (display->cellHeight - display->preSpace - display->postSpace);
			trueRegion.left = trueRegion.right + 1 - (display->cellHeight - display->preSpace - display->postSpace);
#	elif defined(ZIL_MOTIF_STYLE)
			// Motif clip region overlaps parent for shadowing.
			trueRegion.bottom = --trueRegion.top + (display->cellHeight - display->preSpace - display->postSpace);
			trueRegion.left = trueRegion.right - (display->cellHeight - display->preSpace - display->postSpace);
			clip = trueRegion;
#	endif
#endif
		}
		break;

	case L_CONTINUE_SELECT:
		return ccode;

	case L_BEGIN_SELECT:
		UIW_BUTTON::Event(event);
		while (ccode != L_END_SELECT)
		{
			UI_EVENT sEvent;
			eventManager->Get(sEvent);
			ccode = LogicalEvent(sEvent, ID_MINIMIZE_BUTTON);
			// Send all user messages to the window manager.
			if (sEvent.type > 9999)
				windowManager->Event(sEvent);
			else
				UIW_BUTTON::Event(sEvent);
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_MINIMIZE_BUTTON::OSUpdateSettings(ZIL_OBJECTID)
{
	// No code necessary.
}
