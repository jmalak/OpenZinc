//      O_PRINT.CPP - Printer display class.
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// Work around for IBM C Set++ compiler bug.
#if defined(__IBMCPP__)
#       if __IBMCPP__ <= 210
#               define GpiQueryColorData        GpiQueryColordata
#       endif
#endif

#define INCL_GPI
#define INCL_DEV
#define INCL_WIN
#include <stdio.h>

#define INCL_SPLDOSPRINT
#include "ui_win.hpp"
#include <pmspl.h>

static FILE *_outputFile;

static SIZEL _size;
static int _transformNumerator;
static int _transformDenominator;
static int maxColors = 16;

UI_PRINTER::UI_PRINTER(UI_DISPLAY *_display) : UI_DISPLAY(TRUE),
        display(_display)
{
        char printerString[80];
        char buffer[257];
        DEVOPENSTRUC printerInfo;

        if (PrfQueryProfileString(HINI_PROFILE, (PSZ)"PM_SPOOLER", (PSZ)"PRINTER", 0,
                printerString, sizeof(printerString)))
        {
                printerString[::strlen(printerString) - 1] = 0;

                if (PrfQueryProfileString(HINI_PROFILE, (PSZ)"PM_SPOOLER_PRINTER", (PSZ)printerString,
                        0, buffer, sizeof(buffer)))
                {
                        char *ptscan;
                        ptscan = ::strchr(buffer, ';');
                        ptscan++;
                        ptscan = ::strchr(ptscan, ';');
                        ptscan++;
                        *(ptscan + ::strcspn(ptscan, ".,;")) = 0;
                        printerInfo.pszLogAddress = (PSZ)ptscan;

                        ptscan = ::strchr(buffer, ';');
                        ptscan++;
                        *(ptscan + ::strcspn(ptscan, ".,;")) = 0;
                        printerInfo.pszDriverName = (PSZ)ptscan;

                        hdc = DevOpenDC((HAB)0, OD_QUEUED, (PSZ)"*", 2L, (PDEVOPENDATA)&printerInfo, (HDC)0);

                        SIZEL size = {0, 0};
                        hps = GpiCreatePS((HAB)0, hdc, &size, PU_PELS | GPIA_ASSOC);
                        GpiQueryPS(hps, &_size);

                        // Set display members.
                        isText = FALSE;
                        if (display)
                        {
                                columns = display->columns;
                                lines = display->lines;
                                cellWidth = display->cellWidth;
                                cellHeight = display->cellHeight;

                                if (_size.cx / columns <= _size.cy / lines)
                                {
                                        _transformNumerator = _size.cx;
                                        _transformDenominator = display->columns;
                                }
                                else
                                {
                                        _transformNumerator = _size.cy;
                                        _transformDenominator = display->lines;
                                }
                        }
                        else
                        {
                                if (printerMode == PRM_POSTSCRIPT)
                                {
                                        columns = 612;
                                        lines = 792;
                                        cellWidth = 8;
                                        cellHeight = 23;
                                }
                                else
                                {
                                        columns = _size.cx;
                                        lines = _size.cy;
                                }

                                _transformNumerator = _transformDenominator = 1;
                        }

                        installed = TRUE;
                }
        }
}

UI_PRINTER::~UI_PRINTER(void)
{
        if (installed)
        {
                GpiDestroyPS(hps);
                DevCloseDC(hdc);
        }
}

void UI_PRINTER::Bitmap(ZIL_SCREENID, int x, int y,
                int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
                const UI_PALETTE *,     const UI_REGION *,
                ZIL_BITMAP_HANDLE *_colorBitmap, ZIL_BITMAP_HANDLE *_monoBitmap)
{
        if (printerMode == PRM_POSTSCRIPT)
        {
                int tWidth = bitmapWidth % 2 ? bitmapWidth + 1 : bitmapWidth;
                fprintf(_outputFile, "%d %d %d %d Bitmap\n", x, y, tWidth, bitmapHeight);

                for (int i = 0; i < bitmapHeight; i++)
                {
                        for (int j = 0; j < bitmapWidth; j++)
                                fprintf(_outputFile, "%x", bitmapArray[i*bitmapWidth+j] % 16);
                        if (bitmapWidth % 2)
                                fprintf(_outputFile, "f");
                        fprintf(_outputFile, "\n");
                }
                if (bitmapHeight % 2)
                {
                        for (int i = 0; i < tWidth; i++)
                                fprintf(_outputFile, "f");
                        fprintf(_outputFile, "\n");
                }
        }
        else
        {
                ZIL_BITMAP_HANDLE colorBitmap = 0, monoBitmap = 0;
                if (_colorBitmap)
                        colorBitmap = *_colorBitmap;
                if (_monoBitmap)
                        monoBitmap = *_monoBitmap;
                if (!bitmapArray && !colorBitmap)
                        return;

                if (!colorBitmap)
                {
                        BITMAPINFO2 *bitmapInfo = ZIL_NULLP(BITMAPINFO2);
                        bitmapInfo = (BITMAPINFO2 *)new ZIL_INT8[sizeof(BITMAPINFO2) + 255 * sizeof(RGB2)];
                        memset(bitmapInfo, 0, sizeof(BITMAPINFO2) + 255 * sizeof(RGB2));
                        bitmapInfo->cbFix = sizeof(BITMAPINFO2) - sizeof(RGB2);
                        bitmapInfo->cPlanes = 1;
                        bitmapInfo->cBitCount = 8;
                        bitmapInfo->ulCompression = BCA_UNCOMP;
                        bitmapInfo->usUnits = BRU_METRIC;
                        bitmapInfo->usRecording = BRA_BOTTOMUP;
                        bitmapInfo->usRendering = BRH_NOTHALFTONED;
                        bitmapInfo->ulColorEncoding = BCE_RGB;
                        for (int i = 0; i < maxColors; i++)
                        {
                                ZIL_COLOR color = MapColor(&colorMap[i], TRUE);
                                bitmapInfo->argbColor[i].bBlue = (color & 0x000000FF);
                                bitmapInfo->argbColor[i].bGreen = (color & 0x0000FF00) >> 8;
                                bitmapInfo->argbColor[i].bRed = (color & 0x00FF0000) >> 16;
                        }

                        BITMAPINFOHEADER2 *bitmapHeaderInfo = ZIL_NULLP(BITMAPINFOHEADER2);
                        bitmapHeaderInfo = new BITMAPINFOHEADER2;
                        memset(bitmapHeaderInfo, 0, sizeof(BITMAPINFOHEADER2));
                        bitmapHeaderInfo->cbFix = sizeof(BITMAPINFOHEADER2);
                        bitmapHeaderInfo->cPlanes = 1;
                        bitmapHeaderInfo->cBitCount = 8;

                        // Set the bitmap width and height.
                        bitmapInfo->cx = bitmapHeaderInfo->cx = bitmapWidth;
                        bitmapInfo->cy = bitmapHeaderInfo->cy = bitmapHeight;

                        // alignWidth pads the bitmap scan-lines for display purposes.
                        int alignWidth = ((bitmapWidth + sizeof(ULONG) - 1) / sizeof(ULONG)) * sizeof(ULONG);

                        // AND then XOR the bitmap onto the screen.
                        int useBackground = FALSE;
                        ZIL_UINT8 *bitmap = new ZIL_UINT8[alignWidth * bitmapHeight];

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
                                        for (x = bitmapWidth; x < alignWidth; x++)
                                                color[x] = 0x00;
                                }
                                if (mix)
                                        colorBitmap = GpiCreateBitmap(hps, bitmapHeaderInfo, CBM_INIT, (PBYTE)bitmap, bitmapInfo);
                                else if (useBackground && !mix)
                                        monoBitmap = GpiCreateBitmap(hps, bitmapHeaderInfo, CBM_INIT, (PBYTE)bitmap, bitmapInfo);
                        }
                        delete bitmap;
                }

                POINTL point[4];
                point[0].x = x * _transformNumerator / _transformDenominator;
                point[0].y = _size.cy - (y + bitmapHeight - 1) * _transformNumerator / _transformDenominator - 1;
                point[1].x = (x + bitmapWidth - 1) * _transformNumerator / _transformDenominator;
                point[1].y = _size.cy - y * _transformNumerator / _transformDenominator - 1;
                point[2].x = point[2].y = 0;
                point[3].x = bitmapWidth;
                point[3].y = bitmapHeight;

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
        }
}

void UI_PRINTER::Ellipse(ZIL_SCREENID, int x, int y,
                int startAngle, int endAngle, int xRadius, int yRadius,
                const UI_PALETTE *palette, int fill, int,
                const UI_REGION *)
{
        int fColor = (int)palette->colorForeground % 16;
        int bColor = (int)palette->colorBackground % 16;

        if (printerMode == PRM_POSTSCRIPT)
        {
                fprintf(_outputFile, "%d %d %d %d %d %d %s %d %s 1 Ellipse\n",
                        startAngle, endAngle, xRadius, yRadius, x, y,
                        psColorTable[bColor], fill, psColorTable[fColor]);
        }
        else
        {
                while (startAngle < 0)
                        startAngle += 360, endAngle += 360;
                if (startAngle > 360)
                        startAngle %= 360;
                while (endAngle < 0)
                        endAngle += 360;
                if (endAngle > 360)
                        endAngle %= 360;

                // Set the color and xor combinations.
                ZIL_COLOR colorForeground, colorBackground;
                if (palette)
                {
                        GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorForeground : MapColor(palette, TRUE));
                        GpiSetBackColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorBackground : MapColor(palette, FALSE));
                }
                LONG mix;

                // Compute the arc points.
                ARCPARAMS arc;
                arc.lP = xRadius * _transformNumerator / _transformDenominator;
                arc.lQ = yRadius * _transformNumerator / _transformDenominator;
                arc.lR = 0;
                arc.lS = 0;
                GpiSetArcParams(hps, &arc);

                POINTL os2Position;
                os2Position.x = x * _transformNumerator / _transformDenominator;
                os2Position.y = _size.cy - y * _transformNumerator / _transformDenominator - 1;
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
        }
}

void UI_PRINTER::Line(ZIL_SCREENID, int x1, int y1, int x2,     int y2,
        const UI_PALETTE *palette, int width, int,      const UI_REGION *)
{
        int fColor = (int)palette->colorForeground % 16;
        if (printerMode == PRM_POSTSCRIPT)
        {
                fprintf(_outputFile, "%s %d %d %d %d %d Line\n",
                        psColorTable[fColor], width, x2, y2, x1, y1);
        }
        else
        {
                // Set the color and xor combinations.
                if (palette)
                        GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorForeground : MapColor(palette, TRUE));

                // Draw the line.
                POINTL os2Position;
                os2Position.x = x1 * _transformNumerator / _transformDenominator;
                os2Position.y = _size.cy - y1 * _transformNumerator / _transformDenominator - 1;
                GpiMove(hps, &os2Position);     // Move to the starting position.
                os2Position.x = x2 * _transformNumerator / _transformDenominator;
                os2Position.y = _size.cy - y2 * _transformNumerator / _transformDenominator - 1;
                GpiLine(hps, &os2Position);     // Draw to the ending position.
        }
}


ZIL_COLOR UI_PRINTER::MapColor(const UI_PALETTE *palette, int foreground)
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

void UI_PRINTER::Polygon(ZIL_SCREENID, int numPoints,
                const int *polygonPoints, const UI_PALETTE *palette,
                int fill, int, const UI_REGION *)
{
        int fColor = (int)palette->colorForeground % 16;
        int bColor = (int)palette->colorBackground % 16;

        if (printerMode == PRM_POSTSCRIPT)
        {
                int x, y;
                int isClosed = 0;
                int isFrame = fColor != bColor;

                if (numPoints > 2 && polygonPoints[0] == polygonPoints[numPoints * 2 - 2] &&
                        polygonPoints[1] == polygonPoints[numPoints * 2 - 1])
                        isClosed = 1;

                fprintf(_outputFile, "[");
                for (int i = 0; i < numPoints * 2; i+=2)
                {
                        x = polygonPoints[i];
                        y = polygonPoints[i+1];
                        fprintf(_outputFile, "[%d %d]", x, y);
                }
                fprintf(_outputFile, "] %d %s %d %s %d %d Polygon\n", numPoints,
                        psColorTable[bColor], fill, psColorTable[fColor], isFrame, isClosed);
        }
        else
        {
                // Set up the polygon points.
                POINTL *os2Polygon = new POINTL[numPoints];
                for (int i = 0; i < numPoints; i++)
                {
                        os2Polygon[i].x = polygonPoints[i*2] * _transformNumerator / _transformDenominator;
                        os2Polygon[i].y = _size.cy - polygonPoints[i*2+1] * _transformNumerator / _transformDenominator - 1;
                }

                if (palette)
                {
                        GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorForeground : MapColor(palette, TRUE));
                        GpiSetBackColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorBackground : MapColor(palette, FALSE));
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

                delete os2Polygon;
        }
}

void UI_PRINTER::Rectangle(ZIL_SCREENID, int left, int top, int right, int bottom, const UI_PALETTE *palette, int width,
                int fill, int, const UI_REGION *)
{
        int fColor = (int)palette->colorForeground % 16;
        int bColor = (int)palette->colorBackground % 16;


        if (printerMode == PRM_POSTSCRIPT)
        {
                fprintf(_outputFile, "%d %d %d %d %d %s %d %s Rect\n",right-left, bottom-top,
                        left, top, width, psColorTable[fColor], fill, psColorTable[bColor]);
        }
        else
        {
                if (palette)
                {
                        GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorForeground : MapColor(palette, TRUE));
                        GpiSetBackColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorBackground : MapColor(palette, FALSE));
                }

                // Calculate points.
                POINTL os2Position1, os2Position2;
                os2Position1.x = left * _transformNumerator / _transformDenominator;
                os2Position1.y = _size.cy - bottom * _transformNumerator / _transformDenominator - 1;
                os2Position2.x = right * _transformNumerator / _transformDenominator;
                os2Position2.y = _size.cy - top * _transformNumerator / _transformDenominator - 1;

                // Draw the outline.
                for (int i = 0; i < width; i++)
                {
                        GpiMove(hps, &os2Position1);
                        GpiBox(hps, DRO_OUTLINE, &os2Position2, 0, 0);
                        os2Position1.x++;
                        os2Position1.y++;
                        os2Position2.x--;
                        os2Position2.y--;
                }

                // Fill the rectangle.
                if (fill)
                {
                        GpiMove(hps, &os2Position1);
                        GpiBeginArea(hps, BA_NOBOUNDARY | BA_ALTERNATE);
                        GpiBox(hps, DRO_OUTLINE, &os2Position2, 0, 0);
                        GpiEndArea(hps);
                }
        }
}

void UI_PRINTER::Text(ZIL_SCREENID, int x, int y, const ZIL_ICHAR *text,
        const UI_PALETTE *palette, int length, int, int, const UI_REGION *, ZIL_LOGICAL_FONT font)
{

        // Make sure there is a valid string.
        if (!text || text[0] == '\0')
                return;

        static ZIL_ICHAR fillLine[256];
        if (length < 0)
                length = strlen(text);
        else if (length > 255)
                length = 255;
        strncpy(fillLine, text, length);
        fillLine[length] = 0;

        if (printerMode == PRM_POSTSCRIPT)
        {
                int fColor = (int)palette->colorForeground % 16;
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
                char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
#else
#               define TEXT_ fillLine
#endif
                fprintf(_outputFile, "%s %d %d %s %d Text (", psColorTable[fColor],
                        x, y,   psFontTable[font].typeFace, psFontTable[font].pointSize);
                for(char *ptr = TEXT_; *ptr; ptr++)
                        if (*ptr == '(')
                                fprintf(_outputFile, "\\050");
                        else if (*ptr == ')')
                                fprintf(_outputFile, "\\051");
                        else
                                fprintf(_outputFile, "%c", *ptr);
                fprintf(_outputFile,") show\n");
        }
        else
        {
                // Set the font, color and xor combinations.
//              SetFont(font & 0x0FFF);
                if (palette)
                {
                        GpiSetColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorForeground : MapColor(palette, TRUE));
                        GpiSetBackColor(hps, (palette->fillPattern == PTN_SYSTEM_COLOR) ?
                                palette->colorBackground : MapColor(palette, FALSE));
                }
                else
                        GpiSetColor(hps, CLR_BLACK);

                // Map the text.
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
                char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
                int length = ::strlen(TEXT_);
#else
                if (length == -1)
                        length = strlen(fillLine);
#               define TEXT_ fillLine
#endif

                // Draw the text.
                POINTL point;
//              point.x = x * _transformNumerator / _transformDenominator;
                point.x = x;
//              point.y = _size.cy - y * _transformNumerator / _transformDenominator - 1;
                point.y = _size.cy - y - 1;
                GpiMove(hps, &point);
                GpiCharString(hps, length, (PSZ)TEXT_);
        }
}

int UI_PRINTER::TextHeight(const ZIL_ICHAR *string, ZIL_SCREENID, ZIL_LOGICAL_FONT font)
{
        int height;
        static ZIL_ICHAR heightString[4] = { 'W', 'q', 'z', 0 };

        if (printerMode == PRM_POSTSCRIPT)
                height = psFontTable[font].pointSize;
        else
        {
                // Make sure there is a valid string.
                if (!string || !hps)
                        return (cellHeight - preSpace - postSpace);

                // Set the font.
//              SetFont(font & 0x0FFF);

                RECTL os2Region;
                os2Region.xLeft = 0;
                os2Region.yBottom = 0;
                os2Region.xRight = columns - 1;
                os2Region.yTop = lines - 1;
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
//              char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
                char *TEXT_ = MapText(heightString, ZIL_NULLP(char), FALSE);
#else
//#             define TEXT_ string
#               define TEXT_ heightString
#endif
                WinDrawText(hps, ::strlen(TEXT_), (PSZ)TEXT_, &os2Region, 0, 0, DT_QUERYEXTENT | DT_MNEMONIC);
                height = os2Region.yTop - os2Region.yBottom + 1;
        }

        return (height);
}

int UI_PRINTER::TextWidth(const ZIL_ICHAR *string,      ZIL_SCREENID, ZIL_LOGICAL_FONT)
{
        int width;

        if (printerMode == PRM_POSTSCRIPT)
                width = strlen(string) * 8;
        else
        {

                // Make sure there is a valid string.
                if (!string || !string[0] || !hps)
                        return (strlen(string) * cellWidth);

                // Set the font.
//              SetFont(font & 0x0FFF);

                RECTL os2Region;
                os2Region.xLeft = 0;
                os2Region.yBottom = 0;
                os2Region.xRight = columns;
                os2Region.yTop = lines;
#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
                char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
#else
#               define TEXT_ string
#endif
                WinDrawText(hps, ::strlen(TEXT_), (PSZ)TEXT_, &os2Region, 0, 0, DT_QUERYEXTENT | DT_MNEMONIC);
                width = os2Region.xRight - os2Region.xLeft + 1;
        }

        return (width);
}               

int UI_PRINTER::VirtualGet(ZIL_SCREENID, int, int, int, int)
{
        return (0);
}

int UI_PRINTER::VirtualPut(ZIL_SCREENID)
{
        return (0);
}

int UI_PRINTER::BeginPrintJob(ZIL_PRINTER_MODE printMode, char *fileName)
{
        printerMode = printMode;
        char tempFile[12];

        if (!fileName && printMode == PRM_POSTSCRIPT)
        {
                ::strcpy(tempFile, "print.ps");
                fileName = tempFile;
        }

        if (fileName)
        {
                printerMode = PRM_POSTSCRIPT;
                columns = 612;
                lines = 792;
                cellWidth = 8;
                cellHeight = 23;

                _outputFile = fopen(fileName, "w+");
                if (!_outputFile)
                        return FALSE;

                fprintf(_outputFile, "%%!PS\n");
                if (!isText)
                {
                        // Print PostScript routines;
                        fprintf(_outputFile, "/Bitmap { %%args: x, y, width, height\n"
                                "/height exch def /width exch def /m matrix currentmatrix def /temp width string def\n"
                                "translate newpath width height neg scale width height 4 [ width 0 0 height neg 0 0 ]\n"
                                "{ currentfile temp readhexstring pop } image m setmatrix } def\n");
                        fprintf(_outputFile, "/Ellipse { %%args: sAng, eAng, xRad, yRad, x, y, bkgd, isBg, fgnd, isFg\n"
                                "/isFrame exch def /fgColor exch def /isFill exch def /bgColor exch def\n"
                                "/m matrix currentmatrix def translate 0 0 moveto newpath 1 index neg div 1 exch scale\n"
                                "0 0 5 2 roll 3 1 roll arc closepath m setmatrix isFill 0 ne { gsave bgColor setgray\n"
                                "fill grestore } if isFrame 0 ne { 1 setlinewidth fgColor setgray stroke } if } def\n");
                        fprintf(_outputFile, "/Line { %%args: lineColor, lineWidth, x2, y2, x1, y1\n"
                                "newpath moveto lineto setlinewidth setgray stroke } def\n");
                        fprintf(_outputFile, "/Polygon { %%args: points, numPoints bkgd, isBg, fgnd, isFg, isClosed\n"
                                "/isClosed exch def /isFrame exch def /fgColor exch def /isFill exch def /bgColor exch def\n"
                                "/numPoints exch def /points exch def newpath points 0 get aload pop moveto 1 1 numPoints\n"
                                "1 sub { points exch get aload pop lineto } for isClosed 0 ne { closepath } if isFill 0 ne\n"
                                "{ gsave bgColor setgray fill grestore } if isFrame 0 ne { 1 setlinewidth fgColor setgray\n"
                                "stroke } if } def\n");
                        fprintf(_outputFile, "/Rect { %%args: width, height, x1, y1 lWidth fgColor, isFill bgColor\n"
                                "/bgColor exch def /isFill exch def /fgColor exch def /lineWidth exch def newpath moveto\n"
                                "1 index 0 rlineto 0 exch rlineto neg 0 rlineto closepath isFill 0 ne { gsave bgColor\n"
                                "setgray fill grestore } if lineWidth 1 ge { lineWidth setlinewidth fgColor setgray\n"
                                "stroke } if } def\n");
                }
                fprintf(_outputFile, "/Text { %%args: x, y, fontName, size, textColor\n"
                        "/size exch def newpath findfont [size 0 0 size neg 0 size] makefont setfont moveto\n"
                        "setgray } def\n\n");
        }
        else
        {
                static char message[] = "Printing..";
                DevEscape(hdc, DEVESC_STARTDOC, ::strlen(message), (char *)(PSZ)message, 0, 0);
        }
        return TRUE;
}


void UI_PRINTER::EndPrintJob(void)
{
        // finish print job, close file.
        if (printerMode == PRM_POSTSCRIPT)
        {
                fprintf(_outputFile, "%%%%Trailer");
                fclose(_outputFile);
        }
        else
                DevEscape(hdc, DEVESC_ENDDOC, 0, 0, 0, 0);
        _outputFile = ZIL_NULLP(FILE);
}

void UI_PRINTER::BeginPage(void)
{
        if (printerMode == PRM_POSTSCRIPT)
        {
                // Set up environment (8.5 x 11 portrait mode)
                if (display)
                {
                        float temp = 572.0 / display->columns;
                        fprintf(_outputFile, "18 774 translate %f %f scale\n", temp, -temp);
                }
                else
                        fprintf(_outputFile, "18 774 translate 1 -1 scale\n");
        }
}

void UI_PRINTER::BeginPage(int left, int top, int right, int bottom,
        int orientation, int)
{
        if (printerMode == PRM_POSTSCRIPT)
        {
                if (orientation == PRM_LANDSCAPE)
                {
                        float xScale = (right - left) ? 792.0 / (right - left) : 1.0;
                        float yScale = (bottom - top) ? 612.0 / (bottom - top) : 1.0;
                        columns = (right - left) ? 792 : right-left;
                        lines = (bottom - top) ? 612 : bottom-top;
                        fprintf(_outputFile, "%d %d translate %f %f scale -90 rotate\n",
                                left, top, xScale, -yScale);
                }
                else
                {
                        float xScale = (right - left) ? 612.0 / (right - left) : 1.0;
                        float yScale = (bottom - top) ? 792.0 / (bottom - top) : 1.0;
                        columns = (right - left) ? 612 : right-left;
                        lines = (bottom - top) ? 792 : bottom-top;
                        fprintf(_outputFile, "%d %d translate %f %f scale\n",
                                left, bottom, xScale, -yScale);
                }
        }
        else
                BeginPage();
}

void UI_PRINTER::EndPage(void)
{
        if (printerMode == PRM_POSTSCRIPT)
                fprintf(_outputFile, "showpage\n");
        else
                DevEscape(hdc, DEVESC_NEWFRAME, 0, 0, 0, 0);
}

void UI_PRINTER::ScreenDump(ZIL_SCREENID screenID, UI_REGION &region,
        ZIL_PRINTER_MODE printerMode, char *fileName)
{
        if (!display || isText)
                return;

        if (printerMode == PRM_POSTSCRIPT)
        {
                static ZIL_UINT8 _bmpColorTable[] =
                {
                        15, 14, 12, 10, 13, 9, 8, 7, 11, 4, 6, 3, 5, 2, 1, 0
                };

                POINTL testPoint = { 0, 0 };
                WinMapWindowPoints(screenID, HWND_DESKTOP, &testPoint, 1);
                if (testPoint.x > region.left)
                        screenID = WinQueryWindow(screenID, QW_PARENT);

                SWP swp;
                WinQueryWindowPos(screenID, &swp);
                int x = region.left - swp.x;
                int y =  lines - region.bottom - 1 - swp.y;
                int width = region.Width();
                int height = region.Height();


                HPS windowPS = WinGetPS(screenID);
                BeginPrintJob(printerMode, fileName);
                BeginPage();

                int tWidth = width % 2 ? width + 1 : width;
                fprintf(_outputFile, "%d %d %d %d Bitmap\n", x, y, tWidth, height);

                ZIL_COLOR color;
                GpiCreateLogColorTable(windowPS, 0, LCOLF_RGB, 0, 0, ZIL_NULLP(LONG));

                POINTL point;
                for (point.y = y + height - 1; point.y >= 0; point.y--)
                {
                        for (point.x = x; point.x < x + width; point.x++)
                                fprintf(_outputFile, "%x", _bmpColorTable[(int)GpiQueryColorIndex(hps, 0, GpiQueryPel(windowPS, &point))]);

                        if (width % 2)
                                fprintf(_outputFile, "f");
                        fprintf(_outputFile, "\n");
                }
                if (height % 2)
                {
                        for (int i = 0; i < tWidth; i++)
                                fprintf(_outputFile, "f");
                        fprintf(_outputFile, "\n");
                }
                EndPage();
                EndPrintJob();
        }
        else
                WinPostMsg(screenID, WM_CHAR, MPFROM2SHORT(KC_VIRTUALKEY, 1), MPFROM2SHORT(0, VK_PRINTSCRN));
}

void UI_PRINTER::TextFormat(ZIL_SCREENID, int x, int y, ZIL_ICHAR *text,
        const UI_PALETTE *palette, int width, ZIL_LOGICAL_FONT font)
{
        const int BUFFER_LENGTH = 256;
        int row = y, lineWidth = 0;
        int printLine = 0, printPage = 0;

        if (width < 0 || width >= columns)
                width = 70;

        ZIL_ICHAR lineBuffer[BUFFER_LENGTH];
        ZIL_ICHAR *ptr = text;

        while (ptr && *ptr)
        {
                if (*ptr == '\r' || *ptr == '\t')
                        ptr++;
                else if (*ptr == '\n')
                        ptr++, printLine = TRUE;
                else
                {
                        lineBuffer[lineWidth++] = *ptr++;
                        if (lineWidth == width)
                        {
                                // backup to a space character
                                int index = lineWidth;
                                ZIL_ICHAR *space;
                                for (space = ptr; *space != ' ' && *space != '-' &&
                                        index; space--, index--)
                                        ;
                                if (index)
                                {
                                        ptr = space;
                                        while (*ptr == ' ') ++ptr;
                                        if (*ptr == '-') ++index, ++ptr;
                                        lineWidth = index;
                                }
                                printLine = TRUE;
                        }
                }
                if (*ptr == '\0')
                        printLine = TRUE;
                if (printLine)
                {
                        lineBuffer[lineWidth] = '\0';
                        if (lineWidth)
                                Text(0, x, row, lineBuffer, palette, -1, FALSE, FALSE,
                                        ZIL_NULLP(UI_REGION), font);

                        if (printerMode == PRM_POSTSCRIPT)
                                row += TextHeight(lineBuffer) + TextHeight(lineBuffer) / 3;
                        else
                                row += TextHeight(lineBuffer) + 2;

                        lineWidth = printLine = 0;
                        if (row > lines - 108 && *ptr)
                                printPage = TRUE;
                }
                if (printPage)
                {
                        printPage = 0;
                        row = 36;
                        EndPage();
                        BeginPage();
                }
        }
}

