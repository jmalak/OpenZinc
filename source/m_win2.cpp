//	Zinc Interface Library - M_WIN2.CPP
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


#define USE_RAW_KEYS			// Pull in X keysyms.
#include "ui_win.hpp"

extern "C"
{
#	include <stdio.h>
#	include <stdlib.h>
#	if defined(__DVX__)
		// Include for access to core...visible_interest.
		struct _XtStateRec;
#		include <X11/IntrinsP.h>
#	else
#		if defined(__QNX__)
		struct _XtStateRec;
#		endif
#		include <X11/IntrinsicP.h>
#	endif
#	include <X11/keysymdef.h>
#	include <Xm/DrawingA.h>
#	include <Xm/Label.h>
#	include <Xm/Text.h>
#	include <Xm/ScaleP.h>
#	if (ZIL_MOTIF >= 1002)
#		include <Xm/DragDrop.h>
#		include <Xm/AtomMgr.h>
#	endif
}

#if defined(_SUNOS4)
#   include<memory.h>
#endif

extern ZIL_ICHAR *_genericFieldID;

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

Arg UI_WINDOW_OBJECT::args[50];
int UI_WINDOW_OBJECT::nargs;

EVENT_TYPE UI_WINDOW_OBJECT::DrawBorder(ZIL_SCREENID screenID, UI_REGION &region,
	int fillRegion, EVENT_TYPE ccode)
{
	return DrawShadow(screenID, region, 2, fillRegion, ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawShadow(ZIL_SCREENID screenID, UI_REGION &region,
	int depth, int fillRegion, EVENT_TYPE)
{
	struct {
		Pixel foreground;
		Pixel topShadow;
		Pixel bottomShadow;
		Pixel select;
		Pixel tmpShadow;
	} motifColors;

	Pixel background;
	XtVaGetValues(screenID, XmNbackground, &background, NULL);
	XmGetColors(display->xScreen,
		DefaultColormap(display->xDisplay, display->xScreenNumber),
		background, &motifColors.foreground, &motifColors.topShadow,
		&motifColors.bottomShadow, &motifColors.select);
	if (depth < 0)
	{
		motifColors.tmpShadow = motifColors.topShadow;
		motifColors.topShadow = motifColors.bottomShadow;
		motifColors.bottomShadow = motifColors.tmpShadow;
	}
			
	UI_PALETTE palette;
	palette.fillCharacter = ' ';
	palette.fillPattern = PTN_SYSTEM_COLOR;
	palette.colorBackground = background;
	palette.colorForeground = motifColors.foreground;
	
	display->VirtualGet(screenID, region);

	// Fill the region (if so specified).
	if (fillRegion)
		display->Rectangle(screenID, region, ZIL_NULLP(UI_PALETTE), 0, TRUE);

	// Draw the object border.
	for (int i = 0; i < 2; i++)
	{
		palette.colorForeground = motifColors.bottomShadow;
		display->Line(screenID, region.left, region.top,
			region.right, region.top, &palette);
		display->Line(screenID, region.left, region.top + 1,
			region.left, region.bottom - 1, &palette);
		palette.colorForeground = motifColors.topShadow;
		display->Line(screenID, region.left, region.bottom,
			region.right, region.bottom, &palette);
		display->Line(screenID, region.right, region.top + 1,
			region.right, region.bottom - 1, &palette);
		--region;
	}

	// Fill the region (if so specified).
	if (fillRegion)
		display->Rectangle(screenID, region, ZIL_NULLP(UI_PALETTE), 0, TRUE);

	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawFocus(ZIL_SCREENID screenID, UI_REGION &region, EVENT_TYPE)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
#endif
	region.left++;
	region.top++;
	UI_PALETTE *focusPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_ANY, ID_OUTLINE);
	display->VirtualGet(screenID, region);
	display->Rectangle(screenID, region, focusPalette, 2, FALSE, FALSE, &clip);
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event;

	return(ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawText(ZIL_SCREENID screenID, UI_REGION &region,
	const ZIL_ICHAR *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE )
{
	if (!palette)
		palette = lastPalette;

	display->VirtualGet(screenID, region);

	// Make sure there is a valid string.
	if (text && text[0])
	{
		// Fill the region (if so specified).
		if (fillRegion)
			display->Rectangle(screenID, region, palette, 0, TRUE);

		// Draw the text.
		int textWidth = display->TextWidth(text, screenID, font);
		int textHeight = display->TextHeight(text, screenID, font);
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			region.left = region.right - textWidth;
		else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.left = region.left + (region.Width() - textWidth) / 2;
		if (!Inherited(ID_TEXT))
			region.top += (region.Height() - textHeight) / 2;

		display->Text(screenID, region.left, region.top, text, palette, -1,
			FALSE, FALSE, ZIL_NULLP(UI_REGION), font);
		region.bottom = region.top + textHeight - 1;
		region.right = region.left + textWidth - 1;
	}

	display->VirtualPut(screenID);
	return (TRUE);
}


EVENT_TYPE UI_WINDOW_OBJECT::Event(const UI_EVENT &event)
{
	UI_EVENT tEvent = event;
	UI_WINDOW_OBJECT *object;

	// Seed the time indicator.
	//line below changed to conform to ISO C++ standard
	//static initializedTime = FALSE;
	static int initializedTime = FALSE;
	static ZIL_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);

	switch (ccode)
	{
	case S_INITIALIZE:
		if (parent && parent->Inherited(ID_LIST) && windowID[0] != ID_LIST_ITEM)
		{
			for (int i = 4; i; i--)
				windowID[i] = windowID[i - 1];
			windowID[0] = ID_LIST_ITEM;
		}
		if (!numberID && parent)
		{
			for (object = parent; object->parent; object = object->parent)
				;
			numberID = object->NumberID() ? object->NumberID() : 1;
			object->NumberID(numberID + 1);
			if (stringID[0] == '\0')
			{
				strcpy(stringID, _genericFieldID);
				itoa(numberID, &stringID[strlen(stringID)], 10);
			}
		}
		if (Inherited(ID_LIST_ITEM) && parent &&
			FlagSet(woAdvancedFlags, WOAF_COPY_DRAG_OBJECT))
			parent->woAdvancedFlags |= WOAF_COPY_DRAG_OBJECT;
		if (Inherited(ID_LIST_ITEM) && parent &&
			FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT))
			parent->woAdvancedFlags |= WOAF_MOVE_DRAG_OBJECT;
		lastPalette = LogicalPalette(ccode);
		RegionConvert(relative, TRUE);
		break;

	case S_SIZE:
		relative = event.region;
		if (parent)
		{
			if (!shell)
				shell = parent->shell;
			parent->RegionMax(this);
		}
		else
			RegionMax(this);
		if (screenID)
			XtVaSetValues(TopWidget(), XmNx, trueRegion.left, XmNy, trueRegion.top,
				XmNwidth, trueRegion.Width() - 1, XmNheight, trueRegion.Height() - 1, NULL);
		break;

	case S_CHANGED:
		if (Inherited(ID_POP_UP_ITEM) || Inherited(ID_POP_UP_MENU))
			break;
		if (!screenID || FlagSet(woStatus, WOS_INTERNAL_ACTION))
			break;
		// Continue to S_CREATE.
	case S_CREATE:
		if (parent)
			parent->RegionMax(this);
		else
			RegionMax(this);
		// Continue to S_REGISTER_OBJECT.
	case S_REGISTER_OBJECT:
		if (screenID && ccode != S_CHANGED)
		{
			Event(UI_EVENT(S_DEINITIALIZE));
		}
		if (parent && !shell)
			shell = parent->shell;
		
		if (ccode == S_REGISTER_OBJECT)
		{
			if (!shell)
				shell = parent->shell;
        	RegisterObject(xmDrawingAreaWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode);
		}
		break;
		
	case S_DEINITIALIZE:
		if (screenID && !FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			ZIL_SCREENID tScreenID = TopWidget();
			XtUnmanageChild(tScreenID);
			XtDestroyWidget(tScreenID);
		}
		screenID = shell = 0;
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		// Check the object status.
		if (ccode == S_CURRENT)
		{
			if (!FlagSet(woStatus, WOS_CURRENT))
			{
				if (!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
					woStatus |= WOS_REDISPLAY;
				woStatus |= WOS_CURRENT;
			}
			if (screenID && !FlagSet(woStatus, WOS_INTERNAL_ACTION)
					&& (!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX) ||
					Inherited(ID_TEXT)))
				XmProcessTraversal(screenID, XmTRAVERSE_CURRENT);
	
		}
		else if (ccode == S_NON_CURRENT && FlagSet(woStatus, WOS_CURRENT))
		{
			if (!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
				woStatus |= WOS_REDISPLAY;
			woStatus &= ~WOS_CURRENT;
		}

		lastPalette = LogicalPalette(ccode);

		// Continue to S_DISPLAY_ACTIVE.
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		{
		if (screenID && shell && !XtIsRealized(shell) &&
				shell != windowManager->shell)
			XtRealizeWidget(shell);
		UI_PALETTE *palette = LogicalPalette(ccode);
		if (palette != lastPalette)
		{
			lastPalette = palette;
		}
		if (!screenID || (parent && screenID == parent->screenID))
			;
		else if (ccode == S_CURRENT)
		{
			// Make sure the base window is current on the screen.
			//line below changed by jdh to conform to new "for" scope
			//for (UI_WINDOW_OBJECT *root = this; root->parent; root = root->parent)
			UI_WINDOW_OBJECT *root;
			for (UI_WINDOW_OBJECT *root = this; root->parent; root = root->parent)
				if (FlagSet(root->woAdvancedFlags, WOAF_TEMPORARY))
					break;
			if (windowManager->First() && root != windowManager->First())
				break;
			if (FlagSet(woFlags, WOF_AUTO_CLEAR))
			{
				//	Implement WOF_AUTO_CLEAR.
			}
		}
		if (ccode == S_DISPLAY_ACTIVE || ccode == S_DISPLAY_INACTIVE)
			woStatus |= WOS_REDISPLAY;
		if (FlagsSet(woStatus, WOS_OWNERDRAW | WOS_REDISPLAY))
		{
				display->VirtualGet(screenID, trueRegion);
				if (DrawItem(event, ccode))
					woStatus &= ~WOS_REDISPLAY;
				display->VirtualPut(screenID);
		}
		woStatus &= ~WOS_REDISPLAY;

		// Call the associated user or validate function.
		if (!(FlagSet(woStatus, WOS_INTERNAL_ACTION) ||
			(ccode != S_CURRENT && ccode != S_NON_CURRENT)))
			ccode = UserFunction(event, ccode);

		if (ccode == S_DISPLAY_ACTIVE)
		{
			Widget widget = screenID;
			if (parent && parent->screenID)
			{
				for ( ; widget && widget != shell &&
					(widget) != parent->screenID;
					widget = XtParent(widget))
				{
					if (!XtIsManaged(widget) && XtParent(widget))
						XtManageChild(widget);
				}
			}
		}
		}
		break;

	case S_REDISPLAY:
		if (screenID)
		{
			Widget topWidget = TopWidget();
			Window topWindow;
			if ((topWindow = XtWindow(topWidget)) &&
				XtWindow(screenID) &&
				event.type != E_MOTIF && FlagSet(woStatus, WOS_OWNERDRAW))
			{
				if (Inherited(ID_COMBO_BOX))
					XClearArea(display->xDisplay, topWindow, 0, 0, 0, 0, TRUE);
				else
					XClearArea(display->xDisplay, XtWindow(screenID), 0, 0, 0, 0, TRUE);
			}

			Widget widget = screenID;
			if (parent && parent->screenID)
			{
				for ( ; widget && widget != shell &&
					(widget) != parent->screenID;
					widget = XtParent(widget))
				{
					if (!XtIsManaged(widget) && XtParent(widget))
						XtManageChild(widget);
				}
			}
#	if (ZIL_MOTIF == 1002 && XmUPDATE_LEVEL < 3)
			XmDropSiteEndUpdate(screenID);
#	endif
		}
		break;

	case L_BEGIN_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			// Modify the object position.
			Modify(tEvent);

			// Set the new current object.
			tEvent.type = D_SET_OBJECT;
			tEvent.rawCode = searchID;
			tEvent.data = this;
			eventManager->Put(tEvent);
			tEvent.type = D_SET_POSITION;
			tEvent.rawCode = M_LEFT | M_LEFT_CHANGE;
			tEvent.position = event.position;
			eventManager->Put(tEvent);
		}
		else if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			return (S_UNKNOWN);
		break;

	case L_END_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			// See if the event is a double click.
			ZIL_TIME currentTime;
			long elapsedTime = currentTime - lastTime;
			if (AbsValue(elapsedTime) < doubleClickRate)
			{
				tEvent.type = D_EDIT_OBJECT;
				tEvent.rawCode = searchID;
				tEvent.data = this;
				eventManager->Put(tEvent);
				break;
			}
			lastTime = currentTime;
		}
		else
		{
			if (parent && ((UIW_WINDOW *)parent)->Current() != this &&
				!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			{
				UI_EVENT tEvent(S_ADD_OBJECT);
				tEvent.data = this;
				parent->Event(tEvent);
			}
			else if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				return (S_UNKNOWN);
		}
		break;

	case L_SELECT:
		ccode = UserFunction(event, L_SELECT);
		if (ccode != S_ERROR)
			Root(TRUE)->Event(UI_EVENT(L_SELECT));
		break;

	case L_BEGIN_ESCAPE:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			tEvent.type = D_SET_POSITION;
			tEvent.rawCode = M_RIGHT | M_RIGHT_CHANGE;
			tEvent.position.column = event.position.column;
			tEvent.position.line = (trueRegion.bottom - trueRegion.top) - event.position.line;
			eventManager->Put(tEvent);
		}
		break;

	case L_CANCEL:
		if (!(parent && parent->Inherited(ID_TABLE_RECORD)))
			break;
		// Fall thru.

	case L_UP:
	case L_DOWN:
	case L_PREVIOUS:
	case L_NEXT:
	case L_PGUP:
	case L_PGDN:
		if (parent && parent->Inherited(ID_TABLE_RECORD))
		{
			UI_EVENT rEvent(S_ROUTE_EVENT);
			rEvent.event = new UI_EVENT(ccode);
			rEvent.routeTarget = parent;
			eventManager->Put(rEvent);
		}
		else if (parent)
			ccode = parent->Event(event);
		else
			ccode = S_UNKNOWN;
		return ccode;

	case L_HELP:
		if (parent && helpContext == NO_HELP_CONTEXT)
			ccode = S_UNKNOWN;
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

#if (ZIL_MOTIF >= 1002)
	case L_BEGIN_MOVE_DRAG:
	case L_BEGIN_COPY_DRAG:
		if (FlagSet(woAdvancedFlags,
			WOAF_COPY_DRAG_OBJECT | WOAF_MOVE_DRAG_OBJECT))
		{
			// Set this object up as a drag source.
			Atom exportList[1];
			exportList[0] = UI_WINDOW_MANAGER::ZINC_DND;

			//  Don't let Motif drag.
			if (XmIsLabel(screenID) || XmIsText(screenID))
				XtOverrideTranslations(screenID, XtParseTranslationTable(
					"<Btn2Down>:	NoOp()"
					));

			unsigned char dragOperations = 0;
			if (FlagSet(woAdvancedFlags, WOAF_COPY_DRAG_OBJECT))
				dragOperations |= (unsigned char)XmDROP_COPY;
			if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT))
				dragOperations |= (unsigned char)XmDROP_MOVE;
			nargs = 0;
			XtSetArg(args[nargs], XmNexportTargets, exportList); nargs++;
			XtSetArg(args[nargs], XmNnumExportTargets, 1); nargs++;
			XtSetArg(args[nargs], XmNdragOperations, dragOperations); nargs++;
			XtSetArg(args[nargs], XmNconvertProc, DragConvertProc); nargs++;
			XEvent xEvent = event.message;
			Widget dc = XmDragStart(screenID, &xEvent, args, nargs);

			windowManager->dragObject = this;
			if (Inherited(ID_WINDOW))
				Information(I_GET_CURRENT, &windowManager->dragObject);
		}
		break;

	case L_END_MOVE_DRAG:
	case L_END_COPY_DRAG:
		windowManager->dragObject = NULL;
		break;

	case S_DROP_COPY_OBJECT:
	case S_DROP_MOVE_OBJECT:
		{
		if (windowManager->dragObject == this)
			break;

		ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
		if (ccode == S_DROP_MOVE_OBJECT &&
			windowManager->dragObject->Inherited(ID_LIST))
		{
			int count = 0;
			UI_WINDOW_OBJECT *object;
			UI_WINDOW_OBJECT *sObject;
			for(windowManager->dragObject->Information(I_GET_FIRST, &object);
				object; object = object->Next())
				if (FlagSet(object->woStatus, WOS_SELECTED))
				{
					sObject = object;
					count++;
				}

			if (count != 1)
				break;

			if (!userFunction || UserFunction(event, ccode) == S_UNKNOWN)
			{
				UI_EVENT dEvent(S_SUBTRACT_OBJECT);
				dEvent.windowObject = sObject;
				windowManager->dragObject->Event(dEvent);
				windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
				if (Inherited(ID_WINDOW))
				{
					UI_EVENT aEvent(S_ADD_OBJECT);
					aEvent.windowObject = sObject;
					Event(aEvent);
					Event(UI_EVENT(S_REDISPLAY));
				}
				else
				{
					sObject->Information(I_GET_TEXT, &text);
					if (text)
						Information(I_SET_TEXT, text);
					delete sObject;
				}
			}
		}
		else if (ccode == S_DROP_MOVE_OBJECT &&
			windowManager->dragObject->parent &&
			windowManager->dragObject->parent->Inherited(ID_LIST))
		{
			if (!userFunction || UserFunction(event, ccode) == S_UNKNOWN)
			{
				UI_WINDOW_OBJECT *list = windowManager->dragObject->parent;
				UI_EVENT dEvent(S_SUBTRACT_OBJECT);
				dEvent.windowObject = windowManager->dragObject;
				if (list->parent && list->parent->Inherited(ID_COMBO_BOX))
				{
					list->parent->Event(dEvent);
					list->parent->Event(UI_EVENT(S_REDISPLAY));
				}
				else
				{
					list->Event(dEvent);
					list->Event(UI_EVENT(S_REDISPLAY));
				}
				windowManager->dragObject->Information(I_GET_TEXT, &text);
				if (text)
					Information(I_SET_TEXT, text);
				delete windowManager->dragObject;
			}
		}
		else
		{
			if (!userFunction || UserFunction(event, ccode) == S_UNKNOWN)
			{
				windowManager->dragObject->Information(I_GET_TEXT, &text);
				if (text)
					Information(I_SET_TEXT, text);
				if (ccode == S_DROP_MOVE_OBJECT)
					windowManager->dragObject->Information(I_SET_TEXT,
						_blankString);
			}
		}
		}
		break;
#endif

	default:
		if (event.type == E_MOTIF && ccode != E_MOTIF)
			return (ccode);
		ccode = S_UNKNOWN;
		break;
	}

	if (event.type != E_MOTIF)
		return (ccode);

	switch (event.message.type)
	{
		case Expose:
			if (FlagSet(woStatus, WOS_OWNERDRAW))
			{
				if (parent)
					display->VirtualGet(screenID, trueRegion);
				else
					display->VirtualGet(screenID, 0, 0,
						trueRegion.Width() - 1, trueRegion.Height() - 1);
				UI_EVENT tEvent = event;
				tEvent.region.left = event.message.xexpose.x +
					trueRegion.left;
				tEvent.region.top = event.message.xexpose.y +
					trueRegion.top;
				tEvent.region.right = tEvent.region.left +
					event.message.xexpose.width - 1 +
				    	trueRegion.left;
				tEvent.region.bottom = tEvent.region.top +
					event.message.xexpose.height - 1 +
					trueRegion.top;
				DrawItem(tEvent, ccode);
				display->VirtualPut(screenID);
			}
			break;
		
		case FocusIn:
			if (event.message.xfocus.detail != 0)
				break;
			tEvent.type = S_ADD_OBJECT;
			for (object = this; object && object->screenID;
				object = object->parent)
			{
				tEvent.windowObject = object;
				if (object->parent)
					object->parent->Event(tEvent);
				else
				{
					// Set WOS_INTERNAL_ACTION to prevent recursion.
					object->woStatus |= WOS_INTERNAL_ACTION;
					windowManager->Event(tEvent);
					object->woStatus &= ~WOS_INTERNAL_ACTION;
				}
			}
			break;

		case KeyPress:
			if (ccode != L_HELP && ccode != L_CANCEL)
				windowManager->First()->Event(event);
			break;
	}
	return ccode;
}

//	_lastFocusEvent tracks X events that can change focus, (KeyPress and
//	ButtonPress).  It is used so derived objects can implement WOF_AUTO_CLEAR.
int _lastFocusEvent = 0;

// GeneralXEventHandler gets called for each event of the types we requested
// in RegisterObject().  We send the event to the Event() routine of the
// object the event belongs to.
void GeneralXEventHandler(Widget, XtPointer object, XEvent *xEvent, Boolean *)
{
	UI_WINDOW_OBJECT *uObject = (UI_WINDOW_OBJECT *)object;
	if (!uObject->screenID)
		return;
	UI_EVENT event(E_MOTIF, *xEvent);
	if (xEvent->type == KeyPress && event.rawCode == XK_Tab)
		_lastFocusEvent = KeyPress;
	else if (xEvent->type == ButtonPress)
		_lastFocusEvent = ButtonPress;
	uObject->Event(event);
}

// RegisterObject creates a widget.  You must provide either a
//	className or a convenienceFunction, and the one you don't provide must
//	be NULL.
void UI_WINDOW_OBJECT::RegisterObject(WidgetClass className,
	ZIL_MOTIF_CONVENIENCE_FUNCTION convenienceFunction,
	EVENT_TYPE ccode, int useArgs, int manage, ZIL_SCREENID _parent)
{
	if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
		return;
	_parent = _parent ? _parent : parent ? parent->screenID : shell;
	if (_parent == ZIL_NULLP(_WidgetRec)) {
		fprintf(stderr, "RegisterObject() called with a NULL parent.\n");
		abort();
	}
	if (!useArgs)
		nargs = 0;
	if (trueRegion.left > 0)
		XtSetArg(args[nargs], XmNx, trueRegion.left), nargs++;
	if (trueRegion.top > 0)
		XtSetArg(args[nargs], XmNy, trueRegion.top), nargs++;
	if (trueRegion.bottom > trueRegion.top + 1)
		XtSetArg(args[nargs], XmNheight, trueRegion.bottom - trueRegion.top + 1), nargs++;
	if (trueRegion.right > trueRegion.left + 1)
		XtSetArg(args[nargs], XmNwidth, trueRegion.right - trueRegion.left + 1), nargs++;
	XtSetArg(args[nargs], XmNinitialResourcesPersistent, FALSE); nargs++;
	XmFontList defaultFontList = XmFontListCopy(((UI_XT_DISPLAY *)display)->fontTable[font & ~FNT_IGNORE_UNDERSCORE].fontList);
	XtSetArg(args[nargs], XmNfontList,
		defaultFontList); nargs++;
//		((UI_XT_DISPLAY *)display)->fontTable[font & ~FNT_IGNORE_UNDERSCORE].fontList); nargs++;
	// XmNuserData used for Drag-n-Drop.
	XtSetArg(args[nargs], XmNuserData, this); nargs++;
	XtSetArg(args[nargs], XmNtraversalOn,
		FlagSet(woAdvancedFlags, WOAF_NON_CURRENT)
		|| FlagSet(woFlags, WOF_NON_SELECTABLE) ? FALSE : TRUE), nargs++;
	if (!Inherited(ID_PROMPT) && !FlagSet(woStatus, WOS_EDIT_MODE))
		XtSetArg(args[nargs], XmNsensitive,
			FlagSet(woFlags, WOF_NON_SELECTABLE) ? FALSE : TRUE), nargs++;
	if (Inherited(ID_LIST_ITEM))
		XtSetArg(args[nargs], XmNnavigationType, XmNONE), nargs++;

	if (ccode == S_CREATE || ccode == S_REGISTER_OBJECT)
	{
#		if defined(ZIL_UNICODE)
		char *widgetName = MapText(stringID);
#		else
		char *widgetName = stringID;
#		endif
		if (className)		// Create the widget.
		{
			if (manage)
				screenID = XtCreateManagedWidget(widgetName, className, _parent,
					args, nargs);
			else
				screenID = XtCreateWidget(widgetName, className, _parent,
					args, nargs);
		}
		else 				// Call the convenience function.
		{
			screenID = convenienceFunction(_parent, widgetName, args, nargs);
			if (manage)
				XtManageChild(screenID);
		}
#		if (ZIL_MOTIF == 1002 && XmUPDATE_LEVEL < 3)
		XmDropSiteStartUpdate(screenID);
#		endif
#		if defined(ZIL_UNICODE)
		delete widgetName;
#		endif

		//  Override the translation of movement events so only Zinc gets them.
		if (parent && parent->Inherited(ID_LIST))
		{
			XtOverrideTranslations(screenID, XtParseTranslationTable(
				"Ctrl<Key>Up:	NoOp()\n\
				Ctrl<Key>Down:	NoOp()\n\
				Ctrl<Key>Left:	NoOp()\n\
				Ctrl<Key>Right:	NoOp()"
				));
		}

#if (ZIL_MOTIF >= 1002)
	if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP) && screenID)
	{
		// Set this object up as a drag target.
		Atom importList[1];
		importList[0] = UI_WINDOW_MANAGER::ZINC_DND;

		// Unregister in case Motif already registered.
		XmDropSiteUnregister(screenID);

		nargs = 0;
		XtSetArg(args[nargs], XmNimportTargets, importList); nargs++;
		XtSetArg(args[nargs], XmNnumImportTargets, 1); nargs++;
		// Set XmDROP_COPY and XmDROP_MOVE.  Specifics are decided later.
		XtSetArg(args[nargs], XmNdropSiteOperations,
			XmDROP_COPY | XmDROP_MOVE); nargs++;
		XtSetArg(args[nargs], XmNdropProc, HandleDrop); nargs++;
		if (Inherited(ID_WINDOW))
		{
			XtSetArg(args[nargs], XmNdropSiteType, XmDROP_SITE_COMPOSITE);
				nargs++;
		}
		XmDropSiteRegister(screenID, args, nargs);
	}
#endif
	}

	// Add an event handler for all appropriate events.  I use an event
	// handler rather than action procedures because I want the client-data
	// pointer and because ZIL does its own translation.
	if ((ccode == S_CREATE || ccode == S_REGISTER_OBJECT))
	{
		Widget eventWidget = screenID;
		EventMask generalMask = EnterWindowMask | LeaveWindowMask |
			FocusChangeMask;
		if (FlagSet(woStatus, WOS_OWNERDRAW))
			generalMask |= ExposureMask;
		EventMask deviceMask = KeyPressMask | KeyReleaseMask |
			ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
		if (Inherited(ID_SCROLL_BAR))
		{
			// Get events on the scrollbar child of the scale.
			SBF_FLAGS scaleFlags;
			Information(I_GET_FLAGS, &scaleFlags, ID_SCROLL_BAR);
			if (FlagSet(scaleFlags, SBF_SCALE))
				eventWidget = ((XmScaleWidget)screenID)->composite.children[1];
		}
		if (parent && parent->Inherited(ID_LIST))
		{
			// Keyboard and mouse events of list children propagate to the list.
			XtAddEventHandler(eventWidget, generalMask, FALSE,
				GeneralXEventHandler, (XtPointer)this);
			XtAddEventHandler(eventWidget, deviceMask, FALSE,
				GeneralXEventHandler, (XtPointer)parent);
		}
		else
		{
			XtAddEventHandler(eventWidget, generalMask | deviceMask, FALSE,
				GeneralXEventHandler, (XtPointer)this);
		}
	}

	if (!shell)
	{
		fprintf(stderr, "Warning: UI_WINDOW_OBJECT::shell can't be NULL.\n");
		fprintf(stderr, "         Was UI_WINDOW_OBJECT::Event(S_CREATE) called?\n");
	}
	return;
}

void UI_WINDOW_OBJECT::Modify(const UI_EVENT &event)
{
	// Make sure we can move the object.
	if (!parent || (FlagSet(woFlags, WOF_NON_FIELD_REGION)
		&& !Inherited(ID_TABLE_HEADER)) ||
		(event.type == L_SIZE && Inherited(ID_ICON)) ||
		parent->Inherited(ID_LIST))
		return;

	// Don't move child dialogs here.
	if (Inherited(ID_WINDOW))
	{
		long decorations;
		Information(I_GET_DECORATIONS, &decorations, ID_WINDOW);
		if (decorations)
			return;
	}

	// Determine move / size operation.
	UI_REGION sizeBoundary;
	int offset = (trueRegion.Width() > 10) ? 5 : 1;
	sizeBoundary.left = sizeBoundary.top = offset;
	sizeBoundary.right = relative.right - relative.left - offset;
	sizeBoundary.bottom = relative.bottom - relative.top - offset;
	ZIL_RAW_CODE sizeFlags = 0;
	EVENT_TYPE operation = L_SIZE;
	if (event.type == L_MOVE ||
		(event.position.column >= sizeBoundary.left && event.position.line >= sizeBoundary.top &&
		event.position.column <= sizeBoundary.right && event.position.line <= sizeBoundary.bottom))
	{
		sizeFlags = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		operation = L_MOVE;
	}
	if (event.position.column < sizeBoundary.left)
		sizeFlags |= M_LEFT_CHANGE;
	else if (event.position.column > sizeBoundary.right)
		sizeFlags |= M_RIGHT_CHANGE;
	if (event.position.line < sizeBoundary.top)
		sizeFlags |= M_TOP_CHANGE;
	else if (event.position.line > sizeBoundary.bottom)
		sizeFlags |= M_BOTTOM_CHANGE;

	// Determine the minimum height and width of the object.
	int minHeight = display->cellHeight;
	if (parent && searchID != ID_WINDOW)
		minHeight -= (display->preSpace + display->postSpace);
	else
		minHeight += 2 * UIW_BORDER::width;
	int minWidth = 2 * display->cellWidth;
	if (Inherited(ID_WINDOW) && !Inherited(ID_SCROLL_BAR)
		&& !Inherited(ID_LIST) && !Inherited(ID_GROUP))
		minWidth *= 5;

	// Determine the absolute region.
	UI_REGION absolute;
	if (!parent || (parent->parent && parent->parent->Inherited(ID_WINDOW)))
	{
		absolute.left = 0;
		absolute.top = 0;
		absolute.right = display->columns - 1;
		absolute.bottom = display->lines - 1;
	}
	else
	{
		Dimension height, width;
		nargs = 0;
		XtSetArg(args[nargs], XmNwidth, &width); nargs++;
		XtSetArg(args[nargs], XmNheight, &height); nargs++;
		XtGetValues(parent->screenID, args, nargs);

		absolute.left = 0; absolute.top = 0;
		absolute.right = width - 1;
		absolute.bottom = height - 1;
	}

	// Adjust size flags if necessary.
	if (operation != L_MOVE)
	{
		if (Inherited(ID_BUTTON))
		{
			BTF_FLAGS btFlags;
			Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
			if (FlagSet(btFlags, BTF_AUTO_SIZE))
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		}
//		else if (!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX))
		else if ((!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX)) && searchID < 3000)
			sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		else if (Inherited(ID_SCROLL_BAR))
		{
			SBF_FLAGS sbFlags = SBF_NO_FLAGS;
			Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
			if (!FlagSet(sbFlags, SBF_VERTICAL))
			{
				minHeight = relative.bottom - relative.top + 1;
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
			}
			if (!FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				minWidth = relative.right - relative.left + 1;
				sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
			}
		}
		else if (Inherited(ID_TABLE_RECORD))
		{
			sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE;
			if (parent->Inherited(ID_TABLE_HEADER))
			{
				THF_FLAGS thFlags;
				parent->Information(I_GET_FLAGS, &thFlags);
				if (FlagSet(thFlags, THF_COLUMN_HEADER))
					sizeFlags &= ~M_BOTTOM_CHANGE;
				else if (FlagSet(thFlags, THF_ROW_HEADER))
					sizeFlags &= ~M_RIGHT_CHANGE;
				else
					sizeFlags = 0;
			}
		}
		else if (Inherited(ID_TABLE_HEADER))
		{
			THF_FLAGS thFlags;
			Information(I_GET_FLAGS, &thFlags);
			if (FlagSet(thFlags, THF_COLUMN_HEADER))
				sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_RIGHT_CHANGE;
			else if (FlagSet(thFlags, THF_ROW_HEADER))
				sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_BOTTOM_CHANGE;
			else
				sizeFlags = 0;
		}
	}
	else if (Inherited(ID_TABLE_RECORD) || Inherited(ID_TABLE_HEADER))
		return;

	if (!sizeFlags)
		return;

	UI_POSITION origin = event.position;
	UI_REGION newRegion = trueRegion;
	UI_REGION oldRegion = newRegion;
	int xJump = display->cellWidth;
	int yJump = display->cellHeight;
	XGCValues gcValues;
	gcValues.subwindow_mode = IncludeInferiors;
	XChangeGC(display->xDisplay, display->xGC, GCSubwindowMode, &gcValues);
	display->VirtualGet(parent->screenID, 0, 0,
		parent->trueRegion.Width()-1, parent->trueRegion.Height()-1);
	display->Rectangle(parent->screenID, newRegion, display->xorPalette, 1,
		FALSE, TRUE);
	int type;
	EVENT_TYPE ccode = 0;
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		type = event.message.type;
		int deltaX = 0, deltaY = 0;
		if (type == ButtonPress || type == MotionNotify &&
			event.message.xmotion.state == Button1Mask)
		{
			// Check the absolute region.
			if (trueRegion.left + event.position.column > absolute.right)
				event.position.column = absolute.right - trueRegion.left;
			if (trueRegion.top + event.position.line > absolute.bottom)
				event.position.line = absolute.bottom - trueRegion.top;
			deltaX = event.position.column - origin.column;
			deltaY = event.position.line - origin.line;
		}
		else if (type == KeyPress)
		{
			deltaX = newRegion.right - trueRegion.right;
			deltaY = newRegion.bottom - trueRegion.bottom;
			ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
			switch (ccode)
			{
			case L_UP:
			case L_DOWN:
				if (ccode == L_DOWN)
					deltaY += yJump;
				else
					deltaY -= yJump;
				break;

			case L_LEFT:
			case L_RIGHT:
				if (ccode == L_RIGHT)
					deltaX += xJump;
				else
					deltaX -= xJump;
				break;

			default:
				if (event.key.value == 0x000D)		// ENTER
					ccode = L_SELECT;
				else if (event.key.value == 0x001B)	// ESCAPE
					ccode = L_CANCEL;
				break;
			}
		}
		// Send all user messages to the window manager.
		if (event.type > 9999 || event.type < -999)
			windowManager->Event(event);
		else if (event.type == E_MOTIF)
			XtDispatchEvent((XEvent*)&event.message);

		// Update the new region.
		if (deltaX || deltaY)
		{
#if defined(ZIL_EDIT)
			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				// Check the absolute region.
				if (FlagSet(sizeFlags, M_LEFT_CHANGE))
				{
					if (trueRegion.left + deltaX < absolute.left)
						deltaX = absolute.left - trueRegion.left;
				}
				if (FlagSet(sizeFlags, M_TOP_CHANGE))
				{
					if (trueRegion.top + deltaY < absolute.top)
						deltaY = absolute.top - trueRegion.top;
				}
			}

			long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
			long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

			// Check for a cell boundary move or size.
			if (FlagSet(woStatus, WOS_EDIT_MODE) && FlagSet(woFlags, WOF_MINICELL))
			{
				long value = deltaX;
				int ceil = (value >= 0) ? (int)(miniDX - 1) : int(1 - miniDX);
				value = (value * miniDX) / (miniNX * display->cellWidth);
				value = (value * miniNX * display->cellWidth + ceil) / miniDX;
				deltaX = (int)value;

				value = deltaY;
				ceil = (value >= 0) ? (int)(miniDY - 1) : (int)(1 - miniDY);
				value = (value * miniDY) / (miniNY * display->cellHeight);
				value = (value * miniNY * display->cellHeight + ceil) / miniDY;
				deltaY = (int)value;
			}
			else if (FlagSet(woStatus, WOS_EDIT_MODE) && !FlagSet(woFlags, WOF_PIXEL))
			{
				int value = (deltaX > 0) ? deltaX + display->cellWidth / 2 : deltaX - display->cellWidth / 2;
				deltaX = value / display->cellWidth * display->cellWidth;
				value = (deltaY > 0) ? deltaY + display->cellHeight / 2 : deltaY - display->cellHeight / 2;
				deltaY = value / display->cellHeight * display->cellHeight;
			}
#endif

			if (FlagSet(sizeFlags, M_LEFT_CHANGE))
				newRegion.left = trueRegion.left + deltaX;
			if (FlagSet(sizeFlags, M_TOP_CHANGE))
				newRegion.top = trueRegion.top + deltaY;
			if (FlagSet(sizeFlags, M_RIGHT_CHANGE))
				newRegion.right = trueRegion.right + deltaX;
			if (FlagSet(sizeFlags, M_BOTTOM_CHANGE))
				newRegion.bottom = trueRegion.bottom + deltaY;

 			// Check for minimum size.
			if (operation == L_SIZE)
			{
				if (newRegion.left + minWidth - 1 > newRegion.right)
				{
					newRegion.left = oldRegion.left;
					newRegion.right = oldRegion.right;
				}
				if (newRegion.top + minHeight - 1 > newRegion.bottom)
				{
					newRegion.top = oldRegion.top;
					newRegion.bottom = oldRegion.bottom;
				}
			}

			// Move sizing rectangle.
			if (newRegion.left != oldRegion.left || newRegion.top != oldRegion.top ||
				newRegion.right != oldRegion.right || newRegion.bottom != oldRegion.bottom)
			{
				display->RectangleXORDiff(oldRegion, newRegion, parent->screenID);
				oldRegion = newRegion;
			}
		}
	} while (type != ButtonRelease && ccode != L_SELECT && ccode != L_CANCEL);
	display->Rectangle(parent->screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->VirtualPut(parent->screenID);
	gcValues.subwindow_mode = ClipByChildren;
	XChangeGC(display->xDisplay, display->xGC, GCSubwindowMode, &gcValues);
	if (ccode == L_CANCEL)
		return;				// Do not change the object region.

	if (newRegion.left != trueRegion.left || newRegion.right != trueRegion.right ||
		newRegion.top != trueRegion.top || newRegion.bottom != trueRegion.bottom)
	{
		// Size and redisplay the object.
		relative.left += newRegion.left - trueRegion.left;
		relative.top += newRegion.top - trueRegion.top;
		relative.right += newRegion.right - trueRegion.right;
		relative.bottom += newRegion.bottom - trueRegion.bottom;

		if (Inherited(ID_TABLE_HEADER) && !Inherited(ID_TABLE_RECORD))
		{
			UI_WINDOW_OBJECT *tableRecord;
			Information(I_GET_FIRST, &tableRecord);
			if (tableRecord)
			{
				tableRecord->relative.right += newRegion.right - trueRegion.right;
				tableRecord->relative.bottom += newRegion.bottom - trueRegion.bottom;
			}
			parent->Event(UI_EVENT(S_DEINITIALIZE));
			parent->Event(UI_EVENT(S_CREATE));
		}
		else
		{
			Event(UI_EVENT(S_DEINITIALIZE));
			Event(UI_EVENT(S_CREATE));
		}
		UI_WINDOW_OBJECT *current = ZIL_NULLP(UI_WINDOW_OBJECT);
		parent->Information(I_GET_CURRENT, &current);
		if (current)
		{
			// ProcessTraversal to current, after the S_CREATE.
			UI_EVENT aEvent(S_ADD_OBJECT);
			aEvent.windowObject = current;
			parent->Event(aEvent);
		}
	}
}


// Zinc needs ~ or & in a string to mark the hotkey.  This routine strips
// these chars out.
void UI_WINDOW_OBJECT::StripHotKeyMark(ZIL_ICHAR *text)
{
	if (!text)
		return;

	// Strip any non-doubled & or ~ out.
	for (ZIL_ICHAR *hotChar = strchr(text, ZIL_UNDERLINE_CHARACTER);
			hotChar; hotChar = strchr(++hotChar, ZIL_UNDERLINE_CHARACTER))
		if (hotChar[1] && hotChar[1] != ZIL_UNDERLINE_CHARACTER &&
			hotChar[1] != ZIL_OLD_UNDERLINE_CHARACTER)
		{
			memcpy(hotChar, hotChar + 1, strlen(hotChar) * sizeof(ZIL_ICHAR));
		}
}

//	CreateMotifString strips hot key marks from _text if strip is TRUE,
//	converts _text to an XmString, and returns the XmString.  If displayText
//	isn't NULL, it's set to point to static memory that contains the
//	stripped text.
//	User should XmStringFree() the return value.
XmString UI_WINDOW_OBJECT::CreateMotifString(ZIL_ICHAR *_text,
	ZIL_ICHAR **displayText, int strip)
{
	ZIL_ICHAR *fillLine = new ZIL_ICHAR[256];
	_text = _text && *_text ? _text : _blankString;
	int length = strlen(_text);
	length = length > 255 ? 255 : length;
	strncpy(fillLine, _text, length);
	fillLine[length] = '\0';

	if (strip)
		StripHotKeyMark(fillLine);
	if (displayText)
		*displayText = fillLine;

	char *iString;
#	if defined(ZIL_UNICODE)
	iString = ZIL_INTERNATIONAL::MapText(fillLine);
#	else
	iString = fillLine;
#	endif

#if (ZIL_MOTIF < 1002)
	XmString motifString = XmStringCreate(iString, XmSTRING_DEFAULT_CHARSET);
#else
	XmString motifString = XmStringCreateLocalized(iString);
#endif

#	if defined(ZIL_UNICODE)
	delete iString;
#	endif

	if (fillLine)
		delete [] fillLine;
	return motifString;
}
	


//	Return the Widget at the top of a Zinc object's widget tree (because
//	some zinc objects map to more than one Motif object.
ZIL_SCREENID UI_WINDOW_OBJECT::TopWidget(void)
{
	Widget widget = screenID;
	if (parent && parent->screenID)
		for ( ; widget && widget != shell && XtParent(widget) &&
			XtParent(widget) != parent->screenID;
			widget = XtParent(widget));
	else if (!parent)
		widget = shell;
	return(widget);
}

#if (ZIL_MOTIF >= 1002)
Boolean UI_WINDOW_OBJECT::DragConvertProc(Widget, Atom *selection,
	Atom *target, Atom *typeRtn, XtPointer *valueRtn, unsigned long *lengthRtn,
	int *formatRtn, unsigned long *, XtPointer, XtRequestId *)
{
	if (*target != UI_WINDOW_MANAGER::ZINC_DND)
		return (False);

	Atom MOTIF_DROP = XmInternAtom(UI_WINDOW_MANAGER::display->xDisplay,
		"_MOTIF_DROP", False);
	if (*selection != MOTIF_DROP)
		return (False);

	char *pass = XtMalloc(1);
	*typeRtn = UI_WINDOW_MANAGER::ZINC_DND;
	*valueRtn = (XtPointer)pass;
	*lengthRtn = 1;
	*formatRtn = 8;

	return(TRUE);
}

// I can't store both an object and a position in an event, so make a struct.
static struct {
	UI_WINDOW_OBJECT *object;
	UI_EVENT event;
} _dndInfo;

void UI_WINDOW_OBJECT::HandleDrop(Widget w, XtPointer,
	XtPointer callData)
{
	XmDropProcCallback dropData = (XmDropProcCallback)callData;
	XmDropTransferEntryRec transferEntries[2];
	XmDropTransferEntry transferList;

	nargs = 0;
	if (dropData->dropAction == XmDROP)
	{
		transferEntries[0].target = UI_WINDOW_MANAGER::ZINC_DND;
		transferEntries[0].client_data = (XtPointer)w;
		transferList = transferEntries;

		XtSetArg(args[nargs], XmNdropTransfers, transferList); nargs++;
		XtSetArg(args[nargs], XmNnumDropTransfers, 1); nargs++;
		XtSetArg(args[nargs], XmNtransferProc, TransferProc); nargs++;

		// Set up the structure to communicate with DragDropFinish.
		XtVaGetValues(w, XmNuserData, &_dndInfo.object, ZIL_NULLP(void));
		if (_dndInfo.object->windowManager->dragObject)
		{
			if (dropData->operation == XmDROP_COPY)
				_dndInfo.event.type = S_DROP_COPY_OBJECT;
			else if (dropData->operation == XmDROP_MOVE)
				_dndInfo.event.type = S_DROP_MOVE_OBJECT;
			_dndInfo.event.position.column = dropData->x;
			_dndInfo.event.position.line = dropData->y;
			XtAddCallback(dropData->dragContext, XmNdragDropFinishCallback,
				DragDropFinish, ZIL_NULLP(XtPointer));
		}
	}
	else
	{
		XtSetArg(args[nargs], XmNtransferStatus, XmTRANSFER_FAILURE); nargs++;
	}

	XmDropTransferStart(dropData->dragContext, args, nargs);

	if (dropData->dropAction == XmDROP)
	{
	}
}

void UI_WINDOW_OBJECT::TransferProc(Widget, XtPointer, Atom *,
		Atom *, XtPointer, unsigned long *, int *)
{
	// Nothing to transfer.  All we need to know is that a drop happened.
}

void UI_WINDOW_OBJECT::DragDropFinish(Widget, XtPointer,
	XtPointer)
{
	// Use an XmNdragDropFinishCallback so Zinc doesn't do anything
	// until Motif is finished.
	if (_dndInfo.object->Inherited(ID_LIST_ITEM))
		_dndInfo.object->parent->Event(_dndInfo.event);
	else
		_dndInfo.object->Event(_dndInfo.event);
	_dndInfo.object->windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
}

#endif

// ----- OS Specific Functions ----------------------------------------------

void UI_WINDOW_OBJECT::OSInitialize(void)
{
	shell = 0;
}

void UI_WINDOW_OBJECT::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (screenID)
	{
		UI_EVENT event(S_DEINITIALIZE, 0);
		Event(event);
		woStatus |= WOS_REDISPLAY;
	}
}
