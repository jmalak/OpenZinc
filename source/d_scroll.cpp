//	Zinc Interface Library - D_SCROLL.CPP
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

// ----- UIW_SCROLL_BAR -----------------------------------------------------

EVENT_TYPE UIW_SCROLL_BAR::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	if (!event.region.Overlap(trueRegion))
		return FALSE;

	display->VirtualGet(screenID, trueRegion);
	UI_REGION region = trueRegion;
#if !defined(ZIL_TEXT_ONLY)
	if (!display->isText)
	{
	if (FlagSet(sbFlags, SBF_SLIDER))
	{
		UI_PALETTE *windowPalette = parent->LogicalPalette(ccode, ID_WINDOW);
		UI_PALETTE background = *windowPalette;
		background.colorForeground = background.colorBackground;
		background.bwForeground = background.bwBackground;
		background.grayScaleForeground = background.grayScaleBackground;

		display->Rectangle(screenID, region, &background, 4, FALSE, FALSE, &clip);
		region-=4;
		DrawBorder(screenID, region, FALSE, ccode);
#	if defined(ZIL_MSWINDOWS_STYLE)
		UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
		UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
		display->Line(screenID, region.left, region.top, region.right, region.top,	darkShadow, 1, FALSE, &clip);
		display->Line(screenID, region.left, region.top, region.left, region.bottom, darkShadow, 1, FALSE, &clip);
		display->Line(screenID, region.left+1, region.bottom, region.right, region.bottom, lightShadow, 1, FALSE, &clip);
		display->Line(screenID, region.right, region.top+1, region.right, region.bottom, lightShadow, 1, FALSE, &clip);
		--region;
#	endif
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
		display->VirtualPut(screenID);
		mButton->Event(UI_EVENT(S_REDISPLAY));
		return FALSE;
	}

#	if defined(ZIL_OS2_STYLE)
	if (FlagSet(sbFlags, SBF_CORNER))
	{
		DrawBorder(screenID, region, FALSE, ccode);
		UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_LIGHT_SHADOW);
		display->Rectangle(screenID, region, lightShadow, 3, FALSE, FALSE, &clip);
		region-=3;
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
		display->VirtualPut(screenID);
		return (FALSE);	// Draw operation is not complete until children are drawn.
	}
#	endif
#	if defined(ZIL_MOTIF_STYLE)
	if (FlagSet(sbFlags, SBF_VERTICAL))
	{
		UI_REGION tRegion = trueRegion;
		UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
		{
			region.left += display->cellWidth;
			tRegion.right = region.left;
			display->Line(screenID, tRegion.left + 1, tRegion.top + 1,
				tRegion.left + 1, tRegion.bottom - 1, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, tRegion.left, tRegion.top + 2,
				tRegion.left, tRegion.bottom - 2, lightShadow, 1, FALSE, &clip);
			lightShadow = LogicalPalette(ccode, ID_LIGHT_SHADOW);
			display->Rectangle(screenID, tRegion.left + 2, tRegion.top,
				tRegion.right - 1, tRegion.bottom, lightShadow, 0, TRUE, FALSE, &clip);
		}
	}
	else if (FlagSet(sbFlags, SBF_HORIZONTAL))
	{
		UI_REGION tRegion = trueRegion;
		UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
		{
			region.top += display->cellWidth;
			tRegion.bottom = region.top;
			display->Line(screenID, tRegion.left + 1, tRegion.top + 1,
				tRegion.right - 1, tRegion.top + 1, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, tRegion.left + 2, tRegion.top,
				tRegion.right - 2, tRegion.top, lightShadow, 1, FALSE, &clip);
			lightShadow = LogicalPalette(ccode, ID_LIGHT_SHADOW);
			display->Rectangle(screenID, tRegion.left, tRegion.top + 2,
			tRegion.right, tRegion.bottom-1, lightShadow, 0, TRUE, FALSE, &clip);
		}
	}
	else
	{
		UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_LIGHT_SHADOW);
		display->Rectangle(screenID, region, lightShadow, 0, TRUE, FALSE, &clip);
		display->VirtualPut(screenID);
		return (FALSE);	// Draw operation is not complete until children are drawn.
	}

	if (FlagSet(woFlags, WOF_BORDER) && !display->isText)
	{
		UI_REGION save = clip;
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
			clip = parent->clip;
		DrawBorder(screenID, region, FALSE, ccode);
		clip = save;
	}
#	else
	if ((FlagSet(woFlags, WOF_BORDER) && !display->isText))
		DrawBorder(screenID, region, FALSE, ccode);
#	endif
	}
#endif

	UI_REGION tRegion;
	for (UI_REGION_ELEMENT *element = clipList.First(); element; element = element->Next())
		if (element->region.Overlap(event.region, tRegion))
			display->Rectangle(screenID, tRegion, lastPalette, 0, TRUE, FALSE, &clip);

	display->VirtualPut(screenID);
	return (FALSE);	// Draw operation is not complete until children are drawn.
}

EVENT_TYPE UIW_SCROLL_BAR::Event(const UI_EVENT &event)
{
	UI_EVENT sEvent;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_SCROLL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
/* START BLOCK COMMENT
**			if (lButton)
**				lButton->paletteMapTable = parent->paletteMapTable;
**			if (mButton)
**				mButton->paletteMapTable = parent->paletteMapTable;
**			if (rButton)
**				rButton->paletteMapTable = parent->paletteMapTable;
END BLOCK COMMENT */
		if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			wnFlags |= WNF_CONTINUE_SELECT;
		else if (FlagSet(sbFlags, SBF_VERTICAL) && parent)
			parent->Information(I_SET_VSCROLL, this);
		else if (FlagSet(sbFlags, SBF_HORIZONTAL) && parent)
			parent->Information(I_SET_HSCROLL, this);
		if (parent && (parent->Inherited(ID_LIST) || parent->Inherited(ID_TEXT)))
			wnFlags |= WNF_CONTINUE_SELECT;
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_CHANGED:
	case S_CREATE:
		{
		// Compute the object region.
		clipList.Destroy();
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
			if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
			{
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.left = trueRegion.right;
				else
					trueRegion.right = trueRegion.left;
			}
			if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
			{
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.top = trueRegion.bottom;
				else
					trueRegion.bottom = trueRegion.top;
			}
		}
#if !defined(ZIL_TEXT_ONLY)
		else
		{
#	if defined(ZIL_MSWINDOWS_STYLE)
			if (FlagsSet(woFlags, WOF_BORDER | WOF_NON_FIELD_REGION))
				++trueRegion;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.left = trueRegion.right - 16;
				else
					trueRegion.right = trueRegion.left + 16;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.top = trueRegion.bottom - 16;
				else
					trueRegion.bottom = trueRegion.top + 16;
#	elif defined(ZIL_OS2_STYLE)
			if (FlagsSet(woFlags, WOF_BORDER | WOF_NON_FIELD_REGION))
				++trueRegion;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.left = trueRegion.right - 13;
				else
					trueRegion.right = trueRegion.left + 13;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.top = trueRegion.bottom - 13;
				else
					trueRegion.bottom = trueRegion.top + 13;
#	elif defined(ZIL_MOTIF_STYLE)
			if (FlagsSet(woFlags, WOF_BORDER | WOF_NON_FIELD_REGION))
			{
				if (parent && parent->Inherited(ID_TABLE))
				{
					if (FlagSet(sbFlags, SBF_VERTICAL))
						trueRegion.top = MaxValue(trueRegion.top, clip.top);
					else if (FlagSet(sbFlags, SBF_HORIZONTAL))
						trueRegion.left = MaxValue(trueRegion.left, clip.left);
				}
				else
					trueRegion += ZIL_BORDER_WIDTH;
			}
			if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.left = trueRegion.right - 14;
				else
					trueRegion.right = trueRegion.left + 14;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
				if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					trueRegion.top = trueRegion.bottom - 14;
				else
					trueRegion.bottom = trueRegion.top + 14;
#	endif
		}
#endif
		// Compute the scroll button regions.
		UI_REGION region = trueRegion;
#if !defined(ZIL_TEXT_ONLY)
		if (FlagSet(woFlags, WOF_BORDER) && !display->isText)
		{
			region -= ZIL_BORDER_WIDTH;
#	if defined(ZIL_MOTIF_STYLE)
			if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL) && FlagSet(woFlags, WOF_NON_FIELD_REGION))
				trueRegion.left -= display->cellWidth;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL) && FlagSet(woFlags, WOF_NON_FIELD_REGION))
				trueRegion.top -= display->cellWidth;
#	endif
		}
#endif
		if (!display->isText && FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
			region.top+=5, region.bottom-=5;
		else if (!display->isText && FlagsSet(sbFlags, SBF_SLIDER | SBF_HORIZONTAL))
			region.left+=5, region.right-=5;

		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
		UI_WINDOW_OBJECT *object = NULL;
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->trueRegion, FALSE);
		}
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
#if defined(ZIL_MSWINDOWS_STYLE)
			if (!display->isText && !FlagSet(sbFlags, SBF_SLIDER))
				if (FlagSet(sbFlags, SBF_VERTICAL))
					clipList.First()->region.top--, clipList.First()->region.bottom++;
				else if (!display->isText && FlagSet(sbFlags, SBF_HORIZONTAL))
					clipList.First()->region.left--, clipList.First()->region.right++;
#endif
		}
		if (FlagSet(sbFlags, SBF_VERTICAL) && !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			Event(UI_EVENT(S_VSCROLL_SET, 0, scroll));
		else if (FlagSet(sbFlags, SBF_HORIZONTAL) && !FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			Event(UI_EVENT(S_HSCROLL_SET, 0, scroll));
		}

		if (FlagSet(sbFlags, SBF_SLIDER))
			mButton->clip = clip;
		break;

	case S_VSCROLL_SET:
	case S_HSCROLL_SET:
		scroll = event.scroll;
		// Continue to S_VSCROLL.
	case S_VSCROLL:
	case S_HSCROLL:
		{
		int delta = event.scroll.delta;
		int tScroll = (ccode == S_VSCROLL || ccode == S_HSCROLL) ?
			scroll.current + delta : scroll.current;
		if (tScroll < scroll.minimum)
		{
			delta = scroll.current - scroll.minimum;
			tScroll = scroll.minimum;
		}
		else if (tScroll > scroll.maximum)
		{
			delta = scroll.maximum - scroll.current;
			tScroll = scroll.maximum;
		}

		scroll.current = tScroll;
		int size = display->isText ? 0 :
			FlagSet(sbFlags, SBF_VERTICAL) ? mButton->trueRegion.bottom - mButton->trueRegion.top :
				mButton->trueRegion.right - mButton->trueRegion.left;

		UI_REGION region = clipList.First() ? clipList.First()->region : trueRegion;
		UI_REGION update = mButton->trueRegion;
		if (!display->isText && (ccode == S_VSCROLL_SET || ccode == S_HSCROLL_SET) &&
			!FlagSet(sbFlags, SBF_SLIDER) && FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			// Calculate thumb size.
			mButton->trueRegion = region;
#if defined(ZIL_MSWINDOWS_STYLE)
			size = 16;
			if (FlagSet(sbFlags, SBF_VERTICAL))
				mButton->trueRegion.left--, mButton->trueRegion.right++;
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				mButton->trueRegion.top--, mButton->trueRegion.bottom++;
#elif defined(ZIL_OS2_STYLE)
			long sizeValue = ((long)scroll.showing * 100) / MaxValue(1, scroll.maximum - scroll.minimum + scroll.showing);
			size = MinValue(region.Height(), region.Width());
			if (FlagSet(sbFlags, SBF_VERTICAL))
				size = (sizeValue > 0 && scroll.maximum > scroll.minimum ?
					(int)MaxValue(region.Width(), region.Height() * sizeValue / 100) : 0);
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				size = (sizeValue > 0 && scroll.maximum > scroll.minimum ?
					(int)MaxValue(region.Height(), region.Width() * sizeValue / 100) : 0);
			if (size == 0 && FlagSet(sbFlags, SBF_VERTICAL))
				mButton->trueRegion.left = mButton->trueRegion.right = trueRegion.right + 1;
			else if (size == 0 && FlagSet(sbFlags, SBF_HORIZONTAL))
				mButton->trueRegion.top = mButton->trueRegion.bottom = trueRegion.bottom + 1;
#elif defined(ZIL_MOTIF_STYLE)
			long sizeValue = ((long)scroll.showing * 100) / MaxValue(1, scroll.maximum - scroll.minimum + scroll.showing);
			size = MinValue(region.Height(), region.Width());
			if (FlagSet(sbFlags, SBF_VERTICAL))
				size = (sizeValue > 0 && scroll.maximum > scroll.minimum ?
					(int)MaxValue(region.Width(), region.Height() * sizeValue / 100) : region.Height());
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				size = (sizeValue > 0 && scroll.maximum > scroll.minimum ?
					(int)MaxValue(region.Height(), region.Width() * sizeValue / 100) : region.Width());
#endif
		}
		if (FlagSet(sbFlags, SBF_VERTICAL))
		{
			long value = region.bottom - region.top - size;
			value *= (scroll.current - scroll.minimum);
			if (scroll.maximum > scroll.minimum)
				value /= (scroll.maximum - scroll.minimum);
			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				mButton->trueRegion.bottom = region.bottom - (int)value;
				mButton->trueRegion.top = mButton->trueRegion.bottom - size;
			}
			else
			{
				mButton->trueRegion.top = region.top + (int)value;
				mButton->trueRegion.bottom = mButton->trueRegion.top + size;
			}
		}
		else if (FlagSet(sbFlags, SBF_HORIZONTAL))
		{
			long value = region.right - region.left - size;
			value *= (scroll.current - scroll.minimum);
			if (scroll.maximum > scroll.minimum)
				value /= (scroll.maximum - scroll.minimum);
			mButton->trueRegion.left = region.left + (int)value;
			mButton->trueRegion.right = mButton->trueRegion.left + size;
		}
		if (screenID && (ccode == S_VSCROLL || ccode == S_HSCROLL))
		{
			if (ccode == S_VSCROLL && ((event.scroll.delta > 0  && !FlagSet(sbFlags, SBF_SLIDER)) ||
				(event.scroll.delta < 0 && FlagSet(sbFlags, SBF_SLIDER))))
				update.bottom = mButton->trueRegion.top;
			else if (ccode == S_VSCROLL)
				update.top = mButton->trueRegion.bottom;
			else if (ccode == S_HSCROLL && event.scroll.delta > 0)
				update.right = mButton->trueRegion.left;
			else if (ccode == S_HSCROLL)
				update.left = mButton->trueRegion.right;
  			Event(UI_EVENT(RedisplayType(), 0, update));
		}

		if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT) && userFunction &&
		(ccode == S_VSCROLL || ccode == S_HSCROLL) && (delta || event.rawCode == L_END_SELECT))
		{
			UI_EVENT uEvent;
			uEvent.type = event.rawCode;
			uEvent.scroll = scroll;
			userFunction(this, uEvent, L_SELECT);
		}
		}
		break;

	case L_BEGIN_SELECT:
		// Check for the edit mode.
		if (FlagSet(woStatus, WOS_EDIT_MODE) && FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			return (parent->UI_WINDOW_OBJECT::Event(event));
		else if (FlagSet(woStatus, WOS_EDIT_MODE) || FlagSet(sbFlags, SBF_CORNER) ||
			!trueRegion.Overlap(event.position))
			return (UI_WINDOW_OBJECT::Event(event));
		{
		UI_WINDOW_OBJECT *selected = ZIL_NULLP(UI_WINDOW_OBJECT);
		if (mButton && mButton->trueRegion.Overlap(event.position))
			selected = mButton;
		else if (lButton && lButton->trueRegion.Overlap(event.position))
			selected = lButton;
		else if (rButton && rButton->trueRegion.Overlap(event.position))
			selected = rButton;

		UI_REGION updateRegion = clipList.First() ? clipList.First()->region : trueRegion;
		if (selected == mButton)
		{
			UI_REGION startRegion = mButton->trueRegion;
			UI_REGION newRegion = startRegion;
			UI_POSITION position = event.position;
			if (FlagSet(woFlags, WOF_SUPPORT_OBJECT) && FlagSet(wnFlags, WNF_CONTINUE_SELECT))
			{
				mButton->btStatus |= BTS_DEPRESSED;
				mButton->Event(UI_EVENT(S_REDISPLAY));
			}
			else if (!FlagSet(wnFlags, WNF_CONTINUE_SELECT) && !display->isText)
				display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE, &clip);

			while (ccode != L_END_SELECT)
			{
				eventManager->Get(sEvent);
				ccode = LogicalEvent(sEvent, ID_SCROLL_BAR);
				// Send all user messages to the window manager.
				if (sEvent.type > 9999)
					windowManager->Event(sEvent);
				else
				{
					int deltaX = sEvent.position.column - position.column;
					int deltaY = sEvent.position.line - position.line;
					if (FlagSet(sbFlags, SBF_VERTICAL))
						deltaX = 0;
					else if (newRegion.left + deltaX < updateRegion.left)
						deltaX = updateRegion.left - newRegion.left;
					else if (newRegion.right + deltaX > updateRegion.right)
						deltaX = updateRegion.right - newRegion.right;
					if (FlagSet(sbFlags, SBF_HORIZONTAL))
						deltaY = 0;
					else if (newRegion.top + deltaY < updateRegion.top)
						deltaY = updateRegion.top - newRegion.top;
					else if (newRegion.bottom + deltaY > updateRegion.bottom)
						deltaY = updateRegion.bottom - newRegion.bottom;
					if (deltaX || deltaY)
					{
						UI_REGION oldRegion = newRegion;
						newRegion.left += deltaX;
						newRegion.top += deltaY;
						newRegion.right += deltaX;
						newRegion.bottom += deltaY;
						position.column += deltaX;
						position.line += deltaY;
						if (!display->isText && !FlagSet(wnFlags, WNF_CONTINUE_SELECT))
							display->RectangleXORDiff(oldRegion, newRegion, screenID, &clip);
						int newDelta = 0;
						if (FlagSet(sbFlags, SBF_VERTICAL))
						{
							long pos = newRegion.top - updateRegion.top;
							if (FlagSet(sbFlags, SBF_SLIDER))
								pos = updateRegion.bottom - newRegion.bottom;
							long range = scroll.maximum - scroll.minimum;
							long region = updateRegion.Height() - mButton->trueRegion.Height();
							sEvent.type = S_VSCROLL;
							newDelta = region ? (int)((pos * range + region - 1) / region) -
								(scroll.current - scroll.minimum) : 0;
						}
						else if (FlagSet(sbFlags, SBF_HORIZONTAL))
						{
							long pos = (newRegion.left - updateRegion.left);
							long range = scroll.maximum - scroll.minimum;
							long region = updateRegion.Width() - mButton->trueRegion.Width();
							sEvent.type = S_HSCROLL;
							newDelta = region ? (int)((pos * range + region - 1) / region) -
								(scroll.current - scroll.minimum) : 0;
						}
						if (newDelta && (display->isText || FlagSet(wnFlags, WNF_CONTINUE_SELECT)))
						{
							sEvent.rawCode = L_CONTINUE_SELECT;
							sEvent.scroll = scroll;
							sEvent.scroll.delta = newDelta;
							if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
								parent->Event(sEvent);
							else
								Event(sEvent);
						}
					}
				}
			}
			if (FlagSet(woFlags, WOF_SUPPORT_OBJECT) && FlagSet(wnFlags, WNF_CONTINUE_SELECT))
			{
				mButton->btStatus &= ~BTS_DEPRESSED;
				mButton->Event(UI_EVENT(S_REDISPLAY));
			}
			else if (!FlagSet(wnFlags, WNF_CONTINUE_SELECT) && !display->isText)
				display->Rectangle(ID_DIRECT, display->isText ? startRegion : newRegion, display->xorPalette, 1, FALSE, TRUE, &clip);

			if (!trueRegion.Overlap(sEvent.position))
				break;
			else if (FlagSet(sbFlags, SBF_VERTICAL))
			{
				long pos = newRegion.top - updateRegion.top;
				if (FlagSet(sbFlags, SBF_SLIDER))
					pos = updateRegion.bottom - newRegion.bottom;
				long range = scroll.maximum - scroll.minimum;
				long region = updateRegion.Height() - mButton->trueRegion.Height();
				sEvent.type = S_VSCROLL;
				sEvent.scroll = scroll;
				sEvent.scroll.delta = region ? (int)((pos * range + region - 1) / region) -
					(scroll.current - scroll.minimum) : 0;
			}
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				long pos = (newRegion.left - updateRegion.left);
				long range = scroll.maximum - scroll.minimum;
				long region = updateRegion.Width() - mButton->trueRegion.Width();
				sEvent.type = S_HSCROLL;
				sEvent.scroll = scroll;
				sEvent.scroll.delta = region ? (int)((pos * range + region - 1) / region) -
					(scroll.current - scroll.minimum) : 0;
			}
			// Reset the scroll region.
			sEvent.rawCode = L_END_SELECT;
			if (sEvent.scroll.delta && FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				parent->Event(sEvent);
			else
				Event(sEvent);
			break;	// Do not continue to default operation of other buttons.
		}
		else if (FlagSet(sbFlags, SBF_SLIDER))
		{
			long pos, region, range = scroll.maximum - scroll.minimum;
			sEvent.scroll = scroll;
 			if (FlagSet(sbFlags, SBF_VERTICAL))
			{
				pos = (updateRegion.bottom - event.position.line);
				region = updateRegion.Height() - mButton->trueRegion.Height();
				sEvent.type = S_VSCROLL;
			}
			else
			{
				pos = (event.position.column - updateRegion.left);
				region = updateRegion.Width() - mButton->trueRegion.Width();
				sEvent.type = S_HSCROLL;
			}
			sEvent.scroll.delta = region ? (int)((pos * range + region - 1) / region) -
				(scroll.current - scroll.minimum) : 0;
			Event(sEvent);
			break;
		}
		else
		{
			UI_EVENT sEvent;
			sEvent.type = FlagSet(sbFlags, SBF_VERTICAL) ? S_VSCROLL : S_HSCROLL;
			sEvent.scroll = scroll;
			if (selected == lButton)
			{
				sEvent.rawCode = L_UP;
				sEvent.scroll.delta = -scroll.delta;
			}
			else if (selected == rButton)
			{
				sEvent.rawCode = L_DOWN;
				sEvent.scroll.delta = scroll.delta;
			}
			else if (FlagSet(sbFlags, SBF_VERTICAL))
			{
				// Selection of vertical page (previous/next).
				sEvent.scroll.delta = (event.position.line < mButton->trueRegion.top) ?
					-scroll.showing : scroll.showing;
				sEvent.rawCode = sEvent.scroll.delta < 0 ? L_PGUP : L_PGDN;
			}
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				// Selection of horizontal page (previous/next).
				sEvent.scroll.delta = (event.position.column < mButton->trueRegion.left) ?
					-scroll.showing : scroll.showing;
				sEvent.rawCode = sEvent.scroll.delta < 0 ? L_PGUP : L_PGDN;
			}

			if (selected)
				selected->Event(event);
			if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				parent->Event(sEvent);
			else
				Event(sEvent);

			UI_EVENT tEvent = event;
			ZIL_UTIME lastTime, currentTime;
			while (ccode != L_END_SELECT)
			{
				currentTime.Import();
				if (eventManager->Get(tEvent, Q_NO_BLOCK) != -2)
				{
					ccode = LogicalEvent(tEvent, ID_SCROLL_BAR);
					// Send all user messages to the window manager.
					if (tEvent.type > 9999)
						windowManager->Event(tEvent);
				}
				else if (currentTime.Difference(lastTime) > repeatRate && trueRegion.Overlap(tEvent.position))
				{
					if ((selected && selected->trueRegion.Overlap(tEvent.position)) ||
						(FlagSet(sbFlags, SBF_VERTICAL) && ((sEvent.rawCode == L_PGUP &&
							event.position.line < mButton->trueRegion.top) ||
								(sEvent.rawCode == L_PGDN && event.position.line > mButton->trueRegion.bottom)))	||
						(FlagSet(sbFlags, SBF_HORIZONTAL) && ((sEvent.rawCode == L_PGUP &&
							event.position.column < mButton->trueRegion.left) ||
								(sEvent.rawCode == L_PGDN && event.position.column > mButton->trueRegion.right))))
						{
							if (selected)
								selected->Event(tEvent);
							if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
								parent->Event(sEvent);
							else
								Event(sEvent);
							lastTime = currentTime;
						}
				}
				else if (selected && !selected->trueRegion.Overlap(tEvent.position))
					selected->Event(tEvent);
			}
			if (selected)
				selected->Event(tEvent);
		}
		}
		break;

	case L_UP:
	case L_DOWN:
	case L_LEFT:
	case L_RIGHT:
	case L_PGUP:
	case L_PGDN:
	case L_TOP:
	case L_BOTTOM:
		if (FlagSet(sbFlags, SBF_SLIDER))
		{
			UI_EVENT sEvent;
			sEvent.scroll = scroll;
			if (FlagSet(sbFlags, SBF_VERTICAL))
			{
				sEvent.type = S_VSCROLL;
				if (ccode == L_DOWN || ccode == L_RIGHT)
					sEvent.scroll.delta = -scroll.delta;
				else if (ccode == L_PGUP)
					sEvent.scroll.delta = scroll.showing;
				else if (ccode == L_PGDN)
					sEvent.scroll.delta = -scroll.showing;
				else if (ccode == L_TOP)
					sEvent.scroll.delta = scroll.maximum-scroll.minimum;
				else if (ccode == L_BOTTOM)
					sEvent.scroll.delta = -(scroll.maximum-scroll.minimum);
			}
			else
			{
				sEvent.type = S_HSCROLL;
				if (ccode == L_UP || ccode == L_LEFT)
					sEvent.scroll.delta = -scroll.delta;
				else if (ccode == L_PGUP)
					sEvent.scroll.delta = -scroll.showing;
				else if (ccode == L_PGDN)
					sEvent.scroll.delta = scroll.showing;
				else if (ccode == L_TOP)
					sEvent.scroll.delta = -(scroll.maximum-scroll.minimum);
				else if (ccode == L_BOTTOM)
					sEvent.scroll.delta = scroll.maximum-scroll.minimum;
			}
			Event(sEvent);
		}
		else
			UIW_WINDOW::Event(event);
		break;

	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

#define ZIL_UIW_SCROLL_BUTTON_DATA
#include "img_def.cpp"

// ----- UIW_SCROLL_BUTTON --------------------------------------------------

UIW_SCROLL_BAR::UIW_SCROLL_BUTTON::UIW_SCROLL_BUTTON(BUTTON_TYPE _btType) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_TOGGLE | BTF_STATIC_BITMAPARRAY,
		WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_NON_FIELD_REGION,
	ZIL_NULLF(ZIL_USER_FUNCTION), 0, ZIL_NULLP(ZIL_ICHAR)),
	btType(_btType)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	if (!defaultInitialized)
	{
		ZIL_DECORATION_MANAGER::SetDecorations(UIW_SCROLL_BAR::_className, TEXT_TABLE, BITMAP_TABLE);
		defaultInitialized = TRUE;
	}
	myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(UIW_SCROLL_BAR::_className);
#endif

	depth = 1;
	if (FlagSet(btType, LTBUTTON | RBBUTTON))
		woFlags |= WOF_SUPPORT_OBJECT;
}

UIW_SCROLL_BAR::UIW_SCROLL_BUTTON::~UIW_SCROLL_BUTTON(void)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);
#endif

	text = ZIL_NULLP(ZIL_ICHAR);
	bitmapArray = ZIL_NULLP(ZIL_UINT8);
}

EVENT_TYPE UIW_SCROLL_BAR::UIW_SCROLL_BUTTON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_SCROLL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
			// Null out the graphics information.
			bitmapArray = ZIL_NULLP(ZIL_UINT8);
			bitmapWidth = bitmapHeight = 0;
			// Set the text information.
			woFlags |= WOF_NON_FIELD_REGION;
			text = myDecorations->GetText(btType);
			if (btType == HMIDDLE_BUTTON)
				woFlags &= ~WOF_NON_FIELD_REGION;
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
			// Null out the text information.
			text = ZIL_NULLP(ZIL_ICHAR);
			// Set the graphics information.
			woFlags |= WOF_NON_FIELD_REGION;
			bitmapArray = myDecorations->GetBitmap(btType);
			if (btType == HMIDDLE_BUTTON)
				woFlags &= ~WOF_NON_FIELD_REGION;
			if (bitmapArray)
			{
				bitmapWidth = bitmapArray[0];
				bitmapHeight = bitmapArray[1];
				bitmapArray = &bitmapArray[2];
			}
#	if defined (ZIL_MSWINDOWS_STYLE)
			if (FlagSet(parent->woFlags, WOF_NON_FIELD_REGION))
				clip = parent->trueRegion;
#	endif
#endif
		}
		break;

	case S_CHANGED:
	case S_CREATE:
		{
		int width, height, clipWidth, clipHeight;
		UI_WINDOW_OBJECT::Event(event);
		SBF_FLAGS sbFlags;
		parent->Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR);

		if (!FlagSet(btType, MBUTTON) || FlagSet(parent->woFlags, WOF_SUPPORT_OBJECT))
			woAdvancedFlags |= WOAF_NON_CURRENT;

		clipWidth = parent->trueRegion.right - parent->trueRegion.left;
		clipHeight = parent->trueRegion.bottom - parent->trueRegion.top;

#if !defined(ZIL_TEXT_ONLY) && !defined(ZIL_MSWINDOWS_STYLE)
		if (!display->isText && FlagSet(parent->woFlags, WOF_BORDER))
			clipWidth -= 2 * ZIL_BORDER_WIDTH, clipHeight -= 2 * ZIL_BORDER_WIDTH;
#endif
		if (FlagSet(sbFlags, SBF_VERTICAL))
			width = height = clipWidth;
		else if (FlagSet(sbFlags, SBF_HORIZONTAL))
			width = height = clipHeight;

		if (display->isText)
		{
			// Resize if the region doesn't fit all the scroll buttons.
			if (btType == VTOP_BUTTON)
			{
				if (parent->trueRegion.Height() < 2)
					trueRegion.top--;
				trueRegion.bottom = trueRegion.top + height;
			}
			else if (btType == VBOTTOM_BUTTON)
			{
				if (parent->trueRegion.Height() < 2)
					trueRegion.top++;
				trueRegion.top = trueRegion.bottom - height;
			}
			else if (btType == HLEFT_BUTTON)
			{
				if (parent->trueRegion.Width() < 2)
					trueRegion.left--;
				trueRegion.right = trueRegion.left + width;
			}
			else if (btType == HRIGHT_BUTTON)
			{
				if (parent->trueRegion.Width() < 2)
					trueRegion.right++;
				trueRegion.left = trueRegion.right - width;
			}
			else if (FlagSet(btType, MBUTTON))
			{
				if (FlagSet(sbFlags, SBF_VERTICAL) && parent->trueRegion.Height() == 2)
					trueRegion.left++;
				else if (FlagSet(sbFlags, SBF_HORIZONTAL) && parent->trueRegion.Width() == 2)
					trueRegion.top++;
				trueRegion.right = trueRegion.left + width;
				trueRegion.bottom = trueRegion.top + height;
			}
		}

		else
		{
#if defined(ZIL_MSWINDOWS_STYLE)

				++trueRegion;
#elif defined(ZIL_OS2_STYLE)
			if (FlagSet(sbFlags, SBF_VERTICAL))
				height += 4;
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				width += 4;
#endif
			// Resize if the region doesn't fit all the scroll buttons.
			if (btType == VTOP_BUTTON)
			{
				if (clipHeight / 3 < height)
					height = clipHeight / 3;
				trueRegion.bottom = trueRegion.top + height;
			}
			else if (btType == VBOTTOM_BUTTON)
			{
				if (clipHeight / 2 < height)
					height = clipHeight / 2;
				trueRegion.top = trueRegion.bottom - height;
			}
			else if (btType == HLEFT_BUTTON)
			{
				if (clipWidth / 3 < width)
					width = clipWidth / 3;
				trueRegion.right = trueRegion.left + width;
			}
			else if (btType == HRIGHT_BUTTON)
			{
				if (clipWidth / 2 < width)
					width = clipWidth / 2;
				trueRegion.left = trueRegion.right - width;
			}
			else if (FlagSet(btType, MBUTTON))
			{
#if defined(ZIL_MSWINDOWS_STYLE)
				if (FlagSet(sbFlags, SBF_VERTICAL))
					trueRegion.bottom = trueRegion.top + width, trueRegion.right = parent->trueRegion.right;
				else if (FlagSet(sbFlags, SBF_HORIZONTAL))
					trueRegion.right = trueRegion.left + width, trueRegion.bottom = parent->trueRegion.bottom;
#elif defined(ZIL_OS2_STYLE) || defined(ZIL_MOTIF_STYLE)
				if (FlagSet(sbFlags, SBF_VERTICAL))
					trueRegion.bottom = trueRegion.top + height,
					trueRegion.right = parent->trueRegion.right - ZIL_BORDER_WIDTH;
				else if (FlagSet(sbFlags, SBF_HORIZONTAL))
					trueRegion.right = trueRegion.left + width,
					trueRegion.bottom = parent->trueRegion.bottom - ZIL_BORDER_WIDTH;
#endif

#if defined(ZIL_MOTIF_STYLE)
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					++trueRegion, trueRegion.bottom -= 5;
				else if (FlagsSet(sbFlags, SBF_SLIDER | SBF_HORIZONTAL))
					++trueRegion, trueRegion.right -= 5;
#else
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					trueRegion.bottom -= 8;
				else if (FlagsSet(sbFlags, SBF_SLIDER | SBF_HORIZONTAL))
					trueRegion.right -= 8;
#endif
			}
		}
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_SCROLL_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// Delete old buttons if necessary.
	if (screenID)
	{
		if (lButton)
		{
			Subtract(lButton);
			delete lButton;
			lButton = ZIL_NULLP(UIW_SCROLL_BUTTON);
		}
		if (mButton)
		{
			Subtract(mButton);
			delete mButton;
			mButton = ZIL_NULLP(UIW_SCROLL_BUTTON);
		}
		if (rButton)
		{
			Subtract(rButton);
			delete rButton;
			rButton = ZIL_NULLP(UIW_SCROLL_BUTTON);
		}
	}
	else
		lButton = mButton = rButton = ZIL_NULLP(UIW_SCROLL_BUTTON);

	// Add the scroll buttons.
	if (FlagSet(sbFlags, SBF_VERTICAL))
	{
		if (FlagSet(sbFlags, SBF_SLIDER))
			Add((mButton = new UIW_SCROLL_BUTTON(HMIDDLE_BUTTON)));
		else
		{
			Add((lButton = new UIW_SCROLL_BUTTON(VTOP_BUTTON)));
			Add((mButton = new UIW_SCROLL_BUTTON(HMIDDLE_BUTTON)));
			Add((rButton = new UIW_SCROLL_BUTTON(VBOTTOM_BUTTON)));
		}
	}
	else if (FlagSet(sbFlags, SBF_HORIZONTAL))
	{
		if (FlagSet(sbFlags, SBF_SLIDER))
			Add((mButton = new UIW_SCROLL_BUTTON(HMIDDLE_BUTTON)));
		else
		{
			Add((lButton = new UIW_SCROLL_BUTTON(HLEFT_BUTTON)));
			Add((mButton = new UIW_SCROLL_BUTTON(HMIDDLE_BUTTON)));
			Add((rButton = new UIW_SCROLL_BUTTON(HRIGHT_BUTTON)));
		}
	}

	if (!parent ||	FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		return;

	// See if the field needs to be re-computed.
	if (objectID == ID_SCROLL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}

