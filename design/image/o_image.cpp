//	Zinc Application Framework - O_IMAGE.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_GPI
#include "image.hpp"

EVENT_TYPE ZAF_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Determine the update.
	if (event.type == S_NON_CURRENT)
		return (ccode);

	// Determine the update region.
	UI_REGION region = trueRegion;

	// check the grid information.
	grid = (FlagSet(imFlags, IMF_GRID) && bitmapWidth <= 32 && bitmapHeight <= 32) ? 1 : 0;
		
	// Check for a valid bitmap.
	if (!bitmapArray || !bitmapWidth || !bitmapHeight)
	{
		display->VirtualGet(screenID, region);
		display->Rectangle(screenID, region, &display->colorMap[WHITE], 0, TRUE);
		display->VirtualPut(screenID);
		return (ccode);
	}

	// Draw the image.
	display->VirtualGet(screenID, region);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);

	// Convert the background colors.
	ZIL_COLOR screenColor;
	imageEditor->Information(I_GET_SCREEN_COLOR, &screenColor);
	ZIL_UINT8 *bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	memcpy(bitmapArray, ZAF_IMAGE::bitmapArray, bitmapWidth * bitmapHeight);
	for (ZIL_UINT8 *temp = &bitmapArray[bitmapWidth * bitmapHeight - 1]; temp >= bitmapArray; temp--)
		if (*temp == (ZIL_UINT8)BACKGROUND)
			*temp = (ZIL_UINT8)screenColor;

	HPS hps = UI_OS2_DISPLAY::hps;
	// Define the stretch region.
	POINTL point[4];
	point[0].x = region.left - trueRegion.left;
	point[0].y = region.top - trueRegion.top;
	point[1].x = region.right - region.left + 1;
	point[1].y = region.bottom - region.top + 1;
	point[2].x = point[2].y = 0;
	point[3].x = bitmapWidth;
	point[3].y = bitmapHeight;

	// Draw the bitmap.
	ZIL_BITMAP_HANDLE colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	display->BitmapArrayToHandle(screenID, bitmapWidth,
		bitmapHeight, bitmapArray, ZIL_NULLP(UI_PALETTE),
		&colorBitmap, 0);
	GpiWCBitBlt(hps, colorBitmap, 4, point, ROP_SRCCOPY, BBO_IGNORE);
	GpiDeleteBitmap(colorBitmap);

	// Draw the bounding rectangle.
	if (grid && (pixelHeight != 1 || pixelWidth != 1))
	{
		int left = region.left - grid;
		int top = region.top - grid;
		int right = region.right;
		int bottom = region.bottom;
		for (int line = top; line <= bottom; line += pixelHeight)
			display->Line(screenID, left, line, right, line, &display->colorMap[BLACK], 1, FALSE);
		for (int column = left; column <= right; column += pixelWidth)
			display->Line(screenID, column, top, column, bottom, &display->colorMap[BLACK], 1, FALSE);
		// Draw the paste area.
		if (pasteObject == this)
			display->Rectangle(screenID,
				trueRegion.left + pasteRegion.left * pixelWidth,
				trueRegion.top + pasteRegion.top * pixelHeight,
				trueRegion.left + (pasteRegion.right + 1) * pixelWidth,
				trueRegion.top + (pasteRegion.bottom + 1) * pixelHeight,
				&display->colorMap[LIGHTGREEN], 1, FALSE, FALSE);
	}

	// Clean up.
	display->VirtualPut(screenID);
	delete bitmapArray;

	// Return the control code.
	return (ccode);
}

