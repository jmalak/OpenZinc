//	Zinc Interface Library - M_STRING.CPP
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
#if defined(ZIL_UNICODE)
#	include <stdlib.h>
#endif
#include <Xm/DrawnB.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
}

const int STRING_OFFSET = 4;

// ----- UIW_STRING ---------------------------------------------------------

EVENT_TYPE UIW_STRING::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Draw the string item.
	UI_REGION region = trueRegion;
	region.right -= 4; region.bottom -= 4;

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

//	display->Rectangle(tScreenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(tScreenID, region, FALSE, ccode);
	if (FlagSet(stFlags, STF_PASSWORD))	// Check for a password.
	{
		woStatus &= ~WOS_REDISPLAY;
		return (ccode);
	}
//	else if (!display->isText)
//	{
//		region.left += STRING_OFFSET;
//		region.right -= STRING_OFFSET;
//	}
	region.left += 5;	// XmNmarginWidth.
	region.top += 1;
	DrawText(tScreenID, region, text, lastPalette, FALSE, ccode);

	// Draw Focus rectangle if current list item.
	if (ccode == S_CURRENT && !display->isText && parent->Inherited(ID_LIST) &&
		FlagSet(woFlags, WOF_VIEW_ONLY))
	{
		UI_REGION focus = trueRegion;
		focus.left = MaxValue(trueRegion.left, clip.left);
		focus.top = MaxValue(trueRegion.top, clip.top);
		focus.right = MinValue(trueRegion.right, clip.right);
		focus.bottom = MinValue(trueRegion.bottom, clip.bottom);
		display->Rectangle(tScreenID, focus, lastPalette, -1);
	}
	woStatus &= ~WOS_REDISPLAY;
	return (ccode);
}


// If a hotkey is pressed from within a text widget and Motif doesn't recognize
// it as a hot key (ie, it's for a button or prompt), Motif will put the alt
// key sequence in to the text.  So UIW_WINDOW sends us a S_VERIFY_STATUS
// when an alt-key sequence is recognized as a hot key, so the StringCallback
// can know to ignore it.  Unfortunately, sometimes the keystroke is already
// in the text widget before Zinc knows about it.  _hotKeyPressed is a
// convoluted way of flagging hotkeys so they can be rejected or deleted.
enum S_ANSWER {
	no, yes, maybe
};
static S_ANSWER _hotKeyPressed = no;

//	StringCallback is called before each character or string is added
//	to the UIW_STRING.  The input can be altered if necessary.
//	It also tracks focus and movement.
static void StringCallback(Widget, XtPointer client, XtPointer call)
{
	UIW_STRING *object = (UIW_STRING *)client;
	XmAnyCallbackStruct *structPtr = (XmAnyCallbackStruct *)call;
	switch(structPtr->reason)
	{
	case XmCR_MODIFYING_TEXT_VALUE:
		{
		XmTextVerifyPtr verifyData = (XmTextVerifyPtr)call;
		XmTextBlock block = verifyData->text;
		verifyData->doit = TRUE;

		if (structPtr->event)	// Set WOS_CHANGED only in response to X event.
			object->woStatus |= WOS_CHANGED;

		if (FlagSet(object->stFlags, STF_PASSWORD))
		{
			// Store value in .text, but tell Motif to show '******'.
			int maxLen;
			object->Information(I_GET_MAXLENGTH, &maxLen);
			char *text = (char *)object->Information(I_GET_TEXT, ZIL_NULLP(void));
			char *targetStart = &text[verifyData->startPos];
			char *targetEnd   = &text[verifyData->endPos];
			char *sourceStart = block->ptr;
			char *sourceEnd   = &block->ptr[block->length];
			long verifyLength = verifyData->endPos - verifyData->startPos;
			int oldLength = strlen(text);

			// Backspace.
			if (block->length == 0 && verifyData->startPos < verifyData->currInsert)
			{
				if (verifyData->endPos != strlen(text))
				{
					memmove(targetStart, targetEnd, (int)(maxLen - verifyData->endPos));
					text[oldLength - 1] = 0;
				}
				else
					text[verifyData->startPos] = 0;
				return;
			}

			// Insert.
			if ((block->length != strlen(text)) ||
				(verifyLength == 0 && verifyData->startPos != 0))
			{
				char *insertPos = targetEnd;
				targetEnd = targetStart + (sourceEnd - sourceStart);
				memmove(targetEnd, insertPos, maxLen - (targetEnd - text));
			}

			// Replace.
			int replaceLength = targetEnd - targetStart;
			while (replaceLength--)
				*targetStart++ = *sourceStart++;

			// Compress.
			int newLength = strlen(text);
			if (newLength > oldLength && verifyData->endPos != oldLength)
				text[oldLength + 1] = 0;
			else if ((block->length == strlen(text)) ||
				(verifyLength == 0 && newLength == oldLength))
				;
			else if ((verifyLength == 0) || (verifyLength == strlen(text)))
				text[verifyData->startPos + 1] = 0;
		}

		// Force the text widget to show its first character.
		if (verifyData->startPos == 0)
			XmTextShowPosition(object->screenID, verifyData->startPos);

		for (char *ch = block->ptr; ch - block->ptr < block->length; ch++)
		{
			// Convert each character.
			if (FlagSet(object->stFlags, STF_LOWER_CASE) &&
				object->IsUpper(*ch))
				*ch = object->ToLower(*ch);
			if (FlagSet(object->stFlags, STF_UPPER_CASE) &&
				object->IsLower(*ch))
				*ch = object->ToUpper(*ch);
			if (FlagSet(object->stFlags, STF_VARIABLE_NAME) && *ch == ' ')
				*ch = '_';
			if (FlagSet(object->stFlags, STF_PASSWORD))
				*ch = '*';
		}
		if (structPtr->event && structPtr->event->type == KeyPress)
		{
			if (structPtr->event->xkey.state & Mod1Mask && block->length)
			{
				if (_hotKeyPressed == yes)
				{
					verifyData->doit = FALSE;
					_hotKeyPressed = no;
				}
				else
					_hotKeyPressed = maybe;
			}
			else
				_hotKeyPressed = no;
		}
		break;
		}
	}
}

extern int _lastFocusEvent;

EVENT_TYPE UIW_STRING::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STRING);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CREATE:
	case S_REGISTER_OBJECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (trueRegion.Height() < display->cellHeight)
			trueRegion.bottom = trueRegion.top + display->cellHeight;
		if (parent->Inherited(ID_LIST))
		{
			woFlags &= ~WOF_BORDER;
			woFlags |= WOF_VIEW_ONLY;
		}
#if (ZIL_MOTIF > 1001)
		if (FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE))
		{
			// make a label, not a string.
			nargs = 0;
			XmString labelText = CreateMotifString(text);
			XtSetArg(args[nargs], XmNlabelString, labelText); nargs++;
			XtSetArg(args[nargs], XmNtraversalOn, TRUE); nargs++;
			XtSetArg(args[nargs], XmNhighlightThickness, 2); nargs++;
			if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
				XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_CENTER), nargs++;
			else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_END), nargs++;
			else
				XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_BEGINNING), nargs++;
			XtSetArg(args[nargs], XmNmarginWidth, 5); nargs++;
			XtSetArg(args[nargs], XmNrecomputeSize, 0); nargs++;
			XtSetArg(args[nargs], XmNshadowType, XmSHADOW_IN); nargs++;
			if (!FlagSet(woFlags, WOF_BORDER))
				XtSetArg(args[nargs], XmNshadowThickness, 0), nargs++;
			RegisterObject(xmDrawnButtonWidgetClass,
				ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE);
			if (labelText)
				XmStringFree(labelText);
			break;
		}
#endif

		if (ccode == S_CREATE && FlagSet(woStatus, WOS_UNANSWERED))
			strcpy(text, _blankString);
		nargs = 0;
		{
			// Center text vertically.
			static ZIL_ICHAR heightText[] = { 'M','W','I','q','y','j', 0 };
			int marginHeight = trueRegion.Height() -
				display->TextHeight(heightText, ID_SCREEN, font) - 4;
			if (FlagSet(woFlags, WOF_BORDER))
				marginHeight -= 4;
			marginHeight = MaxValue(marginHeight / 2, 1);
			XtSetArg(args[nargs], XmNmarginHeight, marginHeight); nargs++;
		}
		if (!FlagSet(woFlags, WOF_BORDER))
			XtSetArg(args[nargs], XmNshadowThickness, 0), nargs++;
		if (maxLength != -1)
			XtSetArg(args[nargs], XmNmaxLength, maxLength), nargs++;
		if (FlagSet(woFlags, WOF_VIEW_ONLY) ||
			FlagSet(parent->woAdvancedFlags, WOAF_NON_CURRENT) ||
			FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) ||
			FlagSet(woFlags, WOF_NON_SELECTABLE))
		{
			XtSetArg(args[nargs], XmNeditable, FALSE), nargs++;
			XtSetArg(args[nargs], XmNcursorPositionVisible, FALSE), nargs++;
		}
		XtSetArg(args[nargs], XmNverifyBell, FALSE); nargs++;
		if (parent && (parent->Inherited(ID_LIST) ||
			parent->Inherited(ID_GROUP) || parent->Inherited(ID_TOOL_BAR)))
			XtSetArg(args[nargs], XmNnavigationType, XmNONE), nargs++;
		XtSetArg(args[nargs], XmNscrollHorizontal, FALSE); nargs++;
		XtSetArg(args[nargs], XmNscrollVertical, FALSE); nargs++;
		RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateScrolledText, ccode, TRUE, TRUE);
		// RegisterObject(xmTextWidgetClass,
		//		ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE);
		if (screenID)
		{
			if (ccode != S_CHANGED)
			{
				XtAddCallback(screenID, XmNmodifyVerifyCallback,
					StringCallback, (XtPointer)this);
			}
			// Store colors so we can invert.
			XtVaGetValues(screenID, XmNforeground, &colorForeground,
								XmNbackground, &colorBackground, ZIL_NULLP(void));
		}

		// Set value after callback added so it can translate.
		DataSet(text, maxLength);

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

	case S_VERIFY_STATUS:
		if (_hotKeyPressed == no)
			_hotKeyPressed = yes;
		else if (_hotKeyPressed == maybe)
		{
			Widget w = (Widget)event.rawCode;
			if (w)
			{
				XmTextPosition pos = XmTextGetInsertionPosition(w);
				woStatus |= WOS_INTERNAL_ACTION;
				XmTextReplace(w, MaxValue(0, pos-1) , pos, "");
				woStatus &= ~WOS_INTERNAL_ACTION;
			}
			_hotKeyPressed = no;
		}
		break;

	case L_END_MARK:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (Inherited(ID_LIST_ITEM))
			UserFunction(event, L_SELECT);
		break;

	case L_SELECT:
		DataGet();
		ccode = UI_WINDOW_OBJECT::Event(UI_EVENT(L_SELECT));
		if (parent && parent->Inherited(ID_LIST))
		{
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
			{
				if (FlagSet(woStatus, WOS_SELECTED))
					XtVaSetValues(screenID, XmNforeground, colorBackground,
											XmNbackground, colorForeground,
											ZIL_NULLP(void));
				else
					XtVaSetValues(screenID, XmNforeground, colorForeground,
											XmNbackground, colorBackground,
											ZIL_NULLP(void));
			}
		}
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

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_STRING::OSDataGet(void)
{
#	if (ZIL_MOTIF > 1001)
	if (screenID && FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE))
		;
	else
#	endif
	if (screenID && !parent->Inherited(ID_VT_LIST) &&
		!FlagSet(stFlags, STF_PASSWORD))
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

void UIW_STRING::OSDataSet(void)
{
#if (ZIL_MOTIF > 1001)
	if (screenID && FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE))
	{
		XmString labelText = CreateMotifString(text);
		nargs = 0;
		XtSetArg(args[nargs], XmNlabelString, labelText); nargs++;
		XtSetValues(screenID, args, nargs);
		if (labelText)
			XmStringFree(labelText);
	}
	else
#endif
	if (screenID)
	{
		if (!text)
			text = _blankString;
		ZIL_ICHAR *tText = text;
		if (FlagSet(stFlags, STF_PASSWORD))
			tText = strdup(text);

		// Use XtSetValues because XmTextSetString doesn't flush.
		int currentMax;
		ZIL_ICHAR *value;
#if defined(ZIL_UNICODE)
		XtVaGetValues(screenID, XmNmaxLength, &currentMax,
			XmNvalueWcs, &value, ZIL_NULLP(void));
#else
		XtVaGetValues(screenID, XmNmaxLength, &currentMax,
			XmNvalue, &value, ZIL_NULLP(void));
#endif
		nargs = 0;
		if (maxLength != -1 && maxLength != currentMax)
			XtSetArg(args[nargs], XmNmaxLength, maxLength), nargs++;
#if defined(ZIL_UNICODE)
		char *mb = MapText(tText);
		wchar_t *wText = new wchar_t[strlen(tText) + 1];
		::mbstowcs(wText, mb, strlen(tText) + 1);
		XtSetArg(args[nargs], XmNvalueWcs, wText); nargs++;
		delete mb;
#else
		if (strcmp(tText, value))
			XtSetArg(args[nargs], XmNvalue, tText), nargs++;
#endif
		XtSetValues(screenID, args, nargs);
		if (tText != text)
			delete (tText);
#if defined(ZIL_UNICODE)
		delete [] wText;
#endif
	}
}

void UIW_STRING::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_STRING && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
