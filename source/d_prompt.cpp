//	Zinc Interface Library - D_PROMPT.CPP
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

// ----- UIW_PROMPT ---------------------------------------------------------

EVENT_TYPE UIW_PROMPT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Draw the prompt.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);

	if (Inherited(ID_LIST_ITEM))
	{
		lastPalette = paletteMapTable->MapPalette(paletteMapTable,
			ccode == S_CURRENT ? PM_CURRENT : PM_ANY, ID_LIST_ITEM);
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
		if (!display->isText)
			region.left += 4;

		DrawText(screenID, region, text, lastPalette, FALSE, ccode);
		if (!display->isText && ccode == S_CURRENT)
		{
			UI_REGION focus = trueRegion;
			DrawFocus(screenID, focus, ccode);
		}
	}
	else
	{
		lastPalette = LogicalPalette(ccode, ID_PROMPT);
		UI_PALETTE palette = *lastPalette;

		// Use the parent's background.
		UI_WINDOW_OBJECT *oObject = NULL;
		for (oObject = parent; oObject &&
			display->MapColor(oObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE) == BACKGROUND;
			oObject = oObject->parent)
					;
		UI_PALETTE *tPalette = oObject->LogicalPalette(ccode);

#if defined(ZIL_MSDOS)
		palette.colorAttribute &= 0x0F;
		palette.colorAttribute |= (0xF0 & tPalette->colorAttribute);
		if ((palette.colorAttribute & 0x0F) == ((palette.colorAttribute & 0xF0) >> 4))
			palette.colorAttribute = (0xF0 & tPalette->colorAttribute);
		palette.monoAttribute &= 0x0F;
		palette.monoAttribute |= (0xF0 & tPalette->monoAttribute);
		palette.colorBackground = tPalette->colorBackground;
		palette.bwBackground = tPalette->bwBackground;
		palette.grayScaleBackground = tPalette->grayScaleBackground;
#elif defined(ZIL_CURSES)
//		palette.colorAttribute &= 0x07;
//		palette.colorAttribute |= (0x38 & tPalette->colorAttribute);
//		palette.monoAttribute = tPalette->monoAttribute;
		palette = *tPalette;
#endif
		display->Rectangle(screenID, region, &palette, 0, TRUE, FALSE, &clip);
		DrawText(screenID, region, text, &palette, FALSE, ccode);
	}
	return (ccode);
}

EVENT_TYPE UIW_PROMPT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_PROMPT);
	switch (ccode)
	{
	case S_INITIALIZE:
		woStatus |= WOS_OWNERDRAW;
		UI_WINDOW_OBJECT::Event(event);
		if (hotKey && Next())
			Next()->HotKey(hotKey);
		break;

	case S_CHANGED:
	case S_CREATE:
		woFlags |= WOF_NON_SELECTABLE;
		if (!Inherited(ID_LIST_ITEM))
		{
			if (!FlagSet(woFlags, WOF_BORDER))
				relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
			if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT) &&
				relative.Width() < display->TextWidth(text, screenID, font))
				relative.right = relative.left + display->TextWidth(text, screenID, font) - 1;
		}
		UI_WINDOW_OBJECT::Event(event);
		break;

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
		Event(UI_EVENT(S_CHANGED));
		Event(UI_EVENT(S_REDISPLAY));
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
		event.type = S_REDISPLAY;
		Event(event);
	}
}
