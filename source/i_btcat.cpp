//	Zinc Interface Library - I_BTCAT.CPP
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
#define ZIL_MODULE	AT

#include <signal.h>
#include <dos.h>
#include <stdlib.h>
#include <mem.h>
#include <conio.h>
#if defined(DOSX286)
#	include <phapi.h>
#elif defined(DOS16M)
#	include <dos16.h>
#endif
#define USE_RAW_KEYS
#include "ui_evt.hpp"
// First zero is place holder for differences between
// IBM and MS code page 932 (Japanese).
ZIL_ICHAR I_MAKENAME(_textDriverName)[] = { 'A','T',0,0 };

#if defined(DOS32PP)
#	define WORD	w
#else
#	define WORD	x
#endif

static int _enhancedBios = 0;		// 0 or 0x10 (keyboard read)

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
		regs.WORD.ax = 0x6507;
		int86x(0x21, &regs, &regs, &segs);
		unsigned short sel = AllocSelector(0);
		SetSelectorBase(sel, sregs.ds);
		SetSelectorLimit(sel, regs.WORD.si+4*sizeof(mbLeadIn_t));
		mbLeadIn = (mbLeadIn_t *)MK_FP(sel, regs.WORD.si);
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
		regs.WORD.ax = 0x6300;
		int86x(0x21, &regs, &regs, &segregs);
		mbLeadIn = (mbLeadIn_t *)MK_FP(segregs.ds, regs.WORD.si);
#	endif
#else
	// Don't check in normal mode
#endif
	return ((int *)mbLeadIn);
}

// ----- UI_TEXT_DISPLAY ----------------------------------------------------

static int _originalMode = -1;
static int _mode = 0;
static void *_screenAddress = ZIL_NULLP(void);
#if defined(ZIL_DOSEXTENDED)
	static void *_realScreenAddress = ZIL_NULLP(void);
#endif
static int _columns = 0;
static int _lines = 0;
#if !defined(__Seg0040)
	const ZIL_UINT16 __Seg0040 = 0x0040;
#endif
#if defined(DOS32PP)
#	define int86(a, b, c)	int386(a, b, c)
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
		textmode(*mode);
		// Check for special text mode monitors.
		regs.h.ah = 0x0f;
		int86(0x10, &regs, &regs);
		_mode = regs.h.al;
	}
	else
		_mode = _originalMode;
	*mode = _mode;
#if defined(DOS32PP)
	struct text_info r;
	gettextinfo(&r);
	_lines = *lines = r.screenheight;
	_columns = *columns = r.screenwidth;
#else
	_lines = *lines = *((ZIL_UINT8 _far *)MK_FP(__Seg0040, 0x84)) + 1;
	_columns = *columns = *((ZIL_UINT8 _far *)MK_FP(__Seg0040, 0x4a));
#endif
	if (_lines <= 1)	// Fix for 8088/CGA machines
		_lines = *lines = 25;

	// Alternate screen buffer
#if defined(DOSX286)
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
	DPMIREGS16 rRegs;
	memset(&rRegs, 0, sizeof(DPMIREGS16));
	rRegs.ax = 0xfe00;
	rRegs.di = 0x0000;
	rRegs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
	d16SignalRMInterrupt(0x10,(LPDPMIREGS)&rRegs,(LPDPMIREGS) &rRegs);
	_realScreenAddress = MK_FP(rRegs.es, rRegs.di);
	_screenAddress = d16MapRealAddress(_realScreenAddress);
	d16SetSegLimit(FP_SEG(_screenAddress),(long)_lines * _columns * 2);
#elif defined(DOS16PP)
{
#	if 0
		static struct
		{
			long di;
			long si;
			long bp;
			long reserved1;
			long bx;
			long dx;
			long cx;
			long ax;
			short cpu_status;
			short es;
			short ds;
			short fs;
			short gs;
			short ip;
			short cs;
			short sp;
			short ss;
		} regs;
		regs.ax = 0xfe00;
		regs.di = 0x0000;
		regs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
		_BX = 0x0010;
		_CX = 0;
		asm mov ax, DGROUP
		asm mov es, ax
		_DI = FP_OFF(&regs);
		_AX = 0x300;
		geninterrupt(0x31);
		_realScreenAddress = MK_FP(regs.es, regs.di);
		unsigned short sel = AllocSelector(0);
		SetSelectorBase(sel, regs.es);
		SetSelectorLimit(sel, regs.di + (long)_lines * _columns * 2);
		_screenAddress = MK_FP(sel, regs.di);
#	else
		unsigned short sel = (*mode == 0x07 ? __SegB000 : __SegB800);
		_realScreenAddress = MK_FP(sel, 0x0000);
		_screenAddress = MK_FP(sel, 0x0000);
#	endif
}
#elif defined(DOS32PP)
#	if 0
		????;
#	endif
#else
	struct SREGS segs;
	regs.h.ah = 0xfe;
	regs.WORD.di = 0x0000;
	segs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
	int86x(0x10, &regs, &regs, &segs);
	_screenAddress = MK_FP(segs.es, regs.WORD.di);
#endif
	I_CursorRemove();

	// Turn the blink attribute off.
	regs.WORD.ax = 0x1003;
	regs.h.bl = 0x00;
	int86(0x10, &regs, &regs);

#if !defined(DOS32PP)
	ZIL_UINT8 newSetting = *((ZIL_UINT8 _far *)MK_FP(__Seg0040, 0x65));
	newSetting &= ~0x20;
	ZIL_UINT16 addr = *((ZIL_UINT16 _far *)MK_FP(__Seg0040, 0x63)) + 4;
	outportb(addr, newSetting);
	// Update BIOS data area
	*((ZIL_UINT8 _far *)MK_FP(__Seg0040, 0x65)) = newSetting;
#endif
}

void I_MAKENAME(I_ScreenClose)(void)
{
	union REGS regs;

	// Turn the blink attribute back on.
	regs.WORD.ax = 0x1003;
	regs.h.bl = 0x01;
	int86(0x10, &regs, &regs);

	regs.h.ah = 0x00;
	regs.h.al = _originalMode;
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x01;
	regs.WORD.cx = (_originalMode == 0x07 ? 0x0b0c : 0x0607);
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x06;
	regs.h.al = 0x00;
	regs.h.bh = (_originalMode == 0x07 ? 0x0b0c : 0x0607);
	regs.WORD.cx = 0x0000;
	regs.WORD.dx = (_originalMode == 0x07 ? 0x1850 : 0x1850);
	int86(0x10, &regs, &regs);
}

void I_MAKENAME(I_ScreenPut)(int left, int top, int right, int bottom, void *buffer)
{
	int lineLen = 2*(right-left+1);
	for (int i=top, j=0; i <= bottom; i++, j++)
	{
#if defined(DOSX286)
		memcpy((short *)_screenAddress + i * _columns + left,
		       (short *)buffer + j * _columns, lineLen);
		REGS16 regs;
		memset(&regs, 0, sizeof(REGS16));
		regs.ax = 0xFF00;
		short *tmp = (short *)_realScreenAddress + i * _columns + left;
		regs.es  = FP_SEG(tmp);
		regs.di  = FP_OFF(tmp);
		regs.cx = lineLen;
		DosRealIntr(0x10, &regs, 0L, 0);
#elif defined(DOS16M)
		memcpy((short *)_screenAddress + i * _columns + left,
		       (short *)buffer + j * _columns, lineLen);
		DPMIREGS regs;
		memset(&regs, 0, sizeof(DPMIREGS));
		regs.eax = 0xFF00;
		short *tmp = (short *)_realScreenAddress + i * _columns + left;
		regs.es  = FP_SEG(tmp);
		regs.edi  = FP_OFF(tmp);
		regs.ecx = lineLen;
		d16SignalRMInterrupt(0x10, &regs, &regs);
#elif defined(DOS16PP)
		memcpy((short *)_screenAddress + i * _columns + left,
		       (short *)buffer + j * _columns, lineLen);
#	if 0
		static struct
		{
			long di;
			long si;
			long bp;
			long reserved1;
			long bx;
			long dx;
			long cx;
			long ax;
			short cpu_status;
			short es;
			short ds;
			short fs;
			short gs;
			short ip;
			short cs;
			short sp;
			short ss;
		} regs;
		regs.ax = 0xff00;
		short *tmp = (short *)_realScreenAddress + i * _columns + left;
		regs.es  = FP_SEG(tmp);
		regs.di  = FP_OFF(tmp);
		regs.cx = lineLen;
		_BX = 0x0010;
		_CX = 0;
		asm mov ax, DGROUP
		asm mov es, ax
		_DI = FP_OFF(&regs);
		_AX = 0x300;
		geninterrupt(0x31);
#	endif
#elif defined(DOS32PP)
#	if 0
		????;
#	else
		puttext(left + 1, i + 1, right + 1, i + 1, (short *)buffer + j * _columns);
		
#	endif
#else
		memcpy((short *)_screenAddress + i * _columns + left,
		       (short *)buffer + j * _columns, lineLen);
		struct SREGS sregs;
		union REGS regs;
		regs.h.ah = 0xFF;
		short *tmp = (short *)_screenAddress + i * _columns + left;
		sregs.es  = FP_SEG(tmp);
		regs.WORD.di  = FP_OFF(tmp);
		regs.WORD.cx = lineLen;
		int86x(0x10, &regs, &regs, &sregs);
#endif
	}
}

void I_MAKENAME(I_CursorRemove)(void)
{
	union REGS regs;

	regs.h.ah = 0x02;
	regs.h.bh = 0;
	regs.WORD.dx = 0;
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x01;
	regs.h.al = _mode;	// for a bug on some bios's
	regs.WORD.cx = (_mode == 0x07 ? 0x0b0c : 0x0607) | 0x2000;
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
	regs.WORD.cx = (doInsert == DC_INSERT ?
		       (_mode == 0x07 ? 0x000c : 0x0007) :
		       (_mode == 0x07 ? 0x0b0c : 0x0607));
	int86(0x10, &regs, &regs);
}

int I_MAKENAME(I_GetCodePage)(void)
{
#if defined(DOSX286)
	REGS16 rRegs;
	memset(&rRegs, 0, sizeof(REGS16));

	rRegs.ax = 0x6601;
	DosRealIntr(0x21, &rRegs, 0L, 0);
	if (rRegs.flags & 0x0001)
		return (437);
	if (rRegs.bx == 932)
	{
		rRegs.ax = 0x3000;
		DosRealIntr(0x21, &rRegs, 0L, 0);
		I_MAKENAME(_textDriverName)[2] = ((rRegs.bx | rRegs.cx) == 0 ? 'i' : 'm');
		return (932);
	}
	return (rRegs.bx);
#else	// defined(DOS16PP) || defined(DOS16M) || defined(DOS32PP) || realmode
	union REGS regs;
	regs.WORD.ax = 0x6601;
	int86(0x21, &regs, &regs);
	if (regs.WORD.cflag)
		return (437);
	if (regs.WORD.bx == 932)
	{
		regs.WORD.ax = 0x3000;
		int86(0x21, &regs, &regs);
		I_MAKENAME(_textDriverName)[2] = ((regs.WORD.bx | regs.WORD.cx) == 0 ? 'i' : 'm');
		return (932);
	}
	return (regs.WORD.bx);
#endif
}

void I_MAKENAME(I_Beep)(void)
{
	sound(200);
	delay(100);
	nosound();
}

// ----- UID_MOUSE ----------------------------------------------------------

#define mouseInt	0x33
#define mouseSetFunction	0x0c
#define mouseExgFunction	0x14
static int _stopInterrupt = FALSE;
static unsigned char _oldMouseState = 0;
static unsigned _mouseState;
static short _iHMickey, _iVMickey;
static short _iLastHMickey, _iLastVMickey;
static int _horizontalMickeys, _verticalMickeys;

#if defined(DOSX286)
	PIHANDLER dpmi_0x33_vec;
	PIHANDLER phar_0x33_vec;
	extern char _isOS2;
#elif !defined(ZIL_DOSEXTENDED)
	static unsigned _oldMouseMask = 0;
	static unsigned _oldMouseOffset = 0;
	static unsigned _oldMouseSegment = 0;
#endif

#if defined(DOS32PP)
void MouseRead()
{
	static short iState;

	// Get mouse state.
/* START BLOCK COMMENT
	asm mov ax, 3
	asm int mouseInt
	asm mov iState,bx
END BLOCK COMMENT */

	_AX = 3;
	geninterrupt(mouseInt);
	iState = _BX;

	// Get mouse mickeys.
/* START BLOCK COMMENT
	asm mov ax, 11
	asm int mouseInt
	asm mov _iHMickey, cx
	asm mov _iVMickey, dx
END BLOCK COMMENT */

	_AX = 11;
	geninterrupt(mouseInt);
	_iHMickey = _CX;
	_iVMickey = _DX;

	// Set up new mouse state. (Current state is in state)
	_mouseState = (((_oldMouseState ^ iState) << 4) | iState) << 8;
	_oldMouseState = iState;

	_horizontalMickeys = _iHMickey;
	_verticalMickeys = _iVMickey;

	// Set the internal mouse information for use in the Poll routine.
	if (mouseQueueEvent)
		mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
}
#endif

static void MouseISR(void)
{
#if defined(DOS32PP)
/* START BLOCK COMMENT
	asm push ds
	asm push cs
	asm pop  ds
END BLOCK COMMENT */

	short saveDS = _DS;
	_DS = _CS;

	static union REGS regs;
	// Get the mouse information.
#if 0
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

		// Set up new mouse state. (Current state is in _BL)
		regs.w.ax = 0x0003;
		int86(mouseInt, &regs, &regs);
		_mouseState = ((((_oldMouseState ^ regs.h.bl) << 4) | regs.h.bl) << 8);
		_oldMouseState = regs.h.bl;
		_iHMickey = regs.w.si;
		_iVMickey = regs.w.di;

		regs.h.ah = _enhancedBios + 0x01;
		int86(0x16, &regs, &regs);
		_mouseState |= (regs.x.flags >> 4);	// ????

		_horizontalMickeys += _iHMickey - _iLastHMickey;
		_verticalMickeys += _iVMickey - _iLastVMickey;

		_iLastHMickey = _iHMickey;
		_iLastVMickey = _iVMickey;

		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		_stopInterrupt = FALSE;
	}
#endif
/* START BLOCK COMMENT
	asm	pop	ds
END BLOCK COMMENT */

	_DS = saveDS;
#else
	// Get the proper value to ds.
	asm push ds
	asm push ax
	asm mov ax, SEG _stopInterrupt
	asm mov ds, ax
	asm pop ax

	// Get the mouse information.
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

		// Set up new mouse state. (Current state is in _BL)
		_mouseState = ((((_oldMouseState ^ _BL) << 4) | _BL) << 8) | (*((ZIL_UINT8 _far *)MK_FP(__Seg0040, 0x17)) & 0xF);
		_oldMouseState = _BL;
		asm mov _iHMickey, si
		asm mov _iVMickey, di

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
#endif
}

int I_MAKENAME(I_MouseOpen)(void)
{
#if !defined(ZIL_DOSEXTENDED)
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
	union REGS regs;
	regs.WORD.ax = 0;
#if defined(DOS32PP) || defined(DOS386)
	int386(mouseInt, &regs, &regs);
#	else
	int86(mouseInt, &regs, &regs);
#	endif
	if (regs.WORD.ax != 0xFFFF)
		return FALSE;

#if defined (DOSX286)
	// Check for running under Windows 3.x enhanced mode.
	int isWindows = 0;
	regs.WORD.ax = 0x1600;
	int86(0x2f,&regs,&regs);
	isWindows = (regs.h.al>1 && regs.h.al!=0x80) ? 1 : 0;
	// Check for running under OS2 2.x
	regs.WORD.ax = 0x3306;
	int86(0x21,&regs,&regs);
	_isOS2 = regs.h.bl >= 20;

	if ((isWindows || _isOS2) && DosGetSavedProtVec(0x33,&dpmi_0x33_vec)==0)
		DosSetProtVec(0x33,dpmi_0x33_vec,&phar_0x33_vec);
#endif
	_mouseState = 0;
	_iHMickey = _iVMickey = 0;
	_iLastHMickey = _iLastVMickey = 0;
	_horizontalMickeys = _verticalMickeys = 0;
#if defined(DOS32PP)
/* START BLOCK COMMENT
	asm push es
	asm mov eax, 0Ch
	asm mov ecx, 31
	asm push cs
	asm pop es
	asm mov edx, offset MouseISR
	asm int 33h
	asm pop es
END BLOCK COMMENT */
#elif defined (DOS16M)
	LPVOID fpMouse = (LPVOID)MouseISR;
	_CX = 0xFF;
	_DX = FP_OFF(fpMouse);
	_AX = FP_SEG(fpMouse);
	_ES = _AX;
	_AX = mouseSetFunction;
	geninterrupt(mouseInt);
#else
	_CX = 0xFF;
	_DX = FP_OFF(MouseISR);
	_AX = FP_SEG(MouseISR);
	_ES = _AX;
	_AX = mouseSetFunction;
	geninterrupt(mouseInt);
#endif
	return TRUE;
}

void I_MAKENAME(I_MouseClose)(void)
{
#if !defined(ZIL_DOSEXTENDED)
	_CX = _oldMouseMask;
	_DX = _oldMouseOffset;
	_BX = _oldMouseSegment;
	_ES = _BX;
	_AX = mouseExgFunction;
	geninterrupt(mouseInt);
#endif
	union REGS regs;
	regs.WORD.ax = 0;
#if defined(DOS32PP) || defined(DOS386)
	int386(mouseInt, &regs, &regs);
#	else
	int86(mouseInt, &regs, &regs);
#endif
}

// ----- UID_KEYBOARD -------------------------------------------------------

#if defined(DOSX286)
PIHANDLER oldCtrlCProt, oldCtrlBrkProt;
REALPTR oldCtrlC, oldCtrlBrk;

#elif defined(DOS16M)
ISRPTR oldCtrlCProt, oldCtrlBrkProt;
ISRPTR oldCtrlC, oldCtrlBrk;

#elif !defined(DOS32PP)
void interrupt (*oldCtrlC)(...);
void interrupt (*oldCtrlBrk)(...);
#endif

#if !defined(DOS32PP)
void interrupt CtrlCHandler(...)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
}
void interrupt CtrlBrkHandler(...)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
}
#endif

void I_MAKENAME(I_KeyboardOpen)(void)
{
	union REGS inregs, outregs;
	int bufferIndex;

	// Check for enhanced keyboard by calling extended shift status function.
	inregs.WORD.ax = 0x12FF;
#if defined(DOS32PP) || defined(DOS386)
	int386(0x16, &inregs, &outregs);
#	else
	int86(0x16, &inregs, &outregs);
#endif
	if (outregs.h.al != 0xFF)
	{
		_enhancedBios = 0x10;
		goto CONTINUE;
	}
	// Double check for enhanced keyboard BIOS according to the
	// IBM PS/2 Technical Reference manual, page 2-103
	inregs.h.ah = 0x05;
	inregs.WORD.cx = 0xFFFF;
#if defined(DOS32PP) || defined(DOS386)
	int386(0x16, &inregs, &outregs);
#	else
	int86(0x16, &inregs, &outregs);
#endif
	if (outregs.h.al != 0x00)
		goto CONTINUE;
	// success, carry on, read that guy back ...
	inregs.h.ah = 0x10;
	for (bufferIndex = 0; bufferIndex < 16; bufferIndex++)
	{
#if defined(DOS32PP) || defined(DOS386)
		int386(0x16, &inregs, &outregs);
#	else
		int86(0x16, &inregs, &outregs);
#endif
		if (outregs.WORD.ax == 0xFFFF)
		{
			_enhancedBios = 0x10;
			break;
		}
	}
    CONTINUE:
	// Set Control Break and Control C Handlers.
#if defined(DOSX286)
    DosSetPassToProtVec(0x1B, (PIHANDLER)CtrlBrkHandler,
        &oldCtrlBrkProt, &oldCtrlBrk);
    DosSetPassToProtVec(0x23, (PIHANDLER)CtrlCHandler,
        &oldCtrlCProt, &oldCtrlC);
#elif defined(DOS16M)
	if (d16SetPMVector(0x1B, (ISRPTR)CtrlBrkHandler, &oldCtrlBrkProt))
	{
		d16SetRMVector(0x1B, (ISRPTR)CtrlBrkHandler, &oldCtrlBrk);
		d16SetPassup(0x1B);
	}
	if (d16SetPMVector(0x23, (ISRPTR)CtrlCHandler, &oldCtrlCProt))
	{
		d16SetRMVector(0x23, (ISRPTR)CtrlCHandler, &oldCtrlC);
		d16SetPassup(0x23);
	}
#elif !defined(DOS32PP)
	oldCtrlBrk = getvect(0x1B);
	oldCtrlC = getvect(0x23);
	setvect(0x1B, CtrlBrkHandler);
	setvect(0x23, CtrlCHandler);
#endif

	I_MAKENAME(I_CheckLeadIn)();
}

void I_MAKENAME(I_KeyboardClose)(void)
{
	// Reset Control C and Control Break Handlers.
#if defined(DOSX286)
	DosSetRealProtVec(0x1B, oldCtrlBrkProt, oldCtrlBrk, 0, 0);
	DosSetRealProtVec(0x23, oldCtrlCProt, oldCtrlC, 0, 0);
#elif defined(DOS16M)
	d16SetPMVector(0x1B,oldCtrlBrkProt,0);
	d16SetRMVector(0x1B,oldCtrlBrk,0);
	d16SetPMVector(0x23,oldCtrlCProt,0);
	d16SetRMVector(0x23,oldCtrlC,0);
#elif !defined(DOS32PP)
	setvect(0x1B, oldCtrlBrk);
	setvect(0x23, oldCtrlC);
#endif
}

void I_MAKENAME(I_KeyboardRead)(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	union REGS inregs, outregs;

	inregs.h.ah = _enhancedBios + 0x00;
#if defined(DOS32PP) || defined(DOS386)
	int386(0x16, &inregs, &outregs);
#	else
	int86(0x16, &inregs, &outregs);
#endif
	*rawCode = outregs.WORD.ax;
	*value = outregs.h.al;
#if defined(ZIL_UNICODE)
	int doDoubleByte = FALSE;
	for (int i=0; mbLeadIn[i].hi && !doDoubleByte; i++)
		doDoubleByte = mbLeadIn[i].lo <= outregs.h.al && outregs.h.al <= mbLeadIn[i].hi;
	if (doDoubleByte && (
	     outregs.h.ah == 0 ||	// MS-DOS/V JP, PC-DOS/V CN
	     outregs.h.ah == 0xf1	// PC-DOS/V KR
			     ))
	{
		// Is there another character?
		inregs.h.ah = _enhancedBios + 0x00;
#if defined(DOS32PP) || defined(DOS386)
		int386(0x16, &inregs, &outregs);
#	else
		int86(0x16, &inregs, &outregs);
#endif
		// ????
		*rawCode = 0xffff;
		*value = (*value << 8) | outregs.h.al;
	}
#endif
	// Get the shift state using INT 16H, 12H(or 02H if not enhanced).
	inregs.h.ah = _enhancedBios + 0x02;
#if defined(DOS32PP) || defined(DOS386)
	int386(0x16, &inregs, &outregs);
#	else
	int86(0x16, &inregs, &outregs);
#endif
	*shiftState = outregs.h.al;
	if (_enhancedBios && (outregs.WORD.ax & 0x0800) && *value != 0)
		*shiftState &= ~0x08;
}

int I_MAKENAME(I_KeyboardQuery)(unsigned *shiftState)
{
	union REGS inregs, outregs;

#if defined(DOS16M)
	// Test if keystroke waiting.
	ZIL_UINT16 flags;
	inregs.h.ah = _enhancedBios + 0x01;
	outregs.x.flags = 0x00;
	asm	mov		ah, inregs.h.ah
	asm	int		0x16
	asm	pushf
	asm	pop		ax
	asm	mov		flags, ax

	if (flags & 0x40)
	{
		// No keystroke waiting.
		// Check for ALT key using INT 16H, 02H.
		inregs.h.ah = _enhancedBios + 0x02;
		__asm mov	ah, inregs.h.ah
		__asm int 0x16
		__asm mov outregs.h.al, al
		*shiftState = outregs.h.al;
		return FALSE;
	}
#else
	// Test if keystroke waiting.
	inregs.h.ah = _enhancedBios + 0x01;
#if defined(DOS32PP) || defined(DOS386)
	int386(0x16, &inregs, &outregs);
#	else
	int86(0x16, &inregs, &outregs);
#endif
	if (outregs.x.flags & 0x40)
	{
		// No keystroke waiting.
		// Check for ALT key using INT 16H, 02H.
		inregs.h.ah = _enhancedBios + 0x02;
#if defined(DOS32PP) || defined(DOS386)
		int386(0x16, &inregs, &outregs);
#	else
		int86(0x16, &inregs, &outregs);
#endif
		*shiftState = outregs.h.al;
		return FALSE;
	}
#endif
	return TRUE;
}

// ----- UID_PRINTER --------------------------------------------------------

void I_MAKENAME(I_PrintDotMatrixChar)(char c)
{
	union REGS regs;
	do
	{
		regs.h.ah = 2;
#if defined(DOS32PP) || defined(DOS386)
		regs.x.edx = 0;
#else
		regs.x.dx = 0;
#endif
#if defined(DOS32PP) || defined(DOS386)
		int386(0x17,&regs,&regs);
#	else
		int86(0x17,&regs,&regs);
#endif
	} while (!(regs.h.ah & 0x80));
	regs.h.ah = 0;
	regs.h.al = c;
#if defined(DOS32PP) || defined(DOS386)
	regs.x.edx = 0;
#else
	regs.x.dx = 0;
#endif
#if defined(DOS32PP) || defined(DOS386)
	int386(0x17,&regs,&regs);
#	else
	int86(0x17,&regs,&regs);
#endif

}

MAKE_SETFUNCTIONS
