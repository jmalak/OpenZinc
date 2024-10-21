//      Zinc Interface Library - O_DSP.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// Work around for IBM C Set++ compiler bug.
#if defined(__IBMCPP__)
#       if __IBMCPP__ <= 210
#               define GpiQueryColorData        GpiQueryColordata
#       endif
#endif

#define INCL_DOSMISC
#define INCL_GPI
#define INCL_WINSYS
#define INCL_WINERRORS
#define INCL_WINPOINTERS
#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#include <malloc.h>
#include <stdio.h>
#include "ui_dsp.hpp"

static int _virtualCount = 0;
static UI_REGION _virtualRegion;

HPS UI_OS2_DISPLAY::hps = 0;
FONTMETRICS UI_OS2_DISPLAY::fontTable[ZIL_MAXFONTS];

static ZIL_ICHAR _operatingSystem[] = { 'O','S','/','2',0 };
static ZIL_ICHAR _windowingSystem[] = { 'P','M',0 };
static ZIL_ICHAR _codeSet[] = { 'I','B','M','_',0 };

// Initialize the bitmap information structures.
static BITMAPINFO2 *_bitmapInfo = ZIL_NULLP(BITMAPINFO2);
static BITMAPINFOHEADER2 *_bitmapHeaderInfo = ZIL_NULLP(BITMAPINFOHEADER2);
static BITMAPINFO2 *_monoBitmapInfo = ZIL_NULLP(BITMAPINFO2);
static BITMAPINFOHEADER2 *_monoBitmapHeaderInfo = ZIL_NULLP(BITMAPINFOHEADER2);

// ----- UI_OS2_DISPLAY -----------------------------------------------------

UI_OS2_DISPLAY::UI_OS2_DISPLAY(void) :
        UI_DISPLAY(FALSE, _operatingSystem, _windowingSystem), maxColors(16)
{
        hab = WinInitialize(0);
        columns = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
        lines = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
        preSpace = 2, postSpace = 0;

        HPS desktopPS = WinGetPS(HWND_DESKTOP);

        // Work around for OS/2 2.0 bug.
        LONG OS2VersionMajor;
        LONG OS2VersionMinor;
        DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MAJOR, &OS2VersionMajor, sizeof(LONG));
        DosQuerySysInfo(QSV_VERSION_MINOR, QSV_VERSION_MINOR, &OS2VersionMinor, sizeof(LONG));
        int codePage = 850;
        if (OS2VersionMajor == 20 && OS2VersionMinor == 00)
        {
                cellWidth = 7;
                cellHeight = 24;
        }
        else
        {
                // Determine the cell height and width.
                FONTMETRICS defaultFont;
                GpiQueryFontMetrics(desktopPS, sizeof(FONTMETRICS), &defaultFont);

                cellWidth = defaultFont.lAveCharWidth + 1;
                cellHeight = defaultFont.lMaxBaselineExt
                        + defaultFont.lExternalLeading                  // Space between text rows
                        + preSpace + postSpace                                          // Above / below border
                        + 6;                                                                                            // Border Space
                codePage = defaultFont.usCodePage;
        }

        strcpy(codeSet, _codeSet);
        itoa(codePage, &codeSet[strlen(codeSet)], 10, 0);

        // Initialize the font table.
        for (int i = 0; i < ZIL_MAXFONTS; i++)
                memset(&fontTable[i], 0, sizeof(FONTMETRICS));

        // Set the small font (others use OS/2 system settings).
#if !defined(ZIL_UNICODE)
        LONG maxFonts = 0;
        maxFonts = GpiQueryFonts(desktopPS, QF_PUBLIC, (PSZ)"Helv", &maxFonts, sizeof(FONTMETRICS), ZIL_NULLP(FONTMETRICS));

        FONTMETRICS *font = new FONTMETRICS[maxFonts];
        GpiQueryFonts(desktopPS, QF_PUBLIC, (PSZ)"Helv", &maxFonts, sizeof(FONTMETRICS), font);

        for (int j = 0; j < maxFonts; j++)
        {
                if (font[j].sNominalPointSize == 80 && !fontTable[FNT_SMALL_FONT].lMatch)
                {
                        fontTable[FNT_SMALL_FONT] = font[j];
                        break;
                }
        }

        delete font;
#endif
        WinReleasePS(desktopPS);
        installed = TRUE;
}

UI_OS2_DISPLAY::~UI_OS2_DISPLAY(void)
{
        if (hab)
                WinTerminate(hab);
        if (_bitmapInfo)
        {
                delete _bitmapInfo;
                delete _bitmapHeaderInfo;
        }
        if (_monoBitmapInfo)
        {
                delete _monoBitmapInfo;
                delete _monoBitmapHeaderInfo;
        }
}

void UI_OS2_DISPLAY::Bitmap(ZIL_SCREENID screenID, int left, int top,
        int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
        const UI_PALETTE *palette, const UI_REGION *clip, ZIL_BITMAP_HANDLE *_colorBitmap,
        ZIL_BITMAP_HANDLE *_monoBitmap)
{
        // Make sure there is a valid bitmap.
        ZIL_BITMAP_HANDLE colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE), monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
        if (_colorBitmap)
                colorBitmap = *_colorBitmap;
        if (_monoBitmap)
                monoBitmap = *_monoBitmap;
        if (!bitmapArray && !colorBitmap)
                return;

        // Virtualize the display. (This call set up UI_OS2_DISPLAY::hps.)
        VirtualGet(screenID, left, top, left + bitmapWidth - 1, top + bitmapHeight - 1);
        HRGN hRegion = 0;
        if (clip)
        {
                RECTL os2Region;
                os2Region.xLeft = clip->left - _virtualRegion.left;
                os2Region.yBottom = _virtualRegion.bottom - clip->bottom;
                os2Region.xRight = clip->right - _virtualRegion.left + 1;
                os2Region.yTop = _virtualRegion.bottom - clip->top + 1;
                hRegion = GpiCreateRegion(hps, 1, &os2Region);
                if (hRegion)
                        GpiSetClipRegion(hps, hRegion, ZIL_NULLP(HRGN));
        }

        // Determine the bitmap position.
        POINTL point[4];
        point[0].x = left - _virtualRegion.left;
        point[0].y = _virtualRegion.bottom - top - bitmapHeight + 1;
        point[1].x = point[0].x + bitmapWidth - 1;
        point[1].y = point[0].y + bitmapHeight - 1;
        point[2].x = point[2].y = 0;
        point[3].x = bitmapWidth;
        point[3].y = bitmapHeight;

        // Convert the bitmap array then draw the bitmap.
        if (!colorBitmap)
                BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight, bitmapArray,
                        palette, &colorBitmap, &monoBitmap);
        if (monoBitmap)
        {
                // OS2 3.0(Warp) fix.
                POINTL savePoint[4];
                savePoint[0] = point[0];
                savePoint[1] = point[1];
                savePoint[2] = point[2];
                savePoint[3] = point[3];

                GpiWCBitBlt(hps, monoBitmap, 4, point, ROP_SRCAND, BBO_IGNORE);

                // OS2 3.0(Warp) fix.
                point[0] = savePoint[0];
                point[1] = savePoint[1];
                point[2] = savePoint[2];
                point[3] = savePoint[3];

                GpiWCBitBlt(hps, colorBitmap, 4, point, ROP_SRCINVERT, BBO_IGNORE);
        }
        else if (colorBitmap)
                GpiWCBitBlt(hps, colorBitmap, 4, point, ROP_SRCCOPY, BBO_IGNORE);
        if (monoBitmap)
        {
                if (!_monoBitmap)
                        GpiDeleteBitmap(monoBitmap);
                else
                        *_monoBitmap = monoBitmap;
        }
        if (colorBitmap)
        {
                if (!_colorBitmap)
                        GpiDeleteBitmap(colorBitmap);
                else
                        *_colorBitmap = colorBitmap;
        }

        // Un-virtualize the display.
        if (hRegion)
        {
                GpiSetClipRegion(hps, ZIL_NULLH(HRGN), ZIL_NULLP(HRGN));
                GpiDestroyRegion(hps, hRegion);
        }
        VirtualPut(screenID);
}

void UI_OS2_DISPLAY::BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
        int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *,
        ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
        // Make sure there is a valid bitmap.
        if (!bitmapArray || *colorBitmap)
                return;

        if (!_bitmapInfo)
        {
                _bitmapInfo = (BITMAPINFO2 *)new ZIL_INT8[sizeof(BITMAPINFO2) + 255 * sizeof(RGB2)];
                memset(_bitmapInfo, 0, sizeof(BITMAPINFO2) + 255 * sizeof(RGB2));
                _bitmapInfo->cbFix = sizeof(BITMAPINFO2) - sizeof(RGB2);
                _bitmapInfo->cPlanes = 1;
                _bitmapInfo->cBitCount = 8;
                _bitmapInfo->ulCompression = BCA_UNCOMP;
                _bitmapInfo->usUnits = BRU_METRIC;
                _bitmapInfo->usRecording = BRA_BOTTOMUP;
                _bitmapInfo->usRendering = BRH_NOTHALFTONED;
                _bitmapInfo->ulColorEncoding = BCE_RGB;
                for (int i = 0; i < maxColors; i++)
                {
                        ZIL_COLOR color = MapColor(&colorMap[i], TRUE);
                        _bitmapInfo->argbColor[i].bBlue = (BYTE)color;
                        _bitmapInfo->argbColor[i].bGreen = (BYTE)(color >> 8);
                        _bitmapInfo->argbColor[i].bRed = (BYTE)(color >> 16);
                }

                _bitmapHeaderInfo = new BITMAPINFOHEADER2;
                memset(_bitmapHeaderInfo, 0, sizeof(BITMAPINFOHEADER2));
                _bitmapHeaderInfo->cbFix = sizeof(BITMAPINFOHEADER2);
                _bitmapHeaderInfo->cPlanes = 1;
                _bitmapHeaderInfo->cBitCount = 8;
        }

        // Set the bitmap width and height.
        _bitmapInfo->cx = _bitmapHeaderInfo->cx = bitmapWidth;
        _bitmapInfo->cy = _bitmapHeaderInfo->cy = bitmapHeight;

        // alignWidth pads the bitmap scan-lines for display purposes.
        int alignWidth = ((bitmapWidth + sizeof(ULONG) - 1) / sizeof(ULONG)) * sizeof(ULONG);

        // AND then XOR the bitmap onto the screen.
        int useBackground = FALSE;
        ZIL_UINT8 *bitmap = new ZIL_UINT8[alignWidth * bitmapHeight];
        VirtualGet(screenID, 0, 0, bitmapWidth - 1, bitmapHeight - 1);
        for (int mix = monoBitmap ? 0 : 1; mix <= 1; mix++)
        {
                const ZIL_UINT8 *tBitmapArray = bitmapArray;
                for (int y = 0; y < bitmapHeight; y++)
                {
                        ZIL_UINT8 *color = &bitmap[(bitmapHeight - y - 1) * alignWidth];
                        for (int x = 0; x < bitmapWidth; x++, tBitmapArray++)
                                if (*tBitmapArray == BACKGROUND)
                                {
                                        color[x] = mix ? 0x00 : 0x0F;
                                        useBackground = TRUE;
                                }
                                else if (mix)
                                        color[x] = *tBitmapArray;
                                else
                                        color[x] = 0x00;
                        for (int z = bitmapWidth; z < alignWidth; z++)
                                color[z] = 0x00;
                }
                if (mix && colorBitmap)
                        *colorBitmap = GpiCreateBitmap(hps, _bitmapHeaderInfo, CBM_INIT, (PBYTE)bitmap, _bitmapInfo);
                else if (useBackground && !mix && monoBitmap)
                        *monoBitmap = GpiCreateBitmap(hps, _bitmapHeaderInfo, CBM_INIT, (PBYTE)bitmap, _bitmapInfo);
        }
        VirtualPut(screenID);
        delete bitmap;
}

void UI_OS2_DISPLAY::BitmapHandleToArray(ZIL_SCREENID, ZIL_BITMAP_HANDLE colorBitmap,
        ZIL_BITMAP_HANDLE monoBitmap, int *bitmapWidth, int *bitmapHeight, ZIL_UINT8 **bitmapArray)
{
        SIZEL size;
        BITMAPINFOHEADER bitmapInfoHeader;
        GpiQueryBitmapParameters(colorBitmap, &bitmapInfoHeader);
        size.cx = bitmapInfoHeader.cx;
        size.cy = bitmapInfoHeader.cy;
        DEVOPENSTRUC devOpenData = { 0L, (PSZ)"DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L };
        HDC memoryDC = DevOpenDC(hab, OD_MEMORY, (PSZ)"*", 5L, (PDEVOPENDATA)&devOpenData, ZIL_NULLH(HDC));
        HPS memoryPS = GpiCreatePS(hab, memoryDC, &size, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
        GpiSetBitmap(memoryPS, colorBitmap);

        *bitmapWidth = size.cx;
        *bitmapHeight = size.cy;
        if (!(*bitmapArray))
                *bitmapArray = new ZIL_UINT8[size.cx * size.cy];
        ZIL_UINT8 *pixel = *bitmapArray;
        POINTL coord;
        for (coord.y = size.cy - 1; coord.y >= 0; coord.y--)
                for (coord.x = 0; coord.x < size.cx; coord.x++)
                {
                        ZIL_COLOR color = GpiQueryRGBColor(memoryPS, LCOLOPT_REALIZED,
                                GpiQueryPel(memoryPS, &coord));
                        *pixel = 0;
                        for (int i = 0; i <= MAX_COLORMAP_INDEX; i++)
                                if (color == MapColor(&colorMap[i], TRUE))
                                {
                                        *pixel = i;
                                        break;
                                }
                        pixel++;
                }

        if (monoBitmap)
        {
                GpiSetBitmap(memoryPS, monoBitmap);
                pixel = *bitmapArray;
                for (coord.y = size.cy - 1; coord.y >= 0; coord.y--)
                        for (coord.x = 0; coord.x < size.cx; coord.x++)
                        {
                                ZIL_COLOR color = GpiQueryRGBColor(memoryPS, LCOLOPT_REALIZED,
                                        GpiQueryPel(memoryPS, &coord));
                                if (color == MapColor(&colorMap[WHITE], TRUE))
                                        *pixel = 0xFF;
                                pixel++;
                        }
        }

        GpiSetBitmap(memoryPS, ZIL_NULLH(ZIL_BITMAP_HANDLE));
        GpiDestroyPS(memoryPS);
        DevCloseDC(memoryDC);
}

void UI_OS2_DISPLAY::DestroyBitmapHandle(ZIL_SCREENID, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
        // Free the color and monochrome bitmap handles.
        if (colorBitmap && *colorBitmap)
        {
                GpiDeleteBitmap(*colorBitmap);
                *colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
        }
        if (monoBitmap && *monoBitmap)
        {
                GpiDeleteBitmap(*monoBitmap);
                *monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
        }
}

void UI_OS2_DISPLAY::Ellipse(ZIL_SCREENID screenID, int column, int line,
        int startAngle, int endAngle, int xRadius, int yRadius,
        const UI_PALETTE *palette, int fill, int _xorInt, const UI_REGION *clip)
{
        // Virtualize the display. (This call set up UI_OS2_DISPLAY::hps.)
        int left = column - xRadius + 1;
        int top = line - yRadius + 1;
        int right = column + xRadius - 1;
        int bottom = line + yRadius - 1;
        while (startAngle < 0)
                startAngle += 360, endAngle += 360;
        if (startAngle > 360)
                startAngle %= 360;
        while (endAngle < 0)
                endAngle += 360;
        if (endAngle > 360)
                endAngle %= 360;

        VirtualGet(screenID, left, top, right, bottom);
        HRGN hRegion = 0;
        if (clip)
        {
                RECTL os2Region;
                os2Region.xLeft = clip->left - _virtualRegion.left;
                os2Region.yBottom = _virtualRegion.bottom - clip->bottom;
                os2Region.xRight = clip->right - _virtualRegion.left + 1;
                os2Region.yTop = _virtualRegion.bottom - clip->top + 1;
                hRegion = GpiCreateRegion(hps, 1, &os2Region);
                if (hRegion)
                        GpiSetClipRegion(hps, hRegion, ZIL_NULLP(HRGN));
        }

        // Set the color and xor combinations.
        ZIL_COLOR colorForeground, colorBackground;
        if (palette)
        {
                colorForeground = GpiQueryColor(hps);
                colorBackground = GpiQueryBackColor(hps);
                GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorForeground : MapColor(palette, FALSE));
                GpiSetBackColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorBackground : MapColor(palette, TRUE));
        }
        LONG mix;
        if (_xorInt)
        {
                mix = GpiQueryMix(hps);
                GpiSetMix(hps, FM_XOR);
        }

        // Compute the arc points.
        ARCPARAMS arc;
        arc.lP = xRadius;
        arc.lQ = yRadius;
        arc.lR = 0;
        arc.lS = 0;
        GpiSetArcParams(hps, &arc);

        POINTL os2Position;
        os2Position.x = column - _virtualRegion.left;
        os2Position.y = _virtualRegion.bottom - line;
        GpiMove(hps, &os2Position);

        // Draw the arc.
        if (startAngle == 0 && endAngle == 360)
                GpiFullArc(hps, fill ? DRO_OUTLINEFILL : DRO_OUTLINE, MAKEFIXED(1, 0));
        else if (fill)
        {
                if (palette)
                        GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorBackground : MapColor(palette, FALSE));
                GpiBeginArea(hps, BA_ALTERNATE);
                GpiPartialArc(hps, &os2Position, MAKEFIXED(1, 0),
                        MAKEFIXED(startAngle, 0), MAKEFIXED(endAngle - startAngle, 0));
                GpiLine(hps, &os2Position);
                GpiEndArea(hps);
        }
        else
        {
                GpiPartialArc(hps, &os2Position, MAKEFIXED(1, 0),
                        MAKEFIXED(startAngle, 0), MAKEFIXED(endAngle, 0));
                GpiLine(hps, &os2Position);
        }

        // Reset the color and xor combinations.
        if (palette)
        {
                GpiSetBackColor(hps, colorBackground);
                GpiSetColor(hps, colorForeground);
        }
        if (_xorInt)
                GpiSetMix(hps, mix);

        // Un-virtualize the display.
        if (hRegion)
        {
                GpiSetClipRegion(hps, ZIL_NULLH(HRGN), ZIL_NULLP(HRGN));
                GpiDestroyRegion(hps, hRegion);
        }
        VirtualPut(screenID);
}

void UI_OS2_DISPLAY::IconArrayToHandle(ZIL_SCREENID screenID, int iconWidth,
        int iconHeight, const ZIL_UINT8 *iconArray, const UI_PALETTE *palette,
        ZIL_ICON_HANDLE *icon)
{
        // Make sure there is a valid icon.
        if (!iconArray || *icon)
                return;

        int ICON_WIDTH;
        int ICON_HEIGHT;
        if (columns > 800 && lines > 600)
        {
                ICON_WIDTH = 40;
                ICON_HEIGHT = 40;
        }
        else
        {
                ICON_WIDTH = 32;
                ICON_HEIGHT = 32;
        }

        ZIL_UINT8 *newArray = ZIL_NULLP(ZIL_UINT8);
        if (iconWidth < ICON_WIDTH || iconHeight < ICON_HEIGHT)
        {
                int xMargin = (ICON_WIDTH - iconWidth) / 2;
                int yMargin = (ICON_HEIGHT - iconHeight) / 2;
                int index = 0;

                newArray = new ZIL_UINT8[ICON_WIDTH * ICON_HEIGHT];
                memset(newArray, 0xFF, ICON_WIDTH * ICON_HEIGHT);
                for (int y = 0; y < ICON_HEIGHT; y++)
                {
                        if (y >= yMargin && y < yMargin + iconHeight)
                                for (int x = 0; x < ICON_WIDTH; x++)
                                        if (x >= xMargin && x < xMargin + iconWidth)
                                                newArray[y * ICON_WIDTH + x] = iconArray[index++];
                }
                iconArray = newArray;
                iconWidth = ICON_WIDTH;
                iconHeight = ICON_HEIGHT;
        }

        if (!_monoBitmapInfo)
        {
                _monoBitmapInfo = (BITMAPINFO2 *)new ZIL_INT8[sizeof(BITMAPINFO2) + 255 * sizeof(RGB2)];
                memset(_monoBitmapInfo, 0, sizeof(BITMAPINFO2) + 255 * sizeof(RGB2));
                _monoBitmapInfo->cbFix = sizeof(BITMAPINFO2) - sizeof(RGB2);
                _monoBitmapInfo->cPlanes = 1;
                _monoBitmapInfo->cBitCount = 1;
                _monoBitmapInfo->ulCompression = BCA_UNCOMP;
                _monoBitmapInfo->usUnits = BRU_METRIC;
                _monoBitmapInfo->usRecording = BRA_BOTTOMUP;
                _monoBitmapInfo->usRendering = BRH_NOTHALFTONED;
                _monoBitmapInfo->ulColorEncoding = BCE_RGB;
                _monoBitmapInfo->argbColor[0].bBlue = 0x00;
                _monoBitmapInfo->argbColor[0].bGreen = 0x00;
                _monoBitmapInfo->argbColor[0].bRed = 0x00;
                _monoBitmapInfo->argbColor[1].bBlue = 0xFF;
                _monoBitmapInfo->argbColor[1].bGreen = 0xFF;
                _monoBitmapInfo->argbColor[1].bRed = 0xFF;

                _monoBitmapHeaderInfo = new BITMAPINFOHEADER2;
                memset(_monoBitmapHeaderInfo, 0, sizeof(BITMAPINFOHEADER2));
                _monoBitmapHeaderInfo->cbFix = sizeof(BITMAPINFOHEADER2);
                _monoBitmapHeaderInfo->cPlanes = 1;
                _monoBitmapHeaderInfo->cBitCount = 1;
        }

        // Set the bitmap width and height.
        _monoBitmapHeaderInfo->cPlanes = 1;
        _monoBitmapHeaderInfo->cBitCount = 1;
        _monoBitmapInfo->cx = _monoBitmapHeaderInfo->cx = iconWidth;
        _monoBitmapInfo->cy = _monoBitmapHeaderInfo->cy = iconHeight * 2;

        // alignWidth pads the bitmap scan-lines for display purposes.
        int lineWidth = (iconWidth + (8 * sizeof(ULONG)) - 1) / (8 * sizeof(ULONG)) * sizeof(ULONG);
        int size = lineWidth * iconHeight;

        VirtualGet(screenID, 0, 0, iconWidth - 1, iconHeight - 1);

        // Generate AND and XOR monochrome bitmaps.
        ZIL_UINT8 *bitmap = new ZIL_UINT8[size * 2];
        for (int _and = 0; _and <= 1; _and++)
        {
                const ZIL_UINT8 *tBitmapArray = iconArray;
                for (int y = 0; y < iconHeight; y++)
                {
                        int offset = (iconHeight - y - 1) * lineWidth + _and * size;
                        bitmap[offset] = 0x00;
                        ZIL_UINT16 mask = 0x80;
                        for (int x = 0; x < iconWidth; x++)
                        {
                                if (mask == 0x00)
                                {
                                        offset++;
                                        bitmap[offset] = 0x00;
                                        mask = 0x80;
                                }
                                if (_and && *tBitmapArray++ == BACKGROUND)
                                                bitmap[offset] |= mask;
                                mask = mask >> 1;
                        }
                }
        }
        ZIL_BITMAP_HANDLE pointerBitmap = GpiCreateBitmap(hps, _monoBitmapHeaderInfo, CBM_INIT, (PBYTE)bitmap, _monoBitmapInfo);
        delete bitmap;

        ZIL_BITMAP_HANDLE colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
        BitmapArrayToHandle(screenID, iconWidth, iconHeight, iconArray,
                palette, &colorBitmap, ZIL_NULLP(ZIL_BITMAP_HANDLE));

        POINTERINFO pInfo;
        pInfo.fPointer = FALSE;
        pInfo.xHotspot = 15;
        pInfo.yHotspot = 16;
        pInfo.hbmPointer = pointerBitmap;
        pInfo.hbmColor = colorBitmap;
        pInfo.hbmMiniPointer = pointerBitmap;
        pInfo.hbmMiniColor = colorBitmap;

        *icon = WinCreatePointerIndirect(HWND_DESKTOP, &pInfo);
        GpiDeleteBitmap(pointerBitmap);
        GpiDeleteBitmap(colorBitmap);

        if (newArray)
                delete newArray;

        VirtualPut(screenID);
}

void UI_OS2_DISPLAY::IconHandleToArray(ZIL_SCREENID screenID, ZIL_ICON_HANDLE icon,
        int *iconWidth, int *iconHeight, ZIL_UINT8 **iconArray)
{
        /* unused parameters */ (void)screenID; (void)icon; (void)iconWidth; (void)iconHeight; (void)iconArray;
}

void UI_OS2_DISPLAY::DestroyIconHandle(ZIL_SCREENID, ZIL_ICON_HANDLE *icon)
{
        *icon = 0;
}

void UI_OS2_DISPLAY::Line(ZIL_SCREENID screenID, int column1, int line1,
        int column2, int line2, const UI_PALETTE *palette, int width, int _xorInt,
        const UI_REGION *clip)
{
        // Virtualize the display. (This call set up UI_OS2_DISPLAY::hps.)
        VirtualGet(screenID, MinValue(column1, column2), MinValue(line1, line2),
                MaxValue(column1, column2), MaxValue(line1, line2));
        HRGN hRegion = 0;
        if (clip)
        {
                RECTL os2Region;
                os2Region.xLeft = clip->left - _virtualRegion.left;
                os2Region.yBottom = _virtualRegion.bottom - clip->bottom;
                os2Region.xRight = clip->right - _virtualRegion.left + 1;
                os2Region.yTop = _virtualRegion.bottom - clip->top + 1;
                hRegion = GpiCreateRegion(hps, 1, &os2Region);
                if (hRegion)
                        GpiSetClipRegion(hps, hRegion, ZIL_NULLP(HRGN));
        }

        // Set the color and xor combinations.
        ZIL_COLOR color;
        if (palette)
        {
                color = GpiQueryColor(hps);
                GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorForeground : MapColor(palette, TRUE));
        }
        LONG mix;
        if (_xorInt)
        {
                mix = GpiQueryMix(hps);
                GpiSetMix(hps, FM_XOR);
        }

        if (width > 1)
                GpiBeginPath(hps, 1);

        // Draw the line.
        POINTL os2Position;
        os2Position.x = column1 - _virtualRegion.left;
        os2Position.y = _virtualRegion.bottom - line1;
        GpiMove(hps, &os2Position);     // Move to the starting position.
        os2Position.x = column2 - _virtualRegion.left;
        os2Position.y = _virtualRegion.bottom - line2;
        GpiLine(hps, &os2Position);     // Draw to the ending position.

        // Reset the color and xor combinations.
        if (width > 1)
        {
                GpiEndPath(hps);
                GpiSetLineWidthGeom(hps, width);
                GpiStrokePath(hps, 1, 0);
        }

        if (palette)
                GpiSetColor(hps, color);
        if (_xorInt)
                GpiSetMix(hps, mix);

        // Un-virtualize the display.
        if (hRegion)
        {
                GpiSetClipRegion(hps, ZIL_NULLH(HRGN), ZIL_NULLP(HRGN));
                GpiDestroyRegion(hps, hRegion);
        }
        VirtualPut(screenID);
}

ZIL_COLOR UI_OS2_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
        ZIL_COLOR color = foreground ? palette->colorForeground : palette->colorBackground;
        LONG colorData[QCD_LCT_FORMAT + 1];
        GpiQueryColorData(hps, 1, colorData);
        if (palette->fillPattern == PTN_RGB_COLOR)
        {
                if (colorData[QCD_LCT_FORMAT] != LCOLF_RGB)
                        GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, ZIL_NULLP(LONG));
        }
        else
        {
                if (colorData[QCD_LCT_FORMAT] == LCOLF_RGB)
                        GpiCreateLogColorTable(hps, LCOL_RESET, LCOLF_CONSECRGB, 0, 0, ZIL_NULLP(LONG));
                if (palette->fillPattern == PTN_SYSTEM_COLOR)
                        color = GpiQueryRGBColor(hps, LCOLOPT_REALIZED, color);
                else if (foreground)
                        color = colorMap[color].colorForeground;
                else
                        color = colorMap[color].colorBackground;
        }
        return (color);
}

void UI_OS2_DISPLAY::Polygon(ZIL_SCREENID screenID, int numPoints,
        const int *polygonPoints, const UI_PALETTE *palette, int fill, int _xorInt,
        const UI_REGION *clip)
{
        // Determine the maximum region that bounds the polygon.
        int left = 0xFFFF, top = 0xFFFF;
        int right = 0, bottom = 0;
        POINTL *os2Polygon = new POINTL[numPoints];
        for (int i = 0; i < numPoints; i++)
        {
                int column = polygonPoints[i*2];
                int line = polygonPoints[i*2+1];
                if (column < left)
                        left = column;
                if (column > right)
                        right = column;
                if (line < top)
                        top = line;
                if (line > bottom)
                        bottom = line;
        }

        // Virtualize the display. (This call set up UI_OS2_DISPLAY::hps.)
        VirtualGet(screenID, left, top, right, bottom);
        HRGN hRegion = 0;
        if (clip)
        {
                RECTL os2Region;
                os2Region.xLeft = clip->left - _virtualRegion.left;
                os2Region.yBottom = _virtualRegion.bottom - clip->bottom;
                os2Region.xRight = clip->right - _virtualRegion.left + 1;
                os2Region.yTop = _virtualRegion.bottom - clip->top + 1;
                hRegion = GpiCreateRegion(hps, 1, &os2Region);
                if (hRegion)
                        GpiSetClipRegion(hps, hRegion, ZIL_NULLP(HRGN));
        }

        // Set up the polygon points.
        for (int j = 0; j < numPoints; j++)
        {
                os2Polygon[j].x = polygonPoints[j*2] - _virtualRegion.left;
                os2Polygon[j].y = _virtualRegion.bottom - polygonPoints[j*2+1];
        }

        // Set the color and xor combinations.
        ZIL_COLOR colorForeground, colorBackground;
        if (palette)
        {
                colorForeground = GpiQueryColor(hps);
                colorBackground = GpiQueryBackColor(hps);
                GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorForeground : MapColor(palette, TRUE));
                GpiSetBackColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorBackground : MapColor(palette, FALSE));
        }
        LONG mix;
        if (_xorInt)
        {
                mix = GpiQueryMix(hps);
                GpiSetMix(hps, FM_XOR);
        }

        // Draw the polygon.
        if (fill)
        {
                GpiBeginArea(hps, BA_ALTERNATE);
                GpiMove(hps, &os2Polygon[numPoints-1]);
                GpiPolyLine(hps, numPoints, os2Polygon);
                GpiEndArea(hps);
        }
        else
        {
                GpiMove(hps, &os2Polygon[0]);
                GpiPolyLine(hps, numPoints, os2Polygon);
        }

        // Reset the color and xor combinations.
        if (palette)
        {
                GpiSetBackColor(hps, colorBackground);
                GpiSetColor(hps, colorForeground);
        }
        if (_xorInt)
                GpiSetMix(hps, mix);

        // Un-virtualize the display.
        if (hRegion)
        {
                GpiSetClipRegion(hps, ZIL_NULLH(HRGN), ZIL_NULLP(HRGN));
                GpiDestroyRegion(hps, hRegion);
        }
        VirtualPut(screenID);
}

void UI_OS2_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top,
        int right, int bottom, const UI_PALETTE * palette, int width, int fill,
        int _xorInt, const UI_REGION *clip)
{
        // Virtualize the display. (This call set up UI_OS2_DISPLAY::hps.)
        VirtualGet(screenID, left, top, right, bottom);
        HRGN hRegion = 0;
        if (clip)
        {
                RECTL os2Region;
                os2Region.xLeft = clip->left - _virtualRegion.left;
                os2Region.yBottom = _virtualRegion.bottom - clip->bottom;
                os2Region.xRight = clip->right - _virtualRegion.left + 1;
                os2Region.yTop = _virtualRegion.bottom - clip->top + 1;
                hRegion = GpiCreateRegion(hps, 1, &os2Region);
                if (hRegion)
                        GpiSetClipRegion(hps, hRegion, ZIL_NULLP(HRGN));
        }

        // Set the color and xor combinations.
        ZIL_COLOR color;
        if (palette)
        {
                color = GpiQueryColor(hps);
                GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorForeground : MapColor(palette, TRUE));
        }
        LONG mix;
        LONG backMix;
        if (_xorInt)
        {
                mix = GpiQueryMix(hps);
                backMix = GpiQueryBackMix(hps);
                GpiSetMix(hps, FM_XOR);
                GpiSetBackMix(hps, BM_XOR);
//              GpiSetLineType(hps, LINETYPE_ALTERNATE);
        }

        // Draw the outer rectangle.
        POINTL os2Position1, os2Position2;
        os2Position1.x = left - _virtualRegion.left;
        os2Position1.y = _virtualRegion.bottom - bottom;
        os2Position2.x = right - _virtualRegion.left;
        os2Position2.y = _virtualRegion.bottom - top;
        for (int i = 0; i < width; i++)
        {
                GpiMove(hps, &os2Position1);
                os2Position1.x++;
                os2Position1.y++;
                GpiBox(hps, DRO_OUTLINE, &os2Position2, 0, 0);
                os2Position2.x--;
                os2Position2.y--;
        }
        
        // Fill the inner rectangle.
        if (fill)
        {
                GpiMove(hps, &os2Position1);
                os2Position1.x++;
                os2Position1.y++;
                ZIL_COLOR colorBackground;
                if (palette)
                        colorBackground = (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorBackground : MapColor(palette, FALSE);
                else
                        colorBackground = GpiQueryBackColor(hps);
                if (colorBackground == CLR_ERROR)
                        colorBackground = SYSCLR_ENTRYFIELD;
                GpiSetColor(hps, colorBackground);
                GpiBox(hps, DRO_FILL, &os2Position2, 0, 0);
                os2Position2.x--;
                os2Position2.y--;
        }

        // Reset the xor combination and fill the inner rectangle.
        if (_xorInt)
        {
                GpiSetMix(hps, mix);
                GpiSetBackMix(hps, backMix);
//              GpiSetLineType(hps, LINETYPE_DEFAULT);
        }

        // Reset the clip region and color combination.
        if (palette)
                GpiSetColor(hps, color);

        // Un-virtualize the display.
        if (hRegion)
        {
                GpiSetClipRegion(hps, ZIL_NULLH(HRGN), ZIL_NULLP(HRGN));
                GpiDestroyRegion(hps, hRegion);
        }
        VirtualPut(screenID);
}

void UI_OS2_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
        const UI_REGION &newRegion, ZIL_SCREENID screenID, const UI_REGION *)
{
        // Virtualize the display. (This call set up UI_OS2_DISPLAY::hps.)
        VirtualGet(screenID, MinValue(oldRegion.left, newRegion.left),
                MinValue(oldRegion.top, newRegion.top), MaxValue(oldRegion.right, newRegion.right),
                MinValue(oldRegion.bottom, newRegion.bottom));

        // Set the xor mix.
        LONG mix = GpiQueryMix(hps);
        GpiSetMix(hps, FM_XOR);

        // Draw the two rectangles.
        POINTL os2Position;
        os2Position.x = oldRegion.left - _virtualRegion.left;
        os2Position.y = _virtualRegion.bottom - oldRegion.bottom;
        GpiMove(hps, &os2Position);
        os2Position.x = oldRegion.right - _virtualRegion.left + 1;
        os2Position.y = _virtualRegion.bottom - oldRegion.top + 1;
        GpiBox(hps, DRO_OUTLINE, &os2Position, 0, 0);
        os2Position.x = newRegion.left - _virtualRegion.left;
        os2Position.y = _virtualRegion.bottom - newRegion.bottom;
        GpiMove(hps, &os2Position);
        os2Position.x = newRegion.right - _virtualRegion.left + 1;
        os2Position.y = _virtualRegion.bottom - newRegion.top + 1;
        GpiBox(hps, DRO_OUTLINE, &os2Position, 0, 0);

        // Reset the xor mix.
        GpiSetMix(hps, mix);

        // Un-virtualize the display.
        VirtualPut(screenID);
}

void UI_OS2_DISPLAY::RegionMove(const UI_REGION &oldRegion, int newColumn,
        int newLine, ZIL_SCREENID oldScreenID, ZIL_SCREENID newScreenID)
{
        // Get the device context.
        HPS _hDestPS, _hSrcPS;

        if (newScreenID == ID_SCREEN)
                newScreenID = HWND_DESKTOP;
        else
        {
                ZIL_SCREENID clientID = WinWindowFromID(newScreenID, FID_CLIENT);
                if (clientID)
                {
                        if (oldScreenID == newScreenID)
                                oldScreenID = clientID;
                        newScreenID = clientID;
                }
        }
        _hDestPS = WinGetPS(newScreenID);
        if (oldScreenID == newScreenID)
                _hSrcPS = _hDestPS;
        else
        {
                if (oldScreenID == ID_SCREEN)
                        oldScreenID = HWND_DESKTOP;
                else
                {
                        ZIL_SCREENID clientID = WinWindowFromID(oldScreenID, FID_CLIENT);
                        if (clientID)
                                oldScreenID = clientID;
                }
                _hSrcPS = WinGetPS(oldScreenID);
        }

        SWP oldSwp, newSwp;
        WinQueryWindowPos(newScreenID, &oldSwp);
        WinQueryWindowPos(newScreenID, &newSwp);

        POINTL point[3];
        point[0].x = newColumn;
        point[0].y = newSwp.cy - newLine - oldRegion.Height();
        point[1].x = point[0].x + oldRegion.Width();
        point[1].y = point[0].y + oldRegion.Height();
        point[2].x = oldRegion.left;
        point[2].y = oldSwp.cy - oldRegion.bottom - 1;
        GpiBitBlt(_hDestPS, _hSrcPS, 3, point, ROP_SRCCOPY, BBO_IGNORE);

        WinReleasePS(_hDestPS);
        if (oldScreenID != newScreenID)
                WinReleasePS(_hSrcPS);
}

void UI_OS2_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
        const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
        int _xorInt, const UI_REGION *clip, ZIL_LOGICAL_FONT logicalFont)
{
        // Make sure there is a valid string.
        if (!text || text[0] == '\0')
                return;

        // Virtualize the display. (This call set up UI_OS2_DISPLAY::hps.)
        int right = left + TextWidth(text, screenID, logicalFont) - 1;
        int bottom = top + TextHeight(text, screenID, logicalFont) - 1;
        VirtualGet(screenID, left, top, right, bottom);
        HRGN hRegion = 0;
        if (clip)
        {
                RECTL os2Region;
                os2Region.xLeft = clip->left - _virtualRegion.left;
                os2Region.yBottom = _virtualRegion.bottom - clip->bottom;
                os2Region.xRight = clip->right - _virtualRegion.left;
                os2Region.yTop = _virtualRegion.bottom - clip->top;
                hRegion = GpiCreateRegion(hps, 1, &os2Region);
                if (hRegion)
                        GpiSetClipRegion(hps, hRegion, ZIL_NULLP(HRGN));
        }

        // Initialize the region.
        RECTL os2Region;
        os2Region.xLeft = left - _virtualRegion.left;
        os2Region.yBottom = _virtualRegion.bottom - bottom;
        os2Region.xRight = right - _virtualRegion.left + 1;
        os2Region.yTop = _virtualRegion.bottom - top;

        // Set the font, color and xor combinations.
        ULONG underscore = FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE) ? 0 : DT_MNEMONIC;
        logicalFont &= 0x0FFF;

        LONG oldFont = GpiQueryCharSet(hps);
        SetFont(logicalFont);
        ZIL_COLOR colorForeground, colorBackground;
        if (palette)
        {
                colorForeground = GpiQueryColor(hps);
                colorBackground = GpiQueryBackColor(hps);
                GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorForeground : MapColor(palette, TRUE));
                GpiSetBackColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                        palette->colorBackground : MapColor(palette, FALSE));
        }
        else
        {
                colorForeground = GpiQueryColor(hps);
                GpiSetColor(hps, SYSCLR_OUTPUTTEXT);
        }
        LONG mix;
        if (_xorInt)
        {
                mix = GpiQueryMix(hps);
                GpiSetMix(hps, FM_XOR);
        }

        // Compute the text and draw the text.
        ULONG drawFlags = DT_LEFT | DT_VCENTER | DT_TEXTATTRS | underscore;
        if (fill)
                drawFlags |= DT_ERASERECT;
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
        ZIL_ICHAR *tmp = strdup(text);
        if (length != -1)
                tmp[length] = 0;
        char *TEXT_ = MapText(tmp, ZIL_NULLP(char), FALSE);
        delete tmp;
        length = ::strlen(TEXT_);
#else
        if (length == -1)
                length = strlen(text);
#       define TEXT_ text
#endif
        WinDrawText(hps, length, (PSZ)TEXT_, &os2Region, 0, 0, drawFlags);

        // Reset the font, color and xor combinations.
        if (logicalFont < ZIL_MAXFONTS && fontTable[logicalFont].lMatch)
                GpiSetCharSet(hps, oldFont);
        if (palette)
                GpiSetBackColor(hps, colorBackground);
        GpiSetColor(hps, colorForeground);
        if (_xorInt)
                GpiSetMix(hps, mix);

        // Un-virtualize the display.
        if (hRegion)
        {
                GpiSetClipRegion(hps, ZIL_NULLH(HRGN), ZIL_NULLP(HRGN));
                GpiDestroyRegion(hps, hRegion);
        }
        VirtualPut(screenID);
}

int UI_OS2_DISPLAY::TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID screenID,
        ZIL_LOGICAL_FONT logicalFont)
{
        // Make sure there is a valid string.
        if (!string || !string[0] || (!screenID && !hps))
                return (cellHeight - preSpace - postSpace);

        VirtualGet(screenID, 0, 0, lines - 1, columns - 1);

        // Set the font.
        SetFont(logicalFont & 0x0FFF);

        RECTL os2Region;
        os2Region.xLeft = _virtualRegion.left;
        os2Region.yBottom = _virtualRegion.top;
        os2Region.xRight = _virtualRegion.right;
        os2Region.yTop = _virtualRegion.bottom;
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
        char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
#else
#       define TEXT_ string
#endif
        WinDrawText(hps, ::strlen(TEXT_), (PSZ)TEXT_, &os2Region, 0, 0, DT_QUERYEXTENT | DT_MNEMONIC);
        VirtualPut(screenID);

        return (os2Region.yTop - os2Region.yBottom + 1);
}

int UI_OS2_DISPLAY::TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID screenID,
        ZIL_LOGICAL_FONT logicalFont)
{
        // Make sure there is a valid string.
        if (!string || !string[0] || (!screenID && !hps))
                return (strlen(string) * cellWidth);

        HPS _hps = hps ? hps : WinGetPS(screenID);

        // Set the font.
        SetFont(logicalFont & 0x0FFF);

        RECTL os2Region;
        os2Region.xLeft = 0;
        os2Region.yBottom = 0;
        os2Region.xRight = columns - 1;
        os2Region.yTop = lines - 1;
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
        char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
#else
#       define TEXT_ string
#endif
        WinDrawText(_hps, ::strlen(TEXT_), (PSZ)TEXT_, &os2Region, 0, 0, DT_QUERYEXTENT | DT_MNEMONIC);

        if (!hps)
                WinReleasePS(_hps);

        return (os2Region.xRight - os2Region.xLeft + 1);
}

void UI_OS2_DISPLAY::SetFont(ZIL_LOGICAL_FONT logicalFont)
{
        if (GpiQueryCharSet(hps) != logicalFont + 1)
        {
                if (logicalFont < ZIL_MAXFONTS && fontTable[logicalFont].lMatch)
                {
                        if (!GpiSetCharSet(hps, logicalFont + 1))
                        {
                                FATTRS drawFont;
                                drawFont.usRecordLength = sizeof(FATTRS);
                                drawFont.fsSelection = fontTable[logicalFont].fsSelection;
                                drawFont.lMatch = fontTable[logicalFont].lMatch;
                                ::strcpy(drawFont.szFacename, fontTable[logicalFont].szFacename);
                                drawFont.idRegistry = fontTable[logicalFont].idRegistry;
                                drawFont.usCodePage = fontTable[logicalFont].usCodePage;
                                drawFont.lMaxBaselineExt = fontTable[logicalFont].lMaxBaselineExt;
                                drawFont.lAveCharWidth = fontTable[logicalFont].lAveCharWidth;
                                drawFont.fsType = 0;
                                drawFont.fsFontUse = FATTR_FONTUSE_NOMIX;
                                GpiCreateLogFont(hps, ZIL_NULLP(STR8), logicalFont + 1, &drawFont);
                                GpiSetCharSet(hps, logicalFont + 1);
                        }
                }
                else
                        GpiSetCharSet(hps, 0);
        }
}

int UI_OS2_DISPLAY::VirtualGet(ZIL_SCREENID screenID, int left, int top,
        int right, int bottom)
{
        if (--_virtualCount == -1)
        {
                if (screenID == ID_DIRECT)
                {
                        _virtualRegion.left = left;
                        _virtualRegion.top = top;
                        _virtualRegion.right = right;
                        _virtualRegion.bottom = bottom;
                }
                else if (screenID == ID_SCREEN)
                {
                        hps = WinGetPS(HWND_DESKTOP);
                        _virtualRegion.left = _virtualRegion.top = 0;
                        _virtualRegion.right = columns - 1;
                        _virtualRegion.bottom = lines - 1;
                }
                else
                {
                        hps = WinGetPS(screenID);

                        RECTL rect;
                        WinQueryWindowRect(screenID, &rect);
                        _virtualRegion.left = left;
                        _virtualRegion.top = top;
                        _virtualRegion.right = right;
                        _virtualRegion.bottom = top + rect.yTop - rect.yBottom - 1;

                }
        }
        return (TRUE);
}

int UI_OS2_DISPLAY::VirtualPut(ZIL_SCREENID screenID)
{
        if (++_virtualCount == 0)
        {
                if (screenID != ID_DIRECT && hps)
                        WinReleasePS(hps);
                hps = 0;
        }
        return (TRUE);
}
