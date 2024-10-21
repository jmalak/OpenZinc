//	Zinc Interface Library - Z_APP.CPP
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


#if !defined(NO_ZIL_APP)
#include "ui_win.hpp"

#if defined(DOS32PP)
void CommitStack(void)
// NOTE: must compile with the -N switch to enable stack overflow
// checking before this function can work (Borland uses the stack
// check prologue to commit locals that exceed 4K)
{

   // commit a big chunk of stack space
   long stackgrow[6000];

   stackgrow[5999] = 0;

   return;

}
#endif

// ----- UI_APPLICATION -----------------------------------------------------

#if defined(ZIL_MSWINDOWS)
UI_APPLICATION::UI_APPLICATION(HINSTANCE _hInstance, HINSTANCE _hPrevInstance,
	LPSTR _lpszCmdLine, int _nCmdShow) :
	hInstance(_hInstance), hPrevInstance(_hPrevInstance),
	lpszCmdLine(_lpszCmdLine), nCmdShow(_nCmdShow)
{
	// Set up the display search path.
#	if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
	char tPathName[ZIL_MAXPATHLEN];
	GetModuleFileName(_hInstance, tPathName, ZIL_MAXPATHLEN);
	ZIL_ICHAR *pathName = ISOtoUNICODE(tPathName);
#	else
	ZIL_ICHAR tPathName[ZIL_MAXPATHLEN];
	GetModuleFileName(_hInstance, tPathName, ZIL_MAXPATHLEN);
	ZIL_ICHAR *pathName = strdup(tPathName);
#	endif
	ZIL_STORAGE_READ_ONLY::searchPath = UI_APPLICATION::searchPath =
		new UI_PATH(pathName, TRUE);

#	if defined(ZIL_UNICODE)
	ZIL_ICHAR *argString = ISOtoUNICODE(lpszCmdLine);
#	else
	ZIL_ICHAR *argString = strdup(lpszCmdLine);
#	endif

	argc = 1;
	if (argString && *argString)
	{
		ZIL_ICHAR *pos = argString;
		while (*pos)
		{
			if (*pos != ' ' && *pos != '\t')
			{
				argc++;
				while (*pos && *pos != ' ' && *pos != '\t')
					pos++;
			}
			else
				pos++;
		}
	}

	argv = new ZIL_ICHAR *[argc + 1];
	argv[0] = pathName;
	if (argc > 1)
	{
		argv[1] = argString;
		int c = 2;
		for (ZIL_ICHAR *pos = argString; *pos; pos++)
			if (*pos == ' ')
			{
				while (*pos == ' ' && *pos)
					*pos++ = 0;
				argv[c++] = pos;
			}
	}
	else
		delete argString;

	argv[argc] = ZIL_NULLP(ZIL_ICHAR);

	// Create the display.
	display = new UI_MSWINDOWS_DISPLAY(hInstance, hPrevInstance, nCmdShow);

#else
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

#	if defined(ZIL_MSDOS)
#		if defined(DOS32PP)
		CommitStack();
#	    endif
	display = ZIL_NULLP(UI_DISPLAY);
#		if !defined(ZIL_TEXT_ONLY)
	for (int tOffset = 0; argc > 1 && _textSwitches[tOffset].text; tOffset++)
		if (!stricmp(argv[1], _textSwitches[tOffset].text))
		{
			for (int j = 2; j < argc; j++)
				argv[j-1] = argv[j];
			argc--;
#			if !defined(ZIL_GRAPHICS_ONLY)
			display = new UI_TEXT_DISPLAY(_textSwitches[tOffset].numberID);
#			endif
			break;
		}
#		endif

#		if defined(ZIL_TEXT_ONLY)
	// ----- Code for the TEXT display -----
	if (!display)
		display = new UI_TEXT_DISPLAY;
#		elif defined(BGI)
	// ----- Code for the Borland BGI display -----
	UI_BGI_DISPLAY::searchPath = searchPath;
	if (!display)
#			if defined(DOS32PP)
		if (argc > 1 && !stricmp(argv[1], _graphicSwitches[0].text))
			display = new UI_BGI_DISPLAY(256);
		else
			display = new UI_BGI_DISPLAY;
#			else
		display = new UI_BGI_DISPLAY;
#			endif
#		elif defined(MSC)
	// ----- Code for the Microsoft graphics display -----
	UI_MSC_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_MSC_DISPLAY;
#		elif defined(WCC)
	// ----- Code for the Watcom graphics display -----
	UI_WCC_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_WCC_DISPLAY;
#		elif defined(META)
	// ----- Code for the Metawindow graphics display -----
	if (!display)
		display = new UI_META_DISPLAY;
#		else
	// ----- Default for the GFX display -----
//	UI_GRAPHICS_DISPLAY::searchPath = searchPath;
	if (!display)
	{
		int mode = 4;
		for (int gOffset = 0; argc > 1 && _graphicSwitches[gOffset].text; gOffset++)
			if (!stricmp(argv[1], _graphicSwitches[gOffset].text))
			{
				mode = _graphicSwitches[gOffset].numberID;
				for (int j = 2; j < argc; j++)
					argv[j-1] = argv[j];
				argc--;
				break;
			}
		display = new UI_GRAPHICS_DISPLAY(mode);
	}
#		endif

	// ----- Code for generic text display -----
	if (!display->installed)
	{
		delete display;

#		if !defined(ZIL_GRAPHICS_ONLY)
		display = new UI_TEXT_DISPLAY;
#		else
//		puts("Graphics initilialization failed");
//		exit(0);
#		endif
	}

#	elif defined(ZIL_OS2)
	// ----- Code for the OS/2 graphics display -----
	display = new UI_OS2_DISPLAY;

#	elif defined(ZIL_XT)
	// ----- Code for the Motif graphics display -----
	display = new UI_XT_DISPLAY(&argc, _argv);

#	elif defined(ZIL_CURSES)
	// ----- Code for the Curses character mode display -----
	display = new UI_TEXT_DISPLAY;

#	elif defined(ZIL_MACINTOSH)
	// ----- Code for the Macintosh graphics display -----
	display = new UI_MACINTOSH_DISPLAY;

#	elif defined(ZIL_NEXTSTEP)
	// ----- Code for the NEXTSTEP display -------
	display = new UI_NEXTSTEP_DISPLAY;
#	endif
#endif

	// Create the event manager
	eventManager = new UI_EVENT_MANAGER(display);
	// Create the input devices
#if defined(ZIL_PENDOS)
	*eventManager + new UID_KEYBOARD + new UID_PENDOS + new UID_CURSOR;
#else
	*eventManager + new UID_KEYBOARD + new UID_MOUSE + new UID_CURSOR;
#endif
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
#if defined(ZIL_MSWINDOWS)
	delete argv[0];
	delete argv[1];
#else
	for (int i=0; argv[i]; i++)
		delete argv[i];
#endif
	delete []argv;
}

#if defined(ZIL_NEXTSTEP)
void EventHandler (DPSTimedEntry teNumber, double timeDelta, UI_APPLICATION *eventData)
{
	UI_EVENT event;
	EVENT_TYPE ccode;

	eventData->eventManager->Get(event, Q_NO_BLOCK);
	
	// Process the event information.
	
	ccode = eventData->windowManager->Event(event);
	if (ccode == L_EXIT || ccode == S_NO_OBJECT)
		[NXApp terminate:NXApp];
	return;
}
#endif

EVENT_TYPE UI_APPLICATION::Control(void)
{
#if defined(ZIL_NEXTSTEP)
	myTimedEvent = DPSAddTimedEntry(0.1, &EventHandler, (UI_APPLICATION *)this, NX_BASETHRESHOLD);
	[NXApp run];
	return (S_UNKNOWN);
#else
	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		eventManager->Get(event, Q_NORMAL);
		ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);
	return (ccode);
#endif
}
#endif

void UI_APPLICATION::LinkMain(void)
{
}

// ----- Declaration of WinMain & main --------------------------------------

#if defined(ZIL_MSWINDOWS)
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	if (hPrevInstance)
		return (1);
	UI_APPLICATION *application = new UI_APPLICATION(hInstance, hPrevInstance, lpszCmdLine, nCmdShow);
#elif defined(ZIL_MACINTOSH)
int main(int argc, char **argv)
{
	// Initialize stack, heap and non-relocatable blocks,
	// helping prevent application heap fragmentation.
	SetApplLimit(GetApplLimit() - 0xf000L);	// 60K more on the stack.
	MaxApplZone();
	for (int moreMasterBlocks = 0; moreMasterBlocks < 5; moreMasterBlocks++)
		MoreMasters();
	UI_APPLICATION *application = new UI_APPLICATION(argc, argv);
#else
int main(int argc, char **argv)
{
	UI_APPLICATION *application = new UI_APPLICATION(argc, argv);
#endif
	// Call the application program.
	int ccode = application->Main();

	// Restore the system.
	delete application;

	return (ccode);
}
