//	Zinc Interface Library - M_BUTTON.CPP
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
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
}

extern ZIL_ICHAR *_bitmapDirectory;

// Some Motif 1.1s can't draw their bitmaps off the left side of the button
// if the bitmap is added after the button is created.  PIXMAP_BUG fixes this,
// with the one problem of using the parent's background color instead of the
// button's.
#if defined(_SUNOS4) || defined(_IBM_RS6000) || defined(__DVX__) || defined(__sgi) || defined(__DECCXX) || defined(__linux__)
#	define PIXMAP_BUG
#endif

// ----- UIW_BUTTON ---------------------------------------------------------

static void ToggleButtonCallback(Widget, XtPointer client, XtPointer call)
{
	UIW_BUTTON *object = (UIW_BUTTON *)client;
	XmToggleButtonCallbackStruct *structPtr
		= (XmToggleButtonCallbackStruct *)call;
	// Route an event thru the queue.
	UI_EVENT rEvent(S_ROUTE_EVENT);
	rEvent.event = new UI_EVENT(S_VERIFY_STATUS);
	rEvent.routeTarget = object;
	object->eventManager->Put(rEvent);
}

static XtIntervalId _timerID = 0;

void ButtonRepeatCallback(XtPointer client_data, XtIntervalId *)
{
	UIW_BUTTON *button = (UIW_BUTTON *)client_data;

	button->Event(UI_EVENT(L_SELECT));
	_timerID = XtAppAddTimeOut(
		XtWidgetToApplicationContext(button->display->topShell),
		button->repeatRate, ButtonRepeatCallback, client_data);
}

EVENT_TYPE UIW_BUTTON::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_REGION region = trueRegion;
	region.right -= 4; region.bottom -= 4;

	/* unused parameters */ (void)event;

	// Handle case of parent also owner draw.
	ZIL_SCREENID tScreenID = screenID;
	UI_WINDOW_OBJECT *tParent = parent;
	while (!tScreenID && tParent)
	{
		tScreenID = tParent->screenID;
		region.left += tParent->trueRegion.left;
		region.top += tParent->trueRegion.top;
		region.right += tParent->trueRegion.left;
		region.bottom += tParent->trueRegion.top;
		tParent = tParent->parent;
	}
	if (!tScreenID)
		return ccode;

	if (FlagSet(btFlags, BTF_CHECK_BOX))
	{
		UI_REGION check = region;
		check.left += 3;
		check.top += 5;
		check.right = check.left + 12;
		check.bottom = check.top + 12;
		if (FlagSet(woStatus, WOS_SELECTED))
			DrawShadow(tScreenID, check, 2, FALSE, ccode);
		else
			DrawShadow(tScreenID, check, -2, FALSE, ccode);
	}
	return(ccode);
}

//	ZincToXPixmap is an unsupported function that may be changed or
//	removed in future releases.
void ZincToXPixmap(UI_DISPLAY *display, int bitmapWidth, int bitmapHeight,
	ZIL_UINT8 *bitmapArray, UI_PALETTE *paletteArray, Pixmap *pixmap,
	UI_WINDOW_OBJECT *object, ZIL_ICHAR *text, int font, WOF_FLAGS woFlags,
	int &width, int &height, int offset = 0);

EVENT_TYPE UIW_BUTTON::Event(const UI_EVENT &event)
{
	static int initializedTime = FALSE;
	static ZIL_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_BUTTON);
	switch (ccode)
	{
	case L_BEGIN_SELECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(btFlags, BTF_REPEAT))
			_timerID = XtAppAddTimeOut(
				XtWidgetToApplicationContext(display->topShell),
				delayLength, ButtonRepeatCallback, (XtPointer)this);
		break;

	case L_END_SELECT:
		if (FlagSet(btFlags, BTF_REPEAT) && _timerID)
		{
			// Turn off timer.
			XtRemoveTimeOut(_timerID);
			_timerID = 0;
		}

		if (event.type == E_MOTIF)
		{
			if (event.message.xbutton.x < 0 || event.message.xbutton.y < 0)
				break;
			Window root;
			int x, y;
			unsigned int width, height, b, d;
			if (!screenID)
				break;
			XGetGeometry(display->xDisplay, XtWindow(screenID), &root,
				&x, &y, &width, &height, &b, &d);
			if (event.message.xbutton.x >= width ||
					event.message.xbutton.y >= height)
				break;
		}
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		// Fall thru.

	case L_SELECT:
		if (event.type == E_MOTIF && ccode != L_SELECT)
		{
			// Route an event thru the queue.
			UI_EVENT rEvent(S_ROUTE_EVENT);
			rEvent.event = new UI_EVENT(ccode);
			rEvent.routeTarget = this;
			eventManager->Put(rEvent);
			break;
		}
		else
		{
			ZIL_TIME currentTime;
			long elapsedTime = currentTime - lastTime;
			if (FlagSet(woStatus, WOS_OWNERDRAW))
				woStatus |= WOS_REDISPLAY;
			if (parent)
				parent->Information(I_CHECK_SELECTION, ZIL_NULLP(void));
			if (FlagSet(btFlags, BTF_DOUBLE_CLICK) && ccode == L_END_SELECT &&
				userFunction && !parent->Inherited(ID_LIST) &&
				AbsValue(elapsedTime) < doubleClickRate)
			{
				UI_EVENT uEvent = event;
				ccode = (*userFunction)(this, uEvent, L_DOUBLE_CLICK);
			}
			else if (ccode == L_END_SELECT)
			{
				ccode = UserFunction(UI_EVENT(L_END_SELECT), L_SELECT);
				lastTime = currentTime;
			}
			else
				ccode = UserFunction(event, L_SELECT);
		}

		if (FlagsSet(woStatus, WOS_OWNERDRAW | WOS_REDISPLAY))
		{
			Event(UI_EVENT(S_REDISPLAY));
		}
		break;

	case S_INITIALIZE:
		if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent)
			parent->Information(I_SET_DEFAULT_OBJECT, this);
		if (!bitmapArray && bitmapName && defaultStorage && !defaultStorage->storageError)
		{
			defaultStorage->ChDir(_bitmapDirectory);
			ZIL_STORAGE_OBJECT_READ_ONLY bFile(*defaultStorage, bitmapName, ID_BITMAP_IMAGE);
			if (!bFile.objectError)
			{
				ZIL_INT16 _value; bFile.Load(&_value); bitmapWidth = _value;
				bFile.Load(&_value); bitmapHeight = _value;
				bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
				bFile.Load(bitmapArray, sizeof(ZIL_UINT8), bitmapWidth * bitmapHeight);
			}
		}
		// Continue to S_CREATE.

	case S_CHANGED:
	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent && ccode != S_INITIALIZE)
		{
			trueRegion.right -= 10;
			trueRegion.bottom -= 10;
		}
		if (FlagSet(btFlags, BTF_AUTO_SIZE))
		{
			int offset = 4;
			if (FlagSet(woFlags, WOF_BORDER))
				offset += 2;
			if (!FlagSet(btFlags, BTF_NO_3D))
				offset += 4;
			int height = 0;
			if (bitmapHeight)
			{
				height = bitmapHeight + offset;
				if (text && *text && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
					height += display->cellHeight;
			}
			height = MaxValue(height, display->cellHeight * 10 / 9);
			relative.top = relative.bottom - height;
			if (relative.top < 0)
			{
				relative.top = 0;
				relative.bottom = height;
			}
			trueRegion.top = trueRegion.bottom - (relative.bottom - relative.top);
		}
		
		if (ccode == S_INITIALIZE || ccode == S_CHANGED)
			break;
		// Fall thru.
	
	case S_REGISTER_OBJECT:
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			if (FlagSet(woStatus, WOS_OWNERDRAW))
				UI_WINDOW_OBJECT::Event(UI_EVENT(S_REGISTER_OBJECT));
			break;
		}

		{
		ZIL_ICHAR *displayText;
		XmString labelText = CreateMotifString(text, &displayText);

		nargs = 0;
		if (hotKey)
			XtSetArg(args[nargs], XmNmnemonic, hotKey), nargs++;
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_END), nargs++;
		else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_CENTER), nargs++;
		else
			XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_BEGINNING), nargs++;
		{
		if (FlagSet(btFlags, BTF_NO_3D))
			XtSetArg(args[nargs], XmNshadowThickness, 0), nargs++;
		XtSetArg(args[nargs], XmNrecomputeSize, FALSE); nargs++;

		if (!bitmapArray)
			XtSetArg(args[nargs], XmNlabelString, labelText), nargs++;
		else
		{
			int width = trueRegion.Width();
			int height = trueRegion.Height();
			int offset = parent && parent->Inherited(ID_VT_LIST) ?
				relative.left : 0;
			ZincToXPixmap(display, bitmapWidth, bitmapHeight,
				bitmapArray, ZIL_NULLP(UI_PALETTE), &colorBitmap, parent, text,
				font, woFlags, width, height, offset);
			XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
			XtSetArg(args[nargs], XmNlabelPixmap, colorBitmap); nargs++;
			XtSetArg(args[nargs], XmNlabelInsensitivePixmap, colorBitmap); nargs++;
		}

		if (FlagSet(btFlags, BTF_CHECK_BOX))
		{
			XtSetArg(args[nargs], XmNindicatorType, XmN_OF_MANY); nargs++;
			XtSetArg(args[nargs], XmNset,
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0); nargs++;
#if defined(ZIL_UNICODE)
			// Work around bug with XFontSets.
			XtSetArg(args[nargs], XmNindicatorSize,
				display->cellHeight - 14 - 1); nargs++;
#endif
			RegisterObject(ZIL_NULLP(_WidgetClassRec),
				(ZIL_MOTIF_CONVENIENCE_FUNCTION)XmCreateToggleButton, ccode, TRUE);

			// don't let the parent rowcolumn enforce radioBehavior
			nargs = 0;
			XtSetArg(args[nargs], XmNradioBehavior, FALSE); nargs++;
			XtSetValues(XtParent(screenID), args, nargs);
			if (ccode != S_CHANGED)
			{
				XtAddCallback(screenID, XmNvalueChangedCallback,
					ToggleButtonCallback, (XtPointer)this);
			}
		}
		else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
		{
			XtSetArg(args[nargs], XmNindicatorType, XmONE_OF_MANY); nargs++;
			XtSetArg(args[nargs], XmNset,
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0); nargs++;
#if defined(ZIL_UNICODE)
			// Work around bug with XFontSets.
			XtSetArg(args[nargs], XmNindicatorSize,
				display->cellHeight - 14 - 1); nargs++;
#endif
			RegisterObject(ZIL_NULLP(_WidgetClassRec),
				(ZIL_MOTIF_CONVENIENCE_FUNCTION)XmCreateToggleButton, ccode, TRUE);
			if (ccode != S_CHANGED)
			{
				XtAddCallback(screenID, XmNvalueChangedCallback,
					ToggleButtonCallback, (XtPointer)this);
			}
		}
		else if (!FlagSet(btFlags, BTF_NO_TOGGLE | BTF_CHECK_BOX | BTF_RADIO_BUTTON | BTF_NO_3D) && !Inherited(ID_MENU_ITEM))
		{
			if (colorBitmap)
			{
				XtSetArg(args[nargs], XmNselectPixmap, colorBitmap); nargs++;
				XtSetArg(args[nargs], XmNlabelPixmap, colorBitmap); nargs++;
			}
			XtSetArg(args[nargs], XmNshadowThickness, 2); nargs++;
			XtSetArg(args[nargs], XmNindicatorOn, FALSE); nargs++;
			XtSetArg(args[nargs], XmNset,
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0); nargs++;
			RegisterObject(ZIL_NULLP(_WidgetClassRec),
				(ZIL_MOTIF_CONVENIENCE_FUNCTION)XmCreateToggleButton, ccode, TRUE);
			nargs = 0;
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			XtSetArg(args[nargs], XmNradioBehavior,
				FlagSet(wnFlags, WNF_SELECT_MULTIPLE) ? FALSE : TRUE); nargs++;
			XtSetValues(XtParent(screenID), args, nargs);
			XtVaSetValues(screenID, XmNvisibleWhenOff, TRUE, ZIL_NULLP(void));

			if (ccode != S_CHANGED)
			{
				XtAddCallback(screenID, XmNvalueChangedCallback,
					ToggleButtonCallback, (XtPointer)this);
			}
		}
		else if (FlagSet(woStatus, WOS_OWNERDRAW) && !screenID)
			UI_WINDOW_OBJECT::Event(UI_EVENT(S_REGISTER_OBJECT));
		else
		{
			if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent)
			{
				XtSetArg(args[nargs], XmNdefaultButtonShadowThickness, 1); nargs++;
				XtSetArg(args[nargs], XmNshowAsDefault, TRUE); nargs++;
			}
			XtSetArg(args[nargs], XmNfillOnArm, FALSE); nargs++;
			RegisterObject(xmPushButtonWidgetClass, ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE);
		}

		if (parent && FlagSet(parent->woFlags, WOF_NON_SELECTABLE))
			XtSetSensitive(screenID, FALSE);

#if !defined(PIXMAP_BUG)
		if (bitmapArray)
		{
			int width = trueRegion.Width();
			int height = trueRegion.Height();
			int offset = parent && parent->Inherited(ID_VT_LIST) ?
				relative.left : 0;
			ZincToXPixmap(display, bitmapWidth, bitmapHeight,
				bitmapArray, ZIL_NULLP(UI_PALETTE), &colorBitmap, this, text,
				font, woFlags, width, height, offset);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
			XtSetArg(args[nargs], XmNlabelPixmap, colorBitmap); nargs++;
			XtSetArg(args[nargs], XmNlabelInsensitivePixmap, colorBitmap); nargs++;
			XtSetValues(screenID, args, nargs);
		}
#endif
		if (labelText)
			XmStringFree(labelText);
		}
		}
		break;

	case S_VERIFY_STATUS:
		if (FlagSet(btFlags, BTF_RADIO_BUTTON | BTF_CHECK_BOX) ||
			(!FlagSet(btFlags, BTF_NO_TOGGLE | BTF_CHECK_BOX |
			BTF_RADIO_BUTTON | BTF_NO_3D) && !Inherited(ID_MENU_ITEM)))
			XmToggleButtonSetState(screenID,	
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0, FALSE);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(btFlags, BTF_RADIO_BUTTON | BTF_CHECK_BOX))
		{
			// Route an event thru the queue.
			UI_EVENT rEvent(S_ROUTE_EVENT);
			rEvent.event = new UI_EVENT(S_VERIFY_STATUS);
			rEvent.routeTarget = this;
			eventManager->Put(rEvent);
		}
		break;

	case S_REDISPLAY:
		if (screenID && bitmapArray && !colorBitmap)
		{
			ZIL_ICHAR *displayText;
			XmString labelText = CreateMotifString(text, &displayText);
			nargs = 0;
			int width = trueRegion.Width();
			int height = trueRegion.Height();
			int offset = parent && parent->Inherited(ID_VT_LIST) ?
				relative.left : 0;
			ZincToXPixmap(display, bitmapWidth, bitmapHeight,
				bitmapArray, ZIL_NULLP(UI_PALETTE), &colorBitmap, parent, text,
				font, woFlags, width, height, offset);
			XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
			XtSetArg(args[nargs], XmNlabelPixmap, colorBitmap); nargs++;
			XtSetArg(args[nargs], XmNlabelInsensitivePixmap, colorBitmap); nargs++;
			XtSetValues(screenID, args, nargs);
		}
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_LEFT:
	case L_RIGHT:
	case L_PREVIOUS:
	case L_NEXT:
		ccode = parent->Event(event);
		break;

	case S_DEINITIALIZE:
		if (colorBitmap && screenID)	// Don't delete pixmap button in its callback.
			XtVaSetValues(screenID, XmNlabelType, XmSTRING, NULL);
		ccode = UI_WINDOW_OBJECT::Event(event);
		display->DestroyBitmapHandle(screenID, &colorBitmap, &monoBitmap);
		if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent)
			parent->Information(I_SET_DEFAULT_OBJECT, ZIL_NULLP(UI_WINDOW_OBJECT));
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_BUTTON::OSDataSet(void)
{
	if (screenID)
	{
		XmString labelText = CreateMotifString(text);
		nargs = 0;
		XtSetArg(args[nargs], XmNlabelString, labelText); nargs++;
		XtSetValues(screenID, args, nargs);
		if (trueRegion.bottom > trueRegion.top + 1)
			XtSetArg(args[nargs], XmNheight, trueRegion.bottom - trueRegion.top + 1),
				nargs++;
		if (trueRegion.right > trueRegion.left + 1)
			XtSetArg(args[nargs], XmNwidth, trueRegion.right - trueRegion.left + 1),
				nargs++;
		if (labelText)
			XmStringFree(labelText);
		Event(UI_EVENT(S_REDISPLAY));
		if (FlagSet(btFlags, BTF_RADIO_BUTTON | BTF_CHECK_BOX) ||
			(!FlagSet(btFlags, BTF_NO_TOGGLE | BTF_CHECK_BOX |
			BTF_RADIO_BUTTON | BTF_NO_3D) && !Inherited(ID_MENU_ITEM)))
			XmToggleButtonSetState(screenID,	
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0, FALSE);
	}
}

void UIW_BUTTON::OSImageInitialize(void)
{
	if (!colorBitmap && bitmapArray && screenID)
	{
		ZIL_ICHAR *displayText;
		XmString labelText = CreateMotifString(text, &displayText);
		int width = trueRegion.Width();
		int height = trueRegion.Height();
		int offset = parent && parent->Inherited(ID_VT_LIST) ?
			relative.left : 0;
		ZincToXPixmap(display, bitmapWidth, bitmapHeight,
			bitmapArray, ZIL_NULLP(UI_PALETTE), &colorBitmap, this, text,
			font, woFlags, width, height, offset);
		nargs = 0;
		XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
		XtSetArg(args[nargs], XmNlabelPixmap, colorBitmap); nargs++;
		XtSetArg(args[nargs], XmNlabelInsensitivePixmap, colorBitmap); nargs++;
		XtSetValues(screenID, args, nargs);
		if (labelText)
			XmStringFree(labelText);
	}
}

void UIW_BUTTON::OSSystemImage(void)
{
}

void UIW_BUTTON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_BUTTON && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
