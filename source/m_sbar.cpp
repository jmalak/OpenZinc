//	M_SBAR.CPP (STATUSBAR) - This file contains the status bar class.
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


extern "C"
{
#	include <Xm/BulletinB.h>
}

EVENT_TYPE UIW_STATUS_BAR::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);

	lastPalette = LogicalPalette(ccode, ID_DARK_SHADOW);

	UI_REGION region = trueRegion;
//	if (FlagSet(woFlags, WOF_BORDER))
//		DrawBorder(screenID, region, FALSE, ccode);

	// Update the display.
	display->VirtualPut(screenID);
	return (FALSE);
}

EVENT_TYPE UIW_STATUS_BAR::Event(const UI_EVENT &event)
{
	const int STATUS_OFFSET = 4;
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
	case S_REGISTER_OBJECT:
		UI_WINDOW_OBJECT::Event(event);
		// Set top of status bar to be the programmer-specified height
		// plus a few pixels above and below.
		trueRegion.top = (int)(trueRegion.bottom - height * display->cellHeight /
			(FlagSet(woFlags, WOF_MINICELL) ? display->miniDenominatorY : 1) -
			2 * STATUS_OFFSET);
		nargs = 0;
		XtSetArg(args[nargs], XmNautoUnmanage, FALSE); nargs++;
		XtSetArg(args[nargs], XmNdefaultPosition, FALSE); nargs++;
		XtSetArg(args[nargs], XmNmarginWidth, 0); nargs++;
		XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
		XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;
		XtSetArg(args[nargs], XmNshadowThickness, 2); nargs++;
		XtSetArg(args[nargs], XmNshadowType, XmSHADOW_IN); nargs++;
		RegisterObject(xmBulletinBoardWidgetClass,
			ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE, screenID);
		
		{
		clipList.Destroy();
		UI_REGION region;
		region.left = display->cellWidth;
		region.top = STATUS_OFFSET;
		region.right = trueRegion.Width() - display->cellWidth - 1;
		region.bottom = trueRegion.Height() - STATUS_OFFSET - 1;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
		//lines below changed by jdh to conform to current scoping of "for"
		//for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First();
		//	object; object = object->Next())
		UI_WINDOW_OBJECT *object;
		for (object = (UI_WINDOW_OBJECT *)support.First();
			object; object = object->Next())
		{
			object->Event(event);
		}
		for (object = First(); object;
				object = object->Next())
		{
			object->woFlags |= WOF_VIEW_ONLY;
			object->woAdvancedFlags |= WOAF_NON_CURRENT;
			object->Event(event);
		}
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

void UIW_STATUS_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_STATUS_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION) && parent)
			parent->Event(event);
		else
			Event(event);
	}
}
