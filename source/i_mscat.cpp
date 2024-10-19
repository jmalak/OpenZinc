//	Zinc Interface Library - I_MSCAT.CPP
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
#include <time.h>
#include <dos.h>
#if !defined(DOS386)
#	include <graph.h>
#endif
#include <stdlib.h>
#include <conio.h>
#if defined(DOSX286)
#	include <phapi.h>
#elif defined(DOS16M)
#	include <dos16.h>
#elif defined(DOS386)
#	include <pharlap.h>
#	include <pldos32.h>
#endif
#define USE_RAW_KEYS
#include "ui_evt.hpp"

#if !defined(DOSX286) && !defined(DOS16M)
#	define int86	_int86
#	define int86x	_int86x
#	define REGS	_REGS
#	define SREGS	_SREGS
#endif

#if defined(DOS386)
static short _videoSegment;
static int _videoWidth;
#endif

// First zero is place holder for differences between
// IBM and MS code page 932 (Japanese).
ZIL_ICHAR I_MAKENAME(_textDriverName)[] = { 'A','T',0,0 };

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
#	if defined(DOSX286) || defined(DOS386)
		REGS16 rRegs;
		memset(&rRegs, 0, sizeof(REGS16));
		rRegs.ax = 0x6300;
		DosRealIntr(0x21, &rRegs, 0L, 0);
		unsigned short sel;
		DosMapRealSeg(rRegs.ds, rRegs.si+4*sizeof(mbLeadIn_t), &sel);
		mbLeadIn = (mbLeadIn_t *)MK_FP(sel, rRegs.si);
#	elif defined(DOS16M)
		DPMIREGS16 rRegs;
		memset(&rRegs, 0, sizeof(DPMIREGS16));
		rRegs.ax = 0x6300;
		d16SignalRMInterrupt(0x21, (LPDPMIREGS)&rRegs, (LPDPMIREGS)&rRegs);
		LPVOID sel = d16MapRealAddress((LPVOID)MK_FP(rRegs.ds, rRegs.si));
		d16SetSegLimit(FP_SEG(sel), 4*sizeof(mbLeadIn_t));
		mbLeadIn = (mbLeadIn_t *)sel;
#	elif defined(DOS386)
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

// ----- UI_TEXT_DISPLAY ----------------------------------------------------

static int _originalMode = -1;
static int _mode = 0;
static void *_screenAddress = ZIL_NULLP(void);
#if defined(DOSX286) || defined(DOS16M)
	static void *_realScreenAddress = ZIL_NULLP(void);
#endif
static int _columns = 0;
static int _lines = 0;

#define outportb	_outp
#if !defined(DOS386)
#define MK_FP(s,o)	((void *)(((long)s << 16) | o))
//#define FP_SEG(p)	((unsigned)((void far *)(p) >> 16))
//#define FP_OFF(p)	((unsigned)(p))
#endif
#if !defined(_TEXTC80)
#	define	_TEXTC80	3
#endif

void I_MAKENAME(I_ScreenOpen)(int *mode, int *lines, int *columns)
{
#if defined(DOS386)
	FARPTR fptr;
#endif
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
#if defined(DOS386)
		regs.h.ah = 0x0f;
		int86(0x10, &regs, &regs);
		_mode = regs.h.al;
#else
		_setvideomode(*mode);
		if (*mode == 64)
			_setvideomoderows(_TEXTC80, _MAXTEXTROWS);
		// Check for special text mode monitors.
		regs.h.ah = 0x0f;
		int86(0x10, &regs, &regs);
		_mode = regs.h.al;
#endif
	}
	else
		_mode = _originalMode;
	*mode = _mode;
#if defined(DOS386)
	FP_SET(fptr, 0x84, 0x40);
	_lines = *lines =  PeekFarByte(fptr) + 1;
	FP_SET(fptr, 0x4a, 0x40);
	_columns = *columns = PeekFarByte(fptr);
#else
	_lines = *lines = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x84)) + 1;
	_columns = *columns = *((ZIL_UINT8 _far *)MK_FP(0x40, 0x4a));
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
#elif defined(DOS386)
	_videoSegment = (*mode == 7) ? 0xb000 : 0xb800;
	_videoWidth = _columns*2;
#else
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
	outportb(addr, newSetting);
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
#if defined(DOS386)
	REALPTR rptr;
	unsigned int lineLen = 2*(right-left+1);
	for (int i=top, j = 0; i <= bottom; i++, j++)
	{
		RP_SET(rptr, i *_videoWidth + left * 2, _videoSegment);
		WriteRealMem(rptr, buffer, lineLen);
	}
#else
	int lineLen = 2*(right-left+1);
	for (int i=top, j=0; i <= bottom; i++, j++)
	{
		memcpy((short *)_screenAddress + i * _columns + left,
		       (short *)buffer + j * _columns, lineLen);
#	if defined(DOSX286)
		REGS16 regs;
		memset(&regs, 0, sizeof(REGS16));
		regs.ax = 0xFF00;
		short *tmp = (short *)_realScreenAddress + i * _columns + left;
		regs.es  = FP_SEG(tmp);
		regs.di  = FP_OFF(tmp);
		regs.cx = lineLen;
		DosRealIntr(0x10, &regs, 0L, 0);
#	elif defined(DOS16M)
		DPMIREGS16 regs;
		memset(&regs, 0, sizeof(DPMIREGS16));
		regs.ax = 0xFF00;
		short *tmp = (short *)_realScreenAddress + i * _columns + left;
		regs.es  = FP_SEG(tmp);
		regs.di  = FP_OFF(tmp);
		regs.cx = lineLen;
		d16SignalRMInterrupt(0x10,(LPDPMIREGS)&regs,(LPDPMIREGS)&regs);
#	else
		struct SREGS sregs;
		union REGS regs;
		regs.h.ah = 0xFF;
		short *tmp = (short *)_screenAddress + i * _columns + left;
		sregs.es  = FP_SEG(tmp);
		regs.x.di  = FP_OFF(tmp);
		regs.x.cx = lineLen;
		int86x(0x10, &regs, &regs, &sregs);
#	endif
	}
#endif
}

void I_MAKENAME(I_CursorRemove)(void)
{
	union REGS regs;

	regs.h.ah = 0x02;
	regs.h.bh = 0;
	regs.x.dx = 0;
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x01;
	regs.h.al = _mode;	// for a bug on some bios's
	regs.x.cx = (_mode == 0x07 ? 0x0b0c : 0x0607) | 0x2000;
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
#elif defined(DOS386)
// This needs to be upgraded when UNICODE supported for TNT 386
	return 850;
#else
	union REGS regs;
	regs.x.ax = 0x6601;
	int86(0x21, &regs, &regs);
	if (regs.x.cflag)
		return (437);
	if (regs.x.bx == 932)
	{
		regs.x.ax = 0x3000;
		int86(0x21, &regs, &regs);
		I_MAKENAME(_textDriverName)[2] = ((regs.x.bx | regs.x.cx) == 0 ? 'i' : 'm');
		return (932);
	}
	return (regs.x.bx);
#endif
}

void I_MAKENAME(I_Beep)(void)
{
	// Code taken from example for _outp in MSC manual.
	int frequency = 200;
	clock_t duration = 100;
	_outp(0x43, 0xB6);
	frequency = (unsigned)(1193180L / frequency);
	_outp(0x42, (char)frequency);
	_outp(0x42, (char)(frequency >> 8));
	int control = _inp(0x61);
	_outp(0x61, control | 0x3);
	clock_t goal = duration + clock();
	while(goal > clock())
		;
	_outp(0x61, control);
}

// ----- UID_MOUSE ----------------------------------------------------------

static int _stopInterrupt = FALSE;
static unsigned char _oldMouseState;
static unsigned _mouseState;
static int _horizontalMickeys, _verticalMickeys;
static int iLastHMickey, iLastVMickey;
static short iState, iHMickey, iVMickey;

#if !defined(DOS386)
#	pragma check_stack (off)
#endif

#define mouseInt	0x33
#define mouseSetFunction	12
#define mouseExgFunction	20

#if defined(DOSX286)
	PIHANDLER dpmi_0x33_vec;
	PIHANDLER phar_0x33_vec;
#endif

#if defined(DOS386)
void MouseRead()
{
	// Get mouse state.
	_asm mov ax, 3
	_asm int mouseInt
	_asm mov iState,bx

	// Get mouse mickeys.
	_asm mov ax, 11
	_asm int mouseInt
	_asm mov iHMickey, cx
	_asm mov iVMickey, dx

	// Set up new mouse state. (Current state is in state)
	_mouseState = (((_oldMouseState ^ iState) << 4) | iState) << 8;
	_oldMouseState = iState;

	// Add in Keyboard Shift state
	FARPTR fptr;
	FP_SET(fptr, 0x17, 0x40);
	_mouseState |=  PeekFarByte(fptr) & 0xF;

	_horizontalMickeys = iHMickey;
	_verticalMickeys = iVMickey;

	// Set the internal mouse information for use in the Poll routine.
	if (mouseQueueEvent)
		mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
}
#else
extern "C" void __loadds I_MAKENAME(MouseISR)(void)
{
	// Get the mouse information.
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

		// Get the mouse information
		__asm mov iState, bx;
		__asm mov iHMickey, si;
		__asm mov iVMickey, di;

		// Set up new mouse mickeys
		_horizontalMickeys += iHMickey - iLastHMickey;
		_verticalMickeys   += iVMickey - iLastVMickey;
		iLastHMickey = iHMickey;
		iLastVMickey = iVMickey;

		// Set up new mouse state.
		_mouseState = ((((_oldMouseState ^ iState) << 4) | iState) << 8) | (*((ZIL_UINT8 _far *)MK_FP(0x40, 0x17)) & 0xF);
		_oldMouseState = iState;

		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		_stopInterrupt = FALSE;
	}
}
#endif

int I_MAKENAME(I_MouseOpen)(void)
{
	union REGS regs;
	struct SREGS segregs;

	_mouseState = 0;
	_horizontalMickeys = _verticalMickeys = 0;
	iLastHMickey = iLastVMickey = 0;
	iState = iHMickey = iVMickey = 0;

	regs.x.ax = 0;
	int86(mouseInt, &regs, &regs);
	if (regs.x.ax != 0xFFFF)
		return FALSE;

#if defined(DOSX286)
	// Check for running under Windows 3.x enhanced mode.
	int isWindows = 0, isOS2 = 0;
	regs.x.ax = 0x1600;
	int86(0x2f,&regs,&regs);
	isWindows = (regs.h.al>1 && regs.h.al!=0x80) ? 1 : 0;
	// Check for running under OS2 2.x
	regs.x.ax = 0x3306;
	int86(0x21,&regs,&regs);
	isOS2 = regs.h.bl >= 20;

	if ((isWindows || isOS2) && DosGetSavedProtVec(0x33,&dpmi_0x33_vec)==0)
		DosSetProtVec(0x33,dpmi_0x33_vec,&phar_0x33_vec);
#endif

#if !defined(DOS386)
	segregs.ds = 0;
	regs.x.cx = 0xFF;
	regs.x.dx = (unsigned)I_MAKENAME(MouseISR);
	segregs.es = (unsigned)((long)I_MAKENAME(MouseISR) >> 16);
	regs.x.ax = mouseSetFunction;
	int86x(mouseInt, &regs, &regs, &segregs);
#endif
	return TRUE;
}

void I_MAKENAME(I_MouseClose)(void)
{
	union REGS regs;

	regs.x.ax = 0;
	int86(mouseInt, &regs, &regs);
}

// ----- UID_KEYBOARD -------------------------------------------------------

static int _enhancedBios = 0;		// 0 or 0x10 (keyboard read)
static int _oldBreakState = 0;

#if defined(DOS386)
void CtrlCHandler(int sig)
{
	// disallow a second Ctrl-C while handling this one.
	signal(SIGINT, SIG_IGN);
	
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);

	// reinstall this handler
	signal(SIGINT, CtrlCHandler);
}
#else
static void __interrupt __far CtrlCHandler(void)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
}
#endif

#if defined(DOSX286)
static PIHANDLER oldCtrlCProt;
static REALPTR oldCtrlCReal;
#elif defined(DOS16M)
static ISRPTR oldCtrlCProt;
static ISRPTR oldCtrlCReal;
#endif

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
#if defined(DOS386)
	if (signal(SIGINT, CtrlCHandler) == SIG_ERR)
		exit(1);
#elif defined(DOSX286)
	DosSetPassToProtVec(0x23, (PIHANDLER)CtrlCHandler, &oldCtrlCProt, &oldCtrlCReal);
#elif defined(DOS16M)
	if (d16SetPMVector(0x23, (ISRPTR)CtrlCHandler, &oldCtrlCProt))
	{
		d16SetRMVector(0x23, (ISRPTR)CtrlCHandler, &oldCtrlCReal);
		d16SetPassup(0x23);
	}
#else
	_dos_setvect(0x23, CtrlCHandler);
#endif
	// Save previous state of Control-Break using INT 21H, 00H.
	inregs.x.ax = 0x3300;
	int86(0x21, &inregs, &outregs);
	_oldBreakState = outregs.h.dl;

	// Set Control-Break ON using INT 21H, 01H.
	inregs.x.ax = 0x3301;
	inregs.h.dl = 0x01;
	int86(0x21, &inregs, &outregs);
	I_MAKENAME(I_CheckLeadIn)();
}

void I_MAKENAME(I_KeyboardClose)(void)
{
	union REGS inregs, outregs;

#if defined(DOSX286)
	DosSetRealProtVec(0x23, oldCtrlCProt, oldCtrlCReal, 0, 0);
#elif defined(DOS16M)
	d16SetPMVector(0x23,oldCtrlCProt,0);
	d16SetRMVector(0x23,oldCtrlCReal,0);
#endif
	// Set Control-Break status to previous status.
	inregs.x.ax = 0x3301;
	inregs.h.dl = _oldBreakState;
	int86(0x21, &inregs, &outregs);
}

void I_MAKENAME(I_KeyboardRead)(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	union REGS inregs, outregs;

	inregs.h.ah = _enhancedBios + 0x00;
	int86(0x16, &inregs, &outregs);
#if defined(DOS386)
	*rawCode = outregs.w.ax;
#else
	*rawCode = outregs.x.ax;
#endif
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
		int86(0x16, &inregs, &outregs);
		*rawCode = Z_DONT_USE;
		*value = (*value << 8) | outregs.h.al;
	}
#endif
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

	// Test if keystroke waiting.
	ZIL_UINT16 flags;
	inregs.h.ah = _enhancedBios + 0x01;
	__asm	mov		ah, inregs.h.ah
	__asm	int		0x16
	__asm	pushf
	__asm	pop		ax
	__asm	mov		flags, ax
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
