//      Zinc Interface Library - D_WCCDSP.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <malloc.h>
#include <stdio.h>
//#include <iostream.h>
#include <dos.h>
#include <graph.h>
#include "ui_dsp.hpp"

#define RGB(r,g,b) (((long) ((b) << 8 | (g)) << 8) | (r))

// ----- Static member variables --------------------------------------------

UI_PATH *UI_WCC_DISPLAY::searchPath = ZIL_NULLP(UI_PATH);
UI_WCC_DISPLAY::WCCFONT UI_WCC_DISPLAY::fontTable[ZIL_MAXFONTS] =
{
        { "Helv", "n6" },       // FNT_SMALL_FONT
        { "Helv", "n5" },       // FNT_DIALOG_FONT
        { "Helv", "n7" },       // FNT_SYSTEM_FONT
        { "Helv", "n6" },
        { "Helv", "n6" },
        { "Helv", "n6" },
        { "Helv", "n6" },
        { "Helv", "n6" },
        { "Helv", "n6" },
        { "Helv", "n6" }
};

/* START BLOCK COMMENT
**      UI_WCC_DISPLAY::WCCFONT UI_WCC_DISPLAY::fontTable[ZIL_MAXFONTS] =
**      {
**              { "Helv", "bh8" },      // FNT_SMALL_FONT
**              { "Helv", "bh10" },     // FNT_DIALOG_FONT
**              { "Helv", "bh12" },     // FNT_SYSTEM_FONT
**              { "Helv", "bh10" },
**              { "Helv", "bh10" },
**              { "Helv", "bh10" },
**              { "Helv", "bh10" },
**              { "Helv", "bh10" },
**              { "Helv", "bh10" },
**              { "Helv", "bh10" }
**      };
END BLOCK COMMENT */

UI_WCC_DISPLAY::WCCPATTERN UI_WCC_DISPLAY::patternTable[ZIL_MAXPATTERNS] =
{
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },     // EMPTY_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // SOLID_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // LINE_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // LTSLASH_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // SLASH_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // BKSLASH_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // LTBKSLASH_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // HATCH_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // XHATCH_FILL
        { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },     // INTERLEAVE_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // WIDE_DOT_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },     // CLOSE_DOT_FILL

        { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },     // PTN_BACKGROUND_FILL
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

static int _originalMode = -1;
static int _originalCursor;

// ----- UI_WCC_DISPLAY -----------------------------------------------------

static ZIL_ICHAR fontName[] = { 'H', 'E', 'L', 'V', 'B', '.', 'F', 'O', 'N', 0 };
static ZIL_ICHAR _operatingSystem[] = { 'D','O','S',0 };
static ZIL_ICHAR _windowingSystem[] = { 'W','C','C',0 };
static ZIL_ICHAR _Mstr[] = { 'M', 0 };
static ZIL_ICHAR _codeSet[] = { 'I', 'B', 'M', '_', '8', '5', '0', 0 };

#pragma check_stack (off)
UI_WCC_DISPLAY::UI_WCC_DISPLAY(int mode):
        UI_DISPLAY(FALSE, _operatingSystem, _windowingSystem)
{
        strcpy(codeSet, _codeSet);

        // Initialize the basic member variables.
        _virtualCount = 0;
        _stopDevice = FALSE;
        _fillPattern = -1;
        _backgroundColor = -1;
        _foregroundColor = -1;

        // Use temporary path if not installed in main().
        int pathInstalled = searchPath ? TRUE : FALSE;
        if (!pathInstalled)
                searchPath = new UI_PATH;
        const ZIL_ICHAR *pathName = searchPath->FirstPathName();
        int registered = 0;
        ZIL_ICHAR path[128];
        while (pathName && registered <= 0)
        {
                strcpy(path, pathName);
                if (path[strlen(path) - 1] != defaultCharMap->dirSepStr[0])
                        strcat(path, defaultCharMap->dirSepStr);
                strcat(path, fontName);
                char *TEXT_ = MapText(path, ZIL_NULLP(char), FALSE);
                registered = _registerfonts(TEXT_);
                if (registered > 0)
                        break;
                pathName = searchPath->NextPathName();
        }

        //cout<<pathName<<" "<<registered<<" ";
        if (registered <= 0)
                return;

        // Delete path if it was installed temporarily.
        if (!pathInstalled)
        {
                delete searchPath;
                searchPath = ZIL_NULLP(UI_PATH);
        }

        // Set mode to maximum resolution if mode is not specified
        struct videoconfig config;
        if (_originalMode == -1)
        {
                _getvideoconfig(&config);
                _originalMode = config.mode;
                _originalCursor = _gettextcursor();
        }

        if (!mode)
                mode = _MAXRESMODE;
        if (!_setvideomode(mode))
                return;

        // Get specifics about the video setup.
        _getvideoconfig(&config);
        lines = config.numypixels;
        columns = config.numxpixels;
        maxColors = config.numcolors;
        isMono = (maxColors == 2);

        // Reset some palette colors if in >= VGA mode.
        if (maxColors >= 15 && columns >= 639 && lines >= 479)
        {
                _remappalette(7, RGB(50, 50, 50));              // A lighter lightgray.
                _remappalette(8, RGB(35, 35, 35));              // A lighter darkgray.
                _remappalette(14, RGB(63, 63, 50));             // A lighter yellow.
        }

        // Reset the font information.
        SetFont(FNT_NONE);

        // Fill the screen according to the specified palette.
        cellWidth = TextWidth(_Mstr, ID_SCREEN, FNT_DIALOG_FONT) - 2;
        cellHeight = TextHeight(ZIL_NULLP(ZIL_ICHAR), ID_SCREEN, FNT_DIALOG_FONT) +
                preSpace + postSpace + 2 + 3;  // 2 above the text, 2 below the text.

        // Define the screen display region.
        Add(0, new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
        Rectangle(ID_DIRECT, 0, 0, columns - 1, lines - 1, backgroundPalette, 0, TRUE);
        installed = TRUE;
}

UI_WCC_DISPLAY::~UI_WCC_DISPLAY(void)
{
        // Restore the display.
        if (installed)
        {
                _settextcursor(_originalCursor);
                _setvideomode(_originalMode);
                _clearscreen(_GCLEARSCREEN);
        }
}

void UI_WCC_DISPLAY::Bitmap(ZIL_SCREENID screenID, int column, int line,
        int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
        const UI_PALETTE *palette, const UI_REGION *clipRegion, ZIL_BITMAP_HANDLE *,
        ZIL_BITMAP_HANDLE *)
{
        // Assign the bitmap to the region structure.
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, column, line,
                column + bitmapWidth - 1, line + bitmapHeight - 1))
                return;
        if (!palette)
                palette = colorMap;

        // Draw the rectangle on the display.
        int changedScreen = FALSE;
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                        _setviewport(0, 0, columns - 1, lines - 1);
                        for (int y = tRegion.top; y <= tRegion.bottom; y++)
                        {
                                const ZIL_UINT8 *pixel = bitmapArray + (y - line) * bitmapWidth + tRegion.left - column;
                                for (int x = tRegion.left; x <= tRegion.right; x++, pixel++)
                                        if (*pixel != BACKGROUND)
                                        {
                                                _setcolor(MapColor(palette+*pixel, TRUE));
                                                _setpixel(x, y);
                                        }
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }

        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

void UI_WCC_DISPLAY::Ellipse(ZIL_SCREENID screenID, int x, int y,
        int startAngle, int endAngle, int xRadius, int yRadius,
        const UI_PALETTE *palette, int fill, int xorInt, const UI_REGION *clipRegion)
{
        const int pSin[] = { 0, 871, 1736, 2588, 3420, 4226, 5000, 5736, 6428,
                7071, 7660, 8191, 8660, 9063, 9397, 9659, 9848, 9962, 10000 };

        // Assign the ellipse to the region structure.
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, x - xRadius, y - yRadius,
                x + xRadius, y + yRadius))
                return;
        while (startAngle < 0)
                startAngle += 360, endAngle += 360;
        if (startAngle > 360)
                startAngle %= 360;
        while (endAngle < 0)
                endAngle += 360;
        if (endAngle > 360)
                endAngle %= 360;

        // Draw the ellipse on the display.
        int startX = 0;
        int startY = 0;
        int endX = 0;
        int endY = 0;
        int changedScreen = FALSE;
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                        {
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                                SetPattern(palette, xorInt);

                                // Compute points to start and stop.
                                if (startAngle <= 90)
                                {
                                        startX = x + (int)(1L * pSin[(90 - startAngle) / 5] * xRadius / 10000L);
                                        startY = y - (int)(1L * pSin[startAngle / 5] * yRadius / 10000L);
                                }
                                else if (startAngle <= 180)
                                {
                                        startX = x - (int)(1L * pSin[(startAngle - 90) / 5] * xRadius / 10000L);
                                        startY = y - (int)(1L * pSin[(180 - startAngle) / 5] * yRadius / 10000L);
                                }
                                else if (startAngle <= 270)
                                {
                                        startX = x - (int)(1L * pSin[(270 - startAngle) / 5] * xRadius / 10000L);
                                        startY = y + (int)(1L * pSin[(startAngle - 180) / 5] * yRadius / 10000L);
                                }
                                else if (startAngle <= 360)
                                {
                                        startX = x + (int)(1L * pSin[(startAngle - 270) / 5] * xRadius / 10000L);
                                        startY = y + (int)(1L * pSin[(360 - startAngle) / 5] * yRadius / 10000L);
                                }

                                if (endAngle <= 90)
                                {
                                        endX = x + (int)(1L * pSin[(90 - endAngle) / 5] * xRadius / 10000L);
                                        endY = y - (int)(1L * pSin[endAngle / 5] * yRadius / 10000L);
                                }
                                else if (endAngle <= 180)
                                {
                                        endX = x - (int)(1L * pSin[(endAngle - 90) / 5] * xRadius / 10000L);
                                        endY = y - (int)(1L * pSin[(180 - endAngle) / 5] * yRadius / 10000L);
                                }
                                else if (endAngle <= 270)
                                {
                                        endX = x - (int)(1L * pSin[(270 - endAngle) / 5] * xRadius / 10000L);
                                        endY = y + (int)(1L * pSin[(endAngle - 180) / 5] * yRadius / 10000L);
                                }
                                else if (endAngle <= 360)
                                {
                                        endX = x + (int)(1L * pSin[(endAngle - 270) / 5] * xRadius / 10000L);
                                        endY = y + (int)(1L * pSin[(360 - endAngle) / 5] * yRadius / 10000L);
                                }
                        }
                        _setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);

                        if (fill && _fillPattern != PTN_SOLID_FILL)
                        {
                                _setcolor(_foregroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                if (startAngle == 0 && endAngle == 360)
                                        _ellipse(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
                                else
                                        _pie(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);

                                _setcolor(_backgroundColor);
                                _setfillmask(patternTable[_fillPattern]);
                                if (startAngle == 0 && endAngle == 360)
                                        _ellipse(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
                                else
                                        _pie(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);
                        }
                        else if (fill)
                        {
                                _setcolor(_backgroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                if (startAngle == 0 && endAngle == 360)
                                        _ellipse(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
                                else
                                        _pie(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);
                        }
                        if (!fill || _backgroundColor != _foregroundColor)
                        {
                                _setcolor(_foregroundColor);
                                if (startAngle == 0 && endAngle == 360)
                                        _ellipse(_GBORDER, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
                                else
                                        _pie(_GBORDER, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }

        // Update the screen.
        if (changedScreen)
        {
                SetPattern(palette, FALSE);
                VirtualPut(screenID);
        }

}

int UI_WCC_DISPLAY::DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine)
{
        static int i, x, y, oldX, newX, oldY, newY;
        static int width, height, overlap, deltaX, deltaY;
        static UI_DISPLAY_IMAGE *view;
        static UI_REGION newRegion, virtualRegion;
        static ZIL_UINT8 *image, *screen, *backup, *tScreen;

        // Make sure the image type is valid.
        view = &displayImage[imageType];
        if (!view->image || _stopDevice || _virtualCount != 0 ||
                (view->region.left == newColumn && view->region.top == newLine))
                return (FALSE);
        _stopDevice = TRUE;

        width = view->region.right - view->region.left + 1;
        height = view->region.bottom - view->region.top + 1;

        overlap = FALSE;
        if (newColumn < 0 || newLine < 0)
                newColumn = newLine = width = height = 0;
        else
        {
                deltaX = newColumn - view->region.left;
                deltaY = newLine - view->region.top;

                newRegion.left = view->region.left + deltaX;
                newRegion.right = view->region.right + deltaX;
                newRegion.top = view->region.top + deltaY;
                newRegion.bottom = view->region.bottom + deltaY;

                for (i = 0; i < ZIL_MAXIMAGES; i++)
                        if (i != imageType && (view->region.Overlap(displayImage[i].region) ||
                                               newRegion.Overlap(displayImage[i].region)))
                        {
                                overlap = TRUE;
                                break;
                        }

                if (overlap)
                {
                        virtualRegion.left = MinValue(view->region.left, newRegion.left);
                        virtualRegion.top = MinValue(view->region.top, newRegion.top);
                        virtualRegion.right = MaxValue(view->region.right, newRegion.right);
                        virtualRegion.bottom = MaxValue(view->region.bottom, newRegion.bottom);

                        VirtualGet(0, virtualRegion.left, virtualRegion.top,
                                   virtualRegion.right, virtualRegion.bottom);

                        view->region.left += deltaX;
                        view->region.right += deltaX;
                        view->region.top += deltaY;
                        view->region.bottom += deltaY;

                        VirtualPut(0);
                }
                else
                {
                        // Compute the update coordinates.
                        _setviewport(0, 0, columns - 1, lines - 1);
                        image = view->image;
                        screen = view->screen;
                        backup = view->backup;

                        oldY = view->region.top;
                        newY = newLine;
                        for (y = 0; y < height; y++, oldY++, newY++)
                        {
                                tScreen = &screen[deltaY * width + deltaX];
                                oldX = view->region.left;
                                newX = newColumn;
                                for (x = 0; x < width; x++, oldX++, newX++, image++, screen++, backup++, tScreen++)
                                {
                                        if (*image == BACKGROUND)
                                                *backup = BACKGROUND;
                                        else if (*image != BACKGROUND &&
                                                 x + deltaX >= 0 && x + deltaX < width &&
                                                 y + deltaY >= 0 && y + deltaY < height &&
                                                 *tScreen != BACKGROUND)
                                        {
                                                *backup = *tScreen;
                                                *tScreen = BACKGROUND;
                                        }
                                        else
                                                *backup = _getpixel(newX, newY);
                                        if (*image != BACKGROUND)
                                        {
                                                _setcolor(*image);
                                                _setpixel(newX, newY);
                                        }
                                }
                        }
                }
        }
        if (!overlap)
        {
                screen = view->screen;
                for (y = view->region.top; y <= view->region.bottom; y++)
                        for (x = view->region.left; x <= view->region.right; x++, screen++)
                                if (*screen != BACKGROUND)
                                {
                                        _setcolor(*screen);
                                        _setpixel(x, y);
                                }
                memcpy(view->screen, view->backup, width * height);
                view->region.left = newColumn;
                view->region.top = newLine;
                view->region.right = newColumn + width - 1;
                view->region.bottom = newLine + height - 1;
        }

        _stopDevice = FALSE;
        return (TRUE);
}

int UI_WCC_DISPLAY::DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line,
        int width, int height, ZIL_UINT8 *image)
{
        if (_stopDevice)
                return (FALSE);
        _stopDevice = TRUE;

        UI_DISPLAY_IMAGE *view = &displayImage[imageType];
        UI_REGION virtualRegion;
        if (view->image)
        {
                virtualRegion.left = MinValue(view->region.left, column);
                virtualRegion.top = MinValue(view->region.top, line);
                virtualRegion.right = MaxValue(view->region.right, column + width - 1);
                virtualRegion.bottom = MaxValue(view->region.bottom, line + height - 1);
        }
        else
        {
                virtualRegion.left = column;
                virtualRegion.top = line;
                virtualRegion.right = column + width - 1;
                virtualRegion.bottom = line + height - 1;
        }

        VirtualGet(0, virtualRegion.left, virtualRegion.top, virtualRegion.right,
                virtualRegion.bottom);

        view->image = image;
        view->region.left = column;
        view->region.top = line;
        view->region.right = column + width - 1;
        view->region.bottom = line + height - 1;
        if (view->image)
        {
                if (view->screen)
                {
                        delete view->screen;
                        delete view->backup;
                }
                view->screen = new ZIL_UINT8[width * height];
                view->backup = new ZIL_UINT8[width * height];
                memset(view->screen, BACKGROUND, width * height);
        }

        VirtualPut(0);
        _stopDevice = FALSE;
        return (TRUE);
}

void UI_WCC_DISPLAY::Line(ZIL_SCREENID screenID, int x1, int y1,
        int x2, int y2, const UI_PALETTE *palette, int width, int xorInt,
        const UI_REGION *clipRegion)
{
        // Assign the line to a region structure.
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, MinValue(x1, x2), MinValue(y1, y2),
                MaxValue(x1, x2) + width, MaxValue(y1, y2) + width))
                return;

        // Draw the line on the display.
        int changedScreen = FALSE;
        int slope;
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                        {
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                                SetPattern(palette, xorInt);
                                _setcolor(_foregroundColor);
                                slope = x1 == x2 ? 1 : (y1 - y2) / (x1 - x2);
                        }
                        _setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
                        for (int i = 0; i < width; i++)
                                if (slope)
                                {
                                        _moveto(x1 - tRegion.left + i, y1 - tRegion.top);
                                        _lineto(x2 - tRegion.left + i, y2 - tRegion.top);
                                }
                                else
                                {
                                        _moveto(x1 - tRegion.left, y1 - tRegion.top + i);
                                        _lineto(x2 - tRegion.left, y2 - tRegion.top + i);
                                }
                        if (screenID == ID_DIRECT)
                                break;
                }

        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

ZIL_COLOR UI_WCC_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
        // Match the color request based on the type of display.
        if (maxColors == 2)
                return (foreground ? palette->bwForeground : palette->bwBackground);
        else if (maxColors < 16)
                return (foreground ? palette->grayScaleForeground : palette->grayScaleBackground);
        return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_WCC_DISPLAY::Polygon(ZIL_SCREENID screenID, int numPoints,
        const int *polygonPoints, const UI_PALETTE *palette, int fill, int xorInt,
        const UI_REGION *clipRegion)
{
        // Assign the polygon to the region structure.
        int left = 0x0FFF, top = 0x0FFF, right = 0, bottom = 0;
        for (int i = 0; i < numPoints * 2; i += 2)
        {
                left = MinValue(left, polygonPoints[i]);
                top = MinValue(top, polygonPoints[i+1]);
                right = MaxValue(right, polygonPoints[i]);
                bottom = MaxValue(bottom, polygonPoints[i+1]);
        }
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
                return;

        // Draw the polygon on the display.
        int changedScreen = FALSE;
        short *tPolygon = new short[numPoints * 2];
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                        {
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                                SetPattern(palette, xorInt);
                        }
                        _setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
                        for (int i = 0; i < numPoints * 2; i += 2)
                        {
                                tPolygon[i] = polygonPoints[i] - tRegion.left;
                                tPolygon[i+1] = polygonPoints[i+1] - tRegion.top;
                        }
                        if (fill && _fillPattern != PTN_SOLID_FILL)
                        {
                                _setcolor(_foregroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                _polygon(_GFILLINTERIOR, numPoints, (struct xycoord *)tPolygon);

                                _setcolor(_backgroundColor);
                                _setfillmask(patternTable[_fillPattern]);
                                _polygon(_GFILLINTERIOR, numPoints, (struct xycoord *)tPolygon);
                        }
                        else if (fill)
                        {
                                _setcolor(_backgroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                _polygon(_GFILLINTERIOR, numPoints, (struct xycoord *)tPolygon);
                        }
                        if (!fill || _backgroundColor != _foregroundColor)
                        {
                                _setcolor(_foregroundColor);
                                for (int i = 2; i < numPoints * 2; i += 2)
                                {
                                        _moveto(tPolygon[i-2], tPolygon[i-1]);
                                        _lineto(tPolygon[i], tPolygon[i+1]);
                                }
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }
        delete tPolygon;

        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

void UI_WCC_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top,
        int right, int bottom, const UI_PALETTE *palette, int width, int fill,
        int xorInt, const UI_REGION *clipRegion)
{
        // Assign the rectangle to the region structure.
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
                return;

        // Draw the rectangle on the display.
        int changedScreen = FALSE;
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                        {
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                                SetPattern(palette, xorInt);
                        }
                        _setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
                        if (fill && _fillPattern != PTN_SOLID_FILL)
                        {
                                _setcolor(_foregroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                _rectangle(_GFILLINTERIOR, left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);

                                _setcolor(_backgroundColor);
                                _setfillmask(patternTable[_fillPattern]);
                                _rectangle(_GFILLINTERIOR, left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);
                        }
                        else if (fill)
                        {
                                _setcolor(_backgroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                _rectangle(_GFILLINTERIOR, left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);
                        }
                        else if (width < 0)
                        {
                                // Draws a dashed rounded rectangle to indicate focus
                                _setcolor(palette->colorForeground);
                                _setlinestyle(0x5555);
                                _setviewport(0, 0, columns - 1, lines - 1);
                                _moveto(left + 1, top);
                                _lineto(right - 1, top);
                                _moveto(right, top + 1);
                                _lineto(right, bottom - 1);
                                _moveto(left + 1, bottom);
                                _lineto(right - 1, bottom);
                                _moveto(left, top + 1);
                                _lineto(left, bottom - 1);
                                _setlinestyle(0xFFFF);
                        }
                        if (!fill || _backgroundColor != _foregroundColor)
                        {
                                _setcolor(_foregroundColor);
                                for (int i = 0; i < width; i++)
                                {
                                        _rectangle(_GBORDER, left - (tRegion.left - i), top - (tRegion.top - i),
                                                right - (tRegion.left + i), bottom - (tRegion.top + i));
                                }
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }

        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

void UI_WCC_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
        const UI_REGION &newRegion, ZIL_SCREENID, const UI_REGION *clipRegion)
{
        // See if the regions match.
        if (oldRegion.left == newRegion.left && oldRegion.top == newRegion.top &&
                oldRegion.right == newRegion.right && oldRegion.bottom == newRegion.bottom)
                return;

        // Hide the screen devices.
        UI_REGION region;
        if (!RegionInitialize(region, ZIL_NULLP(UI_REGION),
                MinValue(oldRegion.left, newRegion.left), MinValue(oldRegion.top, newRegion.top),
                MaxValue(oldRegion.right, newRegion.right), MaxValue(oldRegion.bottom, newRegion.bottom)))
                return;
        VirtualGet(ID_DIRECT, region.left, region.top, region.right, region.bottom);

        // Draw the XOR rectangles on the display.
        SetPattern(xorPalette, TRUE);
        _setcolor(_foregroundColor);
        if (clipRegion)
        {
                _setviewport(clipRegion->left, clipRegion->top, clipRegion->right, clipRegion->bottom);
                _rectangle(_GBORDER, oldRegion.left - clipRegion->left, oldRegion.top - clipRegion->top,
                        oldRegion.right - clipRegion->left, oldRegion.bottom - clipRegion->top);
                _rectangle(_GBORDER, newRegion.left - clipRegion->left, newRegion.top - clipRegion->top,
                        newRegion.right - clipRegion->left, newRegion.bottom - clipRegion->top);
        }
        else
        {
                _setviewport(0, 0, columns - 1, lines - 1);
                _rectangle(_GBORDER, oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
                _rectangle(_GBORDER, newRegion.left, newRegion.top, newRegion.right, newRegion.bottom);
        }
        SetPattern(xorPalette, FALSE);

        // Update the screen.
        VirtualPut(ID_DIRECT);
}

void UI_WCC_DISPLAY::RegionDefine(ZIL_SCREENID screenID, int left, int top,
        int right, int bottom)
{
        // See if it is a full screen definition.
        UI_REGION region = { left, top, right, bottom };
        if (region.left <= 0 && region.top <= 0 && region.right >= columns - 1 && region.bottom >= lines - 1)
        {
                UI_REGION_LIST::Destroy();
                Add(0, new UI_REGION_ELEMENT(screenID, 0, 0, columns - 1, lines - 1));
                return;
        }

        // Throw away regions with a negative width or height.
        if (region.right < region.left || region.bottom < region.top)
                return;
        
        // Throw away regions that are completely off the screen.
        if (region.left >= columns || region.right < 0 ||
                region.top >= lines || region.bottom < 0)
                return;

        // Clip regions partially off the screen to fit on the screen.
        if (region.left < 0)
                region.left = 0;
        if (region.right >= columns)
                region.right = columns - 1;
        if (region.top < 0)
                region.top = 0;
        if (region.bottom >= lines)
                region.bottom = lines - 1;

        // Split any overlapping regions.
        Split(screenID, region, FALSE);

        // Define the new display region.
        UI_REGION_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region));
}

void UI_WCC_DISPLAY::RegionMove(const UI_REGION &oldRegion,
        int newColumn, int newLine, ZIL_SCREENID oldScreenID, ZIL_SCREENID)
{
        VirtualGet(oldScreenID, MinValue(oldRegion.left, newColumn), MinValue(oldRegion.top, newLine),
                MaxValue(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
                MaxValue(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top)));

        char *buffer = ZIL_NULLP(char);
        _setviewport(0, 0, columns - 1, lines - 1);
        long size = _imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
        buffer = (char *)malloc(size);
        if (buffer)
        {
                _getimage(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom, buffer);
                _putimage(newColumn, newLine, buffer, _GPSET);
        }
        else if (oldRegion.top < newLine)
        {
                size = _imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
                buffer = (char *)malloc(size);
                if (!buffer)
                        return;
                int height = oldRegion.bottom - oldRegion.top;
                for (int i = height; i >= 0; i--)
                        if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
                        {
                                _getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
                                _putimage(newColumn, newLine + i, buffer, _GPSET);
                        }
        }
        else
        {
                size = _imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
                buffer = (char *)malloc(size);
                if (!buffer)
                        return;
                int height = oldRegion.bottom - oldRegion.top;
                for (int i = 0; i <= height; i++)
                        if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
                        {
                                _getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
                                _putimage(newColumn, newLine + i, buffer, _GPSET);
                        }
        }
        free(buffer);

        VirtualPut(oldScreenID);
}

void UI_WCC_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
        const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
        int xorInt, const UI_REGION *clipRegion, ZIL_LOGICAL_FONT logicalFont)
{
        // Make sure we have a valid string.
        if (!text || !text[0] || !palette)
                return;

        // Set up the fill line.
        ZIL_ICHAR fillLine[256];
        if (length < 0)
                length = strlen(text);
        else if (length > 255)
                length = 255;
        strncpy(fillLine, text, length);
        fillLine[length] = 0;

        // Determine the hot-key values and shrink the string to remove '&'.
        ZIL_ICHAR *hotKey = ZIL_NULLP(ZIL_ICHAR);
        int hotKeyLeft, hotKeyRight;
        if (!FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE))
        {
                ZIL_ICHAR hotKeyBuf[2];
                ZIL_ICHAR *hotChar = fillLine;
                for (int i=0; i < length; i++, hotChar++)
                {
                        if ((*hotChar == ZIL_HOTMARK && hotChar[1] == ZIL_HOTMARK) ||
                                (*hotChar == '~' && hotChar[1] == '~'))
                                memmove(hotChar, hotChar + 1, strlen(hotChar) * sizeof(ZIL_ICHAR));
                        else if (*hotChar == ZIL_HOTMARK || *hotChar == '~')
                        {
                                hotKeyBuf[0] = hotChar[1];
                                hotKeyBuf[1] =  *hotChar = '\0';
                                hotKeyLeft = left + TextWidth(fillLine, screenID, logicalFont | FNT_IGNORE_UNDERSCORE);
                                memmove(hotChar, hotChar + 1, (strlen(hotChar + 1) + 1) * sizeof(ZIL_ICHAR));
                                hotKey = hotChar;
                                int markLen = TextWidth(hotKeyBuf, screenID, logicalFont | FNT_IGNORE_UNDERSCORE);
                                hotKeyRight = hotKeyLeft + markLen - 1;
                        }
                }
        }

        // Check for special characters.
        strrepc(fillLine, '\t', ' ');
        strrepc(fillLine, '\r', ' ');
        strrepc(fillLine, '\n', ' ');

        // Assign the rectangle to the region structure.
        SetFont(logicalFont);
        UI_REGION region, tRegion;
        int width = TextWidth(fillLine, screenID, logicalFont | FNT_IGNORE_UNDERSCORE);
        int height = TextHeight(fillLine, screenID, logicalFont);
        int right = left + width;
        int bottom = top + height;
        if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
                return;

        // Draw the rectangle on the display.
        int changedScreen = FALSE;
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                        {
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                                SetPattern(palette, xorInt);
                        }
                        _setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
                        if (fill && _fillPattern != PTN_SOLID_FILL)
                        {
                                _setcolor(_foregroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                _rectangle(_GFILLINTERIOR, region.left - tRegion.left, region.top - tRegion.top,
                                        region.right - tRegion.left, region.bottom - tRegion.top);

                                _setcolor(_backgroundColor);
                                _setfillmask(patternTable[_fillPattern]);
                                _rectangle(_GFILLINTERIOR, region.left - tRegion.left, region.top - tRegion.top,
                                        region.right - tRegion.left, region.bottom - tRegion.top);
                        }
                        else if (fill)
                        {
                                _setcolor(_backgroundColor);
                                _setfillmask(ZIL_NULLP(unsigned char));
                                _rectangle(_GFILLINTERIOR, region.left - tRegion.left, region.top - tRegion.top,
                                        region.right - tRegion.left, region.bottom - tRegion.top);
                        }
                        if (!fill || _backgroundColor != _foregroundColor)
                        {
                                _setcolor(_foregroundColor);
                                _moveto(left - tRegion.left, top - tRegion.top);
                                char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
                                _outgtext(TEXT_);
                        }
                        if (hotKey)
                        {
                                _moveto(hotKeyLeft - tRegion.left, bottom - tRegion.top);
                                _lineto(hotKeyRight - tRegion.left, bottom - tRegion.top);
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }

        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

void UI_WCC_DISPLAY::SetFont(ZIL_LOGICAL_FONT logicalFont)
{
        static int _logicalFont = FNT_NONE;
        static WCCFONT _fontInfo;
        WCCFONT fontInfo;
        char fontString[32];

        // Check for invalid or new font set.
        if (_logicalFont == FNT_NONE || logicalFont == FNT_NONE)
        {
                logicalFont = FNT_DIALOG_FONT;
                fontInfo = fontTable[FNT_DIALOG_FONT];
                ::sprintf(fontString, "t'%s'%s", fontInfo.typeFace, fontInfo.options);
                _setfont(fontString);
                _logicalFont = logicalFont;
                _fontInfo = fontInfo;
        }
        else
        {
                logicalFont &= 0x0FFF;
                if (logicalFont == _logicalFont)
                        return;
        }

        // Update the font information.
        fontInfo = fontTable[logicalFont];
        if (fontInfo.typeFace != _fontInfo.typeFace ||
                fontInfo.options != _fontInfo.options)
        {
                ::sprintf(fontString, "t'%s'%s", fontInfo.typeFace, fontInfo.options);
                _setfont(fontString);
        }
        _fontInfo = fontInfo;
        _logicalFont = logicalFont;
}

void UI_WCC_DISPLAY::SetPattern(const UI_PALETTE *palette, int xorInt)
{
        static int _xorInt = -1;

        if (palette)
        {
                _fillPattern = palette->fillPattern;
                _backgroundColor = MapColor(palette, FALSE);
                _foregroundColor = MapColor(palette, TRUE);
        }
        if (xorInt != _xorInt)
        {
                _setwritemode(xorInt ? _GXOR : _GPSET);
                _xorInt = xorInt;
        }
}

int UI_WCC_DISPLAY::TextHeight(const ZIL_ICHAR *, ZIL_SCREENID,
        ZIL_LOGICAL_FONT logicalFont)
{
        logicalFont &= 0x0FFF;
        SetFont(logicalFont);
        struct _fontinfo fontInfo;
        _getfontinfo(&fontInfo);
        return (fontInfo.pixheight);
}

int UI_WCC_DISPLAY::TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID,
        ZIL_LOGICAL_FONT logicalFont)
{
        if (!string || !(*string))
                return (0);
        SetFont(logicalFont & 0x0FFF);
        char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
        int length = _getgtextextent(TEXT_);

        int lineLen = strlen(string);
        for (int j=0; j < lineLen; j++)
                if (!FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE) && (string[j] == ZIL_HOTMARK || string[j] == '~'))
                {
                        char hotKeyBuf[2];
                        hotKeyBuf[0] = string[j];
                        hotKeyBuf[1] = '\0';
                        length -= _getgtextextent(hotKeyBuf);
                        if ((string[j] == ZIL_HOTMARK && string[j+1] == ZIL_HOTMARK) ||
                                (string[j] == '~' && string[j+1] == '~'))
                                j++;
                }
        return (length);
}

int UI_WCC_DISPLAY::VirtualGet(ZIL_SCREENID, int left, int top,
        int right, int bottom)
{
        static int i, x, y;
        static UI_DISPLAY_IMAGE *view;
        static ZIL_UINT8 *screen;

        if (--_virtualCount == -1)
        {
                _stopDevice = TRUE;
                _virtualRegion.left = MaxValue(0, left);
                _virtualRegion.top = MaxValue(0, top);
                _virtualRegion.right = MinValue(columns - 1, right);
                _virtualRegion.bottom = MinValue(lines - 1, bottom);
                SetPattern(ZIL_NULLP(UI_PALETTE), FALSE);
                for (i = ZIL_MAXIMAGES - 1; i >= 0; i--)
                        if (_virtualRegion.Overlap(displayImage[i].region) &&
                                displayImage[i].image && displayImage[i].screen)
                        {
                                view = &displayImage[i];
                                screen = view->screen;
                                _setviewport(0, 0, columns - 1, lines - 1);
                                for (y = view->region.top; y <= view->region.bottom; y++)
                                        for (x = view->region.left; x <= view->region.right; x++, screen++)
                                                if (*screen != BACKGROUND)
                                                {
                                                        _setcolor(*screen);
                                                        _setpixel(x, y);
                                                }
                        }
        }
        return (TRUE);
}

int UI_WCC_DISPLAY::VirtualPut(ZIL_SCREENID)
{
        static int i, x, y;
        static UI_DISPLAY_IMAGE *view;
        static ZIL_UINT8 *screen, *image;

        if (++_virtualCount == 0)
        {
                SetPattern(ZIL_NULLP(UI_PALETTE), FALSE);
                for (i = 0; i < ZIL_MAXIMAGES; i++)
                        if (_virtualRegion.Overlap(displayImage[i].region) &&
                                displayImage[i].image && displayImage[i].screen)
                        {
                                view = &displayImage[i];
                                screen = view->screen;
                                image = view->image;
                                _setviewport(0, 0, columns - 1, lines - 1);
                                for (y = view->region.top; y <= view->region.bottom; y++)
                                        for (x = view->region.left; x <= view->region.right; x++, screen++, image++)
                                                if (*image != BACKGROUND)
                                                {
                                                        *screen = _getpixel(x, y);
                                                        _setcolor(*image);
                                                        _setpixel(x, y);
                                                }
                        }
                _stopDevice = FALSE;
        }
        else if (_virtualCount > 0)
                _virtualCount = 0;
        return (TRUE);
}

void UI_WCC_DISPLAY::BitmapArrayToHandle(ZIL_SCREENID, int,
        int, const ZIL_UINT8 *, const UI_PALETTE *,
        ZIL_BITMAP_HANDLE *, ZIL_BITMAP_HANDLE *)
{
}

void UI_WCC_DISPLAY::BitmapHandleToArray(ZIL_SCREENID, ZIL_BITMAP_HANDLE,
                ZIL_BITMAP_HANDLE, int *, int *,
                ZIL_UINT8 **)
{
}

void UI_WCC_DISPLAY::DestroyBitmapHandle(ZIL_SCREENID, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
        if (colorBitmap && *colorBitmap)
        {
                delete *colorBitmap;
                *colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
        }
        if (monoBitmap && *monoBitmap)
        {
                delete *monoBitmap;
                *monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
        }
}

void UI_WCC_DISPLAY::IconArrayToHandle(ZIL_SCREENID , int ,
        int , const ZIL_UINT8 *, const UI_PALETTE *,
        ZIL_ICON_HANDLE *)
{
}

void UI_WCC_DISPLAY::IconHandleToArray(ZIL_SCREENID , ZIL_ICON_HANDLE ,
        int *, int *, ZIL_UINT8 **)
{
}

void UI_WCC_DISPLAY::DestroyIconHandle(ZIL_SCREENID, ZIL_ICON_HANDLE *icon)
{
        if (icon && icon->colorBitmap)
        {
                delete icon->colorBitmap;
                icon->colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);

        }
        if (icon && icon->monoBitmap)
        {
                delete icon->monoBitmap;
                icon->monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);

        }
}

