//	Zinc Interface Library - D_BUTTON.CPP
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

// ----- UIW_BUTTON ---------------------------------------------------------

EVENT_TYPE UIW_BUTTON::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
	UI_PALETTE *lastPalette = this->lastPalette;
#endif
	const int BUTTON_OFFSET = Inherited(ID_LIST_ITEM) ? 4 :
		(!FlagSet(btFlags, BTF_NO_3D) || FlagSet(woFlags, WOF_BORDER)) ? 2 : 0;
	UI_REGION region = trueRegion;

	display->VirtualGet(screenID, trueRegion);

	// Handle text mode buttons.
	if (display->isText)
	{
#if !defined(ZIL_GRAPHICS_ONLY)
		// Check for support scroll bar thumb button.
		if (parent && parent->Inherited(ID_SCROLL_BAR) && FlagSet(parent->woFlags, WOF_SUPPORT_OBJECT))
			lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_ANY, ID_BUTTON);
	 	else if (Inherited(ID_LIST_ITEM))
		{
			if ((Inherited(ID_CHECK_BOX) || Inherited(ID_RADIO_BUTTON)) &&
				!FlagSet(woFlags, WOF_NON_SELECTABLE))
				lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable,
					ccode == S_CURRENT ? PM_CURRENT : PM_ANY, ID_LIST_ITEM);
			else
				lastPalette = LogicalPalette(ccode);
		}
		else if (FlagSet(woStatus, WOS_SELECTED) &&
			FlagSet(btFlags, BTF_NO_TOGGLE | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
		{
			if (FlagSet(woStatus, WOS_CURRENT) && !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_CURRENT, ID_BUTTON);
			else
				lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_ANY, ID_BUTTON);
		}
		else
			lastPalette = LogicalPalette(ccode);

#	if defined(ZIL_3D_BORDER)
		if (FlagSet(woFlags, WOF_BORDER) && !FlagSet(btFlags, BTF_NO_3D))
			DrawBorder(screenID, region, FALSE, ccode);

		if (!FlagSet(btFlags, BTF_NO_3D))
			DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth, TRUE, ccode);
		else
			display->Rectangle(screenID, trueRegion, lastPalette, 0, TRUE, FALSE, &clip);
#	else
		if (FlagSet(woFlags, WOF_BORDER) && !FlagSet(btFlags, BTF_AUTO_SIZE))
			DrawBorder(screenID, region, FALSE, ccode);
		DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth, TRUE, ccode);
#	endif

		int rightJustify = FlagSet(woFlags, WOF_JUSTIFY_RIGHT) ? TRUE : FALSE;
		if (FlagSet(btFlags, BTF_CHECK_BOX))
		{
			display->Text(screenID, rightJustify ? region.right - 2 : region.left, region.top,
				myDecorations->GetText(FlagSet(woStatus, WOS_SELECTED) ? ZIL_CheckSelected : ZIL_CheckNormal),
				lastPalette, -1, TRUE, FALSE, &clip);
			if (rightJustify)
				region.right -= 4;
			else
				region.left += 4;
		}
		else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
		{
			display->Text(screenID, rightJustify ? region.right - 2 : region.left, region.top,
				myDecorations->GetText(FlagSet(woStatus, WOS_SELECTED) ? ZIL_RadioSelected : ZIL_RadioNormal),
				lastPalette, -1, TRUE, FALSE, &clip);
			if (rightJustify)
				region.right -= 4;
			else
				region.left += 4;
		}
		if (parent && parent->Inherited(ID_VT_LIST) && relative.left)
		{
			int left = region.left - relative.left;
			display->Rectangle(screenID, left, region.top, region.right,
				region.bottom, lastPalette, 0, TRUE, FALSE, &clip);
			if (text)
				DrawText(screenID, region, text, lastPalette, FALSE, ccode);
		}
		else if (text)
			DrawText(screenID, region, text, lastPalette, TRUE, ccode);
#	endif
	}
	else if (depth > 1 && event.type == E_MOUSE)
	{
#if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
	region -= FlagSet(woFlags, WOF_BORDER) ? 2 : 1;
	if (FlagSet(btStatus, BTS_DEFAULT) || (ccode == S_CURRENT &&
		!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) &&
		!FlagSet(woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT) &&
		!(parent	&& parent->Inherited(ID_SCROLL_BAR))))
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
		display->Rectangle(screenID, region, outline, 1, FALSE, FALSE, &clip);
		--region;
 	}
	if (!FlagSet(btStatus, BTS_DEPRESSED) && !FlagSet(btFlags, BTF_NO_TOGGLE) &&
		FlagSet(woStatus, WOS_SELECTED) && !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		DrawShadow(screenID, region, -2, FALSE, ccode);
	else if (FlagSet(btStatus, BTS_DEPRESSED))
	{
		UI_PALETTE *colorPalette = &display->colorMap[display->MapColor(lastPalette, FALSE)];
		UI_PALETTE *shadowPalette = LogicalPalette(ccode, ID_DARK_SHADOW);
		for (int i = 0; i < depth - 1; i++)
		{
			display->Line(screenID, region.left+i, region.bottom-i, region.right-i,
				region.bottom-i, colorPalette, 1, FALSE, &clip);
			display->Line(screenID, region.right+i, region.top+i, region.right+i,
				region.bottom-i, colorPalette, 1, FALSE, &clip);
			display->Line(screenID, region.left+i, region.top+i, region.right,
				region.top+i, shadowPalette, 1, FALSE, &clip);
			display->Line(screenID, region.left+i, region.top+i, region.left+i,
				region.bottom, shadowPalette, 1, FALSE, &clip);
			--region;
		}
		display->Rectangle(screenID, region, colorPalette, 1, FALSE, FALSE, &clip);
		--region;
	}
	else
		DrawShadow(screenID, region, depth, FALSE, ccode);
#elif defined (ZIL_MOTIF_STYLE)
	UI_REGION tRegion = region;
	if (FlagSet(btStatus, BTS_DEFAULT))
		tRegion -= 3;
	if (!FlagSet(btStatus, BTS_DEPRESSED) && !FlagSet(btFlags, BTF_NO_TOGGLE) &&
		FlagSet(woStatus, WOS_SELECTED) && !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		DrawShadow(screenID, tRegion, -depth, FALSE, ccode);
	else
		DrawShadow(screenID, tRegion, FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth, FALSE, ccode);

	if (depth > 1 && FlagSet(btStatus, BTS_DEFAULT) && ccode != S_CURRENT)
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_DARK_SHADOW);
		display->Rectangle(screenID, trueRegion, outline, 2, FALSE, FALSE, &clip);
	}
#endif
	}
	else
	{
	// Draw the border and shadow.
#if !defined(ZIL_TEXT_ONLY)
#if defined(ZIL_MSWINDOWS_STYLE)
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
#elif defined(ZIL_OS2_STYLE)
	if (parent->Inherited(ID_COMBO_BOX) && !FlagSet(btFlags, BTF_NO_3D))
		DrawBorder(screenID, region, FALSE, ccode);
#endif

#if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
	int extraBorder = 0;
	if (depth > 1)
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
		display->Line(screenID, region.left + 1, region.top,
			region.right - 1, region.top, outline, 1, FALSE, &clip);
		display->Line(screenID, region.left + 1, region.bottom,
			region.right - 1, region.bottom, outline, 1, FALSE, &clip);
		display->Line(screenID, region.left, region.top + 1,
			region.left, region.bottom - 1, outline, 1, FALSE, &clip);
		display->Line(screenID, region.right, region.top + 1,
			region.right, region.bottom - 1, outline, 1, FALSE, &clip);
		--region;

		if (FlagSet(btStatus, BTS_DEFAULT) || (ccode == S_CURRENT &&
			!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) &&
			!FlagSet(woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT) &&
			!(parent	&& parent->Inherited(ID_SCROLL_BAR))))
		{
			extraBorder = 1;
			display->Rectangle(screenID, region, outline, 1, FALSE, FALSE, &clip);
			--region;
		}
	}
#endif

	UI_PALETTE *backgroundPalette = ZIL_NULLP(UI_PALETTE);

 	if (FlagSet(btFlags, BTF_NO_3D))
	{
		if (Inherited(ID_STATUS_ITEM))
			lastPalette = LogicalPalette(ccode, ID_STATUS_ITEM);
 		else if (Inherited(ID_LIST_ITEM))
		{
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
				!FlagSet(woFlags, WOF_NON_SELECTABLE))
				lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable,
					PM_ANY, ID_LIST_ITEM);
			else
				lastPalette = LogicalPalette(ccode);
		}
		else
		{
			UI_WINDOW_OBJECT *oObject = NULL;
			for (oObject = parent; oObject &&
				display->MapColor(oObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE) == BACKGROUND;
				oObject = oObject->parent)
						;
			WOF_FLAGS saveFlags = oObject->woFlags;
			oObject->woFlags = woFlags;
			lastPalette = oObject->LogicalPalette(ccode);
			oObject->woFlags = saveFlags;
			backgroundPalette = lastPalette;
		}
	}
	else
		lastPalette = LogicalPalette(ccode);

	int drawDefault = ccode == S_HIDE_DEFAULT || ccode == S_SHOW_DEFAULT;

#if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
	if (FlagSet(btFlags, BTF_NO_3D | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
		int left = parent->Inherited(ID_VT_LIST) ? region.left - relative.left : region.left;
		display->Rectangle(screenID, left, region.top, region.right,
			region.bottom, backgroundPalette ? backgroundPalette : lastPalette, 0, TRUE, FALSE, &clip);
	}
	else if (!FlagSet(btStatus, BTS_DEPRESSED) && !FlagSet(btFlags, BTF_NO_TOGGLE) &&
		FlagSet(woStatus, WOS_SELECTED) && !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
	{
		DrawShadow(screenID, region, -2, FALSE, ccode);
		if (!drawDefault)
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	}
	else
	{
		DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -1 : depth, FALSE, ccode);
		if (!drawDefault)
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	}
	if (extraBorder)
		++region;

#elif defined(ZIL_MOTIF_STYLE)
	if (depth > 1 && FlagSet(btStatus, BTS_DEFAULT))
	{
		DrawShadow(screenID, region, -1, TRUE, ccode);
		region -= 2;
	}
	if (parent->Inherited(ID_SCROLL_BAR))
	{
		if (previous || next)
		{
			lastPalette = LogicalPalette(ccode, ID_SCROLL_BAR);
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
		}
		else
		{
			if (region.left < clip.left) region.left = clip.left;
			if (region.top < clip.top) region.top = clip.top;
			if (region.right > clip.right) region.right = clip.right;
			if (region.bottom > clip.bottom) region.bottom = clip.bottom;
			DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -2 : 2, TRUE, ccode);
		}
	}
	else if (FlagSet(btFlags, BTF_NO_3D | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
		int left = parent->Inherited(ID_VT_LIST) ? region.left - relative.left : region.left;
		display->Rectangle(screenID, left, region.top, region.right,
			region.bottom, backgroundPalette ? backgroundPalette : lastPalette, 0, TRUE, FALSE, &clip);
	}
	else if (!FlagSet(btStatus, BTS_DEPRESSED) && !FlagSet(btFlags, BTF_NO_TOGGLE) &&
		FlagSet(woStatus, WOS_SELECTED) && !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
	{
		DrawShadow(screenID, region, -depth, TRUE, ccode);
		if (!drawDefault)
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	}
	else
	{
		DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth, FALSE, ccode);
		if (!drawDefault)
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	}
#endif

#if defined (ZIL_OS2_STYLE) || defined (ZIL_MOTIF_STYLE)
	if (FlagSet(woFlags, WOF_BORDER) && FlagSet(btFlags, BTF_NO_3D) &&
		!FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_DARK_SHADOW);
		display->Rectangle(screenID, trueRegion, outline, 1, FALSE, FALSE, &clip);
	}
#endif

	// Draw the text.
	if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER))
	{
		region.left += BUTTON_OFFSET;
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			region.right -= BUTTON_OFFSET;
	}
	if (!bitmapArray && !colorBitmap && !FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
		!drawDefault && text)
		DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	// Draw the bitmap.
	int fieldWidth = region.right - region.left + 1;
	int fieldHeight = region.bottom - region.top + 1;
	ZIL_UINT8 *tempBitmap;
	if (FlagSet(btFlags, BTF_CHECK_BOX))
	{
		tempBitmap = myDecorations->GetBitmap(FlagSet(woStatus, WOS_SELECTED) ? ZIL_CheckSelected : ZIL_CheckNormal),
		bitmapWidth = tempBitmap[0];
		bitmapHeight = tempBitmap[1];
		tempBitmap = &tempBitmap[2];
	}
	else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
	{
		tempBitmap = myDecorations->GetBitmap(FlagSet(woStatus, WOS_SELECTED) ? ZIL_RadioSelected : ZIL_RadioNormal),
		bitmapWidth = tempBitmap[0];
		bitmapHeight = tempBitmap[1];
		tempBitmap = &tempBitmap[2];
	}
	int left = region.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		left = region.right - bitmapWidth + 1;
	else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		left = region.left + (fieldWidth - bitmapWidth) / 2;
	int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && text && text[0]) ?
		region.top + 2 : region.top + (fieldHeight - bitmapHeight) / 2;

#	if defined(ZIL_MOTIF_STYLE)
	if (Inherited(ID_MAXIMIZE_BUTTON) || Inherited(ID_MINIMIZE_BUTTON) ||
		Inherited(ID_SYSTEM_BUTTON))
	{
		UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
		if (!FlagSet(btStatus, BTS_DEPRESSED))
			display->Line(screenID, trueRegion.left+1, trueRegion.bottom-1, trueRegion.right-1,
				trueRegion.bottom-1, darkShadow, 1, FALSE, &clip);
	}
#	endif
	if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			tempBitmap, ZIL_NULLP(UI_PALETTE), &clip, &colorBitmap, &monoBitmap);
	else if (bitmapArray || colorBitmap && !drawDefault)
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			bitmapArray, ZIL_NULLP(UI_PALETTE), &clip, &colorBitmap, &monoBitmap);

	// Draw the text.
	if (text && text[0] && !drawDefault &&
		(bitmapArray || colorBitmap || FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON)))
	{
		WOF_FLAGS flags = woFlags;
		int offset = 0;
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
			!Inherited(ID_LIST_ITEM))
			offset = 4;

		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.top = region.bottom - display->cellHeight + 4;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		{
			region.right -= bitmapWidth - BUTTON_OFFSET - offset;
			woFlags &= ~(WOF_JUSTIFY_RIGHT);
		}
		else
			region.left += bitmapWidth + BUTTON_OFFSET + offset;

		DrawText(screenID, region, text, lastPalette, FALSE, ccode);
		woFlags = flags;
	}

	// Draw focus rectangle.
	if (Inherited(ID_LIST_ITEM) ||
		(!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) &&
		!FlagSet(woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT) &&
		!(parent->Inherited(ID_SCROLL_BAR) && FlagSet(parent->woFlags, WOF_SUPPORT_OBJECT))))
	{
		if (parent->Inherited(ID_LIST) || parent->Inherited(ID_COMBO_BOX))
		{
			region = trueRegion;
			if (parent->Inherited(ID_VT_LIST))
				region.left = region.left - relative.left;
#if !defined(ZIL_OS2_STYLE)
 			if (parent->Inherited(ID_COMBO_BOX) && !FlagSet(parent->woAdvancedFlags, WOAF_TEMPORARY))
				--region;
#endif
		}
		else if (text && text[0])
		{
			region.right = MinValue(trueRegion.right,
				region.left + display->TextWidth(text, screenID, font) + 1);
			region.left -= 2;
			region.bottom = MinValue(trueRegion.bottom,
				region.top + display->TextHeight(text, screenID, font) + 1);
			if (depth <= 1)
				region.top--, region.bottom++;
		}
		else if (bitmapArray || colorBitmap && depth)
			--region;

		if (ccode == S_CURRENT && !drawDefault)
			DrawFocus(screenID, region, ccode);
	}
	// Return the control code.
#if defined(ZIL_OPTIMIZE)
	this->lastPalette = lastPalette;
#endif
#endif
	}
	display->VirtualPut(screenID);
	return (ccode);
}

EVENT_TYPE UIW_BUTTON::Event(const UI_EVENT &event)
{
	static ZIL_UTIME lastTime, currentTime;
	static UI_WINDOW_OBJECT *lastSelected;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		// Tell the parent if BTF_DEFAULT_BUTTON.
		if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent)
		{
			UI_WINDOW_OBJECT *object;
			// Root(TRUE) goes to the MDI child or the non-MDI root.
			Root(TRUE)->Information(I_GET_DEFAULT_OBJECT, &object);
			if (!object)
			{
				Root(TRUE)->Information(I_SET_DEFAULT_OBJECT, this);
				btStatus |= BTS_DEFAULT;
			}
			else
				btStatus &= ~BTS_DEFAULT;
		}

		UI_WINDOW_OBJECT::Event(event);
		int height = Inherited(ID_LIST_ITEM) ||
			FlagSet(btFlags,BTF_AUTO_SIZE) ? 0 : relative.Height() - 1;
		if (height < display->cellHeight)
			height = display->cellHeight - display->preSpace - display->postSpace - 1;
#if !defined(ZIL_TEXT_ONLY)
		if (!display->isText)
		{
			if (FlagSet(btFlags, BTF_AUTO_SIZE))
			{
				int offset = 2;
				if (FlagSet(woFlags, WOF_BORDER))
					offset += 2;
				if (!FlagSet(btFlags, BTF_NO_3D))
					offset += 6;
				if (bitmapHeight)
				{
					height = bitmapHeight + offset;
					if (text && text[0] && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
						height += display->cellHeight - display->preSpace - display->postSpace - 1;
				}
				if (!Inherited(ID_LIST_ITEM) && !bitmapHeight)
  					height = MaxValue(height, display->cellHeight * 10 / 9);
			}
			if (!Inherited(ID_LIST_ITEM) && !parent->Inherited(ID_TOOL_BAR) &&
				!parent->Inherited(ID_GROUP))
				relative.top = relative.bottom - height;
			else
				relative.bottom = relative.top + height;
		}
#endif

#if !defined(ZIL_GRAPHICS_ONLY)
		if (display->isText && FlagSet(btFlags, BTF_AUTO_SIZE) && depth == 2)
		{
#	if defined(ZIL_SHADOW_BORDER)
			relative.top = relative.bottom - 1;
#	elif defined(ZIL_STANDARD_BORDER) || defined(ZIL_3D_BORDER)
			relative.top = relative.bottom;
#	endif
		}
		else if (display->isText)
			relative.bottom = relative.top + height;
		}
#endif
		break;

	case S_DEINITIALIZE:
		if (FlagSet(btFlags, BTF_DEFAULT_BUTTON))
			Root(TRUE)->Information(I_SET_DEFAULT_OBJECT, ZIL_NULLP(UI_WINDOW_OBJECT));
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CREATE:
	case S_CHANGED:
		UI_WINDOW_OBJECT::Event(event);

		if (Inherited(ID_LIST_ITEM))
			btFlags &= ~BTF_NO_TOGGLE;
#if !defined(ZIL_TEXT_ONLY)
		// Convert the bitmap array to a handle if possible.
		if (bitmapArray && !colorBitmap && !display->isText)
		{
			display->BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight,
				bitmapArray, ZIL_NULLP(UI_PALETTE), &colorBitmap, &monoBitmap);
			if (colorBitmap && bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
			{
				delete bitmapArray;
				bitmapArray = ZIL_NULLP(ZIL_UINT8);
			}
		}
#endif
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		UI_WINDOW_OBJECT::Event(event);
		lastPalette = LogicalPalette(ccode);
		if (parent->Inherited(ID_COMBO_BOX) && ccode == S_CURRENT)
			woStatus |= WOS_SELECTED;
		else if (parent->Inherited(ID_COMBO_BOX))
			woStatus &= ~WOS_SELECTED;

		if (!Inherited(ID_MENU_ITEM))
		{
			if (FlagSet(btFlags, BTF_DEFAULT_BUTTON) && parent)
				Root(TRUE)->Information(I_SET_DEFAULT_OBJECT, this);

#if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
			UI_WINDOW_OBJECT *tObject;
			Root(TRUE)->Information(I_GET_DEFAULT_OBJECT, &tObject);
 			if (tObject && ccode == S_CURRENT && !Inherited(ID_LIST_ITEM))
				tObject->Event(UI_EVENT(S_HIDE_DEFAULT));
			else if (tObject && ccode == S_NON_CURRENT)
				tObject->Event(UI_EVENT(S_SHOW_DEFAULT));
#else
			if (FlagSet(btFlags, BTF_DEFAULT_BUTTON))
				Event(S_SHOW_DEFAULT);
#endif
		}
		if (FlagSet(woStatus, WOS_REDISPLAY))
			DrawItem(event, ccode);
		woStatus &= ~WOS_REDISPLAY;
#if defined(ZIL_CURSES) && defined(_IBM_RS6000)
		// RS6000 Curses doesn't display reverse video, so place the cursor
		// on the button to indicate currency.
		if (ccode == S_CURRENT)
		{
			eventManager->DeviceState(E_CURSOR, D_ON);
			eventManager->DevicePosition(E_CURSOR, trueRegion.left + 1, trueRegion.top);
		}
#endif
		break;

	case S_SHOW_DEFAULT:
	case S_HIDE_DEFAULT:
		{
		BTS_STATUS oldStatus = btStatus;
		if (ccode == S_SHOW_DEFAULT)
			btStatus |= BTS_DEFAULT;
		else
			btStatus &= ~BTS_DEFAULT;
		if (screenID && oldStatus != btStatus)
		{
			UI_EVENT tEvent = event;
			tEvent.region = trueRegion;
			DrawItem(tEvent, ccode);
		}
		}
		break;

	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
	case L_CONTINUE_COPY_DRAG:
	case L_END_COPY_DRAG:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));

		if (windowManager->dragObject && windowManager->dragObject != this)
			return UI_WINDOW_OBJECT::Event(event);

		if (Inherited(ID_LIST_ITEM) && ccode == L_END_SELECT && trueRegion.Overlap(event.position))
		{
			currentTime.Import();
			btStatus &= ~BTS_DEPRESSED;
			if (currentTime.Difference(lastTime) < doubleClickRate && this == lastSelected)
			{
				UI_EVENT uEvent = event;
				uEvent.type = L_DOUBLE_CLICK;
				Event(uEvent);
			}
			else
			{
				lastTime = currentTime;
				lastSelected = this;
				WOS_STATUS oldStatus = woStatus;
				ccode = UserFunction(event, L_SELECT);
				if (oldStatus != woStatus)
					Event(UI_EVENT(S_REDISPLAY));
			}
			break;
		}
		else if (!Inherited(ID_LIST_ITEM) && !Inherited(ID_MENU_ITEM))
		{
			if (ccode == L_BEGIN_SELECT)
				lastSelected = this;
			else if (ccode == L_END_SELECT && lastSelected != this)
				break;
		}

		else if (ccode == L_END_SELECT && !FlagSet(btStatus, BTS_DEPRESSED))
			break;	// Case where selection was down-click from another object.
		{
		BTS_STATUS oldStatus = btStatus;
		ccode = UI_WINDOW_OBJECT::Event(event);
		currentTime.Import();
		if (FlagSet(btFlags, BTF_DOUBLE_CLICK) && ccode == L_END_SELECT &&
			currentTime.Difference(lastTime) < doubleClickRate)
		{
			UI_EVENT uEvent = event;
			uEvent.type = L_DOUBLE_CLICK;
			Event(uEvent);
		}

		if (ccode == S_UNKNOWN || ccode == L_END_SELECT)
		{
			btStatus &= ~BTS_DEPRESSED;
			if (ccode == L_END_SELECT)
			{
				WOS_STATUS oldWoStatus = woStatus;
				ccode = UserFunction(UI_EVENT(L_END_SELECT), L_SELECT);
				if (woStatus != oldWoStatus)
					Event(UI_EVENT(S_REDISPLAY));
			}
			lastTime = currentTime;
		}
		else
			btStatus |= BTS_DEPRESSED;

		if (oldStatus != btStatus && depth && !FlagSet(btFlags, BTF_NO_3D))
			DrawItem(event, RedisplayType());
		}
		if (FlagSet(btFlags, BTF_REPEAT) && ccode == L_BEGIN_SELECT)
		{
			UI_EVENT tEvent = event;
			currentTime.Import();
			for(;;)
			{
				currentTime.Import();
				eventManager->Get(tEvent, Q_NO_BLOCK);
				ccode = LogicalEvent(tEvent, ID_BUTTON);
				if(ccode == L_END_SELECT)
					break;
				// Send all user messages to the window manager.
				if (tEvent.type > 9999)
				{
					if (currentTime.Difference(lastTime) > repeatRate)
					{
						windowManager->Event(tEvent);
						lastTime = currentTime;
					}
				}
				else if (trueRegion.Overlap(tEvent.position) &&
					currentTime.Difference(lastTime) > repeatRate && userFunction)
				{
					(*userFunction)(this, tEvent, L_SELECT);
					lastTime = currentTime;
				}
			}
			Event(UI_EVENT(ccode)); // same as Event(UI_EVENT(L_END_SELECT))
			break;
		}
		else if ((FlagSet(btFlags, BTF_DOWN_CLICK) && ccode == L_BEGIN_SELECT))
			ccode = L_SELECT;

		else if (ccode == L_BEGIN_SELECT && depth && !FlagSet(btFlags, BTF_NO_3D) &&
			!FlagSet(woFlags, WOF_SUPPORT_OBJECT) && !parent->Inherited(ID_SPIN_CONTROL))
		{
			lastSelected = this;
			UI_EVENT tEvent;
			do
			{
				eventManager->Get(tEvent);
				ccode = LogicalEvent(tEvent);

				// Send all user messages to the window manager.
				if (tEvent.type > 9999)
					windowManager->Event(tEvent);
				
				// Process mouse drag events.
				else if (event.type == E_MOUSE)
				{
					// Redraw button unpressed when mouse doesn't overlap.
					if (FlagSet(btStatus, BTS_DEPRESSED) && !trueRegion.Overlap(tEvent.position))
					{
						btStatus &= ~BTS_DEPRESSED;
						DrawItem(tEvent, RedisplayType());
					}
					else if (!FlagSet(btStatus, BTS_DEPRESSED) && trueRegion.Overlap(tEvent.position))
					{
						btStatus |= BTS_DEPRESSED;
						DrawItem(tEvent, RedisplayType());
					}
					// Don't pass event along if a new downclick occurred on the button.
					else if (!(ccode == L_BEGIN_SELECT && FlagSet(btStatus, BTS_DEPRESSED)))
						windowManager->Event(tEvent);
				}
			}
			// Don't continue in loop if an upclick occurs or if the user
			// released the mouse without an L_END_SELECT occurring.
			while (ccode != L_END_SELECT && ccode == L_CONTINUE_SELECT);
		}
		else
			break;
		// Continue to L_SELECT.

	case L_SELECT:
		if (ccode == L_SELECT)
		{
			WOS_STATUS oldStatus = woStatus;
			ccode = UserFunction(event, L_SELECT);
			if (oldStatus != woStatus)
				Event(UI_EVENT(S_REDISPLAY));
		}
		if (!Inherited(ID_LIST_ITEM))
			ccode = -1;
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_BUTTON::OSDataSet(void)
{
	Event(UI_EVENT(S_REDISPLAY));
}

void UIW_BUTTON::OSImageInitialize(void)
{
	if (display)
	{
		display->BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight,
			bitmapArray, ZIL_NULLP(UI_PALETTE), &colorBitmap, &monoBitmap);
		if (colorBitmap && bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
		{
			delete bitmapArray;
			bitmapArray = ZIL_NULLP(ZIL_UINT8);
		}
	}
}

void UIW_BUTTON::OSSystemImage(void)
{
}

void UIW_BUTTON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_BUTTON && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}



