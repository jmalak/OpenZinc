//	Zinc Interface Library - M_TITLE.CPP
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
#include <Xm/MwmUtil.h>
}

// ----- UIW_TITLE ----------------------------------------------------------

EVENT_TYPE UIW_TITLE::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_BUTTON);
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		parent->Information(I_SET_TITLE, this);
		{
		long decor;
		decor = MWM_DECOR_TITLE;
		parent->Information(I_SET_DECORATIONS, &decor);
		}
		break;
	case S_CREATE:
	case S_CHANGED:
		ccode = UI_WINDOW_OBJECT::Event(event);
		trueRegion.top = trueRegion.bottom = -1;
		// fall thru
	case S_REDISPLAY:
		if (!parent)
			break;		// somehow a redisplay gets thru with a null parent
		nargs = 0;
#if defined(ZIL_UNICODE)
		if (mappedTitle)
			delete mappedTitle;
		mappedTitle = MapText(text);

		XtSetArg(args[nargs], XmNtitle, mappedTitle); nargs++;
		XtSetArg(args[nargs], XmNiconName, mappedTitle); nargs++;
#else
		XtSetArg(args[nargs], XmNtitle, text); nargs++;
		XtSetArg(args[nargs], XmNtitleEncoding, XA_STRING); nargs++;
		XtSetArg(args[nargs], XmNiconName, text); nargs++;
		XtSetArg(args[nargs], XmNiconNameEncoding, XA_STRING); nargs++;
#endif
		XtSetValues(shell, args, nargs);

		for (object = Previous(); object; object = object->Previous())
			if (object->NumberID() == NUMID_SYSTEM)
				break;
		if (object)
			break;		// There is a system button.
		else
		{
			// There is no system button, so initialize the system menu here.
			// This code is here because sys menu can be posted with a
			// Button3Release on the title bar.
			long systemOptions = 0;
			if (!FlagSet(parent->woAdvancedFlags, WOAF_LOCKED))
				systemOptions |= MWM_FUNC_CLOSE;
			if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE))
				systemOptions |= MWM_FUNC_MOVE;
			if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE))
				systemOptions |= MWM_FUNC_RESIZE;
			if (FlagSet(parent->woStatus, WOS_EDIT_MODE))
				systemOptions |= MWM_FUNC_MINIMIZE | MWM_FUNC_MAXIMIZE;
			else
				for (UI_WINDOW_OBJECT *object = Previous(); object;
					object = object->Previous())
				{
					if (object->Inherited(ID_MAXIMIZE_BUTTON))
						systemOptions |= MWM_FUNC_MAXIMIZE;
					if (object->Inherited(ID_MINIMIZE_BUTTON))
						systemOptions |= MWM_FUNC_MINIMIZE;
				}
			XtVaSetValues(shell, XmNmwmFunctions, systemOptions, NULL);
		}
		break;

	case S_DEINITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TITLE::OSDataSet(void)
{
	// Redisplay the string.
	if (parent && parent->screenID)
		Event(UI_EVENT(S_REDISPLAY));
}

void UIW_TITLE::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// No code necessary.
}
