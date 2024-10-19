//	Zinc Interface Library - Z_EVENT.CPP
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

#if defined(ZIL_CURSES)
#include <unistd.h>
#endif

// ----- UI_EVENT -----------------------------------------------------------

UI_EVENT::UI_EVENT(void) :
	type(0), rawCode(0), modifiers(0) { data = ZIL_NULLP(void); }
UI_EVENT::UI_EVENT(EVENT_TYPE _type, ZIL_RAW_CODE _rawCode) :
	type(_type), rawCode(_rawCode), modifiers(0) { data = ZIL_NULLP(void); }
UI_EVENT::UI_EVENT(EVENT_TYPE _type, ZIL_RAW_CODE _rawCode, const UI_KEY &_key) :
	type(_type), rawCode(_rawCode), modifiers(0) { key = _key; }
UI_EVENT::UI_EVENT(EVENT_TYPE _type, ZIL_RAW_CODE _rawCode, const UI_REGION &_region) :
	type(_type), rawCode(_rawCode), modifiers(0) { region = _region; }
UI_EVENT::UI_EVENT(EVENT_TYPE _type, ZIL_RAW_CODE _rawCode, const UI_POSITION &_position) :
	type(_type), rawCode(_rawCode), modifiers(0) { position = _position; }
UI_EVENT::UI_EVENT(EVENT_TYPE _type, ZIL_RAW_CODE _rawCode, const UI_SCROLL_INFORMATION &_scroll) :
	type(_type), rawCode(_rawCode), modifiers(0) { scroll = _scroll; }

static ZIL_ICHAR ZIL_FARDATA _i18nName[] 			= { 'i','1','8','n','.','d','a','t',0 };

// ----- UI_EVENT_MANAGER ---------------------------------------------------

UI_EVENT_MANAGER::UI_EVENT_MANAGER(UI_DISPLAY *_display, int _noOfElements) :
	UI_LIST((ZIL_COMPARE_FUNCTION)UI_DEVICE::CompareDevices), queueBlock(_noOfElements), level(0),
	qFlags(0)
{
	extern void z_evt_dummy(void);
	z_evt_dummy();

	// Initialize the event manager information.
	display = _display;
	UI_DEVICE::display = display;
	UI_DEVICE::eventManager = this;

	// Initialize some internationalization stuff.
	ZIL_INTERNATIONAL::DefaultI18nInitialize();	// ZIL_PATH should be set by now.
#if defined(ZIL_DO_FILE_I18N)
	if (!ZIL_I18N::i18nName)
		ZIL_I18N::i18nName = _i18nName;
	ZIL_I18N::defaultStorage = new ZIL_STORAGE_READ_ONLY(ZIL_I18N::i18nName);
#endif
	ZIL_ICHAR locName[12], langName[12];
	ZIL_INTERNATIONAL::strcpy(langName, languageManager.defaultName);
	ZIL_INTERNATIONAL::strcpy(locName , localeManager.defaultName);
	// ParseLangEnv() is done twice until we can change parameters in 5.0
	ZIL_INTERNATIONAL::ParseLangEnv(display->codeSet, locName, langName);
	ZIL_INTERNATIONAL::CharMapInitialize();
	ZIL_INTERNATIONAL::OSI18nInitialize(langName, locName);
	ZIL_INTERNATIONAL::ParseLangEnv(display->codeSet, locName, langName);
	ZIL_INTERNATIONAL::I18nInitialize(locName, langName);

#if defined(ZIL_OS2)
	hmq = WinCreateMsgQueue(display->hab, _noOfElements);
#elif defined(ZIL_MACINTOSH)
	eventMouseRegion = NewRgn();
	SetEmptyRgn(eventMouseRegion);
#endif
}

UI_EVENT_MANAGER::~UI_EVENT_MANAGER(void)
{
#if defined(ZIL_DO_FILE_I18N)
	if (ZIL_I18N::defaultStorage)
		delete ZIL_I18N::defaultStorage;
#endif
#if defined(ZIL_OS2)
	if (hmq)
	  WinDestroyMsgQueue(hmq);
#elif defined(ZIL_MACINTOSH)
	if (eventMouseRegion)
		DisposeRgn(eventMouseRegion);
#endif
}

void UI_EVENT_MANAGER::Add(UI_DEVICE *device)
{
	// Add the device to the object list.
	if (device && UI_LIST::Index(device) == -1)
	{
		UI_LIST::Add(device);
		device->Event(UI_EVENT(S_INITIALIZE, 0));
	}
}

EVENT_TYPE UI_EVENT_MANAGER::DevicePosition(ZIL_DEVICE_TYPE deviceType, int column, int line)
{
	// Send the position information to the affected device.
	UI_EVENT event(S_POSITION, 0);
	event.position.column = column;
	event.position.line = line;
	for (UI_DEVICE *device = First(); device; device = device->Next())
		if (deviceType == device->type)
		 	return (device->Event(event));
	return (event.type);
}

EVENT_TYPE UI_EVENT_MANAGER::DeviceState(ZIL_DEVICE_TYPE deviceType, ZIL_DEVICE_STATE deviceState)
{
	// Send the state information to the affected device.
	EVENT_TYPE state = D_OFF;
	UI_EVENT event(deviceState, 0);
	for (UI_DEVICE *device = First(); device; device = device->Next())
		if (deviceType == device->type)
		{
		 	device->Event(event);
			event.type = D_STATE;
		 	state = device->Event(event);
		}
	return (state);
}

EVENT_TYPE UI_EVENT_MANAGER::DeviceImage(ZIL_DEVICE_TYPE deviceType, DEVICE_IMAGE deviceImage)
{
	// Send the image information to the affected device.
	UI_EVENT event(deviceImage, 0);
	for (UI_DEVICE *device = First(); device; device = device->Next())
		if (deviceType == device->type)
		 	return (device->Event(event));
	return (event.type);
}

EVENT_TYPE UI_EVENT_MANAGER::Event(const UI_EVENT &event, ZIL_DEVICE_TYPE deviceType)
{
	// Process the event.
	EVENT_TYPE ccode = S_UNKNOWN;
	if (event.type == S_RESET_DISPLAY && !event.data)
	{
		UI_EVENT tEvent(S_DEINITIALIZE, 0);
		for (UI_DEVICE *device = First(); device; device = device->Next())
		 	ccode = device->Event(tEvent);
	}
	else if (event.type == S_RESET_DISPLAY)
	{
		display = (UI_DISPLAY *)event.data;
		UI_DEVICE::display = display;
		UI_DEVICE *tDevice = First();
		first = current = last = ZIL_NULLP(UI_ELEMENT);
		while (tDevice)
		{
			UI_DEVICE *device = tDevice;
			tDevice = device->Next();
			UI_EVENT_MANAGER::Add(device);
		}
		ccode = S_RESET_DISPLAY;
	}
	else
	{
		for (UI_DEVICE *device = First(); device; device = device->Next())
			if (deviceType == device->type || deviceType == E_DEVICE)
			 	ccode = device->Event(event);
	}

	// Return the associated response code.
	return (ccode);
}

int UI_EVENT_MANAGER::Get(UI_EVENT &event, Q_FLAGS flags)
{
	// Initialize the variables.
	if (++level == 1)
		qFlags = flags;

	UI_DEVICE *device;
	UI_QUEUE_ELEMENT *element;
	int error = -1;

	// Stay in loop while no event conditions are met.
	do
	{
		// Call all the polled devices.
		if (!FlagSet(flags, Q_NO_POLL))
		{
			// Check for a user-device to do special message polling.
#if defined(ZIL_MSWINDOWS)
			MSG message;
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First()) ||
				(!queueBlock.Full() && PeekMessage(&message, 0, 0, 0, PM_NOREMOVE)))
			{
				// Don't destroy windows messages from the windows queue if Q_NO_DESTROY
				if (!FlagSet(flags, Q_NO_DESTROY)) 
				{
					GetMessage(&message, 0, 0, 0);
					// Don't put the user message for help on the queue
					if (message.message != ZM_HELPKEY) 
					{
						UI_EVENT event(E_MSWINDOWS, message.hwnd, message.message,
							message.wParam, message.lParam);
						event.message = message;
						Put(event);
					}
				}
			}
#elif defined(ZIL_OS2)
			QMSG message;
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First()) ||
				(!queueBlock.Full() && WinPeekMsg(display->hab, &message, 0, 0, 0, PM_NOREMOVE)))
			{
				WinGetMsg(display->hab, &message, 0, 0, 0);
				UI_EVENT event(E_OS2, message.hwnd, message.msg, message.mp1, message.mp2);
				event.message = message;
				Put(event, Q_BEGIN);
			}
#elif defined(ZIL_XT)
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First()) ||
				(!queueBlock.Full() && XtAppPending(display->appContext)))
			{
				// Block if necessary and process one X event.
				XEvent message;
				XtAppNextEvent(display->appContext, &message);
				UI_EVENT event(E_XT, message);
				Put(event, Q_BEGIN);
			}
#elif defined(ZIL_MACINTOSH)
			EventRecord macEvent;
#	if ZIL_MACINTOSH < 70
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First()) ||
				(!queueBlock.Full() && GetNextEvent(everyEvent, &macEvent)))
			{
				UI_EVENT event(E_MACINTOSH, macEvent);
				Put(event, Q_BEGIN);
			}
#	else
			const long MOUSE_TICKS = 30L;	// Get mouseMovedEvents at least every half-second.
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First()) ||
				!queueBlock.Full())
			{
				// If the mouse has moved, or MOUSE_TICKS has passed, request a mouseMovedMessage.
				// NOTE: If in a control, TextEdit, or list, we will not get to this code
				// while the button is down; and the mouseUp is eaten by the Toolbox.
				long currentTick = LMGetTicks();
				static long lastTick = currentTick;
				static Point mousePt = { 0, 0 };
				static RgnHandle checkMouseRgn;
				GetMouse(&macEvent.where);
				if (!EqualPt(mousePt, macEvent.where) || (currentTick - lastTick) > MOUSE_TICKS)
				{
					checkMouseRgn = eventMouseRegion;
					lastTick = currentTick;
					mousePt = macEvent.where;
				}
				else
					checkMouseRgn = ZIL_NULLH(RgnHandle);

				if (WaitNextEvent(everyEvent, &macEvent, 5L, checkMouseRgn))
				{
					UI_EVENT event(E_MACINTOSH, macEvent);
					Put(event, Q_BEGIN);
				}
			}
#	endif
#endif

			for (device = First(); device; device = device->Next())
				device->Poll();
		}

#if defined(ZIL_MACINTOSH) && (ZIL_MACINTOSH < 70)
		SystemTask();		// Handle desk accessories.
#endif

		// Get the event.
		element = FlagSet(flags, Q_END) ? queueBlock.Last() : queueBlock.First();
		if (element)
		{
			event = element->event;
			if (!FlagSet(flags, Q_NO_DESTROY))
				queueBlock.Subtract((UI_ELEMENT *)element);
			error = 0;		// Normal operation of the event manager.
		}
		else if (FlagSet(flags, Q_NO_BLOCK))
		{
			if (--level == 0)
				qFlags = 0;
			return (-2);	// No events were in queue.
		}
#if defined(ZIL_CURSES)
		else
		{
		  // Sleep for up to 1 second only if there aren't any events
		  // in the queue
			sleep(0);
		}
#endif
#if defined(ZIL_NEXTSTEP)
		// Under NEXTSTEP not all events are routed through the Zinc event manager.
		// If the event queue is empty and we are blocked, we will hang.
		if (!FlagSet(flags, Q_BLOCK) && !element)
			error = 0;
#endif
	} while (error);

	// Return the error status.
	if (--level == 0)
		qFlags = 0;
	return (error);
}

void UI_EVENT_MANAGER::Put(const UI_EVENT &event, Q_FLAGS flags)
{
	// Place the event back in the event queue.
	UI_QUEUE_ELEMENT *element;
	if (FlagSet(flags, Q_END))
		element = (UI_QUEUE_ELEMENT *)queueBlock.Add(ZIL_NULLP(UI_QUEUE_ELEMENT));
	else
		element = (UI_QUEUE_ELEMENT *)queueBlock.Add(queueBlock.First());
	if (element)
		element->event = event;
}

void UI_EVENT_MANAGER::Subtract(UI_DEVICE *device)
{
	// Remove the device from the object list.
	if (UI_LIST::Index(device) != -1)
	{
		device->Event(UI_EVENT(S_DEINITIALIZE, 0));
		UI_LIST::Subtract(device);
	}
}

// ----- UI_QUEUE_ELEMENT ---------------------------------------------------

UI_QUEUE_ELEMENT::UI_QUEUE_ELEMENT(void) :
	UI_ELEMENT()
{
}

UI_QUEUE_ELEMENT::~UI_QUEUE_ELEMENT(void)
{
}

#if defined(applec)
// Can't hack the array delete on applec, so we make queue block pointer to pointers.
// Without this, CFront generates bogus C code.

UI_QUEUE_BLOCK::UI_QUEUE_BLOCK(int _noOfElements) :
	UI_LIST_BLOCK(_noOfElements)
{
	// Initialize the queue block.
	UI_QUEUE_ELEMENT **queueBlock = (UI_QUEUE_ELEMENT **)(new char[_noOfElements*sizeof(UI_QUEUE_ELEMENT *)]);
	elementArray = queueBlock;
	for (int i = 0; i < _noOfElements; i++)
	{
		queueBlock[i] = new UI_QUEUE_ELEMENT;
		freeList.Add(ZIL_NULLP(UI_ELEMENT), queueBlock[i]);
	}
}

UI_QUEUE_BLOCK::~UI_QUEUE_BLOCK(void)
{
	// Free the queue block.
	UI_QUEUE_ELEMENT **queueBlock = (UI_QUEUE_ELEMENT **)elementArray;

	for (int i = 0; i < noOfElements; i++)
		delete queueBlock[i];
	delete queueBlock;
}

#else	// not applec

UI_QUEUE_BLOCK::UI_QUEUE_BLOCK(int _noOfElements) :
	UI_LIST_BLOCK(_noOfElements)
{
	// Initialize the queue block.
	UI_QUEUE_ELEMENT *queueBlock = new UI_QUEUE_ELEMENT[_noOfElements];
	elementArray = queueBlock;
	for (int i = 0; i < _noOfElements; i++)
		freeList.Add(ZIL_NULLP(UI_ELEMENT), &queueBlock[i]);
}

UI_QUEUE_BLOCK::~UI_QUEUE_BLOCK(void)
{
	// Free the queue block.
	UI_QUEUE_ELEMENT *queueBlock = (UI_QUEUE_ELEMENT *)elementArray;

	// Early C++ versions require explicit number of elements.
	delete [] queueBlock;
}
#endif
