//	Zinc Interface Library - W_FMTSTR.CPP
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

// ----- UIW_FORMATTED_STRING -----------------------------------------------

EVENT_TYPE UIW_FORMATTED_STRING::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = S_UNKNOWN;
	int processed = FALSE;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;
		WPARAM wParam = event.message.wParam;
		processed = TRUE;


		// Switch on the windows message.
		switch (message)
		{
		case WM_CHAR:
			{
			if (wParam == TAB || wParam == ENTER)
			{
				processed = FALSE;
				break;
			}

			// Get current caret position.
			int pos, beginMark, endMark;
#if defined(ZIL_WINNT)
			if (!FlagSet(GetVersion(), 0x80000000)) 		// Win32s Bug
				SendMessage(screenID, EM_GETSEL, (WPARAM)&beginMark, (LPARAM)&endMark);
			else
#endif
			{
				LRESULT block = SendMessage(screenID, EM_GETSEL, 0, 0);
				beginMark = LOWORD(block);
				endMark = HIWORD(block);
			}
			if (beginMark != endMark)
			{
				Event(UI_EVENT(L_DELETE));
				pos = Reposition(L_RIGHT);
			}
			else
				pos = beginMark;


#if defined(ZIL_UNICODE) 
			// Convert pos from mbs pos to unicode pos.
			int length = (int)SendMessage(screenID, WM_GETTEXTLENGTH, 0, 0);
			char *tmpBuff = new char[length + 1];
			SendMessage(screenID, WM_GETTEXT, length+1, (LONG)tmpBuff);
			tmpBuff[pos] = 0;
			ZIL_ICHAR *tmpText = new ZIL_ICHAR[length + 1];
			UnMapText(tmpBuff, tmpText);
			pos = strlen(tmpText);
			delete tmpBuff;
			delete tmpText;

			UI_EVENT tEvent = event;
			char tmp[2] = { 0, 0 };
			tmp[0] = wParam;
			int mbParts = mblen(tmp);
			char *mbChar = new char[mbParts + 1];\
			mbChar[0] = tmp[0];
			int i;
			for (i = 1; i < mbParts; i++)
			{
 				GetMessage(&tEvent.message, screenID, WM_CHAR, WM_CHAR);
				mbChar[i] = tEvent.message.wParam;
			}
			mbChar[i] = 0;
			ZIL_ICHAR textValue = UnMapChar(mbChar);
			if (LegalChar(textValue, pos))
			{
				DataGet();
				text[pos++] = textValue;
				while (editMask[pos] == 'L')
					pos++;
				UIW_STRING::DataSet(text);
			}
			delete mbChar;
			Position(L_RIGHT, pos);
#else
			ZIL_ICHAR textValue = wParam;
			if (LegalChar(textValue, pos))
			{
				// Don't just do a delete because shift key may be down.
				UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
					WM_KEYDOWN, GRAY_RIGHT_ARROW, 1));
				UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
					WM_CHAR, BACKSPACE, 1));

				ccode = UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
					WM_CHAR, (WPARAM)textValue, event.message.lParam));
				Reposition(L_RIGHT);
			}
#endif
			}
			break;

		case WM_LBUTTONUP:
			{
			ccode = UIW_STRING::Event(event);

			// Get current caret position.
			int beginMark, endMark;
#if defined(ZIL_WINNT)
			if (!FlagSet(GetVersion(), 0x80000000)) 		// Win32s Bug
				SendMessage(screenID, EM_GETSEL, (WPARAM)&beginMark, (LPARAM)&endMark);
			else
#endif
			{
				LRESULT block = SendMessage(screenID, EM_GETSEL, 0, 0);
				beginMark = LOWORD(block);
				endMark = HIWORD(block);
			}
			if (beginMark == endMark)
				Reposition(L_RIGHT);
			}
			break;

		// Ambiguous cases.
		case WM_KEYDOWN:
			processed = FALSE;
			break;

		case WM_KEYUP:
			UIW_STRING::Event(event);
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
#if defined(ZIL_WINNT)
			if (!FlagSet(GetVersion(), 0x80000000)) 		// Win32s Bug
				SendMessage(screenID, EM_GETSEL, (WPARAM)&beginMark, (LPARAM)&endMark);
			else
#endif
			{
				LRESULT block = SendMessage(screenID, EM_GETSEL, 0, 0);
				beginMark = LOWORD(block);
				endMark = HIWORD(block);
			}
			pos = beginMark == endMark ? beginMark : -1;
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
			// Convert pos from mbs pos to unicode pos.
			int length = (int)SendMessage(screenID, WM_GETTEXTLENGTH, 0, 0);
			char *tmpBuff = new char[length + 1];
			SendMessage(screenID, WM_GETTEXT, length+1, (LONG)tmpBuff);
			tmpBuff[pos] = 0;
			ZIL_ICHAR *tmpText = new ZIL_ICHAR[length + 1];
			UnMapText(tmpBuff, tmpText);
			pos = strlen(tmpText);
			delete tmpBuff;
			delete tmpText;
#endif
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
				Position(L_RIGHT, 0);
			break;

		case S_CREATE:
			Export(text, TRUE);
			if (FlagSet(woStatus, WOS_UNANSWERED))
				strcpy(text, deleteText);
			ccode = UIW_STRING::Event(event);
			break;

		case L_RIGHT:
		case L_LEFT:
			DataGet();
			Reposition(ccode, 1);
			break;

		case L_BOL:
			DataGet();
			Position(L_RIGHT, 0);
			break;

		case L_PASTE:
			{
			DataGet();
#if defined(ZIL_UNICODE) && defined(ZIL_WINNT)
			if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
#else
			if (!IsClipboardFormatAvailable(CF_TEXT))
#endif
				break;

			OpenClipboard(screenID);
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
			HANDLE hText = GetClipboardData(CF_TEXT);
			if (!hText)
				break;
			char *lpText = (char *)GlobalLock(hText);
			ZIL_ICHAR *nText = new ZIL_ICHAR[::strlen(lpText) + 1];
			UnMapText(lpText, nText);
			GlobalUnlock(hText);
			CloseClipboard();
			ZIL_ICHAR *nPos = nText;
#else
#	if defined(ZIL_UNICODE)
			// Implies defined(ZIL_WINNT)
			HANDLE hText = GetClipboardData(CF_UNICODETEXT);
#	else
			HANDLE hText = GetClipboardData(CF_TEXT);
#	endif
			if (!hText)
			{
				CloseClipboard();	
				break;
			}
			ZIL_ICHAR *lpText = (ZIL_ICHAR *)GlobalLock(hText);
			ZIL_ICHAR *nText = new ZIL_ICHAR[strlen(lpText) + 1];
			ZIL_ICHAR *nPos = nText;
			while ((*nPos++ = *lpText++) != 0)
				;
			GlobalUnlock(hText);
			CloseClipboard();
#endif
			// Got the text, now do the paste.
			if (pos == -1)
			{
				// Delete anything blocked
				Event(UI_EVENT(L_DELETE));
				pos = Reposition(L_RIGHT);
			}

			for(nPos = nText; *nPos; nPos++)
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
			delete nText;
			DataSet(ZIL_NULLP(ZIL_ICHAR));
			Position(L_RIGHT, pos);
			}
			break;

		case L_CUT:
			if (!screenID)
				break;
			else if (pos == -1)
			{
				// Get clip text.
				DataGet();
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
				HANDLE hText = GlobalAlloc(GHND, endMark - beginMark + 1);

				char *lpText = (char *)GlobalLock(hText);
				char *nText = new char[endMark - beginMark + 1];
				MapText(text, nText);
				for(pos = beginMark; pos < endMark; pos++)
					*lpText++ = nText[pos];
				*lpText = 0;
				GlobalUnlock(hText);
				Event(UI_EVENT(L_DELETE));

				// Send text to clipboard.
				OpenClipboard(screenID);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hText);
				CloseClipboard();
				delete nText;
#else
				HANDLE hText = GlobalAlloc(GHND,
					(endMark - beginMark + 1) * sizeof(ZIL_ICHAR));
				ZIL_ICHAR *lpText = (ZIL_ICHAR *)GlobalLock(hText);
				for(pos = beginMark; pos < endMark; pos++)
					*lpText++ = text[pos];
				*lpText = 0;
				GlobalUnlock(hText);
				Event(UI_EVENT(L_DELETE));

				// Send text to clipboard.
				OpenClipboard(screenID);
				EmptyClipboard();
#	if defined(ZIL_UNICODE)
				SetClipboardData(CF_UNICODETEXT, hText);
#	else
				SetClipboardData(CF_TEXT, hText);
#	endif
				CloseClipboard();
#endif
				break;
			}
			// Else fall through to L_BACKSPACE.

		case L_BACKSPACE:
			{
			if (pos != -1)
			{
				DataGet();
				int newPos = Reposition(L_LEFT, 1);
				if (newPos == pos)
					break;
			}

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
				UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
					WM_KEYDOWN, GRAY_RIGHT_ARROW, 1));
				UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
					WM_CHAR, BACKSPACE, 1));
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
				char *tmp = MapChar(deleteText[newPos]);
				for (int i = 0; tmp[i]; i++)
					UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
						WM_CHAR, tmp[i], 1));
#else
				UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
					WM_CHAR, deleteText[newPos], 1));
#endif
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
#if defined(ZIL_WINNT)
	SendMessage(screenID, EM_SETSEL, position, position);
#else
#	if defined(ZIL_UNICODE)
	// Convert from unicode position to mbs position.
	ZIL_ICHAR *tmpText = strdup(text);
	if (position < strlen(text))
		tmpText[position] = 0;
	char *tmpBuff = new char[(position + 1) * 2];
	MapText(tmpText, tmpBuff);
	position = ::strlen(tmpBuff);
	delete tmpBuff;
	delete tmpText;
#	endif

	LPARAM lParam = MAKELONG(position, position);
	SendMessage(screenID, EM_SETSEL, 0, lParam);
#endif
	
	return (Reposition(ccode));
}

int UIW_FORMATTED_STRING::Reposition(EVENT_TYPE ccode, int distance)
{
	int pos, beginMark, endMark;
#if defined(ZIL_WINNT)
			if (!FlagSet(GetVersion(), 0x80000000)) 		// Win32s Bug
				SendMessage(screenID, EM_GETSEL, (WPARAM)&beginMark, (LPARAM)&endMark);
			else
#endif
			{
				LRESULT block = SendMessage(screenID, EM_GETSEL, 0, 0);
				beginMark = LOWORD(block);
				endMark = HIWORD(block);
			}
	pos = beginMark == endMark ? beginMark : -1;

	int newPos;
	if (pos == -1)
	{
		UIW_STRING::Event(UI_EVENT(E_MSWINDOWS, screenID,
			WM_KEYDOWN, ccode == L_LEFT ? GRAY_LEFT_ARROW : GRAY_RIGHT_ARROW, 1));
#if defined(ZIL_WINNT)
			if (!FlagSet(GetVersion(), 0x80000000)) 		// Win32s Bug
				SendMessage(screenID, EM_GETSEL, (WPARAM)&beginMark, (LPARAM)&endMark);
			else
#endif
			{
				LRESULT block = SendMessage(screenID, EM_GETSEL, 0, 0);
				beginMark = LOWORD(block);
				endMark = HIWORD(block);
			}
		newPos = pos = beginMark;
	}
	else
	{
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
		// Convert pos from mbs pos to unicode pos.
		int length = (int)SendMessage(screenID, WM_GETTEXTLENGTH, 0, 0);
		char *tmpBuff = new char[length + 1];
		SendMessage(screenID, WM_GETTEXT, length+1, (LONG)tmpBuff);
		tmpBuff[pos] = 0;
		ZIL_ICHAR *tmpText = new ZIL_ICHAR[length + 1];
		UnMapText(tmpBuff, tmpText);
		pos = strlen(tmpText);
		delete tmpBuff;
		delete tmpText;
#endif
		newPos = pos + (ccode == L_LEFT ? -distance : distance);
	}

	while (newPos >= 0 && editMask[newPos] == 'L')
		newPos += ccode == L_LEFT ? -1 : 1;

	if (pos != newPos && newPos >= 0)
#if defined(ZIL_WINNT)
		SendMessage(screenID, EM_SETSEL, newPos, newPos);
#else
	{
#	if defined(ZIL_UNICODE)
		// Convert from unicode position to mbs position.
		ZIL_ICHAR *tmpText = strdup(text);
		if (newPos < strlen(text))
			tmpText[newPos] = 0;
		char *tmpBuff = new char[(newPos + 1) * 2];
		MapText(tmpText, tmpBuff);
		newPos = ::strlen(tmpBuff);
		delete tmpBuff;
		delete tmpText;
#	endif
		LPARAM lParam = MAKELONG(newPos, newPos);
		SendMessage(screenID, EM_SETSEL, 0, lParam);
	}
#endif
	else
		newPos = pos;

	return (newPos);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_FORMATTED_STRING::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_FORMATTED_STRING && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

