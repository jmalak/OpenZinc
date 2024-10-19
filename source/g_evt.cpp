//	Zinc Interface Library - UI_EVT.CPP
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

void z_evt_dummy(void) { } // Bug fix for broken linkers.

// ----- UI_DEVICE ----------------------------------------------------------

ALT_STATE UI_DEVICE::altState = ALT_NOT_PRESSED;
UI_DISPLAY *UI_DEVICE::display = ZIL_NULLP(UI_DISPLAY);
UI_EVENT_MANAGER *UI_DEVICE::eventManager = ZIL_NULLP(UI_EVENT_MANAGER);

// ----- UID_CURSOR ---------------------------------------------------------

int UID_CURSOR::blinkRate = 40;

// ----- UID_KEYBOARD -------------------------------------------------------

EVENT_TYPE UID_KEYBOARD::breakHandlerSet = L_EXIT;

// ----- UID_MOUSE ----------------------------------------------------------

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	void (* mouseQueueEvent)(unsigned, int *, int *) = ZIL_NULLF(void (*)(unsigned, int *, int *));
#endif
#if defined(ZIL_MSDOS) && !defined(ZIL_TEXT_ONLY)
	ZIL_ICHAR UID_MOUSE::_className[] = { 'U','I','_','M','O','U','S','E',0 };
	int UID_MOUSE::defaultInitialized;
#endif
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	char _devicePainting = FALSE;
#	if defined(DOSX286)
	char _isOS2 = FALSE;
#	endif
#endif

