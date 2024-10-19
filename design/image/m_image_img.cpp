//	Zinc Application Framework - M_IMAGE.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"


EVENT_TYPE ZAF_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Determine the update.
	if (event.type == S_NON_CURRENT || event.type == S_CURRENT)
		return (ccode);

	UI_REGION region = trueRegion;

	// check the grid information.
	grid = (FlagSet(imFlags, IMF_GRID) && bitmapWidth <= 32 && bitmapHeight <= 32) ? 1 : 0;
		
	// Check for a valid bitmap.
	if (!bitmapArray || !bitmapWidth || !bitmapHeight)
	{
		display->Rectangle(screenID, trueRegion, &display->colorMap[WHITE], 0, TRUE);
		return (ccode);
	}

	// Convert the background colors.
	ZIL_COLOR screenColor;
	imageEditor->Information(I_GET_SCREEN_COLOR, &screenColor);

	// Draw the image.
	display->VirtualGet(screenID, region);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (pixelWidth == 1 && pixelHeight == 1)
	{
		ZIL_BITMAP_HANDLE colorBitmap = 0, monoBitmap = 0;
		for (int line = region.top; line <= region.bottom; line += bitmapHeight)
		{
			for (int column = region.left; column <= region.right; column += bitmapWidth)
				display->Bitmap(screenID, column, line, bitmapWidth,
					bitmapHeight, bitmapArray, ZIL_NULLP(UI_PALETTE), ZIL_NULLP(UI_REGION),
					&colorBitmap, &monoBitmap);
		}
	}
	else
	{
		UI_PALETTE color;
		ZIL_COLOR colorForeground = display->colorMap[BLACK].colorForeground;
		int deltaY = 0;
		int left = region.left - grid;
		int top = region.top - grid;
		int right = region.right;
		int bottom = region.bottom;
		if (FlagSet(woFlags, WOF_BORDER))
			right += 2, bottom += 2;
		for (int line = top; line <= bottom; line += pixelHeight, deltaY++)
		{
			if (deltaY == bitmapHeight)
				deltaY = 0;
			int offset = deltaY * bitmapWidth;
			int deltaX = 0;
			for (int column = left; column <= right; column += pixelWidth, deltaX++)
			{
				if (deltaX == bitmapWidth)
				{
					deltaX = 0;
					offset = deltaY * bitmapWidth;
				}
				if (bitmapArray[offset] == (ZIL_UINT8)BACKGROUND)
					color = display->colorMap[screenColor];
				else
					color = display->colorMap[bitmapArray[offset]];
				color.colorForeground = colorForeground;
				display->Rectangle(screenID, column, line,
					column + pixelWidth + grid - 1,
					line + pixelHeight + grid - 1,
					&color, grid, TRUE, FALSE);
				offset++;
			}
		}
	}
	display->VirtualPut(screenID);

	// Return the control code.
	return (ccode);
}
