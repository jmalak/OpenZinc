//	Zinc Interface Library - D_KEYBRD.CPP
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


#define USE_RAW_KEYS
#include <stdlib.h>
#include "ui_evt.hpp"
#if defined(ZIL_MSDOS)
#	include <dos.h>
#endif

// ----- UID_KEYBOARD -------------------------------------------------------

UID_KEYBOARD::UID_KEYBOARD(ZIL_DEVICE_STATE _state) :
	UI_DEVICE(E_KEY, _state)
{
	if (installed)
		return;
	// Initialize the keyboard device.
	installed = TRUE;
	I_KeyboardOpen();
}

UID_KEYBOARD::~UID_KEYBOARD(void)
{
	if (!installed)
		return;
	I_KeyboardClose();
	installed = FALSE;
}

void UID_KEYBOARD::Poll(void)
{
	// Make sure the keyboard is valid.
	if (!installed)
		return;
	unsigned shiftState;
	UI_EVENT event;
	if (!I_KeyboardQuery(&shiftState))
	{
		// Put L_ALT_KEY on queue if pressed and released alone.
		if (FlagSet(shiftState, S_ALT) && altState == ALT_NOT_PRESSED)
			altState = ALT_PRESSED_NO_EVENTS;
		else if (!FlagSet(shiftState, S_ALT))
		{
			if (altState == ALT_PRESSED_NO_EVENTS)
			{
				event.rawCode = 0;
				event.key.value = 0;
				event.key.shiftState = 0;
				event.type = S_ALT_KEY;
				if (state != D_OFF)
					eventManager->Put(event, Q_END);
			}
			altState = ALT_NOT_PRESSED;
		}
		return;
	}

	// Set the key information and call the event manager.
	if (altState == ALT_PRESSED_NO_EVENTS)
		altState = ALT_PRESSED_EVENTS;

	// Get the key from the low level driver.
	event.type = E_KEY;
	I_KeyboardRead(&event.rawCode, &event.key.shiftState, &event.key.value);
#if defined(ZIL_CURSES)
	event.modifiers = event.key.shiftState;
#elif defined(ZIL_MSDOS)
	if (FlagSet(event.key.shiftState, S_SHIFT))
		event.modifiers |= S_SHIFT;
	if (FlagSet(event.key.shiftState, S_CTRL))
		event.modifiers |= S_CTRL;
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
	char tmp[2] = { (char)(event.key.value >> 8), (char)event.key.value };
	if (tmp[0] == 0)
		tmp[0] = tmp[1];
	event.key.value = UnMapChar(tmp);
#endif
#endif
	// Place event on the queue.
	if (state != D_OFF && eventManager)
	{
		if (event.rawCode == CTRL_C || event.rawCode == CTRL_BREAK)
		{
			event.type = breakHandlerSet;
			eventManager->Put(event, Q_BEGIN);
		}
		else
			eventManager->Put(event, Q_END);
	}
}

EVENT_TYPE UID_KEYBOARD::Event(const UI_EVENT &event)
{
	// Switch on the rawCode.
	switch (event.type)
	{
	case D_OFF:
	case D_ON:
		state = event.type;
		break;
	}

	// Return the keyboard state.
	return (state);
}
