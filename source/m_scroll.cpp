//	Zinc Interface Library - M_SCROLL.CPP
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
#include <Xm/ScrollBar.h>
#include <Xm/Scale.h>
}

//	ScrollCallback is called each time the slider position is changed.
void UIW_SCROLL_BAR::ScrollCallback(Widget widget, XtPointer client,
	XtPointer call)
{
	UIW_SCROLL_BAR *object = (UIW_SCROLL_BAR *)client;
	if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) && !(object->userFunction))
		return;

	UI_EVENT sEvent(S_UNKNOWN);
	XmScrollBarCallbackStruct *structPtr = (XmScrollBarCallbackStruct *)call;

	int value, max, min, inc, pageInc;
	XtVaGetValues(widget, XmNvalue, &value,
		XmNmaximum, &max,
		XmNminimum, &min,
		XmNincrement, &inc,
		XmNpageIncrement, &pageInc, ZIL_NULLP(void));
	sEvent.scroll.current = value;
	sEvent.scroll.maximum = max;
	sEvent.scroll.minimum = min;
	sEvent.scroll.showing = pageInc;

	if (FlagSet(object->sbFlags, SBF_SCALE))
	{
		XtVaGetValues(widget, XmNvalue, &value,
			XmNscaleMultiple, &pageInc, ZIL_NULLP(void));
		sEvent.scroll.current = value;
		sEvent.scroll.showing = pageInc;
	}

	if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
	{
		int oldCurrent;
		object->Information(I_GET_VALUE, &oldCurrent);

		// Zinc scrolled window scrollbar.
		sEvent.type = FlagSet(object->sbFlags, SBF_VERTICAL) ?
			S_VSCROLL : S_HSCROLL;
		int delta = 0;
		switch(structPtr->reason)
		{
		case XmCR_DECREMENT:
		case XmCR_INCREMENT:
		case XmCR_PAGE_DECREMENT:
		case XmCR_PAGE_INCREMENT:
		case XmCR_DRAG:
			delta = value - oldCurrent;
			break;

		default:
			break;
		}
   		if (delta)
		{
			sEvent.scroll.delta = delta;
			object->parent->Event(sEvent);
		}
	}
	else
	{
		switch(structPtr->reason)
		{
			case XmCR_DECREMENT:
				sEvent.type = L_UP;
				break;
			case XmCR_INCREMENT:
				sEvent.type = L_DOWN;
				break;
			case XmCR_PAGE_INCREMENT:
				sEvent.type = L_PGDN;
				break;
			case XmCR_PAGE_DECREMENT:
				sEvent.type = L_PGUP;
				break;
			case XmCR_VALUE_CHANGED:
				sEvent.type = L_END_SELECT;
				break;
			case XmCR_DRAG:
				sEvent.type = L_CONTINUE_SELECT;
				break;
			case XmCR_TO_TOP:
				sEvent.type = L_TOP;
				break;
			case XmCR_TO_BOTTOM:
				sEvent.type = L_BOTTOM;
				break;
		}
		object->scroll.current = value;
	}
	if (object->userFunction)
		object->userFunction(object, sEvent, L_SELECT);
}


// ----- UIW_SCROLL_BAR -----------------------------------------------------

EVENT_TYPE UIW_SCROLL_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	return (FALSE);
}

EVENT_TYPE UIW_SCROLL_BAR::Event(const UI_EVENT &event)
{
	UI_EVENT sEvent;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_SCROLL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			;
		else if (FlagSet(sbFlags, SBF_VERTICAL) && parent)
			parent->Information(I_SET_VSCROLL, this);
		else if (FlagSet(sbFlags, SBF_HORIZONTAL) && parent)
			parent->Information(I_SET_HSCROLL, this);
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_CHANGED:
	case S_CREATE:
		// Undo RegionConvert of a 0 width or height.
		if (relative.right == relative.left + display->cellWidth - 1)
			relative.right = relative.left + 20;

		// Compute the object region.
		ccode = UI_WINDOW_OBJECT::Event(event);

		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			trueRegion.left = trueRegion.top = trueRegion.right = trueRegion.bottom = 0;
			if (FlagSet(sbFlags, SBF_CORNER))
				return ccode;
		}
		else
		{
			// Not a support object, just a regular scrollbar that calls UserFunction.
			if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
			{
				if (FlagSet(sbFlags, SBF_HORIZONTAL))
					trueRegion.top = trueRegion.bottom - 14;
				else
					trueRegion.left = trueRegion.right - 14;
			}
			else
			{
				if (FlagSet(sbFlags, SBF_HORIZONTAL))
					trueRegion.bottom = trueRegion.top + 14;
				else
					trueRegion.right = trueRegion.left + 14;
			}
		}

		nargs = 0;
		if (FlagSet(sbFlags, SBF_SCALE))
		{
			XtSetArg(args[nargs], XmNvalue, (int)scroll.current); nargs++;
			XtSetArg(args[nargs], XmNminimum, (int)scroll.minimum); nargs++;
			XtSetArg(args[nargs], XmNmaximum, (int)scroll.maximum); nargs++;
			if (scroll.showing)
				XtSetArg(args[nargs], XmNscaleMultiple, (int)scroll.showing),
					nargs++;
			XtSetArg(args[nargs], XmNorientation,
				FlagSet(sbFlags, SBF_HORIZONTAL) ? XmHORIZONTAL :
				XmVERTICAL); nargs++;
			RegisterObject(xmScaleWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE);
			XtAddCallback(screenID, XmNvalueChangedCallback,
				ScrollCallback, (XtPointer)this);
			XtAddCallback(screenID, XmNdragCallback,
				ScrollCallback, (XtPointer)this);
		}
		else
		{
			// Not a scale.  Show it as a scrollbar.
			XtSetArg(args[nargs], XmNvalue, scroll.current); nargs++;
			// Keeps a warning from happening
			if (scroll.minimum < scroll.maximum)
			{
				XtSetArg(args[nargs], XmNminimum, scroll.minimum); nargs++;
				XtSetArg(args[nargs], XmNmaximum, scroll.maximum); nargs++;
			}
			if (scroll.delta)
				XtSetArg(args[nargs], XmNincrement, scroll.delta), nargs++;
			if (scroll.showing)
				XtSetArg(args[nargs], XmNpageIncrement, scroll.showing), nargs++;
			XtSetArg(args[nargs], XmNorientation,
				FlagSet(sbFlags, SBF_HORIZONTAL) ? XmHORIZONTAL :
				XmVERTICAL); nargs++;
			XtSetArg(args[nargs], XmNhighlightOnEnter, TRUE); nargs++;
			XtSetArg(args[nargs], XmNnavigationType, XmTAB_GROUP); nargs++;
			UI_REGION region = trueRegion;
			if (FlagSet(woFlags, WOF_NON_FIELD_REGION) ||
				FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				trueRegion.left = trueRegion.top = trueRegion.right = trueRegion.bottom = 0;
			if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT) ||
				(FlagSet(parent->woStatus, WOS_OWNERDRAW) &&
				!parent->Inherited(ID_LIST)))
				RegisterObject(xmScrollBarWidgetClass,
					ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE,
					TRUE, FlagSet(woFlags, WOF_SUPPORT_OBJECT) ?
					XtParent(parent->screenID) : 0);
		 
			trueRegion = region;
			if (screenID == 0)
				break;	// Scrollbar doesn't exist because parent is MDI parent.
			if (ccode != S_CHANGED)
			{
				XtAddCallback(screenID, XmNincrementCallback,
					ScrollCallback, (XtPointer)this);
				XtAddCallback(screenID, XmNdecrementCallback,
					ScrollCallback, (XtPointer)this);
				XtAddCallback(screenID, XmNpageIncrementCallback,
					ScrollCallback, (XtPointer)this);
				XtAddCallback(screenID, XmNpageDecrementCallback,
					ScrollCallback, (XtPointer)this);
				XtAddCallback(screenID, XmNvalueChangedCallback,
					ScrollCallback, (XtPointer)this);
				XtAddCallback(screenID, XmNdragCallback,
					ScrollCallback, (XtPointer)this);
				XtAddCallback(screenID, XmNtoTopCallback,
					ScrollCallback, (XtPointer)this);
				XtAddCallback(screenID, XmNtoBottomCallback,
					ScrollCallback, (XtPointer)this);
			}

			if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			{
				int sliderSize;
				Dimension height, width;
				XtVaGetValues(screenID, XmNsliderSize, &sliderSize, XmNheight, &height, XmNwidth, &width, ZIL_NULLP(void));
				XtVaSetValues(screenID, XmNmaximum, scroll.maximum + sliderSize, ZIL_NULLP(void));
				trueRegion.right = trueRegion.left + width - 1;
				trueRegion.bottom = trueRegion.top + height - 1;
			}
		}
		break;

	case S_CURRENT:
	case S_REDISPLAY:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			ccode = UIW_WINDOW::Event(event);
		break;

	case S_DEINITIALIZE:
		if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_VSCROLL_SET:
	case S_HSCROLL_SET:
	case S_VSCROLL:
	case S_HSCROLL:
		if (((ccode == S_HSCROLL_SET || ccode == S_HSCROLL) &&
			!FlagSet(sbFlags, SBF_HORIZONTAL)) ||
			((ccode == S_VSCROLL_SET || ccode == S_VSCROLL) &&
			!FlagSet(sbFlags, SBF_VERTICAL)))
			return (S_ERROR);

		if (ccode == S_VSCROLL || ccode == S_HSCROLL)
		{
			scroll.current += event.scroll.delta;
			if (scroll.current < scroll.minimum)
				scroll.current = scroll.minimum;
			else if (scroll.current > scroll.maximum)
				scroll.current = scroll.maximum;
		}
		else
			scroll = event.scroll;

		if (!screenID || (FlagSet(woFlags, WOF_SUPPORT_OBJECT) &&
				(!FlagSet(parent->woStatus, WOS_OWNERDRAW) ||
				parent->Inherited(ID_LIST))))
			break;
		if (FlagSet(sbFlags, SBF_SCALE))
			XtVaSetValues(screenID, XmNvalue, (int)scroll.current,
				XmNmaximum, (int)scroll.maximum,
				XmNminimum, (int)scroll.minimum,
				XmNpageIncrement, (int)scroll.showing, ZIL_NULLP(void));
		else
			XtVaSetValues(screenID,
				XmNvalue, (int)MinValue(scroll.current, scroll.maximum),
				XmNmaximum, (int)scroll.maximum + scroll.showing,
				XmNminimum, (int)scroll.minimum,
				XmNsliderSize, (int)scroll.showing,
				XmNpageIncrement, (int)scroll.showing, ZIL_NULLP(void));
		break;

	case L_BEGIN_SELECT:
	case L_END_SELECT:
#if defined(ZIL_EDIT)	
		if (FlagSet(woStatus, WOS_EDIT_MODE) &&
			parent && parent->Inherited(ID_TABLE) &&
			FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			return (S_UNKNOWN);
		else
#endif
			ccode = UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_SCROLL_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_SCROLL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
