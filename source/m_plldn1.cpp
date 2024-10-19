//	Zinc Interface Library - M_PLLDN1.CPP
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
#include <Xm/CascadeB.h>
}

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	return (ccode);
}

EVENT_TYPE UIW_PULL_DOWN_ITEM::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_PULL_DOWN_ITEM);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		relative.right = relative.left + display->TextWidth(text,
			screenID, font) + 2 * display->cellWidth - 1;
		if (menu.First())
			menu.Event(event);
		break;

	case S_CREATE:
	case S_REGISTER_OBJECT:
		ccode = UI_WINDOW_OBJECT::Event(event);

		nargs = 0;
		if (hotKey)
			XtSetArg(args[nargs], XmNmnemonic, hotKey), nargs++;
		{
		XmString labelText = CreateMotifString(text);
		XtSetArg(args[nargs], XmNlabelString, labelText), nargs++;
		RegisterObject(xmCascadeButtonWidgetClass,
			ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE);
		if (labelText)
			XmStringFree(labelText);
		}
		if (menu.First())
		{
			menu.Event(event);	// create the widget for the menu

			nargs = 0;	// attach the menu to the button
			XtSetArg(args[nargs], XmNsubMenuId, menu.screenID); nargs++;
			XtSetValues(screenID, args, nargs);	
		}
		break;

	case L_CANCEL:
		break;

	case S_CURRENT:
	case S_ADD_OBJECT:
		if (event.windowObject != menu.Current())
			menu.Event(event);
		break;

	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case S_DEINITIALIZE:
		menu.woStatus |= WOS_INTERNAL_ACTION;
		menu.Event(event);
		menu.woStatus &= ~WOS_INTERNAL_ACTION;
		// Fall thru.

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PULL_DOWN_ITEM::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (objectID == ID_PULL_DOWN_ITEM && screenID)
		XtSetSensitive(screenID, FlagSet(woFlags, WOF_NON_SELECTABLE) ? 0 : 1);
}

