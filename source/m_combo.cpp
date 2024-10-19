//	Zinc Interface Library - M_COMBO.CPP
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
#if defined(ZIL_UNICODE)
#	include <stdlib.h>
#endif
#include <Xm/BulletinB.h>
#include <Xm/DrawnB.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/MwmUtil.h>
#include <X11/cursorfont.h>
}

#define comboArrow_width 16
#define comboArrow_height 16
static unsigned char comboArrow_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03,
   0xc0, 0x03, 0xc0, 0x03, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01,
   0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00};
static Pixmap _comboArrow = 0;

void UIW_COMBO_BOX::PopupCallback(Widget, XtPointer client, XtPointer)
{
	UIW_COMBO_BOX *object = (UIW_COMBO_BOX *)client;
	UI_EVENT rEvent(S_ROUTE_EVENT);
	rEvent.event = new UI_EVENT(L_SELECT);
	rEvent.routeTarget = object;
	eventManager->Put(rEvent);
}

//	Motif doesn't yet provide a combo box, so we build one from a
//	label or string, a button and a UIW_VT_LIST on a transient shell.
//	For the Motif key, each vlist has a comboShell member which will
//	be NULL unless the list is part of a combo box, in which case it
//	contains the widget id of the transient shell.  The vlist's parent
//	is the combobox.

// ----- UIW_COMBO_BOX ------------------------------------------------------

EVENT_TYPE UIW_COMBO_BOX::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_COMBO_BOX);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		list.Event(event);
		if (popupCursor == 0)
			popupCursor = XCreateFontCursor(display->xDisplay, XC_right_ptr);
		break;

	case S_CHANGED:
		break;

	case S_CREATE:
	case S_REGISTER_OBJECT:
		{
		UI_WINDOW_OBJECT::Event(event);
		UI_REGION region = trueRegion;
		trueRegion.bottom = trueRegion.top + display->cellHeight - 1;

		// Create a bulletin board to contain the string and arrow button.
		nargs = 0;
		XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
		XtSetArg(args[nargs], XmNmarginWidth, 0); nargs++;
		XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;
		RegisterObject(xmBulletinBoardWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE);

		// Create an arrow button.
		nargs = 0;
		XtSetArg(args[nargs], XmNtraversalOn, FALSE); nargs++;
		XtSetArg(args[nargs], XmNx, trueRegion.Width() - 3 * display->cellWidth); nargs++;
		XtSetArg(args[nargs], XmNy, 4); nargs++;
		XtSetArg(args[nargs], XmNwidth, 3 * display->cellWidth - 3); nargs++;
		XtSetArg(args[nargs], XmNheight, display->cellHeight - 8); nargs++;
		XtSetArg(args[nargs], XmNrecomputeSize, FALSE); nargs++;
		if (!_comboArrow)
		{
			// create the arrow in S_CREATE, not S_INITIALIZE, because parent
			// must be realized.
			ZIL_COLOR background, foreground;
			XtVaGetValues(screenID, XmNforeground, &foreground,
				XmNbackground, &background, ZIL_NULLP(void));
			_comboArrow = XCreatePixmapFromBitmapData(display->xDisplay,
				RootWindowOfScreen(display->xScreen), (char *)comboArrow_bits,
				comboArrow_width, comboArrow_height,
				foreground, background, DefaultDepthOfScreen(display->xScreen));
		}
		XtSetArg(args[nargs], XmNhighlightThickness, 0); nargs++;
		XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
		XtSetArg(args[nargs], XmNlabelPixmap, _comboArrow); nargs++;
#		if defined(ZIL_UNICODE)
		char *widgetName = MapText(stringID);
#		else
		char *widgetName = stringID;
#		endif
		arrow = XtCreateManagedWidget(widgetName, xmPushButtonWidgetClass,
			screenID, args, nargs);


		// Create the override shell to hold the list.
		nargs = 0;
		XtSetArg(args[nargs], XmNallowShellResize, TRUE); nargs++;
		XtSetArg(args[nargs], XmNkeyboardFocusPolicy, XmEXPLICIT); nargs++;
		XtSetArg(args[nargs], XmNmwmDecorations, MWM_DECOR_BORDER); nargs++;
		XtSetArg(args[nargs], XmNtransientFor, shell); nargs++;
		listShell = XtCreatePopupShell("", transientShellWidgetClass,
			display->topShell, args, nargs);
#		if defined(ZIL_UNICODE)
		delete widgetName;
#		endif
		XtAddCallback(arrow, XmNactivateCallback, UIW_COMBO_BOX::PopupCallback,
			(XtPointer)this);

		// Create the list.
		list.relative.right = list.relative.left + region.Width() - 1;
		list.relative.bottom += 10;		// Leave room for MWM_DECOR_BORDER.
		list.comboShell = listShell;
		list.parent = this;
		list.Event(event);
		current = list.current;

		trueRegion = region;
		trueRegion.left = trueRegion.top = 0;
		trueRegion.bottom = trueRegion.top + display->cellHeight - 1;
		trueRegion.right = region.Width() - 3 * display->cellWidth - 1;
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY))
			trueRegion.right -= display->cellWidth / 2;
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		{
			// Create a xmDrawnButton to show the bitmaps.

			nargs = 0;
			trueRegion.right -= 4;
			trueRegion.bottom -= 4;
			XtSetArg(args[nargs], XmNshadowThickness, 2); nargs++;
			XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
			if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
				XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_CENTER), nargs++;
			else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_END), nargs++;
			else
				XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_BEGINNING), nargs++;
			XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
#if !defined(_IBM_RS6000) && !defined(_SUNOS4) && !defined(__sgi)
			XtSetArg(args[nargs], XmNrecomputeSize, FALSE); nargs++;
#endif

			XtSetArg(args[nargs], XmNshadowType, XmSHADOW_IN); nargs++;
			int maxHeight = display->cellHeight;
			for (object = list.First(); object; object = object->Next())
				maxHeight = MaxValue(object->trueRegion.Height(), maxHeight);
			trueRegion.bottom = trueRegion.top + maxHeight + 4;
			RegisterObject(xmDrawnButtonWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE, screenID);
			XtVaSetValues(XtParent(screenID), XmNheight, maxHeight + 6,
				ZIL_NULLP(void));
		}
		else
		{
			// Create a text field.
			// Assume all list children know how to handle I_GET_TEXT.
			nargs = 0;
			if (FlagSet(woFlags, WOF_VIEW_ONLY) ||
				FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) ||
				FlagSet(woFlags, WOF_NON_SELECTABLE))
			{
				XtSetArg(args[nargs], XmNeditable, FALSE); nargs++;
				XtSetArg(args[nargs], XmNcursorPositionVisible, FALSE); nargs++;
			}
			XtSetArg(args[nargs], XmNmarginHeight, 3); nargs++;
			XtSetArg(args[nargs], XmNverifyBell, FALSE); nargs++;
			RegisterObject(xmTextWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
				ccode, TRUE, TRUE, screenID);
		}


		UIW_COMBO_BOX::Information(I_RESET_SELECTION, list.Current() ?
			list.Current() : list.First());
		trueRegion = region;
		}
		break;

	case S_ADD_OBJECT:
		object = event.windowObject;
		if (object != &list && (UI_LIST::Index(object) == -1))
		{
			ccode = list.Event(event);
			current = list.current;
			Information(I_RESET_SELECTION, current);
		}
		break;

	case S_REDISPLAY:
		for (object = (UI_WINDOW_OBJECT *)support.First(); object;
			object = object->Next())
		{
			object->Event(event);
		}
		for (object = list.First(); object; object = object->Next())
		{
			object->Event(event);
		}
		if (screenID)
		{
			Widget widget = list.screenID;
			if (screenID && list.screenID)
			{
				if (!XtIsManaged(widget))
					XtManageChild(widget);
				widget = XtParent(widget);
				if (!XtIsManaged(widget))
					XtManageChild(widget);
				if (!XtIsManaged(listShell))
					XtManageChild(listShell);
			}
		}
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_DEINITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		listShell = 0;		// listShell got destroyed with list.
		// Fall thru.

	case L_UP:
	case L_DOWN:
	case S_SUBTRACT_OBJECT:
		ccode = list.Event(event);
		current = list.current;
		break;

	case L_SELECT:
		ccode = UserFunction(event, L_SELECT);
		UI_WINDOW_OBJECT::Event(UI_EVENT(S_CURRENT));

		// Pop up the listShell.
		if (!FlagSet(woStatus, WOS_EDIT_MODE))
		{
			if (list.first)
			{
				char popupGeometry[10];
				Position motifX, motifY;
				XtTranslateCoords(screenID, 0, 0, &motifX, &motifY);
				::sprintf(popupGeometry, "+%d+%d", motifX, motifY + display->cellHeight);
				XtUnrealizeWidget(listShell);
				XtVaSetValues(listShell, XmNgeometry, popupGeometry, ZIL_NULLP(void));
				XtRealizeWidget(listShell);
				XDefineCursor(display->xDisplay, XtWindow(listShell),
					popupCursor);
				XtPopup(listShell, XtGrabExclusive);
				if (list.current)
					XmProcessTraversal(list.Current()->screenID,
						XmTRAVERSE_CURRENT);
			}
			else
				UI_ERROR_SYSTEM::Beep();		
		}
//		eventManager->Put(UI_EVENT(S_VERIFY_STATUS));
		break;

	case L_CANCEL:
		if (listShell)
		{
//			XtUngrabKeyboard(list.screenID, CurrentTime);
//			XtUngrabPointer(list.screenID, CurrentTime);
			XtPopdown(listShell);
		}
		break;

	case S_VERIFY_STATUS:
		{
//			XtGrabPointer(list.screenID, FALSE, 0, GrabModeAsync, GrabModeAsync,
//				None, popupCursor, CurrentTime);
//			XtGrabKeyboard(list.screenID, FALSE, GrabModeAsync, GrabModeAsync,
//				CurrentTime);
		}
		break;

	case S_DROP_DEFAULT:
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		ccode = list.Event(event);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);

#if 0
		// Try to do first character recognition.
		if (event.type == E_MOTIF && event.message.type == KeyPress && current)
		{
			ZIL_ICHAR keyBuf[2];
#if defined(ZIL_UNICODE)
			char *tmp = new char[20];
			Status status;
			int len = XmbLookupString(display->ic, (XKeyEvent *)(&event.message),
						  tmp, 20, ZIL_NULLP(KeySym), &status);
			if (status == XBufferOverflow)
			{
				delete tmp;
				tmp = new char[len];
				len = XmbLookupString(display->ic, (XKeyEvent *)(&event.message),
						      tmp, len, ZIL_NULLP(KeySym), &status);
			}
			UnMapText(tmp, keyBuf);
			delete tmp;
			if (keyBuf[0] < 0x20)
				break;
#else
			if ((XLookupString((XKeyEvent *)(&event.message), keyBuf, 1,
					ZIL_NULLP(KeySym), ZIL_NULLP(XComposeStatus)) != 1) ||
					keyBuf[0] < 0x20)
				break;

			keyBuf[1] = 0;
#endif
			for (object = Current()->Next(); object; object = object->Next())
				if (keyBuf[0] == *(ZIL_ICHAR *)object->Information(I_GET_TEXT,
					ZIL_NULLP(void)))
					break;
			if (!object)
				for (object = First(); object != Current(); object = object->Next())
					if (keyBuf[0] == *(ZIL_ICHAR *)object->Information(I_GET_TEXT,
						ZIL_NULLP(void)))
						break;
			if (object && object != Current())
			{
				Add(object);
				UIW_COMBO_BOX::Information(I_RESET_SELECTION, list.Current() ?
					list.Current() : list.First());
			}
		}
#endif
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_COMBO_BOX::OSDataGet(void)
{
	if (screenID && !FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
	{
#	if defined(ZIL_UNICODE)
		wchar_t *wText = XmTextGetStringWcs(screenID);
		for (wchar_t *wPtr = wText; *wPtr; wPtr++)
			;
		char *mb = new char[4 * (wPtr - wText)];
		::wcstombs(mb, wText, (wPtr - wText) + 1);
		UnMapText(mb, text);
		XtFree((char *)wText);
		delete(mb);
#	else
		char *tText = XmTextGetString(screenID);
		strcpy(text, tText);
		XtFree(tText);
#	endif
	}
}

void UIW_COMBO_BOX::OSDataSet(void)
{
	if (screenID && !FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
	{
		if (!text)
			text = _blankString;

		// Use XtSetValues because XmTextSetString doesn't flush.
		ZIL_ICHAR *value;
#if defined(ZIL_UNICODE)
		XtVaGetValues(screenID, XmNvalueWcs, &value, ZIL_NULLP(void));
#else
		XtVaGetValues(screenID, XmNvalue, &value, ZIL_NULLP(void));
#endif
		nargs = 0;
#if defined(ZIL_UNICODE)
		char *mb = MapText(text);
		wchar_t *wText = new wchar_t[strlen(text) + 1];
		::mbstowcs(wText, mb, strlen(text) + 1);
		XtSetArg(args[nargs], XmNvalueWcs, wText), nargs++;
		delete mb;
#else
		if (strcmp(text, value))
			XtSetArg(args[nargs], XmNvalue, text), nargs++;
#endif
		XtSetValues(screenID, args, nargs);
	}
}

void UIW_COMBO_BOX::OSDestroy(void)
{
	// No code necessary.
}

void UIW_COMBO_BOX::OSMatchInput(void)
{
	// No code available.
}

void UIW_COMBO_BOX::OSResetSelection(UI_WINDOW_OBJECT *object)
{
	if (!object)
		return;
	else if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
	{
		if (object->screenID &&
			XtIsSubclass(object->screenID, xmLabelWidgetClass))
		{
			Pixmap pixmap;
			XmString string;
			unsigned char type;
			XtVaGetValues(object->screenID, XmNlabelPixmap, &pixmap,
				XmNlabelString, &string, XmNlabelType, &type,
				ZIL_NULLP(void));
			XtVaSetValues(screenID, XmNlabelPixmap, pixmap,
				XmNlabelString, string, XmNlabelType, type,
				ZIL_NULLP(void));
		}
	}
	else
	{
		Information(I_SET_TEXT, object ?
			object->Information(I_GET_TEXT, ZIL_NULLP(void)) : _blankString);
	}
}

void UIW_COMBO_BOX::OSSort(void)
{
	if (screenID)
	{
		Event(UI_EVENT(S_DEINITIALIZE));
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

void UIW_COMBO_BOX::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_COMBO_BOX && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
