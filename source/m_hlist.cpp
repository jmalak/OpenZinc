//	Zinc Interface Library - M_HLIST.CPP
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
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
//include added by jdh to allow a form under linux
#include <Xm/Form.h>
}

// ----- UIW_HZ_LIST --------------------------------------------------------

EVENT_TYPE UIW_HZ_LIST::Event(const UI_EVENT &event)
{
	static int initializedTime = FALSE;
	static ZIL_TIME lastTime;
	static UI_WINDOW_OBJECT *lastObject;
	if (!initializedTime)
	{
		lastTime.Import();
		lastObject = ZIL_NULLP(UI_WINDOW_OBJECT);
		initializedTime = TRUE;
	}
#if (ZIL_MOTIF > 1001)
	void ScrollObscuredCallback(Widget, XtPointer, XtPointer);
#endif

	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);

	// Check for Zinc specific messages.
	EVENT_TYPE ccode = LogicalEvent(event, ID_HZ_LIST);
	switch (ccode)
	{
	case S_INITIALIZE:
		for (object = First(); object; object = object->Next())
			object->relative.bottom = object->relative.top;
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
	case S_REGISTER_OBJECT:
		clipList.Destroy();
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woFlags, WOF_BORDER))
			trueRegion -= 2;
		clipList.Add(new UI_REGION_ELEMENT(screenID, 0, 0,
			trueRegion.right - trueRegion.left, trueRegion.bottom - trueRegion.top));
			
	// line below added by jdh to allow region to be available throughout
	UI_REGION region;


#if defined(ZIL_LINUX)

//under linux UIW_HZ_LIST needs to be enclosed in a frame and a form to 
//function properly spacially
	
		//create frame
		region = trueRegion;
		nargs = 0;
		trueRegion.bottom -= 2;
		RegisterObject(xmFrameWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
			ccode, TRUE);
		trueRegion.right -= trueRegion.left;
		trueRegion.bottom -= trueRegion.top;
		trueRegion.left = 0;
		trueRegion.top = 0;
		nargs = 0;
		XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;
		RegisterObject(xmFormWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
			ccode, TRUE, TRUE, screenID);
		
#endif

		// Create the Window
		if (hScroll)		// scroll bar?
		{
			nargs = 0;
			XtSetArg(args[nargs], XmNscrollingPolicy, XmAUTOMATIC); nargs++;
			XtSetArg(args[nargs], XmNscrollBarDisplayPolicy, XmSTATIC); nargs++;
			RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateScrolledWindow,
				ccode, TRUE, TRUE, screenID);
#			if (ZIL_MOTIF > 1001)
			XtAddCallback(screenID, XmNtraverseObscuredCallback,
					ScrollObscuredCallback, (XtPointer)this);
#			endif

			Widget vertScroll = ZIL_NULLP(_WidgetRec),
				horizScroll = ZIL_NULLP(_WidgetRec);
			nargs = 0;
			XtSetArg(args[nargs], XmNhorizontalScrollBar, &horizScroll);
				nargs++;
			XtSetArg(args[nargs], XmNverticalScrollBar, &vertScroll);
				nargs++;
			XtGetValues(screenID, args, nargs);
			hScroll->screenID = horizScroll;
			XtVaSetValues(horizScroll, XmNtraversalOn, FALSE, NULL);
			trueRegion.bottom -= 31;
			XtUnmanageChild(vertScroll);
		}



// code below becomes unnecessary because we have already provided a frame

/*		if (FlagSet(woFlags, WOF_BORDER) && !(hScroll || vScroll) &&
			ccode == S_CREATE)
		{
			nargs = 0;
			XtSetArg(args[nargs], XmNshadowType, XmSHADOW_IN); nargs++;
			XtSetArg(args[nargs], XmNshadowThickness,
				FlagSet(woFlags, WOF_BORDER) ? 2 : 0); nargs++;
			RegisterObject(xmFrameWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE);
		}
		
		*/

		// create the RowColumn for the window
		{
			nargs = 0;
			XtSetArg(args[nargs], XmNpacking, XmPACK_TIGHT); nargs++;
			XtSetArg(args[nargs], XmNorientation, XmVERTICAL); nargs++;
			XtSetArg(args[nargs], XmNnumColumns, trueRegion.bottom - trueRegion.top); nargs++;
			XtSetArg(args[nargs], XmNspacing, 0); nargs++;
			XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
			XtSetArg(args[nargs], XmNmarginWidth, 0); nargs++;
			XtSetArg(args[nargs], XmNentryAlignment, XmALIGNMENT_BEGINNING); nargs++;			
			XtSetArg(args[nargs], XmNresizeHeight, FALSE); nargs++;
			XtSetArg(args[nargs], XmNresizeWidth, FALSE); nargs++;
			XtSetArg(args[nargs], XmNadjustLast, FALSE); nargs++;
			RegisterObject(xmRowColumnWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, FALSE, screenID ? screenID : 0);
		}

		// Compute the support object regions.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object;
			object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, FALSE);
		}

		// Fall thru.    

	case S_REDISPLAY:
		for (object = First(); object; object = object->Next())
			object->Event(event);
		
		// Initialize the scroll information.
		if (ccode == S_CREATE)
		{
			UI_EVENT sEvent(S_SCROLLRANGE);
			UIW_WINDOW::ScrollEvent(sEvent);
		}

		UI_WINDOW_OBJECT::Event(UI_EVENT(S_DISPLAY_ACTIVE));
		UI_WINDOW_OBJECT::Event(UI_EVENT(S_REDISPLAY));

		// Use location and width of last child to figure width of row col.
		if (screenID && First() && (hScroll || vScroll))
		{
			WidgetList children;
			Cardinal numChildren;
			XtVaGetValues(screenID, XmNchildren, &children,
				XmNnumChildren, &numChildren, ZIL_NULLP(void));
			Dimension childX, childWidth;
			if (numChildren)
				XtVaGetValues(children[numChildren-1], XmNx, &childX,
					XmNwidth, &childWidth, ZIL_NULLP(void));
			XtVaSetValues(screenID, XmNwidth, childX + childWidth + 2,
				ZIL_NULLP(void));
		}
		break;

	case S_CHANGED:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_BEGIN_SELECT:
	case L_SELECT:
	case L_END_SELECT:
		{
		UI_WINDOW_OBJECT *current = Current();

		// Handle case of non-selectable child getting clicked.
		if (current && current->screenID && event.type == E_MOTIF &&
			(event.message.type == ButtonPress ||
			event.message.type == ButtonRelease) &&
			event.message.xbutton.window != XtWindow(current->screenID))
		{
			for (object = First(); object; object = object->Next())
				if (event.message.xbutton.window == XtWindow(object->screenID))
				{
					current = object;
					break;
				}
		}
		if (current)
		{
			// Determine if button release occured in current child.
			int inChild = TRUE;
			if (event.type == E_MOTIF && (event.message.type == ButtonPress ||
				event.message.type == ButtonRelease) &&
				event.message.xbutton.window == XtWindow(screenID))
			{
				inChild = FALSE;
				ccode = UIW_WINDOW::Event(event);
			}
			else
			{
				if (ccode == L_END_SELECT)
				{
					ZIL_TIME currentTime;
					long elapsedTime = currentTime - lastTime;
					BTF_FLAGS btFlags = 0;
					Current()->Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
					if (FlagSet(btFlags, BTF_DOUBLE_CLICK) &&
						AbsValue(elapsedTime) < doubleClickRate &&
						lastObject == current)
					{
						ccode = current->UserFunction(event, L_DOUBLE_CLICK);
					}
					else
					{
						ccode = current->Event(event);
					}
					lastTime = currentTime;
					lastObject = current;
				}
				else
				{
					ccode = Current()->Event(event);
				}
			}
		}
		else
		{
			ccode = UIW_WINDOW::Event(event);
		}
		}
		break;

	case L_HELP:
		ccode = UIW_WINDOW::Event(event);
		break;
		
	default:
		if (hScroll && event.type == E_MOTIF && event.message.type == EnterNotify && First())
			XtVaSetValues(hScroll->screenID, XmNincrement, First()->trueRegion.Width(), ZIL_NULLP(void));
		if (!(event.type == E_MOTIF && event.message.type == KeyPress))
			ccode = UIW_WINDOW::Event(event);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_HZ_LIST::OSDestroy(void)
{
	if (screenID)
		UIW_WINDOW::Destroy();
}

void UIW_HZ_LIST::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_HZ_LIST && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}

void UIW_HZ_LIST::OSSort(void)
{
	Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_HZ_LIST);
}
