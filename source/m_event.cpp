//	Zinc Interface Library - M_EVENT.CPP
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


#include "ui_evt.hpp"

UI_EVENT::UI_EVENT(EVENT_TYPE _type, XEvent &xEvent) : type(_type)
{
	message = xEvent;
	rawCode = modifiers = 0;
	data = ZIL_NULLP(void);

	switch (xEvent.type)
	{
	case ButtonPress:
	case ButtonRelease:
		position.line = xEvent.xbutton.y;
		position.column = xEvent.xbutton.x;
		modifiers = xEvent.xbutton.state;
		modifiers &= ~(Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask);
		if (xEvent.type == ButtonPress)
		{
			if (xEvent.xbutton.button == Button1)
				rawCode = M_LEFT | M_LEFT_CHANGE;
			else if (xEvent.xbutton.button == Button2)
				rawCode = M_MIDDLE | M_MIDDLE_CHANGE;
			else if (xEvent.xbutton.button == Button3)
				rawCode = M_RIGHT | M_RIGHT_CHANGE;
		}
		else	// ButtonRelease
		{
			if (xEvent.xbutton.button == Button1)
				rawCode = M_LEFT_CHANGE;
			else if (xEvent.xbutton.button == Button2)
				rawCode = M_MIDDLE_CHANGE;
			else if (xEvent.xbutton.button == Button3)
				rawCode = M_RIGHT_CHANGE;
		}
		break;

	case KeyPress:
	case KeyRelease:

		// For key presses, the keysym is stored in rawCode and the
		// ascii value is stored in key.value. The keycode isn't stored,
		// but it's accesible thru XKeysymToKeycode().
		modifiers = xEvent.xkey.state;
		key.shiftState = 0;
		if (modifiers & ShiftMask)
			key.shiftState |= S_SHIFT;
		if (modifiers & ControlMask)
			key.shiftState |= S_CTRL;
		if (modifiers & Mod1Mask)
			key.shiftState |= S_ALT;
		if (modifiers & LockMask)
			key.shiftState |= S_CAPS_LOCK;

		{
		KeySym keySym;
		rawCode = 0;
		ZIL_ICHAR c;
#if defined(ZIL_UNICODE)
		int numChars = 1;
		char *tmp = new char[20];
		if (xEvent.type == KeyPress)
		{
			Status status;
			numChars =
				XmbLookupString(UI_DEVICE::display->ic, &xEvent.xkey, tmp,
						20, &keySym, &status);
			if (status == XBufferOverflow)
			{
				delete tmp;
				tmp = new char[numChars];
				numChars =
					XmbLookupString(UI_DEVICE::display->ic,
							&xEvent.xkey,
							tmp, numChars, &keySym,
							&status);
			}
		}
		else
		{
			numChars = XLookupString(&xEvent.xkey, tmp, 20,&keySym,
						 ZIL_NULLP(XComposeStatus));
		}
		c = ZIL_INTERNATIONAL::UnMapChar(tmp);
		delete tmp;
#else
		int numChars = XLookupString(&xEvent.xkey, &c, 1, &keySym,
			ZIL_NULLP(XComposeStatus));
#endif
		rawCode = keySym;
		if (numChars == 1)
		{
			key.value = c;
		}
		else if (numChars == 0 && message.xkey.state & Mod1Mask)
		{
			// Key isn't mapped in this font.  Try with no modifiers.
			rawCode = XLookupKeysym(&xEvent.xkey, 0);
			key.value = 0;
		}
		else
			key.value = 0;
		}

		break;

	case MotionNotify:
		position.line = xEvent.xmotion.y;
		position.column = xEvent.xmotion.x;
		if (xEvent.xmotion.state & Button1Mask)
			rawCode = M_LEFT;
		else if (xEvent.xmotion.state & Button2Mask)
			rawCode = M_MIDDLE;
		else if (xEvent.xmotion.state & Button3Mask)
			rawCode = M_RIGHT;
		break;
	}
}

EVENT_TYPE UI_EVENT::InputType(void) const
{
	// Check for mouse or keyboard information.
	if (type == E_MOTIF)
	{
		switch (message.type)
		{
		case ButtonPress:
		case ButtonRelease:
		case MotionNotify:
			return (E_MOUSE);

		case KeyPress:
		case KeyRelease:
			return (E_KEY);
		}
	}

	return (type);
}

