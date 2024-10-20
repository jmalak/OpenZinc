//	Zinc Interface Library - M_WIN1.CPP
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
#include <stdio.h>
#include <Xm/AtomMgr.h>
#include <Xm/BulletinB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Protocols.h>
#include <Xm/ScrolledW.h>
#include <Xm/ToggleB.h>
#if defined(__DVX__)
	// Include for access to core...visible_interest.
	struct _XtStateRec;
#	include <X11/IntrinsP.h>
#else
#	if defined(__QNX__)
	struct _XtStateRec;
#	endif
#	include <X11/IntrinsicP.h>
#endif
}

const int motifBorderWidth = 6;
const int motifTitleHeight = 19;

// A note about trueRegion and relative for shell windows:
// X clients (Zinc programs) have no way of knowing about the size of the
// decorations the window manager will put on them.  The Zinc relative region
// is supposed to include the decorations, but Motif won't tell us how big
// the decorations are, so we kind of guess.  We figure the size of trueRegion to
// be relative minus constants for border width and height and title bar
// height.  If the user changes Motif's decoration's sizes, the client
// area will stay the same size.

// True is the region I ask motif for, not the client area or relative.
// It is the same height and width as the client area, but its left and top
// are the same as relative's.
// Motif creates a window the height and width of trueRegion, then moves it down
// by about borderheight and about titlebar height, and to the right by
// about borderwidth, and then adds decorations around trueRegion.
// When mwm is done, the corner of the window's decorations is at trueRegion.left,
// trueRegion.top.
// Because I use constants for decoration widths, relative is just a guess,
// rather than the actual size and location of the shell, including decorations.
// trueRegion.left,top == relative.left,top, but right and bottom don't match.
// When I query motif for the size and location of shell, I can compute trueRegion
// by shifing the answer up and left.
// For child windows, trueRegion is relative to the screen, and relative is relative
// to the parent.
// True will never be set to relative for a shell.

// A note about shell positioning.
//	Application shells:
//		Initial position is determined only by XmNgeomtry, not XmNx, XmNy.
//		To move the shell, set XmN[xy] of the shell, not the child.
//	Dialog shells:
//		Dialogs can be placed by XmNgeometry or by the XmN[xy] of the
//			BulletinBoard (form) child, with the latter method overriding the
//			former.

void ShellCallback(Widget, XtPointer client_data, XEvent *xEvent, Boolean *)
{
	UIW_WINDOW *object = (UIW_WINDOW *)client_data;
	if (xEvent->type == ConfigureNotify)
	{
		object->Event(UI_EVENT(E_MOTIF, *xEvent));
	}
	else if (xEvent->type == UnmapNotify)
	{
		// Assume UnmapNotify happens only upon iconization.
		object->woStatus |= WOS_MINIMIZED;
	}
	else if (xEvent->type == VisibilityNotify)
	{
		//  Maybe restored from icon.
		object->woStatus &= ~WOS_MINIMIZED;
	}
	else if (xEvent->type == FocusIn)
	{
		// Assume FocusIn happens on a window becoming current.
		object->woStatus |= WOS_CURRENT;
	}
	else if (xEvent->type == FocusOut)
	{
		// Assume FocusOut happens on a window becoming non-current.
		object->woStatus &= ~WOS_CURRENT;
	}
}

#if (ZIL_MOTIF > 1001)
void ScrollObscuredCallback(Widget scroll, XtPointer, XtPointer call)
{
	XmTraverseObscuredCallbackStruct *structPtr =
		(XmTraverseObscuredCallbackStruct *)call;

	XmScrollVisible(scroll, structPtr->traversal_destination, 3, 3);
}
#endif
	

// ----- UIW_WINDOW ---------------------------------------------------------

UI_WINDOW_OBJECT *UIW_WINDOW::Add(UI_WINDOW_OBJECT *object)
{
	int newObject = FALSE;

	// See if the new object exists or is already current.
	if (!object)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
	else if (object == current)
		return (object);

	// Add the object to the list.
	if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		support.Index(object) == -1)
	{
		support.Add(object);
		object->parent = this;
		newObject = TRUE;
	}
	else if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		((!object->Previous() && !object->Next() && object != First()) ||
		 UI_LIST::Index(object) == -1))
	{
		newObject = TRUE;
		object->parent = this;
		UI_LIST::Add(object);
	}
	else if (!(FlagSet(object->woFlags, WOF_NON_SELECTABLE) ||
		FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
	{
		// Place MDI objects at the end of the list.
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			UI_LIST::Subtract(object);
			UI_LIST::Add(object);
		}

		UI_EVENT event;
		event.type = S_NON_CURRENT;
		if (current)
		{
			CheckSelection();
			Current()->Event(event);
		}

		current = object;
		event.type = S_CURRENT;
		if (!(object->Inherited(ID_MENU_ITEM)))
			object->Event(event);

		if (screenID && !FlagSet(object->woStatus, WOS_INTERNAL_ACTION) &&
			FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			// Pop the window to the top of the window stack.
			XRaiseWindow(display->xDisplay, XtWindow(object->shell));
		}

		if (Inherited(ID_COMBO_BOX))
			parent->Information(I_RESET_SELECTION, object);
		else if (object->screenID && FlagSet(wnFlags, WNF_AUTO_SELECT) &&
			!FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
			object->Event(L_SELECT);
	}

	// Initialize the object if its parent is already shown on the screen.
	if (screenID && !object->screenID && newObject)
	{
		if (object->Inherited(ID_SCROLL_BAR) && Inherited(ID_COMBO_BOX))
			return(object);
		else if (object->Inherited(ID_SCROLL_BAR) &&
			FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
		{
			Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
			return(object);
		}
		object->Event(UI_EVENT(S_INITIALIZE, 0));
		object->Event(UI_EVENT(S_CREATE, 0));
		if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
		if (!current && !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
			!FlagSet(object->woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT))
		{
			current = object;
			object->woStatus |= WOS_CURRENT;
		}
	}

	// Return a pointer to the object.
	return (object);
}


EVENT_TYPE UIW_WINDOW::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event; (void)ccode;

	if (screenID && !Inherited(ID_COMBO_BOX))
		XClearArea(display->xDisplay, XtWindow(screenID), 0, 0, 0, 0, FALSE);
	return (FALSE);
}


EVENT_TYPE UIW_WINDOW::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object;
				object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
		{
			if (FlagSet(object->woStatus, WOS_CURRENT) ||
				(!current && !FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
				 !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
			{
				if (current)
					Current()->woStatus &= ~WOS_CURRENT;
				if (current && FlagSet(wnFlags, WNF_AUTO_SELECT) &&
						!FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					Current()->woStatus &= ~WOS_SELECTED;
				current = object;
			//	object->woStatus |= WOS_CURRENT;
				if (FlagSet(wnFlags, WNF_AUTO_SELECT) &&
						!FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					object->woStatus |= WOS_SELECTED;
			}
			else
				object->woStatus &= ~WOS_CURRENT;
			object->Event(event);
		}
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		for (object = (UI_WINDOW_OBJECT *)support.First();
			object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		break;

	case S_CHANGED:
		// Resize and/or move.  Figure new trueRegion and relative from client area.
		if (event.type == E_MOTIF && event.message.type == ConfigureNotify)
		{
			if (event.message.xconfigure.x != 0
				&& event.message.xconfigure.y != 0)
			{
				// The window moved, too.
				trueRegion.left = event.message.xconfigure.x - motifBorderWidth;
				trueRegion.top = event.message.xconfigure.y - motifBorderWidth -
					motifTitleHeight;
			}
			trueRegion.right = trueRegion.left + event.message.xconfigure.width - 1;
			trueRegion.bottom = trueRegion.top + event.message.xconfigure.height - 1;

			relative = trueRegion;
			relative.bottom += 2 * motifBorderWidth + motifTitleHeight;
			relative.right += 2 * motifBorderWidth;

			if (event.message.xconfigure.send_event == 1
				&& !FlagSet(woStatus, WOS_EDIT_MODE))
				// send_event == 1 for very first event after creation.
				// Don't re-execute the code because we just did in S_CREATE.
				// Note: we may not be able to count on this in the future.
				return(S_UNKNOWN);

			if (parent)
			{
				Position motifX, motifY;
				XtTranslateCoords(parent->screenID, 0, 0, &motifX, &motifY);
				relative.left -= motifX;
				relative.top -= motifY;
				relative.right -= motifX;
				relative.bottom -= motifY;
			}

			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				UI_EVENT dEvent;
				dEvent.type = D_SET_OBJECT;
				dEvent.data = this;
				eventManager->Put(dEvent);
			}

			if ((vScroll || hScroll) && !FlagSet(woStatus, WOS_OWNERDRAW))
				break;

			// This is a ConfigureNotify Event.  Route an event thru the queue.
			UI_EVENT rEvent(S_ROUTE_EVENT);
			rEvent.event = new UI_EVENT(S_CHANGED);
			rEvent.routeTarget = this;
			eventManager->Put(rEvent);
			break;
		}
		else
		{
			if (parent && (!IsMoveable() || !IsSizeable()))
				break;
			// This is the event that went thru the queue, after Motif finished.
			if (!Inherited(ID_TOOL_BAR))
				UI_WINDOW_OBJECT::Event(event);
			clipList.Destroy();
			if (parent && supportDecorations)
			{
				trueRegion.right -= 2 * motifBorderWidth;
				trueRegion.bottom -= 2 * motifBorderWidth + motifTitleHeight;
			}
			if (supportDecorations || FlagSet(woFlags, WOF_BORDER))
			{
				UI_REGION region;
				region.left = 0; region.top = 0;
				region.right = trueRegion.Width() - 2;
				region.bottom = trueRegion.Height() - 2;
				--region;
				if (Inherited(ID_TOOL_BAR))
					--region;
				clipList.Add(new UI_REGION_ELEMENT(screenID, region));
			}
	
			for (object = (UI_WINDOW_OBJECT *)support.First(); object;
				object = object->Next())
			{
				object->Event(UI_EVENT(S_CHANGED, 0));
				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion,
						parent ? FALSE : TRUE);
				if (screenID && (object->Inherited(ID_TOOL_BAR) ||
						object->Inherited(ID_STATUS_BAR)))
					XtConfigureWidget(object->TopWidget(),
						object->trueRegion.left, object->trueRegion.top,
						object->trueRegion.Width(),
						object->trueRegion.Height(), 0);
			}
			for (object = First(); object; object = object->Next())
			{
				UI_REGION region = object->trueRegion;
				object->Event(UI_EVENT(S_CHANGED));
				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);

				if (object->Inherited(ID_TABLE) ||
					FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
					continue;

				if (object->trueRegion != region && screenID)
				{
					XtConfigureWidget(object->TopWidget(),
						MaxValue(object->trueRegion.left,0), MaxValue(object->trueRegion.top,0),
						MaxValue(object->trueRegion.Width(),1),
						MaxValue(object->trueRegion.Height(),1), 0);
				}
	
				if (FlagSet(object->woStatus, WOS_OWNERDRAW) && object->screenID &&
					XtWindow(object->screenID))
				{
					XClearArea(display->xDisplay, XtWindow(object->screenID),
						0, 0, 0, 0, TRUE);
				}
			}
	
			if (!FlagSet(wnFlags, WNF_NO_SCROLL))
			{
				UI_EVENT range(S_SCROLLRANGE);
				ScrollEvent(range);
			}
	
			// Enforce positioning only on shells that are MDI children.
//			if (!(supportDecorations && parent &&
//				FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT)))
//			{
//				UI_WINDOW_OBJECT::Event(event);
//				break;
//			}
		}

		if ((!parent && (vScroll || hScroll)) || Inherited(ID_NOTEBOOK) ||
			(parent && !supportDecorations))
			break;
		//Fall thru.

	case S_CREATE:
		if (!parent && !supportDecorations && ccode == S_CREATE)
		{
			// A parent shell must have some decorations.  Invent some.
			UIW_SYSTEM_BUTTON *systemButton = UIW_SYSTEM_BUTTON::Generic();
			UIW_WINDOW::Add(systemButton);
			systemButton->Event(UI_EVENT(S_INITIALIZE));
			systemButton->Event(UI_EVENT(S_CREATE));
			UIW_BORDER *border = new UIW_BORDER;
			UIW_WINDOW::Add(border);
			border->Event(UI_EVENT(S_INITIALIZE));
			border->Event(UI_EVENT(S_CREATE));
		}

		if (ccode == S_CREATE && !Inherited(ID_TOOL_BAR))
			ccode = UI_WINDOW_OBJECT::Event(event);
		// Fall thru.

	case S_REGISTER_OBJECT:
		{

		if (supportDecorations)
		{
			// Create a new app shell.

			trueRegion = relative;
			trueRegion.right -= 2 * motifBorderWidth;
			trueRegion.bottom -= 2 * motifBorderWidth + motifTitleHeight;

			UI_REGION motifRegion = trueRegion;

			nargs = 0;

			//	Geometry static because some Xs seem to trash it.
			static char geometry[50];
		
			if (parent)
			{
				// Place child shell according to current parent position.
				if (parent->shell && !XtIsRealized(parent->shell))
					XtRealizeWidget(parent->shell);

				// Place this shell "in" it's parent's shell.
				int xOffset = motifBorderWidth;
				int yOffset = motifBorderWidth + motifTitleHeight;
				for (UI_WINDOW_OBJECT *p = parent; p; p = p->parent)
				{
					xOffset += p->trueRegion.left;
					yOffset += p->trueRegion.top;

//	To fix the bug that moves MDI children if the parent is moved, figure out
//	how to execute the following code on all S_CHANGED except the first one
//	after the S_CREATE.
					if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
					{
						UI_REGION *pr =
							&((UIW_WINDOW*)p)->clipList.First()->region;
						xOffset += pr->left;
						yOffset += pr->top;
					}
				}
				motifRegion.left += xOffset;
				motifRegion.top += yOffset;
				motifRegion.right += xOffset;
				motifRegion.bottom += yOffset;
				trueRegion = motifRegion;
			}
			if (!screenID)
			{
				::sprintf(geometry, "+%d+%d",
					motifRegion.left, motifRegion.top);
				XtSetArg(args[nargs], XmNgeometry, geometry); nargs++;
			}
			else
			{
				break;
			}

			XtSetArg(args[nargs], XmNdeleteResponse, XmDO_NOTHING); nargs++;
			XtSetArg(args[nargs], XmNiconic,
				FlagSet(woStatus, WOS_MINIMIZED) ? TRUE : FALSE); nargs++;

			// Since mwm won't let us start up maximized, clear the status.
			woStatus &= ~WOS_MAXIMIZED;

			if (parent)
			{
				RegisterObject(ZIL_NULLP(_WidgetClassRec),
					(ZIL_MOTIF_CONVENIENCE_FUNCTION)XmCreateDialogShell,
					ccode, TRUE, FALSE, parent->shell);
				shell = screenID;
			}
			else 	// No parent; make a new app context.
			{
				shell = XtAppCreateShell(ZIL_NULLP(char), display->appClass,
					applicationShellWidgetClass,
					display->xDisplay, args, nargs);
				screenID = shell;
			}

			// Tell Motif to call ExitShell when user closes window.
			Atom deleteWindowAtom = XmInternAtom(display->xDisplay,
				"WM_DELETE_WINDOW", FALSE);
#if defined(_IBM_RS6000) || defined(__DVX__) || defined(__DECCXX)
			XmAddWMProtocolCallback(shell, deleteWindowAtom,
				(XtCallbackProc)ExitShell,
				(caddr_t)this);
#else
			XmAddWMProtocolCallback(shell, deleteWindowAtom,
				(XtCallbackProc)ExitShell,
				(XtPointer)this);
#endif

			// Add callbacks for shell size and position changes.
			EventMask mask = VisibilityChangeMask | StructureNotifyMask | FocusChangeMask;
			// Ask for info on visibility changes (restore from icon).
			shell->core.widget_class->core_class.visible_interest = TRUE;
			if (!parent || shell != parent->shell)
				XtAddEventHandler(shell, mask, FALSE, ShellCallback,
					(XtPointer)this);
		}

		// Create a scrolled window, but not on an MDI parent.
		if (vScroll || hScroll)
		{
			if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
				vScroll = hScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
			else
			{
				nargs = 0;
				if (FlagSet(woStatus, WOS_OWNERDRAW))
				{
					// XmAPPLICATION_DEFINED scrolling.
					XtSetArg(args[nargs], XmNscrollingPolicy,
						XmAPPLICATION_DEFINED); nargs++;
					XtSetArg(args[nargs], XmNscrollBarDisplayPolicy, XmSTATIC);
						nargs++;
					RegisterObject(ZIL_NULLP(_WidgetClassRec),
						XmCreateScrolledWindow, ccode, TRUE, TRUE, screenID);
				}
				else
				{
					// XmAUTOMATIC scrolling.
					XtSetArg(args[nargs], XmNscrollingPolicy, XmAUTOMATIC); nargs++;
					XtSetArg(args[nargs], XmNscrollBarDisplayPolicy, XmSTATIC);
						nargs++;
					RegisterObject(ZIL_NULLP(_WidgetClassRec),
						XmCreateScrolledWindow, ccode, TRUE, TRUE, screenID);
#	if (ZIL_MOTIF > 1001)
					if (supportDecorations)
					{
						ZIL_SCREENID frameID = screenID;
						while (!XmIsScrolledWindow(frameID) && XtParent(frameID))
							frameID = XtParent(frameID);
						XtAddCallback(frameID, XmNtraverseObscuredCallback,
							ScrollObscuredCallback, (XtPointer)this);
					}
#	endif
		
					// remove unneeded scroll bars
					Widget vertScroll = ZIL_NULLP(_WidgetRec),
						horizScroll = ZIL_NULLP(_WidgetRec);
					nargs = 0;
					XtSetArg(args[nargs], XmNhorizontalScrollBar, &horizScroll);
						nargs++;
					XtSetArg(args[nargs], XmNverticalScrollBar, &vertScroll);
						nargs++;
					XtGetValues(screenID, args, nargs);
					if (hScroll)
					{
						XtVaSetValues(horizScroll, XmNtraversalOn, FALSE, NULL);
						hScroll->screenID = horizScroll;
					}
					else
						XtUnmanageChild(horizScroll);
					if (vScroll)
					{
						XtVaSetValues(vertScroll, XmNtraversalOn, FALSE, NULL);
						vScroll->screenID = vertScroll;
					}
					else
						XtUnmanageChild(vertScroll);
				}
			}
		}

		// create the Form for the window
		nargs = 0;
		if (FlagSet(woFlags, WOF_BORDER))
		{
			XtSetArg(args[nargs], XmNshadowThickness, 2), nargs++;
			if (parent)
				XtSetArg(args[nargs], XmNshadowType, XmSHADOW_IN), nargs++;
			else
				XtSetArg(args[nargs], XmNshadowType, XmSHADOW_ETCHED_IN),
					nargs++;
		}
	
		
		XtSetArg(args[nargs], XmNautoUnmanage, FALSE); nargs++;
		XtSetArg(args[nargs], XmNdefaultPosition, FALSE); nargs++;
		XtSetArg(args[nargs], XmNmarginWidth, 0); nargs++;
		XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
		XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;

		UI_REGION region = trueRegion;
		if ((vScroll || hScroll) && !FlagSet(woStatus, WOS_OWNERDRAW))
		{
			trueRegion.left = trueRegion.top = 0;
			trueRegion.bottom = hScrollInfo.maximum;
			trueRegion.right = vScrollInfo.maximum;
		}
			
		RegisterObject(supportDecorations ? xmFormWidgetClass : xmBulletinBoardWidgetClass,
			       ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
					  ccode, TRUE, TRUE, screenID);
		if (vScroll || hScroll)
		{
			trueRegion = region;

			// hook form to scrolled win
			nargs = 0;
			XtSetArg(args[nargs], XmNworkWindow, screenID); nargs++;
			XtSetValues(XtParent(XtParent(screenID)), args, nargs);
			if (ccode != S_CHANGED)
			{
				UI_EVENT tmp(S_SCROLLRANGE);
				ScrollEvent(tmp);
			}
		}
		}

		// add the decorations, like borders, etc
		if (supportDecorations)
		{
			nargs = 0;
			XtSetArg(args[nargs], XmNmwmDecorations, supportDecorations);
				nargs++;
			XtSetValues(shell, args, nargs);
		}

		if (ccode == S_REGISTER_OBJECT)
			break;

		clipList.Destroy();
		if (FlagSet(woFlags, WOF_BORDER))
		{
			UI_REGION region;
			region.left = 0; region.top = 0;
			region.right = trueRegion.Width() - 1;
			region.bottom = trueRegion.Height() - 1;
			region -= 2;
			clipList.Add(new UI_REGION_ELEMENT(screenID, region));
		}

		for (object = (UI_WINDOW_OBJECT *)support.First(); object;
				object = object->Next())
		{
			if (FlagSet(woStatus, WOS_WINDOWS_ACTION))
				object->woStatus |= WOS_WINDOWS_ACTION;
			object->Event(event);
			if (FlagSet(woStatus, WOS_WINDOWS_ACTION))
				object->woStatus &= ~WOS_WINDOWS_ACTION;
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
		}

		for (object = First(); object; object = object->Next())
		{
			if (FlagSet(woStatus, WOS_WINDOWS_ACTION))
				object->woStatus |= WOS_WINDOWS_ACTION;
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
			if (FlagSet(woStatus, WOS_WINDOWS_ACTION))
				object->woStatus &= ~WOS_WINDOWS_ACTION;
		}

		if ((vScroll || hScroll) && !FlagSet(woStatus, WOS_OWNERDRAW))
		{
			XtVaSetValues(screenID, XmNwidth, (Dimension)scroll.Width(),
				XmNheight, (Dimension)scroll.Height(), NULL);
		}

		if (FlagSet(woAdvancedFlags, WOAF_MODAL))
			XtAddGrab(shell, TRUE, FALSE);

		if (supportDecorations)
			XtRealizeWidget(shell);

		if (current)	// Now that kids are managed, traverse to Current().
			Current()->Event(UI_EVENT(S_CURRENT, 0));
		break;


	case S_NON_CURRENT:
	case S_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (defaultObject && ccode == S_CURRENT)
			Root(TRUE)->Information(I_SET_DEFAULT_OBJECT, defaultObject);
		else if (ccode == S_CURRENT)
			Root(TRUE)->Information(I_SET_DEFAULT_OBJECT, ZIL_NULLP(UI_WINDOW_OBJECT));
		if (current)
			ccode = Current()->Event(event);
		break;

	case S_ADD_OBJECT:
		object = event.windowObject;
#if defined(ZIL_EDIT)
		if (UI_LIST::Index(object) == -1 && FlagSet(object->woStatus, WOS_EDIT_MODE))
		{
			UI_REGION region;
			if (clipList.First())
				region = clipList.First()->region;
			else
			{
				region.left = region.top = 0;
				region.right = relative.right - relative.left;
				region.bottom = relative.bottom - relative.top;
			}
			if (!FlagSet(object->woStatus, WOS_GRAPHICS))
			{
				region.left /= display->cellWidth;
				region.top /= display->cellHeight;
			}
			object->relative.left -= region.left;
			object->relative.top -= region.top;
			object->relative.right -= region.left;
			object->relative.bottom -= region.top;
			Add(object);
		}
#endif
		Add(object);
		break;

	case S_SUBTRACT_OBJECT:
		Subtract(event.windowObject);
		break;

	case S_DEINITIALIZE:
		{
		ZIL_SCREENID tScreenID = TopWidget();
		ZIL_SCREENID tShell = shell;

		woStatus |= WOS_INTERNAL_ACTION;
		UI_WINDOW_OBJECT::Event(event);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object;
			object = object->Next())
		{
			// Set internal action flag so widget isn't destroyed yet.
			object->woStatus |= WOS_INTERNAL_ACTION;
			object->Event(event);
			object->woStatus &= ~WOS_INTERNAL_ACTION;
		}
		for (object = First(); object; object = object->Next())
		{
			// Set internal action flag so widget isn't destroyed yet.
			object->woStatus |= WOS_INTERNAL_ACTION;
			ZIL_SCREENID cShell = 0;
			if (object->shell != tShell)
				cShell = object->shell;
			object->Event(event);
			if (cShell)
			{
				XtUnrealizeWidget(cShell);
				XtDestroyWidget(cShell);	// Kill child shell.
			}
			object->woStatus &= ~WOS_INTERNAL_ACTION;
		}
		woStatus &= ~WOS_INTERNAL_ACTION;

		if (!tScreenID
			|| (parent && FlagSet(parent->woStatus, WOS_INTERNAL_ACTION)))
			break;
	
		XtDestroyWidget(tScreenID);
		}
		break;

	case S_REDISPLAY:
		for (object = (UI_WINDOW_OBJECT *)support.First(); object;
			object = object->Next())
		{
			object->Event(event);
		}
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
		}
		UI_WINDOW_OBJECT::Event(event);
		break;

	case L_BEGIN_SELECT:
		// Scrolled windows don't get a FocusIn if they don't have kids.
		if ((vScroll || hScroll) && !first && !parent)
			windowManager->Add(this);
	case L_END_SELECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_HELP:
		// See if the low level object processes the help.
		if (event.type == E_MOTIF)
			eventManager->Put(UI_EVENT(ccode, 0));
		else if (current && Current()->Event(event) != S_UNKNOWN)
			break;
		else if (parent && helpContext == NO_HELP_CONTEXT)
			ccode = S_UNKNOWN;
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;


	case S_VSCROLL:
	case S_HSCROLL:
	case S_VSCROLL_CHECK:
	case S_HSCROLL_CHECK:
		{
		UI_EVENT sEvent = event;
		ccode = ScrollEvent(sEvent);
		}
		break;

	case L_UP:
		if (screenID)
			XmProcessTraversal(screenID, XmTRAVERSE_UP);
		break;

	case L_DOWN:
		if (screenID)
			XmProcessTraversal(screenID, XmTRAVERSE_DOWN);
		break;

	case L_LEFT:
		if (screenID)
			XmProcessTraversal(screenID, XmTRAVERSE_LEFT);
		break;

	case L_RIGHT:
		if (screenID)
			XmProcessTraversal(screenID, XmTRAVERSE_RIGHT);
		break;

	case L_PREVIOUS:
		if (screenID)
			XmProcessTraversal(screenID, XmTRAVERSE_PREV);
		break;

	case L_NEXT:
		if (screenID)
			XmProcessTraversal(screenID, XmTRAVERSE_NEXT);
		break;

	case L_CANCEL:
		break;

	case S_MINIMIZE:
		XIconifyWindow(UI_WINDOW_OBJECT::display->xDisplay, XtWindow(shell),
			UI_WINDOW_OBJECT::display->xScreenNumber);
		break;

	case S_RESTORE:
		XtMapWidget(shell);
		break;

	case L_SELECT:
		if (defaultObject)
			ccode = defaultObject->Event(UI_EVENT(L_SELECT));
		else
			ccode = S_UNKNOWN;
		break;

	case S_CLOSE + S_MDICHILD_EVENT:
		object = Last();
		Subtract(object);
		if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
			delete object;
		break;

	default:
		if (ccode == E_MOTIF)
		{
			if (event.message.type == KeyPress &&
				(FlagSet(event.key.shiftState, S_ALT) ||
				FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS)))
			{
				// What would it be without the alt key pressed?
				KeySym unmodifiedKey = XKeycodeToKeysym(display->xDisplay,
					XKeysymToKeycode(display->xDisplay, event.rawCode), 0);
				char *pHotKey = XKeysymToString(unmodifiedKey);
				if (::strlen(pHotKey) == 1)
				{
					ZIL_UINT8 tHotKey = *pHotKey;
					for (object = (UI_WINDOW_OBJECT *)support.First(); object;
							object = object->Next())
						if (object->hotKey == HOT_KEY_SUB_WINDOW &&
							object->Event(event) != S_UNKNOWN)
						{
							if (object != current)
							{
								object->userObject = current;
								UIW_WINDOW::Add(object);
							}
							return (ccode);
						}
						else if (ToLower(object->hotKey) == tHotKey &&
							!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
						{
							ccode = object->Event(UI_EVENT(L_SELECT));
							CheckSelection();
							return (ccode);
						}
					if (current && Inherited(ID_NOTEBOOK))
						return (Current()->Event(event));
					for (object = First(); object; object = object->Next())
						if (object->hotKey == HOT_KEY_SUB_WINDOW &&
							object->Event(event) != S_UNKNOWN)
						{
							UIW_WINDOW::Add(object);
							return (ccode);
						}
						else if (ToLower(object->hotKey) == tHotKey &&
							!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
						{
							// Key is hot key; delete it from the current item.
							if (Current() && Current()->Inherited(ID_STRING))
							{
								UI_EVENT evt(S_VERIFY_STATUS,
									(ZIL_RAW_CODE)Current()->screenID);
								Current()->Event(evt);
							}
							UIW_WINDOW::Add(object);
							ccode = object->Event(UI_EVENT(L_SELECT));
							CheckSelection();
							return (ccode);
						}
					return(S_UNKNOWN);
				}
			}
			else if (event.message.type != KeyPress)
				ccode = UI_WINDOW_OBJECT::Event(event);
		}
		else
			ccode = (current && event.type != E_MOTIF) ?
				Current()->Event(event) : UI_WINDOW_OBJECT::Event(event);
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_WINDOW::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch(ccode)
	{

	case S_SCROLLRANGE:
		{
		UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
		int minx = 32767, miny = 32767, maxx = 0, maxy = 0;
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			{
				minx = MinValue(minx, object->relative.left);
				miny = MinValue(miny, object->relative.top);
				maxx = MaxValue(maxx, object->relative.right);
				maxy = MaxValue(maxy, object->relative.bottom);
			}
		scroll.right = scroll.left + minx + (maxx - minx) + display->cellWidth;
		scroll.bottom = scroll.top + miny + (maxy - miny) + display->cellHeight;
		scroll.right = MaxValue(scroll.right, trueRegion.Width() - 1);
		scroll.bottom = MaxValue(scroll.bottom, trueRegion.Height() - 1);

		vScrollInfo.minimum = hScrollInfo.minimum = 0;
		vScrollInfo.maximum = scroll.Height();
		hScrollInfo.maximum = scroll.Width();

		vScrollInfo.delta = hScrollInfo.delta = display->cellHeight;
		vScrollInfo.showing = client.Height();
		hScrollInfo.showing = client.Width();

		event.type = S_VSCROLL_SET;
		event.scroll = vScrollInfo;
		ScrollEvent(event);
		event.type = S_HSCROLL_SET;
		event.scroll = hScrollInfo;
		ScrollEvent(event);
		}
		break;

	case S_VSCROLL_SET:
		// Set the scroll range and thumb position.
		vScrollInfo = event.scroll;
		if (vScroll)
			vScroll->Event(event);
		break;

	case S_HSCROLL_SET:
		// Set the scroll range and thumb position.
		hScrollInfo = event.scroll;
		if (hScroll)
			hScroll->Event(event);
		break;

	case S_VSCROLL:
	case S_HSCROLL:
		{
		int delta = event.scroll.delta;
		UI_SCROLL_INFORMATION sInfo = ccode == S_VSCROLL ? vScrollInfo : hScrollInfo;
		if ((sInfo.current == sInfo.minimum && delta <= 0) ||
			(sInfo.current == sInfo.maximum && delta >= 0))
			return ccode;
		sInfo.current += delta;
		if (sInfo.current < sInfo.minimum)
		{
			delta -= (sInfo.current - sInfo.minimum);
			sInfo.current = sInfo.minimum;
		}
		else if (sInfo.current > sInfo.maximum)
		{
			delta -= (sInfo.current - sInfo.maximum);
			sInfo.current = sInfo.maximum;
		}

		// Update scroll information and thumb button.
		event.scroll = sInfo;
		event.scroll.delta = delta;
		if (ccode == S_VSCROLL && delta)
		{
			vScrollInfo = sInfo;
			if (vScroll)
			{
				vScroll->Event(event);
				XtVaSetValues(vScroll->screenID,
					XmNvalue, (int)vScrollInfo.current, ZIL_NULLP(void));
			}
			event.type = S_VSCROLL_WINDOW;
			ScrollEvent(event);
		}
		else if (ccode == S_HSCROLL && delta)
		{
			hScrollInfo = sInfo;
			if (hScroll)
			{
				hScroll->Event(event);
				XtVaSetValues(hScroll->screenID,
					XmNvalue, (int)hScrollInfo.current, ZIL_NULLP(void));
			}
			event.type = S_HSCROLL_WINDOW;
			ScrollEvent(event);
		}
		}
		break;

	case S_VSCROLL_WINDOW:
	case S_HSCROLL_WINDOW:
		if (!FlagSet(wnFlags, WNF_NO_SCROLL))
		{
			if (ccode == S_VSCROLL_WINDOW)
			{
				scroll.top -= event.scroll.delta;
				scroll.bottom -= event.scroll.delta;
			}
	 		else
			{
				scroll.left -= event.scroll.delta;
				scroll.right -= event.scroll.delta;
			}
			if (FlagSet(woStatus, WOS_OWNERDRAW))
			{
				Event(UI_EVENT(S_REDISPLAY));
			}
		}
		break;

	case S_VSCROLL_CHECK:
	case S_HSCROLL_CHECK:
#if ZIL_MOTIF >= 1002
		if (screenID && current && Current()->screenID &&
			XtIsSubclass(XtParent(screenID), xmScrolledWindowWidgetClass))
		{
			XmScrollVisible(XtParent(screenID), Current()->TopWidget(), 3, 3);
		}
#endif
		break;

	default:
		ccode = S_UNKNOWN;
	}
	return ccode;
}

void UIW_WINDOW::RegionMax(UI_WINDOW_OBJECT *object)
{
	// Check to see if it is the top level object.
	if (object == this && (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
	{
		clipList.Destroy();
		clip = trueRegion;
		return;
	}

	// Find the appropriate region element.
	UI_REGION region;
	UI_REGION_ELEMENT *element = clipList.First();
	if (element)						// Get first available region.
		region = element->region;
	else								// Determine the update region.
	{
		region = trueRegion;
		clipList.Add(new UI_REGION_ELEMENT(screenID, 1, 1,
			region.Width()-3, region.Height()-3));
		element = clipList.First();
		region = element->region;
	}
int bottomRegion = FlagsSet(object->woFlags, WOF_NON_FIELD_REGION | WOF_JUSTIFY_RIGHT);
	for ( ; element; element = element->Next())
	{
		if (!bottomRegion && region.top > element->region.top)
			region = element->region;
		else if (bottomRegion && region.bottom < element->region.bottom)
			region = element->region;
	}

	// Compute the actual field region.
	object->clip = region;
	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION) ||
		FlagSet(object->woStatus, WOS_MAXIMIZED))
	{
		object->trueRegion = region;
		object->relative.right = object->relative.right - object->relative.left;
		object->relative.bottom = object->relative.bottom - object->relative.top;
		object->relative.left = object->relative.top = 0;
	}
	else
	{
		object->trueRegion.left = region.left + scroll.left + object->relative.left;
		object->trueRegion.top = region.top + scroll.top + object->relative.top;
		object->trueRegion.right = region.left + scroll.left + object->relative.right;
		object->trueRegion.bottom = region.top + scroll.top + object->relative.bottom;
	}
}

UI_WINDOW_OBJECT *UIW_WINDOW::Subtract(UI_WINDOW_OBJECT *object)
{
	UI_WINDOW_OBJECT *nextObject =
		object ? object->Next() : ZIL_NULLP(UI_WINDOW_OBJECT);

	// Remove the object from the list.
	if (!object)
		return (nextObject);
	UI_EVENT event;
	if (UI_LIST::Index(object) != -1)
	{
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
			;
		else if (object->screenID != screenID)
		{
			if (Inherited(ID_LIST))
				XtUnmanageChild(TopWidget());
		//		XtUnmanageChild(object->TopWidget());
			event.type = S_DEINITIALIZE;
			object->Event(event);
		}
		if (object == current)	// Check for a current support object.
		{
			current = ZIL_NULLP(UI_WINDOW_OBJECT);
			for (UI_WINDOW_OBJECT *tObject = First(); tObject; tObject = tObject->Next())
				if (!FlagSet(tObject->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(tObject->woAdvancedFlags, WOAF_NON_CURRENT) &&
					tObject != object)
				{
					Add(tObject);
					break;
				}
		}
		if (UI_LIST::Index(object) != -1)	// Make sure the object is still in the list.
			UI_LIST::Subtract(object);
	}
	else if (support.Index(object) != -1)
	{
		if (object->screenID && object->screenID != screenID)
		{
			object->woStatus |= WOS_INTERNAL_ACTION;
			event.type = S_DEINITIALIZE;
			object->Event(event);
			object->woStatus &= ~WOS_INTERNAL_ACTION;
		}
		support.Subtract(object);
		if (object == current)	// Check for a current support object.
			current = First();
		if (object == vScroll)
			vScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
		else if (object == hScroll)
			hScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
	}

	// Return a pointer to the next object.
	return (nextObject);
}

void UIW_WINDOW::ExitShell(Widget, XtPointer closure, XtPointer)
{
 	UIW_WINDOW *window = (UIW_WINDOW *)closure;
	if (FlagSet(window->woAdvancedFlags, WOAF_TEMPORARY))
		window->eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY), Q_BEGIN);
	else if (window->parent)
		// NOTE:  This doesn't free this.  Fix it later.
		((UIW_WINDOW *)window->parent)->Subtract(window);
	else
	{
		if (FlagSet(window->woStatus, WOS_MINIMIZED))
			window->windowManager->Add(window);
		window->eventManager->Put(UI_EVENT(S_CLOSE), Q_BEGIN);
	}
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_WINDOW::CheckSelection(UI_WINDOW_OBJECT *selectedObject)
{
	// Check for proper single-select status.
	if (!selectedObject)
		selectedObject = Current();
	if (!selectedObject || !FlagSet(selectedObject->woStatus, WOS_SELECTED) ||
		FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
		return;

	// Make sure other items are not selected.
	int position = 0;
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next(), position++)
	{
		if (object != selectedObject && FlagSet(object->woStatus, WOS_SELECTED))
		{
			object->woStatus &= ~WOS_SELECTED;
			if (object->screenID && object->Inherited(ID_LIST_ITEM))
			{
				object->woStatus |= WOS_CHANGED;
				object->parent->woStatus |= WOS_CHANGED;
			}
			if (object->Inherited(ID_BUTTON) && object->screenID)
			{
				BTF_FLAGS btFlags;
				object->Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
				if (FlagSet(btFlags, BTF_RADIO_BUTTON | BTF_CHECK_BOX) ||
					(!FlagSet(btFlags, BTF_NO_TOGGLE | BTF_CHECK_BOX |
					BTF_RADIO_BUTTON | BTF_NO_3D) && !Inherited(ID_MENU_ITEM)))
					XmToggleButtonSetState(object->screenID, FALSE, FALSE);
				else if (object->Inherited(ID_POP_UP_ITEM))
				{
					MNIF_FLAGS mniFlags;
					object->Information(I_GET_FLAGS, &mniFlags, ID_POP_UP_ITEM);
					if (FlagSet(mniFlags, MNIF_CHECK_MARK))
						XmToggleButtonSetState(object->screenID, FALSE, FALSE);
				}
			}
			object->Event(UI_EVENT(S_REDISPLAY));
		}
	}
}

void UIW_WINDOW::OSDestroy(void)
{
	// Remove the window objects from the system.
	if (screenID)
		XtUnmanageChild(screenID);

	// Destroy the list elements.
	UI_EVENT event(S_DEINITIALIZE);
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
		object->Event(event);
}

void UIW_WINDOW::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	supportDecorations = 0;
	// See if the window needs to be re-computed.
	if (objectID == ID_WINDOW && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

void UIW_WINDOW::OSInitialize(void)
{
	supportDecorations = 0;
	vScroll = hScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
}
