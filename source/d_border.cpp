//	Zinc Interface Library - D_BORDER.CPP
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

// ----- UIW_BORDER ---------------------------------------------------------

EVENT_TYPE UIW_BORDER::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_WINDOW_OBJECT *parent = this->parent;
	UI_REGION trueRegion = this->trueRegion;
	UI_REGION clip = this->clip;
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif

#if !defined(ZIL_GRAPHICS_ONLY)
	// Check for text mode display.
	if (display->isText)
	{
#	if defined(ZIL_SHADOW_BORDER)
	// Check for MDI child window.
	if (parent->parent && FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) &&
		FlagSet(parent->woStatus, WOS_MAXIMIZED))   // MDI ROLM bug
		return (ccode);

		UI_REGION region = parent->trueRegion;
		UI_REGION clipRegion;
		if (ccode == S_CURRENT)
			clipRegion = clip;
		else if (region.Overlap(event.region))
		{
			clipRegion.left = MaxValue(clip.left, event.region.left);
			clipRegion.top = MaxValue(clip.top, event.region.top);
			clipRegion.right = MinValue(clip.right, event.region.right);
			clipRegion.bottom = MinValue(clip.bottom, event.region.bottom);
		}
		else
			return (ccode);
		if (region.top < display->lines - 1 &&	region.top < clipRegion.bottom)
			display->Rectangle(screenID, region, ZIL_NULLP(UI_PALETTE), 0, TRUE, FALSE, &clipRegion);
#	elif defined(ZIL_STANDARD_BORDER)
		display->Rectangle(screenID, trueRegion, lastPalette, (ccode == S_DISPLAY_ACTIVE) ? 2 : 1);
#	elif defined(ZIL_3D_BORDER)
		UI_REGION region = parent->trueRegion;
		UI_REGION clipRegion;
		if (ccode == S_CURRENT)
			clipRegion = clip;
		else if (region.Overlap(event.region))
		{
			clipRegion.left = MaxValue(clip.left, event.region.left);
			clipRegion.top = MaxValue(clip.top, event.region.top);
			clipRegion.right = MinValue(clip.right, event.region.right);
			clipRegion.bottom = MinValue(clip.bottom, event.region.bottom);
		}
		else
			return (ccode);
		display->Rectangle(screenID, region, ZIL_NULLP(UI_PALETTE), 0, TRUE, FALSE, &clipRegion);
		region.right--;
		region.bottom--;
		display->Rectangle(screenID, region, lastPalette, 1);

		ZIL_ICHAR value = 0x89;
		display->Text(screenID, region.right, region.top + 1, &value,
			lastPalette, 1, TRUE, FALSE, &clip);
		display->Text(screenID, region.right, region.bottom - 1, &value,
			lastPalette, 1, TRUE, FALSE, &clip);
		value = 0x84;
		display->Text(screenID, region.left, region.top + 1, &value,
			lastPalette, 1, TRUE, FALSE, &clip);
		display->Text(screenID, region.left, region.bottom - 1, &value,
			lastPalette, 1, TRUE, FALSE, &clip);
		value = 0x88;
		display->Text(screenID, region.left + 2, region.bottom, &value,
			lastPalette, 1, TRUE, FALSE, &clip);
		display->Text(screenID, region.right - 2, region.bottom, &value,
			lastPalette, 1, TRUE, FALSE, &clip);
#	endif
		return (ccode);
	}
#endif

	// Draw the Graphics mode border.
#if !defined(ZIL_TEXT_ONLY) && defined(ZIL_MSWINDOWS_STYLE)
	int size = display->cellHeight;
	UI_REGION region = parent->trueRegion;
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	display->Rectangle(screenID, region, outline, 1, FALSE, FALSE, &clip);
	display->Rectangle(screenID, trueRegion, outline, 1, FALSE, FALSE, &clip);
	--region;
	if (ccode == S_DISPLAY_ACTIVE && FlagSet(parent->woAdvancedFlags, WOAF_DIALOG_OBJECT))
		lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_SPECIAL, ID_BORDER);

	// Fill the border region.
	display->Rectangle(screenID, region.left, region.top,
		region.right, trueRegion.top - 1, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, region.left, trueRegion.bottom + 1,
		region.right, region.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, region.left, trueRegion.top,
		trueRegion.left - 1, trueRegion.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, trueRegion.right + 1, trueRegion.top,
		region.right, trueRegion.bottom, lastPalette, 0, TRUE, FALSE, &clip);

	// Draw the size regions.
	if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE))
	{
		++region;
		display->Rectangle(screenID, trueRegion.left + size, region.top,
			trueRegion.right - size, trueRegion.top, outline, 1, FALSE, FALSE, &clip);
		display->Rectangle(screenID, trueRegion.left + size, trueRegion.bottom,
			trueRegion.right - size, region.bottom, outline, 1, FALSE, FALSE, &clip);
		display->Rectangle(screenID, region.left, trueRegion.top + size,
			trueRegion.left, trueRegion.bottom - size, outline, 1, FALSE, FALSE, &clip);
		display->Rectangle(screenID, trueRegion.right, trueRegion.top + size,
			region.right, trueRegion.bottom - size, outline, 1, FALSE, FALSE, &clip);
	}
#elif !defined(ZIL_TEXT_ONLY) && defined(ZIL_OS2_STYLE)
	UI_REGION region = parent->trueRegion;

	UI_PALETTE *outline = LogicalPalette(ccode, ID_WHITE_SHADOW);
	display->Line(screenID, region.left, region.top, region.right, region.top, outline, 1, FALSE, &clip);
	display->Line(screenID, region.left, region.top, region.left, region.bottom - 1, outline, 1, FALSE, &clip);
	display->Line(screenID, trueRegion.right, trueRegion.top + 1, trueRegion.right, trueRegion.bottom, outline, 1, FALSE, &clip);
	display->Line(screenID, trueRegion.left, trueRegion.bottom, trueRegion.right, trueRegion.bottom, outline, 1, FALSE, &clip);

	outline = LogicalPalette(ccode, ID_DARK_SHADOW);
	display->Line(screenID, region.right, region.top + 1, region.right, region.bottom, outline, 1, FALSE, &clip);
	display->Line(screenID, region.left, region.bottom, region.right, region.bottom, outline, 1, FALSE, &clip);
	display->Line(screenID, trueRegion.left, trueRegion.top, trueRegion.right, trueRegion.top, outline, 1, FALSE, &clip);
	display->Line(screenID, trueRegion.left, trueRegion.top, trueRegion.left, trueRegion.bottom - 1, outline, 1, FALSE, &clip);

	region -= 1;
	if (ccode == S_DISPLAY_ACTIVE && FlagSet(parent->woAdvancedFlags, WOAF_DIALOG_OBJECT))
		lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_SPECIAL, ID_BORDER);

	// Fill the border region.
	display->Rectangle(screenID, region.left, region.top,
		region.right, trueRegion.top - 1, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, region.left, trueRegion.bottom + 1,
		region.right, region.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, region.left, trueRegion.top,
		trueRegion.left - 1, trueRegion.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, trueRegion.right + 1, trueRegion.top,
		region.right, trueRegion.bottom, lastPalette, 0, TRUE, FALSE, &clip);
#elif !defined(ZIL_TEXT_ONLY) && defined(ZIL_MOTIF_STYLE)
	int size = display->cellHeight - 2;
	UI_REGION outside = parent->trueRegion;
	UI_REGION inside = trueRegion; inside += 1;
	UI_REGION region = inside;

	// Draw the size regions.
	UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
	UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
	int title = FALSE;
	for (UI_WINDOW_OBJECT *object = Next(); object; object = object->Next())
		if (object->Inherited(ID_TITLE))
			title = TRUE;
	for (int i = 0; i < 2; i++)
	{
		display->Line(screenID, outside.left + i, outside.top + i,
			outside.right - i, outside.top + i, lightShadow, 1, FALSE, &clip);
		display->Line(screenID, outside.left + i, outside.top + i,
			outside.left + i, outside.bottom - i, lightShadow, 1, FALSE, &clip);
		display->Line(screenID, outside.right - i, outside.top + i + 1,
			outside.right - i, outside.bottom - i, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, outside.left + i + 1, outside.bottom - i,
			outside.right - i - 1, outside.bottom - i, darkShadow, 1, FALSE, &clip);
		if (title && i)
			region.top--;
		display->Line(screenID, region.left + i, region.top + i,
			region.right - i, region.top + i, darkShadow, 1, FALSE, &clip);
		if (title && i)
			region.top += size - 1;
		display->Line(screenID, region.left + i, region.top + i,
			region.left + i, region.bottom - i, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, region.right - i, region.top + i + 1,
			region.right - i, region.bottom - i, lightShadow, 1, FALSE, &clip);
		display->Line(screenID, region.left + i + 1, region.bottom - i,
			region.right - i - 1, region.bottom - i, lightShadow, 1, FALSE, &clip);

	}
	outside -= 2;

	// Fill the border region.
	display->Rectangle(screenID, outside.left, outside.top,
		outside.right, inside.top - 1, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, outside.left, inside.bottom + 1,
		outside.right, outside.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, outside.left, inside.top,
		inside.left - 1, inside.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, inside.right + 1, inside.top,
		outside.right, inside.bottom, lastPalette, 0, TRUE, FALSE, &clip);

	outside += 1;
	if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE))
	{
		// Top ticks.
		display->Line(screenID, inside.left + size - 1, outside.top,
			inside.left + size - 1, inside.top, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, inside.left + size, outside.top,
			inside.left + size, inside.top, lightShadow, 1, FALSE, &clip);

		display->Line(screenID, inside.right - size, outside.top,
			inside.right - size, inside.top, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, inside.right - size + 1, outside.top,
			inside.right - size + 1, inside.top, lightShadow, 1, FALSE, &clip);

		// Bottom ticks.
		display->Line(screenID, inside.left + size - 1, inside.bottom + 1,
			inside.left + size - 1, outside.bottom, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, inside.left + size, inside.bottom + 1,
			inside.left + size, outside.bottom, lightShadow, 1, FALSE, &clip);

		display->Line(screenID, inside.right - size, inside.bottom + 1,
			inside.right - size, outside.bottom, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, inside.right - size + 1, inside.bottom + 1,
			inside.right - size + 1, outside.bottom, lightShadow, 1, FALSE, &clip);

		// Left ticks.
		display->Line(screenID, outside.left + 1, inside.top + size - 1,
			inside.left - 1, inside.top + size - 1, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, outside.left + 1, inside.top + size,
			inside.left - 1, inside.top + size, lightShadow, 1, FALSE, &clip);

		display->Line(screenID, outside.left + 1, inside.bottom - size - 1,
			inside.left - 1, inside.bottom - size - 1, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, outside.left + 1, inside.bottom - size,
			inside.left - 1, inside.bottom - size, lightShadow, 1, FALSE, &clip);

		// Right ticks.
		display->Line(screenID, inside.right + 1, inside.top + size - 1,
			outside.right - 1, inside.top + size - 1, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, inside.right + 1, inside.top + size,
			outside.right - 1, inside.top + size, lightShadow, 1, FALSE, &clip);

		display->Line(screenID, inside.right + 1, inside.bottom - size - 1,
			outside.right - 1, inside.bottom - size - 1, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, inside.right + 1, inside.bottom - size,
			outside.right - 1, inside.bottom - size, lightShadow, 1, FALSE, &clip);
	}
#endif

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_BORDER::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_BORDER);
	switch (ccode)
	{
	case S_CHANGED:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		if (parent->parent &&
			FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) &&
			FlagSet(parent->woStatus, WOS_MAXIMIZED))
		{
#if !defined(ZIL_TEXT_ONLY)
			if (!display->isText)
				++trueRegion;
#endif
		}
		else if (display->isText)
		{
#if defined(ZIL_SHADOW_BORDER)
			trueRegion = parent->trueRegion;
			trueRegion.right--;
			trueRegion.bottom--;
#elif defined(ZIL_3D_BORDER)
			trueRegion.right--;
			trueRegion.bottom--;
#endif
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
			trueRegion -= width;
#endif
		}
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		{
		UI_EVENT tEvent = event;
		UI_REGION region = event.region;
		UI_WINDOW_OBJECT::Event(tEvent);
		if (parent->trueRegion.Overlap(region))
			woStatus |= WOS_REDISPLAY;
		if (display->isText && region.Encompassed(trueRegion))
			woStatus &= ~WOS_REDISPLAY;
		if (FlagSet(woStatus, WOS_REDISPLAY) || display->isText)
			DrawItem(tEvent, ccode);
		woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case L_VIEW:
	case L_BEGIN_SELECT:
		{
		DEVICE_IMAGE image = DM_VIEW;
		if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE) &&
			!FlagSet(parent->woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
		{
			UI_REGION region = trueRegion;
#if defined(ZIL_SHADOW_BORDER)
#if !defined(ZIL_TEXT_ONLY)
			if (!display->isText)
				region -= display->cellHeight;
#endif
#elif defined(ZIL_STANDARD_BORDER) || defined(ZIL_3D_BORDER)
			region -= display->cellHeight;
#endif
			// Check for small window.
			if (!display->isText && region.top >= region.bottom)
			{
#if !defined(ZIL_TEXT_ONLY)
				region.top = trueRegion.top + trueRegion.Height() / 2;
				region.bottom = trueRegion.bottom - trueRegion.Height() / 2;
#endif
			}

			ZIL_RAW_CODE sizeFlags = 0;
			if (event.position.column <= region.left)
				sizeFlags |= M_LEFT_CHANGE;
			else if (event.position.column >= region.right)
				sizeFlags |= M_RIGHT_CHANGE;
			if (event.position.line <= region.top)
				sizeFlags |= M_TOP_CHANGE;
			else if (event.position.line >= region.bottom)
				sizeFlags |= M_BOTTOM_CHANGE;
			if (ccode == L_BEGIN_SELECT)
			{
				parent->Event(UI_EVENT(L_SIZE, sizeFlags, event.position));
				break;
			}
			if (FlagsSet(sizeFlags, M_LEFT_CHANGE | M_TOP_CHANGE) ||
			    FlagsSet(sizeFlags, M_RIGHT_CHANGE | M_BOTTOM_CHANGE))
				image = DM_DIAGONAL_ULLR;
			else if (FlagsSet(sizeFlags, M_LEFT_CHANGE | M_BOTTOM_CHANGE) ||
				 FlagsSet(sizeFlags, M_RIGHT_CHANGE | M_TOP_CHANGE))
				image = DM_DIAGONAL_LLUR;
			else if (FlagSet(sizeFlags, M_LEFT_CHANGE | M_RIGHT_CHANGE))
				image = DM_HORIZONTAL;
			else if (FlagSet(sizeFlags, M_TOP_CHANGE | M_BOTTOM_CHANGE))
				image = DM_VERTICAL;
		}
		eventManager->DeviceState(E_MOUSE, image);
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_BORDER::OSDataGet(void)
{
	// No code necessary.
}

void UIW_BORDER::OSDataSet(void)
{
	// No code necessary.
}

void UIW_BORDER::OSUpdateSettings(ZIL_OBJECTID)
{
	// No code necessary.
}
