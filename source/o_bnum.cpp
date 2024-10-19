//	Zinc Interface Library - O_BNUM.CPP
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


#include "ui_win.hpp"

// ----- OS Specific Functions ----------------------------------------------

void UIW_BIGNUM::OSDataGet(void)
{
	// No code necessary.
}

void UIW_BIGNUM::OSDataSet(void)
{
	// No code necessary.
}

int UIW_BIGNUM::OSEvent(const UI_EVENT &event, EVENT_TYPE *ccode)
{
	if (event.type == E_OS2 && event.message.msg != WM_CHAR)
	{
		*ccode = UIW_STRING::Event(event);
		return (TRUE);
	}
	return (FALSE);
}

void UIW_BIGNUM::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_BIGNUM && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}

