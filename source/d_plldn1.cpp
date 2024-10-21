//	Zinc Interface Library - D_PLLDN1.CPP
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

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_ITEM::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	const int ITEM_OFFSET = 4;
	UI_REGION region = trueRegion;

	/* unused parameters */ (void)event;

	// Draw the string.
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
#if defined(ZIL_MOTIF_STYLE)
	if (ccode == S_CURRENT)
		DrawShadow(screenID, region, 2, FALSE, ccode);
#elif defined(ZIL_OS2_STYLE)
	if (ccode == S_CURRENT)
		DrawShadow(screenID, region, 1, FALSE, ccode);
#endif
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
#if !defined(ZIL_TEXT_ONLY)
	if (!display->isText)
		region.left += ITEM_OFFSET;
#endif
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);
	return (ccode);
}

EVENT_TYPE UIW_PULL_DOWN_ITEM::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_PULL_DOWN_ITEM);
	switch (ccode)
	{
	case S_INITIALIZE:
		woStatus |= WOS_OWNERDRAW;
		ccode = UIW_BUTTON::Event(event);
		relative.right = relative.left + display->TextWidth(text, screenID, font) + 2 * display->cellWidth - 1;
		relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace;
		break;

	case S_RESET_DISPLAY:
		ccode = UIW_BUTTON::Event(event);
		menu.Event(event);
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case L_BEGIN_SELECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (ccode == S_UNKNOWN)
			break;
		// Continue to L_SELECT.
	case L_SELECT:
		if (userFunction)
		{
			UI_EVENT uEvent = event;
			ccode = (*userFunction)(this, uEvent, L_SELECT);
			break;
		}
		else if (!menu.First() && FlagSet(btFlags, BTF_SEND_MESSAGE))
		{
			eventManager->Put(UI_EVENT(value), Q_BEGIN);
			break;
		}

		// Continue to L_UP & L_DOWN.
	case L_UP:
	case L_DOWN:
		if (menu.First() && windowManager->Index(&menu) == -1)
		{
	 		menu.RegionConvert(menu.relative, TRUE);
			menu.relative.left = trueRegion.left;
			menu.relative.top = trueRegion.bottom + 1;
			if (menu.Current())
				menu.Current()->woStatus &= ~WOS_CURRENT;
			*windowManager + &menu;
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_PULL_DOWN_ITEM::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	/* unused parameters */ (void)objectID;

	if (screenID && FlagSet(woFlags, WOF_NON_SELECTABLE) &&
	    parent->Information(I_GET_CURRENT, ZIL_NULLP(void)) == this)
	{
			// This sets the right menu item but colors it active.
			parent->Event(UI_EVENT(L_RIGHT));
			// This uncolors it.
			parent->Event(UI_EVENT(S_REDISPLAY));
	}
	else if (screenID)
		Event(UI_EVENT(S_REDISPLAY));
}
