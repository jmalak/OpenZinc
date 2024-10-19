//	Zinc Interface Library - M_BITMAP.CPP
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

#if defined(ZIL_XKEY)
#	include <X11/StringDefs.h>
#endif


// ZincToXPixmap converts a Zinc bitmap to a motif pixmap, complete
// with justified text.
void ZincToXPixmap(UI_DISPLAY *display, int bitmapWidth, int bitmapHeight,
	ZIL_UINT8 *bitmapArray, UI_PALETTE *paletteArray, Pixmap *pixmap,
	UI_WINDOW_OBJECT *object, ZIL_ICHAR *text, int font, WOF_FLAGS woFlags,
	int &_width, int &_height, int offset)
{
	ZIL_UINT8 *pixel;
	int column, row;
	ZIL_COLOR background, foreground;
	Arg args[2];

	// Get default background.
	XtSetArg(args[0], XtNbackground, &background);
	XtGetValues(object->screenID, args, 1);
	XtSetArg(args[0], XtNforeground, &foreground);
	XtGetValues(object->screenID, args, 1);

	int height = bitmapHeight;
	int width = bitmapWidth + offset;

	int pictureX = 0, pictureY = 0;
	int textX, textY;
	int textWidth = display->TextWidth(text, ZIL_NULLP(_WidgetRec), font);
	int textHeight = display->TextHeight(text, ZIL_NULLP(_WidgetRec), font);
	int bWidth = width;
	int bHeight = height;

	// Set the font on the gc before writing to the pixmap.
#if !defined(ZIL_UNICODE)
	XSetFont(display->xDisplay, display->xGC,
		((UI_XT_DISPLAY *)display)->fontTable[font].fontStruct->fid);
#endif

	if (text && !*text)
		text = 0;

	if (FlagSet(WOF_JUSTIFY_RIGHT, woFlags))
	{
		if (text)
		{
			width = bWidth + textWidth + display->cellWidth * 2;
			height = MaxValue(bHeight + 8, textHeight);
		}

		pictureX = width - bitmapWidth + offset;
		pictureY = height/2 - bitmapHeight/2;
		textX = display->cellWidth + offset;
		textY = bHeight / 2;
	}
	else if (FlagSet(WOF_JUSTIFY_CENTER, woFlags))
	{
		if (text)
		{
			height = bHeight + textHeight + 8;
			width = MaxValue(bWidth + 8, textWidth);
		}
		pictureX = width/2 - bitmapWidth/2;
		pictureY = height/2 - bitmapHeight/2;
		if (text)
			pictureY -= 4;
		textX = width/2 - textWidth/2;
		textY = bHeight + textHeight;
	}
	else
	{
		// Left justified.
		if (text)
		{
			width = bWidth + textWidth + display->cellWidth * 2;
			height = MaxValue(bHeight + 8, textHeight);
		}
		pictureX = offset;
		pictureY = height/2 - bitmapHeight/2;
		textX = bitmapWidth + 4 + display->cellWidth + offset;
		textY = bHeight / 2;
	}

	*pixmap = XCreatePixmap(display->xDisplay, display->xScreen->root,
		width, height,
		XDefaultDepth(display->xDisplay, display->xScreenNumber));

	XSetFillStyle(display->xDisplay, display->xGC, FillSolid);

	// Clear background.
	XSetForeground(display->xDisplay, display->xGC, background);
	XFillRectangle(display->xDisplay, *pixmap, display->xGC, 0, 0,
		width, height);

	if (pixel = bitmapArray)
		for (row = pictureY; row - pictureY < bitmapHeight; row++)
		{
			for (column = pictureX; column - pictureX < bitmapWidth; column++)
			{
				ZIL_COLOR color;
				if (paletteArray)
					color = display->MapColor(&paletteArray[*pixel], TRUE);
				else if (*pixel <= MAX_COLORMAP_INDEX)
					color = display->MapColor(&display->colorMap[*pixel], TRUE);
				else
					color = background;
				pixel++;
				if (color != background)
				{
					XSetForeground(display->xDisplay, display->xGC, color);
					XDrawPoint(display->xDisplay, *pixmap, display->xGC,
						column, row);
				}
			}
		}

	// Add text, if any.
	if (text)
	{
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			XSetFillStyle(display->xDisplay, display->xGC, FillStippled);
		XSetForeground(display->xDisplay, display->xGC, foreground);
#if defined(ZIL_UNICODE)
		char *cText = ZIL_INTERNATIONAL::MapText(text);
		XmbDrawString(display->xDisplay, *pixmap,
			UI_XT_DISPLAY::fontTable[font].fontSet, display->xGC,
			textX, textY + textHeight / 2, cText, ::strlen(cText));
		delete cText;
#else
		XDrawString(display->xDisplay, *pixmap, display->xGC,
			textX, textY + textHeight / 2, text, object->strlen(text));
#endif
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			XSetFillStyle(display->xDisplay, display->xGC, FillSolid);
	}

	if (_width == 0)
		_width = width;
	if (_height == 0)
		_height = height;
}

