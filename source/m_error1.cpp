//	Zinc Interface Library - M_ERROR.CPP
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

extern "C"
{
#include <stdio.h>
#include <stdarg.h>
#include <Xm/MessageB.h>
}

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

static UIS_STATUS _returnValue;

static void ErrorCallback(Widget w, XtPointer client, XtPointer call)
{
	XmAnyCallbackStruct *cStruct = (XmAnyCallbackStruct *)call;
	if (cStruct->reason == XmCR_MAP)
	{
	Position left, top;
	Dimension width, height;
	XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, ZIL_NULLP(void));
	Display *xDisplay = XtDisplay(w);
	left = (DisplayWidth(xDisplay, DefaultScreen(xDisplay)) - width) / 2;
	top = (DisplayHeight(xDisplay, DefaultScreen(xDisplay)) - height) / 2;
	XtVaSetValues(w, XmNx, left, XmNy, top, ZIL_NULLP(void));
	}
	else if (cStruct->reason != XmCR_UNMAP ||
		(cStruct->reason == XmCR_UNMAP && _returnValue == WOS_UNANSWERED))
		_returnValue = (UIS_STATUS) (int) client;
	XtPopdown(XtParent(w));
}

UIS_STATUS UI_ERROR_SYSTEM::ErrorMessage(UI_WINDOW_MANAGER *windowManager,
	UIS_STATUS errorStatus, ZIL_ICHAR *message, ZIL_ICHAR *titleMessage)
{
	// Beep the system.
	Beep();
	if (!windowManager || !message)
		return (WOS_NO_STATUS);

	_returnValue = WOS_UNANSWERED;

	Arg args[5];
	int nargs;
	Widget screenID;

	XmString messageString = UI_WINDOW_OBJECT::CreateMotifString(message);
	nargs = 0;
	XtSetArg(args[nargs], XmNdefaultPosition, FALSE); nargs++;
	XtSetArg(args[nargs], XmNmessageString, messageString); nargs++;
#	if defined(ZIL_UNICODE)
	char *msg = ZIL_INTERNATIONAL::MapText(titleMessage ? titleMessage :
		myLanguage->GetMessage(MSG_TITLE));
	XtSetArg(args[nargs], XmNtitle, msg); nargs++;
#	else
	XtSetArg(args[nargs], XmNtitle, titleMessage ? titleMessage :
		 myLanguage->GetMessage(MSG_TITLE)); nargs++;
#	endif
	XtSetArg(args[nargs], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); nargs++;
#if defined(ZIL_UNICODE) || (ZIL_MOTIF <= 1001)
	XtSetArg(args[nargs], XmNlabelFontList,
		((UI_XT_DISPLAY *)(windowManager->display))
			->fontTable[FNT_SYSTEM_FONT].fontList); nargs++;
#else
	XmFontList fontList;
	XtVaGetValues(windowManager->display->topShell, XmNdefaultFontList, &fontList, ZIL_NULLP(void));
	if (fontList)
		XtSetArg(args[nargs], XmNlabelFontList, fontList), nargs++;
#endif
	screenID =
		XmCreateErrorDialog(windowManager->display->topShell, "Error", args, nargs);
#if defined(ZIL_UNICODE)
	delete msg;
#endif

	if (!FlagSet(errorStatus, WOS_INVALID))
		XtUnmanageChild(XmMessageBoxGetChild(screenID, XmDIALOG_CANCEL_BUTTON));
	else
		XtAddCallback(screenID, XmNcancelCallback, ErrorCallback,
			(XtPointer)WOS_NO_STATUS);
	XtUnmanageChild(XmMessageBoxGetChild(screenID, XmDIALOG_HELP_BUTTON));
	XtAddCallback(screenID, XmNokCallback, ErrorCallback,
		(XtPointer)WOS_INVALID);
	XtAddCallback(screenID, XmNmapCallback, ErrorCallback,
		(XtPointer)this);
	XtAddCallback(screenID, XmNunmapCallback, ErrorCallback,
		(XtPointer)WOS_INVALID);

	XmStringFree(messageString);
	XtManageChild(screenID);

	XEvent event;

	do {
		XtAppNextEvent(UI_WINDOW_OBJECT::display->appContext, &event);
		XtDispatchEvent(&event);
	} while (_returnValue == WOS_UNANSWERED);

	return(_returnValue);
}

