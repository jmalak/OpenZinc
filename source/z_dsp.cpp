//      Zinc Interface Library - Z_DSP.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_dsp.hpp"

// ----- UI_DISPLAY ---------------------------------------------------------

UI_DISPLAY::UI_DISPLAY(int _isText, const ZIL_ICHAR *_operatingSystem,
        const ZIL_ICHAR *_windowingSystem) : installed(FALSE), isText(_isText),
        isMono(FALSE), columns(0), lines(0), cellWidth(1), cellHeight(1),
        preSpace(2), postSpace(2), miniNumeratorX(1), miniDenominatorX(10),
        miniNumeratorY(1), miniDenominatorY(10)
{
#if !defined(__SC__)
        extern void z_gen_dummy(void);
        z_gen_dummy();
        extern void z_dsp_dummy(void);
        z_dsp_dummy();
#endif

        operatingSystem = strdup(_operatingSystem);
        windowingSystem = strdup(_windowingSystem);

        // Set up the default color maps.
        for (int i = 0; i < ZIL_MAXIMAGES; i++)
        {
                displayImage[i].image = displayImage[i].screen = displayImage[i].backup = ZIL_NULLP(ZIL_UINT8);
                displayImage[i].region.left = displayImage[i].region.right = -1;
                displayImage[i].region.top = displayImage[i].region.bottom = -1;
        }
        if (!ZIL_INTERNATIONAL::machineName[0])
                ZIL_INTERNATIONAL::MachineName();
}

UI_DISPLAY::~UI_DISPLAY(void)
{
        if (operatingSystem)
                delete operatingSystem;
        if (windowingSystem)
                delete windowingSystem;

        for (int i = 0; i < ZIL_MAXIMAGES; i++)
        {
                if (displayImage[i].screen)
                        delete displayImage[i].screen;
                if (displayImage[i].backup)
                        delete displayImage[i].backup;
        }
}

void UI_DISPLAY::Bitmap(ZIL_SCREENID, int, int, int, int, const ZIL_UINT8 *,
        const UI_PALETTE *, const UI_REGION *, ZIL_BITMAP_HANDLE *, ZIL_BITMAP_HANDLE *)
{
}

void UI_DISPLAY::BitmapArrayToHandle(ZIL_SCREENID, int, int, const ZIL_UINT8 *,
        const UI_PALETTE *, ZIL_BITMAP_HANDLE *, ZIL_BITMAP_HANDLE *)
{
}

void UI_DISPLAY::BitmapHandleToArray(ZIL_SCREENID, ZIL_BITMAP_HANDLE, ZIL_BITMAP_HANDLE, int *,
        int *, ZIL_UINT8 **)
{
}

void UI_DISPLAY::DestroyBitmapHandle(ZIL_SCREENID, ZIL_BITMAP_HANDLE *, ZIL_BITMAP_HANDLE *)
{
}

void UI_DISPLAY::Ellipse(ZIL_SCREENID, int, int, int, int, int, int,
        const UI_PALETTE *, int, int, const UI_REGION *)
{
}

void UI_DISPLAY::IconArrayToHandle(ZIL_SCREENID, int, int, const ZIL_UINT8 *,
        const UI_PALETTE *, ZIL_ICON_HANDLE *)
{
}

void UI_DISPLAY::IconHandleToArray(ZIL_SCREENID, ZIL_ICON_HANDLE, int *, int *, ZIL_UINT8 **)
{
}

void UI_DISPLAY::DestroyIconHandle(ZIL_SCREENID, ZIL_ICON_HANDLE *)
{
}

void UI_DISPLAY::Line(ZIL_SCREENID, int, int, int, int, const UI_PALETTE *,
        int, int, const UI_REGION *)
{
}

ZIL_COLOR UI_DISPLAY::MapColor(const UI_PALETTE *, int)
{
        return (0);
}

void UI_DISPLAY::Polygon(ZIL_SCREENID, int, const int *, const UI_PALETTE *,
        int, int, const UI_REGION *)
{
}

void UI_DISPLAY::Rectangle(ZIL_SCREENID, int, int, int, int, const UI_PALETTE *,
        int, int, int, const UI_REGION *)
{
}

void UI_DISPLAY::RectangleXORDiff(const UI_REGION &, const UI_REGION &,
        ZIL_SCREENID, const UI_REGION *)
{
}

void UI_DISPLAY::RegionDefine(ZIL_SCREENID, int, int, int, int)
{
}

void UI_DISPLAY::RegionMove(const UI_REGION &, int, int, ZIL_SCREENID, ZIL_SCREENID)
{
}

void UI_DISPLAY::Text(ZIL_SCREENID, int, int, const ZIL_ICHAR *, const UI_PALETTE *,
        int, int, int, const UI_REGION *, ZIL_LOGICAL_FONT)
{
}

int UI_DISPLAY::TextHeight(const ZIL_ICHAR *, ZIL_SCREENID, ZIL_LOGICAL_FONT)
{
        return (1);
}

int UI_DISPLAY::TextWidth(const ZIL_ICHAR *, ZIL_SCREENID, ZIL_LOGICAL_FONT)
{
        return (1);
}

int UI_DISPLAY::VirtualGet(ZIL_SCREENID, int, int, int, int)
{
        return (FALSE);
}

int UI_DISPLAY::VirtualPut(ZIL_SCREENID)
{
        return (FALSE);
}

int UI_DISPLAY::DeviceMove(ZIL_IMAGE_TYPE, int, int)
{
        return (TRUE);
}

int UI_DISPLAY::DeviceSet(ZIL_IMAGE_TYPE, int, int, int, int, ZIL_UINT8 *)
{
        return (TRUE);
}

int UI_DISPLAY::RegionInitialize(UI_REGION &region,
        const UI_REGION *clipRegion, int left, int top, int right, int bottom)
{
        region.left = MaxValue(0, left);
        region.top = MaxValue(0, top);
        region.right = MinValue(columns - 1, right);
        region.bottom = MinValue(lines - 1, bottom);
        if (clipRegion)
        {
                region.left = MaxValue(region.left, clipRegion->left);
                region.top = MaxValue(region.top, clipRegion->top);
                region.right = MinValue(region.right, clipRegion->right);
                region.bottom = MinValue(region.bottom, clipRegion->bottom);
        }
        return (region.left <= region.right && region.top <= region.bottom);
}
