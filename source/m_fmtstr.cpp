//	Zinc Interface Library - M_FMTSTR.CPP
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
#include <X11/keysym.h>
#include <Xm/Text.h>

// ----- UIW_FORMATTED_STRING -----------------------------------------------

static void FormatStringCallback(Widget, XtPointer client, XtPointer call_data)
{
	// Do all processing in Event because we can't access private members.
	UIW_STRING *object = (UIW_STRING *)client;
	UI_EVENT event(S_VERIFY_STATUS);
	event.data = call_data;
	object->Event(event);
}

EVENT_TYPE UIW_FORMATTED_STRING::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_FORMATTED_STRING);
	switch (ccode)
	{
	case S_CREATE:
		Export(text, TRUE);
		ccode = UIW_STRING::Event(event);
		if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE))
		{
			XtAddCallback(screenID, XmNmodifyVerifyCallback,
				FormatStringCallback, (XtPointer)this);
			XtAddCallback(screenID, XmNmotionVerifyCallback,
				FormatStringCallback, (XtPointer)this);

			int newPosition = Reposition(L_RIGHT, 0);
			if(newPosition != 0)
			{
				// Send an event thru the queue to move the cursor
				// after the callback has returned.
				XmTextSetCursorPosition(screenID, (XmTextPosition)newPosition);
			}
		}
		break;

	case S_VERIFY_STATUS:
		if (event.data)
		{
			UI_EVENT tEvent(S_VERIFY_STATUS);
			// event.data is the call_data from FormatStringCallback.
			XmTextVerifyPtr call = (XmTextVerifyPtr)event.data;
			if (call->reason == XmCR_MODIFYING_TEXT_VALUE)
			{
				XmTextBlock block = call->text;
				if (call->text->length == 0)
				{
					// User is deleting 1 or more chars.
//					call->doit = FALSE;	// Causes core dump on Sun.
					DataGet();

					int start = Reposition(L_LEFT, 0, call->startPos);
					int end = Reposition(L_RIGHT, 0, call->endPos);
					for (int newPos = start; newPos < end; newPos++)
						text[newPos] = deleteText[newPos];
						
					// Force a DataSet().
					tEvent.rawCode = 0xffff;
					eventManager->Put(tEvent);
				}
				else if (call->text->length > 1)
				{
					// Put cursor at beginning of block.
					XtRemoveCallback(screenID, XmNmodifyVerifyCallback,
						FormatStringCallback, (XtPointer)this);
					XtRemoveCallback(screenID, XmNmotionVerifyCallback,
						FormatStringCallback, (XtPointer)this);
					XmTextSetCursorPosition(screenID, 1);
					XtAddCallback(screenID, XmNmodifyVerifyCallback,
						FormatStringCallback, (XtPointer)this);
					XtAddCallback(screenID, XmNmotionVerifyCallback,
						FormatStringCallback, (XtPointer)this);
				}
				else
				{
					// User entered some text.
					XmTextPosition left, right;
					XmTextGetSelectionPosition(screenID, &left, &right);
					if (left != right && call->newInsert != left)
					{
						// There is blocked text; set it to deleteText.
						left = Reposition(L_LEFT, 0, left);
						right = Reposition(L_RIGHT, 0, right);
						for (int newPos = left; newPos < right; newPos++)
							text[newPos] = deleteText[newPos];
						call->currInsert = left;
						call->newInsert = left + call->text->length;
					}
					XmTextClearSelection(screenID, CurrentTime);
					ZIL_ICHAR textValue = *call->text->ptr;
					if (LegalChar(textValue, (int)call->currInsert))
					{
						// Force DataSet().
						text[call->currInsert] = textValue;
						tEvent.rawCode = 0xffff;
						eventManager->Put(tEvent);

						// Move cursor.
						tEvent.rawCode = Reposition(L_RIGHT, call->currInsert,
							call->text->length);
						eventManager->Put(tEvent);
					}
					else
					{
						// Force DataSet() in case something was blocked.
						tEvent.rawCode = 0xffff;
						eventManager->Put(tEvent);

						// Put cursor at beginning of block.
						tEvent.rawCode = Reposition(L_RIGHT,
							call->currInsert, 0);
						eventManager->Put(tEvent);
					}
					call->doit = FALSE;
				}
			}
			else if (call->reason == XmCR_MOVING_INSERT_CURSOR)
			{
				int newPosition = Reposition(call->currInsert >
					call->newInsert ? L_LEFT : L_RIGHT,
					(int)AbsValue(call->currInsert - call->newInsert));

				if(newPosition != call->newInsert)
				{
					// Send an event thru the queue to move the cursor
					// after the callback has returned.
					UI_EVENT tEvent(S_VERIFY_STATUS);
					tEvent.rawCode = newPosition;
					eventManager->Put(tEvent);
					call->doit = FALSE;
				}
			}
		}
		else
		{
			// Note that if event.data is NULL, S_VERIFY_STATUS was sent
			// as the second part of a two-step cursor relocation or DataSet().
			// Temporarily remove the callback to prevent recursion.
			XtRemoveCallback(screenID, XmNmodifyVerifyCallback,
				FormatStringCallback, (XtPointer)this);
			XtRemoveCallback(screenID, XmNmotionVerifyCallback,
				FormatStringCallback, (XtPointer)this);
			if (event.rawCode != 0xffff)
				XmTextSetInsertionPosition(screenID,
					(XmTextPosition)event.rawCode);
			else
			{
				XmTextPosition pos = XmTextGetCursorPosition(screenID);
				DataSet(ZIL_NULLP(ZIL_ICHAR));
				XmTextSetCursorPosition(screenID, pos);
			}
			XtAddCallback(screenID, XmNmodifyVerifyCallback,
				FormatStringCallback, (XtPointer)this);
			XtAddCallback(screenID, XmNmotionVerifyCallback,
				FormatStringCallback, (XtPointer)this);
		}
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

int UIW_FORMATTED_STRING::Position(EVENT_TYPE ccode, int)
{
	return (Reposition(ccode));
}


int UIW_FORMATTED_STRING::Reposition(EVENT_TYPE ccode, int distance,
	int position)
{
	XmTextPosition currInsert = position == -1 ?
		XmTextGetInsertionPosition(screenID) : position;
	int newPos = (int)currInsert + (ccode == L_LEFT ? -distance : distance);
	while (newPos >= 0 && editMask[newPos] == 'L')
		newPos += ccode == L_LEFT ? -1 : 1;
	if (newPos < 0)
		return (Reposition(L_RIGHT, 1, 0));
	return (newPos);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_FORMATTED_STRING::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_FORMATTED_STRING && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}

