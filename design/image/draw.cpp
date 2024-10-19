//	Zinc Application Framework - DRAW.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

int ZAF_IMAGE::Clear(void)
{
	memset(bitmapArray, 0x0F, bitmapWidth * bitmapHeight);
	return (TRUE);
}

int ZAF_IMAGE::Ellipse(int left, int top, int right, int bottom)
{
	// 4b^2x^2 + 4a^2y^2 = 4ab^2x + 4a^2by - a^2b^2
	int changed = FALSE;
	long w = right - left + 1;
	long h = bottom - top + 1;

	for (int x = 1; x <= w / 2; x++)
	{
		int y = 1;
		while (y < h && 4*h*h*x*x + 4*w*w*y*y > 4*w*h*h*x + 4*w*w*h*y - w*w*h*h)
			++y;
		changed |= Pixel(left + x, top + y - 1);
		changed |= Pixel(left + x, bottom - y + 1);
		changed |= Pixel(right - x, top + y - 1);
		changed |= Pixel(right - x, bottom - y + 1);
	}
	for (int y = 1; y <= h / 2; y++)
	{
		int x = 1;
		while (x < w && 4*h*h*x*x + 4*w*w*y*y > 4*w*h*h*x + 4*w*w*h*y - w*w*h*h)
			++x;
		changed |= Pixel(left + x - 1, top + y);
		changed |= Pixel(right - x + 1, top + y);
		changed |= Pixel(left + x - 1, bottom - y);
		changed |= Pixel(right - x + 1, bottom - y);
	}
	if (fill)
	{
 		for (int y = 0; y <= h / 2; y++)
			for (int x = 0; x <= w / 2; x++)
			{
 				if (4*h*h*x*x + 4*w*w*y*y <= 4*w*h*h*x + 4*w*w*h*y - w*w*h*h)
				{
					changed |= Pixel(left + x, top + y);
					changed |= Pixel(left + x, bottom - y);
					changed |= Pixel(right - x, top + y);
					changed |= Pixel(right - x, bottom - y);
				}
			}
	}
	return (changed);
}

int ZAF_IMAGE::Fill(int column, int line)
{
	ZIL_UINT8 oldColor = bitmapArray[line * bitmapWidth + column];
	if (oldColor == color)
		return (FALSE);

	int changed = FALSE;
	changed |= Pixel(column, line);
	ZIL_UINT8 *queue = new ZIL_UINT8[2 * bitmapWidth * bitmapHeight];
	ZIL_UINT8 head = 0;
	ZIL_UINT8 tail = 0;
	queue[head++] = (ZIL_UINT8)column;
	queue[head++] = (ZIL_UINT8)line;
	while (tail != head)
	{
		int x = queue[tail++];
		int y = queue[tail++];
		// move left 1 pixel.
		if (x > 0 && bitmapArray[y * bitmapWidth + x - 1] == oldColor)
		{
			changed |= Pixel(x - 1, y);
			queue[head++] = (ZIL_UINT8)(x - 1);
			queue[head++] = (ZIL_UINT8)y;
		}
		// move up 1 pixel.
		if (y > 0 && bitmapArray[(y - 1) * bitmapWidth + x] == oldColor)
		{
			changed |= Pixel(x, y - 1);
			queue[head++] = (ZIL_UINT8)x;
			queue[head++] = (ZIL_UINT8)(y - 1);
		}
		// move right 1 pixel.
		if (x < bitmapWidth - 1 && bitmapArray[y * bitmapWidth + x + 1] == oldColor)
		{
			changed |= Pixel(x + 1, y);
			queue[head++] = (ZIL_UINT8)(x + 1);
			queue[head++] = (ZIL_UINT8)y;
		}
		// move down 1 pixel.
		if (y < bitmapHeight - 1 && bitmapArray[(y + 1) * bitmapWidth + x] == oldColor)
		{
			changed |= Pixel(x, y + 1);
			queue[head++] = (ZIL_UINT8)x;
			queue[head++] = (ZIL_UINT8)(y + 1);
		}
	}
	delete queue;
	return (changed);
}

int ZAF_IMAGE::Line(int column1, int line1, int column2, int line2)
{
	// Check for a dot.
	if (column1 == column2 && line1 == line2)
		return (Pixel(column1, line1));

	// Draw the line.
	int changed = FALSE;
	int left = MinValue(column1, column2);
	int top = MinValue(line1, line2);
	int right = MaxValue(column1, column2);
	int bottom = MaxValue(line1, line2);
	int deltaX = right - left;
	int deltaY = bottom - top;
	int sign = (column2 > column1) ? (line2 > line1) : (line1 > line2);
	if (deltaX >= deltaY)
	{
		int adjust = deltaY ? deltaX / deltaY / 2 : 0;
		for (int x = 0; x <= deltaX; x++)
		{
			int y = deltaY * (x + adjust) / deltaX;
			if (sign)
				changed |= Pixel(left + x, top + y);
			else
				changed |= Pixel(left + x, bottom - y);
		}
	}
	else
	{
		int adjust = deltaX ? deltaY / deltaX / 2 : 0;
		for (int y = 0; y <= deltaY; y++)
		{
			int x = deltaX * (y + adjust) / deltaY;
			if (sign)
				changed |= Pixel(left + x, top + y);
			else
				changed |= Pixel(right - x, top + y);
		}
	}
	return (changed);
}

int ZAF_IMAGE::Pixel(int column, int line)
{
	// Draw the point on the bitmap.
	if (!bitmapArray || column < 0 || column >= bitmapWidth ||
		line < 0 || line >= bitmapHeight ||
		bitmapArray[line * bitmapWidth + column] == color)
		return (FALSE);
	bitmapArray[line * bitmapWidth + column] = (ZIL_UINT8)color;
	// Reflect the change to the display.
	if (updateDisplay)
	{
		UI_REGION region;
		region.left = trueRegion.left + column * pixelWidth;
		region.top = trueRegion.top + line * pixelHeight;
		region.right = region.left + pixelWidth;
		region.bottom = region.top + pixelHeight;
		if (grid)
			--region;
#if defined(ZIL_MOTIF)
		if (FlagSet(woFlags, WOF_BORDER))
			region.left++, region.top++, region.right++, region.bottom++;
#else
		else if (FlagSet(woFlags, WOF_BORDER))
			region.left++, region.top++;
#endif
		if (color == BACKGROUND)
		{
			ZIL_COLOR screenColor;
			imageEditor->Information(I_GET_SCREEN_COLOR, &screenColor);
			display->Rectangle(screenID, region, &display->colorMap[(int)screenColor], 0, TRUE);
		}
		else
			display->Rectangle(screenID, region, &display->colorMap[(int)color], 0, TRUE);
	}
	return (TRUE);
}

int ZAF_IMAGE::Rectangle(int left, int top, int right, int bottom)
{
	// Draw the rectangle.
	int changed = FALSE;
	if (fill)
	{
		for (int line = top; line <= bottom; line++)
			for (int column = left; column <= right; column++)
				changed |= Pixel(column, line);
	}
	else
	{
		changed |= Line(left, top, right, top);
		changed |= Line(left, bottom, right, bottom);
		changed |= Line(left, top, left, bottom);
		changed |= Line(right, top, right, bottom);
	}
	return (changed);
}
