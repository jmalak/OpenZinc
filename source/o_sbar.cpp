//	O_SBAR.CPP (STATUSBAR) - This file contains the status bar class.
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

const int STATUS_OFFSET = 2;

EVENT_TYPE UIW_STATUS_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE)
{
	display->VirtualGet(screenID, trueRegion);
	display->Rectangle(screenID, trueRegion, lastPalette, 0, TRUE);
	display->Line(screenID, trueRegion.left, trueRegion.top, trueRegion.right, trueRegion.top, lastPalette);
	display->VirtualPut(screenID);
	return (FALSE);
}

EVENT_TYPE UIW_STATUS_BAR::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
		return (UIW_WINDOW::Event(event));

	// Check for Zinc specific messages.
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	ccode = LogicalEvent(event, ID_STATUS_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		for (object = First(); object; object = object->Next())
		{
			object->woFlags |= WOF_VIEW_ONLY;
			object->woAdvancedFlags |= WOAF_NON_CURRENT;
		}
		UIW_WINDOW::Event(event);
		break;

	case S_CHANGED:
		{
		int pixelHeight = height;
		if (!FlagSet(woFlags, WOF_PIXEL))
		{
			pixelHeight *= display->cellHeight;
			if (FlagSet(woFlags, WOF_MINICELL))
				pixelHeight = pixelHeight * display->miniNumeratorY / display->miniDenominatorY;
			else
				pixelHeight -= display->preSpace + display->postSpace;
		}
		pixelHeight += 2 * STATUS_OFFSET;
		trueRegion.top = trueRegion.bottom - pixelHeight;
		relative = trueRegion;
		}
		break;

	case S_SIZE:
		if (!event.rawCode)
		{
			height = event.region.Height();
			if (FlagSet(woFlags, WOF_MINICELL))
				height = height * display->miniDenominatorY / display->miniNumeratorY;
			else if (!FlagSet(woFlags, WOF_PIXEL))
			{
				height += display->preSpace + display->postSpace - 2 * STATUS_OFFSET;
				height /= display->cellHeight;
			}
		}
		ccode = UIW_WINDOW::Event(event);
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

void UIW_STATUS_BAR::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);
	object->trueRegion.left += display->cellWidth;
	object->trueRegion.top += STATUS_OFFSET + 1;
	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
	{
		object->trueRegion.right -= display->cellWidth;
		object->trueRegion.bottom -= STATUS_OFFSET;
	}
	else if (!FlagSet(woFlags, WOF_MINICELL))
		object->trueRegion.top -= display->preSpace;
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_STATUS_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_STATUS_BAR && FlagSet(woStatus, WOS_REDISPLAY))
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
