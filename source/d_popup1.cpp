//	Zinc Interface Library - D_POPUP1.CPP
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


#include <string.h>
#include "ui_win.hpp"

const int ITEM_OFFSET = 4;
extern ZIL_ICHAR _tabString[];

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
	UI_PALETTE *lastPalette = this->lastPalette;
	UI_REGION clip = this->clip;
#endif
	UI_REGION region = trueRegion;

	// Draw the pop-up item.
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	region.right = parent->trueRegion.right;
#if defined(ZIL_MOTIF_STYLE)
	if (FlagSet(woStatus, WOS_CURRENT) && FlagSet(parent->woStatus, WOS_CURRENT))
		DrawShadow(screenID, region, 2, FALSE, ccode);
#elif defined(ZIL_OS2_STYLE)
	if (!display->isText && FlagSet(woStatus, WOS_CURRENT) && FlagSet(parent->woStatus, WOS_CURRENT))
	{
		region.right-=2;
		region.bottom--;
		DrawShadow(screenID, region, 1, FALSE, ccode);
	}
#endif
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	if (FlagSet(mniFlags, MNIF_SEPARATOR))
	{
		lastPalette = LogicalPalette(S_DISPLAY_ACTIVE, ID_OUTLINE);
		region.top = (region.top + region.bottom) / 2;
		display->Line(screenID, region.left, region.top, region.right,
			region.top, lastPalette, 1, FALSE, &clip);
#if defined(ZIL_MOTIF_STYLE)
		lastPalette = LogicalPalette(S_DISPLAY_ACTIVE, ID_WHITE_SHADOW);
		display->Line(screenID, region.left, region.top + 1, region.right,
			region.top + 1, lastPalette, 1, FALSE, &clip);
#endif
		return (ccode);
	}
#if !defined(ZIL_TEXT_ONLY)
	if (!display->isText)
		region.left += ITEM_OFFSET;
#endif

	// Draw the check mark.
	ZIL_COLOR foregroundColor = display->MapColor(lastPalette, TRUE);
	if (!FlagSet(woStatus, WOS_SELECTED) || !FlagSet(mniFlags, MNIF_CHECK_MARK))
		;
	else if (display->isText)
		display->Text(screenID, region.left, region.top, myDecorations->GetText(ZIL_PopupCheckMark), lastPalette, 1, FALSE, FALSE, &clip);
	else
	{
#if !defined(ZIL_TEXT_ONLY)
		ZIL_UINT8 *bitmap = myDecorations->GetBitmap(ZIL_PopupCheckMark);
		int width = bitmap[0];
		int height = bitmap[1];
		bitmap = &bitmap[2];
		int size = width * height;
		for (int i = 0; i < size; i++)
			if (bitmap[i] != BACKGROUND)
				bitmap[i] = foregroundColor;
		int top = trueRegion.top + (trueRegion.bottom - trueRegion.top - height) / 2;
		display->Bitmap(screenID, region.left, top, width, height,
			bitmap, ZIL_NULLP(UI_PALETTE), &clip);
#endif
	}
	region.left += 2 * display->cellWidth;

	// Handle tab character.
	if (text && strchr(text, '\t'))
	{
		ZIL_ICHAR tabBuffer[256];
		ZIL_ICHAR *string;
		strcpy(tabBuffer, text);
		string = strtok(tabBuffer, _tabString);

		UI_REGION textRegion = region;
		DrawText(screenID, textRegion, string, lastPalette, FALSE, ccode);
		int tabColumn;
		parent->Information(I_GET_TABCOLUMN, &tabColumn);
		region.left += tabColumn;
		string = strtok(0, _blankString);
		if (string)
			DrawText(screenID, region, string, lastPalette, FALSE, ccode);
	}
	else
		DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	// Draw the right arrow.
	if (menu.First() && display->isText)
		display->Text(screenID, region.right-1, region.top, myDecorations->GetText(ZIL_PopupRightArrow), lastPalette, 1, FALSE, FALSE, &clip);
	else if (menu.First())
	{
#if !defined(ZIL_TEXT_ONLY)
		ZIL_UINT8 *bitmap = myDecorations->GetBitmap(ZIL_PopupRightArrow);
		int width = bitmap[0];
		int height = bitmap[1];
		bitmap = &bitmap[2];
		int size = width * height;
		for (int i = 0; i < size; i++)
			if (bitmap[i] != BACKGROUND)
				bitmap[i] = foregroundColor;
		region.left = trueRegion.right - 2 * ITEM_OFFSET;
#	if defined(ZIL_OS2_STYLE)
		region.left -= 2;
#	endif
		region.top = trueRegion.top + (trueRegion.bottom - trueRegion.top - height) / 2;
		display->Bitmap(screenID, region.left, region.top, width, height,
			bitmap, ZIL_NULLP(UI_PALETTE), &clip);
#endif
	}

#if defined(ZIL_OPTIMIZE)
	this->lastPalette = lastPalette;
#endif
	return (ccode);
}

EVENT_TYPE UIW_POP_UP_ITEM::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_POP_UP_ITEM);
	switch (ccode)
	{
	case S_INITIALIZE:
		// Check for system commands.
		if (FlagSet(mniFlags, 0x00FF))
		{
			if (FlagSet(mniFlags, MNIF_RESTORE))
				value = L_RESTORE;
			else if (FlagSet(mniFlags, MNIF_MOVE))
				value = L_MOVE;
  			else if (FlagSet(mniFlags, MNIF_SIZE))
				value = L_SIZE;
			else if (FlagSet(mniFlags, MNIF_MINIMIZE))
				value = L_MINIMIZE;
			else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
				value = L_MAXIMIZE;
			else if (FlagSet(mniFlags, MNIF_CLOSE))
				value = S_CLOSE;

			for (UI_WINDOW_OBJECT *object = this; object; object = object->parent)
				if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT) && object->parent)
				{
					if (value < 0)
						value += S_MDICHILD_EVENT;
					else
						value += L_MDICHILD_EVENT;
					break;
				}
		}
		woStatus |= WOS_OWNERDRAW;
		ccode = UIW_BUTTON::Event(event);
		break;

	case S_RESET_DISPLAY:
		ccode = UIW_BUTTON::Event(event);
		menu.Event(event);
		break;

	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(mniFlags, MNIF_SEPARATOR))
		{
			woFlags |= WOF_NON_SELECTABLE;
			relative.right = relative.left;
			relative.bottom = relative.top + display->cellHeight / 2;
		}
		else if (display->isText)
		{
 			relative.right = relative.left + display->TextWidth(text, screenID, font) + 2;
			if (menu.First())
				relative.right += 4;
 			relative.bottom = relative.top;
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
 			relative.right = display->TextWidth(text, screenID, font) + display->cellHeight;
			if (menu.First())
				relative.right += display->cellHeight;
 			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace;
#endif
		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case L_CONTINUE_SELECT:
		UIW_BUTTON::Event(event);

		if	(windowManager->Index(parent) != -1 && trueRegion.Overlap(event.position))
		{
			UI_WINDOW_OBJECT *popup = windowManager->First();
			while(popup && popup != parent && popup != &menu)
				if (FlagSet(popup->woAdvancedFlags, WOAF_TEMPORARY))
				{
					windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
					popup = windowManager->First();
				}
			if (menu.First() && windowManager->Index(&menu) == -1)
				Event(UI_EVENT(L_RIGHT));
		}
		break;

	case L_BEGIN_SELECT:
		{
		if (menu.First())
			Event(UI_EVENT(L_SELECT));
		else
			UIW_BUTTON::Event(event);

		UI_WINDOW_OBJECT *pWindow = NULL;
		for (pWindow = parent; pWindow &&
			!pWindow->Inherited(ID_PULL_DOWN_MENU); pWindow = pWindow->parent)
			;
		if (pWindow)
			pWindow->Event(event);
		}	
		break;

	case L_END_SELECT:
		if (!trueRegion.Overlap(event.position) || !FlagSet(btStatus, BTS_DEPRESSED))
			return (UIW_BUTTON::Event(event));
		ccode = L_SELECT;
		// Continue to L_SELECT.
	case L_SELECT:
	case L_LEFT:
	case L_RIGHT:
		if (windowManager->Index(parent) == -1)
			ccode = S_UNKNOWN;
		else if (ccode == L_SELECT && menu.First() && windowManager->Index(&menu) != -1)
			break;
		else if ((ccode == L_RIGHT || ccode == L_SELECT) && menu.First())
		{
 			menu.RegionConvert(menu.relative, TRUE);
			menu.relative.left = display->isText ? trueRegion.right - 1 :
				trueRegion.right - 2 * ITEM_OFFSET;
			menu.relative.top = trueRegion.top - 1;
			if (menu.Current())
				menu.Current()->woStatus &= ~WOS_CURRENT;
			*windowManager + &menu;
		}
		else if (ccode == L_SELECT)
		{	// Changes start here
			ccode = UIW_BUTTON::Event(event);
			if (parent && parent == windowManager->First() &&
				FlagsSet(parent->woAdvancedFlags, WOAF_TEMPORARY | WOAF_NO_DESTROY))
				windowManager->Event(UI_EVENT(S_ALT_KEY));
			else if (parent && parent == windowManager->First() &&
				FlagSet(parent->woAdvancedFlags, WOAF_TEMPORARY))
			{
				for (UI_WINDOW_OBJECT *object = parent; object &&
					FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY);
					object = object->parent)
					eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY), Q_BEGIN);
			}
		}  // Changes end here
		else if (ccode == L_LEFT && parent->parent &&
			parent->parent->Inherited(ID_POP_UP_ITEM))
			windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
		else
		{
			for (UI_WINDOW_OBJECT *object = windowManager->First();
				object && FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY);
				object = windowManager->First())
				windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
			eventManager->Put(UI_EVENT(ccode));
			eventManager->Put(UI_EVENT(L_DOWN));
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_POP_UP_ITEM::OSUpdateSettings(ZIL_OBJECTID)
{
	if (FlagSet(woFlags, WOF_NON_SELECTABLE) && parent &&
		parent->screenID && ((UIW_POP_UP_MENU *)parent)->Current() == this)
	{
		// This sets the right menu item.
		((UIW_POP_UP_MENU *)parent)->Event(UI_EVENT(L_DOWN));
	}
}
