//	Zinc Interface Library - M_PLLDN.CPP
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
#include <Xm/RowColumn.h>
}

static Cardinal OrderProc(Widget widget)
{
	// This could be used for an XtOrderProc() for any UIW_WINDOW.
	UI_WINDOW_OBJECT *object;
	XtVaGetValues(widget, XmNuserData, &object, NULL);
	Cardinal ret = 0;

	int index = ((UIW_WINDOW* )object->parent)->Index(object);
	if (index == -1)
		XtVaGetValues(XtParent(widget), XmNnumChildren, &ret, NULL);
	else
		ret = (Cardinal)index;
	return ret;
}

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_MENU::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_PULL_DOWN_MENU);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		break;

	case S_CHANGED:
	case S_CREATE:
	case S_REGISTER_OBJECT:
		{
		ccode = UI_WINDOW_OBJECT::Event(event);
		trueRegion.bottom = trueRegion.top + display->cellHeight;

		nargs = 0;
		XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_FORM); nargs++;
		XtSetArg(args[nargs], XmNrightAttachment, XmATTACH_FORM); nargs++;
		XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_FORM); nargs++;
		XtSetArg(args[nargs], XmNresizeHeight, FALSE); nargs++;
		XtSetArg(args[nargs], XmNinsertPosition, OrderProc); nargs++;
		RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateMenuBar, ccode, TRUE);
		trueRegion.bottom -= 1;

		if (ccode != S_CREATE)
			break;

		for (UI_WINDOW_OBJECT *object = First(); object;
				object = object->Next())
			object->Event(event);
		}
		break;

	case L_UP:
	case L_DOWN:
	case L_LEFT:
	case L_RIGHT:
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PULL_DOWN_MENU::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (objectID == ID_PULL_DOWN_MENU && screenID)
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
