//	Zinc Interface Library - D_WIN2.CPP
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

extern ZIL_ICHAR *_genericFieldID;

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

EVENT_TYPE UI_WINDOW_OBJECT::DrawBorder(ZIL_SCREENID screenID, UI_REGION &region,
	int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
#endif

#if !defined(ZIL_GRAPHICS_ONLY)
	// Draw the text mode border.
#	if defined(ZIL_STANDARD_BORDER)
	if (display->isText && region.bottom == region.top)
	{
		if (region.left == region.right)
			return (ccode);
		ZIL_ICHAR _leftBracket[2]; _leftBracket[0] = '['; _leftBracket[1] = 0;
		ZIL_ICHAR _rightBracket[2]; _rightBracket[0] = ']'; _rightBracket[1] = 0;
		display->Text(screenID, region.left++, region.top, _leftBracket,
			lastPalette, -1, TRUE, FALSE, &clip);
		display->Text(screenID, region.right--, region.top, _rightBracket,
			lastPalette, -1, TRUE, FALSE, &clip);
		return (ccode);
	}
#	elif defined(ZIL_SHADOW_BORDER)
	if (display->isText && region.bottom == region.top)
		return (ccode);
#	elif defined(ZIL_3D_BORDER)
	if (display->isText)
	{
		if (region.left == region.right)
			return (ccode);
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT) && !Inherited(ID_MENU))
		{
			UI_PALETTE *palette = LogicalPalette(ccode, ID_BORDER);
			ZIL_ICHAR _leftBracket[2]; _leftBracket[0] = '['; _leftBracket[1] = 0;
			ZIL_ICHAR _rightBracket[2]; _rightBracket[0] = ']'; _rightBracket[1] = 0;
			if (Inherited(ID_TITLE))
			{
				_leftBracket[0] = _rightBracket[0] = FlagSet(parent->woStatus, WOS_CURRENT) ? ' ' : 0x8A;
			}
			display->Text(screenID, region.left++, region.top, _leftBracket,
				palette, -1, TRUE, FALSE, &clip);
			display->Text(screenID, region.right--, region.top, _rightBracket,
				palette, -1, TRUE, FALSE, &clip);
		}
		else if (parent)
		{
			UI_REGION clipRegion;
			UI_PALETTE *lightShadow;
			UI_PALETTE *darkShadow;
			if (Inherited(ID_PULL_DOWN_MENU))
			{
				clipRegion = clip;
				lightShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
				darkShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
			}
			else if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
			{
				clipRegion = clip;
				lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
				darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
				display->Rectangle(screenID, region.left, region.top,
					region.right + 1, region.bottom + 1, ZIL_NULLP(UI_PALETTE),
					0, TRUE, FALSE, ZIL_NULLP(UI_REGION));
			}
			else
			{
				++region;
				parent->Information(I_GET_CLIPREGION, &clipRegion);
				if (parent->Inherited(ID_COMBO_BOX))
					++clipRegion;
				lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
				darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
			}
			display->Line(screenID, region.left, region.top, region.right, region.top, darkShadow, 1, FALSE, &clipRegion);
			display->Line(screenID, region.left, region.top, region.left, region.bottom, darkShadow, 1, FALSE, &clipRegion);
			display->Line(screenID, region.right, region.top, region.right, region.bottom, lightShadow, 1, FALSE, &clipRegion);
			display->Line(screenID, region.left, region.bottom, region.right, region.bottom, lightShadow, 1, FALSE, &clipRegion);
			display->Text(screenID, region.left, region.top, UI_TEXT_DISPLAY::_tCornerUL, darkShadow, 1, TRUE, FALSE, &clipRegion);
			display->Text(screenID, region.left, region.bottom, UI_TEXT_DISPLAY::_tCornerLL, darkShadow, 1, TRUE, FALSE, &clipRegion);
			display->Text(screenID, region.right, region.top, UI_TEXT_DISPLAY::_tCornerUR, lightShadow, 1, TRUE, FALSE, &clipRegion);
			display->Text(screenID, region.right, region.bottom, UI_TEXT_DISPLAY::_tCornerLR, lightShadow, 1, TRUE, FALSE, &clipRegion);
			--region;
		}
		return (ccode);
	}
#	endif

	if (display->isText)
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
		display->Rectangle(screenID, region, outline, 1, FALSE, FALSE, &clip);
		region -= 1;
		if (fillRegion)
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
		return (ccode);
	}
#endif

	// Draw the object border.
	if (Inherited(ID_STATUS_ITEM) || Inherited(ID_TABLE_RECORD))
	{
		// Draw chiseled border
		UI_PALETTE *leftTop = LogicalPalette(ccode, ID_DARK_SHADOW);
		UI_PALETTE *rightBottom = LogicalPalette(ccode, ID_WHITE_SHADOW);
		lastPalette = LogicalPalette(ccode, ID_STATUS_ITEM);
		display->Line(screenID, region.right, region.top, region.right,
			region.bottom, rightBottom, 1, FALSE, &clip);
		display->Line(screenID, region.left, region.bottom, region.right,
			region.bottom, rightBottom, 1, FALSE, &clip);
		display->Line(screenID, region.left, region.bottom, region.left,
			region.top, leftTop, 1, FALSE, &clip);
		display->Line(screenID, region.left, region.top, region.right,
			region.top, leftTop, 1, FALSE, &clip);
		--region;
	}
#if defined(ZIL_MSWINDOWS_STYLE)
	else
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
		display->Rectangle(screenID, region, outline, 1, FALSE, FALSE, &clip);
		region -= 1;
	}
#elif defined(ZIL_OS2_STYLE)
	else
	{
		// Draw border dark at left & top, light at right and bottom.
		UI_PALETTE *leftTop = LogicalPalette(ccode, ID_DARK_SHADOW);
		UI_PALETTE *rightBottom = LogicalPalette(ccode, ID_WHITE_SHADOW);
		if (Inherited(ID_PULL_DOWN_MENU))
		{
			UI_PALETTE *temp = leftTop;
			leftTop = rightBottom;
			rightBottom = temp;
		}
		if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY) && !Inherited(ID_LIST))
		{
			display->Rectangle(screenID, region, leftTop, 1, FALSE, FALSE, &clip);
			--region;
			display->Line(screenID, region.left, region.top, region.left,
				region.bottom, rightBottom, 1, FALSE, &clip);
			region.left++;
		}
		else if ((Inherited(ID_STRING) && !Inherited(ID_TEXT)) || Inherited(ID_COMBO_BOX) &&
			!FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
		{
			display->Line(screenID, region.right, region.top, region.right,
				region.bottom, rightBottom, 1, FALSE, &clip);
			region.right--;
			display->Line(screenID, region.left, region.bottom, region.right,
				region.bottom, rightBottom, 1, FALSE, &clip);
			region.bottom--;
			display->Rectangle(screenID, region, leftTop, 1, FALSE, FALSE, &clip);
			--region;
		}
		else
		{
			display->Line(screenID, region.left, region.top, region.left,
				region.bottom-1, leftTop, 1, FALSE, &clip);
			region.left++;
			display->Line(screenID, region.left, region.top, region.right-1,
				region.top, leftTop, 1, FALSE, &clip);
			region.top++;
			if (Inherited(ID_STATUS_BAR))
			{
				display->Line(screenID, region.left, region.top, region.right-1,
					region.top, rightBottom, 1, FALSE, &clip);
				region.top++;
			}
			display->Line(screenID, region.right, region.top, region.right,
				region.bottom, rightBottom, 1, FALSE, &clip);
			region.right--;
			if (Inherited(ID_HZ_LIST))
			{
				display->Line(screenID, region.right, region.top+1, region.right,
					region.bottom-1, leftTop, 1, FALSE, &clip);
				region.right--;
			}
			display->Line(screenID, region.left, region.bottom, region.right,
				region.bottom, rightBottom, 1, FALSE, &clip);
			region.bottom--;
			if (Inherited(ID_VT_LIST))
			{
				display->Line(screenID, region.left+1, region.bottom, region.right-1,
					region.bottom, leftTop, 1, FALSE, &clip);
				region.bottom--;
			}
		}
	}
#elif defined(ZIL_MOTIF_STYLE)
	else
	{
		if (!Inherited(ID_BUTTON))
			DrawShadow(screenID, region, Inherited(ID_MENU) ? 2 : -2, FALSE, ccode);
	}
#endif

	// Fill in the region.
	if (fillRegion)
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	return (ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawFocus(ZIL_SCREENID screenID, UI_REGION &region, EVENT_TYPE)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
#endif

	display->VirtualGet(screenID, region);
	display->Rectangle(screenID, region, display->xorPalette, -1, FALSE, TRUE, &clip);
	display->VirtualPut(screenID);

	return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	return (FALSE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawShadow(ZIL_SCREENID screenID, UI_REGION &region,
	int depth, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
#endif

	// Make sure there is depth to the object
#if defined(ZIL_3D_BORDER) && !defined(ZIL_GRAPHICS_ONLY)
	if (!depth || (display->isText && (AbsValue(depth) < 2 || region.bottom - region.top < 1)))
	{
		if (region.left == region.right || FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			return (ccode);
		UI_REGION clipRegion;
		parent->Information(I_GET_CLIPREGION, &clipRegion);
		++region;
		UI_PALETTE *lightShadow = LogicalPalette(ccode, (depth > 0) ? ID_DARK_SHADOW : ID_WHITE_SHADOW);
		UI_PALETTE *darkShadow = LogicalPalette(ccode, (depth > 0) ? ID_WHITE_SHADOW : ID_DARK_SHADOW);
		int width = FlagSet(woFlags, WOF_BORDER) ? 2 : 1;
		display->Line(screenID, region.left, region.top, region.right, region.top, darkShadow, width, FALSE, &clipRegion);
		display->Line(screenID, region.left, region.top, region.left, region.bottom, darkShadow, width, FALSE, &clipRegion);
		display->Line(screenID, region.right, region.top, region.right, region.bottom, lightShadow, width, FALSE, &clipRegion);
		display->Line(screenID, region.left, region.bottom, region.right, region.bottom, lightShadow, width, FALSE, &clipRegion);
		display->Text(screenID, region.left, region.top, &UI_TEXT_DISPLAY::_tCornerUL[width-1], darkShadow, 1, TRUE, FALSE, &clipRegion);
		display->Text(screenID, region.left, region.bottom, &UI_TEXT_DISPLAY::_tCornerLL[width-1], darkShadow, 1, TRUE, FALSE, &clipRegion);
		display->Text(screenID, region.right, region.top, &UI_TEXT_DISPLAY::_tCornerUR[width-1], lightShadow, 1, TRUE, FALSE, &clipRegion);
		display->Text(screenID, region.right, region.bottom, &UI_TEXT_DISPLAY::_tCornerLR[width-1], lightShadow, 1, TRUE, FALSE, &clipRegion);
		--region;
		return (ccode);
	}
#else
	if (!depth || (display->isText && (AbsValue(depth) < 2 || region.bottom - region.top < 1)))
		return (ccode);
#endif

	// Draw the shadow.
 	if (!display->isText)
	{
#if !defined(ZIL_TEXT_ONLY)
#	if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
		// Load the shadow colors.
		UI_PALETTE palette;
		UI_PALETTE *lightShadow = LogicalPalette(ccode, (depth > 0) ? ID_WHITE_SHADOW : ID_DARK_SHADOW);
		UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);

		// Set darkShadow to be the button color when depressed.
		if (depth < 0 && lastPalette)
		{
			palette = *darkShadow;
			darkShadow = &palette;
			palette.colorForeground = lastPalette->colorBackground;
			palette.bwForeground = lastPalette->bwBackground;
			palette.grayScaleForeground = lastPalette->grayScaleBackground;
		}
#	elif defined(ZIL_MOTIF_STYLE)
		// Load the shadow colors.
		UI_PALETTE *lightShadow = LogicalPalette(ccode, (depth > 0) ? ID_WHITE_SHADOW : ID_DARK_SHADOW);
		UI_PALETTE *darkShadow = LogicalPalette(ccode, (depth > 0) ? ID_DARK_SHADOW : ID_WHITE_SHADOW);

		// Status bars have reversed shadow
		if (Inherited(ID_STATUS_BAR))
		{
			UI_PALETTE *temp = lightShadow;
			lightShadow = darkShadow;
			darkShadow = temp;
		}
#	endif

		// Draw the shadow pattern.
		int absoluteDepth = AbsValue(depth);
		for (int i = 0; i < absoluteDepth; i++)
		{
#	if defined(ZIL_MSWINDOWS_STYLE)
			display->Line(screenID, region.right, region.top, region.right,
				region.bottom, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.bottom, region.right - 1,
				region.bottom, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top, depth < 0 ? region.right + i :
				region.right - 1, region.top, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top + 1, region.left, depth < 0 ?
				region.bottom + i : region.bottom - 1, lightShadow, 1, FALSE, &clip);
			--region;
#	elif defined(ZIL_OS2_STYLE)
			// Swap the outline colors for menu items.
			if (Inherited(ID_MENU_ITEM))
			{
				UI_PALETTE *temp = lightShadow;
				lightShadow = darkShadow;
				darkShadow = temp;
			}
			display->Line(screenID, region.left, region.top, region.right,
				region.top, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top + 1, region.left,
				region.bottom, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.right, region.top + 1, region.right,
				region.bottom, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left + 1, region.bottom, region.right - 1,
				region.bottom, darkShadow, 1, FALSE, &clip);
			--region;
#	elif defined(ZIL_MOTIF_STYLE)
			display->Line(screenID, region.left, region.top, region.right,
				region.top, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top + 1, region.left,
				region.bottom, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.right, region.top + 1, region.right,
				region.bottom, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left + 1, region.bottom, region.right - 1,
				region.bottom, darkShadow, 1, FALSE, &clip);
			--region;
#	endif
		}
#endif
	}
#if !defined(ZIL_GRAPHICS_ONLY)
	else if (searchID == ID_BORDER)
	{
		display->Rectangle(screenID, region, ZIL_NULLP(UI_PALETTE), 0, TRUE, FALSE,
			(parent->parent ? &clip : ZIL_NULLP(UI_REGION)));
		region.right--;
		region.bottom--;
	}
	else if (depth > 0)
	{
		extern ZIL_ICHAR _tLowerShadow, _tUpperShadow, _tFullShadow;
		UI_PALETTE *palette = parent->lastPalette;
		ZIL_ICHAR shadow = _tLowerShadow;
		display->Text(screenID, region.right, region.top, &shadow, palette, 1, TRUE, FALSE, &clip);
		shadow = _tFullShadow;
		for (int row = region.top + 1; row < region.bottom; row++)
			display->Text(screenID, region.right, row, &shadow, palette, 1, TRUE, FALSE, &clip);
		shadow = palette->fillCharacter;
		display->Text(screenID, region.left, region.bottom, &shadow, palette, 1, TRUE, FALSE, &clip);
		shadow = _tUpperShadow;
		for (int column = region.left + 1; column <= region.right; column++)
			display->Text(screenID, column, region.bottom, &shadow, palette, 1, TRUE, FALSE, &clip);
		region.right--;
		region.bottom--;
	}
	else
	{
		UI_PALETTE *palette = parent->LogicalPalette(ccode);
		display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);
		region.left++;
		region.bottom--;
	}
#endif

	// Fill in the region.
	if (fillRegion && region.bottom > region.top)
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	return (ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawText(ZIL_SCREENID screenID, UI_REGION &region,
	const ZIL_ICHAR *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
#endif

	// Fill in the region.
	if (!palette)
		palette = lastPalette;
	display->VirtualGet(screenID, region);
#if defined(ZIL_3D_BORDER) && !defined(ZIL_GRAPHICS_ONLY)
	if (display->isText && fillRegion && Inherited(ID_TITLE) && !FlagSet(parent->woStatus, WOS_CURRENT))
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_BORDER);
		display->Line(screenID, region.left, region.top, region.right,
			region.top, outline, 1, FALSE, &clip);
	}
	else if (fillRegion)
		display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);
#else
	if (fillRegion)
		display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);
#endif
	UI_REGION clipRegion;
	clip.Overlap(region, clipRegion);

	// Draw the text.
	if (text && text[0])
	{
		int position = region.left;
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			position = region.right - display->TextWidth(text, screenID, font);
		else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			// Add 1 to round during divide, and 1 more for the
			// white space at the right side of the character.
			position = region.left + (region.right - region.left + 2 - display->TextWidth(text, screenID, font)) / 2;
		if (position > region.left)
			region.left = position;
		position = region.top;
		position += (region.Height() - display->TextHeight(text, screenID, font)) / 2;
		if (position > region.top)
			region.top = position;
#if defined(ZIL_3D_BORDER)
		if (Inherited(ID_TITLE) && !FlagSet(parent->woStatus, WOS_CURRENT))
		{
			region.right = region.left + display->TextWidth(text, screenID, font) - 1;
			display->Rectangle(screenID, region.left - 1, region.top,
				region.right + 1, region.top, palette, 0, TRUE, FALSE, &clip);
		}
#endif
		display->Text(screenID, region.left, region.top, text, palette, -1, FALSE, FALSE, &clipRegion, font);
#if !defined(ZIL_GRAPHICS_ONLY)
		if (display->isText && !FlagSet(font, FNT_IGNORE_UNDERSCORE))
		{
			int length = strlen(text);
			const ZIL_ICHAR *hotChar = text;
			for (int i=0; i < length; i++, hotChar++)
		 	{
				if ((*hotChar == ZIL_HOTMARK && hotChar[1] == ZIL_HOTMARK) ||
					(*hotChar == '~' && hotChar[1] == '~'))
					++hotChar, ++i;
				else if (*hotChar == ZIL_HOTMARK || *hotChar == '~')
				{
					++hotChar;
					palette = LogicalPalette(ccode, ID_HOT_KEY);
					ZIL_ICHAR *lead = strdup(text);
					lead[(int)(hotChar-text)-1] = 0;
					display->Text(screenID, region.left + display->TextWidth(lead, screenID, font),
						region.top, hotChar, palette, 1, FALSE, FALSE, &clipRegion, font);
					delete lead;
					break;
				}
			}
		}
#endif
	}
	display->VirtualPut(screenID);
	return (ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::Event(const UI_EVENT &event)
{
	static int initializedTime = FALSE;
	static ZIL_UTIME lastTime, currentTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

	// Switch on the system event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (!numberID && parent)
		{
			UI_WINDOW_OBJECT *object = NULL;
			for (object = parent; object->parent; object = object->parent)
				;
			numberID = object->NumberID() ? object->NumberID() : 1;
			object->NumberID(numberID + 1);
			if (stringID[0] == '\0')
			{
				strcpy(stringID, _genericFieldID);
				itoa(numberID, &stringID[strlen(stringID)], 10);
			}
		}
		RegionConvert(relative, (parent && !FlagSet(woFlags, WOF_NON_FIELD_REGION)) ? FALSE : TRUE);
		break;

	case S_DEINITIALIZE:
		screenID = 0;
		break;

	case S_CHANGED:
	case S_CREATE:
		woStatus |= WOS_REDISPLAY;
		if (!screenID)
		{
			if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT | WOAF_TEMPORARY))
				screenID = ++(windowManager->currentScreenID);
			else
				screenID = parent->screenID;
		}
		if (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
			RegionMax(this);
		else
		{
			parent->RegionMax(this);
			if (parent->Inherited(ID_GROUP))
			{
				trueRegion.top -= display->cellHeight / 4;
				trueRegion.bottom -= display->cellHeight / 4;
			}
		}
		break;

	case S_SIZE:
		{
		WOS_STATUS _woStatus = woStatus;
		woStatus |= WOS_INTERNAL_ACTION;	// Don't call user-function.
		UI_REGION region = trueRegion;
		relative = event.region;

		UI_WINDOW_OBJECT *root = Root();
		root->Event(UI_EVENT(S_CHANGED));

		if (!parent)
		{
			display->VirtualGet(ID_SCREEN, 0, 0, display->columns, display->lines);
			display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
			UI_WINDOW_OBJECT *object = NULL;
			for (object = windowManager->Last(); object; object = object->Previous())
				object->Event(UI_EVENT(S_REGION_DEFINE));
			display->Rectangle(ID_SCREEN, region, display->backgroundPalette, 0, TRUE);
			for (object = windowManager->Last(); object; object = object->Previous())
				if (object == this)
					object->Event(UI_EVENT(S_CURRENT, 0, trueRegion));
				else if (object->trueRegion.Overlap(region))
					object->Event(UI_EVENT(S_DISPLAY_INACTIVE, 0, region));
			display->VirtualPut(ID_SCREEN);
		}
		else
		{
			if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
				parent->Event(UI_EVENT(S_REGION_DEFINE));
			{
				region.left = MinValue(region.left, trueRegion.left);
				region.top = MinValue(region.top, trueRegion.top);
				region.right = MaxValue(region.right, trueRegion.right);
				region.bottom = MaxValue(region.bottom, trueRegion.bottom);
				woStatus |= WOS_REDISPLAY;
				parent->Event(UI_EVENT(S_CURRENT, 0, region));
			}
		}
		if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
			woStatus &= ~WOS_INTERNAL_ACTION;
		}
		break;

#if !defined(ZIL_GRAPHICS_ONLY)
	case S_RESET_DISPLAY:
		{
		UI_REGION region;
		region = relative;
		if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS) && FlagSet(woFlags, WOF_MINICELL))
		{
#	if !defined(ZIL_TEXT_ONLY)
			long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
			long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
			region.left = (int)((miniDX * region.left) / (miniNX * display->cellWidth));
			region.top = (int)((miniDY * region.top) / (miniNY * display->cellHeight));
			region.right = (int)((miniDX * region.right) / (miniNX * display->cellWidth));
			region.bottom = (int)((miniDY * region.bottom) / (miniNY * display->cellHeight));
			woStatus &= ~WOS_GRAPHICS;
			woStatus |= WOS_REDISPLAY;
#	endif
		}
		else if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS))
		{
#	if !defined(ZIL_TEXT_ONLY)
			int width = (region.right - region.left) / display->cellWidth;
			int height = (region.bottom - region.top) / display->cellHeight;
			region.left /= display->cellWidth;
			region.bottom /= display->cellHeight;
			region.right = region.left + width;
			region.top = region.bottom - height;
			woStatus &= ~WOS_GRAPHICS;
			woStatus |= WOS_REDISPLAY;
#	endif
		}
		relative = region;
		trueRegion = relative;
		screenID = 0;
		}
		break;
#endif

	case S_REGION_DEFINE:
		display->RegionDefine(screenID, trueRegion);
		break;

	case S_CURRENT:
		if (parent && FlagSet(parent->woStatus, WOS_REDISPLAY))
			woStatus |= WOS_REDISPLAY;
		// Continue to S_NON_CURRENT.
	case S_NON_CURRENT:
		// Check the object status.
		if (ccode == S_CURRENT)
		{
			if (!FlagSet(woStatus, WOS_CURRENT))
				woStatus |= WOS_REDISPLAY;
			woStatus |= WOS_CURRENT;
		}
		else
		{
			if (FlagSet(woStatus, WOS_CURRENT))
				woStatus |= WOS_REDISPLAY;
			woStatus &= ~WOS_CURRENT;
		}
		// Continue to S_DISPLAY_ACTIVE.
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		{
		UI_PALETTE *palette = LogicalPalette(ccode);
		if (FlagSet(woStatus, WOS_REDISPLAY) || palette != lastPalette ||
			(ccode != S_NON_CURRENT && trueRegion.Overlap(event.region)))
		{
			woStatus |= WOS_REDISPLAY;
			lastPalette = palette;
		}

		// Set list background to normal list item color.
		if (Inherited(ID_LIST))
			lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_ANY, ID_LIST_ITEM);

		// Call the associated user or validate function.
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION) &&
			(ccode == S_CURRENT || ccode == S_NON_CURRENT))
			ccode = UserFunction(event, ccode);
		if (FlagsSet(woStatus, WOS_OWNERDRAW | WOS_REDISPLAY))
		{
			display->VirtualGet(screenID, trueRegion);
			if (DrawItem(event, ccode))
				woStatus &= ~WOS_REDISPLAY;
			display->VirtualPut(screenID);
		}
		}
		break;

	case S_REDISPLAY:
		if (screenID)
		{
			// Find the root to determine activity.
			UI_EVENT event;
			event.type = RedisplayType();
			event.region = trueRegion;
			WOS_STATUS _woStatus = woStatus;
			woStatus |= WOS_REDISPLAY | WOS_INTERNAL_ACTION;	// Don't call user-function.
			Event(event);
			if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
				woStatus &= ~WOS_INTERNAL_ACTION;
		}
		else if (parent && parent->screenID)
			parent->Event(UI_EVENT(parent->RedisplayType(), 0, trueRegion));

		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		if (parent)
			parent->Event(event);
		break;

	case L_LEFT:
	case L_RIGHT:
	case L_UP:
	case L_DOWN:
		if (parent && !parent->Inherited(ID_TABLE_RECORD))
		{
			UI_WINDOW_OBJECT *object;
			UI_WINDOW_OBJECT *nObject = ZIL_NULLP(UI_WINDOW_OBJECT);
			UI_WINDOW_OBJECT *wObject = ZIL_NULLP(UI_WINDOW_OBJECT);
			UI_WINDOW_OBJECT *wwObject = ZIL_NULLP(UI_WINDOW_OBJECT);

			parent->Information(I_GET_FIRST, &object);
			for (; object; object = object->Next())
			{
				// Is it another object that can get focus.
				if (object == this ||
					FlagSet(object->woFlags, WOF_NON_SELECTABLE) ||
					FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
						continue;

				// Is it in the wrong direction.
				if ((ccode == L_LEFT &&
						(object->trueRegion.top > trueRegion.bottom ||
						(object->trueRegion.bottom >= trueRegion.top && object->trueRegion.left > trueRegion.left))) ||
					(ccode == L_RIGHT &&
						(object->trueRegion.bottom < trueRegion.top ||
						(object->trueRegion.top <= trueRegion.bottom && object->trueRegion.right < trueRegion.right))) ||
						(ccode == L_UP &&
						(object->trueRegion.left > trueRegion.right ||
						(object->trueRegion.right >= trueRegion.left && object->trueRegion.top > trueRegion.top))) ||
					(ccode == L_DOWN &&
						(object->trueRegion.right < trueRegion.left ||
						(object->trueRegion.left <= trueRegion.right && object->trueRegion.bottom < trueRegion.bottom))))
				{
					// Is it farthest so far.
					if (!wwObject ||
						(ccode == L_LEFT &&
							(object->trueRegion.bottom > wwObject->trueRegion.bottom ||
							(object->trueRegion.bottom == wwObject->trueRegion.bottom &&
							object->trueRegion.right > wwObject->trueRegion.right))) ||
						(ccode == L_RIGHT &&
							(object->trueRegion.top < wwObject->trueRegion.top ||
							(object->trueRegion.top == wwObject->trueRegion.top &&
							object->trueRegion.left < wwObject->trueRegion.left))) ||
						(ccode == L_UP &&
							(object->trueRegion.right > wwObject->trueRegion.right ||
							(object->trueRegion.right == wwObject->trueRegion.right &&
							object->trueRegion.bottom > wwObject->trueRegion.bottom))) ||
						(ccode == L_DOWN &&
							(object->trueRegion.left < wwObject->trueRegion.left ||
							(object->trueRegion.left == wwObject->trueRegion.left &&
							object->trueRegion.top < wwObject->trueRegion.top))))
								wwObject = object;
						continue;
				}

				// Is it in correct direction.
				if ((ccode == L_LEFT && object->trueRegion.bottom > trueRegion.top) ||
					(ccode == L_RIGHT && object->trueRegion.top < trueRegion.bottom) ||
					(ccode == L_UP && object->trueRegion.right > trueRegion.left) ||
					(ccode == L_DOWN && object->trueRegion.left < trueRegion.right))
				{
					// Is it closest so far.
					if (!nObject ||
						(ccode == L_LEFT && object->trueRegion.right > nObject->trueRegion.right) ||
						(ccode == L_RIGHT && object->trueRegion.left < nObject->trueRegion.left) ||
						(ccode == L_UP && object->trueRegion.bottom > nObject->trueRegion.bottom) ||
						(ccode == L_DOWN && object->trueRegion.top < nObject->trueRegion.top))
							nObject = object;
				}
				else if (!nObject)
				{
					// Is it closest so far.
					if (!wObject ||
						(ccode == L_LEFT &&
							(object->trueRegion.bottom > wObject->trueRegion.bottom ||
							(object->trueRegion.bottom == wObject->trueRegion.bottom &&
							object->trueRegion.right > wObject->trueRegion.right))) ||
						(ccode == L_RIGHT &&
							(object->trueRegion.top < wObject->trueRegion.top ||
							(object->trueRegion.top == wObject->trueRegion.top &&
							object->trueRegion.left < wObject->trueRegion.left))) ||
						(ccode == L_UP &&
							(object->trueRegion.right > wObject->trueRegion.right ||
							(object->trueRegion.right == wObject->trueRegion.right &&
							object->trueRegion.bottom > wObject->trueRegion.bottom))) ||
						(ccode == L_DOWN &&
							(object->trueRegion.left < wObject->trueRegion.left ||
							(object->trueRegion.left == wObject->trueRegion.left &&
							object->trueRegion.top < wObject->trueRegion.top))))
								wObject = object;
				}
			}

			WNF_FLAGS wnFlags = 0;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (nObject || (!FlagSet(wnFlags, WNF_NO_WRAP) && (wObject || wwObject)))
			{
				UI_EVENT event(S_ADD_OBJECT);
				event.windowObject = nObject ? nObject : (wObject ? wObject : wwObject);
				parent->Event(event);
			}
			else
				ccode = event.type == E_MSWINDOWS ? S_UNKNOWN : S_ERROR;
		}
		else
			ccode = S_UNKNOWN;
		break;

	case L_HELP:
		if (parent && helpContext == NO_HELP_CONTEXT)
			return (S_UNKNOWN);
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case L_VIEW:
		if (trueRegion.Overlap(event.position))
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case L_SELECT:
		ccode = UserFunction(event, ccode);
		break;

	case S_DRAG_DEFAULT:
		ccode = S_DRAG_MOVE_OBJECT;
		// Fall through to S_DRAG_MOVE_OBJECT.
	case S_DRAG_MOVE_OBJECT:
	case S_DRAG_COPY_OBJECT:
		if (!FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP) &&
			windowManager->dragObject != this)
			   eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
		else if (windowManager->dragObject->Inherited(ID_LIST))
		{
			int count = 0;
			UI_WINDOW_OBJECT *object;
			for (windowManager->dragObject->Information(I_GET_FIRST, &object);
				object; object = object->Next())
					if (FlagSet(object->woStatus, WOS_SELECTED))
						count++;
				if (count == 1)
				eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
					DM_DRAG_MOVE : DM_DRAG_COPY);
			else
			   eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
		}
		else
			eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
				DM_DRAG_MOVE : DM_DRAG_COPY);
		break;

	case S_DROP_DEFAULT:
		ccode = S_DROP_MOVE_OBJECT;
		// Fall through to S_DROP_MOVE_OBJECT.
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		{
		ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
		UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
			
		if (ccode == S_DROP_MOVE_OBJECT && dragObject->Inherited(ID_LIST))
		{
			int count = 0;
			UI_WINDOW_OBJECT *object, *sObject;
			for(dragObject->Information(I_GET_FIRST, &object); object; object = object->Next())
				if (FlagSet(object->woStatus, WOS_SELECTED))
					count++,	sObject = object;
			if (count != 1)
				break;

			UI_EVENT dEvent(S_SUBTRACT_OBJECT);
			dEvent.windowObject = sObject;
			dragObject->Event(dEvent);
			dragObject->Event(UI_EVENT(S_REDISPLAY));
			sObject->Information(I_GET_TEXT, &text);
			if (text)
				Information(I_SET_TEXT, text);
			delete sObject;
		}
		else if (ccode == S_DROP_MOVE_OBJECT && dragObject->parent &&
				dragObject->parent->Inherited(ID_LIST))
		{
			UI_WINDOW_OBJECT *list = dragObject->parent;
			UI_EVENT dEvent(S_SUBTRACT_OBJECT);
			dEvent.windowObject = dragObject;
			list->Event(dEvent);
		 	list->Event(UI_EVENT(S_REDISPLAY));
		 	dragObject->Information(I_GET_TEXT, &text);
			if (text)
				Information(I_SET_TEXT, text);
			delete dragObject;
		}
		else
		{
			dragObject->Information(I_GET_TEXT, &text);
			if (text)
				Information(I_SET_TEXT, text);
			if (ccode == S_DROP_MOVE_OBJECT)
				dragObject->Information(I_SET_TEXT, &_blankString);
		}
		}
		break;

	case L_DOUBLE_CLICK:
		if (userFunction)
		{
			UI_EVENT tEvent = event;
			ccode = (*userFunction)(this, tEvent, ccode);
		}
		break;

	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
	case L_CONTINUE_COPY_DRAG:
	case L_END_COPY_DRAG:
		{
		if (windowManager->dragObject)
		{
			if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP) &&
				(ccode == L_END_SELECT || ccode == L_END_COPY_DRAG) &&
				windowManager->dragObject != this)
			{
				EVENT_TYPE tCode;
				if (ccode == L_END_SELECT)
				{
					if (FlagsSet(windowManager->dragObject->woAdvancedFlags,
						WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
						tCode = S_DROP_DEFAULT;
					else if (FlagSet(windowManager->dragObject->woAdvancedFlags,
						WOAF_MOVE_DRAG_OBJECT))
						tCode = S_DROP_MOVE_OBJECT;
					else
						tCode = S_DROP_COPY_OBJECT;
				}
				else
					tCode = ccode == L_END_MOVE_DRAG ?
					S_DROP_MOVE_OBJECT : S_DROP_COPY_OBJECT;

				if (!userFunction || UserFunction(event, tCode) == S_UNKNOWN)
					Event(UI_EVENT(tCode, 0, event.position));
			}

			else if
				(ccode == L_CONTINUE_SELECT || ccode == L_CONTINUE_COPY_DRAG)
			{
				if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP) ||
					windowManager->dragObject == this)
				{
					EVENT_TYPE tCode;
					if (FlagsSet(windowManager->dragObject->woAdvancedFlags,
						WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
					{
						tCode = (ccode == L_CONTINUE_SELECT) ?
							S_DRAG_MOVE_OBJECT :
							ccode == L_CONTINUE_COPY_DRAG ?
							S_DRAG_COPY_OBJECT : S_DRAG_DEFAULT;
					}
					else if (FlagSet(windowManager->dragObject->woAdvancedFlags,
						WOAF_MOVE_DRAG_OBJECT))
						tCode = S_DRAG_MOVE_OBJECT;
					else
						tCode = S_DRAG_COPY_OBJECT;

					if (windowManager->dragObject == this || !userFunction ||
						UserFunction(event, tCode) == S_UNKNOWN)
						Event(UI_EVENT(tCode, 0, event.position));
				}
				else
					eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
			}
			else
				eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
		break;
		}

		if (ccode == L_BEGIN_SELECT &&
			FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
			windowManager->dragObject = this;

		UI_EVENT dEvent;
		if (!trueRegion.Overlap(event.position) || !clip.Overlap(event.position))
			ccode = S_UNKNOWN;
#if defined(ZIL_EDIT)
		if (ccode == L_END_SELECT && !parent && FlagSet(woStatus, WOS_EDIT_MODE))
		{
			currentTime.Import();
			if (currentTime.Difference(lastTime) < doubleClickRate)
			{
				dEvent.type = D_EDIT_OBJECT;
				dEvent.rawCode = searchID;
				dEvent.data = this;
				eventManager->Put(dEvent);
			}
			else
				lastTime = currentTime;
			break;
		}
#endif

		// Check for an edit mode object.
		if (ccode != L_BEGIN_SELECT || !FlagSet(woStatus, WOS_EDIT_MODE))
			break;

#if defined(ZIL_EDIT)
		dEvent.position = event.position;
		int hOffset = 1, vOffset = 0;
		if (!display->isText)
			hOffset = vOffset = (trueRegion.Width() > 10) ? 5 : 1;
		else if (trueRegion.right == trueRegion.left)
			hOffset = 0;
		if (event.position.column >= trueRegion.left + hOffset && event.position.line >= trueRegion.top + vOffset &&
			event.position.column <= trueRegion.right - hOffset && event.position.line <= trueRegion.bottom - vOffset)
		{
			dEvent.type = L_MOVE;
			dEvent.rawCode = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		}
		else
		{
			dEvent.type = L_SIZE;
			dEvent.rawCode = 0;
			if (event.position.column <= trueRegion.left + hOffset)
				dEvent.rawCode |= M_LEFT_CHANGE;
			else if (event.position.column >= trueRegion.right - hOffset)
				dEvent.rawCode |= M_RIGHT_CHANGE;
			if (event.position.line <= trueRegion.top + vOffset)
				dEvent.rawCode |= M_TOP_CHANGE;
			else if (event.position.line >= trueRegion.bottom - vOffset)
				dEvent.rawCode |= M_BOTTOM_CHANGE;
		}
		if (parent)
			Modify(dEvent);

		// Set the current object.
		dEvent.type = D_SET_OBJECT;
		dEvent.rawCode = searchID;
		dEvent.data = this;
		eventManager->Put(dEvent);
		dEvent.type = D_SET_POSITION;
		dEvent.rawCode = M_LEFT | M_LEFT_CHANGE;
		dEvent.position = event.position;
		eventManager->Put(dEvent);
		if (!parent)
			break;
		currentTime.Import();
		if (currentTime.Difference(lastTime) < doubleClickRate)
		{
			dEvent.type = D_EDIT_OBJECT;
			dEvent.rawCode = searchID;
			dEvent.data = this;
			eventManager->Put(dEvent);
		}
		else
			lastTime = currentTime;

#endif
		}
		break;

#if defined(ZIL_EDIT)
	case L_BEGIN_ESCAPE:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			UI_EVENT dEvent;
			dEvent.type = D_SET_POSITION;
			dEvent.rawCode = event.rawCode;
			dEvent.position = event.position;
			eventManager->Put(dEvent);
			break;
		}
		// Continue to default.
#endif
	default:
		ccode = S_UNKNOWN;
		break;
	}

	// Return the control code.
	return (ccode);
}

void UI_WINDOW_OBJECT::Modify(const UI_EVENT &event)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
#endif

	static UI_PALETTE rubberBandPalette =
	{
		'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
		PTN_SOLID_FILL, LIGHTCYAN, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
	};

	// Make sure we can move the object.
	if (event.type == L_SIZE && Inherited(ID_ICON))
		return;

	WNF_FLAGS wnFlags = WNF_NO_FLAGS;
	Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);

	if (parent && (FlagSet(woFlags, WOF_NON_FIELD_REGION) &&
		!Inherited(ID_TABLE_HEADER) && !Inherited(ID_STATUS_BAR) &&
		!(Inherited(ID_TOOL_BAR) && FlagSet(wnFlags, WNF_NO_WRAP))))
		return;

	// Initialize the modify variables.
	UI_EVENT mEvent; mEvent.type = 0;
	UI_REGION newRegion = trueRegion, oldRegion = trueRegion;
	ZIL_RAW_CODE sizeFlags = event.rawCode;
	UI_POSITION origin = event.position;
	EVENT_TYPE operation = L_SIZE;
	if (event.type == L_MOVE || Inherited(ID_ICON))
	{
		sizeFlags = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		operation = L_MOVE;
	}
	else if (!event.rawCode)
		sizeFlags = M_RIGHT_CHANGE | M_BOTTOM_CHANGE;

	// Determine the absolute region.
	UI_REGION absolute, clipRegion;

	if (parent)
	{
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			absolute = clipRegion = parent->trueRegion;
		else
		{
			absolute = clip;
			parent->Information(I_GET_CLIPREGION, &clipRegion);
		}
	}
	else
	{
		absolute.left = absolute.top = 0;
		absolute.right = display->columns - 1;
		absolute.bottom = display->lines - 1;
		clipRegion = absolute;
	}

	// Determine the minimum height and width of the object.
	int minHeight = display->cellHeight;
	if (parent && (!Inherited(ID_WINDOW) || Inherited(ID_LIST) || Inherited(ID_TABLE_RECORD) ||
		Inherited(ID_TABLE_HEADER) || Inherited(ID_TOOL_BAR) || Inherited(ID_STATUS_BAR)))
		minHeight -= (display->preSpace + display->postSpace);
	else if (!display->isText)
		minHeight += 2 * UIW_BORDER::width;
	int minWidth = 2 * display->cellWidth;
	if (Inherited(ID_WINDOW) && !Inherited(ID_SCROLL_BAR)
		&& !Inherited(ID_LIST) && !Inherited(ID_GROUP) && !Inherited(ID_TABLE_RECORD) &&
			!Inherited(ID_TABLE_HEADER) && !Inherited(ID_TOOL_BAR) && !Inherited(ID_STATUS_BAR))
		minWidth *= 6;

	// Check for special object movement.
	if (operation != L_MOVE)
	{
		if (Inherited(ID_BUTTON))
		{
			BTF_FLAGS btFlags;
			Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
			if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_RADIO_BUTTON | BTF_CHECK_BOX))
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		}
		else if ((!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX) ||
			Inherited(ID_SPIN_CONTROL)) && searchID < 3000)
			sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		else if (Inherited(ID_SCROLL_BAR))
		{
			SBF_FLAGS sbFlags = SBF_NO_FLAGS;
			Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
			if (!FlagSet(sbFlags, SBF_VERTICAL))
			{
				minHeight = trueRegion.bottom - trueRegion.top + 1;
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
			}
			if (!FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				minWidth = trueRegion.right - trueRegion.left + 1;
				sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
			}
			if (!sizeFlags)
				return;
		}
		else if (Inherited(ID_TOOL_BAR))
			sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_RIGHT_CHANGE;
		else if (Inherited(ID_STATUS_BAR))
			sizeFlags &= ~M_LEFT_CHANGE & ~M_BOTTOM_CHANGE & ~M_RIGHT_CHANGE;
		else if (Inherited(ID_TABLE_RECORD))
		{
			sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE;
			if (parent->Inherited(ID_TABLE_HEADER))
			{
				THF_FLAGS thFlags;
				parent->Information(I_GET_FLAGS, &thFlags);
				if (FlagSet(thFlags, THF_COLUMN_HEADER))
					sizeFlags &= ~M_BOTTOM_CHANGE;
				else if (FlagSet(thFlags, THF_ROW_HEADER))
					sizeFlags &= ~M_RIGHT_CHANGE;
				else
					sizeFlags = 0;
			}
		}
		else if (Inherited(ID_TABLE_HEADER))
		{
			THF_FLAGS thFlags;
			Information(I_GET_FLAGS, &thFlags);
			if (FlagSet(thFlags, THF_COLUMN_HEADER))
				sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_RIGHT_CHANGE;
			else if (FlagSet(thFlags, THF_ROW_HEADER))
				sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_BOTTOM_CHANGE;
			else
				sizeFlags = 0;
		}
	}
	else if (Inherited(ID_TABLE_RECORD) || Inherited(ID_TABLE_HEADER))
		return;

	// Reverse the window region.
	int xJump = display->cellWidth;
	int yJump = display->cellHeight;
	EVENT_TYPE ccode = S_UNKNOWN;

	// Prevent sizing illusion caused by double XOR with mouse cursor.
	if (display->isText)
		eventManager->DeviceState(E_MOUSE, D_HIDE);
	else if (!event.rawCode)
		eventManager->DeviceState(E_MOUSE, DM_MOVE);

	int deltaX = 0, deltaY = 0;
	UI_PALETTE *oldPalette = display->xorPalette;
	display->xorPalette = &rubberBandPalette;
	display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE, &clipRegion);
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		ccode = UI_EVENT_MAP::MapEvent(eventMapTable, event);
		switch (ccode)
		{
		case L_BEGIN_SELECT:
			if (event.position.column < newRegion.left ||
			    event.position.column > newRegion.right ||
			    event.position.line < newRegion.top ||
			    event.position.line > newRegion.bottom)
			{
				ccode = L_END_SELECT;
				break;
			}
			origin = event.position;
			// Continue to L_CONTINUE_SELECT.
		case L_CONTINUE_SELECT:
			mEvent = event;
			mEvent.type = L_VIEW;
			if (event.position.column > absolute.right)
				event.position.column = absolute.right;
			else if (event.position.column < absolute.left)
				event.position.column = absolute.left;
			if (event.position.line > absolute.bottom)
				event.position.line = absolute.bottom;
			else if (event.position.line < absolute.top)
				event.position.line = absolute.top;
			deltaX = event.position.column - origin.column;		// Compute the change.
			deltaY = event.position.line - origin.line;
			break;

		case L_END_SELECT:
			break;

		case L_UP:
		case L_DOWN:
			deltaY = (ccode == L_UP) ? deltaY - yJump : deltaY + yJump;
			if (trueRegion.bottom + deltaY < absolute.top ||
				trueRegion.top + deltaY > absolute.bottom)
				deltaY = (ccode == L_UP) ? deltaY + yJump : deltaY - yJump;
			break;

		case L_LEFT:
		case L_RIGHT:
			deltaX = (ccode == L_LEFT) ? deltaX - xJump : deltaX + xJump;
			if (trueRegion.right + deltaX < absolute.left ||
				trueRegion.left + deltaX > absolute.right)
				deltaX = (ccode == L_LEFT) ? deltaX + xJump : deltaX - xJump;
			break;

		default:
			// Send all user messages to the window manager.
			if (event.type > 9999)
				windowManager->Event(event);
			break;
		}

#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			// Check the absolute region.
			if (FlagSet(sizeFlags, M_LEFT_CHANGE))
			{
				if (trueRegion.left + deltaX < absolute.left)
					deltaX = absolute.left - trueRegion.left;
			}
			if (FlagSet(sizeFlags, M_TOP_CHANGE))
			{
				if (trueRegion.top + deltaY < absolute.top)
					deltaY = absolute.top - trueRegion.top;
			}
		}
		
		long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
		long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

		// Check for a cell boundary move or size.
		if (FlagSet(woStatus, WOS_EDIT_MODE) && FlagSet(woFlags, WOF_MINICELL))
		{
			long value = deltaX;
			int ceil = (value >= 0) ? (int)(miniDX - 1) : int(1 - miniDX);
			value = (value * miniDX) / (miniNX * display->cellWidth);
			value = (value * miniNX * display->cellWidth + ceil) / miniDX;
			deltaX = (int)value;

			value = deltaY;
			ceil = (value >= 0) ? (int)(miniDY - 1) : (int)(1 - miniDY);
			value = (value * miniDY) / (miniNY * display->cellHeight);
			value = (value * miniNY * display->cellHeight + ceil) / miniDY;
			deltaY = (int)value;
		}
		else if (FlagSet(woStatus, WOS_EDIT_MODE) && !FlagSet(woFlags, WOF_PIXEL))
		{
			int value = (deltaX > 0) ? deltaX + display->cellWidth / 2 : deltaX - display->cellWidth / 2;
			deltaX = value / display->cellWidth * display->cellWidth;
			value = (deltaY > 0) ? deltaY + display->cellHeight / 2 : deltaY - display->cellHeight / 2;
			deltaY = value / display->cellHeight * display->cellHeight;
		}
#endif

		if (FlagSet(sizeFlags, M_LEFT_CHANGE))
			newRegion.left = trueRegion.left + deltaX;
		if (FlagSet(sizeFlags, M_TOP_CHANGE))
			newRegion.top = trueRegion.top + deltaY;
		if (FlagSet(sizeFlags, M_RIGHT_CHANGE))
			newRegion.right = trueRegion.right + deltaX;
		if (FlagSet(sizeFlags, M_BOTTOM_CHANGE))
			newRegion.bottom = trueRegion.bottom + deltaY;

		// Check for minimum size.
		if (operation == L_SIZE)
		{
			if (newRegion.left + minWidth - 1 > newRegion.right &&
			  !FlagSet(woStatus, WOS_MINIMIZED))
			{
				newRegion.left = oldRegion.left;
				newRegion.right = oldRegion.right;
			}
			if (newRegion.top + minHeight - 1 > newRegion.bottom &&
			  !FlagSet(woStatus, WOS_MINIMIZED))
			{
				newRegion.top = oldRegion.top;
				newRegion.bottom = oldRegion.bottom;
			}
		}
		// Move sizing rectangle.
		if (oldRegion != newRegion)
		{
			display->RectangleXORDiff(oldRegion, newRegion, 0, &clipRegion);
			oldRegion = newRegion;
		}
	} while (ccode != L_END_SELECT && ccode != L_SELECT && ccode != L_CANCEL);

	// Restore the region.
	display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE, &clipRegion);
	display->xorPalette = oldPalette;

	// Restore mouse cursor.
 	if (display->isText)
		eventManager->DeviceState(E_MOUSE, D_ON);
	else if (eventManager->DeviceState(E_MOUSE, D_STATE) == DM_MOVE)
		eventManager->DeviceState(E_MOUSE, DM_VIEW);

	if (ccode == L_END_SELECT && !Inherited(ID_ICON))
		eventManager->Put(UI_EVENT(L_END_SELECT));

	if (ccode == L_CANCEL || newRegion == trueRegion)
		return;				// Do not change the object region.

	relative.left += newRegion.left - trueRegion.left;
	relative.top += newRegion.top - trueRegion.top;
	relative.right += newRegion.right - trueRegion.right;
	relative.bottom += newRegion.bottom - trueRegion.bottom;

	if (Inherited(ID_TABLE_HEADER) && !Inherited(ID_TABLE_RECORD))
	{
		UI_WINDOW_OBJECT *tableRecord;
		Information(I_GET_FIRST, &tableRecord);
		if (tableRecord)
		{
			tableRecord->relative.right += newRegion.right - trueRegion.right;
			tableRecord->relative.bottom += newRegion.bottom - trueRegion.bottom;
		}
		parent->Event(UI_EVENT(S_REDISPLAY));
	}

	UI_REGION region = trueRegion;
	if (FlagSet(woStatus, WOS_MINIMIZED))
	{
		// Reset the icon region if window iconized.
		UI_WINDOW_OBJECT *icon;
		Information(I_GET_ICON, &icon);
		if (icon)
			icon->relative = relative;
		Event(UI_EVENT(S_CREATE));
	}
	else
	{
		if (Inherited(ID_STATUS_BAR))
			Event(UI_EVENT(S_SIZE, 0, relative));
		Root()->Event(UI_EVENT(S_CHANGED));
		if (parent && parent->Inherited(ID_TOOL_BAR))
			Root()->Event(UI_EVENT(S_REDISPLAY));
	}

	WOS_STATUS _woStatus = woStatus;
	woStatus |= WOS_INTERNAL_ACTION;	// Don't call user-function.
	if (!parent)
	{
		if (this != windowManager->First())
			windowManager->Add(this);

		display->VirtualGet(ID_SCREEN, 0, 0, display->columns, display->lines);
		display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
		UI_WINDOW_OBJECT *object = NULL;
		for (object = windowManager->Last(); object; object = object->Previous())
			object->Event(UI_EVENT(S_REGION_DEFINE));
		display->Rectangle(ID_SCREEN, region, display->backgroundPalette, 0, TRUE);
		for (object = windowManager->Last(); object; object = object->Previous())
			if (object == this)
				object->Event(UI_EVENT(S_CURRENT, 0, trueRegion));
			else if (object->trueRegion.Overlap(region))
			{				
				object->Event(UI_EVENT(S_DISPLAY_INACTIVE, 0, region));
#if !defined(ZIL_GRAPHICS_ONLY)
				if (display->isText)	// Account for window shadowing.
				{
					region.left = MinValue(object->trueRegion.left, region.left);
					region.top = MinValue(object->trueRegion.top, region.top);
					region.right = MaxValue(object->trueRegion.right + 1, region.right);
					region.bottom = MaxValue(object->trueRegion.bottom + 1, region.bottom);
				}
#endif
			}
		display->VirtualPut(ID_SCREEN);
	}
	else
	{
		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			parent->Event(UI_EVENT(S_REGION_DEFINE));
		region.left = MinValue(region.left, trueRegion.left);
		region.top = MinValue(region.top, trueRegion.top);
		region.right = MaxValue(region.right, trueRegion.right);
		region.bottom = MaxValue(region.bottom, trueRegion.bottom);
		woStatus |= WOS_REDISPLAY;
		if ((parent->Inherited(ID_GROUP) || FlagSet(woStatus, WOS_EDIT_MODE))
			&& !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			parent->Event(UI_EVENT(S_REDISPLAY));
		else
			parent->Event(UI_EVENT(S_CURRENT, 0, region));
	}

	if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
		woStatus &= ~WOS_INTERNAL_ACTION;
	if (mEvent.type)
		eventManager->Put(mEvent, Q_BEGIN);
}

// ----- OS Specific Functions ----------------------------------------------

void UI_WINDOW_OBJECT::OSInitialize(void)
{
}

void UI_WINDOW_OBJECT::OSUpdateSettings(ZIL_OBJECTID)
{
	if (screenID)
		woStatus |= WOS_REDISPLAY;
	if (parent && parent->screenID && Inherited(ID_LIST_ITEM))
		parent->Event(UI_EVENT(S_CHANGED));
}
