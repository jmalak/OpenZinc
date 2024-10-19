//	Zinc Interface Library - O_MOUSE.CPP
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


#define INCL_WINPOINTERS		// OS/2 pointer functions.
#include "ui_evt.hpp"

// ----- UID_MOUSE ----------------------------------------------------------

UID_MOUSE::UID_MOUSE(ZIL_DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_MOUSE, _state), image(_image)
{
	// Initialize the new mouse information.
	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_MOUSE::~UID_MOUSE(void)
{
}

EVENT_TYPE UID_MOUSE::Event(const UI_EVENT &event)
{
	// Initialize the standard windows cursors.
	struct MOUSE_ITEM
	{
		EVENT_TYPE image;
		LONG identification;
	} _mouseTable[] = {
		{ DM_VIEW, 			SPTR_ARROW },
		{ DM_EDIT, 			SPTR_TEXT },
		{ DM_WAIT, 			SPTR_WAIT },
		{ DM_MOVE, 			SPTR_MOVE },
		{ DM_HORIZONTAL, 	SPTR_SIZEWE },
		{ DM_VERTICAL, 		SPTR_SIZENS },
		{ DM_DIAGONAL_ULLR, SPTR_SIZENWSE },
		{ DM_DIAGONAL_LLUR, SPTR_SIZENESW },
		{ 0, 0 } };

	// Switch on the raw code.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_POSITION:
		position = event.position;
		WinSetPointerPos(HWND_DESKTOP, position.column, position.line);
		break;

	case D_STATE:
		return ((state == D_OFF) ? D_OFF : image);

	case DM_CANCEL:
	case DM_DRAG_MOVE:
	case DM_DRAG_COPY:
	case DM_DRAG_MOVE_MULTIPLE:
	case DM_DRAG_COPY_MULTIPLE:
		image = ccode;
		break;

	case S_INITIALIZE:
		ccode = image;
		image = 0;
		// Continue to default.

	default:
		{
		for (int i = 0; _mouseTable[i].image; i++)
			if (_mouseTable[i].image == ccode)
			{
				HPOINTER hPointer = WinQuerySysPointer(HWND_DESKTOP, _mouseTable[i].identification, FALSE);
				WinSetPointer(HWND_DESKTOP, hPointer);
				image = ccode;
				break;
			}
		}
		break;
	}

	// Return the control code.
	return (ccode);
}

void UID_MOUSE::Poll(void)
{
}

