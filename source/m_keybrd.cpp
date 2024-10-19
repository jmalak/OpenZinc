//	Zinc Interface Library - M_KEYBRD.CPP
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


extern "C"
{
  //#if !defined(SCO_UNIX)
#	include <signal.h>
  //#else
  //#	include <sys/signal.h>
	// The next line fixes a problem with sigaction() for SCO Unix.
  //	extern int sigaction(int, struct sigaction *, struct sigaction *);
  //#endif
#include <stdlib.h>
}
#include "ui_win.hpp"

#define USE_SIGACTION
#define SA_HANDLER_ARGS int
#if defined(__sgi)
#   undef SA_HANDLER_ARGS
#   define SA_HANDLER_ARGS ...
#elif defined(_SUNOS4)
#   undef SA_HANDLER_ARGS
#   define SA_HANDLER_ARGS int, ...
#elif defined(_SINIX) && defined(mips)
#	undef SA_HANDLER_ARGS
#	define SA_HANDLER_ARGS int ...
#elif defined(__WATCOMC__)
#	undef USE_SIGACTION
#endif

// ----- UID_KEYBOARD -------------------------------------------------------

#if defined(USE_SIGACTION)
void CtrlCHandler(SA_HANDLER_ARGS)
#else
void CtrlCHandler(int)
#endif
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
	UI_EVENT event;
	event.rawCode = 0;
	event.key.value = 0;
	event.key.shiftState = 0;
	event.type = UID_KEYBOARD::breakHandlerSet;
	UID_KEYBOARD::eventManager->Put(event, Q_BEGIN);

	// Generate an X event because the event loop may be blocked.
	// Send an event to so the eventManager will stop blocking and poll.
	XEvent xEvent;
	xEvent.type = ClientMessage;
	xEvent.xclient.message_type = 0;
	xEvent.xclient.format = 32;
	Status res = XSendEvent(UI_WINDOW_OBJECT::windowManager->display->xDisplay,
		XtWindow(UI_WINDOW_OBJECT::windowManager->First()->shell),
		TRUE, NoEventMask, &xEvent);
	XFlush(UI_WINDOW_OBJECT::windowManager->display->xDisplay);

#if !defined(USE_SIGACTION)
	signal(SIGINT, CtrlCHandler);
#endif
}

UID_KEYBOARD::UID_KEYBOARD(ZIL_DEVICE_STATE _state) :
	UI_DEVICE(E_KEY, _state)
{
	if (installed)
		return;
	// Initialize the keyboard device.
	installed = TRUE;
#if defined(USE_SIGACTION)
	struct sigaction act;
	act.sa_handler = CtrlCHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, ZIL_NULLP(struct sigaction));
#else
	signal(SIGINT, CtrlCHandler);
#endif
}

UID_KEYBOARD::~UID_KEYBOARD(void)
{
	if (!installed)
		return;
#if defined(USE_SIGACTION)
	struct sigaction act;
	act.sa_handler = (void (*)(SA_HANDLER_ARGS))SIG_DFL;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, ZIL_NULLP(struct sigaction));
#elif defined(__sgi)
	signal(SIGINT, (void (*)(int, ...))SIG_DFL);
#elif defined(_SUNOS4)
	signal(SIGINT, SIG_DFL);
#else
	signal(SIGINT, (void (*)(int))SIG_DFL);
#endif
	installed = FALSE;
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

void UID_KEYBOARD::Poll(void)
{
}

