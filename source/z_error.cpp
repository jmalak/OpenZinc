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
#include "z_stdarg.hpp"

UI_ERROR_STUB::~UI_ERROR_STUB(void)
{ }

UIS_STATUS UI_ERROR_STUB::ReportError(UI_WINDOW_MANAGER *windowManager,
	UIS_STATUS errorStatus, ZIL_ICHAR *format, ...)
{
	// Format the error message then report the error.
	ZIL_ICHAR *message = new ZIL_ICHAR[1024];
	{
		va_list args;
	 	va_start(args, format);
		ZIL_STDARG::vsprintf(message, format, args);
		va_end(args);
	}
	UIS_STATUS status = ErrorMessage(windowManager, errorStatus, message);
	delete message;

	return (status);
}

UIS_STATUS UI_ERROR_STUB::ReportError(UI_WINDOW_MANAGER *windowManager,
	ZIL_ICHAR *titleMessage, UIS_STATUS errorStatus, ZIL_ICHAR *format, ...)
{
	// Format the error message then report the error.
	ZIL_ICHAR *message = new ZIL_ICHAR[1024];
	{
		va_list args;
	 	va_start(args, format);
		ZIL_STDARG::vsprintf(message, format, args);
		va_end(args);
	}
	UIS_STATUS status = ErrorMessage(windowManager, errorStatus, message, titleMessage);
	delete message;

	return (status);
}

