//	Zinc Interface Library - Z_TIMER.CPP
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


#define INCL_WINTIMER
#include "ui_win.hpp"

UID_TIMER::UID_TIMER(ZIL_DEVICE_STATE state, unsigned _interval,
	TMR_FLAGS _tmrFlags) :
	UI_DEVICE(E_TIMER, state), msec(_interval),
	interval(0, 0, 0, 0, 0, _interval / 1000, _interval % 1000),
	tmrFlags(_tmrFlags)
{
#if defined(ZIL_OS2) || defined(ZIL_MSWINDOWS) || defined(ZIL_XT)
	timerHandle = 0;
#endif
}

UID_TIMER::~UID_TIMER(void)
{
}

#if !defined(ZIL_NEXTSTEP)
#if defined(applec)
// CFront on applec can't deal with nested classes right.
NOTIFY_ELEMENT::NOTIFY_ELEMENT(UI_WINDOW_OBJECT *object) :
#else
UID_TIMER::NOTIFY_ELEMENT::NOTIFY_ELEMENT(UI_WINDOW_OBJECT *object) :
#endif
	notifyObject(object)
{
}

#if defined(applec)
// CFront on applec can't deal with nested classes right.
NOTIFY_ELEMENT::~NOTIFY_ELEMENT(void)
#else
UID_TIMER::NOTIFY_ELEMENT::~NOTIFY_ELEMENT(void)
#endif
{
}
#endif

EVENT_TYPE UID_TIMER::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
#if defined (ZIL_OS2)
		if (!timerHandle)
			timerHandle = WinStartTimer(display->hab, ZIL_NULLH(HWND), 0, msec);
#elif defined(ZIL_MSWINDOWS)
		if (!timerHandle)
#	if defined(STRICT)
			timerHandle = SetTimer(ZIL_NULLH(HWND), 0, msec, TimerCallback);
#	else
			timerHandle = SetTimer(ZIL_NULLH(HWND), 0, msec, (FARPROC)TimerCallback);
#	endif
#elif defined(ZIL_XT)
		void TimerCallback(XtPointer client_data, XtIntervalId *id);
		if (!timerHandle)
			timerHandle = XtAppAddTimeOut(XtWidgetToApplicationContext(
				UI_DEVICE::display->topShell), msec, TimerCallback,
				(XtPointer)this);
#endif
		break;

	case S_DEINITIALIZE:
#if defined (ZIL_OS2)
		if (timerHandle)
		{
			WinStopTimer(display->hab, ZIL_NULLH(HWND), timerHandle);
			timerHandle = 0;
		}
#elif defined (ZIL_MSWINDOWS)
		if (timerHandle)
		{
			KillTimer(ZIL_NULLH(HWND), timerHandle);
			timerHandle = 0;
		}
#elif defined(ZIL_XT)
		if (timerHandle)
		{
			XtRemoveTimeOut(timerHandle);
			timerHandle = 0;
		}
#endif
		break;

	case D_ON:
		lastTime.Import();
		// Continue.

	case D_OFF:
		state = ccode;
		break;

	case D_STATE:
		ccode = state;
		break;

	case S_ADD_OBJECT:
		{
		notifyList.Add(new NOTIFY_ELEMENT(event.windowObject));
		}
		break;

	case S_SUBTRACT_OBJECT:
		{
		for (NOTIFY_ELEMENT *notifyElement = notifyList.First(); notifyElement;
			notifyElement = notifyElement->Next())
		{
			if (notifyElement->notifyObject == event.windowObject)
			{
				notifyList.Subtract(notifyElement);
				delete notifyElement;
				break;
			}
		}
		}
		break;

	default:
		ccode = S_UNKNOWN;
		break;
	}

	return ccode;
}

void UID_TIMER::Poll(void)
{
	ZIL_UTIME now;

	if (state != D_OFF && (msec == 0 || *(now - lastTime) >= interval))
	{
		lastTime.Import();

		UI_EVENT timerEvent(E_TIMER);
		timerEvent.device = this;

		// Notify objects with timer event.
		for (NOTIFY_ELEMENT *notifyElement = notifyList.First(); notifyElement;
			notifyElement = notifyElement->Next())
			notifyElement->notifyObject->Event(timerEvent);

		if (FlagSet(tmrFlags, TMR_QUEUE_EVENTS))
			eventManager->Put(timerEvent);
	}
}

#if defined(ZIL_MSWINDOWS)
void FAR PASCAL TimerCallback(HWND, UINT, UINT, DWORD)
{
	UI_EVENT event;
	UI_WINDOW_OBJECT::eventManager->Get(event, Q_POLL | Q_NO_DESTROY | Q_NO_BLOCK);
}
#elif defined(ZIL_XT)
void TimerCallback(XtPointer client_data, XtIntervalId *)
{
	UID_TIMER *timer = (UID_TIMER *)client_data;

	// Send an event to so the eventManager will stop blocking and poll.
	XEvent event;
	event.type = ClientMessage;
	event.xclient.message_type = 0;
	event.xclient.format = 32;
	Status res = XSendEvent(timer->display->xDisplay,
		XtWindow(UI_WINDOW_OBJECT::windowManager->First()->shell),
		TRUE, NoEventMask, &event);

	timer->timerHandle = XtAppAddTimeOut(XtWidgetToApplicationContext(
		UI_WINDOW_OBJECT::windowManager->shell),
		timer->msec, TimerCallback, client_data);
}
#endif
