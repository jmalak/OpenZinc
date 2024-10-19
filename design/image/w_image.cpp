//	Zinc Application Framework - W_IMAGE.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

EVENT_TYPE ZAF_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_REGION region = trueRegion;

	// Determine the update.
	if (event.type == S_NON_CURRENT || event.type == S_CURRENT)
	{
		display->VirtualGet(screenID, region);
		display->VirtualPut(screenID);
		return (TRUE);
	}

	// check the grid information.
	grid = (FlagSet(imFlags, IMF_GRID) && bitmapWidth <= 32 && bitmapHeight <= 32) ? 1 : 0;
		
	// Check for a valid bitmap.
	if (!bitmapArray || !bitmapWidth || !bitmapHeight)
	{
		display->VirtualGet(screenID, region);
		display->Rectangle(screenID, region, &display->colorMap[(int)WHITE], 0, TRUE);
		display->VirtualPut(screenID);
		return (TRUE);
	}

	// Draw the image.
	display->VirtualGet(screenID, region);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);

	HDC hDC = UI_MSWINDOWS_DISPLAY::hDC;
	// Define the stretch region.
	int left = region.left - trueRegion.left;
	int top = region.top - trueRegion.top;
	int right = region.Width();
	int bottom = region.Height();

	// Convert the background colors.
	ZIL_COLOR screenColor;
	imageEditor->Information(I_GET_SCREEN_COLOR, &screenColor);
	ZIL_UINT8 *bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	memcpy(bitmapArray, ZAF_IMAGE::bitmapArray, bitmapWidth * bitmapHeight);
	for (ZIL_UINT8 *temp = &bitmapArray[bitmapWidth * bitmapHeight - 1]; temp >= bitmapArray; temp--)
		if (*temp == (ZIL_UINT8)BACKGROUND)
			*temp = (ZIL_UINT8)screenColor;

	// Draw the bitmap.
	// convert the image
	ZIL_BITMAP_HANDLE colorBitmap = 0;
	display->BitmapArrayToHandle(screenID, bitmapWidth,
		bitmapHeight, bitmapArray, ZIL_NULLP(UI_PALETTE),
		&colorBitmap, ZIL_NULLP(ZIL_BITMAP_HANDLE));
	// draw the image
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, colorBitmap);
	StretchBlt(hDC, left, top, right - left + 1, bottom - top + 1, hMemDC,
		0, 0, bitmapWidth, bitmapHeight, SRCCOPY);
		DeleteDC(hMemDC);
	// cleanup
	DeleteObject(colorBitmap);

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
				&display->colorMap[(int)LIGHTGREEN], 1, FALSE, FALSE);
	}

	// Clean up.
	display->VirtualPut(screenID);
	delete bitmapArray;

	// Return the control code.
	return (TRUE);
}

