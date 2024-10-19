//	Zinc Interface Library - I_ZTCAT.CPP
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


#define ZIL_MODULE	AT

#include <signal.h>
#include <dos.h>
#include <stdlib.h>
#include <controlc.h>
#include <disp.h>
#include <sound.h>
#ifdef DOSX286
#include <phapi.h>
#endif
#ifdef DOS16M
#include <dos16.h>
#endif
#define USE_RAW_KEYS
#include "ui_evt.hpp"

#if defined(DOS386)
extern "C" void *_x386_zero_base_ptr;
#endif

#if !defined(ZIL_HARDWARE)
	ZIL_ICHAR _textDriverName[] = { 'A','T',0 };
#endif

int *I_MAKENAME(I_CheckLeadIn)(void)
{
	return (ZIL_NULLP(int));
}
// ----- UI_TEXT_DISPLAY ----------------------------------------------------

static int _originalMode = -1;
static void *_screenAddress = ZIL_NULLP(void);
#if defined(DOSX286) || defined(DOS16M)
static void *_realScreenAddress = ZIL_NULLP(void);
#endif
static int _columns = 0;
static int _lines = 0;

#if !defined(DOS386)
#define OutPortByte outp
#endif

void I_MAKENAME(I_ScreenOpen)(int *mode, int *lines, int *columns)
{
	union REGS regs;

	// Save original text mode.
	if (_originalMode == -1)
	{
		regs.h.ah = 0x0f;
		int86(0x10, &regs, &regs);
		_originalMode = regs.h.al;
	}

	// Initialize the text display.
	if (*mode != TDM_AUTO)
	{
		// Set the mode
		if (*mode != 64)
			disp_reset43();
		if (*mode == 64)
			disp_set43();
		else
			disp_setmode(*mode);
		// Check for special text mode monitors.
		regs.h.ah = 0x0f;
		int86(0x10, &regs, &regs);
		*mode = regs.h.al;
	}
	else
		*mode = _originalMode;

#if defined(DOSX286)
	_lines = *lines = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x84)) + 1;
	_columns = *columns = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x4a));

	// Alternate screen buffer
	REGS16 rRegs;
	memset(&rRegs, 0, sizeof(REGS16));
	rRegs.ax = 0xfe00;
	rRegs.di = 0x0000;
	rRegs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
	DosRealIntr(0x10, &rRegs, 0L, 0);
	_realScreenAddress = MK_FP(rRegs.es, rRegs.di);
	unsigned short sel;
	DosMapRealSeg(rRegs.es, (long)_lines * _columns * 2, &sel);
	_screenAddress = MK_FP(sel, rRegs.di);
#elif defined(DOS16M)
	_lines = *lines = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x84)) + 1;
	_columns = *columns = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x4a));

	DPMIREGS16 rRegs;
	memset(&rRegs, 0, sizeof(DPMIREGS16));
	rRegs.ax = 0xfe00;
	rRegs.di = 0x0000;
	rRegs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
	d16SignalRMInterrupt(0x10,(LPDPMIREGS)&rRegs,(LPDPMIREGS) &rRegs);
	_realScreenAddress = MK_FP(rRegs.es, rRegs.di);
	_screenAddress = d16MapRealAddress(_realScreenAddress);
	d16SetSegLimit(FP_SEG(_screenAddress),(long)_lines * _columns * 2);
#elif defined(DOS386)
	_lines = *lines = *((ZIL_UINT8 *) _x386_zero_base_ptr + 0x484L) + 1;
	_columns = *columns = *((ZIL_UINT8 *) _x386_zero_base_ptr + 0x44aL);
	_screenAddress = _x386_map_physical_address((void *)(*mode == 0x07 ? 0xb0000 : 0xb8000), 2 * _lines * _columns);
#else
	_lines = *lines = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x84)) + 1;
	_columns = *columns = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x4a));

	if (_lines <= 1)
		_lines = *lines = 25;   // BUG fix for XT machines

	// Alternate screen buffer
	struct SREGS segs;
	regs.h.ah = 0xfe;
	regs.x.di = 0x0000;
	segs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
	int86x(0x10, &regs, &regs, &segs);
	_screenAddress = MK_FP(segs.es, regs.x.di);
#endif
	I_CursorRemove();

	// Turn the blink attribute off.
	regs.x.ax = 0x1003;
	regs.h.bl = 0x00;
	int86(0x10, &regs, &regs);

#if !defined(DOS386)
	ZIL_UINT8 newSetting = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x65));
	newSetting &= ~0x20;
	ZIL_UINT16 addr = *((ZIL_UINT16 _far *)MK_FP(0x40, 0x63)) + 4;
	OutPortByte(addr, newSetting);
	// Update BIOS data area
	*((ZIL_UINT8 _far *)MK_FP(0x40, 0x65)) = newSetting;
#endif
}

void I_MAKENAME(I_ScreenClose)(void)
{
	union REGS regs;

	// Turn the blink attribute back on.
	regs.x.ax = 0x1003;
	regs.h.bl = 0x01;
	int86(0x10, &regs, &regs);

	regs.h.ah = 0x00;
	regs.h.al = _originalMode;
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x01;
	regs.x.cx = (_originalMode == 0x07 ? 0x0b0c : 0x0607);
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x06;
	regs.h.al = 0x00;
	regs.h.bh = (_originalMode == 0x07 ? 0x0b0c : 0x0607);
	regs.x.cx = 0x0000;
	regs.x.dx = (_originalMode == 0x07 ? 0x1850 : 0x1850);
	int86(0x10, &regs, &regs);
}

void I_MAKENAME(I_ScreenPut)(int left, int top, int right, int bottom, void *buffer)
{
	int lineLen = 2*(right-left+1);
	for (int i=top, j=0; i <= bottom; i++, j++)
	{
		memcpy((ZIL_INT16 *)_screenAddress + i * _columns + left,
		       (ZIL_INT16 *)buffer + j * _columns, lineLen);
#if !defined(DOS386)
		struct SREGS sregs;
		union REGS regs;

		regs.h.ah = 0xFF;
		ZIL_INT16 *tmp = (ZIL_INT16 *)_screenAddress + i * _columns + left;
		sregs.es  = FP_SEG(tmp);
		regs.x.di  = FP_OFF(tmp);
		regs.x.cx = lineLen;
		int86x(0x10, &regs, &regs, &sregs);
#endif
	}
}

void I_MAKENAME(I_CursorRemove)(void)
{
	union REGS regs;

	regs.h.ah = 0x02;
	regs.h.bh = 0;
	regs.h.dh = _lines;
	regs.h.dl = 0;
	int86(0x10, &regs, &regs);
}

void I_MAKENAME(I_CursorPosition)(int y, int x, int doInsert)
{
	union REGS regs;

	regs.h.ah = 0x02;
	regs.h.bh = 0;
	regs.h.dl = x;
	regs.h.dh = y;
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x01;
	regs.x.cx = (doInsert == DC_INSERT ?
		       (_originalMode == 0x07 ? 0x000c : 0x0007) :
		       (_originalMode == 0x07 ? 0x0b0c : 0x0607));
	int86(0x10, &regs, &regs);
}

int I_MAKENAME(I_GetCodePage)(void)
{
#if defined(DOSX286)
	REGS16 rRegs;
	memset(&rRegs, 0, sizeof(REGS16));
	rRegs.ax = 0x6601;
#	if defined(DOSX286)
	DosRealIntr(0x21, &rRegs, 0L, 0);
#	elif defined(DOS16M)
	d16SignalRMInterrupt(0x21, &rRegs, &rRegs);
#	endif
	if (rRegs.flags & 0x0001)
		rRegs.bx = 437;
	return (rRegs.bx);
#else
	union REGS regs;
	regs.x.ax = 0x6601;
	int86(0x21, &regs, &regs);
	if (regs.x.cflag)
		regs.x.bx = 437;
	return (regs.x.bx);
#endif
}

void I_MAKENAME(I_Beep)(void)
{
	sound_beep(1331);
}

// ----- UID_MOUSE ----------------------------------------------------------

static unsigned char _oldMouseState;
static unsigned _mouseState;
static int _horizontalMickeys, _verticalMickeys;
static int _stopInterrupt;
static int xCounter, yCounter;

#include <msmouse.h>

static char _mouseStack[1024];

void _cdecl MouseISR(unsigned, unsigned state, unsigned x, unsigned y)
{
	// Get the mouse information.
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

		// Set up new mouse state. (Current state is in state)
#if defined(DOS386)
		_mouseState = ((((_oldMouseState ^ state) << 4) | state) << 8) |
			(*((ZIL_UINT8 *) _x386_zero_base_ptr + 0x417L) & 0xF);
#else
		_mouseState = ((((_oldMouseState ^ state) << 4) | state) << 8) | (*((ZIL_UINT8 far *)MK_FP(0x40, 0x17)) & 0xF);
#endif
		_oldMouseState = state;

		// Compute the mouse movement.
		msm_readcounters(&xCounter, &yCounter);
		_horizontalMickeys += xCounter;
		_verticalMickeys += yCounter;

		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		_stopInterrupt = FALSE;
	}
}

int I_MAKENAME(I_MouseOpen)(void)
{
	_horizontalMickeys = _verticalMickeys = 0;

#if defined (__SC__) && defined(DOS386)
	if (msm_init() != -1)
#else
	if (msm_init() != 0xFFFF)
#endif
		return FALSE;
	msm_signal(0xFF, MouseISR, &_mouseStack[1024]);
	return TRUE;
}

void I_MAKENAME(I_MouseClose)(void)
{
	msm_term();
}

// ----- UID_KEYBOARD -------------------------------------------------------

static int _enhancedBios = 0;		// 0 or 0x10 (keyboard read)
static int _oldBreakState = 0;
static int sendBreak = 0;

#if __ZTC__ >= 0x310
static void _cdecl CtrlCHandler(void)
#else
static void _far _cdecl CtrlCHandler(void)
#endif
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		;
	sendBreak = 1;
}

void I_MAKENAME(I_KeyboardOpen)(void)
{
	union REGS inregs, outregs;

	// Check for enhaced keyboard by calling extended shift status function.
	inregs.x.ax = 0x12FF;
	int86(0x16, &inregs, &outregs);
	if (outregs.h.al != 0xFF)
	{
		_enhancedBios = 0x10;
	}
	else
	{
		// Double check for enhanced keyboard BIOS according to the
		// IBM PS/2 Technical Reference manual, page 2-103
		inregs.h.ah = 0x05;
		inregs.x.cx = 0xFFFF;
		int86(0x16, &inregs, &outregs);
		if (outregs.h.al == 0x00)
		{
			// success, carry on, read that guy back ...
			inregs.h.ah = 0x10;
			for (int bufferIndex = 0; bufferIndex < 16; bufferIndex++)
			{
				int86(0x16, &inregs, &outregs);
				if (outregs.x.ax == 0xFFFF)
				{
					_enhancedBios = 0x10;
					break;
				}
			}
		}
	}
	// Set break interupt to ignore Control-Break/Control-C.
	_controlc_handler = CtrlCHandler;
	controlc_open();
	// Save previous state of Control-Break using INT 21H, 00H.
	inregs.x.ax = 0x3300;
	int86(0x21, &inregs, &outregs);
	_oldBreakState = outregs.h.dl;

	// Set Control-Break OFF using INT 21H, 01H.
	inregs.x.ax = 0x3301;
	inregs.h.dl = 0x00;
	int86(0x21, &inregs, &outregs);
}

void I_MAKENAME(I_KeyboardClose)(void)
{
	union REGS inregs, outregs;

	// Set Control-Break status to previous status.
	inregs.x.ax = 0x3301;
	inregs.h.dl = _oldBreakState;
	int86(0x21, &inregs, &outregs);
	controlc_close();
}

void I_MAKENAME(I_KeyboardRead)(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	union REGS inregs, outregs;

	if (sendBreak)
	{
		*rawCode = CTRL_C;
		*shiftState = 0;
		*value = 0x3;
		sendBreak = 0;
		return;
	}
	inregs.h.ah = _enhancedBios + 0x00;
	int86(0x16, &inregs, &outregs);
	*rawCode = outregs.x.ax;
	*value = outregs.h.al;

	// Get the shift state using INT 16H, 12H(or 02H if not enhanced).
	inregs.h.ah = _enhancedBios + 0x02;
	int86(0x16, &inregs, &outregs);
	*shiftState = outregs.h.al;
	if (_enhancedBios && (outregs.x.ax & 0x0800) && *value != 0)
		*shiftState &= ~0x08;
}

int I_MAKENAME(I_KeyboardQuery)(unsigned *shiftState)
{
	union REGS inregs, outregs;

	if (sendBreak)
		return (TRUE);
	// Test if keystroke waiting.
	inregs.h.ah = _enhancedBios + 0x01;
	int86(0x16, &inregs, &outregs);
	if (outregs.x.flags & 0x40)
	{
		// No keystroke waiting.
		// Check for ALT key using INT 16H, 02H.
		inregs.h.ah = _enhancedBios + 0x02;
		int86(0x16, &inregs, &outregs);
		*shiftState = outregs.h.al;
		return FALSE;
	}
	return TRUE;
}
// ----- UI_PRINTER --------------------------------------------------------

void I_MAKENAME(I_PrintDotMatrixChar)(char c)
{
	union REGS regs;
	do
	{
		regs.h.ah = 2;
		regs.x.dx = 0;
		int86(0x17,&regs,&regs);
	} while (!(regs.h.ah & 0x80));
	regs.h.ah = 0;
	regs.h.al = c;
	regs.x.dx = 0;
	int86(0x17,&regs,&regs);
}

MAKE_SETFUNCTIONS
