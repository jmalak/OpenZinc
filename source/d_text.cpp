//	Zinc Interface Library - D_TEXT.CPP
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


#include <limits.h>
#include <stdio.h>
#define USE_RAW_KEYS
#include "ui_win.hpp"

#define HSCROLL_SHOWING		5
#define LEADING				2
#define MARGIN_SIZE			2

typedef ZIL_ICHAR TEXT_CODE;
const TEXT_CODE TXC_END				= -1;
const TEXT_CODE TXC_MARK			= -2;
const TEXT_CODE TXC_RETURN			= -3;
const TEXT_CODE TXC_SPLIT_END		= -4;
const TEXT_CODE TXC_SPLIT_START		= -5;
const TEXT_CODE TXC_START			= -6;
const TEXT_CODE TXC_WRAP			= -7;

enum MOVE_CODE
{
	LEFT = 1,
	RIGHT,
	NEXT_LINE,
	PREV_LINE
};

int UIW_TEXT::GetTextWidth(ZIL_ICHAR *string)
{
#if defined(DOS16PP)
	// Workaround for DPMI16 PowerPak textwidth() bug.
	int bufferLength = 255;
	if (strlen(string) > bufferLength)
	{
		ZIL_ICHAR *stringPtr = string;
		ZIL_ICHAR saveChar;
		int tempLength = bufferLength;
		int textWidth = 0;
		do	{
			saveChar = stringPtr[tempLength];
			stringPtr[tempLength] = 0;
			textWidth += display->TextWidth(stringPtr, screenID, font);
			stringPtr[tempLength] = saveChar;
			stringPtr += tempLength;
			tempLength = MinValue(strlen(stringPtr), bufferLength);
		} while (*stringPtr);

		return textWidth;
	}
	else
		return (display->TextWidth(string, screenID, font));
#else
	return (display->TextWidth(string, screenID, font));
#endif
}

int UIW_TEXT::Move(int *offset, int moveCode)
{
	int newOffset = *offset;
	int direction = (moveCode == LEFT || moveCode == PREV_LINE) ? -1 : 1;
	int moved = FALSE;

	int finished = FALSE;
	while (!finished)
	{
		if (moveCode == 0 && editBuffer[newOffset])
		{
			*offset = newOffset;
			return TRUE;
		}

		if (direction > 0 && editBuffer[newOffset])
		{
			if (moveCode == RIGHT)
			{
				if (moved)
					finished = TRUE;
				else
				{
					moved = TRUE;
					newOffset++;
				}
			}
			else
				newOffset++;
		}
		else if (direction < 0 && editBuffer[newOffset - 2])
		{
			newOffset--;
			if (moveCode == LEFT)
				finished = TRUE;
		}
		else
		{
			switch (editBuffer[newOffset + direction])
			{
			case TXC_START:
			case TXC_END:
				if (moveCode == PREV_LINE && moved)
					finished = TRUE;
				else
				{
					*offset = newOffset;
					return moved;
				}
				break;

			case TXC_RETURN:
			case TXC_WRAP:
				if (moveCode == NEXT_LINE)
				{
					newOffset += 2;
					finished = TRUE;
				}
				else if (moveCode == PREV_LINE)
				{
					if (moved)
						finished = TRUE;
					else
					{
						newOffset -= 2;
						moved = TRUE;
					}
				}
				else
				{
					*offset = newOffset;
					return moved;
				}
				break;

			case TXC_SPLIT_START:
				if (direction > 0)
					newOffset += splitSize;
				else
					newOffset -= 2;
				break;

			case TXC_SPLIT_END:
				if (direction < 0)
					newOffset -= splitSize;
				else
					newOffset += 2;
				break;

			default:
				newOffset += direction * 2;
				break;
			}
		}
	}
	*offset = newOffset;
	return TRUE;
}

void UIW_TEXT::SetCursorPos(const UI_POSITION &position)
{
	int line = 0;
	int column = 0;
	int offset = 2;
	while (line < position.line && Move(&offset, NEXT_LINE))
		line++;
	while (column < position.column && Move(&offset, RIGHT))
		column++;
	SetCursor(offset);
	UpdateScroll();
}

void UIW_TEXT::GetCursorPos(UI_POSITION *position)
{
	position->column = position->line = 0;
	int offset = offsetCursor;
	while (Move(&offset, LEFT))
		position->column++;
	while (Move(&offset, PREV_LINE))
		position->line++;
}

int UIW_TEXT::TextOffset(int editOffset)
{
	int textOffset = 0;
	int offset = 2;
	while (offset < editOffset)
	{
		if (Move(&offset, RIGHT))
			textOffset++;
		else if (Move(&offset, NEXT_LINE))
		{
			if (editBuffer[offset - 1] == TXC_RETURN)
				textOffset += 2;
		}
		else
			break;
	}
	return (textOffset);
}

int UIW_TEXT::EditOffset(int textOffset)
{
	int offset = 0;
	int editOffset = 2;
	while (offset < textOffset)
	{
		if (Move(&editOffset, RIGHT))
			offset++;
		else if (Move(&editOffset, NEXT_LINE))
		{
			if (editBuffer[editOffset - 1] == TXC_RETURN)
				offset += 2;
		}
		else
			break;
	}
	return (editOffset);
}

int UIW_TEXT::CursorOffset(int textOffset)
{
	if (textOffset == -1)
		return TextOffset(offsetCursor);
	else
	{
		SetCursor(EditOffset(textOffset));
		UpdateScroll();
		return (textOffset);
	}
}

void UIW_TEXT::Insert(int *offset, ZIL_ICHAR value, ZIL_ICHAR code)
{
	if (splitSize - 2 < 4)
	{
		UnformatText(text);
		delete editBuffer;
		editSize += editSize / 5 + 10;
		editBuffer = new ZIL_ICHAR[editSize];
		FormatText();
	}

	SetSplit(offset);
	if (value == 0)
	{
		editBuffer[offsetSplit++] = 0;
		splitSize--;
		value = code;
		if (code == TXC_RETURN)
			textLength += 2;
	}
	else
		textLength++;

	editBuffer[offsetSplit++] = (ZIL_ICHAR)value;
	splitSize--;
	editBuffer[offsetSplit] = 0;
	editBuffer[offsetSplit + 1] = TXC_SPLIT_START;
	if (value == TXC_RETURN || value == TXC_WRAP)
		lines++;
}

ZIL_ICHAR UIW_TEXT::Delete(int *offset)
{
	SetSplit(offset);
	if (editBuffer[offsetSplit + splitSize] == 0)
	{
		splitSize++;
		if (editBuffer[offsetSplit + splitSize] == TXC_RETURN)
			textLength -= 2;
	}
	else
	{
		editBuffer[offsetSplit + splitSize - 1] = 0;
		textLength--;
	}

	ZIL_ICHAR delChar = editBuffer[offsetSplit + splitSize];
	editBuffer[offsetSplit + splitSize++] = TXC_SPLIT_END;
	if (delChar == TXC_RETURN || delChar == TXC_WRAP)
		lines--;
	return delChar;
}

void UIW_TEXT::FormatText(void)
{
	// Create edit buffer.
	if (!editBuffer || editSize < maxLength + maxLength / 5 + 10)
	{
		if (editBuffer)
			delete editBuffer;
		editSize = maxLength + maxLength / 5 + 10;
		editBuffer = new ZIL_ICHAR[editSize];
	}

	// Initialize edit buffer.
	splitSize = editSize - 4;
	offsetCursor = offsetTopLine = offsetSplit = 2;
	editBuffer[0] = 0;
	editBuffer[1] = TXC_START;
	editBuffer[offsetSplit] = 0;
	editBuffer[offsetSplit + 1] = TXC_SPLIT_START;
	editBuffer[offsetSplit + splitSize - 2] = 0;
	editBuffer[offsetSplit + splitSize - 1] = TXC_SPLIT_END;
	editBuffer[editSize - 2] = 0;
	editBuffer[editSize - 1] = TXC_END;
	textLength = 0;

	// Copy / Format text.
	lines = width = 1;
	int textIndex = 0;
#if 0
	int lastOffsetSplit = 0;
	int lastSplitSize = 0;
	int lastTextLength = 0;
	int lastLines = 0;
	int lastTextIndex = -1;
	int didWrap = FALSE;
	int maxWidth = client.Width();
	int currentWidth = 0;
	while (text[textIndex])
	{
		if (text[textIndex] == '\r' || text[textIndex] == '\n')
		{
			editBuffer[offsetSplit++] = 0;
			editBuffer[offsetSplit++] = (ZIL_ICHAR)TXC_RETURN;
			textLength += 2;
			splitSize -= 2;
			lines++;
			if (text[textIndex] == '\r' && text[textIndex + 1] == '\n')
				textIndex++;
			currentWidth = 0;
			textIndex++;
			lastTextIndex = -1;
		}
		else
		{
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
			{
				ZIL_ICHAR charString[2];
				charString[0] = text[textIndex];
				charString[1] = 0;
				currentWidth += display->TextWidth(charString, screenID, font);
				if (currentWidth > maxWidth)
				{
					// backup to the last space
					if (text[textIndex] != ' ' && lastTextIndex >= 0)
					{
						offsetSplit = lastOffsetSplit;
						splitSize = lastSplitSize;
						textLength = lastTextLength;
						lines = lastLines;
						textIndex = lastTextIndex;
					}
					editBuffer[offsetSplit++] = 0;
					editBuffer[offsetSplit++] = (ZIL_ICHAR)TXC_WRAP;
					splitSize -= 2;
					lines++;
					currentWidth = 0;
					didWrap = TRUE;
					lastTextIndex = -1;
				}
			}
			if (!(didWrap && text[textIndex] == ' '))
			{
				textLength++;
				editBuffer[offsetSplit++] = (ZIL_ICHAR)text[textIndex];
				splitSize--;
			}
			textIndex++;
			didWrap = FALSE;
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
			{
				if (text[textIndex-1] == ' ')
				{
					lastOffsetSplit = offsetSplit;
					lastSplitSize = splitSize;
					lastTextLength = textLength;
					lastLines = lines;
					lastTextIndex = textIndex;
				}
			}
		}
	}
	editBuffer[offsetSplit] = 0;
	editBuffer[offsetSplit + 1] = TXC_SPLIT_START;

#else
	int bufferIndex = 2;
	do
	{
		int lineIndex = bufferIndex;
		while (text[textIndex] && text[textIndex] != '\r' && text[textIndex] != '\n')
		{
			Insert(&bufferIndex, text[textIndex++]);
			bufferIndex = offsetSplit;
		}
		if (text[textIndex])
		{
			Insert(&bufferIndex, 0, TXC_RETURN);
			bufferIndex = offsetSplit;
		}
		if (!FlagSet(wnFlags, WNF_NO_WRAP))
		{
			Wrap(lineIndex, FALSE, FALSE);
			bufferIndex = editSize - 2;
			SetSplit(&bufferIndex);
		}
		else
			width = MaxValue(width, LineWidth(lineIndex) / (client.Width() / HSCROLL_SHOWING) + 1);
		if (text[textIndex] == '\r' && text[textIndex + 1] == '\n')
			textIndex++;
	} while (text[textIndex++]);
#endif

	// Initialize Data.
	scroll.left = scroll.top = 0;
	cursorPos.column = cursorPos.line = 0;
	lastCursorColumn = 0;
	vScrollInfo.minimum = vScrollInfo.maximum = vScrollInfo.current =
		vScrollInfo.showing = 0;
	vScrollInfo.delta = 1;
	hScrollInfo.minimum = hScrollInfo.maximum = hScrollInfo.current =
		hScrollInfo.showing = 0;
	hScrollInfo.delta = 1;
}

int UIW_TEXT::UnformatText(ZIL_ICHAR *string, int startOffset, int endOffset)
{
	if (!editBuffer)
		return (0);

	int stringIndex = 0;
	int editIndex = startOffset;
	int finished = FALSE;
	if (!editBuffer)
		return (strlen(text));
	while (!finished)
	{
		if (editBuffer[editIndex])
		{
			if (string)
				string[stringIndex] = editBuffer[editIndex];
			stringIndex++;
			editIndex++;
		}
		else
		{
			if (editBuffer[editIndex + 1] == TXC_RETURN)
			{
				if (string)
				{
					string[stringIndex] = '\r';
					string[stringIndex + 1] = '\n';
				}
				stringIndex += 2;
				editIndex += 2;
			}
			else if (editBuffer[editIndex + 1] == TXC_SPLIT_START)
				editIndex += splitSize;
			else if (editBuffer[editIndex + 1] == TXC_END)
				finished = TRUE;
			else
				editIndex += 2;
		}
		if (endOffset > 0 && editIndex >= endOffset)
			finished = TRUE;
	}
	if (string)
		string[stringIndex] = 0;
	return (stringIndex);
}

void UIW_TEXT::Wrap(int offset, int testPrevLine, int draw, int drawToEnd)
{
	int oldLines = lines;
	offsetSave = offsetCursor;
	SetCursor(offset);
	int maxWidth = client.Width();

	while (Move(&offset, LEFT))
		;
	if (testPrevLine && editBuffer[offset - 1] == TXC_WRAP)
		Move(&offset, PREV_LINE);

	int finished = FALSE;
	while (!finished)
	{
		int lineWidth = LineWidth(offset, TRUE);
		if (lineWidth > maxWidth)
		{
		 	while (Move(&offset, RIGHT))
				;
			while (Move(&offset, LEFT) && editBuffer[offset] == ' ')
				;
			int wrapOffset = 0;
			int forceOffset = 0;
			while (!wrapOffset)
			{
				int prevOffset = offset;
				if (!Move(&prevOffset, LEFT))
					wrapOffset = forceOffset;
				else
				{
					lineWidth -= CharWidth(offset);
					if (lineWidth < maxWidth)
					{
						if (editBuffer[prevOffset] == ' ')
							wrapOffset = offset;
						else if (!forceOffset)
							forceOffset = offset;
					}
					offset = prevOffset;
				}
			}
	
			if (wrapOffset <= offsetCursor)
				SetCursor(wrapOffset);

			Insert(&wrapOffset, 0, TXC_WRAP);
	
			if (draw)
				Draw(offsetCursor, 0, TRUE);

			offset = offsetSplit;
			SetCursor(offsetSplit);
		}
		else
		{
			int lineOffset = offset;
			if (Move(&offset, NEXT_LINE) && editBuffer[offset - 1] == TXC_WRAP)
			{
				int wrapOffset = offset - 2;
				int lastChar = wrapOffset;
				Move(&lastChar, LEFT);

				if (editBuffer[lastChar] == ' ' && editBuffer[offset] != ' ')
				{
					do
					{
						lineWidth += CharWidth(offset);
					} while (lineWidth <= maxWidth && Move(&offset, RIGHT) && editBuffer[offset] != ' ');
					lineWidth += CharWidth(offset);
				}
	
				if (lineWidth <= maxWidth)
				{
					if (wrapOffset < offsetCursor)
						SetCursor(wrapOffset);

					Delete(&wrapOffset);
					woStatus |= WOS_CHANGED;

					offset = lineOffset;
					continue;
				}
				offset = lineOffset;
			}

			if (!Move(&offset, NEXT_LINE) || offsetCursor < offset)
			{
				if (draw)
				{
					if (lines != oldLines)
						drawToEnd = TRUE;
					if (drawToEnd || editBuffer[offset + 1] == TXC_END)
						Draw(offsetCursor, -1, TRUE);
					else
						Draw(offsetCursor, 0, TRUE);
				}
				finished = TRUE;
			}
		}
	}
	SetCursor(offsetSave);
}

void UIW_TEXT::UpdateScroll(int redisplay, int scrollCursor)
{
	UI_EVENT scrollEvent;

	int showing = client.Height() / lineHeight;
	int maximum = MaxValue(0, lines - showing);
	vScrollInfo.maximum = maximum;
	vScrollInfo.showing = showing;
	if (vScroll)
	{
		scrollEvent.type = S_VSCROLL_SET;
		scrollEvent.scroll = vScrollInfo;
		vScroll->Event(scrollEvent);
	}

	maximum = MaxValue(0, width - HSCROLL_SHOWING);
	hScrollInfo.maximum = maximum;
	hScrollInfo.showing = HSCROLL_SHOWING;
	if (hScroll)
	{
		scrollEvent.type = S_HSCROLL_SET;
		scrollEvent.scroll = hScrollInfo;
		hScroll->Event(scrollEvent);
	}

	if (cursorPos.column < 0 && scrollCursor)
	{
		scrollEvent.type = S_HSCROLL;
		scrollEvent.scroll.delta = cursorPos.column / (client.Width() / HSCROLL_SHOWING) - 1;
		Event(scrollEvent);
	}
	else if (cursorPos.column >= client.Width() && scrollCursor)
	{
		scrollEvent.type = S_HSCROLL;
		scrollEvent.scroll.delta = (cursorPos.column - client.Width()) / (client.Width() / HSCROLL_SHOWING) + 1;
		Event(scrollEvent);
	}

	if (cursorPos.line < 0 && scrollCursor)
	{
		scrollEvent.type = S_VSCROLL;
		scrollEvent.scroll.delta = cursorPos.line / lineHeight;
		Event(scrollEvent);
	}
	else if (cursorPos.line + lineHeight - 1 >= client.Height() && scrollCursor)
	{
		scrollEvent.type = S_VSCROLL;
		scrollEvent.scroll.delta = (cursorPos.line + lineHeight - 1) / lineHeight - showing + 1;
		Event(scrollEvent);
	}

	if (redisplay && vScroll)
		vScroll->Event(UI_EVENT(S_REDISPLAY));
	if (redisplay && hScroll)
		hScroll->Event(UI_EVENT(S_REDISPLAY));
	UpdateCursor();
}

void UIW_TEXT::UpdateCursor(void)
{
	if (FlagSet(woStatus, WOS_CURRENT))
	{
		int cSize = display->isText ? 0 : 14;

		if (cursorPos.column >= 0 && cursorPos.column <= client.Width() &&
			cursorPos.line >= 0 && cursorPos.line + lineHeight <= client.Height() &&
			client.left + cursorPos.column >= clip.left && client.left + cursorPos.column <= clip.right &&
			client.top + cursorPos.line >= clip.top && client.top + cursorPos.line + cSize <= clip.bottom)
		{
			eventManager->DeviceState(E_CURSOR, insertMode ? DC_INSERT : DC_OVERSTRIKE);
			eventManager->DevicePosition(E_CURSOR, client.left + cursorPos.column -
				(display->isText ? 0 : 1), client.top + cursorPos.line);
		}
		else
			eventManager->DeviceState(E_CURSOR, D_OFF);
	}
}

int UIW_TEXT::CharWidth(int offset)
{
	while (!editBuffer[offset])
	{
		switch (editBuffer[offset + 1])
		{
		case TXC_END:
		case TXC_RETURN:
		case TXC_WRAP:
			return 0;

		case TXC_SPLIT_START:
			offset += splitSize;
			break;

		default:
			offset += 2;
			break;
		}
	}

	ZIL_ICHAR charString[2];
	charString[0] = editBuffer[offset];
	charString[1] = 0;

	return (GetTextWidth(charString));
}

int UIW_TEXT::LineWidth(int offset, int wrap)
{
	int width = 0;
	int finished = FALSE;
	while (!finished)
	{
		if (editBuffer[offset])
		{
			width += GetTextWidth(&editBuffer[offset]);
			while (editBuffer[offset])
				offset++;
		}

		switch (editBuffer[offset + 1])
		{
		case TXC_END:
		case TXC_RETURN:
		case TXC_WRAP:
			if (wrap)
				while (Move(&offset, LEFT) && editBuffer[offset] == ' ')
					width -= CharWidth(offset);
			finished = TRUE;
			break;

		case TXC_SPLIT_START:
			offset += splitSize;
			break;

		default:
			offset += 2;
			break;
		}
	}
	return width;
}

void UIW_TEXT::SetSplit(int *offset)
{
	int newOffset = *offset;
	if (newOffset != offsetSplit)
	{
		if (newOffset < offsetSplit)
		{
			memmove(&editBuffer[newOffset + splitSize], &editBuffer[newOffset],
				(offsetSplit - newOffset) * sizeof(ZIL_ICHAR));
			if (offsetCursor > newOffset && offsetCursor <= offsetSplit)
				offsetCursor += splitSize;
			if (offsetTopLine > newOffset && offsetTopLine <= offsetSplit)
				offsetTopLine += splitSize;
			if (offsetMarkAnchor >= newOffset && offsetMarkAnchor <= offsetSplit)
				offsetMarkAnchor += splitSize;
			if (offsetMarkFloat >= newOffset && offsetMarkFloat <= offsetSplit)
				offsetMarkFloat += splitSize;
			if (offsetSave >= newOffset && offsetSave <= offsetSplit)
				offsetSave += splitSize;
		}
		else if (newOffset > offsetSplit)
		{
			memmove(&editBuffer[offsetSplit], &editBuffer[offsetSplit + splitSize],
				(newOffset - offsetSplit - splitSize) * sizeof(ZIL_ICHAR));
			if (offsetCursor <= newOffset && offsetCursor > offsetSplit)
				offsetCursor -= splitSize;
			if (offsetTopLine <= newOffset && offsetTopLine > offsetSplit)
				offsetTopLine -= splitSize;
			if (offsetMarkAnchor < newOffset && offsetMarkAnchor > offsetSplit)
				offsetMarkAnchor -= splitSize;
			if (offsetMarkFloat < newOffset && offsetMarkFloat > offsetSplit)
				offsetMarkFloat -= splitSize;
			if (offsetSave < newOffset && offsetSave > offsetSplit)
				offsetSave -= splitSize;
			newOffset -= splitSize;
		}
		offsetSplit = newOffset;
		editBuffer[offsetSplit] = 0;
		editBuffer[offsetSplit + 1] = TXC_SPLIT_START;
		editBuffer[offsetSplit + splitSize - 2] = 0;
		editBuffer[offsetSplit + splitSize - 1] = TXC_SPLIT_END;
	}
	*offset = newOffset;
}

EVENT_TYPE UIW_TEXT::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	display->VirtualGet(screenID, trueRegion);
	if (FlagSet(woFlags, WOF_VIEW_ONLY) && FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
		lastPalette = LogicalPalette(ccode, ID_WINDOW);
	UIW_WINDOW::DrawItem(event, ccode);
	Draw(offsetTopLine, -1);
	display->VirtualPut(screenID);
	return (ccode);
}

void UIW_TEXT::Draw(int startOffset, int endOffset, int erase, int markOnly)
{
	UI_POSITION drawPosition;

	UI_REGION clipRegion;
	client.Overlap(clip, clipRegion);

	GetPosition(startOffset, &drawPosition);
	int markStart = MinValue(offsetMarkAnchor, offsetMarkFloat);
	int markEnd = MaxValue(offsetMarkAnchor, offsetMarkFloat);

	display->VirtualGet(screenID, client);

	int offset = startOffset;
	int finished = FALSE;
	while (!finished)
	{
		if (editBuffer[offset])
		{
			UI_PALETTE *palette = ((markOnly && !erase) || (!markOnly &&
					offset > markStart && offset < markEnd)) ?
					display->markPalette : lastPalette;
			display->Text(screenID, client.left + drawPosition.column,
				client.top + drawPosition.line, &editBuffer[offset], palette,
				-1, TRUE, FALSE, &clipRegion, font);
			drawPosition.column += GetTextWidth(&editBuffer[offset]);
			while (editBuffer[offset])
				offset++;
		}

		ZIL_ICHAR code = editBuffer[offset + 1];
		switch (code)
		{
		case TXC_END:
		case TXC_RETURN:
		case TXC_WRAP:
			if (erase && !markOnly)
			{
				if (drawPosition.column < client.Width())
				display->Rectangle(screenID, client.left + drawPosition.column,
					client.top + drawPosition.line, client.right, client.top +
					drawPosition.line + lineHeight, lastPalette, 0, TRUE,	FALSE, &clipRegion);
			}
			drawPosition.column = -scroll.left;
			drawPosition.line += lineHeight;
			offset += 2;
			if (!endOffset || code == TXC_END)
				finished = TRUE;
			break;

		case TXC_SPLIT_START:
			offset += splitSize;
			break;

		default:
			offset += 2;
			break;
		}

		if (endOffset > 0 && offset >= endOffset || drawPosition.line > client.Height())
			finished = TRUE;
	}

	if (endOffset == -1 && erase && !markOnly)
	{
		if (drawPosition.line < client.Height())
			display->Rectangle(screenID, client.left, client.top + drawPosition.line,
				client.right, client.bottom, lastPalette, 0, TRUE,
				FALSE, &clipRegion);
	}
	display->VirtualPut(screenID);
}


EVENT_TYPE UIW_TEXT::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_TEXT);

	if (ccode >= L_MARK_LEFT && ccode <= L_MARK_WORD_LEFT && !offsetMarkAnchor)
		SetMark(offsetCursor);
	else if (ccode >= L_UP && ccode <= L_PGDN && offsetMarkAnchor)
		SetMark(0);

	switch (ccode)
	{

// For Testing purposes only!
/* START BLOCK COMMENT
**		case L_PREVIOUS:
**			{
**			printf("\n!%d!$%d$", offsetCursor, offsetSplit);
**			int index = 0;
**			int finished = FALSE;
**			while (!finished)
**			{
**				if (editBuffer[index])
**				{
**					printf("%c",editBuffer[index]);
**					index++;
**				}
**				else
**				{
**					printf("(%d)[%d]", editBuffer[index], editBuffer[index + 1]);
**					if (editBuffer[index+1] == TXC_SPLIT_START)
**					{
**						printf("{%d}", splitSize);
**						index += splitSize - 2;
**					}
**					else if (editBuffer[index+1] == TXC_END)
**						finished = TRUE;
**					else
**						index += 2;
**				}
**			}
**			}
**			break;
END BLOCK COMMENT */

	case S_INITIALIZE:
		wnFlags |= WNF_NO_SCROLL;
		ccode = UIW_WINDOW::Event(event);
		lineHeight = display->TextHeight(ZIL_NULLP(ZIL_ICHAR), screenID, font) +
			(display->isText ? 0 : LEADING);
		marginSize = display->isText ? 0 : MARGIN_SIZE;
		offsetMarkAnchor = offsetMarkFloat = 0;
		offsetCursor = offsetTopLine = 2;
		break;

	case S_CREATE:
	case S_CHANGED:
		{
		int oldWidth = trueRegion.Width();
		int oldTopLine = TextOffset(offsetTopLine);
		int oldCursor = TextOffset(offsetCursor);
		int oldMarkAnchor = offsetMarkAnchor ? TextOffset(offsetMarkAnchor) : -1;

		ccode = UIW_WINDOW::Event(event);

		// Don't do any calculations if a parent is minimized.
		for (UI_WINDOW_OBJECT *pObject = parent; pObject; pObject = pObject->parent)
			if (FlagSet(pObject->woStatus, WOS_MINIMIZED))
				return ccode;

		client = clipList.First()->region;
		client.left += marginSize;

		if (ccode == S_CREATE)
		{
			FormatText();
			UpdateScroll(FALSE, FALSE);
		}
		else if (!FlagSet(wnFlags, WNF_NO_WRAP) && trueRegion.Width() != oldWidth)
		{
			// Reformat edit buffer.
			SetMark(0, FALSE);
			UnformatText(text);
			FormatText();

			// Restore scroll position.
			int offset = EditOffset(oldTopLine);
			while (Move(&offset, LEFT))
				;
			while (offsetTopLine < offset && Move(&offsetTopLine, NEXT_LINE))
				vScrollInfo.current++;
			SetCursor(offsetTopLine);
			cursorPos.column = cursorPos.line = 0;
			UpdateScroll(FALSE, FALSE);

			// Restore cursor and mark positions.
			SetCursor(EditOffset(oldCursor));
			if (oldMarkAnchor > -1)
			{
				SetMark(EditOffset(oldMarkAnchor), FALSE);
				SetMark(offsetCursor, FALSE);
			}
		}
		else
			UpdateScroll(FALSE, FALSE);
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		{
		WOS_STATUS oldStatus = woStatus;
		ccode = UIW_WINDOW::Event(event);
		if (!text || !*text)
			offsetMarkAnchor = 0;
		if (event.type == S_CURRENT)
		{
			if (FlagSet(woFlags, WOF_AUTO_CLEAR) && !FlagSet(oldStatus, WOS_CURRENT))
			{
				SetCursor(2);
				SetMark(offsetCursor, FALSE);
				SetCursor(editSize - 2);
				SetMark(offsetCursor, TRUE);
			}
		  	UpdateCursor();
		}	
		else
			SetMark(0);
		}
		break;

	case S_HSCROLL:
		{
		int scrollDelta = event.scroll.delta;
		if (scrollDelta + hScrollInfo.current < 0)
			scrollDelta = -hScrollInfo.current;
		else if (scrollDelta + hScrollInfo.current > hScrollInfo.maximum)
			scrollDelta = hScrollInfo.maximum - hScrollInfo.current;

		if (scrollDelta == 0)
			break;

		scroll.left += scrollDelta * (client.Width() / HSCROLL_SHOWING);
		cursorPos.column -= scrollDelta * (client.Width() / HSCROLL_SHOWING);

		Draw(offsetTopLine, -1, TRUE);
		if (hScroll)
			hScroll->Event(event);

		hScrollInfo.current += scrollDelta;
		UpdateCursor();
		}
		break;

	case S_VSCROLL:
		{
		int scrollDelta = event.scroll.delta;

		if (scrollDelta + vScrollInfo.current < 0)
			scrollDelta = -vScrollInfo.current;
		else if (scrollDelta + vScrollInfo.current > vScrollInfo.maximum)
			scrollDelta = vScrollInfo.maximum - vScrollInfo.current;

		if (scrollDelta == 0)
			break;

		int i;
		if (scrollDelta > 0)
			for (i = 0; i < scrollDelta; i++)
				Move(&offsetTopLine, NEXT_LINE);
		else
			for (i = 0; i > scrollDelta; i--)
				Move(&offsetTopLine, PREV_LINE);

		scroll.top += scrollDelta * lineHeight;
		cursorPos.line -= scrollDelta * lineHeight;
		Draw(offsetTopLine, -1, TRUE);
		if (vScroll)
			vScroll->Event(event);
		vScrollInfo.current += scrollDelta;
		UpdateCursor();
		}
		break;

	case L_EOL:
	case L_MARK_EOL:
		{
		int offset = offsetCursor;
		while (Move(&offset, RIGHT))
			;
		SetCursor(offset);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_BOL:
	case L_MARK_BOL:
		{
		int offset = offsetCursor;
		while (Move(&offset, LEFT))
			;
		SetCursor(offset);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_TOP:
		SetCursor(2);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		break;

	case L_BOTTOM:
		SetCursor(editSize - 2);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		break;

	case L_WORD_LEFT:
	case L_MARK_WORD_LEFT:
		{
		int offset = offsetCursor;
		if (!Move(&offset, LEFT) && editBuffer[offset - 1] != TXC_START)
			offset -= 2;
		int wordOffset = offset;
		while (Move(&offset, LEFT) && editBuffer[offset] != ' ')
			wordOffset = offset;
		SetCursor(wordOffset);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_WORD_RIGHT:
	case L_MARK_WORD_RIGHT:
		{
		int offset = offsetCursor;
		while (editBuffer[offset] != ' ' && Move(&offset, RIGHT))
			;
		if (!Move(&offset, RIGHT) && editBuffer[offset + 1] != TXC_END)
			offset += 2;
		SetCursor(offset);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_LEFT:
	case L_MARK_LEFT:
		{
		int offset = offsetCursor;
		if (!Move(&offset, LEFT))
		{
			if (editBuffer[offset - 1] != TXC_START)
				offset -= 2;
			else
				return S_UNKNOWN;
		}
		SetCursor(offset);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_RIGHT:
	case L_MARK_RIGHT:
		{
		int offset = offsetCursor;
		if (!Move(&offset, RIGHT) && editBuffer[offset + 1] != TXC_END)
			offset += 2;
		SetCursor(offset);
		UpdateScroll();
		lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_UP:
	case L_MARK_UP:
		{
		UI_POSITION position = cursorPos;
		position.column = lastCursorColumn;
		position.line -= lineHeight;
		SetCursor(position);
		UpdateScroll();
		}
		break;
	
	case L_DOWN:
	case L_MARK_DOWN:
		{
		int offset = offsetCursor;
		if (Move(&offset, NEXT_LINE))
		{
			UI_POSITION position = cursorPos;
			position.column = lastCursorColumn;
			position.line += lineHeight + 1;
			SetCursor(position);
			UpdateScroll();
		}
		}
		break;

	case L_PGUP:
	case L_MARK_PGUP:
		{
		UI_POSITION position = cursorPos;
		UI_EVENT scrollEvent(S_VSCROLL);
		scrollEvent.scroll.delta = -MinValue(vScrollInfo.showing - 1, vScrollInfo.current);
		position.line -= (vScrollInfo.showing + scrollEvent.scroll.delta - 1) * lineHeight;
		position.column = lastCursorColumn;
		Event(scrollEvent);
		SetCursor(position);
		UpdateCursor();
		}
		break;

	case L_PGDN:
	case L_MARK_PGDN:
		{
		UI_POSITION position = cursorPos;
		UI_EVENT scrollEvent(S_VSCROLL);
		scrollEvent.scroll.delta = MinValue(vScrollInfo.showing - 1,
			vScrollInfo.maximum - vScrollInfo.current);
		position.line += (vScrollInfo.showing - scrollEvent.scroll.delta - 1) * lineHeight;
		position.column = lastCursorColumn;
		Event(scrollEvent);
		SetCursor(position);
		UpdateCursor();
		}
		break;

	case L_DELETE:
		if(!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			if (offsetMarkAnchor)
			{
				SetCursor(MinValue(offsetMarkAnchor, offsetMarkFloat));
				UpdateScroll();
				int oldLines = lines;
				Delete(&offsetCursor);
				woStatus |= WOS_CHANGED;
				while (Delete(&offsetCursor) != TXC_MARK)
					;
				offsetMarkAnchor = offsetMarkFloat = 0;

				int drawToEnd = lines != oldLines;

				if (!FlagSet(wnFlags, WNF_NO_WRAP))
					Wrap(offsetCursor, TRUE, TRUE, drawToEnd);
				else if (drawToEnd)
					Draw(offsetSplit, -1, TRUE);
				else
					Draw(offsetSplit, 0, TRUE);
			}
			else
			{
				int offset = offsetCursor;
				if (Move(&offset, RIGHT))
				{
					Delete(&offsetCursor);
					woStatus |= WOS_CHANGED;

					if (!FlagSet(wnFlags, WNF_NO_WRAP))
						Wrap(offsetSplit, TRUE);
					else
						Draw(offsetSplit, 0, TRUE);
				}
				else if (Move(&offset, NEXT_LINE))
				{
					Delete(&offsetCursor);
					woStatus |= WOS_CHANGED;

					if (!FlagSet(wnFlags, WNF_NO_WRAP))
						Wrap(offsetSplit, FALSE, TRUE, TRUE);
					else
						Draw(offsetSplit, -1, TRUE);
				}
			}
			UpdateScroll(TRUE);
			lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_BACKSPACE:
		if(!FlagSet(woFlags, WOF_VIEW_ONLY) && Event(UI_EVENT(L_LEFT)) != S_UNKNOWN)
			Event(UI_EVENT(L_DELETE));
		break;

	case L_INSERT_TOGGLE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
			insertMode = !insertMode;
		eventManager->DeviceState(E_CURSOR, insertMode ? DC_INSERT : DC_OVERSTRIKE);
		break;

	case E_KEY:
		if(!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			if (offsetMarkAnchor)
				Event(UI_EVENT(L_DELETE));

			if (IsPrint(event.key.value) && textLength < maxLength)
			{
				int insertOffset = offsetCursor;
				SetSplit(&insertOffset);
				if (!insertMode && editBuffer[insertOffset + splitSize])
					Delete(&insertOffset);
				Insert(&insertOffset, event.key.value);
				woStatus |= WOS_CHANGED;
				if (!FlagSet(wnFlags, WNF_NO_WRAP))
					Wrap(insertOffset, event.key.value == ' ');
				else
				{
					Draw(insertOffset, 0, TRUE);
					while (Move(&insertOffset, LEFT))
						;
					width = MaxValue(width, LineWidth(insertOffset) / (client.Width() / HSCROLL_SHOWING) + 1);
				}
				SetCursor(offsetCursor + 1);
			}
#if defined(ZIL_CURSES)
			else if (event.rawCode == ENTER && textLength + 2 <= maxLength)
#else
			else if ((event.rawCode == ENTER || event.rawCode == GRAY_ENTER) &&
				textLength + 2 <= maxLength)
#endif
			{
				int insertOffset = offsetCursor;
				Insert(&insertOffset, 0, TXC_RETURN);
				woStatus |= WOS_CHANGED;
				SetCursor(offsetSplit);

				if (!FlagSet(wnFlags, WNF_NO_WRAP))
					Wrap(insertOffset, TRUE, TRUE, TRUE);
				else
					Draw(insertOffset, -1, TRUE);
				// Don't call default button.
				ccode = S_UNKNOWN;
			}
			UpdateScroll();
			lastCursorColumn = scroll.left + cursorPos.column;
		}
		break;

	case L_BEGIN_MARK:
	case L_CONTINUE_MARK:
	case L_END_MARK:
	case L_CONTINUE_COPY_DRAG:
	case L_END_COPY_DRAG:
	case L_VIEW:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if (windowManager->dragObject)
		{
			UI_WINDOW_OBJECT::Event(event);
			break;
		}

		if (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			return ccode;

		if (clipList.First()->region.Overlap(event.position))
		{
			if (ccode == L_VIEW)
				eventManager->DeviceState(E_MOUSE, DM_EDIT);
			else if (ccode != L_END_MARK)
			{
				UI_POSITION position = event.position;
				position.column -= client.left;
				position.line -= client.top;
				SetCursor(position);
				if (ccode == L_BEGIN_MARK)
					SetMark(0);
				SetMark(offsetCursor);
				UpdateCursor();
				lastCursorColumn = scroll.left + cursorPos.column;

				if (ccode == L_BEGIN_MARK &&
					FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
				   windowManager->dragObject = this;
			}
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;

	case L_CUT:
	case L_COPY_MARK:
		{
		if (!offsetMarkAnchor)
			break;

		int markStart = MinValue(offsetMarkAnchor, offsetMarkFloat);
		int markEnd = MaxValue(offsetMarkAnchor, offsetMarkFloat);

		if (pasteBuffer)
			delete pasteBuffer;

		int pasteSize = UnformatText(ZIL_NULLP(ZIL_ICHAR), markStart, markEnd);
		pasteBuffer = new ZIL_ICHAR[pasteSize + 1];
		UnformatText(pasteBuffer, markStart, markEnd);

		if (ccode == L_CUT)
			Event(UI_EVENT(L_DELETE));
		else
			SetMark(0);
		UpdateScroll(TRUE);
		}
		break;

	case L_PASTE:
		if (pasteBuffer)
		{
			int oldLines = lines;
			int updateOffset = offsetCursor;

			// Insert the text.
			int pasteIndex = 0;
			int bufferIndex = offsetCursor;
			do
			{
				while (pasteBuffer[pasteIndex] && pasteBuffer[pasteIndex] != '\r' && pasteBuffer[pasteIndex] != '\n')
				{
					Insert(&bufferIndex, pasteBuffer[pasteIndex++]);
					woStatus |= WOS_CHANGED;
					bufferIndex = offsetSplit;
				}
				if (pasteBuffer[pasteIndex])
				{
					Insert(&bufferIndex, 0, TXC_RETURN);
					bufferIndex = offsetSplit;
				}
				if (pasteBuffer[pasteIndex] == '\r' && pasteBuffer[pasteIndex + 1] == '\n')
					pasteIndex++;
			} while (pasteBuffer[pasteIndex++]);

			// Update.
			SetCursor(bufferIndex);
			int drawToEnd = lines != oldLines;
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
				Wrap(updateOffset, TRUE, TRUE, drawToEnd);
			else if (drawToEnd)
				Draw(updateOffset, -1, TRUE);
			else
				Draw(updateOffset, 0, TRUE);
			UpdateScroll();
		}
		break;

	case S_DEINITIALIZE:
		if (FlagSet(woStatus, WOS_CURRENT))
		{
			woStatus &= ~WOS_CURRENT;
			eventManager->DeviceState(E_CURSOR, D_OFF);
		}
		return (UIW_WINDOW::Event(event));
	
	default:
		ccode = UIW_WINDOW::Event(event);
	}

	if (ccode >= L_MARK_LEFT && ccode <= L_MARK_WORD_LEFT)
		SetMark(offsetCursor);

	return (ccode);
}

void UIW_TEXT::SetMark(int offset, int repaint)
{
	if (offset == 0)
	{
		if (offsetMarkAnchor)
		{
			if (repaint)
				Draw(MinValue(offsetMarkAnchor, offsetMarkFloat),
					MaxValue(offsetMarkAnchor, offsetMarkFloat), TRUE, TRUE);	
			Delete(&offsetMarkAnchor);
			Delete(&offsetMarkFloat);
			offsetMarkAnchor = offsetMarkFloat = 0;
		}
	}
	else if (!offsetMarkAnchor)
	{
		offsetMarkAnchor = offsetMarkFloat = offset;
		Insert(&offsetMarkAnchor, 0, TXC_MARK);
		Insert(&offsetMarkFloat, 0, TXC_MARK);
	}
	else
	{
		offsetSave = MinValue(MaxValue(MinValue(offsetCursor, offsetMarkFloat),
			offsetMarkAnchor), MaxValue(offsetCursor, offsetMarkFloat));

		// Insert mark code.
		Insert(&offsetCursor, 0, TXC_MARK);

		if (offsetCursor == offsetMarkAnchor)
			errorSystem->Beep();

		// Erase.
		if (offsetMarkFloat != offsetSave && repaint)
			Draw(MinValue(offsetMarkFloat, offsetSave),
				MaxValue(offsetMarkFloat, offsetSave), TRUE, TRUE);

		// Draw.
		if (offsetCursor != offsetSave && repaint)
			Draw(MinValue(offsetCursor, offsetSave),
				MaxValue(offsetCursor, offsetSave), FALSE, TRUE);

		// Delete mark code.
		Delete(&offsetMarkFloat);
		offsetMarkFloat = offsetCursor;
	}
}

void UIW_TEXT::SetCursor(const UI_POSITION &position)
{
	UI_POSITION tPosition = cursorPos;
	int offset = offsetCursor;
	while (tPosition.line + lineHeight - 1 < position.line && Move(&offset, NEXT_LINE))
		tPosition.line += lineHeight;
	while (tPosition.line > position.line && Move(&offset, PREV_LINE))
		tPosition.line -= lineHeight;

	if (offset != offsetCursor)
		tPosition.column = -scroll.left;

	if (tPosition.column < position.column)
	{
		int charWidth = CharWidth(offset);
		while (tPosition.column + charWidth / 2 < position.column && Move(&offset, RIGHT))
		{
			tPosition.column += charWidth;
			charWidth = CharWidth(offset);
		}
	}
	else
	{
		int prevOffset = offset;
		Move(&prevOffset, LEFT);
		int charWidth = CharWidth(prevOffset);
		tPosition.column -= charWidth;
		while (tPosition.column + charWidth / 2 >= position.column)
		{
			offset = prevOffset;
			if (!Move(&prevOffset, LEFT))
				break;
			charWidth = CharWidth(prevOffset);
			tPosition.column -= charWidth;
		}
	}

	SetCursor(offset);
}

void UIW_TEXT::SetCursor(int offset)
{
	GetPosition(offset, &cursorPos);
	offsetCursor = offset;
}

void UIW_TEXT::GetPosition(int offset, UI_POSITION *position)
{
	*position = cursorPos;
	int tOffset = offsetCursor;

	int lineOffset = tOffset;
	if (tOffset < offset)
	{
		while (Move(&tOffset, NEXT_LINE) && tOffset <= offset)
		{
			position->line += lineHeight;
			lineOffset = tOffset;
		}
	}
	else
	{
		while(Move(&tOffset, LEFT))
			;
		while (tOffset > offset && Move(&tOffset, PREV_LINE))
		{
			position->line -= lineHeight;
			lineOffset = tOffset;
		}
	}

	tOffset = lineOffset;
	if (tOffset != offsetCursor)
		position->column = -scroll.left;
	Move(&tOffset, 0);

	if (tOffset < offset)
		do
			position->column += CharWidth(tOffset);
		while (Move(&tOffset, RIGHT) && tOffset < offset);
	else
		while (Move(&tOffset, LEFT) && tOffset >= offset)
			position->column -= CharWidth(tOffset);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TEXT::OSDataGet(void)
{
	UnformatText(text);
}

void UIW_TEXT::OSDataSet(void)
{
	// Redisplay the text field.
	if (screenID)
	{
		SetMark(0, FALSE);
		FormatText();
		UpdateScroll();
		Event(UI_EVENT(S_REDISPLAY));
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
		event.type = S_REDISPLAY;
		Event(event);
	}
}
