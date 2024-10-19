//	Zinc Interface Library - M_SYS.CPP
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
#include <stdlib.h>
#include <Xm/AtomMgr.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
}

static ZIL_ICHAR _formatString[] = { '%','s',' ','%','c','%','c',' ','f','.','s','e','n','d','_','m','s','g',' ','%','d','\\','n', 0 };

// ----- UIW_SYSTEM_BUTTON ------------------------------------------------

EVENT_TYPE UIW_SYSTEM_BUTTON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_BUTTON);
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		long decor = MWM_DECOR_MENU;
		parent->Information(I_SET_DECORATIONS, &decor);
		}
		break;

	case S_CREATE:
		{
		UI_WINDOW_OBJECT::Event(event);
		trueRegion.top = trueRegion.bottom = -1;
		long systemOptions = 0;
		if (!FlagSet(syFlags, SYF_GENERIC) && menu.First())
		{
			ZIL_ICHAR menuString[512], *menuPtr = menuString;
			Atom motifMessages = 0, zincProtocol = 0;
			XtCallbackProc UserButtonCallback;
			int userButtonCount;
			*menuPtr = 0;
			for (UI_WINDOW_OBJECT *object = menu.First(); object;
				object = object->Next())
			{
				MNIF_FLAGS mFlags;
				object->Information(I_GET_FLAGS, &mFlags, ID_POP_UP_ITEM);
				EVENT_TYPE flags = ((UIW_POP_UP_ITEM *)object)->mniFlags;
				if (FlagSet(flags, MNIF_MOVE)
						&& !FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE))
					systemOptions |= MWM_FUNC_MOVE;
				else if (FlagSet(flags, MNIF_SIZE) &&
						!FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE))
					systemOptions |= MWM_FUNC_RESIZE;
				else if (FlagSet(flags, MNIF_MINIMIZE))
					systemOptions |= MWM_FUNC_MINIMIZE;
				else if (FlagSet(flags, MNIF_MAXIMIZE))
					systemOptions |= MWM_FUNC_MAXIMIZE;
				else if (FlagSet(flags, MNIF_CLOSE))
					systemOptions |= MWM_FUNC_CLOSE;
				else if (FlagSet(mFlags, MNIF_SEPARATOR))
					continue;
				else if (FlagSet(mFlags, MNIF_RESTORE))
					continue;	// Motif always adds restore.
				else
				{
					// User defined button.
					object->Event(UI_EVENT(S_INITIALIZE));
					if (!motifMessages)
					{
						// First user button.  Intern atoms.
						motifMessages = XmInternAtom(display->xDisplay,
							_XA_MWM_MESSAGES, FALSE);
						XmAddWMProtocols(shell, &motifMessages, 1);
						userButtonCount = 0;
					}
					XmAddProtocolCallback(shell, motifMessages,
						(Atom)userButtonCount,
						UserButtonCallback,
#if defined(_IBM_RS6000) || defined(__DVX__) || defined(__DECCXX)
						(caddr_t)object);
#else
						(XtPointer)object);
#endif
					object->HotKey((ZIL_ICHAR *)object->Information(I_GET_TEXT, NULL));
					ZIL_ICHAR *displayText = strdup((ZIL_ICHAR *)
						object->Information(I_GET_TEXT, NULL));
					StripHotMark(displayText);
					sprintf(menuPtr, _formatString,
						displayText, object->HotKey() ? '_' : ' ',
						object->HotKey() ? object->HotKey() : ' ',
						userButtonCount++);
					if (displayText)
						delete (displayText);
					menuPtr += strlen(menuPtr);
				}
			}
			if (*menuString)
				XtVaSetValues(shell, XmNmwmMenu, menuString, NULL);
		}
		else
		{
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
		}
		XtVaSetValues(shell, XmNmwmFunctions, systemOptions, NULL);
		}
		break;

	case S_CHANGED:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
	}

	return (ccode);
}

void UserButtonCallback(Widget, XtPointer client, XtPointer)
{
	UIW_BUTTON *button = (UIW_BUTTON *)client;
	button->Event(UI_EVENT(L_SELECT));
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_SYSTEM_BUTTON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// No code necessary.
}
