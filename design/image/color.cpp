//	Zinc Interface Library - COLOR.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

UIW_COLOR_BAR::UIW_COLOR_BAR(int left, int top, int width, int height,
	CLF_FLAGS _clFlags, WOF_FLAGS _woFlags) :
	UI_WINDOW_OBJECT(left, top, width, height, _woFlags, WOAF_NO_FLAGS),
	cellWidth(1), maxColors(16), clFlags(_clFlags)
{
	woStatus |= WOS_OWNERDRAW;
	if (FlagsSet(clFlags, CLF_LEFT_ACTIVATE | CLF_RIGHT_ACTIVATE))
		color = maxColors / 2 - 1;
	else if (FlagsSet(clFlags, CLF_RIGHT_ACTIVATE))
		color = maxColors - 1;
	else
		color = 0;
}

UIW_COLOR_BAR::~UIW_COLOR_BAR(void)
{

}

EVENT_TYPE UIW_COLOR_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == S_NON_CURRENT)
		return (TRUE);

	UI_REGION region = trueRegion;
	display->VirtualGet(screenID, region);

	lastPalette = parent->LogicalPalette(ccode, parent->SearchID());
	UI_PALETTE palette = *lastPalette;
	palette.colorForeground = lastPalette->colorBackground;
	display->Line(screenID, region.left, region.top, region.right, region.top, &palette, 1, FALSE, &clip);
	display->Line(screenID, region.left, region.bottom, region.right, region.bottom, &palette, 1, FALSE, &clip);

	++region.top, --region.bottom;
	int borderWidth = FlagSet(woFlags, WOF_BORDER) ? 2 : 0;
	region.right = region.left + maxColors * cellWidth + borderWidth - 1;
	if (region.right < trueRegion.right)
		display->Rectangle(screenID, region.right + 1, region.top, trueRegion.right, region.bottom, &palette, 0, TRUE, FALSE, &clip);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	for (int i = 0; i < maxColors; i++)
	{
		region.right = region.left + cellWidth - 1;
		display->Rectangle(screenID, region, &display->colorMap[i], 0, TRUE, FALSE, &clip);
		if (i == color)
		{
			display->Rectangle(screenID, region.left, trueRegion.top, region.right,
				trueRegion.bottom, &display->colorMap[BLACK], 1, FALSE, FALSE, &clip);
		}
		region.left = region.right + 1;
	}

	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_COLOR_BAR::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		ccode = UI_WINDOW_OBJECT::Event(event);
		UI_REGION region = relative;
		int borderWidth = FlagSet(woFlags, WOF_BORDER) ? 2 : 0;
		if (FlagSet(woFlags, WOF_BORDER))
			--region;
		cellWidth = (region.Width() - borderWidth) / maxColors;
		}
		break;

	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		UI_WINDOW_OBJECT::Event(S_REGISTER_OBJECT);
		break;

	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
	case L_BEGIN_ESCAPE:
	case L_CONTINUE_ESCAPE:
	case L_END_ESCAPE:
		{
		// Check for a left or right selection.
		INFO_REQUEST request;
		if (FlagSet(clFlags, CLF_LEFT_ACTIVATE) &&
			(ccode == L_BEGIN_SELECT || ccode == L_CONTINUE_SELECT || ccode == L_END_SELECT))
			request = I_SET_LEFT_COLOR;
		else if (FlagSet(clFlags, CLF_RIGHT_ACTIVATE) &&
			(ccode == L_BEGIN_ESCAPE || ccode == L_CONTINUE_ESCAPE || ccode == L_END_ESCAPE))
			request = I_SET_RIGHT_COLOR;
		else
			request = FlagSet(clFlags, CLF_LEFT_ACTIVATE) ? I_SET_LEFT_COLOR : I_SET_RIGHT_COLOR;

		// Identify the color selection.
		UI_WINDOW_OBJECT::Event(event);
		// Normalize the coordinates.
		UI_POSITION position = event.position;
		NormalizePosition(this, event, position);
		// Check the overlap.
		UI_REGION region = trueRegion;
		if (FlagSet(woFlags, WOF_BORDER))
			--region;
		region.top++, region.bottom--;
		if (region.Overlap(position))
		{
			ZIL_COLOR newColor = (position.column - region.left) / cellWidth;
			if ((newColor != color || ccode == L_BEGIN_SELECT || ccode == L_BEGIN_ESCAPE) &&
				 newColor < maxColors)
			{
				if (FlagsSet(clFlags, CLF_LEFT_ACTIVATE | CLF_RIGHT_ACTIVATE))
				{
					color = BACKGROUND; // set screen and selected color.
					Root()->Information(request, &color);
					request = I_SET_SCREEN_COLOR;
				}
				// Redisplay the object and set the root color.
				color = newColor;
#if defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP)
				// Eliminate the flashing from an InvalRect() call.
				Event(S_DISPLAY_ACTIVE);
#else
				Event(S_REDISPLAY);
#endif
				Root()->Information(request, &color);
			}
		}
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_COLOR_BAR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	case I_GET_COLOR:
		*(ZIL_COLOR *)data = color;
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	return (data);
}
