//	Zinc Interface Library - D_IMAGE.CPP
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


#include <ui_win.hpp>
#if defined(ZIL_CURSES) || defined(ZIL_TEXT_ONLY)
EVENT_TYPE UIW_IMAGE::DrawItem(const UI_EVENT &, EVENT_TYPE) { return (0); }
int UIW_IMAGE::LoadImageFromApplication(void) { return (FALSE); }
int UIW_IMAGE::LoadImageFromFile(void) { return (FALSE); }
void UIW_IMAGE::DestroyImageHandle(void) {}
#else
extern "C"
{
#	include <gfx.h>
#	include <gfx_pro.h>
}

// --------------------------------------------------------------------------
// ----- UIW_IMAGE ----------------------------------------------------------
// --------------------------------------------------------------------------

EVENT_TYPE UIW_IMAGE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	if (display->isText)
		return (FALSE);

	// Virtualize the display.
	UI_REGION tRegion, region = trueRegion;
	display->VirtualGet(screenID, region);

	UI_REGION parentRegion;
	parent->Information(I_GET_CLIPREGION, &parentRegion);

	UI_REGION clipRegion;

	clipRegion.left = MaxValue(event.region.left, parentRegion.left);
	clipRegion.top = MaxValue(event.region.top, parentRegion.top);
	clipRegion.right = MinValue(event.region.right, parentRegion.right);
	clipRegion.bottom = MinValue(event.region.bottom, parentRegion.bottom);

	// Draw the background of the image region.
	lastPalette = LogicalPalette(PM_ANY, ID_WINDOW_OBJECT);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clipRegion);

	// Draw the border if necessary.
	if (IsBordered())
		DrawBorder(screenID, region, FALSE, ccode);

#if defined(ZIL_UNICODE)
	char *mapped = MapText(pathName);
#	define Mapped mapped
#else
#	define Mapped pathName
#endif

	for (UI_REGION_ELEMENT *dRegion = ((UI_GRAPHICS_DISPLAY *)display)->First();
			dRegion; dRegion = dRegion->Next())
		if (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion))
		{
			_gfx.min_x = MaxValue(parentRegion.left, tRegion.left);
			_gfx.min_y = MaxValue(parentRegion.top, tRegion.top);
			_gfx.max_x = MinValue(parentRegion.right, tRegion.right);
			_gfx.max_y = MinValue(parentRegion.bottom, tRegion.bottom);

 			if (_gfx.max_x <= _gfx.min_x || _gfx.max_y <= _gfx.min_y)
				continue;
			
			// Draw the image.
			int left = region.left;
			int top = region.top;
			int width = region.Width();
			int height = region.Height();

			if (IsTiled())
			{
				for (int y = top; y < top + height; y += imageHeight)
					for (int x = left; x < left + width; x += imageWidth)
						PutPCXFile(x, y, Mapped, 'P' + NO_PCX_PAL);
			}
			else
  				PutPCXFile(left, top, Mapped, 'P' + NO_PCX_PAL);
		}
#if defined(ZIL_UNICODE)
	delete mapped;
#endif

	// Un-virtualize the display.
	display->VirtualPut(screenID);
	return (ccode);
}

int UIW_IMAGE::LoadImageFromApplication(void)
{
	return (FALSE);
}

int UIW_IMAGE::LoadImageFromFile(void)
{
	if (display->isText)
		return (FALSE);

	// Open the file.
	ZIL_ICHAR _pathName[ZIL_MAXPATHLEN];
	strcpy(_pathName, pathName);
	strcat(_pathName, ZIL_TEXT(".pcx"));
	if (pathName)
		delete pathName;
	pathName = strdup(_pathName);
	PCX_HDR pcxHdr;
#if defined(ZIL_UNICODE)
	char *mapped = MapText(_pathName);
	ReadPCXHeader(mapped, &pcxHdr);
	delete mapped;
#else
	ReadPCXHeader(_pathName, &pcxHdr);
#endif

	imageWidth = pcxHdr.max_x;
	imageHeight = pcxHdr.max_y;
	
	return (FALSE);
}

void UIW_IMAGE::DestroyImageHandle(void)
{
}
#endif

