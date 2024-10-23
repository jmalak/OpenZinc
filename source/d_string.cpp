//	Zinc Interface Library - D_STRING.CPP
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

const int STRING_OFFSET = 4;

// ----- UIW_STRING ---------------------------------------------------------

ZIL_ICHAR UIW_STRING::CharacterConvert(ZIL_ICHAR key)
{
	if (FlagSet(stFlags, STF_LOWER_CASE))
		key = ToLower(key);
	if (FlagSet(stFlags, STF_UPPER_CASE))
		key = ToUpper(key);
	if (FlagSet(stFlags, STF_VARIABLE_NAME) && key == ' ')
		key = '_';
	return (key);
}

int UIW_STRING::CursorUpdate(int newOffset, int scrollWidth, int showCursor)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
	UI_REGION active = this->active;
#endif
	int oldLeftClip = leftClip;
	// Check for valid region.
	if (active.right <= active.left)
	{
		leftClip = cursor = newOffset;
		return (scrollWidth);
	}

	ZIL_ICHAR *passWd = ZIL_NULLP(ZIL_ICHAR);
	if (FlagSet(stFlags, STF_PASSWORD))
	{
		passWd = new ZIL_ICHAR[strlen(text) + 1];
		strcpy(passWd, text);
		StringSet(passWd, '*');
	}

	// Check for cursor moving to the left of the field.
	while (leftClip > 0 && newOffset < leftClip)
		leftClip = (leftClip > scrollCount) ? leftClip - scrollCount : 0;

	ZIL_ICHAR *buffer = FlagSet(stFlags, STF_PASSWORD) ? passWd : text;
	ZIL_ICHAR saveChar = buffer[newOffset];
	buffer[newOffset] = '\0';
	int count = buffer[leftClip] ?
			display->TextWidth(&buffer[leftClip], screenID, font) : 0;

	// Check for cursor moving to the right of the field.
	while (active.left + count > active.right)
	{
		int width = newOffset - leftClip < 8 ? 1 : (newOffset - leftClip) / 4;
		leftClip = leftClip + width;
		count = display->TextWidth(&buffer[leftClip], screenID, font);
	}
	if (showCursor && RedisplayType() == S_CURRENT)
	{
		if (active.left + count >= clip.left && active.top >= clip.top &&
			active.left + count <= clip.right && active.top <= clip.bottom &&
			active.bottom <= clip.bottom)
		{
			eventManager->DeviceState(E_CURSOR, insertMode ? DC_INSERT : DC_OVERSTRIKE);
			eventManager->DevicePosition(E_CURSOR, active.left + count, active.top);
		}
		else
			eventManager->DeviceState(E_CURSOR, D_OFF);
	}

	buffer[newOffset] = saveChar;

	// Reset the string flag and cursor position.
	cursor = newOffset;

	// Determine and return the actual scroll offset.
	if (oldLeftClip < leftClip)
	{
		saveChar = buffer[leftClip];
		buffer[leftClip] = '\0';
		scrollWidth = -display->TextWidth(&buffer[oldLeftClip], screenID, font);
		buffer[leftClip] = saveChar;
	}
	else if (oldLeftClip > leftClip)
	{
		saveChar = buffer[oldLeftClip];
		buffer[oldLeftClip] = '\0';
		scrollWidth = display->TextWidth(&buffer[leftClip], screenID, font);
		buffer[oldLeftClip] = saveChar;
	}
	if (FlagSet(stFlags, STF_PASSWORD))
		delete passWd;

	// Return the scroll width.
	return (scrollWidth);
}

EVENT_TYPE UIW_STRING::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Draw the string item.
	UI_REGION region = trueRegion;
	UI_PALETTE *palette;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (Inherited(ID_STATUS_ITEM))
		palette = LogicalPalette(ccode, ID_STATUS_ITEM);
	else
		palette = lastPalette;
	display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);
	if (FlagSet(stFlags, STF_PASSWORD))	// Check for a password.
	{
		ZIL_ICHAR *passWd = new ZIL_ICHAR[strlen(text)+1];
		strcpy(passWd, text);
		StringSet(passWd, '*');
		DrawText(screenID, active, passWd, palette, FALSE, ccode);
		delete passWd;
	}
	else
		DrawText(screenID, active, text, palette, FALSE, ccode);

	// Draw Focus rectangle if current list item.
	if (ccode == S_CURRENT && !display->isText && Inherited(ID_LIST_ITEM) &&
		FlagSet(woFlags, WOF_VIEW_ONLY))
	{
		UI_REGION focus = trueRegion;
		if (parent->Inherited(ID_COMBO_BOX) &&
			!FlagSet(parent->woAdvancedFlags, WOAF_TEMPORARY))
			--focus;
		DrawFocus(screenID, focus, ccode);
	}
	woStatus &= ~WOS_REDISPLAY;
	return (ccode);
}

EVENT_TYPE UIW_STRING::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode;
	int insertCount = 0;
	int deleteCount = 0;
	int scrollWidth = 0;
	int tCursor = cursor;
	int tBeginMark = beginMark;
	int tEndMark = endMark;
	int noEditCursor = (Inherited(ID_LIST_ITEM) && FlagSet(woFlags, WOF_VIEW_ONLY)) ||
		FlagSet(((UIW_WINDOW *)parent)->wnFlags, WNF_BITMAP_CHILDREN) ||
		FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) ||
		FlagSet(woStatus, WOS_OWNERDRAW) ||
		FlagSet(woFlags, WOF_NON_SELECTABLE) ||
		FlagSet(parent->woFlags, WOF_NON_SELECTABLE);

	static ZIL_UTIME lastTime, currentTime;
	static UI_WINDOW_OBJECT *lastSelected;

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_STRING);
	if (ccode == L_CUT_PASTE)
		ccode = (beginMark != -1) ? L_CUT : L_PASTE;
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
#if !defined(ZIL_TEXT_ONLY)
		if (!display->isText)
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
#endif
		break;

	case S_CHANGED:
	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		active = trueRegion;
#if defined(ZIL_STANDARD_BORDER)
		if (display->isText && FlagSet(woFlags, WOF_BORDER))
			active.left++, active.right--;
#endif
		if (active.Width() < display->cellWidth * 8)
			scrollCount = 1;
		else
			scrollCount = active.Width() / display->cellWidth / 4;
		if (Inherited(ID_LIST_ITEM))
			tCursor = cursor = 0;
		if (ccode == S_CREATE && FlagSet(woStatus, WOS_UNANSWERED))
			strcpy(text, _blankString);
		break;

	case S_REDISPLAY:
		return (UI_WINDOW_OBJECT::Event(event));

	case S_CURRENT:
	case S_NON_CURRENT:
		if (ccode != S_CURRENT)
		{
			woStatus &= ~WOS_AUTO_MARK;
			beginMark = endMark = -1;
		}
		else if (FlagSet(woFlags, WOF_AUTO_CLEAR) &&
			!FlagSet(woFlags, WOF_NON_SELECTABLE) &&
			!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) &&
			(!FlagSet(woStatus, WOS_CURRENT) ||
			parent->Inherited(ID_COMBO_BOX) ||
			parent->Inherited(ID_SPIN_CONTROL)))
		{
			woStatus |= WOS_AUTO_MARK;
			tCursor = length;
			UI_REGION region = trueRegion;
#if defined(ZIL_STANDARD_BORDER)
		if (display->isText && FlagSet(woFlags, WOF_BORDER))
			region.left++, region.right--;
#endif
			if (!display->isText)
				region.left += STRING_OFFSET, region.right -= STRING_OFFSET;
			if (display->TextWidth(text, screenID, font) >= region.Width() &&
				!FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
				tCursor = 0;
			beginMark = 0, endMark = strlen(text);
		}
		// Continue to S_DISPLAY_ACTIVE.
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (!screenID || FlagSet(parent->woStatus, WOS_MINIMIZED))
			break;
		{
		EVENT_TYPE returnCode = UI_WINDOW_OBJECT::Event(event);
		if (parent->Inherited(ID_COMBO_BOX) && ccode == S_CURRENT)
			woStatus |= WOS_SELECTED;
		else if (parent->Inherited(ID_COMBO_BOX))
			woStatus &= ~WOS_SELECTED;
		if (parent->Inherited(ID_STATUS_ITEM) || parent->Inherited(ID_STATUS_BAR))
			lastPalette = LogicalPalette(ccode, ID_STATUS_ITEM);
		else if (Inherited(ID_LIST_ITEM) && FlagSet(woFlags, WOF_VIEW_ONLY))
				lastPalette = LogicalPalette(ccode, ID_LIST_ITEM);
#if defined (ZIL_OS2_STYLE)
		else if (!display->isText && FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			lastPalette = LogicalPalette(ccode, ID_WINDOW);
#endif
		else
			lastPalette = LogicalPalette(ccode, ID_STRING);

		active = trueRegion;
		if (!display->isText)	// Center the text vertically.
		{
#if !defined(ZIL_TEXT_ONLY)
			active.left += STRING_OFFSET;
			active.right -= STRING_OFFSET;
			int height = display->TextHeight(text, screenID, font);
			active.top += (active.Height() - height) / 2;
			active.bottom = active.top + height;
#endif
		}

#if defined(ZIL_STANDARD_BORDER)
		if (display->isText && FlagSet(woFlags, WOF_BORDER))
			active.left++, active.right--;
#endif

		if (FlagSet(woStatus, WOS_OWNERDRAW) ||
			FlagSet(woFlags, WOF_NON_SELECTABLE) ||
			(Inherited(ID_LIST_ITEM) && parent->SearchID() != ID_COMBO_BOX) ||
			(ccode != S_CURRENT && FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT)))
			DrawItem(event, ccode);
		else
		{
			// Compute the active region.
			UI_REGION region = trueRegion;
			length = strlen(text);
			display->VirtualGet(screenID, trueRegion);
			if (FlagSet(woFlags, WOF_BORDER))
				DrawBorder(screenID, region, FALSE, ccode);
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
			// Update the string information.
			if (ccode == S_CURRENT || FlagSet(woStatus, WOS_CURRENT))
				scrollWidth = CursorUpdate(tCursor, scrollWidth, !noEditCursor ? TRUE : FALSE);
			StringUpdate(insertCount, deleteCount, scrollWidth);
			display->VirtualPut(screenID);
		}
		return (returnCode);
		}

	case L_LEFT:
	case L_MARK_LEFT:
		if (ccode == L_UP || ccode == L_LEFT)
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		else if (beginMark == -1)
		{
			beginMark = endMark = tCursor;
			tBeginMark = tEndMark = -1;
		}
		if (tCursor > 0)
			tCursor--;
		else
			ccode = S_UNKNOWN;
		break;

	case L_RIGHT:
	case L_MARK_RIGHT:
		if (ccode == L_DOWN || ccode == L_RIGHT)
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		else if (beginMark == -1)
		{
			beginMark = endMark = tCursor;
			tBeginMark = tEndMark = -1;
		}
		if (tCursor < length)
			tCursor++;
		else
			ccode = S_UNKNOWN;
		break;

	case L_BOL:
	case L_MARK_BOL:
		if (ccode == L_MARK_BOL && beginMark == -1)
		{
			beginMark = endMark = tCursor;
			tBeginMark = tEndMark = -1;
		}
		tCursor = 0;
		break;

	case L_EOL:
	case L_MARK_EOL:
		if (ccode == L_MARK_EOL && beginMark == -1)
		{
			beginMark = endMark = tCursor;
			tBeginMark = tEndMark = -1;
		}
		tCursor = length;
		if (FlagSet(woStatus, WOS_AUTO_MARK))
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		break;

	case L_WORD_LEFT:
		if (tCursor > 0)
			tCursor--;
		else
			ccode = S_UNKNOWN;
		while (tCursor > 0 && text[tCursor] == ' ')
			tCursor--;
		while (tCursor > 0 && text[tCursor-1] != ' ')
			tCursor--;
		break;

	case L_WORD_RIGHT:
		while (tCursor < length && text[tCursor] != ' ')
			tCursor++;
		while (tCursor < length && text[tCursor] == ' ')
			tCursor++;
		if (tCursor == length)
			tCursor = cursor;
		if (tCursor == cursor)
			ccode = S_UNKNOWN;
		else if (FlagSet(woStatus, WOS_AUTO_MARK))
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		break;

	case L_INSERT_TOGGLE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			insertMode = !insertMode;
			eventManager->DeviceState(E_CURSOR, insertMode ? DC_INSERT : DC_OVERSTRIKE);
		}
		break;

	case L_MARK:
		if (noEditCursor)
			break;
		if (beginMark == -1)
			beginMark = endMark = cursor;
		else
			beginMark = endMark = -1;
		break;

	case S_HSCROLL:
		scrollWidth = event.scroll.delta;
		leftClip += scrollWidth;
		if (leftClip < 0)
			leftClip = 0;
		cursor = tCursor = leftClip;
		break;

	case L_VIEW:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if ((!noEditCursor && !FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			&& trueRegion.Overlap(event.position))
			eventManager->DeviceState(E_MOUSE, DM_EDIT);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		return (ccode);

	case L_BEGIN_MARK:
	case L_CONTINUE_MARK:
	case L_END_MARK:
	case L_CONTINUE_COPY_DRAG:
	case L_END_COPY_DRAG:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if (windowManager->dragObject)
		{
			UI_WINDOW_OBJECT::Event(event);
			break;
		}
		if (ccode == L_BEGIN_MARK &&
			FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
		   windowManager->dragObject = this;

		if (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) ||
			(ccode == L_CONTINUE_MARK && lastSelected != this))
			return ccode;

		if (!noEditCursor)
		{
			if (ccode == L_END_MARK)
			{
				lastSelected = ZIL_NULLP(UI_WINDOW_OBJECT);
				if (beginMark == endMark)
					beginMark = endMark = -1;
				woStatus |= WOS_AUTO_MARK;
				return ccode;
			}

			ZIL_ICHAR *passWd, *buffer;
			if (FlagSet(stFlags, STF_PASSWORD))
			{
				passWd = new ZIL_ICHAR[strlen(text)+1];
				strcpy(passWd, text);
				StringSet(passWd, '*');
				buffer = passWd;
			}
			else
				buffer = text;

			ZIL_ICHAR saveChar = buffer[tCursor];
			buffer[tCursor] = '\0';
			int currentLength = display->TextWidth(&buffer[leftClip], screenID, font);
 			buffer[tCursor] = saveChar;

			if (active.left + currentLength > event.position.column)
				while (tCursor >= 0)
				{
					saveChar = buffer[tCursor];
  					buffer[tCursor] = '\0';

					if (!buffer[leftClip] || !tCursor || active.left + display->TextWidth(&buffer[leftClip],
						screenID, font) < event.position.column)
					{
						buffer[tCursor] = saveChar;
						if (event.position.column > active.left)
							tCursor = MinValue(length, tCursor + 1);
						woStatus |= WOS_REDISPLAY;
						break;
					}
					buffer[tCursor] = saveChar;
					tCursor = MaxValue(0, tCursor-1);
				}
			else
			{
				while (tCursor < length)
				{
					buffer[tCursor] = '\0';
					if (display->TextWidth(&buffer[leftClip], screenID, font) + active.left + 1 > event.position.column)
						break;
					buffer[tCursor] = saveChar;
					tCursor++;
					saveChar = buffer[tCursor];
				}
				buffer[tCursor] = saveChar;
			}

			if (FlagSet(woStatus, WOS_AUTO_MARK))
			{
				woStatus &= ~WOS_AUTO_MARK;
				beginMark = endMark = -1;
			}

			if (FlagSet(stFlags, STF_PASSWORD))
				delete passWd;

			if (ccode == L_BEGIN_MARK && trueRegion.Overlap(event.position))
			{
				beginMark = endMark = tCursor;
				tBeginMark = tEndMark = -1;
				if (ccode == L_BEGIN_MARK)
				{
					currentTime.Import();
					if (currentTime.Difference(lastTime) < doubleClickRate)
					{
						woStatus |= WOS_AUTO_MARK;
						beginMark = 0, endMark = strlen(text);
						tCursor = cursor = endMark;
						Event(UI_EVENT(S_CURRENT, 0, trueRegion));
						return ccode;
					}
					else
					{
						lastTime.Import();
						lastSelected = this;
					}
				}
			}
		}
		else if (ccode == L_END_MARK && Inherited(ID_LIST_ITEM) &&
			!FlagSet(woFlags, WOF_NON_SELECTABLE) && trueRegion.Overlap(event.position))
		{
			UI_EVENT uEvent = event;
			currentTime.Import();
			if (currentTime.Difference(lastTime) < doubleClickRate &&
				this == lastSelected && userFunction)
			{
				uEvent.type = L_DOUBLE_CLICK;
				Event(uEvent);
			}
			else
			{
				WOS_STATUS oldStatus = woStatus;
				ccode = UserFunction(uEvent, L_SELECT);
				if (oldStatus != woStatus)
					Event(UI_EVENT(S_REDISPLAY));
				lastTime = currentTime;
				lastSelected = this;
			}
		}
		else
			ccode = S_UNKNOWN;
		break;

	case L_BACKSPACE:
		if (UIW_STRING::Event(L_LEFT) != S_UNKNOWN)
		{
			UIW_STRING::Event(L_DELETE);
			return ccode;
		}
		break;

	case L_CUT:
	case L_DELETE:
	case L_DELETE_EOL:
	case L_DELETE_WORD:
		{
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;
		else if (beginMark != -1)
		{
			deleteCount = endMark - beginMark;
			if (ccode == L_CUT)
			{
				if (pasteBuffer)
					delete pasteBuffer;
				pasteLength = deleteCount;
				if (pasteLength)
				{
					pasteBuffer = new ZIL_ICHAR[pasteLength];
					memcpy(pasteBuffer, &text[beginMark], pasteLength * sizeof(ZIL_ICHAR));
				}
				else
					pasteBuffer = ZIL_NULLP(ZIL_ICHAR);
			}
			tCursor = beginMark;
			beginMark = endMark = -1;
			length -= deleteCount;
			memmove(&text[tCursor], &text[tCursor+deleteCount], (length - tCursor + 1) * sizeof(ZIL_ICHAR));
			break;
		}
		int emptyQueue;
		UI_EVENT tEvent = event;
		do
		{
			if (ccode == L_DELETE)
			{
				if (tCursor < length)
					++deleteCount;
			}
			else if (ccode == L_DELETE_EOL)
				deleteCount = length - tCursor;
			else if (ccode == L_DELETE_WORD)
			{
				while (tCursor > 0 && text[tCursor-1] != ' ')
					tCursor--;
				deleteCount = tCursor;
				while (deleteCount < length && text[deleteCount] != ' ')
					deleteCount++;
				while (deleteCount < length && text[deleteCount] == ' ')
					deleteCount++;
				deleteCount -= tCursor;
			}
			emptyQueue = eventManager->Get(tEvent, Q_NO_BLOCK | Q_NO_DESTROY);
			if (!emptyQueue && tEvent.type == event.type && tEvent.rawCode == event.rawCode)
				eventManager->Get(tEvent);
			else
				emptyQueue = TRUE;
		} while (!emptyQueue);
		if (deleteCount > 0)
		{
			length -= deleteCount;
			memmove(&text[tCursor], &text[tCursor+deleteCount], (length - tCursor + 1) * sizeof(ZIL_ICHAR));
		}
		}
		break;

	case L_COPY_MARK:
		if (beginMark != -1)
		{
			pasteLength = endMark - beginMark;
			if (pasteBuffer)
				delete pasteBuffer;
			if (pasteLength)
				pasteBuffer = new ZIL_ICHAR[pasteLength];
			else
				pasteBuffer = ZIL_NULLP(ZIL_ICHAR);
			memcpy(pasteBuffer, &text[beginMark], pasteLength * sizeof(ZIL_ICHAR));
			beginMark = endMark = -1;
		}
		break;

	case L_PASTE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY) &&
		    pasteLength <= maxLength - length)
		{
			memmove(&text[tCursor+pasteLength], &text[tCursor], (length - tCursor) * sizeof(ZIL_ICHAR));
			length += pasteLength;
			text[length] = 0;
			for (int i = 0; i < pasteLength; i++)
			{
				ZIL_ICHAR key = CharacterConvert(pasteBuffer[i]);
				text[tCursor++] = key;
			}
			insertCount = pasteLength;
		}
		break;

	case L_SELECT:
		if (cursor > length)
			cursor = length;
		{
		WOS_STATUS oldStatus = woStatus;
		ccode = UserFunction(UI_EVENT(L_SELECT), L_SELECT);
		if (oldStatus != woStatus)
			Event(UI_EVENT(S_REDISPLAY));
		}
		break;

	case E_KEY:
		if (Inherited(ID_LIST_ITEM) && !parent->Inherited(ID_COMBO_BOX) &&
			event.key.value == ' ')
		{
			Event(UI_EVENT(L_SELECT));
			break;
		}
		if (FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE) ||
			FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) ||
			FlagSet(woStatus, WOS_OWNERDRAW) || !IsPrint(event.key.value))
			break;

		if (tCursor > length)
			tCursor = cursor = length;

		if (beginMark != -1)
		{
			UIW_STRING::Event(UI_EVENT(L_CUT));
			tCursor = cursor;
			tBeginMark = beginMark, tEndMark = endMark;
		}
		{
		insertCount = 0;
		ZIL_ICHAR key = CharacterConvert((ZIL_ICHAR)event.key.value);
		if (length < maxLength && (insertMode || tCursor == length ||
			text[tCursor] == '\r' || text[tCursor] == '\n'))
		{
			length++;
			memmove(&text[tCursor+1], &text[tCursor], (length - tCursor) * sizeof(ZIL_ICHAR));
			text[tCursor++] = key;
			insertCount++;
		}
		else if (!insertMode && tCursor < length)
		{
			text[tCursor++] = key;
			insertCount++;
		}
		}
		break;

	case S_DEINITIALIZE:
		if (FlagSet(woStatus, WOS_CURRENT))
		{
			woStatus &= ~WOS_CURRENT;
			eventManager->DeviceState(E_CURSOR, D_OFF);
		}
		return (UI_WINDOW_OBJECT::Event(event));

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	if (FlagSet(woFlags, WOF_NON_SELECTABLE) ||
		FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) || FlagSet(woStatus, WOS_OWNERDRAW))
		return ccode;

	// Check the mark information.
	if (beginMark != -1)
	{
		if (cursor == beginMark)
			beginMark = tCursor;
		else
			endMark = tCursor;
		if (beginMark > endMark)	// Make sure the mark order is correct.
		{
			int temp = beginMark;
			beginMark = endMark;
			endMark = temp;
		}
	}
	if (cursor != tCursor && beginMark != -1 && ccode != S_CURRENT)
	{
		// Reset the mark region if the mark status is set.
		if (FlagSet(woStatus, WOS_AUTO_MARK))
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		else
			tBeginMark = tEndMark = -1;
	}

	// Recompute the cursor position and redraw the string.
	if (!screenID)
	{
		if (insertCount || deleteCount)
			woStatus |= WOS_CHANGED;
		cursor = tCursor;
		return (ccode);
	}
	if (cursor != tCursor)
		scrollWidth = CursorUpdate(tCursor, scrollWidth, (!noEditCursor) ? TRUE : FALSE);
	if (insertCount || deleteCount || scrollWidth ||
		beginMark != tBeginMark || endMark != tEndMark)
		StringUpdate(insertCount, deleteCount, scrollWidth);

	// Return the control code.
	return (ccode);
}

void UIW_STRING::StringSet(ZIL_ICHAR *string, ZIL_ICHAR replaceChar)
{
	if (!string)
		return;

	for (int i = 0; i < strlen(string); i++)
		string[i] = replaceChar;
}

void UIW_STRING::StringUpdate(int insertCount, int deleteCount, int scrollWidth)
{
	// Check for a valid screen region and password.
	if (insertCount || deleteCount)
		woStatus |= WOS_CHANGED;

	if (active.right <= active.left)
	{
		woStatus &= ~WOS_REDISPLAY;
		return;
	}

	ZIL_ICHAR *passWd = ZIL_NULLP(ZIL_ICHAR);
	if (FlagSet(stFlags, STF_PASSWORD))
	{
		passWd = new ZIL_ICHAR[strlen(text) + 1];
		strcpy(passWd, text);
		StringSet(passWd, '*');
	}

	// Compute the actual clip region and save the display.
	UI_REGION clipRegion = clip;
	if (clipRegion.right > active.right)
		clipRegion.right = active.right;
	if (clipRegion.bottom > active.bottom)
		clipRegion.bottom = active.bottom;

	// Draw the string information.
	int left = active.left;
	int scrollLeft = leftClip;
	if (!scrollWidth && (insertCount || deleteCount))
	{
		scrollLeft = cursor - insertCount;
		ZIL_ICHAR *buffer = FlagSet(stFlags, STF_PASSWORD) ? passWd : text;
		ZIL_ICHAR saveChar = buffer[scrollLeft];
	 	buffer[scrollLeft] = '\0';
	 	left += display->TextWidth(&buffer[leftClip], screenID, font);
	 	buffer[scrollLeft] = saveChar;
	}
	int top = active.top;
	int beginOffset;
	int endOffset = 0;
	for (int i = 0; endOffset < length; i++)
	{
		beginOffset = (endOffset > scrollLeft) ? endOffset : scrollLeft;
		if (i == 0)			// Begin mark region.
			endOffset = (beginMark != -1) ? beginMark : length;
		else if (i == 1)	// End mark region.
			endOffset = endMark;
		else				// End of string.
			endOffset = length;
		if (beginOffset != endOffset && endOffset > scrollLeft && left < clipRegion.right)
		{
			ZIL_ICHAR *buffer = FlagSet(stFlags, STF_PASSWORD) ? passWd : text;
			ZIL_ICHAR saveChar = buffer[endOffset];
			buffer[endOffset] = '\0';
			display->Text(screenID, left, top, &buffer[beginOffset],
				(i == 1) ? display->markPalette : lastPalette,
				endOffset - beginOffset, TRUE, FALSE, &clipRegion, font);
			left += display->TextWidth(&buffer[beginOffset], screenID, font);
			buffer[endOffset] = saveChar;
		}
	}
	// Update the end of the field.
	if (left <= clipRegion.right)
		display->Rectangle(screenID, left, active.top, active.right,
			active.bottom, lastPalette, 0, TRUE, FALSE, &clip);

	lineWidth = left - scrollWidth - active.left + 1;

	if (FlagSet(stFlags, STF_PASSWORD))
		delete passWd;

	// Restore the display and text.
	woStatus &= ~WOS_REDISPLAY;
}


// ----- OS Specific Functions ----------------------------------------------

void UIW_STRING::OSDataGet(void)
{
}

void UIW_STRING::OSDataSet(void)
{
	if (text)
	{
		length = strlen(text);
		cursor = (FlagSet(woFlags, WOF_AUTO_CLEAR) && !FlagSet(woFlags, WOF_VIEW_ONLY)) ? length : 0;
		beginMark = endMark = -1;
		leftClip = 0;
	}

	if (screenID)
		Event(UI_EVENT(S_REDISPLAY));
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
		event.type = S_REDISPLAY;
		Event(event);
	}
}
