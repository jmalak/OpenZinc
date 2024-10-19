//      Zinc Interface Library - W_IMAGE.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#include <windowsx.h>

#if defined(__WATCOMC__)
    #undef  GlobalFreePtr
    #define     GlobalFreePtr(lp) (GlobalUnlockPtr(lp), (BOOL)GlobalFree(GlobalPtrHandle(lp)))
#endif

// --------------------------------------------------------------------------
// ----- UIW_IMAGE ----------------------------------------------------------
// --------------------------------------------------------------------------

void UIW_IMAGE::DestroyImageHandle(void)
{
        // Destroy the image handle.
        if (image)
                DeleteObject(image);
        image = 0;

        // Destroy the palette handle.
        if (hPalette)
                DeleteObject(hPalette);
        hPalette = 0;
}

EVENT_TYPE UIW_IMAGE::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
        // Virtualize the display.
        UI_REGION region = trueRegion;
        display->VirtualGet(screenID, region);

        // Draw the border if necessary.
        if (IsBordered())
                DrawBorder(screenID, region, FALSE, ccode);
        if (!image)
        {
                display->VirtualPut(screenID);
                return (S_ERROR);
        }

        // Draw the image.
        int left = region.left - trueRegion.left;
        int top = region.top - trueRegion.top;
        int width = region.Width();
        int height = region.Height();

        HDC hDC = UI_MSWINDOWS_DISPLAY::hDC;
        HDC hMemDC = CreateCompatibleDC(hDC);
        SelectPalette(hDC, hPalette, FALSE);
        RealizePalette(hDC);
        SelectPalette(hMemDC, hPalette, FALSE);
        RealizePalette(hMemDC);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, image);

        if (IsScaled())
                StretchBlt(hDC, left, top, width, height, hMemDC, 0, 0, imageWidth, imageHeight, SRCCOPY);
        else if (IsTiled())
        {
                for (int y = top; y < height; y += imageHeight)
                        for (int x = left; x < width; x += imageWidth)
                        {
                                int sizeX = (x + imageWidth) < width ? imageWidth : width - x + 1;
                                int sizeY = (y + imageHeight) < height ? imageHeight : height - y + 1;
                                BitBlt(hDC, x, y, sizeX, sizeY, hMemDC, 0, 0, SRCCOPY);
                        }
        }
        else
                BitBlt(hDC, left, top, width, height, hMemDC, 0, 0, SRCCOPY);

        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);

        // Un-virtualize the display.
        display->VirtualPut(screenID);
        return (ccode);
}

int UIW_IMAGE::LoadImageFromApplication(void)
{
        woStatus |= WOS_READ_ERROR;
#if defined(ZIL_UNICODE)
                char *mappedStr = MapText(pathName, ZIL_NULLP(char), FALSE);
#               define TEXT_ mappedStr
#else
#               define TEXT_ pathName
#endif

        // Try to load the bitmap from the system.
    HRSRC hRsrc = FindResource(display->hInstance, TEXT_, RT_BITMAP);
        if (hRsrc)
        {
                HGLOBAL hGlobal = LoadResource(display->hInstance, hRsrc);
                BITMAPINFO *bitmapInfo = (BITMAPINFO *)LockResource(hGlobal);

                int colors = 0;
                if (bitmapInfo->bmiHeader.biBitCount <= 8)
                        colors = (1 << bitmapInfo->bmiHeader.biBitCount);

                if (colors)
                {
                        HANDLE hLogPalette = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
                                sizeof (PALETTEENTRY) * (colors));
                        LOGPALETTE *logPalette = (LOGPALETTE *) GlobalLock (hLogPalette);
                        logPalette->palVersion = 0x300;
                        logPalette->palNumEntries = colors;
                        for (int i = 0;  i < colors;  i++)
                        {
                                logPalette->palPalEntry[i].peRed = bitmapInfo->bmiColors[i].rgbRed;
                                logPalette->palPalEntry[i].peGreen = bitmapInfo->bmiColors[i].rgbGreen;
                                logPalette->palPalEntry[i].peBlue  = bitmapInfo->bmiColors[i].rgbBlue;
                                logPalette->palPalEntry[i].peFlags = 0;
                        }
                        hPalette = CreatePalette(logPalette);
                        GlobalUnlock (hLogPalette);
                        GlobalFree   (hLogPalette);
                }

                HDC hdc = GetDC(NULL);
                if (hPalette)
                {
                        SelectPalette(hdc, hPalette, FALSE);
                        RealizePalette(hdc);
                }
                image = CreateDIBitmap(hdc, &bitmapInfo->bmiHeader,
                        CBM_INIT, (BYTE *)bitmapInfo + bitmapInfo->bmiHeader.biSize +
                        (colors * sizeof(RGBQUAD)),     bitmapInfo, DIB_RGB_COLORS );
                ReleaseDC(NULL,hdc);

                UnlockResource(hGlobal);
                FreeResource(hGlobal);
        }

        if (image)
        {
                BITMAP info;
                GetObject(image, sizeof(BITMAP), (LPSTR)&info);
                imageHeight = info.bmHeight;
                imageWidth = info.bmWidth;
                woStatus &= ~WOS_READ_ERROR;
        }
        return (image ? TRUE : FALSE);
}

int UIW_IMAGE::LoadImageFromFile(void)
{
        woStatus |= WOS_READ_ERROR;

        // Open the file.
        ZIL_ICHAR _pathName[ZIL_MAXPATHLEN];
        strcpy(_pathName, pathName);
        strcat(_pathName, ZIL_TEXT(".bmp"));
        ZIL_FILE file(_pathName);

        // Load the file header.
        BITMAPFILEHEADER bitmapFileHeader;
        file.Read(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1);
        if (bitmapFileHeader.bfType != 0x4D42) // 'BM' format.
                return (FALSE);

        // Load the info header.
        BITMAPINFO *bitmapInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256];
        file.Read(&bitmapInfo->bmiHeader, sizeof(BITMAPINFOHEADER), 1);
        imageWidth = (int)bitmapInfo->bmiHeader.biWidth;
        imageHeight = (int)bitmapInfo->bmiHeader.biHeight;

        // Load the color mapping.
        int colors = 0;
        if (bitmapInfo->bmiHeader.biBitCount <= 8)
                colors = (1 << bitmapInfo->bmiHeader.biBitCount);

        if (colors)
        {
                file.Read(bitmapInfo->bmiColors, sizeof(RGBQUAD), colors);
                HANDLE hLogPalette = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
                        sizeof (PALETTEENTRY) * (colors));
                LOGPALETTE *logPalette = (LOGPALETTE *) GlobalLock (hLogPalette);
                logPalette->palVersion = 0x300;
                logPalette->palNumEntries = colors;
                for (int i = 0;  i < colors;  i++)
                {
                        logPalette->palPalEntry[i].peRed = bitmapInfo->bmiColors[i].rgbRed;
                        logPalette->palPalEntry[i].peGreen = bitmapInfo->bmiColors[i].rgbGreen;
                        logPalette->palPalEntry[i].peBlue  = bitmapInfo->bmiColors[i].rgbBlue;
                        logPalette->palPalEntry[i].peFlags = 0;
                }
                hPalette = CreatePalette(logPalette);
                GlobalUnlock (hLogPalette);
                GlobalFree   (hLogPalette);
        }

        // Load the bitmap.
        ZIL_UINT32 dibSize = bitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER);
        ZIL_UINT32 tdibSize = dibSize;
        ZIL_UINT8 huge *dib = (ZIL_UINT8 huge *)GlobalAllocPtr(GMEM_MOVEABLE, dibSize);
        ZIL_UINT8 huge *tdib = dib;
        for ( ; dibSize; dibSize -= tdibSize, tdib += tdibSize)
        {
                tdibSize = (30000 < dibSize) ? 30000 : dibSize;
                file.Read(tdib, sizeof(ZIL_UINT8), (int)tdibSize);
        }

        // Create the image.
        HDC hDC = GetDC(NULL);
        if (hPalette)
        {
                SelectPalette(hDC, hPalette, FALSE);
                RealizePalette(hDC);
        }
        image = CreateDIBitmap(hDC, &bitmapInfo->bmiHeader, CBM_INIT, dib, (LPBITMAPINFO)bitmapInfo, DIB_RGB_COLORS);
        ReleaseDC(NULL, hDC);

        // Clean up
#if defined(__WATCOMC__)
        GlobalFreePtr((unsigned)dib);
#else   
         GlobalFreePtr(dib);
#endif
        delete bitmapInfo;
        woStatus &= ~WOS_READ_ERROR;
        return (image ? TRUE : FALSE);
}

