//	Zinc Interface Library - I_WCCAT.CPP
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

#include <conio.h>
#include <signal.h>
#include <dos.h>
#include <graph.h>
#include <stdlib.h>
#define USE_RAW_KEYS
#include "ui_evt.hpp"

#if !defined(ZIL_HARDWARE)
	ZIL_ICHAR _textDriverName[] = { 'A','T',0 };
#endif

int *I_MAKENAME(I_CheckLeadIn)(void)
{
	return (ZIL_NULLP(int));
}
// ----- UI_TEXT_DISPLAY ----------------------------------------------------

static int _originalMode = -1;

#define OutPortByte 		outp
#define MKFP(s,o)	    	(((long)s << 16) | o)
#define MDA_SEGMENT			0xb000
#define MDA_LIN_SEGMENT		(char *)((MDA_SEGMENT) << 4)
#define ZIL_COLOR_SEGMENT		0xb800
#define ZIL_COLOR_LIN_SEGMENT	(char *)((ZIL_COLOR_SEGMENT) << 4)

static char *_videoSegment;
static int _videoWidth;
static int _originalCursor;

void I_ScreenOpen(int *mode, int *lines, int *columns)
{
	union REGS inregs, outregs;

	// Save original text mode.
	struct videoconfig textInfo;
	if (_originalMode == -1)
	{
		_getvideoconfig(&textInfo);
		_originalMode = textInfo.mode;
	}
	if (*mode == TDM_43x80 &&
		_originalMode != 0 && _originalMode != 2 && _originalMode != 7)
	{
		_setvideomoderows(_DEFAULTMODE, 43);
	}
	else if (*mode == TDM_25x80 || *mode == TDM_25x40)
		_setvideomoderows(*mode, 25);
	else if (*mode != TDM_AUTO)
		_setvideomode(*mode);

	// Check for special text mode monitors.
	_getvideoconfig(&textInfo);
	*mode = textInfo.mode;
	*lines = textInfo.numtextrows;
	*columns = textInfo.numtextcols;

	_videoSegment = (*mode == 7) ? MDA_LIN_SEGMENT : ZIL_COLOR_LIN_SEGMENT;
	_videoWidth = textInfo.numtextcols * 2;

	// Hide the text cursor;
	_originalCursor = _gettextcursor();
	I_CursorRemove();
	// Turn the blink attribute off.
	inregs.w.ax = 0x1003;
	inregs.h.bl = 0x0000;

#ifdef __386__
	int386(0x10, &inregs, &outregs);
#else
	int86(0x10, &inregs, &outregs);
#endif
}

void I_ScreenClose(void)
{
	// Turn the blink attribute back on.
	union REGS inregs, outregs;
	inregs.w.ax = 0x1003;
	inregs.h.bl = 0x01;
#ifdef __386__
	int386(0x10, &inregs, &outregs);
#else
	int86(0x10, &inregs, &outregs);
#endif

	_settextcursor(_originalCursor);
	_setvideomode(_originalMode);
	_clearscreen(_GCLEARSCREEN);
}

void I_ScreenPut(int left, int top, int right, int bottom, void *_buffer)
{
	int y, x; // Warning, coordinates are 1 based.
	char *buffer = (char *)_buffer;
	for (y = top; y <= bottom; y++)
	{
		char *screen = _videoSegment + y * _videoWidth;
		for (x = left; x <= right; x++)
		{
			screen[x*2] = *buffer++;
			screen[x*2+1] = *buffer++;
		}
	}
}

void I_CursorRemove(void)
{
	_settextcursor(0x2000);
}

void I_CursorPosition(int y, int x, int val)
{
	_settextposition(y+1, x+1);
	_settextcursor((val == DC_INSERT) ? 0x0007 : 0x0707);
}

int I_MAKENAME(I_GetCodePage)(void)
{
	union REGS regs;
	regs.w.ax = 0x6601;

#ifdef __386__
	int386(0x21, &regs, &regs);
#else
	int86(0x21, &regs, &regs);
#endif
	if (regs.w.cflag)
		regs.w.bx = 437;
	return (regs.w.bx);
}

void I_MAKENAME(I_Beep)(void)
{
	sound(200);
	delay(100);
	nosound();
}

// ----- UID_MOUSE ----------------------------------------------------------

static unsigned _oldMouseState;
static unsigned _mouseState;
static int _horizontalMickeys, _verticalMickeys;
static int _iHMickey, _iVMickey;
static int _iLastHMickey, _iLastVMickey;

#define mouseInt	0x33
#define mouseInit	0
#define mouseUninit	0
#define mouseMickeys	11
#define mouseSetFunction	12
#define mouseExgFunction	20
static char _stopInterrupt = FALSE;

#pragma off (check_stack)
void _loadds far MouseISR(unsigned short max, short msi, short mdi)
{
#pragma aux MouseISR parm [EAX] [ESI] [EDI]

	// Get the mouse information.
	if (_stopInterrupt)
		return;

	_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

	// Set up new mouse state. (Current state is in max)
	if (FlagSet(max, 0x10))
	{
		_oldMouseState = 0;
		_mouseState = M_RIGHT_CHANGE;
	}
	else if (FlagSet(max, 0x08))
	{
		_oldMouseState = M_RIGHT;
		_mouseState = M_RIGHT | M_RIGHT_CHANGE;
	}
	else if (FlagSet(max, 0x04))
	{
		_oldMouseState = 0;
		_mouseState = M_LEFT_CHANGE;
	}
	else if (FlagSet(max, 0x02))
	{
		_oldMouseState = M_LEFT;
		_mouseState = M_LEFT | M_LEFT_CHANGE;
	}
	else if (FlagSet(max, 0x01))
		_mouseState = _oldMouseState;
	else
		_oldMouseState = _mouseState = 0;
#ifdef __386__
	_mouseState = _mouseState | (*((ZIL_UINT8 *)MK_FP(0x34, 0x417)) & 0xF);
#else
	_mouseState = _mouseState | (*((ZIL_UINT8 _far *)MKFP(0x40, 0x17)) & 0xF);
#endif

	// Compute the mouse movement.
	_iHMickey = msi;
	_iVMickey = mdi;

	_horizontalMickeys += (_iHMickey - _iLastHMickey);
	_verticalMickeys += (_iVMickey - _iLastVMickey);

	_iLastHMickey = _iHMickey;
	_iLastVMickey = _iVMickey;

	// Set the internal mouse information for use in the Poll routine.
	if (mouseQueueEvent)
		mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
	_stopInterrupt = FALSE;
}
#pragma on (check_stack)

int I_MouseOpen(void)
{
	union REGS regs;
	struct SREGS segregs;

	_mouseState = 0;
	_horizontalMickeys = _verticalMickeys = 0;
	_iHMickey = _iVMickey = 0;
	_iLastHMickey = _iLastVMickey = 0;

	// Check for mouse driver installed.
	regs.w.ax = 0;
#ifdef __386__
	int386(mouseInt, &regs, &regs);
#else
	int86(mouseInt, &regs, &regs);
#endif

	if (regs.w.ax != 0xFFFF)
		return FALSE;

    segread(&segregs);
	regs.w.ax = mouseSetFunction;
	regs.w.cx = 0xFF;
	regs.x.edx = FP_OFF(MouseISR);
	segregs.es = FP_SEG(MouseISR);
#ifdef __386__
	int386x(mouseInt, &regs, &regs, &segregs);
#else
	int86x(mouseInt, &regs, &regs, &segregs);
#endif
	return TRUE;
}

void I_MouseClose(void)
{
	union REGS regs;

	regs.w.ax = mouseUninit;
#ifdef __386__
	int386(mouseInt, &regs, &regs);
#else
	int86(mouseInt, &regs, &regs);
#endif
}

// ----- UID_KEYBOARD -------------------------------------------------------

static int _enhancedBios = 0;		// 0 or 0x10 (keyboard read)
static int sendBreak = 0;

void CtrlCHandler(int)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
	sendBreak = 1;
}

void I_KeyboardOpen(void)
{
	union REGS inregs, outregs;

	// Check for enhaced keyboard by calling extended shift status function.
	inregs.w.ax = 0x12FF;
#ifdef __386__
	int386(0x16, &inregs, &outregs);
#else
	int86(0x16, &inregs, &outregs);
#endif
	if (outregs.h.al != 0xFF)
	{
		_enhancedBios = 0x10;
	}
	else
	{
		// Double check for enhanced keyboard BIOS according to the
		// IBM PS/2 Technical Reference manual, page 2-103
		inregs.h.ah = 0x05;
		inregs.w.cx = 0xFFFF;
#ifdef __386__
		int386(0x16, &inregs, &outregs);
#else
		int86(0x16, &inregs, &outregs);
#endif
		if (outregs.h.al == 0x00)
		{
			// success, carry on, read that guy back ...
			inregs.h.ah = 0x10;
			for (int bufferIndex = 0; bufferIndex < 16; bufferIndex++)
			{
#ifdef __386__
				int386(0x16, &inregs, &outregs);
#else
				int86(0x16, &inregs, &outregs);
#endif
				if (outregs.w.ax == 0xFFFF)
				{
					_enhancedBios = 0x10;
					break;
				}
			}
		}
	}

	// Set break interupt to ignore Control-Break/Control-C.
	signal(SIGINT, CtrlCHandler);
//	signal(SIGBREAK, CtrlCHandler);
	signal(SIGBREAK, SIG_IGN);
}

void I_KeyboardClose(void)
{
}

void I_KeyboardRead(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	union REGS inregs, outregs;

	if (sendBreak)
	{
		*rawCode = CTRL_C;
		*shiftState = 0;
		*value = 0x3;
		sendBreak = 0;
		signal(SIGINT, CtrlCHandler);
		signal(SIGBREAK, SIG_IGN);
		return;
	}
	inregs.h.ah = _enhancedBios + 0x00;
#ifdef __386__
	int386(0x16, &inregs, &outregs);
#else
	int86(0x16, &inregs, &outregs);
#endif
	*rawCode = outregs.w.ax;
	*value = outregs.h.al;

	// Get the shift state using INT 16H, 12H(or 02H if not enhanced).
	inregs.h.ah = _enhancedBios + 0x02;
#ifdef __386__
	int386(0x16, &inregs, &outregs);
#else
	int86(0x16, &inregs, &outregs);
#endif
	*shiftState = outregs.h.al;
	if (_enhancedBios && (outregs.w.ax & 0x0800) && *value != 0)
		*shiftState &= ~0x08;
}

int I_KeyboardQuery(unsigned *shiftState)
{
	union REGS inregs, outregs;

	if (sendBreak)
		return (TRUE);
	// Test if keystroke waiting.
	if (!kbhit())
	{
		// No keystroke waiting.
		// Check for ALT key using INT 16H, 02H.
		inregs.h.ah = _enhancedBios + 0x02;
#ifdef __386__
		int386(0x16, &inregs, &outregs);
#else
		int86(0x16, &inregs, &outregs);
#endif
		*shiftState = outregs.h.al;
		return FALSE;
	}
	return TRUE;
}

// ----- UI_PRINTER --------------------------------------------------------

void I_PrintDotMatrixChar(char c)
{
	union REGS regs;
	do
	{
		regs.h.ah = 2;
#if defined(__386__)
		regs.x.edx = 0;
#else
		regs.x.dx = 0;
#endif
#if defined(__386__)
		int386(0x17,&regs,&regs);
#else
		int86(0x17,&regs,&regs);
#endif
	} while (!(regs.h.ah & 0x80));
	regs.h.ah = 0;
	regs.h.al = c;
#if defined(__386__)
	regs.x.edx = 0;
#else
	regs.x.dx = 0;
#endif
#if defined(__386__)
		int386(0x17,&regs,&regs);
#else
		int86(0x17,&regs,&regs);
#endif
}

