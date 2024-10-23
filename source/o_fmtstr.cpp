//	Zinc Interface Library - O_FMTSTR.CPP
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


#define INCL_WINENTRYFIELDS
#define USE_RAW_KEYS
#include "ui_win.hpp"

// ----- UIW_FORMATTED_STRING -----------------------------------------------

EVENT_TYPE UIW_FORMATTED_STRING::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = S_UNKNOWN;
	int processed = FALSE;

	if (event.type == E_OS2)
	{
		ULONG msg = event.message.msg;
		processed = TRUE;

		// Switch on the windows message.
		switch (msg)
		{
		case WM_CHAR:
			if (!FlagSet(LOWORD(event.message.mp1), KC_KEYUP) && IsPrint(event.key.value))
			{
				Reposition(L_RIGHT);
				int pos = LOWORD(WinSendMsg(screenID, EM_QUERYSEL, 0, 0));
				ZIL_ICHAR key = (ZIL_ICHAR)event.key.value;
				if (LegalChar(key, pos))
				{
					WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(pos, pos+1), 0);
#if defined (ZIL_UNICODE)
					USHORT os2Key = (UCHAR)MapChar(key)[0];
//					?????
#elif defined(ZIL_ISO8859_1)
					USHORT os2Key = (UCHAR)MapChar(key)[0];
#else
					USHORT os2Key = (UCHAR)key;
#endif
					ccode = UIW_STRING::Event(UI_EVENT(E_OS2, screenID,	WM_CHAR,
						MPFROM2SHORT(KC_CHAR, 1), MPFROMSHORT(os2Key)));
					Reposition(L_RIGHT);
				}
			}
			else
				processed = FALSE;
			break;

		case WM_BUTTON1UP:
			{
			ccode = UIW_STRING::Event(event);
			int beginMark, endMark;
			MRESULT mark = WinSendMsg(screenID, EM_QUERYSEL, 0, 0);
			beginMark = LOWORD(mark);
			endMark = HIWORD(mark);
			if (beginMark == endMark)
				Reposition(L_RIGHT);
			}
			break;

		default:
			ccode = UIW_STRING::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Get current caret position.
		int pos, beginMark, endMark;
		if (screenID)
		{
			MRESULT mark = WinSendMsg(screenID, EM_QUERYSEL, 0, 0);
			beginMark = LOWORD(mark);
			endMark = HIWORD(mark);
			pos = beginMark == endMark ? beginMark : -1;
		}
		else
			pos = -1;

		// Switch on the event type.
		ccode = LogicalEvent(event);

		switch (ccode)
		{
		case S_CURRENT:
			ccode = UIW_STRING::Event(event);
			if (!FlagSet(woFlags, WOF_AUTO_CLEAR))
			{
				WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(0, 0), 0);
				Reposition(ccode, 1);
			}
			break;

		case S_CREATE:
			Export(text, TRUE);
			ccode = UIW_STRING::Event(event);
			break;

		case L_RIGHT:
		case L_LEFT:
			{
			int pos = LOWORD(WinSendMsg(screenID, EM_QUERYSEL, 0, 0));
			WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(pos, pos), 0);
			Reposition(ccode, 1);
			}
			break;

		case L_BOL:
			ccode = UIW_STRING::Event(event);
			Reposition(L_RIGHT);
			break;

		case L_BACKSPACE:
			{
			DataGet();
			int newPos = Reposition(L_LEFT, 1);
			if (newPos == pos)
				break;

			Event(UI_EVENT(L_DELETE));
			}
			break;

		case L_DELETE:
			{
			DataGet();

			if (pos == -1)
			{
				// Block delete.
				for (int newPos = pos = beginMark; newPos < endMark; newPos++)
					text[newPos] = deleteText[newPos];
				DataSet(ZIL_NULLP(ZIL_ICHAR));
			}
			else
			{
				// Find pos of next char to delete.
				int newPos = pos;
				while (deleteText[newPos] && text[newPos] == deleteText[newPos])
					newPos++;
				if (!deleteText[newPos])
					break;

				// Move to correct position and insert deleteText.
				Position(L_RIGHT, newPos);
				WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(newPos, newPos + 1), 0);
#if defined (ZIL_UNICODE)
					USHORT os2Key = (UCHAR)MapChar(deleteText[newPos])[0];
//					?????
#elif defined(ZIL_ISO8859_1)
					USHORT os2Key = (UCHAR)MapChar(deleteText[newPos])[0];
#else
					USHORT os2Key = (UCHAR)deleteText[newPos];
#endif
				UIW_STRING::Event(UI_EVENT(E_OS2, screenID,	WM_CHAR,
					MPFROM2SHORT(KC_CHAR, 1), MPFROMSHORT(os2Key)));
			}
			Position(L_RIGHT, pos);
			}
			break;

		case L_DELETE_EOL:
			if (pos == -1)
				Event(UI_EVENT(L_DELETE));
			{
				for (int newPos = pos; deleteText[newPos]; newPos++)
					text[newPos] = deleteText[newPos];
				DataSet(ZIL_NULLP(ZIL_ICHAR));
				Position(L_RIGHT, pos);
			}
			break;

		case S_NON_CURRENT:
			DataGet();
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		default:
			ccode = UIW_STRING::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

int UIW_FORMATTED_STRING::Position(EVENT_TYPE ccode, int position)
{
	WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(position, position), 0);
	return (Reposition(ccode));
}

int UIW_FORMATTED_STRING::Reposition(EVENT_TYPE ccode, int distance)
{
	int pos, beginMark, endMark;
	MRESULT mark = WinSendMsg(screenID, EM_QUERYSEL, 0, 0);
	beginMark = LOWORD(mark);
	endMark = HIWORD(mark);
	pos = beginMark;
	if (beginMark != endMark)
		WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(pos, pos), 0);

	int newPos = pos + (ccode == L_LEFT ? -distance : distance);
	while (newPos >= 0 && editMask[newPos] == 'L')
		newPos += ccode == L_LEFT ? -1 : 1;
	if (newPos < 0)
		newPos = Reposition(L_RIGHT);
	else if (newPos != pos)
		WinSendMsg(screenID, EM_SETSEL, MPFROM2SHORT(newPos, newPos), 0);

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
