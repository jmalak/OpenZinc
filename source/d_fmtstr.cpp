//	Zinc Interface Library - D_FMTSTR.CPP
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


#include <stdlib.h>
#include "ui_win.hpp"

// ----- UIW_FORMATTED_STRING -----------------------------------------------

EVENT_TYPE UIW_FORMATTED_STRING::Event(const UI_EVENT &event)
{
	// Check for Zinc specific messages.
	EVENT_TYPE ccode = LogicalEvent(event, ID_FORMATTED_STRING);
	switch (ccode)
	{
	case S_CREATE:
		Export(text, TRUE);
		ccode = UIW_STRING::Event(event);
		break;

	case L_BACKSPACE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			Reposition(L_LEFT);
			text[cursor] = deleteText[cursor];
			Event(UI_EVENT(S_REDISPLAY));
		}
		break;

	case L_DELETE:
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;

		if (beginMark != endMark)
			Event(UI_EVENT(L_CUT));
		else
		{
		int length = strlen(editMask);
		if (cursor == length)
			break;
		if (editMask[cursor] == 'L')
			Reposition(L_RIGHT);
		for (int i = cursor; i < length; i++)
			if (text[i] && text[i] != deleteText[i])
			{
				text[i] = deleteText[i];
				UIW_STRING::Event(UI_EVENT(S_REDISPLAY));
				break;
			}
		}
		break;

	case L_CUT:
		if (beginMark != -1 && !FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			int length = endMark - beginMark;

			if (pasteBuffer)
				delete pasteBuffer;
			pasteLength = length;
			if (pasteLength)
			{
				pasteBuffer = new ZIL_ICHAR[pasteLength];
				memcpy(pasteBuffer, &text[beginMark], pasteLength * sizeof(ZIL_ICHAR));
			}
			else
				pasteBuffer = ZIL_NULLP(ZIL_ICHAR);

			for (int begin = beginMark; begin < endMark; begin++)
				text[begin] = deleteText[begin];

			cursor = beginMark;
			beginMark = endMark = -1;
			UIW_STRING::Event(UI_EVENT(S_REDISPLAY));
		}
		break;

	case L_PASTE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
		int pos = cursor;
		for(ZIL_ICHAR *nPos = pasteBuffer; *nPos; nPos++)
		{
			if (*nPos == deleteText[pos])
				pos++;
			else
			{
				while (editMask[pos] == 'L')
					pos++;
				if (LegalChar(*nPos, pos))
					text[pos++] = *nPos;
			}
		}
		UIW_STRING::Event(UI_EVENT(S_REDISPLAY));
		}
		break;

	case L_BOL:
	case L_WORD_LEFT:
		UIW_STRING::Event(event);
		if (editMask[cursor] == 'L')
			Reposition(L_RIGHT);
		break;

	case L_EOL:
	case L_WORD_RIGHT:
		UIW_STRING::Event(event);
		if (cursor < strlen(editMask) && editMask[cursor] == 'L')
			Reposition(L_RIGHT);
		break;

	case L_INSERT_TOGGLE:
		break;

	case L_LEFT:
	case L_RIGHT:
		Reposition(ccode);
		break;

	case L_END_MARK:
		ccode = UIW_STRING::Event(event);
		if (editMask[cursor] == 'L' && beginMark == endMark)
			Reposition(L_RIGHT);
		break;

	case E_KEY:
		{
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;

		// Check for marked text.
		if (beginMark != -1)
		{
			for (int begin = beginMark; begin <= endMark; begin++)
				text[begin] = deleteText[begin];
			cursor = beginMark;
			beginMark = endMark = -1;
			UIW_STRING::Event(UI_EVENT(S_REDISPLAY));
		}

		// Reposition cursor if on literal char.
  		if (editMask[cursor] == 'L')
			cursor = Reposition(L_RIGHT);
		if (cursor == strlen(editMask))
			break;
		
		// See if char is legal.
	   ZIL_ICHAR tChar = event.key.value;
		if (LegalChar(tChar, cursor))
		{
			UI_EVENT tEvent = event;
			tEvent.key.value = tChar;
			UIW_STRING::Event(tEvent);
			if (editMask[cursor] == 'L')
				Reposition(L_RIGHT);
		}
		}
		break;

	case S_CURRENT:
		while (editMask[cursor] == 'L')
			cursor++;
		insertMode = 0;
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


int UIW_FORMATTED_STRING::Reposition(EVENT_TYPE ccode, int)
{
	int pos = cursor;

	if (ccode == L_LEFT)
		while (pos && editMask[pos-1] == 'L')
			pos--;
	else if (ccode == L_RIGHT)
		while (editMask[pos] && editMask[pos+1] == 'L')
			pos++;

	if (beginMark != -1)
	{
		beginMark = endMark = -1;
		UIW_STRING::Event(UI_EVENT(S_REDISPLAY));
	}
	if ((ccode == L_LEFT && pos && editMask[pos-1] != 'L') ||
		(ccode == L_RIGHT && editMask[pos] && editMask[pos+1] != 'L'))
	{
		cursor = pos;
		UIW_STRING::Event(UI_EVENT(ccode));
	}
	return cursor;
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
		event.type = S_REDISPLAY;
		Event(event);
	}
}
