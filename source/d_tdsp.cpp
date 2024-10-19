//      Zinc Interface Library - D_TDSP.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <stdlib.h>
#include "ui_dsp.hpp"
#if defined(ZIL_MSDOS)
#       include <dos.h>
#       if defined(_MSC_VER)
#       endif
#endif

static ZIL_ICHAR _operatingSystem[] = { 'D','O','S',0 };
static ZIL_ICHAR _windowingSystem[] = { 'T','E','X','T',0 };
#if defined(ZIL_CURSES)
static ZIL_ICHAR _codeSet[] = { 'A','S','C','I','I',0 };
#else
static ZIL_ICHAR _codeSet[] = { 'I','B','M','_',0 };
#endif

#if defined(ZIL_UNICODE) || defined(ZIL_CURSES)
#       define FONTOUT(x)       (ZIL_UINT8)(x)
#else
#       define FONTOUT(x)       ((ZIL_UINT8)MapChar(x)[0])
#endif

// Macros that differentiate between DOS and other systems.
#if defined(ZIL_MSDOS)
#       define BuildCell(attribute,character) (((attribute) << 8) | (ZIL_UINT8)character)
#       define CellAttribute(cell) ((cell) & 0xFF00)
#       define CellCharacter(cell) ((cell) & 0x00FF)
#       define CellBackground(cell) ((cell) & 0xF000)
#       define CellXOR(cell) ((cell & 0x00FF) | (~(cell & 0xFF00) & 0xFF00))
#elif defined(ZIL_CURSES)
#       define BuildCell(attribute,character) ((attribute) | (character))
#       define CellAttribute(cell) ((cell) & A_ATTRIBUTES)
#       define CellCharacter(cell) ((cell) & A_CHARTEXT)
#       define CellBackground(cell) ((cell) & A_REVERSE)
#       if defined(SCO_UNIX)
#               define CellXOR(cell) ((cell) & A_COLOR ? \
                        COLOR_PAIR(63-PAIR_NUMBER(cell)) | ((cell) & A_CHARTEXT) :\
                        (cell) & A_REVERSE ? \
                        (cell) & A_CHARTEXT : ((cell) & A_CHARTEXT) | A_REVERSE)
#       else
#               define CellXOR(cell) ((cell) & A_REVERSE ? (cell) & A_CHARTEXT : ((cell) & A_CHARTEXT) | A_REVERSE)
#       endif
#endif

// ----- Static member variables --------------------------------------------

extern ZIL_ICHAR _tLowerShadow, _tUpperShadow, _tFullShadow;
#if defined(ZIL_UNICODE)
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUL[]         = { 0x250c, 0x2554 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUR[]         = { 0x2510, 0x2557 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLL[]         = { 0x2514, 0x255a };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLR[]         = { 0x2518, 0x255d };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tHorizontal[]       = { 0x2500, 0x2550 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tVertical[]         = { 0x2502, 0x2551 };

        static ZIL_ICHAR _tLowerShadowMapped;
        static ZIL_ICHAR _tUpperShadowMapped;
        static ZIL_ICHAR _tFullShadowMapped;
        static int shadowMapped = FALSE;
        static int lineMapped = FALSE;
#elif defined(ZIL_CURSES)
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUL[]         = { 0xDA, 0xC9 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUR[]         = { 0xBF, 0xBB };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLL[]         = { 0xC0, 0xC8 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLR[]         = { 0xD9, 0xBC };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tHorizontal[]       = { 0xC4, 0xCD };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tVertical[]         = { 0xB3, 0xBA };

#       define _tLowerShadowMapped _tLowerShadow
#       define _tUpperShadowMapped _tUpperShadow
#       define _tFullShadowMapped _tFullShadow
#elif defined(ZIL_ISO8859_1)
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUL[]         = { 0x8d, 0x9d };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUR[]         = { 0x85, 0x95 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLL[]         = { 0x86, 0x96 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLR[]         = { 0x8c, 0x9c };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tHorizontal[]       = { 0x8a, 0x9a };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tVertical[]         = { 0x83, 0x93 };

#       define _tLowerShadowMapped _tLowerShadow
#       define _tUpperShadowMapped _tUpperShadow
#       define _tFullShadowMapped _tFullShadow
#else // OEM
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUL[]         = { 0xDA, 0xC9 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerUR[]         = { 0xBF, 0xBB };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLL[]         = { 0xC0, 0xC8 };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tCornerLR[]         = { 0xD9, 0xBC };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tHorizontal[]       = { 0xC4, 0xCD };
        ZIL_ICHAR UI_TEXT_DISPLAY::_tVertical[]         = { 0xB3, 0xBA };

#       define _tLowerShadowMapped _tLowerShadow
#       define _tUpperShadowMapped _tUpperShadow
#       define _tFullShadowMapped _tFullShadow
#endif

#if defined(ZIL_HARDWARE)
        ZIL_ICHAR *_textDriverName;
        int *(*I_CheckLeadIn)(void);
        void (*I_ScreenOpen)(int *, int *, int *);
        void (*I_ScreenClose)(void);
        void (*I_ScreenPut)(int, int, int, int, void *);
        void (*I_CursorRemove)(void);
        void (*I_CursorPosition)(int, int, int);
        int (*I_GetCodePage)(void);
        void (*I_Beep)(void);

        void (*I_KeyboardOpen)(void);
        void (*I_KeyboardClose)(void);
        void (*I_KeyboardRead)(unsigned *, unsigned *, unsigned *);
        int (*I_KeyboardQuery)(unsigned *);

        int (*I_MouseOpen)(void);
        void (*I_MouseClose)(void);

        void (*I_PrintDotMatrixChar)(char c);
#endif

static ZIL_UINT16 borderFlags;

// ----- UI_TEXT_DISPLAY ----------------------------------------------------

UI_TEXT_DISPLAY::UI_TEXT_DISPLAY(TDM_MODE _mode) :
        UI_DISPLAY(TRUE, _operatingSystem, _windowingSystem)
{
        int cp = I_GetCodePage();
        strcpy(codeSet, _codeSet);
        itoa(cp, &codeSet[strlen(codeSet)], 10, 0);

        _virtualCount = 0;
#if defined(ZIL_CURSES)
        _stopDevice = TRUE;     // Don't show mouse till they use it.
#else
        _stopDevice = FALSE;
#endif

        mode = _mode;
        I_ScreenOpen(&mode, &lines, &columns);
        isMono = (mode == 0 || mode == 2 || mode == 5 || mode == 6 || mode == 7);

        cellWidth = cellHeight = 1;     
        preSpace = postSpace = 0;

        // Fill the screen according to the specified palette.
        ZIL_SCREEN_CELL color = MapColor(backgroundPalette);
        color = BuildCell(color, MapChar(backgroundPalette->fillCharacter)[0]);
        int screenSize = columns * lines;
        _screen = new ZIL_SCREEN_CELL[screenSize];
        int i;
        for (i = 0; i < screenSize; i++)
                _screen[i] = color;
        int offset = 0;
#if defined(ZIL_CURSES)
        for (i = lines - 1; i >= 0; i--)  // Paint bottom up -- fix Procomm dropout.
#else
        for (i = 0; i < lines; i++)
#endif
        {
                I_ScreenPut(0, i, columns-1, i, &_screen[offset]);
                offset += columns;
        }
        // Define the screen display region.
        Add(ZIL_NULLP(UI_ELEMENT), new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
        installed = TRUE;
#if defined(ZIL_UNICODE)
        // Leave room for input method for codepages 932, 942, 949
        // the FEP fixes 938, 950, and 1381
        if (cp == 932 || cp == 942 || cp == 949)
                lines--;
#endif
}

UI_TEXT_DISPLAY::~UI_TEXT_DISPLAY(void)
{
        // Restore the display.
        if (installed)
        {
                delete _screen;
                I_ScreenClose();
        }
}

int UI_TEXT_DISPLAY::DeviceMove(ZIL_IMAGE_TYPE imageType, int newColumn, int newLine)
{
        if (imageType != IMT_MOUSE_IMAGE || _stopDevice)
                return (FALSE);
        _stopDevice = TRUE;

        UI_DISPLAY_IMAGE *view = &displayImage[IMT_MOUSE_IMAGE];
        int column = view->region.left;
        int line = view->region.top;
        view->region.left = view->region.right = newColumn;
        view->region.top = view->region.bottom = newLine;

        if (column >= 0 && column < columns && line >= 0 && line < lines)
        {
#if defined(ZIL_UNICODE)
                ZIL_SCREEN_CELL *screen = &_screen[line * columns];
                int i = 0, j = 0;
                for (;;) {
                        char tmp[2];
                        tmp[0] = CellCharacter(*screen);
                        tmp[1] = CellCharacter(screen[1]);
                        j = mblen(tmp);
                if (i+j > column) break;
                        screen += j;
                        i += j;
                }
                I_ScreenPut(i, line, i+j-1, line, screen);
#else
                ZIL_SCREEN_CELL *screen = &_screen[line * columns + column];
                I_ScreenPut(column, line, column, line, screen);
#endif
        }
        if (newColumn >= 0 && newColumn < columns && newLine >= 0 && newLine < lines)
        {
#if defined(ZIL_UNICODE)
                ZIL_SCREEN_CELL *screen = &_screen[newLine * columns];
                int i = 0, j = 0;
                for (;;) {
                        char tmp[2];
                        tmp[0] = CellCharacter(*screen);
                        tmp[1] = CellCharacter(screen[1]);
                        j = mblen(tmp);
                if (i+j > newColumn) break;
                        screen += j;
                        i += j;
                }
                ZIL_SCREEN_CELL screenX[2];
                screenX[0] = CellXOR(screen[0]);
                screenX[1] = CellXOR(screen[1]);
                I_ScreenPut(i, newLine, i+j-1, newLine, screenX);
#else
                ZIL_SCREEN_CELL screen = CellXOR(_screen[newLine * columns + newColumn]);
                I_ScreenPut(newColumn, newLine, newColumn, newLine, &screen);
#endif
        }

        _stopDevice = FALSE;
        return (TRUE);
}

int UI_TEXT_DISPLAY::DeviceSet(ZIL_IMAGE_TYPE imageType, int column, int line,
        int, int, ZIL_UINT8 *)
{
        DeviceMove(imageType, column, line);
        return (TRUE);
}

void UI_TEXT_DISPLAY::Line(ZIL_SCREENID screenID, int column1, int line1,
        int column2, int line2, const UI_PALETTE *palette, int width, int xorInt,
        const UI_REGION *clipRegion)
{
        // Ensure a left-top to right-bottom line.
        if (column2 < column1)
        {
                int column = column1;
                column1 = column2;
                column2 = column;
        }
        if (line2 < line1)
        {
                int line = line1;
                line1 = line2;
                line2 = line;
        }

        // Set up the line region.
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, column1, line1, column2, line2))
                return;

        // Draw the line on the display.
        int changedScreen = FALSE;
        ZIL_SCREEN_CELL color = MapColor(palette);
#if defined(ZIL_UNICODE)
        if (!lineMapped)
        {
                _tHorizontal[0] = MapChar(_tHorizontal[0])[0];
                _tHorizontal[1] = MapChar(_tHorizontal[1])[0];
                _tVertical[0] = MapChar(_tVertical[0])[0];
                _tVertical[1] = MapChar(_tVertical[1])[0];
                lineMapped = TRUE;
        }
#endif
        if (line1 == line2 && width >= 1)
                color = BuildCell(color, FONTOUT(_tHorizontal[width - 1]));
        else if (column1 == column2 && width >= 1)
                color = BuildCell(color, FONTOUT(_tVertical[width - 1]));
        else
                color = BuildCell(color, MapChar(palette->fillCharacter)[0]);
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                        for (int row = tRegion.top; row <= tRegion.bottom; row++)
                        {
                                int offset = columns * row + tRegion.left;
                                for (int column = tRegion.left; column <= tRegion.right; column++, offset++)
                                        _screen[offset] = xorInt ? CellXOR(_screen[offset]) : color;
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }

        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

ZIL_COLOR UI_TEXT_DISPLAY::MapColor(const UI_PALETTE *palette, int)
{
        // Match the color request based on the type of display.
        return (isMono ? palette->monoAttribute : palette->colorAttribute);
}

void UI_TEXT_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top, int right,
        int bottom, const UI_PALETTE *palette, int width, int fill, int xorInt,
        const UI_REGION *clipRegion)
{
        // Assign the rectangle to the region structure.
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
                return;

        // Check for the text shadow ``feature''.
        if (!palette)
        {
                VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                UI_REGION clip;
                if (clipRegion)                         // Un-optimized for Zortech bug.
                        clip = *clipRegion;
                else
                        clip = region;
                int offset = columns * region.top + region.right;
#if defined(ZIL_UNICODE)
                if (!shadowMapped)
                {
                        _tLowerShadowMapped = MapChar(_tLowerShadow)[0];
                        _tUpperShadowMapped = MapChar(_tUpperShadow)[0];
                        _tFullShadowMapped = MapChar(_tFullShadow)[0];
                        shadowMapped = TRUE;
                }
#endif
                if (top == region.top && right == region.right)
                {
                        int fullShadow = FONTOUT(_tFullShadowMapped);
                        int upperShadow = FONTOUT(_tUpperShadowMapped);

                        // Check for overlapping another shadow border character.
                        if ((_screen[offset] & 0xFF) == fullShadow || (_screen[offset] & 0xFF) == upperShadow)
                                _screen[offset] = CellBackground(_screen[offset]) | FONTOUT(_tFullShadowMapped);
                        else
                                _screen[offset] = CellBackground(_screen[offset]) | FONTOUT(_tLowerShadowMapped);
                        region.top++;
                        offset += columns;
                }
                if (right == region.right)
                {
                        for (int line = region.top; line < region.bottom; line++, offset += columns)
                                if (line >= clip.top && line <= clip.bottom)
                                        _screen[offset] = CellBackground(_screen[offset]) | FONTOUT(_tFullShadowMapped);
                        if (bottom != region.bottom && region.bottom >= clip.top && region.bottom <= clip.bottom)
                                _screen[offset] = CellBackground(_screen[offset]) | FONTOUT(_tFullShadowMapped);
                }
                if (left == region.left)
                        region.left++;
                offset = columns * region.bottom + region.left;
                if (bottom == region.bottom)
                {
                        int fullShadow = FONTOUT(_tFullShadowMapped);
                        for (int column = region.left; column <= region.right; column++, offset++)
                        if (column >= clip.left && column <= clip.right)
                        {
                                // Check for overlapping another shadow border character.
                                if ((_screen[offset] & 0xFF) == fullShadow)
                                        _screen[offset] = CellBackground(_screen[offset]) | FONTOUT(_tFullShadowMapped);
                                else
                                        _screen[offset] = CellBackground(_screen[offset]) | FONTOUT(_tUpperShadowMapped);
                        }
                }
                VirtualPut(screenID);
                return;
        }

        // Check for a no fill-pattern.
        if (!fill && top != bottom)
        {
                VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                if (left < right - 1)
                {
                        // Optimization of these lines doesn't work because
                        // Line() can't tell which direction the line is going.
                        Line(screenID, left, top, right, top, palette, width, xorInt, clipRegion);
                        Line(screenID, left, bottom, right, bottom, palette, width, xorInt, clipRegion);
                }
                if (top < bottom)
                {
                        // Optimization of these lines doesn't work because
                        // Line() can't tell which direction the line is going.
                        Line(screenID, left, top, left, bottom, palette, width, xorInt, clipRegion);
                        Line(screenID, right, top, right, bottom, palette, width, xorInt, clipRegion);
                }
                // Tell Text() to xor the border.
                borderFlags = TRUE;
                Text(screenID, left, top, &_tCornerUL[width - 1], palette, 1, TRUE, xorInt, clipRegion);
                Text(screenID, right, top, &_tCornerUR[width - 1], palette, 1, TRUE, xorInt, clipRegion);
                Text(screenID, left, bottom, &_tCornerLL[width - 1], palette, 1, TRUE, xorInt, clipRegion);
                Text(screenID, right, bottom, &_tCornerLR[width - 1], palette, 1, TRUE, xorInt, clipRegion);
                borderFlags = FALSE;

                VirtualPut(screenID);
                return;
        }

        // Draw the rectangle on the display.
        int changedScreen = FALSE;
        ZIL_SCREEN_CELL color = MapColor(palette);
        color = BuildCell(color, MapChar(palette->fillCharacter)[0]);
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                        for (int row = tRegion.top; row <= tRegion.bottom; row++)
                        {
                                int offset = columns * row + tRegion.left;
                                for (int column = tRegion.left; column <= tRegion.right; column++, offset++)
                                        _screen[offset] = xorInt ? CellXOR(_screen[offset]) : color;
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }

        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

void UI_TEXT_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
        const UI_REGION &newRegion, ZIL_SCREENID, const UI_REGION *clipRegion)
{
        // Set up the hide region.
        if (oldRegion.left == newRegion.left && oldRegion.top == newRegion.top &&
                oldRegion.right == newRegion.right && oldRegion.bottom == newRegion.bottom)
                return;
        UI_REGION region;

        if (clipRegion)
        {
                region.left = MaxValue(MinValue(oldRegion.left, newRegion.left), MaxValue(clipRegion->left, 0));
                region.top = MaxValue(MinValue(oldRegion.top, newRegion.top), MaxValue(clipRegion->top, 0));
                region.right = MinValue(MaxValue(oldRegion.right, newRegion.right), MinValue(clipRegion->right, columns - 1));
                region.bottom = MinValue(MaxValue(oldRegion.bottom, newRegion.bottom), MinValue(clipRegion->bottom, lines - 1));
        }
        else
        {
                region.left = MaxValue(MinValue(oldRegion.left, newRegion.left), 0);
                region.top = MaxValue(MinValue(oldRegion.top, newRegion.top), 0);
                region.right = MinValue(MaxValue(oldRegion.right, newRegion.right), columns - 1);
                region.bottom = MinValue(MaxValue(oldRegion.bottom, newRegion.bottom), lines - 1);
        }
        // XOR the screen region.
        int row, column, offset;
        VirtualGet(ID_SCREEN, region.left, region.top, region.right, region.bottom);
        for (row = MaxValue(oldRegion.top, region.top);
                row <= MinValue(oldRegion.bottom, region.bottom); row++)
        {
                offset = columns * row + MaxValue(oldRegion.left, region.left);
                for (column = MaxValue(oldRegion.left, region.left);
                        column <= MinValue(oldRegion.right, region.right); column++, offset++)
                        if (row == oldRegion.top || row == oldRegion.bottom ||
                                column == oldRegion.left || column == oldRegion.right)
                                _screen[offset] = CellXOR(_screen[offset]);
        }
        for (row = MaxValue(newRegion.top, region.top);
                row <= MinValue(newRegion.bottom, region.bottom); row++)
        {
                offset = columns * row + MaxValue(newRegion.left, region.left);
                for (column = MaxValue(newRegion.left, region.left);
                        column <= MinValue(newRegion.right, region.right); column++, offset++)
                        if (row == newRegion.top || row == newRegion.bottom ||
                                column == newRegion.left || column == newRegion.right)
                        _screen[offset] = CellXOR(_screen[offset]);
        }

        // Update the screen.
        VirtualPut(ID_SCREEN);
}

void UI_TEXT_DISPLAY::RegionDefine(ZIL_SCREENID screenID, int left, int top,
        int right, int bottom)
{
        // See if it is a full screen definition.
        UI_REGION region;
        region.left = left;
        region.top = top;
        region.right = right;
        region.bottom = bottom;
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

void UI_TEXT_DISPLAY::RegionMove(const UI_REGION &oldRegion,
        int newColumn, int newLine, ZIL_SCREENID oldScreenID, ZIL_SCREENID)
{
        UI_REGION region;
        if (!RegionInitialize(region, ZIL_NULLP(UI_REGION), MinValue(oldRegion.left, newColumn),
                MinValue(oldRegion.top, newLine),
                MaxValue(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
                MaxValue(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top))))
                return;
        VirtualGet(oldScreenID, region.left, region.top, region.right, region.bottom);
        int row;
        for (row = region.top; row <= region.bottom; row++)
        {
                int offset = row * columns + region.left;
                I_ScreenPut(region.left, row, region.right, row, &_screen[offset]);
        }
        if (oldRegion.right >= oldRegion.left &&
            oldRegion.bottom >= oldRegion.top)
        {
                int partRow = oldRegion.right-oldRegion.left+1;
                int col;

                ZIL_SCREEN_CELL *moveBuffer = new ZIL_SCREEN_CELL[(oldRegion.right-oldRegion.left+1)*(oldRegion.bottom-oldRegion.top+1)];
                for (row = oldRegion.left; row <= oldRegion.right; row++)
                        for (col = oldRegion.top; row <= oldRegion.bottom; col++)
                                moveBuffer[(row-oldRegion.left) * partRow + (col-oldRegion.left)] = _screen[row * columns + col];
                for (row = newLine; row <= newLine + oldRegion.bottom - oldRegion.top; row++)
                        for (col = newColumn; row <= newColumn + oldRegion.right - oldRegion.left; col++)
                                _screen[row * columns + col] =  moveBuffer[(row-newLine) * partRow + (col-newColumn)];
                I_ScreenPut(newColumn, newLine,
                            newColumn + oldRegion.right - oldRegion.left,
                            newLine + oldRegion.bottom - oldRegion.top,
                            moveBuffer);
                delete moveBuffer;
        }
        VirtualPut(oldScreenID);
}

void UI_TEXT_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
        const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
        int xorInt, const UI_REGION *clipRegion, ZIL_LOGICAL_FONT logicalFont)
{
        // Make sure we have a valid string.
        if (!text || !text[0])
                return;

        // Set up the fill line.
        static ZIL_ICHAR ZIL_FARDATA fillLine[256];
        if (length < 0)
                length = strlen(text);
        else if (length > 255)
                length = 255;
        strncpy(fillLine, text, length);
        fillLine[length] = '\0';

        if (!FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE))
                StripHotMark(fillLine);

        // Check for special characters.
        strrepc(fillLine, '\t', ' ');
        strrepc(fillLine, '\r', ' ');
        strrepc(fillLine, '\n', ' ');

        // Convert the cell coordinates.
        UI_REGION region, tRegion;
        if (!RegionInitialize(region, clipRegion, left, top,
                left + TextWidth(fillLine, screenID, logicalFont | FNT_IGNORE_UNDERSCORE) - 1,
                top + TextHeight(fillLine, screenID, logicalFont) - 1))
                return;

        // Draw the rectangle on the display.
        int changedScreen = FALSE;
        ZIL_SCREEN_CELL color = MapColor(palette);
        color = BuildCell(color, 0);
#if defined(ZIL_UNICODE)
        char *TEXT_ = MapText(fillLine, ZIL_NULLP(char), FALSE);
#else
#       define TEXT_ fillLine
#
#endif
        for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
                if (screenID == ID_DIRECT ||
                        (screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
                {
                        if (screenID == ID_DIRECT)
                                tRegion = region;
                        if (!changedScreen)
                                changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
                        int sOffset = columns * tRegion.top + tRegion.left;
                        int tOffset = tRegion.left - left;

                        if (xorInt && !borderFlags)
                        {
                                for (int column = tRegion.left; column <= tRegion.right; column++, sOffset++, tOffset++)
                                        _screen[sOffset] = CellBackground(_screen[sOffset]) |
                                                (CellXOR(_screen[sOffset]) & 0x0F00) | FONTOUT(TEXT_[tOffset]);
                        }
                        else
                        {
                                for (int column = tRegion.left; column <= tRegion.right; column++, sOffset++, tOffset++)
                                        _screen[sOffset] = xorInt ? CellXOR(_screen[sOffset]) :
                                                (fill ? color : (CellBackground(_screen[sOffset]) | (color))) | FONTOUT(TEXT_[tOffset]);
                        }
                        if (screenID == ID_DIRECT)
                                break;
                }
        // Update the screen.
        if (changedScreen)
                VirtualPut(screenID);
}

int UI_TEXT_DISPLAY::TextHeight(const ZIL_ICHAR *, ZIL_SCREENID, ZIL_LOGICAL_FONT)
{
        return (1);
}

int UI_TEXT_DISPLAY::TextWidth(const ZIL_ICHAR *string, ZIL_SCREENID, ZIL_LOGICAL_FONT logicalFont)
{
        if (!string || !string[0])
                return (0);
#if defined(ZIL_UNICODE)
        char *TEXT_ = MapText(string, ZIL_NULLP(char), FALSE);
#else
        const char *TEXT_ = string;
#endif
        int length = ::strlen(TEXT_);
        int textLength = length;

        for (int i=0; i < length; i++)
                if (!FlagSet(logicalFont, FNT_IGNORE_UNDERSCORE) && (string[i] == ZIL_HOTMARK || string[i] == '~'))
                {
                        textLength--;
                        if ((string[i] == ZIL_HOTMARK && string[i+1] == ZIL_HOTMARK) ||
                                (string[i] == '~' && string[i+1] == '~'))
                                i++;
                }
        return (textLength);
}

int UI_TEXT_DISPLAY::VirtualGet(ZIL_SCREENID, int left, int top, int right, int bottom)
{
#ifndef ZINC_DEBUG
        if (--_virtualCount == -1)
        {
#endif
                _stopDevice = TRUE;
                _virtualRegion.left = MaxValue(0, left);
                _virtualRegion.top = MaxValue(0, top);
                _virtualRegion.right = MinValue(columns - 1, right);
                _virtualRegion.bottom = MinValue(lines - 1, bottom);
                if (_virtualRegion.Overlap(displayImage[IMT_MOUSE_IMAGE].region))
                {
                        UI_DISPLAY_IMAGE *view = &displayImage[IMT_MOUSE_IMAGE];
                        int column = view->region.left;
                        int line = view->region.top;
                        I_ScreenPut(column, line, column, line, &_screen[line * columns + column]);
                }
#ifndef ZINC_DEBUG
        }
        else
        {
                _virtualRegion.left = MaxValue(0, MinValue(_virtualRegion.left, left));
                _virtualRegion.top = MaxValue(0, MinValue(_virtualRegion.top, top));
                _virtualRegion.right = MinValue(columns - 1, MaxValue(_virtualRegion.right, right));
                _virtualRegion.bottom = MinValue(lines - 1, MaxValue(_virtualRegion.bottom, bottom));
        }
#endif
        return (TRUE);
}

int UI_TEXT_DISPLAY::VirtualPut(ZIL_SCREENID)
{
#ifndef ZINC_DEBUG
        if (++_virtualCount == 0)
        {
#endif
                for (int row = _virtualRegion.top; row <= _virtualRegion.bottom; row++)
                {
                        int offset = row * columns + _virtualRegion.left;
                        if (_virtualRegion.left <= _virtualRegion.right)
                                I_ScreenPut(_virtualRegion.left, row,
                                        _virtualRegion.right, row, &_screen[offset]);
                }
                UI_DISPLAY_IMAGE *view = &displayImage[IMT_MOUSE_IMAGE];
                _stopDevice = FALSE;
                if (_virtualRegion.Overlap(view->region))
                        DeviceMove(IMT_MOUSE_IMAGE, view->region.left, view->region.top);
#ifndef ZINC_DEBUG
        }
        else if (_virtualCount > 0)
                _virtualCount = 0;
#endif
        return (TRUE);
}
