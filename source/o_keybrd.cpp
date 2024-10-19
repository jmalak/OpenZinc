//	Zinc Interface Library - O_KEYBRD.CPP
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

// ----- UID_KEYBOARD -------------------------------------------------------

UID_KEYBOARD::UID_KEYBOARD(ZIL_DEVICE_STATE _state) :
	UI_DEVICE(E_KEY, _state)
{
	installed = TRUE;
}

UID_KEYBOARD::~UID_KEYBOARD(void)
{
}

EVENT_TYPE UID_KEYBOARD::Event(const UI_EVENT &event)
{
	return (event.type);
}

void UID_KEYBOARD::Poll(void)
{
}

