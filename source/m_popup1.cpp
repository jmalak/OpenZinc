//	Zinc Interface Library - M_POPUP1.CPP
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
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
}

#if defined(_SUNOS4)
#   include<memory.h>
#endif

extern ZIL_ICHAR _tabString[];

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_ITEM::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	return(UIW_BUTTON::DrawItem(event, ccode));
}

// I use a callback to implement hotkeys cause the pulldown menu eats the event.
void PopupItemHotKeyCallback(Widget, XtPointer call, XtPointer client)
{
	XmPushButtonCallbackStruct *data = (XmPushButtonCallbackStruct *)client;
	if (data->event->type == KeyRelease)
		((UI_WINDOW_OBJECT *)call)->Event(UI_EVENT(L_SELECT));
}

static void ToggleButtonCallback(Widget, XtPointer client, XtPointer call)
{
	UIW_BUTTON *object = (UIW_BUTTON *)client;
	XmToggleButtonCallbackStruct *structPtr
		= (XmToggleButtonCallbackStruct *)call;
	object->eventManager->Put(UI_EVENT(S_VERIFY_STATUS));
}

EVENT_TYPE UIW_POP_UP_ITEM::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_POP_UP_ITEM);
	// Switch on the event type.
	switch (ccode)
	{
	case L_END_SELECT:
		if (event.type == E_MOTIF)
		{
			Event(UI_EVENT(L_SELECT));
			break;
		}
		ccode = UIW_BUTTON::Event(event);
		if (event.type != E_MOTIF && FlagSet(mniFlags, MNIF_CHECK_MARK))
			XmToggleButtonSetState(screenID,
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0, FALSE);
		break;

	case L_SELECT:
		ccode = UIW_BUTTON::Event(UI_EVENT(L_SELECT));
		if (event.type != E_MOTIF && FlagSet(mniFlags, MNIF_CHECK_MARK))
			XmToggleButtonSetState(screenID,
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0, FALSE);
		break;

	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (menu.First())
			menu.Event(event);
		else if (FlagSet(mniFlags, MNIF_SEPARATOR))
		{
			woFlags |= WOF_NON_SELECTABLE;
			relative.right = relative.left;
			relative.bottom = relative.top + display->cellHeight / 4;
		}
		else
		{
 			relative.right = relative.left +
				display->TextWidth(text, screenID, font) + display->cellHeight;
			if (menu.First())
				relative.right += display->cellHeight;
 			relative.bottom = relative.top +
				display->cellHeight - display->preSpace - display->postSpace - 1;
		}
		break;

	case S_CREATE:
	case S_REGISTER_OBJECT:
		ccode = UI_WINDOW_OBJECT::Event(event);

		if (screenID)
			break;

		nargs = 0;

		if (FlagSet(mniFlags, MNIF_SEPARATOR))
			RegisterObject(ZIL_NULLP(_WidgetClassRec), XmCreateSeparator, ccode);
		else
		{
			ZIL_ICHAR *_text = text;

			ZIL_ICHAR outText[256];
			memset(outText, ' ', 255);
			outText[255] = 0;

			// Handle tab character.
			if (text && strchr(text, '\t'))
			{
				ZIL_ICHAR tabBuffer[256];
				ZIL_ICHAR *string;
				strcpy(tabBuffer, text);
				StripHotKeyMark(tabBuffer);
				string = strtok(tabBuffer, _tabString);
				strncpy(outText, string, strlen(string));
				int tabColumn;
				parent->Information(I_GET_TABCOLUMN, &tabColumn);
				string = strtok(0, _blankString);
				if (string)
				{
					strncpy(&outText[tabColumn], string, strlen(string));
					outText[tabColumn + strlen(string)] = 0;
				}
				_text = outText;
			}

			if (hotKey)
				XtSetArg(args[nargs], XmNmnemonic, hotKey), nargs++;
			XmString labelText = CreateMotifString(_text);
			XtSetArg(args[nargs], XmNlabelString, labelText), nargs++;

			if (menu.First())
			{
				// this button has another menu attached
				RegisterObject(xmCascadeButtonWidgetClass,
					ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE);
				menu.Event(event);	// create the widget for the menu
	
				nargs = 0;	// attach the menu to the button
				XtSetArg(args[nargs], XmNsubMenuId, menu.screenID); nargs++;
				XtSetValues(screenID, args, nargs);	
			}
			else if (FlagSet(mniFlags, MNIF_CHECK_MARK))
			{
				XtSetArg(args[nargs], XmNset,
					FlagSet(woStatus, WOS_SELECTED) ? 1 : 0); nargs++;
				XtSetArg(args[nargs], XmNindicatorSize,
					display->cellHeight - 14 - 1); nargs++;
				WNF_FLAGS wnFlags;
				parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
				XtSetArg(args[nargs], XmNindicatorType,
					FlagSet(wnFlags, WNF_SELECT_MULTIPLE) ?
					XmN_OF_MANY : XmONE_OF_MANY); nargs++;
				RegisterObject(xmToggleButtonWidgetClass,
					ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE);
				XtAddCallback(screenID, XmNvalueChangedCallback,
					ToggleButtonCallback, (XtPointer)this);
			}
			else
				RegisterObject(xmPushButtonWidgetClass,
					ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode, TRUE, TRUE);
			if (menu.First())
				XtAddCallback(screenID, XmNactivateCallback,
					PopupItemHotKeyCallback, (XtPointer)this);
			if (labelText)
				XmStringFree(labelText);
		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case S_DEINITIALIZE:
		if (menu.First())
			menu.Event(event);
		// Fall thru.

	case S_CURRENT:
	case S_REDISPLAY:
		ccode = UIW_BUTTON::Event(event);
		break;

	case S_VERIFY_STATUS:
		if (screenID && FlagSet(mniFlags, MNIF_CHECK_MARK))
			XmToggleButtonSetState(screenID,	
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0, FALSE);
		break;

	default:
		if (event.type == E_MOTIF && event.message.type == KeyRelease &&
			parent && ccode != L_CANCEL)
			parent->Event(event);
		else if (event.type == E_MOTIF && event.message.type != FocusIn)
		{
			menu.Event(event);
			ccode = UIW_BUTTON::Event(event);
		}
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_POP_UP_ITEM::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (objectID == ID_POP_UP_ITEM && screenID)
	{
		XtSetSensitive(screenID, FlagSet(woFlags, WOF_NON_SELECTABLE) ?
			False : True);
		Event(UI_EVENT(S_VERIFY_STATUS));
	}
}

