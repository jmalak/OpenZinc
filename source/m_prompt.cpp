//	Zinc Interface Library - M_PROMPT.CPP
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
#include <stdlib.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
}

// ----- UIW_PROMPT ---------------------------------------------------------

EVENT_TYPE UIW_PROMPT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	UI_REGION region = trueRegion;

	// Handle case of parent also owner draw.
	ZIL_SCREENID tScreenID = screenID;
	UI_WINDOW_OBJECT *tParent = parent;
	while (!tScreenID && tParent)
	{
		tScreenID = tParent->screenID;
		region.left += tParent->trueRegion.left;
		region.top += tParent->trueRegion.top;
		region.right += tParent->trueRegion.left;
		region.bottom += tParent->trueRegion.top;
		tParent = tParent->parent;
	}
	if (!tScreenID)
		return ccode;

	// Draw the prompt.
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(tScreenID, region, FALSE, ccode);
	lastPalette = LogicalPalette(ccode, ID_PROMPT);
	UI_PALETTE palette = *lastPalette;
	// Use the parent's background.
	UI_PALETTE *tPalette = parent->LogicalPalette(ccode);
//	display->Rectangle(tScreenID, region, &palette, 0, TRUE, FALSE, &clip);
	DrawText(tScreenID, region, text, &palette, FALSE, ccode);
	return (ccode);
}


EVENT_TYPE UIW_PROMPT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_PROMPT);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		lastPalette = LogicalPalette(ccode);
		if (hotKey && Next())
			Next()->HotKey(hotKey);
		break;

	case S_CHANGED:
	case S_CREATE:
		if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))
		{
			relative.right =
				relative.left + display->TextWidth(text, screenID, font) +
				(FlagSet(woFlags, WOF_BORDER) ? 4 : 0);
		}
		relative.bottom = relative.top + display->cellHeight -
			display->preSpace - display->postSpace - 1;
		UI_WINDOW_OBJECT::Event(event);
		trueRegion.top += 2; trueRegion.bottom -= 2;
		if (ccode == S_CHANGED)
			break;
		// Fall thru.

	case S_REGISTER_OBJECT:
		{
		if (FlagSet(woFlags, WOF_BORDER))
		{
			nargs = 0;
			RegisterObject(xmFrameWidgetClass, NULL, ccode, TRUE);
			trueRegion.right -= 2;
			trueRegion.bottom -= 2;
		}

		nargs = 0;
		if (hotKey)
			XtSetArg(args[nargs], XmNmnemonic, hotKey), nargs++;

		XmString labelText = CreateMotifString(text);
		XtSetArg(args[nargs], XmNlabelString, labelText), nargs++;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_CENTER), nargs++;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_END), nargs++;
		else
			XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_BEGINNING), nargs++;
		XtSetArg(args[nargs], XmNshadowThickness,
			FlagSet(woFlags, WOF_BORDER) ? 2 : 0); nargs++;
		XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
		XtSetArg(args[nargs], XmNmarginWidth, 0); nargs++;
		woAdvancedFlags |= WOAF_NON_CURRENT;	// Set traversalOn to FALSE.
		XtSetArg(args[nargs], XmNtraversalOn, FALSE); nargs++;
		RegisterObject(xmLabelWidgetClass, NULL, ccode, TRUE, TRUE,
			screenID ? screenID : parent ? parent->screenID : shell);
		if (labelText)
			XmStringFree(labelText);
		}
		break;

#if 0
	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			DrawItem(event, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;
#endif

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_PROMPT::OSDataSet(void)
{
	if (screenID)
	{
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))
		{
			XmString labelText = CreateMotifString(text);

			// Two calls to XtVaSetValues because of Motif bug.
			XtVaSetValues(screenID, XmNlabelString, labelText, ZIL_NULLP(void));
			XtVaSetValues(screenID, XmNwidth, trueRegion.Width(), ZIL_NULLP(void));
			if (labelText)
				XmStringFree(labelText);
		}
		else
			Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
	}
}

void UIW_PROMPT::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-displayed.
	if (objectID == ID_PROMPT && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
