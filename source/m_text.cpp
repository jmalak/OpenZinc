//	Zinc Interface Library - M_TEXT.CPP
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


#define USE_RAW_KEYS
#include "ui_win.hpp"


extern "C"
{
#if defined(ZIL_UNICODE)
#	include <stdlib.h>
#endif
#include <Xm/Text.h>
}

// ----- UIW_TEXT -----------------------------------------------------------

int UIW_TEXT::CursorOffset(int offset)
{
	if (offset == -1)
	{
		XmTextPosition textOffset = XmTextGetCursorPosition(screenID);
		offset = (int)textOffset;
	}
	else
	{
		if (offset > strlen(text))
			offset = strlen(text);
		XmTextPosition textOffset = (XmTextPosition)offset;
		XmTextShowPosition(screenID, textOffset);
		XmTextSetCursorPosition(screenID, textOffset);
	}
	return (offset);
}

void UIW_TEXT::GetCursorPos(UI_POSITION *position)
{
	if (position)
	{
		position->column = position->line = 0;
		Position x = 0;
		Position y = 0;
		XmTextPosition textOffset = XmTextGetCursorPosition(screenID);
		XmTextPosToXY(screenID, textOffset, &x, &y);
		position->column = x;
		position->line = y;
	}
}

void UIW_TEXT::SetCursorPos(const UI_POSITION &position)
{
	XmTextPosition textOffset = XmTextXYToPos(screenID, position.column, position.line);
	XmTextShowPosition(screenID, textOffset);
	XmTextSetCursorPosition(screenID, textOffset);
}

EVENT_TYPE UIW_TEXT::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Draw the string item.
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

	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(tScreenID, region, FALSE, ccode);

	region.left += 5;	// XmNmarginWidth.
	region.top += 1;
	DrawText(tScreenID, region, text, lastPalette, FALSE, ccode);

	woStatus &= ~WOS_REDISPLAY;
	return (ccode);
}


// If a hotkey is pressed from within a text widget and Motif doesn't recognize
// it as a hot key (ie, it's for a button or prompt), Motif will put the alt
// key sequence in to the text.  So UIW_WINDOW sends us a S_VERIFY_STATUS
// when an alt-key sequence is recognized as a hot key, so the StringCallback
// can know to ignore it.  Unfortunately, sometimes the keystroke is already
// in the text widget before Zinc knows about it.  _wasHotKey is a
// convoluted way of flagging hotkeys so they can be rejected or deleted.
enum T_ANSWER {
	no, yes, maybe
};
static T_ANSWER _wasHotKey = maybe;

//	TextCallback is called before each character or string is added
//	to the UIW_TEXT to handle hotkeys.
static void TextCallback(Widget, XtPointer client, XtPointer call)
{
	UIW_TEXT *object = (UIW_TEXT *)client;
	XmAnyCallbackStruct *structPtr = (XmAnyCallbackStruct *)call;
	switch(structPtr->reason)
	{
	case XmCR_MODIFYING_TEXT_VALUE:
		{
		object->woStatus |= WOS_CHANGED;
		XmTextVerifyPtr verifyData = (XmTextVerifyPtr)call;
		XmTextBlock block = verifyData->text;
		verifyData->doit = TRUE;

		if (structPtr->event && structPtr->event->type == KeyPress)
		{
			if (structPtr->event->xkey.state & Mod1Mask && block->length)
			{
				if (_wasHotKey == yes)
				{
					verifyData->doit = FALSE;
					_wasHotKey = no;
				}
				else
					_wasHotKey = maybe;
			}
			else
				_wasHotKey = no;
		}
		break;
		}
	}
}

extern int _lastFocusEvent;

EVENT_TYPE UIW_TEXT::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_TEXT);
	if (event.type == E_MOTIF)
	{
		switch(event.message.type)
		{
		case KeyPress:
			// TEXT derived from WINDOW. Don't let base handle hotkeys.
			if (event.rawCode == XK_Return || event.rawCode == XK_KP_Enter)
				break;
			if (ccode != L_HELP)
				return UI_WINDOW_OBJECT::Event(event);
		}
	}

	// Switch on the event type.
	UI_WINDOW_OBJECT *object;
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, 0, 0,
			trueRegion.right - trueRegion.left, trueRegion.bottom - trueRegion.top));
		for (object = (UI_WINDOW_OBJECT *)support.First();
			object; object = object->Next())
		{
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
		}

		if ((vScroll || hScroll) && ccode == S_CHANGED)
		{
			nargs = 0;
			XtSetArg(args[nargs], XmNx, trueRegion.left); nargs++;
			XtSetArg(args[nargs], XmNy, trueRegion.top); nargs++;
			XtSetArg(args[nargs], XmNheight, trueRegion.bottom - trueRegion.top + 1);
				nargs++;
			XtSetArg(args[nargs], XmNwidth, trueRegion.right - trueRegion.left + 1);
				nargs++;
			XtSetValues(XtParent(screenID), args, nargs);
			break;
		}

		if (ccode == S_CHANGED && !FlagSet(woFlags, WOF_NON_FIELD_REGION))
			break;
		// Fall thru.

	case S_REGISTER_OBJECT:
		nargs = 0;
		XtSetArg(args[nargs], XmNresizeHeight, FALSE); nargs++;
		XtSetArg(args[nargs], XmNresizeWidth, FALSE); nargs++;
		XtSetArg(args[nargs], XmNeditMode, XmMULTI_LINE_EDIT); nargs++;
		XtSetArg(args[nargs], XmNmarginHeight, 1); nargs++;
		XtSetArg(args[nargs], XmNmarginWidth, 3); nargs++;

		if (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			woFlags |= WOF_VIEW_ONLY;	// Prevent Motif warning.
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		{
			XtSetArg(args[nargs], XmNeditable, FALSE), nargs++;
			XtSetArg(args[nargs], XmNcursorPositionVisible, FALSE), nargs++;
		}
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			XtSetArg(args[nargs], XmNeditable, FALSE), nargs++;
			XtSetArg(args[nargs], XmNcursorPositionVisible, FALSE), nargs++;
		}

		XtSetArg(args[nargs], XmNwordWrap,
			FlagSet(wnFlags, WNF_NO_WRAP) ? FALSE : TRUE); nargs++;
		XtSetArg(args[nargs], XmNverifyBell, FALSE); nargs++;
		if (vScroll || hScroll)
		{
			XtSetArg(args[nargs], XmNscrollHorizontal, hScroll ? TRUE : FALSE);
				nargs++;
			XtSetArg(args[nargs], XmNscrollVertical, vScroll ? TRUE : FALSE);
				nargs++;
			RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateScrolledText,
				ccode, TRUE);
			Widget vertScroll = ZIL_NULLP(_WidgetRec),
				horizScroll = ZIL_NULLP(_WidgetRec);
			XtVaGetValues(XtParent(screenID),
					XmNhorizontalScrollBar, &horizScroll,
					XmNverticalScrollBar, &vertScroll, NULL);
			if (hScroll)
					hScroll->screenID = horizScroll;
			if (vScroll)
					vScroll->screenID = vertScroll;
		}
		else
		{
			XtSetArg(args[nargs], XmNshadowThickness,
				FlagSet(woFlags, WOF_BORDER) ? 2 : 0); nargs++;
			XtSetArg(args[nargs], XmNscrollHorizontal, FALSE); nargs++;
			XtSetArg(args[nargs], XmNscrollVertical, FALSE); nargs++;
			RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateScrolledText, ccode, TRUE);
			// RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateText,
			//		ccode, TRUE);
		}
		if (ccode != S_CHANGED)
			DataSet(text, maxLength);

		if (ccode != S_CHANGED)
		{
			XtAddCallback(screenID, XmNmodifyVerifyCallback,
				TextCallback, (XtPointer)this);
		}
		if (ccode == S_REGISTER_OBJECT)
			break;

		for (object = (UI_WINDOW_OBJECT *)support.First();
			object; object = object->Next())
		{
			object->Event(event);
		}
		
		break;

	case S_VERIFY_STATUS:
		if (_wasHotKey == no)
			_wasHotKey = yes;
		else if (_wasHotKey == maybe)
		{
			Widget w = (Widget)event.rawCode;
			if (w)
			{
				XmTextPosition pos = XmTextGetInsertionPosition(w);
				woStatus |= WOS_INTERNAL_ACTION;
				XmTextReplace(w, MaxValue(0, pos-1) , pos, "");
				woStatus &= ~WOS_INTERNAL_ACTION;
			}
			_wasHotKey = no;
		}
		break;


	case S_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (screenID && FlagSet(woFlags, WOF_AUTO_CLEAR)
#if (ZIL_MOTIF > 1001)
			&& !FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE)
#endif
			)
		{
			if (_lastFocusEvent != ButtonPress)
				XmTextSetSelection(screenID, 0,
					XmTextGetLastPosition(screenID), 0);
		}
		break;

	case S_NON_CURRENT:
		DataGet();
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (screenID && FlagSet(woFlags, WOF_AUTO_CLEAR)
#if (ZIL_MOTIF > 1001)
			&& !FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE)
#endif
			)
		{
			XmTextClearSelection(screenID, 0);
		}
		break;

	case S_SUBTRACT_OBJECT:
	case S_ADD_OBJECT:
		UIW_WINDOW::Event(event);
		break;

	case E_KEY:
		{
#if defined(ZIL_UNICODE)
		char *buf = MapChar(event.key.value);
#else
		char buf[2];
		buf [0] = (char)event.key.value;
		buf[1] = 0;
#endif
		XmTextInsert(screenID, XmTextGetInsertionPosition(screenID), buf);
#if (ZIL_MOTIF > 1001)
		XmTextSetInsertionPosition(screenID,
			XmTextGetInsertionPosition(screenID) + 1);
#endif
		}
		break;

	case L_SELECT:
		ccode = UserFunction(event, L_SELECT);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TEXT::OSDataGet(void)
{
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
	{
#	if defined(ZIL_UNICODE)
		wchar_t *wText = XmTextGetStringWcs(screenID);
		for (wchar_t *wPtr = wText; *wPtr; wPtr++)
			;
		char *mb = new char[4 * (++wPtr - wText)];
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

void UIW_TEXT::OSDataSet(void)
{
	// kill CR/LF characters.
	if (text)
		strstrip(text, '\r');

	// Redisplay the text field.
	if (screenID)
	{
		nargs = 0;
		if (maxLength != -1)
			XtSetArg(args[nargs], XmNmaxLength, maxLength), nargs++;
#if defined(ZIL_UNICODE)
		char *mb = MapText(text);
		wchar_t *wText = new wchar_t[strlen(text) + 1];
		::mbstowcs(wText, mb, strlen(text) + 1);
		XtSetArg(args[nargs], XmNvalueWcs, wText); nargs++;
		delete mb;
#else
		XtSetArg(args[nargs], XmNvalue, text); nargs++;
#endif
		XtSetValues(screenID, args, nargs);
#if defined(ZIL_UNICODE)
		delete [] wText;
#endif
	}
}

void UIW_TEXT::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TEXT && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
