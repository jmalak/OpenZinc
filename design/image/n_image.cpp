//	Zinc Application Framework - N_IMAGE.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
extern "Objective-C"
{
#	import <appkit/NXBitmapImageRep.h>
#	import <appkit/View.h>
#	import <appkit/Window.h>
}

EVENT_TYPE ZAF_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Determine the update.
	if (event.type == S_NON_CURRENT)
		return (ccode);

	UI_REGION region = trueRegion;
	UI_REGION updateRegion;
	event.region.Overlap(region, updateRegion);

	// check the grid information.
	grid = (FlagSet(imFlags, IMF_GRID) && bitmapWidth <= 32 && bitmapHeight <= 32) ? 1 : 0;
		
	// Check for a valid bitmap.
	if (!bitmapArray || !bitmapWidth || !bitmapHeight)
	{
		display->VirtualGet(screenID, region);
		//display->Rectangle(screenID, region, &display->colorMap[WHITE], 0, TRUE);
		display->Rectangle(screenID, updateRegion, &display->colorMap[WHITE], 0, TRUE, FALSE, &updateRegion);
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
	
	// convert the image
	ZIL_BITMAP_HANDLE colorBitmap = 0;
	display->BitmapArrayToHandle(screenID, bitmapWidth,
		bitmapHeight, bitmapArray, ZIL_NULLP(UI_PALETTE),
		&colorBitmap, ZIL_NULLP(ZIL_BITMAP_HANDLE));
		
	// Composite the bitmap onto the view.
	NXPoint thePoint;
	thePoint.x = region.left - trueRegion.left;
	thePoint.y = region.Height();
	[screenID lockFocus];
	if (colorBitmap)
	{
		NXSize newSize;
		newSize.width = region.Width();
		newSize.height = thePoint.y;
		[colorBitmap setScalable: YES];
		[colorBitmap setSize:&newSize];
		[colorBitmap composite:NX_SOVER toPoint:&thePoint];
	}
	[screenID unlockFocus];
		
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
	}
	display->VirtualPut(screenID);
	// Return the control code.
	return (ccode);
}

