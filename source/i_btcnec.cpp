//	Zinc Interface Library - I_BTCNEC.CPP
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


// Warning!!!  geninterrupt() generates different code depending on
// whether the parameter is a small constant, large constant, or variable.
// This difference can cause some of this module to break, especially
// where ES and AX are used.  Change carefully.
#define ZIL_MODULE	NEC

#include <signal.h>
#include <stdlib.h>
#include <dos.h>
#include <mem.h>
#if defined(DOSX286)
#	include <phapi.h>
#endif
#if defined(DOS16M)
#	include <dos16.h>
#endif
#pragma inline
#define USE_RAW_KEYS
#include "ui_evt.hpp"

ZIL_ICHAR I_MAKENAME(_textDriverName)[] = { 'N','E','C',0 };

// ----- CheckLeadIn ----------------------------------------------------

typedef struct
{
	unsigned char lo;
	unsigned char hi;
} mbLeadIn_t;
static mbLeadIn_t *mbLeadIn = ZIL_NULLP(mbLeadIn_t);

int *I_MAKENAME(I_CheckLeadIn)(void)
{
	static int initialized = FALSE;
	if (initialized)
		return ((int *)mbLeadIn);
	initialized = TRUE;
#if defined(ZIL_UNICODE)
#	if defined(DOSX286)
		REGS16 rRegs;
		memset(&rRegs, 0, sizeof(REGS16));
		rRegs.ax = 0x6300;
		DosRealIntr(0x21, &rRegs, 0L, 0);
		unsigned short sel;
		DosMapRealSeg(rRegs.ds, rRegs.si+4*sizeof(mbLeadIn_t), &sel);
		mbLeadIn = (mbLeadIn_t *)MK_FP(sel, rRegs.si);
#	elif defined(DOS16PP)
		union REGS regs;
		struct SREGS segs;
		memset(&regs, 0, sizeof(REGS));
		regs.x.ax = 0x6507;
		int86x(0x21, &regs, &regs, &segs);
		unsigned short sel = AllocSelector(0);
		SetSelectorBase(sel, sregs.ds);
		SetSelectorLimit(sel, regs.x.si+4);
		mbLeadIn = (mbLeadIn_t *)MK_FP(sel, regs.x.si);
#	elif defined(DOS16M)
		DPMIREGS16 rRegs;
		memset(&rRegs, 0, sizeof(DPMIREGS16));
		rRegs.ax = 0x6300;
		d16SignalRMInterrupt(0x21, (LPDPMIREGS)&rRegs, (LPDPMIREGS)&rRegs);
		LPVOID sel = d16MapRealAddress((LPVOID)MK_FP(rRegs.ds, rRegs.si));
		d16SetSegLimit(FP_SEG(sel), 4*sizeof(mbLeadIn_t));
		mbLeadIn = (mbLeadIn_t *)sel;
#	elif defined(DOS32PP)
		????;
#	else
		union REGS regs;
		struct SREGS segregs;
		regs.x.ax = 0x6300;
		int86x(0x21, &regs, &regs, &segregs);
		mbLeadIn = (mbLeadIn_t *)MK_FP(segregs.ds, regs.x.si);
#	endif
#else
	// Don't check in normal mode
#endif
	return ((int *)mbLeadIn);
}

// ----- UID_DISPLAY ----------------------------------------------------------

static int _originalMode = -1;
static int _columns;
static int _lines;
static void far *_screenAddress;

static char toForegroundColor[8] =
{
//	Black Blue  Green	Red		  White
	0x01, 0x21, 0x81, 0xa1, 0x41, 0x61, 0xc1, 0xe1,
};

static char toBackgroundColor[8] =
{
//	Black Blue  Green	Red		  White
	0x00, 0x01, 0x04, 0x05, 0x02, 0x03, 0x06, 0x07,
};

struct {
	unsigned char GBON_PTN;
	unsigned char GBBCC;
	unsigned char GBDOTU;
	unsigned char GBDSP;
	unsigned char GBCPC[4];
	unsigned short GBSX1;
	unsigned short GBSY1;
	unsigned short GBLNG1;
	unsigned short GBWDPA;
	unsigned short GBRBUF[3];
	unsigned short GBSX2;
	unsigned short GBSY2;
	unsigned short GBMDOT;
	unsigned short GBCIR;
	unsigned short GBLNG2;
#define GBLPTN	GBDOT1[0]
	unsigned char GBDOT1[8];
	unsigned char GBDTYP;
	unsigned char GBFILL;
	unsigned short GBGWK1;
	unsigned short GBGWK2;
	unsigned short GBGWK3;
	unsigned short GBGWK4;
	unsigned short GBGWK5;
	unsigned short GBGWK6;
	unsigned short GBGWK7;
	unsigned short GBGWK8;
	unsigned short GBGP12;
	unsigned short GBGP34;
	unsigned short GBGP56;
	unsigned short GBGP78;
	unsigned short GBGP910;
	unsigned short GBGP1112;
	unsigned short GBGP1314;
	unsigned short GBGP1516;
}
#if defined(RESET_COLORMAP)
UCWColorMap = {		// Color map
	0, 0, 0, 0, { 0x67, 0x54, 0x32, 0x10 },
	0, 0, 0, 0, { 0, 0, 0 },
	0, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 },
	0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
},
#endif
UCWBackground = {		// Background pattern
	0, 0, 0, 0, { 0, 0, 0, 0 },
	0, 0, 16, 0, { 0, 0, 0 },
	0, 0, 0, 0, 8, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
	0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

static char screenMode;

static void setBackground(int x1, int x2, int y, unsigned char color)
{
	if (screenMode & 0x08 )
	{
		// Hireso mode is not implemented yet.
	}
	else
	{
		UCWBackground.GBON_PTN = toBackgroundColor[(color >> 4) & 0x07];	// ZIL_COLOR
		UCWBackground.GBSX1 = x1 * 8;
		UCWBackground.GBLNG2 = (x2-x1+1) * 8;
		UCWBackground.GBSY1 = y * 16;
#if defined(DOS16M)
		static _UCW *_UCWBackground = ZIL_NULLP(_UCW);
		static LPVOID lowmem;
		if (_UCWBackground == ZIL_NULLP(_UCW))
		{
			short oldStrat = d16SetMemStrat(MForceLow);
			_UCWBackground = (_UCW *)d16AllocMem(sizeof(_UCW));
			d16SetMemStrat(oldStrat);
			lowmem = d16GetRealAddress(_UCWBackground);
		}
		*_UCWBackground = UCWBackground;
		DPMIREGS regs;
		memset(&regs, 0, sizeof(regs));
		regs.eax = 0x4900;
		regs.ecx = 0xb000;
		regs.ebx = FP_OFF(lowmem);
		regs.ds = FP_SEG(lowmem);
		d16SignalRMInterrupt(0x18, &regs, &regs);
#elif defined(DOS16PP)
		????;
#elif defined(DOS32PP)
		????;
#else
		_CH = 0xb0;
		_BX = (short)&UCWBackground;
		asm	push	ds
		_AX = ((long)&UCWBackground) >> 16;
		_DS = _AX;
		_AH = 0x49;
		geninterrupt(0x18);
		asm	pop	ds
#endif
	}
}

void I_MAKENAME(I_ScreenOpen)(int *mode, int *lines, int *columns)
{
	//               CLS    kanji 25line  cur-off func-on
	static char initLine[] = "\x1b[2J\x1b)0\x1b[>3l\x1b[>5h\x1b[>1l$";
	_AH = 0x09;
	_DX = (int)&initLine[0];
	geninterrupt(0x21);

	// Save original text mode.
	if (_originalMode == -1)
	{
		_AH = 0x0B;
		geninterrupt(0x18);
		// Save original text mode.
		_originalMode = _AL;
		_columns = (_originalMode & 0x02 ? 40 : 80);
		_lines = (_originalMode & 0x01 ? 20 : 25) - 1;	// bottom line for input
	}
#if defined(DOS16M)
	char *ptr = (char *)d16MapRealAddress((char *)0x501L);
	screenMode = *ptr;
	d16FreeDesc(FP_SEG(ptr));
	_screenAddress = d16MapRealAddress((screenMode & 0x08 ? (char far *)0xE0000000L : (char far *)0xA0000000L));
#elif defined(DOSX286)
	????;
#elif defined(DOS16PP)
	????;
#elif defined(DOS32PP)
	????;
#else
	screenMode = *(char *)0x501L;
	_screenAddress = (screenMode & 0x08 ? (char far *)0xE0000000L : (char far *)0xA0000000L);
#endif

	// Initialize the text display.
	// I_CursorRemove();

	*columns = _columns;
	*lines = _lines;
	*mode = 1;	// ???? fix this, 0 = mono, 1 = color

	// _AH = 0x10;
	// _AL = 1;
	// geninterrupt(0x18);		// Turn the cursor blink off.

	if (screenMode & 0x08 )
	{
		// Hireso mode is not implement yet
	}
	else
	{
		_AH = 0x40;
		geninterrupt(0x18);		// start graphics screen

		_AH = 0x42;
		_CH = 0xc0;
		geninterrupt(0x18);		// all Banks
#if defined(RESET_COLORMAP)
		_BX = (short)&UCWColorMap;
		asm	push	ds
		_AX = ((long)&UCWColorMap) >> 16;
		_DS = _AX;
		_AH = 0x43;
		geninterrupt(0x18);		// set color palette
		asm	pop	ds
#endif
		while (!(inp(0xA0) & 0x04)); 	// wait GDC empty
		outp(0xA2, 0x46);		// Zoom Command
		outp(0xA0, 0x00);		//    ZW = 0
		outp(0x6A, 0);			// 8 color mode
		// clear PRXGCPTN
#if defined(DOS16M)
		long *tmp = (long *)d16MapRealAddress((char *)0x54eL);
		tmp[0] = 0L;
		tmp[1] = 0L;
		d16FreeDesc(FP_SEG(tmp));
#elif defined(DOSX286)
		????;
#elif defined(DOS16PP)
		????;
#elif defined(DOS32PP)
		????;
#else
		*((unsigned long far*)0x054EL) = 0L;
		*((unsigned long far*)0x0552L) = 0L;
#endif
	}
}

void I_MAKENAME(I_ScreenClose)(void)
{
	_AH = 0x41;
	geninterrupt(0x18);		// stop graphics screen

	// Clear G-VRAM
	if (screenMode & 0x08 )
	{
		memset((void far *)0xC0000000L, '\0', 0x8000);
		memset((void far *)0xC8000000L, '\0', 0x8000);
		memset((void far *)0xD0000000L, '\0', 0x8000);
		memset((void far *)0xD8000000L, '\0', 0x8000);
	}
	else
	{
		memset((void far *)0xA8000000L, '\0', 0x8000);
		memset((void far *)0xB0000000L, '\0', 0x8000);
		memset((void far *)0xB8000000L, '\0', 0x8000);
		memset((void far *)0xE0000000L, '\0', 0x8000);
	}
	//               CLS    kanji 25line  cur-on  func-on
	static char initLine[] = "\x1b[2J\x1b)0\x1b[>3l\x1b[>5l\x1b[>1l$";
	_AH = 0x09;
	_DX = (int)&initLine[0];
	geninterrupt(0x21);
}

static ZIL_UINT16 localMap[0x1c] =
{
	0x00, 0xEA, 0xEC, 0x95, 0x96, 0x98, 0x99, 0x9a,
	0x9b, 0x95, 0x96, 0x9c, 0x9d, 0x9e, 0x9f, 0x483,
	0x83, 0x87, 0xEA, 0xF0
};

void I_MAKENAME(I_ScreenPut)(int left, int top, int right, int bottom, void *buffer)
{
	ZIL_UINT8 *_buffer = (ZIL_UINT8 *)buffer;
	int i, j;

	for (i = top; i <= bottom; i++)
	{
		int l = 2 * ((i-top) * _columns);
		int min = left;
		ZIL_UINT8 color = _buffer[l+1];
		for (j = left+1; j <= right; j++)
		{
			l = 2 * ((i-top) * _columns + (j-left));
			if (_buffer[l+1] != color)
			{
				setBackground(min, j-1, i, color);
				min = j;
				color = _buffer[l+1];
			}
		}
		setBackground(min, j-1, i, color);
	}
	for (i = top; i <= bottom; i++)
	{
		int wk1 = (i * _columns) + left;
		if (((((ZIL_UINT16 *)_screenAddress)[wk1]) & 0xFF00) &&
			((((ZIL_UINT16 *)_screenAddress)[wk1]) & 0x80)) {
				((ZIL_UINT16 *)_screenAddress)[wk1-1] = 0x20;
		}

		for (j = left; j <= right; j++)
		{
			int k = ((i * _columns) + j);
			int l = 2 * ((i-top) * _columns + (j-left));
			ZIL_UINT8 c1 = _buffer[l];
			ZIL_UINT8 color = _buffer[l+1];
			if ((c1 >= 0x81 && c1 <= 0x9f) ||
			    (c1 >= 0xe0 && c1 <= 0xfc))
			{
				// Shift JIS to JIS
				unsigned char c2 = _buffer[l+2];
				int adjust = c2 < 159;
				int rowOffset = c1 < 160 ? 112 : 176;
				int cellOffset = adjust ? (c2 > 127 ? 32 : 31) : 126;
				c1 = ((c1 - rowOffset) << 1) - adjust;
				c2 -= cellOffset;

				ZIL_UINT16 iIndex = (c2 * 256) + c1 - 0x20;

				((ZIL_UINT16 *)_screenAddress)[k] = iIndex;
				((ZIL_UINT8 *)_screenAddress)[2 * k + 0x2000] = toForegroundColor[color & 0x07];
				if (j == right)
				{
					((ZIL_UINT16 *)_screenAddress)[k] = 0x20;
					break;
				}
				k++;		// Next half character

				// color = _buffer[l+3];
				((ZIL_UINT16 *)_screenAddress)[k] = iIndex | 0x80;
				((ZIL_UINT8 *)_screenAddress)[2 * k + 0x2000] = toForegroundColor[color & 0x07];
				// Skip the next character
				j++;
			}
			else
			{
				int reverse = toForegroundColor[color & 0x07];
				if (c1 < 0x1c)		// marker for special chars
				{
					reverse |= localMap[c1] >> 8;
					c1 = localMap[c1];
				}
				((ZIL_UINT16 *)_screenAddress)[k] = c1;
				((ZIL_UINT8 *)_screenAddress)[2 * k + 0x2000] = reverse;
			}
		}
		wk1 = ((i * _columns) + right);
		if (((((ZIL_UINT16 *)_screenAddress)[wk1+1]) & 0xFF00) &&
			((((ZIL_UINT16 *)_screenAddress)[wk1+1]) & 0x80)) {
				((ZIL_UINT16 *)_screenAddress)[wk1+1] = 0x20;
		}
	}
}

void I_MAKENAME(I_CursorRemove)(void)
{
	_AH = 0x12;
	geninterrupt(0x18);
}

void I_MAKENAME(I_CursorPosition)(int y, int x, int val)
{
	_DX = ((y * _columns) + x) << 1;
	_AH = 0x13;
	geninterrupt(0x18);

	_AL = val == DC_INSERT;
	_AH = 0x10;
	geninterrupt(0x18);

	// cursor add
	_AH = 0x11;
	geninterrupt(0x18);		// turn cursor on
}

int I_MAKENAME(I_GetCodePage)(void)
{
#if defined(DOSX286)
	REGS16 rRegs;
	memset(&rRegs, 0, sizeof(REGS16));

	rRegs.ax = 0x6601;
	DosRealIntr(0x21, &rRegs, 0L, 0);
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
	static char initLine[] = "\x07$";

	_AH = 0x09;
	_DX = (int)&initLine[0];
	geninterrupt(0x21);
}

// ----- UID_KEYBOARD -------------------------------------------------------

static int _oldBreakState = 0;

static int CtrlCHandler(void)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
	return 1;
}

void I_MAKENAME(I_KeyboardOpen)(void)
{
	// Set break interupt to ignore Control-Break/Control-C.
	ctrlbrk(CtrlCHandler);
	// Save previous state of Control-Break using INT 21H, 00H.
	_AX = 0x3300;
	geninterrupt(0x21);
	_oldBreakState = _DL;

	// Set Control-Break ON using INT 21H, 01H.
	_DL = 0x01;
	_AX = 0x3301;
	geninterrupt(0x21);
	I_MAKENAME(I_CheckLeadIn)();
}

void I_MAKENAME(I_KeyboardClose)(void)
{
	// Set Control-Break status to previous status.
	_DL = _oldBreakState;
	_AX = 0x3301;
	geninterrupt(0x21);
}

static unsigned setShift(void)
{
	unsigned tmp = 0;
	unsigned raw;

	// Get SHIFT/CTRL state
	_AH = 0x02;
	geninterrupt(0x18);
	raw = _AL;

	if (raw & 0x10)
		tmp |= S_CTRL;
	if (raw & 0x01)
		tmp |= S_SHIFT;
	if (raw & 0x08)
		tmp |= S_ALT;
	return tmp;
}

void I_MAKENAME(I_KeyboardRead)(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	*rawCode = Z_DONT_USE;
	unsigned tmp;
	do
	{
		_AH = 0x01;
		geninterrupt(0x18);
		tmp = _AX;
	} while (_BH != 1);
	*rawCode = tmp >> 8;
	*value = tmp & 0xff;
	*shiftState = setShift();
	if (*value == 0 || (*shiftState & S_ALT))
	{
		_AH = 0x00;
		geninterrupt(0x18);
		if (*shiftState & S_ALT)
		{
			switch (*rawCode & 0xFF)
			{
			case 0x01: *rawCode = ALT_1; return;
			case 0x02: *rawCode = ALT_2; return;
			case 0x03: *rawCode = ALT_3; return;
			case 0x04: *rawCode = ALT_4; return;
			case 0x05: *rawCode = ALT_5; return;
			case 0x06: *rawCode = ALT_6; return;
			case 0x07: *rawCode = ALT_7; return;
			case 0x08: *rawCode = ALT_8; return;
			case 0x09: *rawCode = ALT_9; return;
			case 0x0a: *rawCode = ALT_0; return;
			case 0x1d: *rawCode = ALT_A; return;
			case 0x2d: *rawCode = ALT_B; return;
			case 0x2b: *rawCode = ALT_C; return;
			case 0x1f: *rawCode = ALT_D; return;
			case 0x12: *rawCode = ALT_E; return;
			case 0x20: *rawCode = ALT_F; return;
			case 0x21: *rawCode = ALT_G; return;
			case 0x22: *rawCode = ALT_H; return;
			case 0x17: *rawCode = ALT_I; return;
			case 0x23: *rawCode = ALT_J; return;
			case 0x24: *rawCode = ALT_K; return;
			case 0x25: *rawCode = ALT_L; return;
			case 0x2f: *rawCode = ALT_M; return;
			case 0x2e: *rawCode = ALT_N; return;
			case 0x18: *rawCode = ALT_O; return;
			case 0x19: *rawCode = ALT_P; return;
			case 0x10: *rawCode = ALT_Q; return;
			case 0x13: *rawCode = ALT_R; return;
			case 0x1e: *rawCode = ALT_S; return;
			case 0x14: *rawCode = ALT_T; return;
			case 0x16: *rawCode = ALT_U; return;
			case 0x2c: *rawCode = ALT_V; return;
			case 0x11: *rawCode = ALT_W; return;
			case 0x2a: *rawCode = ALT_X; return;
			case 0x15: *rawCode = ALT_Y; return;
			case 0x29: *rawCode = ALT_Z; return;
			case 0x31: *rawCode = ALT_PERIOD; return;
			case 0x00: *rawCode = ALT_ESCAPE; return;
			case 0x34: *rawCode = ALT_SPACE; return;
			case 0x26: *rawCode = ALT_WHITE_PLUS; return;
			case 0x38: *rawCode = ALT_GRAY_INSERT; return;
			case 0x39: *rawCode = ALT_GRAY_DELETE; return;
			case 0x36: *rawCode = ALT_GRAY_PGDN; return;
			case 0x37: *rawCode = ALT_GRAY_PGUP; return;
			case 0x3a: *rawCode = ALT_GRAY_UP_ARROW; return;
			case 0x3b: *rawCode = ALT_GRAY_LEFT_ARROW; return;
			case 0x3c: *rawCode = ALT_GRAY_RIGHT_ARROW; return;
			case 0x3d: *rawCode = ALT_GRAY_DOWN_ARROW; return;
			}
return;
		}
		if (*shiftState & S_CTRL)
		{
			switch (*rawCode & 0xFF)
			{
			case 0x38: *rawCode = CTRL_WHITE_INSERT; return;
			case 0x39: *rawCode = CTRL_WHITE_DELETE; return;
			case 0x36: *rawCode = CTRL_WHITE_PGDN; return;
			case 0x37: *rawCode = CTRL_WHITE_PGUP; return;
			case 0x3a: *rawCode = CTRL_WHITE_UP_ARROW; return;
			case 0x3b: *rawCode = CTRL_WHITE_LEFT_ARROW; return;
			case 0x3c: *rawCode = CTRL_WHITE_RIGHT_ARROW; return;
			case 0x3d: *rawCode = CTRL_WHITE_DOWN_ARROW; return;
			}
		}
		switch (*rawCode & 0xFF)
		{
		case 0x62: *rawCode = F1; return;
		case 0x63: *rawCode = F2; return;
		case 0x64: *rawCode = F3; return;
		case 0x65: *rawCode = F4; return;
		case 0x66: *rawCode = F5; return;
		case 0x67: *rawCode = F6; return;
		case 0x68: *rawCode = F7; return;
		case 0x69: *rawCode = F8; return;
		case 0x6a: *rawCode = F9; return;
		case 0x6b: *rawCode = F10; return;
		case 0x38: *rawCode = WHITE_INSERT; return;
		case 0x39: *rawCode = WHITE_DELETE; return;
		case 0x36: *rawCode = WHITE_PGDN; return;
		case 0x37: *rawCode = WHITE_PGUP; return;
		case 0x3e: *rawCode = WHITE_END; return; // clr key
		case 0xae: *rawCode = WHITE_HOME; return;
		case 0x3a: *rawCode = WHITE_UP_ARROW; return;
		case 0x3b: *rawCode = WHITE_LEFT_ARROW; return;
		case 0x3c: *rawCode = WHITE_RIGHT_ARROW; return;
		case 0x3d: *rawCode = WHITE_DOWN_ARROW; return;
		case 0x82: *rawCode = SHIFT_F1; return;
		case 0x83: *rawCode = SHIFT_F2; return;
		case 0x84: *rawCode = SHIFT_F3; return;
		case 0x85: *rawCode = SHIFT_F4; return;
		case 0x86: *rawCode = SHIFT_F5; return;
		case 0x87: *rawCode = SHIFT_F6; return;
		case 0x88: *rawCode = SHIFT_F7; return;
		case 0x89: *rawCode = SHIFT_F8; return;
		case 0x8a: *rawCode = SHIFT_F9; return;
		case 0x8b: *rawCode = SHIFT_F10; return;
		case 0x92: *rawCode = CTRL_F1; return;
		case 0x93: *rawCode = CTRL_F2; return;
		case 0x94: *rawCode = CTRL_F3; return;
		case 0x95: *rawCode = CTRL_F4; return;
		case 0x96: *rawCode = CTRL_F5; return;
		case 0x97: *rawCode = CTRL_F6; return;
		case 0x98: *rawCode = CTRL_F7; return;
		case 0x99: *rawCode = CTRL_F8; return;
		case 0x9a: *rawCode = CTRL_F9; return;
		case 0x9b: *rawCode = CTRL_F10; return;
		}
	}
	_AH = 0x07;		// Do not Check CTRL-C
	geninterrupt(0x21);
	// do this to get data before the compiler trashes AX
	*value = _AL;
	if (*value == 0x1b)	// esc
		*rawCode = ESCAPE;
	if (*value == 0x09)	// tab
		*rawCode = (*shiftState & S_CTRL) ? CTRL_TAB : (*shiftState & S_SHIFT) ? BACKTAB : TAB;
	if (*value == 0x0d)	// enter
		*rawCode = (*shiftState & S_CTRL) ? CTRL_ENTER : ENTER;
	if (*value == ' ')	// space
		*rawCode = SPACE;
	if (*value == 0x08)	// backspace
		*rawCode = BACKSPACE;
#if defined(ZIL_UNICODE)
	if (mbLeadIn)
		for (int i = 0; mbLeadIn[i].lo != 0 || mbLeadIn[i].hi != 0; i++)
			if (mbLeadIn[i].lo <= *value && *value <= mbLeadIn[i].hi)
			{
				*value = (*value << 8);
				_AH = 0x07;
				geninterrupt(0x21);
				*value |= _AL;
				break;
			}
#endif
}

int I_MAKENAME(I_KeyboardQuery)(unsigned *shiftState)
{
	// cursor move to home
	// I_MAKENAME(I_CursorPosition)(0, 0, _val);

	// Test if keystroke waiting.
	_AH = 0x01;
	geninterrupt(0x18);
	if (_BH == 1)
		return TRUE;
	// No keystroke waiting.
	*shiftState = setShift();
	return FALSE;
}

// ----- UID_MOUSE ----------------------------------------------------------
#if 0
static unsigned _mouseState = 0;
static int _horizontalMickeys = 0;
static int _verticalMickeys = 0;

#define mouseInt	0x33
static int _stopInterrupt = FALSE;

static void MouseISR(void)
{
	// Get the proper value to ds.
	asm push ds
	asm mov dx, DGROUP
	asm mov ds, dx

#if defined(DOSX286)
	REGS16 regs;
#else
	union REGS regs;
#endif
	
	// Get the mouse information.
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.
		unsigned short val = _BX;
		unsigned _newMouseState;

		_newMouseState = 0;
		if (val & 0x00ff)
			_newMouseState |= M_LEFT;
		if (val & 0xff00)
			_newMouseState |= M_RIGHT;
		if ((_newMouseState ^ _mouseState) & M_LEFT)
			_newMouseState |= M_LEFT_CHANGE;
		if ((_newMouseState ^ _mouseState) & M_RIGHT)
			_newMouseState |= M_RIGHT_CHANGE;
		_mouseState = _newMouseState | setShift();

		// Compute the mouse movement.
#if defined(DOSX286)
		// We can't call int86(0x33,,) here; we blow up in the extender.
		// Instead, we simply issue the interrupt in real mode and
		// shuttle the results back up to protected mode.

		memset(&regs, 0, sizeof(REGS16));
		regs.ax = 0x0b;			// mouseMickeys
		DosRealIntr(mouseInt, &regs, 0L, 0);
		_horizontalMickeys += regs.cx;
		_verticalMickeys += regs.dx;
#else
		regs.x.ax = 0x0b;		// mouseMickeys
		int86(mouseInt, &regs, &regs);
		_horizontalMickeys += regs.x.cx;
		_verticalMickeys += regs.x.dx;
#endif
		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		_stopInterrupt = FALSE;
	}

	// Restore the ds register.
	asm pop ds
}

int I_MAKENAME(I_MouseOpen)(void)
{
	_AX = 0;
	geninterrupt(mouseInt);
	if (_AX != 0xFFFF)
		return FALSE;

	_CX = 0x1F;
	_DX = FP_OFF(MouseISR);
	_AX = FP_SEG(MouseISR);
	_ES = _AX;
	_AX = 0x0c;
	geninterrupt(mouseInt);

	return TRUE;
}

void I_MAKENAME(I_MouseClose)(void)
{
	_AX = 0;
	geninterrupt(mouseInt);
}

#else				// based on AT Version

static unsigned char _oldMouseState = 0;
static unsigned _mouseState = 0;
static int _iHMickey = 0, _iVMickey = 0;
static int _iLastHMickey = 0, _iLastVMickey = 0;
static int _horizontalMickeys = 0, _verticalMickeys = 0;

#define mouseInt	0x33
#define mouseSetFunction	0x0c
#define mouseExgFunction	0x14
static int _stopInterrupt = FALSE;

#if !defined(DOSX286)
static unsigned _oldMouseMask = 0;
static unsigned _oldMouseOffset = 0;
static unsigned _oldMouseSegment = 0;
#endif

static void MouseISR(void)
{
	// Get the proper value to ds.
	asm push ds
	asm mov ax, DGROUP
	asm mov ds, ax
	
	// Get the mouse information.
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

		// Set up new mouse state. (Current state is in _BL)
//		_mouseState = ((((_oldMouseState ^ _BL) << 4) | _BL) << 8) | (*((uint8 _far *)MK_FP(0x40, 0x17)) & 0xF);
		_mouseState = ((((_oldMouseState ^ _BL) << 4) | _BL) << 8);

		_oldMouseState = _BL;
		asm mov _iHMickey, si
		asm mov _iVMickey, di

		_mouseState |= setShift();

		_horizontalMickeys += _iHMickey - _iLastHMickey;
		_verticalMickeys += _iVMickey - _iLastVMickey;

		_iLastHMickey = _iHMickey;
		_iLastVMickey = _iVMickey;

		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		_stopInterrupt = FALSE;
	}

	// Restore the ds register.
	asm pop ds
}

int I_MAKENAME(I_MouseOpen)(void)
{
#if !defined(DOSX286)
	unsigned char far *far *mouseVector = (unsigned char far *far *) (4 * mouseInt);
	if (*mouseVector && **mouseVector != 0xCF)
	{
		_AX = mouseExgFunction;
		_BX = 0;
		_DX = 0;
		_CX = 0;
		geninterrupt(mouseInt);
		_BX = _ES;
		_oldMouseMask = _CX;
		_oldMouseOffset = _DX;
		_oldMouseSegment = _BX;
	}
#endif
	_AX = 0;
	geninterrupt(mouseInt);
	if (_AX != 0xFFFF)
		return FALSE;
	_CX = 0xFF;
	_DX = FP_OFF(MouseISR);
	_AX = FP_SEG(MouseISR);
	_ES = _AX;
	_AX = mouseSetFunction;
	geninterrupt(mouseInt);
	return TRUE;
}

void I_MAKENAME(I_MouseClose)(void)
{
#if !defined(DOSX286)
	_CX = _oldMouseMask;
	_DX = _oldMouseOffset;
	_BX = _oldMouseSegment;
	_ES = _BX;
	_AX = mouseExgFunction;
	geninterrupt(mouseInt);
#endif
	_AX = 0;
	geninterrupt(mouseInt);
}
#endif

// ----- UI_PRINTER --------------------------------------------------------

void I_MAKENAME(I_PrintDotMatrixChar)(char c)
{
		// output data
		asm mov ah, 17;//11 Hex

		// JIS 8 bit code
		asm mov al, c;
		asm int 26 //1A Hex
}

MAKE_SETFUNCTIONS
