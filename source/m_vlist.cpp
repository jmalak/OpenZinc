//	Zinc Interface Library - M_VLIST.CPP
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

// ----- UIW_VT_LIST --------------------------------------------------------

EVENT_TYPE UIW_VT_LIST::Event(const UI_EVENT &event)
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

	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_VT_LIST);
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
	case S_REGISTER_OBJECT:
		{
		clipList.Destroy();
		ccode = UI_WINDOW_OBJECT::Event(event);

	// line below added by jdh to allow region to be available throughout
	UI_REGION region;


#if defined(ZIL_LINUX)

//under linux UIW_VT_LIST needs to be enclosed in a frame and a form to 
//function properly spacially
	
	if (!comboShell)
	{	
		//create frame
		region = trueRegion;
		nargs = 0;
		trueRegion.bottom -= 2;
		RegisterObject(xmFrameWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
			ccode, TRUE);
		trueRegion -= 2;
		trueRegion.right -= trueRegion.left;
		trueRegion.bottom -= trueRegion.top;
		trueRegion.left = 0;
		trueRegion.top = 0;
		nargs = 0;
		XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;
		RegisterObject(xmFormWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
			ccode, TRUE, TRUE, screenID);
		trueRegion -=2;
		}

#endif

		// Create the Window
		if (vScroll && ccode != S_CHANGED)		// scroll bars?
		{
			nargs = 0;
			XtSetArg(args[nargs], XmNscrollingPolicy, XmAUTOMATIC); nargs++;
			XtSetArg(args[nargs], XmNscrollBarDisplayPolicy, XmSTATIC); nargs++;
			RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateScrolledWindow,
				ccode, TRUE, TRUE, comboShell ? comboShell : screenID);
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
			vScroll->screenID = vertScroll;
			XtVaSetValues(vertScroll, XmNtraversalOn, FALSE, NULL);
			XtUnmanageChild(horizScroll);
		}

		//UI_REGION region = trueRegion;
		region = trueRegion;
		
		if (vScroll && (ccode == S_CREATE || (ccode == S_CHANGED &&
			!FlagSet(woFlags, WOF_NON_FIELD_REGION))))
			region.right -= 26;
		if (FlagSet(woFlags, WOF_BORDER))
			region -= 2;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region.left,
			region.top, region.right, region.bottom));


// code below becomes unnecessary because we have already provided a frame
/*		if (FlagSet(woFlags, WOF_BORDER) && !(hScroll || vScroll) &&
			ccode == S_CREATE)
		{
			nargs = 0;
			XtSetArg(args[nargs], XmNshadowType, XmSHADOW_IN); nargs++;
			XtSetArg(args[nargs], XmNshadowThickness,
				FlagSet(woFlags, WOF_BORDER) ? 2 : 0); nargs++;
			RegisterObject(xmFrameWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE,
				comboShell ? comboShell : 0);
		} */
		
		

		// create the RowColumn for the window
		if (ccode != S_CHANGED)
		{
		nargs = 0;
		XtSetArg(args[nargs], XmNpacking, XmPACK_TIGHT), nargs++;
		XtSetArg(args[nargs], XmNorientation, XmVERTICAL); nargs++;
		XtSetArg(args[nargs], XmNnumColumns, trueRegion.bottom - trueRegion.top); nargs++;
		XtSetArg(args[nargs], XmNmarginWidth, 2), nargs++;
		XtSetArg(args[nargs], XmNmarginHeight, 2), nargs++;
		XtSetArg(args[nargs], XmNspacing, 0), nargs++;
		if (FlagSet(woStatus, WOS_EDIT_MODE) ||
			(!vScroll && FlagSet(woFlags, WOF_NON_FIELD_REGION)))
			XtSetArg(args[nargs], XmNresizeHeight, FALSE), nargs++;
		if (!vScroll || (vScroll && FlagSet(woFlags, WOF_NON_FIELD_REGION)))
			XtSetArg(args[nargs], XmNresizeWidth, FALSE), nargs++;

		RegisterObject(xmRowColumnWidgetClass,
			ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, FALSE,
			screenID ? screenID : comboShell ? comboShell : 0);
		}

		// Compute the support object regions.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object;
			object = object->Next())
			object->Event(event);
		region = clipList.First()->region;

		if (ccode == S_CREATE || (ccode == S_CHANGED &&
			FlagSet(woFlags, WOF_NON_FIELD_REGION)))
			XtVaSetValues(screenID, XmNwidth, region.Width(),
				XmNheight, region.Height(), ZIL_NULLP(void));

		// Compute the list element regions.
		for (object = First(); object; object = object->Next())
		{
			if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
				object->relative.bottom = object->relative.top;
			if (ccode == S_CHANGED)
				RegionMax(object);
			object->Event(event);
			if (vScroll && ccode == S_CREATE &&
				FlagSet(woFlags, WOF_NON_FIELD_REGION))
				object->trueRegion.right -= 4;
			else if (vScroll && ccode == S_CHANGED &&
				FlagSet(woFlags, WOF_NON_FIELD_REGION))
				object->trueRegion.right -= 30;
		}

		// Initialize the scroll information.
		UI_EVENT sEvent(S_SCROLLRANGE);
		UIW_WINDOW::ScrollEvent(sEvent);

		XtManageChild(screenID);
		}
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
		if (comboShell &&
			(ccode == L_END_SELECT || ccode == L_SELECT))
		{
			if (current)
				parent->Information(I_RESET_SELECTION, current);
			current->Event(UI_EVENT(S_CURRENT));
			parent->Event(UI_EVENT(L_CANCEL));	// Popdown menu.
			parent->woStatus |= WOS_CHANGED;
		}
		if (current)
		{
			// Determine if button release occurred in current child.
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
				if (ccode == L_END_SELECT && !comboShell)
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
		
	case L_CANCEL:
		if (comboShell)
			parent->Event(event);
		break;
		
	case L_DOWN:
		if (comboShell)
		{
			EVENT_TYPE tcode = ccode;
			ccode = S_UNKNOWN;
			// Make sure there is a current object.
			if (!current)
			{
				for (object = First(); object && FlagSet(object->woFlags, WOF_NON_SELECTABLE); object = object->Next())
					;
				if (object)
				{
					UIW_WINDOW::Add(object);
					parent->Information(I_RESET_SELECTION, object);
				}
			}
			// Don't move if on the last field of a non-wrapping window.
			else if (!Current()->Next() && FlagSet(wnFlags, WNF_NO_WRAP))
				;
			// Go to the next field in the window.
			else
			{
				object = Current()->Next() ? Current()->Next() : First();
				while (object && object != current && ccode == S_UNKNOWN)
					if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
						!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
					{
						wnFlags |= WNF_MOVE_FORWARD;
						UIW_WINDOW::Add(object);
						parent->Information(I_RESET_SELECTION, object);
						wnFlags &= ~WNF_MOVE_FORWARD;
						parent->woStatus |= WOS_CHANGED;
						ccode = tcode;
					}
					else if (object->Next())
						object = object->Next();
					else if (FlagSet(wnFlags, WNF_NO_WRAP))
						break;
					else
						object = First();
				if (FlagSet(Current()->woFlags, WOF_NON_SELECTABLE))
					current = ZIL_NULLP(UI_WINDOW_OBJECT);
			}
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	
	case L_UP:
		if (comboShell)
		{
			EVENT_TYPE tcode = ccode;
			ccode = S_UNKNOWN;
			// Make sure there is a current object.
			if (!current)
			{
				UIW_WINDOW::Add(Last());
				parent->Information(I_RESET_SELECTION, Last());
			}
			// Don't move if on the first field of a non-wrapping window.
			else if (!Current()->Previous() && FlagSet(wnFlags, WNF_NO_WRAP))
				break;
			// Go to the previous field in the window.
			else
			{
				object = Current()->Previous() ? Current()->Previous() : Last();
				while (object && object != current && ccode == S_UNKNOWN)
					if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
						!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
					{
						wnFlags |= WNF_MOVE_REVERSE;
						UIW_WINDOW::Add(object);
						parent->Information(I_RESET_SELECTION, object);
						wnFlags &= ~WNF_MOVE_REVERSE;
						parent->woStatus |= WOS_CHANGED;
						ccode = tcode;
					}
					else if (object->Previous())
						object = object->Previous();
					else if (FlagSet(wnFlags, WNF_NO_WRAP))
						break;
					else
						object = Last();
				if (FlagSet(Current()->woFlags, WOF_NON_SELECTABLE))
					current = ZIL_NULLP(UI_WINDOW_OBJECT);
			}
		}
		break;

		case S_DROP_DEFAULT:
		case S_DROP_MOVE_OBJECT:
			if (windowManager->dragObject == this ||
				windowManager->dragObject->parent == this)
					break;
			else if (windowManager->dragObject->Inherited(ID_LIST))
			{
				UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
				UI_WINDOW_OBJECT *nextObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				WNF_FLAGS twnFlags;
				windowManager->dragObject->Information(I_GET_FLAGS, &twnFlags, ID_WINDOW);
				for (windowManager->dragObject->Information(I_GET_FIRST, &object);
					object; object = nextObject)
				{
					nextObject = object->Next();
					if (FlagSet(object->woStatus, WOS_SELECTED))
					{
						UI_EVENT dEvent(S_SUBTRACT_OBJECT);
						dEvent.windowObject = object;
						windowManager->dragObject->Event(dEvent);
						object->woStatus &= ~WOS_CURRENT;
						dEvent.type = S_ADD_OBJECT;
						Event(dEvent);
						if (!FlagSet(twnFlags, WNF_SELECT_MULTIPLE))
							break;
					}
				}
				windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
			}
			else if (windowManager->dragObject->parent &&
				windowManager->dragObject->parent->Inherited(ID_LIST) &&
				windowManager->dragObject->parent != this)
			{
				UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
				UI_WINDOW_OBJECT *dragParent = dragObject->parent;
				UI_EVENT dEvent(S_SUBTRACT_OBJECT);
				dEvent.windowObject = dragObject;
				if (dragParent->parent && dragParent->parent->Inherited(ID_COMBO_BOX))
				{
					dragParent->parent->Event(dEvent);
					dragParent->parent->Event(UI_EVENT(S_REDISPLAY));
				}
				else
				{
					dragParent->Event(dEvent);
					dragParent->Event(UI_EVENT(S_REDISPLAY));
				}
				dragObject->relative.right = dragObject->relative.Width() - 1;
				dragObject->relative.left = 0;
				dEvent.type = S_ADD_OBJECT;
				dEvent.windowObject->woStatus &= ~WOS_CURRENT;
				Event(dEvent);
			}
			Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
			break;

		case S_DROP_COPY_OBJECT:
			if (windowManager->dragObject == this)
				break;
#if defined(ZIL_LOAD) && defined(ZIL_STORE)
			if (windowManager->dragObject->Inherited(ID_LIST))
			{
				UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
				UI_WINDOW_OBJECT *nextObject = ZIL_NULLP(UI_WINDOW_OBJECT);
				for (windowManager->dragObject->Information(I_GET_FIRST, &object);
					object; object = nextObject)
				{
					nextObject = object->Next();
					if (FlagSet(object->woStatus, WOS_SELECTED))
					{
						static ZIL_ICHAR name[] = { 'd','r','a','g',0 };
						ZIL_NEW_FUNCTION newFunction = object->NewFunction();
						ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
						ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
						object->Store(name, &directory, &file);
						file.Seek(0);
						object = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
						if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
							object->woStatus |= WOS_SELECTED;
						UI_EVENT dEvent(S_ADD_OBJECT);
						dEvent.windowObject = object;
						Event(dEvent);
					}
				}
				windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
			}
			else
			{
				static ZIL_ICHAR name[] = { 'd','r','a','g',0 };
				UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
				ZIL_NEW_FUNCTION newFunction = dragObject->NewFunction();
				ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
				ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
				dragObject->Store(name, &directory, &file);
				file.Seek(0);
				dragObject = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
				if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					dragObject->woStatus |= WOS_SELECTED;
				dragObject->relative.right = dragObject->relative.Width() - 1;
				dragObject->relative.left = 0;
				UI_EVENT dEvent(S_ADD_OBJECT);
				dEvent.windowObject = dragObject;
				Event(dEvent);
			}
			Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
#endif
			break;

	case S_DEINITIALIZE:
		XtUnmanageChild(screenID);
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_DISPLAY_ACTIVE:
		if (event.region == trueRegion)
			Event(UI_EVENT(S_REDISPLAY));
		else 
			ccode = UIW_WINDOW::Event(event);
		break;

	case S_REDISPLAY:
		if (screenID)
		{
			// Make sure no one rearranged the list by bypassing UIW_VT_LIST,
			// going straight to UI_LIST.
			WidgetList children;
			Cardinal numChildren;
			XtVaGetValues(screenID, XmNchildren, &children,
				XmNnumChildren, &numChildren, ZIL_NULLP(void));
			int inOrder = TRUE;
			if (numChildren != UI_LIST::Count())
				inOrder = FALSE;
			UI_WINDOW_OBJECT *object = First();
			for (int i = 0; i < numChildren && inOrder && object;
				i++, object = object->Next())
			{
				if (children[i] != object->TopWidget())
					inOrder = FALSE;
			}
			if (!inOrder)
				Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
		}
		ccode = UIW_WINDOW::Event(event);
		break;

	default:
		if (vScroll && event.type == E_MOTIF && event.message.type == EnterNotify && First())
			XtVaSetValues(vScroll->screenID, XmNincrement, First()->trueRegion.Height(), ZIL_NULLP(void));
		if (!(event.type == E_MOTIF && event.message.type == KeyPress))
			ccode = UIW_WINDOW::Event(event);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_VT_LIST::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);
	object->trueRegion.right = object->trueRegion.left +
		clipList.First()->region.Width() - 7;
}

ZIL_SCREENID UIW_VT_LIST::TopWidget(void)
{
	if (comboShell)
	{
		WidgetList kids = 0;
		int n = 0;
		XtVaGetValues(comboShell, XmNchildren, &kids, XmNnumChildren, &n, NULL);
		return(kids[0]);
	}
	return UI_WINDOW_OBJECT::TopWidget();
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_VT_LIST::OSDestroy(void)
{
	if (screenID)
		UIW_WINDOW::Destroy();
}

void UIW_VT_LIST::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_VT_LIST && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}

void UIW_VT_LIST::OSSort(void)
{
	Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_VT_LIST);
}
