//	Zinc Interface Library - Z_DEVICE.CPP
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

// ----- UI_DEVICE ----------------------------------------------------------

UI_DEVICE::UI_DEVICE(ZIL_DEVICE_TYPE _type, ZIL_DEVICE_STATE _state) :
	installed(FALSE), type(_type), state(_state)
{
}

UI_DEVICE::~UI_DEVICE(void)
{
}

int UI_DEVICE::CompareDevices(void *device1, void *device2)
{
	return ((int)(((UI_DEVICE *)device1)->type - ((UI_DEVICE *)device2)->type));
}
