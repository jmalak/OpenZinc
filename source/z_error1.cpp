//	Zinc Interface Library - Z_ERROR.CPP
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

#define ZIL_UI_ERROR_SYSTEM_DATA
#include "lang_def.cpp"

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

UI_ERROR_SYSTEM::UI_ERROR_SYSTEM(void)
{
	if (!defaultInitialized)
	{
		ZIL_LANGUAGE_MANAGER::SetLanguage(_className, STRINGS);
		defaultInitialized = TRUE;
	}
	myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className);
}

UI_ERROR_SYSTEM::~UI_ERROR_SYSTEM(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
}

