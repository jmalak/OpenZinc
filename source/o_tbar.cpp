//	Zinc Interface Library - O_TBAR.CPP
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

// ----- UIW_TOOL_BAR -------------------------------------------------------

EVENT_TYPE UIW_TOOL_BAR::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		if (event.message.msg == WM_PAINT)
		{
			RECTL rect;
			UI_OS2_DISPLAY::hps = WinBeginPaint(screenID, ZIL_NULLH(HPS), &rect);
			display->VirtualGet(ID_DIRECT, trueRegion);
			display->Rectangle(screenID, trueRegion, lastPalette, 0, TRUE);
			display->Line(screenID, trueRegion.left, trueRegion.bottom, trueRegion.right,
				trueRegion.bottom, lastPalette, 1, FALSE);
			WinEndPaint(UI_OS2_DISPLAY::hps);
			display->VirtualPut(ID_DIRECT);
			return(0);
		}

		return (UIW_WINDOW::Event(event));
	}

	// Check for Zinc specific messages.
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_CHANGED:
		// Adjust Size and Position children.
		if (!FlagSet(wnFlags, WNF_NO_WRAP) && First())
		{
			int height = 1;
			for (object = First(); object; object = object->Next())
				if (height < object->relative.Height())
					height = object->relative.Height();

			int left = 0, top = 0;
			for (object = First(); object; object = object->Next())
			{
				int width = object->relative.Width();
				if (left > 0 && left + width > trueRegion.Width())
				{
					left = 0;
					top += height;
				}
				object->relative.left = left;
				object->relative.top = top;
				left += width;
				object->relative.right = left - 1;
				object->relative.bottom = top + height - 1;
			}

			trueRegion.bottom = trueRegion.top + top + height + 1;
		}
		else
			trueRegion.bottom = trueRegion.top + relative.Height() + 1;
		break;

	case S_REGISTER_OBJECT:
		{
		UI_WINDOW_OBJECT::Event(event);
		ULONG value = SYSCLR_BUTTONMIDDLE;
		WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(value), &value);
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TOOL_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TOOL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);

		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			ZIL_SCREENID frameID;
			parent->Information(I_GET_FRAMEID, &frameID);
			WinSendMsg(frameID, WM_UPDATEFRAME, (MPARAM)0, (MPARAM)0);
		}
	}
}
