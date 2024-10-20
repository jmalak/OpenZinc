//	Zinc Interface Library - M_IMAGE.CPP
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

// --------------------------------------------------------------------------
// ----- UIW_IMAGE ----------------------------------------------------------
// --------------------------------------------------------------------------

void UIW_IMAGE::DestroyImageHandle(void)
{
	// Destroy the image handle.
	if (image)
		XFreePixmap(display->xDisplay, image);
	image = 0;
}

EVENT_TYPE UIW_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event;

	// Virtualize the display.
	UI_REGION region = trueRegion;
	display->VirtualGet(screenID, region);

	// Set the foreground and background colors for the bitmap.
	static UI_PALETTE palette = 
		{ ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, WHITE, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE };
	unsigned long foreground = display->MapColor(&palette, TRUE);
	unsigned long background = display->MapColor(&palette, FALSE);
	XSetForeground(display->xDisplay, display->xGC, foreground);
	XSetBackground(display->xDisplay, display->xGC, background);

	// Draw the border if necessary.
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (!image)
	{
		display->VirtualPut(screenID);
		return (S_ERROR);
	}

	// Draw the image.
	int left = region.left - trueRegion.left;
	int top = region.top - trueRegion.top;
	unsigned int width = region.Width();
	unsigned int height = region.Height();

	if (IsScaled())
		;
	else if (IsTiled())
	{
		for (int y = top; y < height; y += imageHeight)
			for (int x = left; x < width; x += imageWidth)
			{
				unsigned int sizeX = (x + imageWidth) < width ? imageWidth : width - x + 1;
				unsigned int sizeY = (y + imageHeight) < height ? imageHeight : height - y + 1;
				XCopyPlane(display->xDisplay, image, XtWindow(screenID),
					display->xGC, 0, 0, sizeX, sizeY, x, y, 1);
			}
	}
	else
		XCopyPlane(display->xDisplay, image, XtWindow(screenID),
			display->xGC, 0, 0, width, height, left, top, 1);

	// Un-virtualize the display.
	display->VirtualPut(screenID);
	return (ccode);
}

int UIW_IMAGE::LoadImageFromApplication(void)
{
	// Try to load the bitmap from the system.
	woStatus |= WOS_READ_ERROR;
	return (image ? TRUE : FALSE);
}

static const ZIL_ICHAR _xbm[] = { '.','x','b','m',0 };

int UIW_IMAGE::LoadImageFromFile(void)
{
	// Open the file.
	woStatus |= WOS_READ_ERROR;
	ZIL_ICHAR _pathName[ZIL_MAXPATHLEN];
	strcpy(_pathName, pathName);
	strcat(_pathName, _xbm);
	ZIL_FILE file(_pathName);

	// Load the bitmap.
	int xHotReturn, yHotReturn;
	unsigned int width, height;
#if defined(ZIL_UNICODE)
	char *tmp = MapText(_pathName);
	XReadBitmapFile(display->xDisplay, RootWindow(display->xDisplay, display->xScreenNumber),
		tmp, &width, &height, &image, &xHotReturn, &yHotReturn);
	delete tmp;	
#else
	XReadBitmapFile(display->xDisplay, RootWindow(display->xDisplay, display->xScreenNumber),
		(char *)_pathName, &width, &height, &image, &xHotReturn, &yHotReturn);
#endif
	imageWidth = width;
	imageHeight = height;

	woStatus &= ~WOS_READ_ERROR;
	return (image ? TRUE : FALSE);
}
