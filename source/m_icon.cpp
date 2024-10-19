//	Zinc Interface Library - M_ICON.CPP
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
#if defined(ZIL_MOTIF)
#	include <Xm/DrawnB.h>
#endif
}

#if defined(ZIL_XKEY)
#	include <X11/Shell.h>
#endif

extern ZIL_ICHAR *_iconDirectory;

// ----- UIW_ICON -----------------------------------------------------------

EVENT_TYPE UIW_ICON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Return the control code.
	return (ccode);
}

//	ZincToXPixmap is an unsupported function that may be changed or
//	removed in future releases.
void ZincToXPixmap(UI_DISPLAY *display, int bitmapWidth, int bitmapHeight,
	ZIL_UINT8 *bitmapArray, UI_PALETTE *paletteArray, Pixmap *pixmap,
	UI_WINDOW_OBJECT *object, ZIL_ICHAR *text, int font, WOF_FLAGS woFlags,
	int &width, int &height, int offset = 0);

EVENT_TYPE UIW_ICON::Event(const UI_EVENT &event)
{
	static int initializedTime = FALSE;
	static ZIL_UTIME lastTime, currentTime;
	static UI_REGION mouseRegion;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}


	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_ICON);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (!iconArray && iconName && defaultStorage && !defaultStorage->storageError)
		{
			defaultStorage->ChDir(_iconDirectory);
			ZIL_STORAGE_OBJECT_READ_ONLY iFile(*defaultStorage, iconName, ID_ICON_IMAGE);
			if (!iFile.objectError)
			{
				ZIL_INT16 _value; iFile.Load(&_value); iconWidth = _value;
				iFile.Load(&_value); iconHeight = _value;
				iconArray = new ZIL_UINT8[iconWidth * iconHeight];
				iFile.Load(iconArray, sizeof(ZIL_UINT8), iconWidth * iconHeight);
			}
		}
		break;

	case S_CHANGED:
		ccode = UI_WINDOW_OBJECT::Event(event);
		trueRegion.right += 4;
		trueRegion.bottom += 4;
		break;

	case S_CREATE:
	case S_REGISTER_OBJECT:
		{
		int width = 0, height = 0;
		ZincToXPixmap(display, iconWidth, iconHeight, iconArray,
			ZIL_NULLP(UI_PALETTE), &icon, parent,
			FlagSet(icFlags, ICF_MINIMIZE_OBJECT) ? ZIL_NULLP(ZIL_ICHAR) : title,
			font, WOF_JUSTIFY_CENTER, width, height);

		relative.right = relative.left + width + 2;
		relative.bottom = relative.top + height + 6;
		}
		ccode = UI_WINDOW_OBJECT::Event(event);
		trueRegion.right += 4;
		trueRegion.bottom += 4;

		nargs = 0;

		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		{
#if defined THIS_WILL_WORK_WHEN_MOTIF_BUG_IS_FIXED
			XtSetArg(args[nargs], XtNiconPixmap, icon); nargs++;
			XtSetValues(shell, args, nargs);
#else	
			//	Do it the old-fashioned way.  O'Reilly vol. six 17.2.3.
			Window iconWindow = 0;
			XtVaGetValues(shell, XtNiconWindow, &iconWindow, ZIL_NULLP(void));
			if (!iconWindow)
			{
				if (!(iconWindow = XCreateSimpleWindow(display->xDisplay,
					RootWindow(display->xDisplay, display->xScreenNumber),
					0, 0, iconWidth, iconHeight, 0,
					CopyFromParent, CopyFromParent)))
					XtVaSetValues(shell, XtNiconPixmap, icon, ZIL_NULLP(void));
				else
					XtVaSetValues(shell, XtNiconWindow, iconWindow, ZIL_NULLP(void));
			}
			XSetWindowBackgroundPixmap(display->xDisplay, iconWindow, icon);
			XClearWindow(display->xDisplay, iconWindow);
#endif
		}
		else
		{
#if defined(ZIL_MOTIF)
			XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
			XtSetArg(args[nargs], XmNlabelPixmap, icon); nargs++;
			XtSetArg(args[nargs], XmNlabelInsensitivePixmap, icon); nargs++;
			XtSetArg(args[nargs], XmNhighlightThickness, 2); nargs++;
			XtSetArg(args[nargs], XmNshadowThickness, 0); nargs++;
			XtSetArg(args[nargs], XmNmultiClick, XmMULTICLICK_KEEP); nargs++;
			RegisterObject(ZIL_NULLP(_WidgetClassRec),
				(ZIL_MOTIF_CONVENIENCE_FUNCTION)XmCreateDrawnButton, ccode, TRUE);
#endif
		}

		if (title)
			DataSet(title);
		break;

	case L_END_SELECT:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		{
			currentTime.Import();
			if (FlagSet(icFlags, ICF_DOUBLE_CLICK &&
				currentTime.Difference(lastTime) < doubleClickRate))
				ccode = UserFunction(UI_EVENT(L_END_SELECT), L_DOUBLE_CLICK);
			else
			{
				ccode = UserFunction(UI_EVENT(L_END_SELECT), L_SELECT);
				lastTime = currentTime;
			}
		}
		break;


	case S_CURRENT:
	case S_REDISPLAY:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (!FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_DEINITIALIZE:
		if (icon)
			XFreePixmap(display->xDisplay, icon);
		icon = 0;
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_ICON::OSDataSet(void)
{
	if (screenID && FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && shell)
		XtVaSetValues(shell, XtNiconName, title, ZIL_NULLP(void));
}

void UIW_ICON::OSImageInitialize(void)
{
	if (iconArray && !icon)
	{
		display->IconArrayToHandle(screenID, iconWidth, iconHeight,
			iconArray, ZIL_NULLP(UI_PALETTE), &icon);
		if (icon)
		{
			delete iconArray;
			iconArray = ZIL_NULLP(ZIL_UINT8);
		}
	}
}

void UIW_ICON::OSSystemImage(void)
{
}

void UIW_ICON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (parent && FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		parent->Information(I_SET_ICON, &icon);
	else if (objectID == ID_ICON && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
