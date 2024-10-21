//	Zinc Interface Library - Z_APP.CPP
//	COPYRIGHT (C) 1990-1994.  All Rights Reserved.
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


//	Quick hack to make Zinc support Flash Graphics and X-32VM
//  under Watcom
//
//	Written	01-24-1995
//	By		Roland Hughes
//

#if !defined(NO_ZIL_APP)
#include "ui_win.hpp"

#include "ui_fg.hpp"

#include <stdlib.h>

// ----- UI_APPLICATION -----------------------------------------------------

#if defined(ZIL_MSDOS)
UI_APPLICATION::UI_APPLICATION(int _argc, char **_argv) :
	argc(_argc)
{
	// Set up the display search path.
#	if defined(ZIL_UNICODE)
	ZIL_ICHAR *pathName = ISOtoUNICODE(_argv[0]);	
#if defined(ZIL_NEXTSTEP)
	// Set up the display search path.
	ZIL_STORAGE_READ_ONLY::searchPath = (UI_PATH *)(UI_APPLICATION::searchPath) =
		new UI_PATH(pathName, TRUE);
#else	
	ZIL_STORAGE_READ_ONLY::searchPath = UI_APPLICATION::searchPath =
		new UI_PATH(pathName, TRUE);
#endif
	delete pathName;
#	else
#if defined(ZIL_NEXTSTEP)
	// Set up the display search path.
	ZIL_STORAGE_READ_ONLY::searchPath = (UI_PATH *)(UI_APPLICATION::searchPath) =
		new UI_PATH(_argv[0], TRUE);
#else
	// Don't combine these two lines!  It breaks gnu C++ 2.5.8.
	ZIL_STORAGE_READ_ONLY::searchPath = new UI_PATH(_argv[0], TRUE);
	UI_APPLICATION::searchPath = ZIL_STORAGE_READ_ONLY::searchPath;
#endif
#	endif


	argv = new ZIL_ICHAR *[argc + 1];
	for (int i = 0; i < argc; i++)
		argv[i] = UnMapText(_argv[i]);
	argv[argc] = ZIL_NULLP(ZIL_ICHAR);

	display = 0;

	// ----- Code for the Flashgraphics display -----
	UI_FG_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_FG_DISPLAY;

	// Create the event manager
	eventManager = new UI_EVENT_MANAGER(display);
	// Create the input devices
	*eventManager + new UID_KEYBOARD + new UID_MOUSE + new UID_CURSOR;
	// Create the window manager
	windowManager = new UI_WINDOW_MANAGER(display, eventManager);
}

UI_APPLICATION::~UI_APPLICATION(void)
{
	delete windowManager;
	delete eventManager;
	delete display;
	delete searchPath;
	delete defaultCharMap;
}

EVENT_TYPE UI_APPLICATION::Control(void)
{
	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		eventManager->Get(event, Q_NORMAL);
		ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);
	return (ccode);
}

void UI_APPLICATION::LinkMain(void)
{
}

// ----- Declaration of WinMain & main --------------------------------------

int main(int argc, char **argv)
{
	UI_APPLICATION *application = new UI_APPLICATION(argc, argv);

	// Call the application program.
	int ccode = application->Main();

	// Restore the system.
	delete application;

	return (ccode);
}

#endif
#endif