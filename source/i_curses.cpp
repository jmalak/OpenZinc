//	Zinc Interface Library - I_CURSES.CPP
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


#include <unistd.h>
#include <stdlib.h>
#define USE_RAW_KEYS
#include "ui_evt.hpp"

#if !defined(ACS_BULLET)
#	define ACS_BULLET	'o'
#	define ACS_UARROW	'^'
#	define ACS_DIAMOND	'-'
#	define ACS_DARROW	'v'
#	define ACS_LARROW	'<'
#	define ACS_RARROW	'>'
#	define ACS_CKBOARD	'#'
#	define ACS_BLOCK	'#'
#endif

int *I_CheckLeadIn(void)
{
	return (ZIL_NULLP(int));
}
// ----- UI_TEXT_DISPLAY ----------------------------------------------------

//void InitializeOutputSymbols(void);
void I_ScreenOpen(int *mode, int *lines, int *columns)
{
	initscr();
	*mode = 0;
#	if defined(SCO_UNIX)
	start_color();
	if (COLOR_PAIRS == 64)
	{
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				init_pair(i*8+j, i, j);
		*mode = 1;
	}
#	endif
	*lines = LINES;
	*columns = COLS;
}

void I_ScreenClose(void)
{
#if defined(__ghs)
	wclear(stdscr);
#else
	wclear(stdscr);
	//clear();
#endif
	refresh();
	endwin();
}

void I_ScreenPut(int left, int top, int right, int bottom, void *buffer)
{
	static char lastAttr = 0;
	int lineLen = 2*(right-left+1);
	int cursorX, cursorY;
	getyx(stdscr, cursorY, cursorX);
	ZIL_SCREEN_CELL *ch = (ZIL_SCREEN_CELL *)buffer;
	ZIL_SCREEN_CELL tmp;
	for (int y=top; y <= bottom; y++)
		for (int x=left; x <= right; x++)
		{
			tmp = *ch++;
			// Check for special characters.
			chtype newChar = 0;
			if (tmp & 0x00000080 || (tmp & 0xFF) < 0x20)
			{
				switch(tmp & 0x000000FF)
				{
				case 0x07:
					newChar = ACS_BULLET;
					break;
				case 0x10:
					newChar = ACS_RARROW;
					break;
				case 0x11:
					newChar = ACS_LARROW;
					break;
				case 0x12:
					newChar = ACS_DIAMOND;
					break;
				case 0x19:
				case 0x1F:
				case 0xF5:
					newChar = ACS_DARROW;
					break;
				case 0x18:
				case 0x1E:
				case 0xFD:
					newChar = ACS_UARROW;
					break;
				case 0xFE:
					newChar = ACS_BLOCK;
					break;
				case 0xF3:
					newChar = ACS_CKBOARD;
					break;

				// Line Draw characters.
				case 0xDA:
				case 0xC9:
					newChar = ACS_ULCORNER;
					break;
				case 0xBF:
				case 0xBB:
					newChar = ACS_URCORNER;
					break;
				case 0xC0:
				case 0xC8:
					newChar = ACS_LLCORNER;
					break;
				case 0xD9:
				case 0xBC:
					newChar = ACS_LRCORNER;
					break;
				case 0xC4:
				case 0xCD:
					newChar = ACS_HLINE;
					break;
				case 0xB3:
				case 0xBA:
					newChar = ACS_VLINE;
					break;
				}
			}
			if (newChar)
				tmp = (A_ATTRIBUTES & tmp) | newChar;
					
			mvaddch(y, x, tmp);
		}
	move(cursorY, cursorX);
	refresh();
}

void I_CursorRemove(void)
{
//	_setcursortype(_NOCURSOR);
//	curs_set(0);
	move(LINES - 1, COLS - 1);
}

void I_CursorPosition(int y, int x, int val)
{
	move(y, x);
//	curs_set(1);
}

int I_GetCodePage(void)
{
//	return (437);
	return (850);
}

void I_MAKENAME(I_Beep)(void)
{
	beep();
}

// ----- UID_MOUSE ----------------------------------------------------------

int I_MouseOpen(void)
{
	return TRUE;
}

void I_MouseClose(void)
{
}

// ----- UID_KEYBOARD -------------------------------------------------------

static int CtrlCHandler(void)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
	return 1;
}

void I_KeyboardOpen(void)
{
	cbreak();
#if defined(SCO_UNIX)
	raw();
#endif
	noecho();
	nonl();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
}

void I_KeyboardClose(void)
{
}

// Assume I_KeyboardQuery() is always called before I_KeyboardRead().
static int _ungot = 0;
static int _altKeyPressed = FALSE;
#if defined(ZIL_PC_KEYBRD)
static int _ctrlKeyPressed = FALSE;
#endif

void I_KeyboardRead(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
#if 0
	while (!_ungot)
		I_KeyboardQuery(shiftState);
	*value = _ungot;
	_ungot = 0;
#else
	if (_ungot)
	{
		*value = _ungot;
		_ungot = 0;
	}
	else
		*value = 0;
#endif
	*shiftState = 0;
	if (_altKeyPressed)
	{
		*shiftState |= S_ALT;
		_altKeyPressed = FALSE;
	}
#if defined(ZIL_PC_KEYBRD)
	else if (_ctrlKeyPressed)
	{
		*shiftState |= S_CTRL;
		_ctrlKeyPressed = FALSE;
	}
#endif
	*rawCode = *value;
}

int I_KeyboardQuery(unsigned *shiftState)
{
	*shiftState = 0;
	if (_ungot)
		return TRUE;

	int ch;
	UI_EVENT event;
/* START BLOCK COMMENT
**		if (UI_DEVICE::eventManager->Get(event, Q_NO_POLL|Q_NO_BLOCK|Q_NO_DESTROY) == -2)
**		{
**			nodelay(stdscr, FALSE);
**			ch = getch();
**			nodelay(stdscr, TRUE);
**		}
**		else
END BLOCK COMMENT */
		ch = getch();
#if defined(SCO_UNIX)
	while (ch == 005) // Repeat while mouse action.
	{
		// Mouse event.
		static unsigned _mouseState = 0;
		static int _horizontalMickeys = 0;
		static int _verticalMickeys = 0;
		_horizontalMickeys = 0;
		_verticalMickeys = 0;
		int mouseCode = getch();
		int button;
		_mouseState &= ~(M_LEFT_CHANGE | M_MIDDLE_CHANGE | M_RIGHT_CHANGE);
		switch (mouseCode)
		{
		case 'L':	// Left button.
			button = getch();
			if (button == 'U')
			{
				_mouseState |= M_LEFT_CHANGE;
				_mouseState &= ~M_LEFT;
			}
			else if (button == 'D')
				_mouseState |= M_LEFT | M_LEFT_CHANGE;
			break;
		case 'M':	// Middle button.
			button = getch();
			if (button == 'U')
			{
				_mouseState |= M_MIDDLE_CHANGE;
				_mouseState &= ~M_MIDDLE;
			}
			else if (button == 'D')
				_mouseState |= M_MIDDLE | M_MIDDLE_CHANGE;
			break;
		case 'R':	// Right button.
			button = getch();
			if (button == 'U')
			{
				_mouseState |= M_RIGHT_CHANGE;
				_mouseState &= ~M_RIGHT;
			}
			else if (button == 'D')
				_mouseState |= M_RIGHT | M_RIGHT_CHANGE;
			break;
		case 'l':
			_horizontalMickeys = -8;
			break;
		case 'r':
			_horizontalMickeys = 8;
			break;
		case 'u':
			_verticalMickeys = -8;
			break;
		case 'd':
			_verticalMickeys = 8;
			break;
		}
		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		ch = getch();
	}
#endif

	if (ch == ERR)
	{
		_ungot = 0;
		return FALSE;
	}
	if (ch == ALT_KEY)
	{
		if (_altKeyPressed)
			_altKeyPressed = FALSE;
		else
		{
			_altKeyPressed = TRUE;
			return FALSE;
		}
	}
#if defined(ZIL_PC_KEYBRD)
	else if (ch == CTRL_KEY)
	{
		if (_ctrlKeyPressed)
			_ctrlKeyPressed = FALSE;
		else
		{
			_ctrlKeyPressed = TRUE;
			return FALSE;
		}
	}
#endif
	_ungot = ch;
	return TRUE;
}

