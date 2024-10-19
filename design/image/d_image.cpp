//	Zinc Application Framework - D_IMAGE.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

EVENT_TYPE ZAF_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Determine the update.
	if (event.type == S_NON_CURRENT)
		return (ccode);

	// Determine the update region.
	UI_REGION region = trueRegion;
	UI_REGION updateRegion;
	event.region.Overlap(region, updateRegion);

	// check the grid information.
	grid = (FlagSet(imFlags, IMF_GRID) && bitmapWidth <= 32 && bitmapHeight <= 32) ? 1 : 0;

	// Check for a valid bitmap.
	if (!bitmapArray || !bitmapWidth || !bitmapHeight)
	{
		display->Rectangle(screenID, updateRegion, &display->colorMap[WHITE], 0, TRUE, FALSE, &updateRegion);
		return (ccode);
	}

	// Convert the background colors.
	ZIL_COLOR screenColor;
	imageEditor->Information(I_GET_SCREEN_COLOR, &screenColor);
	ZIL_UINT8 *bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	memcpy(bitmapArray, ZAF_IMAGE::bitmapArray, bitmapWidth * bitmapHeight);
	for (ZIL_UINT8 *temp = &bitmapArray[bitmapWidth * bitmapHeight - 1]; temp >= bitmapArray; temp--)
		if (*temp == (ZIL_UINT8)BACKGROUND)
			*temp = (ZIL_UINT8)screenColor;

	// Draw the image.
	display->VirtualGet(screenID, region);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (pixelWidth == 1 && pixelHeight == 1)
	{
		ZIL_BITMAP_HANDLE colorBitmap = 0;
		for (int line = region.top; line <= region.bottom; line += bitmapHeight)
		{
			for (int column = region.left; column <= region.right; column += bitmapWidth)
				display->Bitmap(screenID, column, line, bitmapWidth,
					bitmapHeight, bitmapArray, ZIL_NULLP(UI_PALETTE),
					&updateRegion, &colorBitmap);
		}
		delete colorBitmap;
	}
	else
	{
		int deltaY = 0;
		int left = region.left - grid;
		int top = region.top - grid;
		int right = region.right;
		int bottom = region.bottom;
		if (FlagSet(woFlags, WOF_BORDER))
			right--, bottom--;
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
				// Draw the pixel.
				if (column + pixelWidth >= updateRegion.left &&
					line + pixelHeight >= updateRegion.top &&
					column <= updateRegion.right &&
					line <= updateRegion.bottom)
				{
					display->Rectangle(screenID, column + grid, line + grid,
						column + pixelWidth - 1, line + pixelHeight - 1,
						&display->colorMap[bitmapArray[offset]],
						0, TRUE, FALSE, &updateRegion);
				}
				offset++;
			}
		}
	}

	// Draw the bounding rectangle.
	if (grid && (pixelHeight != 1 || pixelWidth != 1))
	{
		int left = region.left - grid;
		int top = region.top - grid;
		int right = region.right;
		int bottom = region.bottom;
		for (int line = top; line <= bottom; line += pixelHeight)
			display->Line(screenID, left, line, right, line, &display->colorMap[BLACK], 1, FALSE, &updateRegion);
		for (int column = left; column <= right; column += pixelWidth)
			display->Line(screenID, column, top, column, bottom, &display->colorMap[BLACK], 1, FALSE, &updateRegion);
		// Draw the paste area.
		if (pasteObject == this)
			display->Rectangle(screenID,
				trueRegion.left + pasteRegion.left * pixelWidth,
				trueRegion.top + pasteRegion.top * pixelHeight,
				trueRegion.left + (pasteRegion.right + 1) * pixelWidth,
				trueRegion.top + (pasteRegion.bottom + 1) * pixelHeight,
				&display->colorMap[LIGHTGREEN], 1, FALSE, FALSE, &updateRegion);
	}

	// Clean up.
	display->VirtualPut(screenID);
	delete bitmapArray;

	// Return the control code.
	return (ccode);
}
