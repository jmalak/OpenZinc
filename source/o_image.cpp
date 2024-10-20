//	Zinc Interface Library - W_IMAGE.CPP
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


#if !defined(INCL_GPIBITMAPS)
	#define INCL_GPIBITMAPS		// OS/2 GpiQueryBitmapParameters
#endif
#include "ui_win.hpp"

// --------------------------------------------------------------------------
// ----- UIW_IMAGE ----------------------------------------------------------
// --------------------------------------------------------------------------

void UIW_IMAGE::DestroyImageHandle(void)
{
	// Destroy the image handle.
	if (image)
		GpiDeleteBitmap(image);
	image = 0;
}

EVENT_TYPE UIW_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Virtualize the display.
	UI_REGION region = trueRegion;
	display->VirtualGet(screenID, region);

	/* unused parameters */ (void)event;

	// Draw the border if necessary.
	if (IsBordered())
		DrawBorder(screenID, region, FALSE, ccode);
	if (!image)
	{
		display->VirtualPut(screenID);
		return (S_ERROR);
	}

	// Determine the image position.
	int left = region.left - trueRegion.left;
	int top = region.top - trueRegion.top;
	int width = region.Width();
	int height = region.Height();

	POINTL point[4];
	point[2].x = point[2].y = 0; // source location.
	point[3].x = imageWidth;
	point[3].y = imageHeight;

	// Draw the image.
	HPS hps = UI_OS2_DISPLAY::hps;
	if (IsScaled())
	{
		point[0].x = left; // target location.
		point[0].y = top;
		point[1].x = left + width - 1;
		point[1].y = top + height - 1;
		GpiWCBitBlt(hps, image, 4, point, ROP_SRCCOPY, BBO_IGNORE);
	}
	else if (IsTiled())
	{
		for (int y = top; y < height; y += imageHeight)
			for (int x = left; x < width; x += imageWidth)
			{
				int sizeX = (x + imageWidth) < width ? imageWidth : width - x + 1;
				int sizeY = (y + imageHeight) < height ? imageHeight : height - y + 1;
				point[0].x = x; // target location.
				point[0].y = height - y - sizeY + 2;
				point[1].x = point[0].x + sizeX - 1;
				point[1].y = point[0].y + sizeY - 1;
				point[2].y = imageHeight - sizeY; // source location.
				point[3].x = sizeX;
				point[3].y = imageHeight;
				GpiWCBitBlt(hps, image, 4, point, ROP_SRCCOPY, BBO_IGNORE);
			}
	}
	else
	{
		point[0].x = left; // target location.
		point[0].y = height - top - imageHeight + 2;
		point[1].x = point[0].x + imageWidth - 1;
		point[1].y = point[0].y + imageHeight - 1;
		GpiWCBitBlt(hps, image, 4, point, ROP_SRCCOPY, BBO_IGNORE);
	}

	// Un-virtualize the display.
	display->VirtualPut(screenID);
	return (ccode);
}

int UIW_IMAGE::LoadImageFromApplication(void)
{
	// Try to load the bitmap from the system (requires pathID).
	woStatus |= WOS_READ_ERROR;
	if (pathID >= 0)
	{
		HPS hps = WinGetPS(HWND_DESKTOP);
		image = GpiLoadBitmap(hps, ZIL_NULLH(HMODULE), (ULONG)pathID, 0, 0);
		if (image)
		{
			BITMAPINFOHEADER info;
			GpiQueryBitmapParameters(image, &info);
			imageWidth = info.cx;
			imageHeight = info.cy;
			woStatus &= ~WOS_READ_ERROR;
		}
		WinReleasePS(hps);
	}
	return (image ? TRUE : FALSE);
}

int UIW_IMAGE::LoadImageFromFile(void)
{
	// Open the file.
	woStatus |= WOS_READ_ERROR;
	ZIL_ICHAR _pathName[ZIL_MAXPATHLEN];
	strcpy(_pathName, pathName);
	strcat(_pathName, ZIL_TEXT(".bmp"));
	ZIL_FILE file(_pathName);

	// Load the file header.
	BITMAPFILEHEADER bmiFileHeader;
	file.Read(&bmiFileHeader, sizeof(BITMAPFILEHEADER)-sizeof(BITMAPINFOHEADER), 1);
	if (bmiFileHeader.usType != 0x4D42) // 'BM' format.
		return (FALSE);

	// Load the info header.
	BITMAPINFO2 *bmiInfo = (BITMAPINFO2 *)new char[sizeof(BITMAPINFOHEADER2)+sizeof(RGB2)*256];
	if (bmiFileHeader.cbSize != sizeof(BITMAPFILEHEADER2))
	{
		file.Read(&bmiFileHeader.bmp, sizeof(BITMAPINFOHEADER), 1);
		bmiInfo->cx = bmiFileHeader.bmp.cx;
		bmiInfo->cy = bmiFileHeader.bmp.cy;
		bmiInfo->cPlanes = bmiFileHeader.bmp.cPlanes;
		bmiInfo->cBitCount = bmiFileHeader.bmp.cBitCount;
		bmiInfo->usUnits = bmiInfo->usReserved = bmiInfo->usRecording =
			bmiInfo->usRendering = 0; // ZIL_UINT16 variables.
    	bmiInfo->ulCompression = bmiInfo->cbImage = bmiInfo->cxResolution = 
			bmiInfo->cyResolution = bmiInfo->cclrUsed =
			bmiInfo->cclrImportant = bmiInfo->cSize1 =
			bmiInfo->cSize2 = bmiInfo->ulColorEncoding =
			bmiInfo->ulIdentifier = 0; // ZIL_UINT32 variables.
	}
	else
		file.Read(bmiInfo, sizeof(BITMAPINFOHEADER2), 1);
	bmiInfo->cbFix = sizeof(BITMAPINFOHEADER2);
	imageWidth = (int)bmiInfo->cx;
	imageHeight = (int)bmiInfo->cy;

	// Load the color mapping.
	RGB rgb;
	int colorsUsed = 0x01 << bmiInfo->cBitCount;
	for (int i = 0; i < colorsUsed; i++)
	{
		file.Read(&rgb, sizeof(RGB), 1);
		bmiInfo->argbColor[i].bBlue = rgb.bBlue;
		bmiInfo->argbColor[i].bGreen = rgb.bGreen;
		bmiInfo->argbColor[i].bRed = rgb.bRed;
		bmiInfo->argbColor[i].fcOptions = 0;
	}

	// Load the bitmap.
	ULONG dibSize = bmiInfo->cbImage;
	if (!dibSize)
	{
		dibSize = bmiInfo->cx * bmiInfo->cBitCount / 8;
		dibSize = (dibSize + sizeof(ZIL_UINT32) - 1) / sizeof(ZIL_UINT32) * sizeof(ZIL_UINT32);
		dibSize *= bmiInfo->cy;
	}
	ULONG tdibSize = dibSize;
	ZIL_UINT8 *dib;
	DosAllocMem((PPVOID)&dib, dibSize, fALLOC);
	ZIL_UINT8 *tdib = dib;
	for ( ; dibSize; dibSize -= tdibSize, tdib += tdibSize)
	{
		tdibSize = (30000 < dibSize) ? 30000 : dibSize;
		file.Read(tdib, sizeof(ZIL_UINT8), (int)tdibSize);
	}

	// Create the image.
	HPS hps = WinGetPS(HWND_DESKTOP);
	image = GpiCreateBitmap(hps, (PBITMAPINFOHEADER2)bmiInfo, CBM_INIT, (PBYTE)dib, bmiInfo);
	WinReleasePS(hps);

	// Clean up.
	DosFreeMem(dib);
   	delete bmiInfo;
	woStatus &= ~WOS_READ_ERROR;
	return (image ? TRUE : FALSE);
}

