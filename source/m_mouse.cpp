//	Zinc Interface Library - M_MOUSE.CPP
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


// Thanks to John Swanson for some ideas about cursors.

#include "ui_win.hpp"
extern "C" {
#include <X11/cursorfont.h>
}

// ----- UID_MOUSE ----------------------------------------------------------

UID_MOUSE::UID_MOUSE(ZIL_DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_MOUSE, _state), image(_image)
{
	waitCursor = XCreateFontCursor(display->xDisplay, XC_watch);
	dragCursor = XCreateFontCursor(display->xDisplay, XC_hand1);
	cancelCursor = XCreateFontCursor(display->xDisplay, XC_cross);
	installed = TRUE;
}

UID_MOUSE::~UID_MOUSE(void)
{
	XFreeCursor(display->xDisplay, waitCursor);
	XFreeCursor(display->xDisplay, dragCursor);
	XFreeCursor(display->xDisplay, cancelCursor);
}

EVENT_TYPE UID_MOUSE::Event(const UI_EVENT &event)
{
	if (!installed)
		return (D_OFF);

	UI_WINDOW_OBJECT *object;

	switch(event.type)
	{
	case DM_VIEW:
		image = DM_VIEW;
		for (object = UI_WINDOW_OBJECT::windowManager->First();
			object; object = object->Next())
		{
			XUndefineCursor(display->xDisplay, XtWindow(object->shell));
		}
		XFlush(UI_WINDOW_OBJECT::display->xDisplay);
		break;

	case DM_WAIT:
	case DM_DRAG_COPY:
	case DM_CANCEL:
		image = event.type;

		for (object = UI_WINDOW_OBJECT::windowManager->First();
			object; object = object->Next())
		{
			XDefineCursor(display->xDisplay, XtWindow(object->shell),
				(event.type == DM_WAIT) ? waitCursor :
				(event.type == DM_DRAG_COPY) ? dragCursor :
				(event.type == DM_CANCEL) ? cancelCursor : 0);
		}
		XFlush(UI_WINDOW_OBJECT::display->xDisplay);
		break;

	case D_STATE:
		return (image);

	case S_POSITION:
		// Note that warping the pointer is very bad manners in X.
		XWarpPointer(display->xDisplay, None,
			RootWindowOfScreen(display->xScreen), 0, 0, 0, 0,
			event.position.column, event.position.line);
		break;
	}
		
	return (event.type);
}

void UID_MOUSE::Poll(void)
{
}

