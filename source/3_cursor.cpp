//	Zinc Interface Library - W_CURSOR.CPP
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

// ----- UID_CURSOR ---------------------------------------------------------

// WATCOM has a compiler bug if the constructor and destructor don't
// have any other functions in the file (Poll, Event). So definition
// of constructor & destructor are in d_cursor, w_cursor, o_cursor, etc.
#if defined(__WATCOMC__)

UID_CURSOR::UID_CURSOR(ZIL_DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_CURSOR, _state), image(_image)
{
	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_CURSOR::~UID_CURSOR(void)
{
	// No cursor information needs to be destroyed.
}

#endif

EVENT_TYPE UID_CURSOR::Event(const UI_EVENT &event)
{
	// Switch on the raw code.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_POSITION:
		position = event.position;
		SetCaretPos(position.column, position.line);
		break;

	case D_STATE:
		return ((state == D_OFF) ? D_OFF : image);
	}

	// Return the control code.
	return (ccode);
}

void UID_CURSOR::Poll(void)
{
}

