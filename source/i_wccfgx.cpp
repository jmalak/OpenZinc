//      Zinc Interface Library - I_WCCFGX.CPP
//      COPYRIGHT (C) 1990-1994.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

//      This source is a modified version of the I_WCCAT.CPP file
//      shipped with Zinc.  It is used to add X-32VM and
//      FlashGraphics support to the Zinc library under the
//      Watcom compiler.
//
//      Logikal Solutions       (708)-420-0210
//

#include <conio.h>
#include <signal.h>
#include <dos.h>
#include <graph.h>
#include <stdlib.h>
#define USE_RAW_KEYS
#include "ui_evt.hpp"

#include "ui_fg.hpp"

#include <bios.h>

extern "C" {
void * _cdecl _x32_real_to_protected( unsigned int real_ptr);
}

// ----- UI_TEXT_DISPLAY ----------------------------------------------------


#define OutPortByte             outp
#define MKFP(s,o)               (((long)s << 16) | o)
#define MDA_SEGMENT                     0xb000
#define MDA_LIN_SEGMENT         (char *)((MDA_SEGMENT) << 4)
#define COLOR_SEGMENT           0xb800
#define COLOR_LIN_SEGMENT       (char *)((COLOR_SEGMENT) << 4)


void I_ScreenOpen(int *mode, int *lines, int *columns)
{
}

void I_ScreenClose(void)
{
}

void I_ScreenPut(int left, int top, int right, int bottom, void *_buffer)
{
}

void I_CursorRemove(void)
{
}

void I_CursorPosition(int y, int x, int val)
{
}

//function below added by jdh
void I_MAKENAME(I_Beep)(void)
{
	sound(200);
	delay(100);
	nosound();
}


// ----- UID_MOUSE ----------------------------------------------------------

static unsigned _oldMouseMask = 0;
static unsigned _oldMouseState = 0;
static unsigned _mouseState = 0;
static int _horizontalMickeys = 0;
static int _verticalMickeys = 0;
static int _iHMickey = 0, _iVMickey = 0;
static int _iLastHMickey = 0, _iLastVMickey = 0;
static int mouse_inited = 0;


void MouseRead()
{
	unsigned short                  *fptr;
	fg_coord_t                              x_coord;
	fg_coord_t                              y_coord;
	unsigned int                    buttons_pressed;

	x_coord                 = 0;
	y_coord                 = 0;
	buttons_pressed = fg_msm_getstatus( ( fg_coord_t _FG_FAR *)&x_coord, ( fg_coord_t _FG_FAR *)&y_coord);
	
	//;;;;;
	//              Y coordinates come back as negative values
	//;;;;;
	y_coord = fg.displaybox [FG_Y2] - y_coord;

	
	//      Set up new mouse state.  (Current state is in buttons_pressed)
	//
	_mouseState     = (((_oldMouseState ^ buttons_pressed) << 4) | buttons_pressed) << 8;
	_mouseState &= 0xffff;
		
	//      Add in keyboard shift state
	//
	_mouseState |= _bios_keybrd( _KEYBRD_SHIFTSTATUS) & 0xf;
	
	if ( mouse_inited)
	{
		_iHMickey       = (int)( x_coord - _iLastHMickey);
		_iVMickey       = (int)( y_coord - _iLastVMickey);
	}
	else
	{
		_iHMickey               = 
		_iVMickey               = 0;
		mouse_inited    = 1;
	}

	//;;;;;
	//              If nothing has happened with the mouse we just
	//              bail out.
	//;;;;;
	if ( _iLastHMickey == x_coord)
		if ( _iLastVMickey == y_coord)
			if ( _oldMouseState == buttons_pressed) return;
			
			
	_iLastHMickey   = x_coord;
	_iLastVMickey   = y_coord;
	_oldMouseState  = buttons_pressed;
		
	_horizontalMickeys      += _iHMickey;
	_verticalMickeys        += _iVMickey;
		
	//      Set the internal mouse information for use in the Poll Routine
	//
	if (mouseQueueEvent)
	{
		mouseQueueEvent( _mouseState, &_horizontalMickeys, &_verticalMickeys);
	}
	
}  // end MouseRead subroutine

int I_MouseOpen(void)
{

	//              Flash Graphics installs its own mouse driver
	//
	if ( fg.msm == 0)       return FALSE;
	
	fg_msm_setcurpos( fg.displaybox [ FG_X2]/2, fg.displaybox [FG_Y2]/2);
	
	return TRUE;
}

void I_MouseClose(void)
{
	//      Flash Graphics automatically terminates its own mouse
}

// ----- UID_KEYBOARD -------------------------------------------------------

static int _enhancedBios = 0;           // 0 or 0x10 (keyboard read)
static int _oldBreakState = 0;
static int sendBreak = 0;

void __interrupt __far CtrlCHandler(void)
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
	_dos_setvect(0x23, CtrlCHandler);

	// Save previous state of Control-Break using INT 21H, 00H.
	inregs.w.ax = 0x3300;
#ifdef __386__
	int386(0x21, &inregs, &outregs);
#else
	int86(0x21, &inregs, &outregs);
#endif
	_oldBreakState = outregs.h.dl;

	// Set Control-Break ON using INT 21H, 01H.
	inregs.w.ax = 0x3301;
	inregs.h.dl = 0x01;
#ifdef __386__
	int386(0x21, &inregs, &outregs);
#else
	int86(0x21, &inregs, &outregs);
#endif
}

void I_KeyboardClose(void)
{
	union REGS inregs, outregs;

	// Set Control-Break status to previous status.
	inregs.w.ax = 0x3301;
	inregs.h.dl = _oldBreakState;
#ifdef __386__
	int386(0x21, &inregs, &outregs);
#else
	int86(0x21, &inregs, &outregs);
#endif
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
