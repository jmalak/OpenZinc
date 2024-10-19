//	Zinc Interface Library - M_GROUP.CPP
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
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
}

// ----- UIW_GROUP ---------------------------------------------------------

EVENT_TYPE UIW_GROUP::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	return UIW_WINDOW::DrawItem(event, ccode);
}

EVENT_TYPE UIW_GROUP::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_GROUP);
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
	case S_REGISTER_OBJECT:
		{
		UI_WINDOW_OBJECT::Event(event);
		labelWidget = ZIL_NULLP(_WidgetRec);
		UI_REGION region = trueRegion;
		nargs = 0;
		trueRegion.bottom -= 2;
		RegisterObject(xmFrameWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
			ccode, TRUE);

		trueRegion -= 2;
		trueRegion.right -= trueRegion.left;
		trueRegion.bottom -= trueRegion.top;
		trueRegion.left = 0;
#if (ZIL_MOTIF >= 1002)
		if (text && *text)
			trueRegion.top = display->cellHeight / 2 + 1;
		else
#endif
			trueRegion.top = 0;
		nargs = 0;
		XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;
		RegisterObject(xmFormWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
			ccode, TRUE, TRUE, screenID);


		if (text && *text && ccode != S_CHANGED)
		{
			XmString labelText = CreateMotifString(text);
			Widget store = screenID;
			nargs = 0;
			if (hotKey)
				XtSetArg(args[nargs], XmNmnemonic, hotKey), nargs++;
			XtSetArg(args[nargs], XmNlabelString, labelText); nargs++;
			Widget labelParent = screenID;
#if (ZIL_MOTIF <= 1001)
			XtSetArg(args[nargs], XmNshadowThickness, 1); nargs++;
			XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_BEGINNING); nargs++;
#else
			labelParent = XtParent(screenID);
			XtSetArg(args[nargs], XmNchildType, XmFRAME_TITLE_CHILD); nargs++;
#endif
			trueRegion.right = trueRegion.bottom = 0;
			RegisterObject(xmLabelWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE,
				TRUE, labelParent);
			if (ccode != S_CHANGED)
				labelWidget = screenID;
			nargs = 0;
#if (ZIL_MOTIF <= 1001)
			XtSetArg(args[nargs], XmNheight,
				display->cellHeight * 3 / 4); nargs++;
			XtSetArg(args[nargs], XmNwidth,  trueRegion.right - trueRegion.left);
				nargs++;
#endif
			XtSetArg(args[nargs], XmNtraversalOn, FALSE); nargs++;
			XtSetValues(labelWidget, args, nargs);
			if (labelText)
				XmStringFree(labelText);
			screenID = store;
		}

		for (UI_WINDOW_OBJECT *object = First(); object;
				object = object->Next())
		{
			object->Event(event);
		}
		trueRegion = region;
		}
		break;

	case S_REDISPLAY:
		if (screenID && labelWidget && !XtIsManaged(labelWidget))
			XtManageChild(labelWidget);
		ccode = UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_GROUP::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);

	int offset = display->cellHeight * 7 / 8;
#if (ZIL_MOTIF < 1002)
	if (text && *text)
		offset = display->cellHeight / 4;
#else
	offset = display->cellHeight;
#endif

	object->trueRegion.top -= offset;
	object->trueRegion.bottom -= offset;
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_GROUP::OSDataSet(void)
{
	if (screenID)
	{
		nargs = 0;
		XmString xms = CreateMotifString(text);
		XtSetArg(args[nargs], XmNlabelString, xms); nargs++;
		XtSetValues(labelWidget, args, nargs);
		XmStringFree(xms);
		Event(UI_EVENT(S_REDISPLAY));
	}
}

void UIW_GROUP::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_GROUP && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
