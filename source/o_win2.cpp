//      Zinc Interface Library - O_WIN2.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_GPI
#define INCL_INPUT                              // OS/2 pointer functions and flags.
#define INCL_WINPOINTERS                // OS/2 pointer functions and flags.
#define INCL_WINBUTTONS                 // OS/2 button messages and flags.
#define INCL_WINMENUS                   // OS/2 OWNERITEM definition.
#define INCL_WINLISTBOXES               // OS/2 list messages and flags.
#define INCL_WINSTDDRAG                 // OS/2 drag messages and flags.
#define USE_RAW_KEYS                    // Zinc raw keys.
#include <stdio.h>
#include "ui_win.hpp"

extern ZIL_ICHAR *_genericFieldID;

#if !defined(NO_ZIL_APP) && defined(__ZTC__)
// Bug fix for linkers that don't look for main in the .LIBs.
extern "C"
{
extern int main(int argc, char **argv);
static int (*linkAddr)(int, char **) = main;
}
#endif

#if defined(ZIL_EDIT)
static ZIL_UINT8 _placePointerData[] =
{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
        0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
        0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
        0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static ZIL_UINT8 _movePointerData[] =
{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0xFF, 0xFF,
        0xFF, 0x0F, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x0F, 0xFF,
        0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
        0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
        0xFF, 0x0F, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x0F, 0xFF,
        0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
#endif


// ----- UI_WINDOW_OBJECT ---------------------------------------------------

EVENT_TYPE UI_WINDOW_OBJECT::DrawBorder(ZIL_SCREENID screenID, UI_REGION &region,
        int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
        UI_DISPLAY *display = this->display;
#endif

        display->VirtualGet(screenID, trueRegion);

        // Draw the outer rectangle.
        if (Inherited(ID_STATUS_ITEM) || Inherited(ID_TABLE_RECORD))
        {
                // Draw chiseled border
                UI_PALETTE *leftTop = LogicalPalette(ccode, ID_DARK_SHADOW);
                UI_PALETTE *rightBottom;
                if (Inherited(ID_TABLE_RECORD) && !parent->Inherited(ID_TABLE_HEADER))
                        rightBottom = &display->colorMap[LIGHTGRAY];
                else
                        rightBottom = LogicalPalette(ccode, ID_WHITE_SHADOW);                   
                lastPalette = LogicalPalette(ccode, ID_STATUS_ITEM);
                display->Line(screenID, region.right, region.top, region.right,
                        region.bottom, rightBottom, 1, FALSE);
                display->Line(screenID, region.left, region.bottom, region.right,
                        region.bottom, rightBottom, 1, FALSE);
                display->Line(screenID, region.left, region.bottom, region.left,
                        region.top, leftTop, 1, FALSE);
                display->Line(screenID, region.left, region.top, region.right,
                        region.top, leftTop, 1, FALSE);
        }
        else if (Inherited(ID_BUTTON))
        {
                UI_PALETTE *outline = &display->colorMap[BLACK];
                display->Line(screenID, region.left + 1, region.top,
                        region.right - 1, region.top, outline, 1, FALSE);
                display->Line(screenID, region.left + 1, region.bottom,
                        region.right - 1, region.bottom, outline, 1, FALSE);
                display->Line(screenID, region.left, region.top + 1, region.left,
                        region.bottom - 1, outline, 1, FALSE);
                display->Line(screenID, region.right, region.top + 1, region.right,
                        region.bottom - 1, outline, 1, FALSE);
        }
        else
                display->Rectangle(screenID, region, lastPalette, 1, fillRegion);
        --region;

        display->VirtualPut(screenID);
        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawFocus(ZIL_SCREENID screenID, UI_REGION &region, EVENT_TYPE ccode)
{
        if (ccode != S_CURRENT)
                return (FALSE);

        display->VirtualGet(screenID, trueRegion);
        display->Rectangle(screenID, region, display->xorPalette, 1, FALSE, FALSE);
        display->Rectangle(screenID, region, display->xorPalette, 1, FALSE, TRUE);
        display->VirtualPut(screenID);
        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
        // OS/2 automatically displays text when return is FALSE.
        return (FALSE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawShadow(ZIL_SCREENID screenID, UI_REGION &region,
        int depth, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
        UI_DISPLAY *display = this->display;
#endif

        display->VirtualGet(screenID, region);

        // Draw the outer shadow.
        UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
        int delta = (region == trueRegion) ? 1 : 0;
        display->Line(screenID, region.left + delta, region.top,
                region.right - delta, region.top, outline, 1, FALSE);
        display->Line(screenID, region.left + delta, region.bottom,
                region.right - delta, region.bottom, outline, 1, FALSE);
        display->Line(screenID, region.left, region.top + delta, region.left,
                region.bottom - delta, outline, 1, FALSE);
        display->Line(screenID, region.right, region.top + delta, region.right,
                region.bottom - delta, outline, 1, FALSE);
        --region;

        // Draw the inner shadow.
        if (depth)
        {
                UI_PALETTE *lightShadow = LogicalPalette(ccode, (depth > 0) ? ID_WHITE_SHADOW : ID_DARK_SHADOW);
                UI_PALETTE *darkShadow = LogicalPalette(ccode, (depth > 0) ? ID_DARK_SHADOW : ID_LIGHT_SHADOW);
                display->Line(screenID, region.left, region.top + 1, region.left,
                        region.bottom - 1, lightShadow, 1, FALSE);
                display->Line(screenID, region.left, region.top, region.right,
                        region.top, lightShadow, 1, FALSE);
                display->Line(screenID, region.right, region.top + 1, region.right,
                        region.bottom, darkShadow, 1, FALSE);
                display->Line(screenID, region.left, region.bottom, region.right - 1,
                        region.bottom, darkShadow, 1, FALSE);
                --region;
        }

        // Fill the region (if so specified).
        if (fillRegion)
                display->Rectangle(screenID, region, ZIL_NULLP(UI_PALETTE), 0, TRUE, FALSE);

        display->VirtualPut(screenID);
        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawText(ZIL_SCREENID screenID, UI_REGION &region,
        const ZIL_ICHAR *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE )
{
#if defined(ZIL_OPTIMIZE)
        UI_DISPLAY *display = this->display;
#endif

        display->VirtualGet(screenID, region);

        // Fill the region (if so specified).
        if (fillRegion)
                display->Rectangle(screenID, region, palette, 0, TRUE, FALSE);

        // Make sure there is a valid string.
        if (text && text[0])
        {

                // Draw the text.
                int textWidth = display->TextWidth(text, screenID, font);
                int textHeight = display->TextHeight(text, screenID, font);
                if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
                        region.left = region.right - textWidth;
                else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
                        region.left = region.left + (region.Width() - textWidth) / 2;
                region.top += (region.Height() - textHeight) / 2;

                display->Text(screenID, region.left, region.top, text, palette, -1,
                        FALSE, FALSE, &region, font);
                region.bottom = region.top + textHeight - 1;
                region.right = region.left + textWidth - 1;
        }

        display->VirtualPut(screenID);
        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::Event(const UI_EVENT &event)
{
        static PFNWP _objectCallback = ZIL_NULLP(FNWP);
        UI_EVENT tEvent = event;
        UI_WINDOW_OBJECT *object;

        // Seed the time indicator.
        static int initializedTime = FALSE;
        static ZIL_TIME lastTime;
        if (!initializedTime)
        {
                lastTime.Import();
                initializedTime = TRUE;
        }

        // Check for OS/2 specific messages.
        EVENT_TYPE ccode = event.type;
        if (ccode == E_OS2)
        {
                int processed = TRUE;
                EVENT_TYPE returnValue = 0;

                switch (event.message.msg)
                {
                case WM_WINDOWPOSCHANGED:
                        {
                        SWP *swp = (SWP *)event.message.mp1;

                        if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT) &&
                                !FlagSet(woStatus, WOS_INTERNAL_ACTION))
                        {
                                if (FlagSet(swp[0].fl, SWP_MOVE))
                                {
                                        int xDelta = (short)swp[0].x - (short)swp[1].x;
                                        int yDelta = (short)swp[1].y - (short)swp[0].y;
                                        trueRegion.left += xDelta;
                                        trueRegion.top += yDelta;
                                        trueRegion.right += xDelta;
                                        trueRegion.bottom += yDelta;
                                        relative.left += xDelta;
                                        relative.top += yDelta;
                                        relative.right += xDelta;
                                        relative.bottom += yDelta;
                                }
                                if (FlagSet(swp[0].fl, SWP_SIZE))
                                {
                                        trueRegion.right = trueRegion.left + swp[0].cx - 1;
                                        trueRegion.top = trueRegion.bottom - swp[0].cy + 1;
                                        relative.right = relative.left + swp[0].cx - 1;
                                        relative.top = relative.bottom - swp[0].cy + 1;
//WinInvalidateRect(screenID, ZIL_NULLP(RECTL), TRUE);
                                }
                        }

                        if (FlagSet(swp[0].fl, SWP_SIZE))
                                Event(UI_EVENT(S_SIZE, TRUE, relative));

                        returnValue = DefaultCallback(event);
                        }
                        break;

                case WM_MEASUREITEM:
                        returnValue = relative.Width() << 16;
                        returnValue |= (display->cellHeight - display->preSpace - display->postSpace);
                        break;

                case WM_DESTROY:
                        returnValue = DefaultCallback(event);
                        screenID = 0;
                        break;

                case WM_PAINT:
                        if (FlagSet(woStatus, WOS_OWNERDRAW))
                        {
                                RECTL rect;
                                UI_OS2_DISPLAY::hps = WinBeginPaint(screenID, ZIL_NULLH(HPS), &rect);
                                WinQueryWindowRect(screenID, &rect);
                                display->VirtualGet(ID_DIRECT, trueRegion.left, trueRegion.top,
                                        trueRegion.right, trueRegion.top + rect.yTop - rect.yBottom - 1);
                                DrawItem(event, FlagSet(woStatus, WOS_CURRENT) ?
                                        S_CURRENT : S_DISPLAY_ACTIVE);
                                WinEndPaint(UI_OS2_DISPLAY::hps);
                                display->VirtualPut(ID_DIRECT);
                        }
                        else
                                DefaultCallback(event);
                        return (0);

                case WM_DRAWITEM:
                        DrawItem(event, FlagSet(woStatus, WOS_CURRENT) ?
                                S_CURRENT : S_DISPLAY_ACTIVE);
                        return (TRUE);

                case WM_FOCUSCHANGE:
                        returnValue = DefaultCallback(event);
                        if (LOWORD(event.message.mp2) &&
                                !FlagSet(HIWORD(event.message.mp2), FC_NOSETFOCUS) &&
                                parent)
                        {
                                tEvent.type = S_ADD_OBJECT;
                                tEvent.data = this;
                                parent->Event(tEvent);
                        }
                        break;

                case WM_HELP:
                        if (parent && helpContext == NO_HELP_CONTEXT)
                                parent->Event(UI_EVENT(L_HELP, 0));
                        else if (helpSystem)
                                helpSystem->DisplayHelp(windowManager, helpContext);
                        return (0);

                case WM_MOUSEMOVE:
#if defined(ZIL_EDIT)
                        if (FlagSet(woStatus, WOS_EDIT_MODE))
                        {
                                static ZIL_ICHAR windowEditorName[] = { 'Z','A','F','_','W','I','N','D','O','W','_','E','D','I','T','O','R', 0 };
                                static ZIL_ICHAR placeFieldName[] = { 'P','L','A','C','E','_','O','B','J','E','C','T','_','F','I','E','L','D', 0 };
                                UI_WINDOW_OBJECT *windowEditor = windowManager->Get(windowEditorName);
                                if (windowEditor)
                                {
                                        ZIL_ICHAR *placeText;
                                        windowEditor->Get(placeFieldName)->Information(I_GET_TEXT, &placeText);
                                        if (!placeText || !*placeText)
                                        {
                                                if (!parent || parent->Inherited(ID_NOTEBOOK))
                                                {
                                                        returnValue = DefaultCallback(event);
                                                        break;
                                                }

                                                // Convert to frame coordinates.
                                                UI_POSITION mousePosition = event.position;
                                                ZIL_SCREENID frameID;
                                                Information(I_GET_FRAMEID, &frameID);
                                                if (frameID && frameID != screenID &&
                                                        event.message.hwnd != frameID)
                                                {
                                                        SWP swp;
                                                        SWP frameSwp;
                                                        WinQueryWindowPos(screenID, &swp);
                                                        WinQueryWindowPos(frameID, &frameSwp);
                                                        mousePosition.column += swp.x;
                                                        mousePosition.line += frameSwp.cy - swp.cy - swp.y;
                                                }

                                                ZIL_RAW_CODE sizeFlags = 0;
                                                if (mousePosition.column > trueRegion.right - 5)
                                                        sizeFlags |= M_RIGHT_CHANGE;
                                                else if (mousePosition.column < trueRegion.left + 5)
                                                        sizeFlags |= M_LEFT_CHANGE;
                                                if (mousePosition.line > trueRegion.bottom - 5)
                                                        sizeFlags |= M_BOTTOM_CHANGE;
                                                else if (mousePosition.line < trueRegion.top + 5)
                                                        sizeFlags |= M_TOP_CHANGE;

                                                if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
                                                {
                                                        if (Inherited(ID_TABLE_HEADER))
                                                        {
                                                                THF_FLAGS thFlags;
                                                                Information(I_GET_FLAGS, &thFlags);
                                                                if (FlagSet(thFlags, THF_COLUMN_HEADER))
                                                                        sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_RIGHT_CHANGE;
                                                                else if (FlagSet(thFlags, THF_ROW_HEADER))
                                                                        sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_BOTTOM_CHANGE;
                                                                else
                                                                        sizeFlags = 0;
                                                        }
                                                        else if (Inherited(ID_TOOL_BAR))
                                                        {
                                                                WNF_FLAGS wnFlags = WNF_NO_FLAGS;
                                                                Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
                                                                if (FlagSet(wnFlags, WNF_NO_WRAP))
                                                                        sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_RIGHT_CHANGE;
                                                                else
                                                                        sizeFlags = 0;
                                                        }
                                                        else if (Inherited(ID_STATUS_BAR))
                                                                sizeFlags &= ~M_LEFT_CHANGE & ~M_RIGHT_CHANGE & ~M_BOTTOM_CHANGE;
                                                        else
                                                                sizeFlags = 0;
                                                }
                                                else if (Inherited(ID_ICON))
                                                        sizeFlags = 0;
                                                else if (Inherited(ID_BUTTON))
                                                {
                                                        BTF_FLAGS btFlags;
                                                        Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
                                                        if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
                                                                sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
                                                }
                                                else if (Inherited(ID_SCROLL_BAR))
                                                {
                                                        SBF_FLAGS sbFlags = SBF_NO_FLAGS;
                                                        Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
                                                        if (!FlagSet(sbFlags, SBF_VERTICAL))
                                                                sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
                                                        if (!FlagSet(sbFlags, SBF_HORIZONTAL))
                                                                sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
                                                }
                                                else if (Inherited(ID_TABLE_RECORD))
                                                {
                                                        sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE;
                                                        if (parent->Inherited(ID_TABLE_HEADER))
                                                        {
                                                                THF_FLAGS thFlags;
                                                                parent->Information(I_GET_FLAGS, &thFlags);
                                                                if (FlagSet(thFlags, THF_COLUMN_HEADER))
                                                                        sizeFlags &= ~M_BOTTOM_CHANGE;
                                                                else if (FlagSet(thFlags, THF_ROW_HEADER))
                                                                        sizeFlags &= ~M_RIGHT_CHANGE;
                                                                else
                                                                        sizeFlags = 0;
                                                        }
                                                }
                                                else if ((!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX) ||
                                                        Inherited(ID_SPIN_CONTROL)) && searchID < 3000)
                                                        sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);

                                                LONG pointerID = SPTR_ARROW;
                                                if (FlagsSet(sizeFlags, M_LEFT_CHANGE | M_TOP_CHANGE) ||
                                                        FlagsSet(sizeFlags, M_RIGHT_CHANGE | M_BOTTOM_CHANGE))
                                                        pointerID = SPTR_SIZENWSE;
                                                else if (FlagsSet(sizeFlags, M_RIGHT_CHANGE | M_TOP_CHANGE) ||
                                                        FlagsSet(sizeFlags, M_LEFT_CHANGE | M_BOTTOM_CHANGE))
                                                        pointerID = SPTR_SIZENESW;
                                                else if (FlagSet(sizeFlags, M_LEFT_CHANGE | M_RIGHT_CHANGE))
                                                        pointerID = SPTR_SIZEWE;
                                                else if (FlagSet(sizeFlags, M_TOP_CHANGE | M_BOTTOM_CHANGE))
                                                        pointerID = SPTR_SIZENS;
                                                else if (!FlagSet(woFlags, WOF_NON_FIELD_REGION) && !Inherited(ID_TABLE_RECORD))
                                                {
                                                        static ZIL_ICON_HANDLE movePointer = 0;
                                                        if (!movePointer)
                                                                display->IconArrayToHandle(screenID, 18, 18, _movePointerData,
                                                                        ZIL_NULLP(UI_PALETTE), &movePointer);   
                                                        WinSetPointer(HWND_DESKTOP, movePointer);
                                                        returnValue = TRUE;
                                                        break;
                                                }

                                                WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, pointerID, FALSE));
                                                returnValue = TRUE;
                                                break;
                                        }
                                        else
                                        {
                                                static ZIL_ICON_HANDLE placePointer = 0;
                                                if (!placePointer)
                                                        display->IconArrayToHandle(screenID, 16, 16, _placePointerData,
                                                                ZIL_NULLP(UI_PALETTE), &placePointer);  
                                                WinSetPointer(HWND_DESKTOP, placePointer);
                                                returnValue = TRUE;
                                                break;
                                        }

                                }
                        }
#endif
                        if ((FlagSet(woFlags, WOF_NON_SELECTABLE) || FlagSet(woAdvancedFlags, WOAF_NON_CURRENT)) &&
                                !Inherited(ID_SCROLL_BAR))
                        {
                                WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
                                returnValue = TRUE; // Prevent the mouse from being reset.
                        }
                        else
                                returnValue = DefaultCallback(event);
                        break;

                case WM_BUTTON1UP:
                case WM_BUTTON2UP:
                case WM_BUTTON3UP:
                        if (!FlagSet(woStatus, WOS_EDIT_MODE))
                                ccode = DefaultCallback(event);
                        break;

                case WM_BEGINDRAG:
                        if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT) ||
                                Inherited(ID_LIST))
                        {
                                int count = 1;
                                if (Inherited(ID_LIST) || Inherited(ID_COMBO_BOX))
                                {
                                        if (!FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
                                        {
                                                Information(I_GET_CURRENT, &object);
                                                if (object && FlagSet(object->woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
                                                        windowManager->dragObject = object;
                                                else
                                                        return (DefaultCallback(event));
                                        }
                                        else
                                        {
                                                count = 0;
                                                for(Information(I_GET_FIRST, &object); object; object = object->Next())
                                                        if (FlagSet(object->woStatus, WOS_SELECTED))
                                                                count++;
                                                if (count > 0)
                                                        windowManager->dragObject = this;
                                                else
                                                        return (DefaultCallback(event));
                                        }
                                }
                                else
                                        windowManager->dragObject = this;

                                DRAGITEM dragItem;
                                dragItem.hwndItem = screenID;
                                dragItem.ulItemID = numberID;
                                dragItem.hstrType = DrgAddStrHandle((PSZ)"DRT_ZINC");
                                dragItem.hstrRMF = DrgAddStrHandle((PSZ)"<DRM_WINDOW_MANAGER, DRT_ZINC_OBJECT>");
                                dragItem.hstrContainerName = DrgAddStrHandle((PSZ)"");
                                dragItem.hstrSourceName = DrgAddStrHandle((PSZ)"");
                                dragItem.hstrTargetName = DrgAddStrHandle((PSZ)"");
                                dragItem.cxOffset = 0;
                                dragItem.cyOffset = 0;
                                dragItem.fsControl = 0;
                                dragItem.fsSupportedOps = 0;

                                DRAGIMAGE *dragImage;
                                int imageWidth = 0, imageHeight = 0;
                                ZIL_UINT8 *imageArray = ZIL_NULLP(ZIL_UINT8);
                                if (Inherited(ID_ICON))
                                {
                                        if (Information(I_GET_ICON_WIDTH, &imageWidth) &&
                                                Information(I_GET_ICON_HEIGHT, &imageHeight) &&
                                                imageWidth && imageHeight)
                                                        Information(I_GET_ICON_ARRAY, &imageArray);
                                }
                                else if (Inherited(ID_BUTTON))
                                {
                                        BTF_FLAGS btFlags;
                                        Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
                                        if (!FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
                                                Information(I_GET_BITMAP_WIDTH, &imageWidth) &&
                                                Information(I_GET_BITMAP_HEIGHT, &imageHeight) &&
                                                imageWidth && imageHeight)
                                                        Information(I_GET_BITMAP_ARRAY, &imageArray);
                                }
                                else if (Information(I_GET_BITMAP_WIDTH, &imageWidth) &&
                                        Information(I_GET_BITMAP_HEIGHT, &imageHeight) &&
                                        imageWidth && imageHeight)
                                                Information(I_GET_BITMAP_ARRAY, &imageArray);

                                if (imageArray)
                                {
                                        ZIL_ICON_HANDLE pointerImage = ZIL_NULLH(ZIL_ICON_HANDLE);
                                        Information(I_GET_BITMAP_ARRAY, &imageArray);
                                        display->IconArrayToHandle(screenID, imageWidth,
                                                imageHeight, imageArray, ZIL_NULLP(UI_PALETTE),
                                                &pointerImage);
                                        dragImage = new DRAGIMAGE;
                                        dragImage->cb = sizeof(DRAGIMAGE);
                                        dragImage->cptl = 0;
                                        dragImage->hImage = pointerImage;
                                        dragImage->fl = DRG_ICON;
                                        dragImage->cxOffset = 0;
                                        dragImage->cyOffset = 0;
                                        count = 1;
                                }
                                else
                                {
                                        dragImage = new DRAGIMAGE[count];
                                        for (int i = 0; i < MinValue(count, 10); i++)
                                        {
                                                dragImage[i].cb = sizeof(DRAGIMAGE);
                                                dragImage[i].cptl = 0;
                                                dragImage[i].hImage = WinQuerySysPointer(HWND_DESKTOP,
                                                        (count > 10 && i == 9) ? SPTR_MULTFILE : SPTR_FILE, FALSE);
                                                dragImage[i].fl = DRG_ICON;
                                                dragImage[i].cxOffset = i * 10;
                                                dragImage[i].cyOffset = i * 10;
                                        }
                                        count = MinValue(count, 10);
                                }

                                DRAGINFO *dragInfo = DrgAllocDraginfo(1);
                                if (!FlagsSet(windowManager->dragObject->woAdvancedFlags,
                                        WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
                                {
                                        if (FlagsSet(windowManager->dragObject->woAdvancedFlags, WOAF_COPY_DRAG_OBJECT))
                                                dragInfo->usOperation = DO_COPY;
                                        else
                                                dragInfo->usOperation = DO_MOVE;
                                }

                                DrgSetDragitem(dragInfo, &dragItem, sizeof(dragItem), 0);
                                DrgDrag(screenID, dragInfo, dragImage, count, VK_ENDDRAG, ZIL_NULLP(void));
                                delete dragImage;
                                windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                                return (TRUE);
                        }
                        return (FALSE);

                case DM_DRAGOVER:
                        if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP))
                        {
                                DRAGINFO *dragInfo = (DRAGINFO *)event.message.mp1;
                                DrgAccessDraginfo(dragInfo);
                                DRAGITEM *dragItem = DrgQueryDragitemPtr(dragInfo, 0);

                                if (DrgVerifyTrueType(dragItem, (PSZ)"DRT_ZINC") &&
                                        windowManager->dragObject)
                                {
                                        WOAF_FLAGS dragFlags = windowManager->dragObject->woAdvancedFlags;
                                        if (dragItem->hwndItem != screenID &&
                                                !((dragInfo->usOperation == DO_MOVE && !FlagSet(dragFlags, WOAF_MOVE_DRAG_OBJECT)) ||
                                                (dragInfo->usOperation == DO_COPY && !FlagSet(dragFlags, WOAF_COPY_DRAG_OBJECT))))
                                        {
                                                UI_EVENT dragEvent;
                                                if (dragInfo->usOperation == DO_MOVE)
                                                        dragEvent.type = S_DRAG_MOVE_OBJECT;
                                                else if (dragInfo->usOperation == DO_COPY)
                                                        dragEvent.type = S_DRAG_COPY_OBJECT;
                                                else
                                                        dragEvent.type = S_DRAG_DEFAULT;

                                                EVENT_TYPE response = userFunction ?
                                                        UserFunction(dragEvent, dragEvent.type) : S_UNKNOWN;
                                                if (response == S_UNKNOWN)
                                                        response = Event(dragEvent);

                                                if (response != S_ERROR)
                                                {
                                                        EVENT_TYPE image = eventManager->DeviceState(E_MOUSE, D_STATE);
                                                        if (image == DM_DRAG_COPY || image == DM_DRAG_COPY_MULTIPLE)
                                                                return ((EVENT_TYPE)MRFROM2SHORT(DOR_DROP, DO_COPY));
                                                        else
                                                                return ((EVENT_TYPE)MRFROM2SHORT(DOR_DROP, DO_UNKNOWN));
                                                }
                                        }
                                }
                        }
                        return ((EVENT_TYPE)MRFROM2SHORT(DOR_NEVERDROP, 0));

                case DM_DROP:
                        {
                                DRAGINFO *dragInfo = (DRAGINFO *)event.message.mp1;
                                DrgAccessDraginfo(dragInfo);

                                UI_EVENT dragEvent;
                                if (dragInfo->usOperation == DO_MOVE)
                                        dragEvent.type = S_DROP_MOVE_OBJECT;
                                else if (dragInfo->usOperation == DO_COPY)
                                        dragEvent.type = S_DROP_COPY_OBJECT;
                                else
                                        dragEvent.type = S_DROP_DEFAULT;
                                
                                POINTL point;
                                point.x = dragInfo->xDrop;
                                point.y = dragInfo->yDrop;
                                WinMapWindowPoints(HWND_DESKTOP, screenID, &point, 1);

                                SWP swp;
                                WinQueryWindowPos(screenID, &swp);
                                dragEvent.position.column = trueRegion.left + point.x;
                                dragEvent.position.line = trueRegion.top + swp.cy - point.y - 1;

                                EVENT_TYPE response = userFunction ?
                                        UserFunction(dragEvent, dragEvent.type) : S_UNKNOWN;
                                if (response == S_UNKNOWN)
                                        Event(dragEvent);
                                windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                        }
                        return (0);

                case WM_BUTTON1DOWN:
                case WM_BUTTON2DOWN:
                case WM_BUTTON3DOWN:
                case WM_BUTTON1DBLCLK:
                case WM_BUTTON2DBLCLK:
                case WM_BUTTON3DBLCLK:
                        if (FlagSet(woStatus, WOS_EDIT_MODE) ||
                                (FlagSet(woAdvancedFlags, WOAF_NON_CURRENT) && !Inherited(ID_BUTTON)
                                && !Inherited(ID_SCROLL_BAR)))
                        {
                                ZIL_SCREENID rootID;
                                Root()->Information(I_GET_FRAMEID, &rootID);
                                if (WinQueryActiveWindow(HWND_DESKTOP) != rootID)
                                        WinSetActiveWindow(HWND_DESKTOP, rootID);
#if defined(ZIL_EDIT)
                                if (!FlagSet(woStatus, WOS_EDIT_MODE))
#endif
                                        break;
                        }
#if defined(ZIL_EDIT)
                        if (FlagSet(woStatus, WOS_EDIT_MODE))
                        {
                                if (!Inherited(ID_STATUS_ITEM))
                                        WinSetFocus(HWND_DESKTOP, screenID);
                                if (event.message.msg == WM_BUTTON1DOWN)
                                {
                                        // Modify the object position.
                                        Modify(tEvent);

                                        // Set the new current object.
                                        tEvent.type = D_SET_OBJECT;
                                        tEvent.rawCode = searchID;
                                        tEvent.data = this;
                                        eventManager->Put(tEvent);
                                        tEvent.type = D_SET_POSITION;
                                        tEvent.rawCode = M_LEFT | M_LEFT_CHANGE;
                                        tEvent.position = event.position;
                                        eventManager->Put(tEvent);
                                }
                                else if (event.message.msg == WM_BUTTON2DOWN)
                                {
                                        tEvent.type = D_SET_POSITION;
                                        tEvent.rawCode = M_RIGHT | M_RIGHT_CHANGE;
                                        tEvent.position.column = event.position.column;
                                        tEvent.position.line = (trueRegion.bottom - trueRegion.top) - event.position.line;
                                        eventManager->Put(tEvent);
                                        eventManager->Put(UI_EVENT(E_OS2, screenID, WM_MOUSEMOVE,
                                                MPFROM2SHORT(event.position.column - trueRegion.left,
                                                trueRegion.Height() - (event.position.line - trueRegion.top)), 0));
                                }
                                else if (event.message.msg == WM_BUTTON1DBLCLK)
                                {
                                        tEvent.type = D_EDIT_OBJECT;
                                        tEvent.rawCode = searchID;
                                        tEvent.data = this;
                                        eventManager->Put(tEvent);
                                }
                        }
                        else
#endif
                                returnValue = DefaultCallback(event);
                        break;

                default:
                        {
                        EVENT_TYPE inputType = event.InputType();
                        if (inputType == E_KEY)
                                processed = FALSE;
                        else
                                returnValue = DefaultCallback(event);
                        }
                        break;
                }

                if (processed)
                        return returnValue;
        }

        if (ccode == E_OS2)
                ccode = LogicalEvent(event);

        switch (ccode)
        {
        case S_INITIALIZE:
                flStyle = WS_VISIBLE;
                if (!numberID && parent)
                {
                        object = Root();
                        numberID = object->NumberID() ? object->NumberID() : 1;
                        object->NumberID(numberID + 1);
                        if (stringID[0] == '\0')
                        {
                                strcpy(stringID, _genericFieldID);
                                itoa(numberID, &stringID[strlen(stringID)], 10);
                        }
                }
                RegionConvert(relative, (parent && !FlagSet(woFlags, WOF_NON_FIELD_REGION)) ? FALSE : TRUE);
                lastPalette = LogicalPalette(S_DISPLAY_ACTIVE);
                break;

        case S_DEINITIALIZE:
                if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                {
                        woStatus &= ~WOS_SYSTEM_OBJECT;
                        if (event.windowObject == this)
                                WinDestroyWindow(screenID);
                }
                screenID = 0;
                break;

        case S_CREATE:
                if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                {
                        tEvent.type = S_DEINITIALIZE;
                        tEvent.windowObject = this;
                        Event(tEvent);
                }

                // Let parent adjust size/position.
                if (parent)
                        parent->RegionMax(this);
                else
                        RegionMax(this);

                // Let object adjust size/position.
                Event(UI_EVENT(S_CHANGED));

                // Create the object;
                Event(UI_EVENT(S_REGISTER_OBJECT));
                break;

        case S_REGISTER_OBJECT:
                RegisterObject("UI_WINDOW_OBJECT", 0, &_objectCallback, _blankString);
                break;

        case S_REDISPLAY:
                if (screenID)
                {
                        if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                        {
                                ZIL_SCREENID frameID;
                                Information(I_GET_FRAMEID, &frameID);
                                WinEnableWindowUpdate(frameID, TRUE);
                                WinEnableWindowUpdate(screenID, TRUE);
                                WinInvalidateRect(frameID, ZIL_NULLP(RECTL), TRUE);
                        }
                        else
                        {
                                SWP swp;
                                WinQueryWindowPos(screenID, &swp);
                                RECTL rect;
                                rect.xLeft = trueRegion.left;
                                rect.yTop = swp.cy - trueRegion.top;
                                rect.xRight = trueRegion.right + 1;
                                rect.yBottom = swp.cy - trueRegion.bottom - 1;
                                WinInvalidateRect(screenID, &rect, TRUE);
                        }
                        woStatus &= ~WOS_REDISPLAY;
                }
                break;

        case S_DISPLAY_ACTIVE:
        case S_DISPLAY_INACTIVE:
                if (screenID)
                {
                        WinEnableWindowUpdate(screenID, TRUE);
                        if (event.region == trueRegion)
                                WinInvalidateRect(screenID, ZIL_NULLP(RECTL), TRUE);
                        else
                        {
                                SWP swp;
                                WinQueryWindowPos(screenID, &swp);
                                RECTL rect;
                                rect.xLeft = event.region.left;
                                rect.yBottom = swp.cy - event.region.bottom - 1;
                                rect.xRight = event.region.right + 1;
                                rect.yTop = swp.cy - event.region.top;
                                WinInvalidateRect(screenID, &rect, TRUE);
                        }
                }
                break;

        case S_SIZE:
                if (!event.rawCode)
                {
                        relative = event.region;

                        // Let parent adjust size/position.
                        if (parent)
                                parent->RegionMax(this);
                        else
                                RegionMax(this);

                        // Let object adjust size/position.
                        Event(UI_EVENT(S_CHANGED));

                        // Size the object;
                        if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                        {
                                SWP swp;
                                WinQueryWindowPos(parent ? parent->screenID : HWND_DESKTOP, &swp);
                                ZIL_SCREENID frameID;
                                Information(I_GET_FRAMEID, &frameID);
                                woStatus |= WOS_INTERNAL_ACTION;
                                WinSetWindowPos(frameID, 0, trueRegion.left, swp.cy - trueRegion.bottom - 1,    
                                        trueRegion.Width(), trueRegion.Height(), SWP_MOVE | SWP_SIZE | SWP_NOADJUST);
                                woStatus &= ~WOS_INTERNAL_ACTION;
                        }
                }
                break;

        case S_CURRENT:
        case S_NON_CURRENT:
                if (ccode == S_CURRENT && !FlagSet(woStatus, WOS_CURRENT))
                {
                        woStatus |= WOS_REDISPLAY;
                        woStatus |= WOS_CURRENT;
                }
                else if (ccode == S_NON_CURRENT && FlagSet(woStatus, WOS_CURRENT))
                {
                        woStatus |= WOS_REDISPLAY;
                        woStatus &= ~WOS_CURRENT;
                }

                if (ccode == S_CURRENT && FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                        WinSetFocus(HWND_DESKTOP, screenID);

                if (FlagSet(woStatus, WOS_REDISPLAY))
                {
                        if (FlagsSet(woStatus, WOS_OWNERDRAW))
                                Event(UI_EVENT(S_REDISPLAY, 0));
                        woStatus &= ~WOS_REDISPLAY;
                }

                // Call the associated user or validate function.
                ccode = UserFunction(event, ccode);
                break;

        case S_DRAG_DEFAULT:
        case S_DRAG_MOVE_OBJECT:
        case S_DRAG_COPY_OBJECT:
                if (windowManager->dragObject->Inherited(ID_LIST))
                {
                        int count = 0;
                        for(windowManager->dragObject->Information(I_GET_FIRST, &object); object; object = object->Next())
                                if (FlagSet(object->woStatus, WOS_SELECTED))
                                        count++;
                        if (count != 1)
                                return (S_ERROR);
                }
                return (0);

        case S_DROP_DEFAULT:
        case S_DROP_MOVE_OBJECT:
        case S_DROP_COPY_OBJECT:
                {
                ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
                if (ccode == S_DROP_MOVE_OBJECT &&
                        windowManager->dragObject->Inherited(ID_LIST))
                {
                        int count = 0;
                        UI_WINDOW_OBJECT *selectedObject;
                        for(windowManager->dragObject->Information(I_GET_FIRST, &object); object; object = object->Next())
                                if (FlagSet(object->woStatus, WOS_SELECTED))
                                {
                                        selectedObject = object;
                                        count++;
                                }

                        if (count == 1)
                        {
                                UI_EVENT subtractEvent(S_SUBTRACT_OBJECT);
                                subtractEvent.windowObject = selectedObject;
                                windowManager->dragObject->Event(subtractEvent);
                                windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
                                selectedObject->Information(I_GET_TEXT, &text);
                                if (text)
                                        Information(I_SET_TEXT, text);
                                delete selectedObject;
                        }
                }
                else if (ccode == S_DROP_MOVE_OBJECT &&
                        windowManager->dragObject->parent &&
                        windowManager->dragObject->parent->Inherited(ID_LIST))
                {
                        UI_WINDOW_OBJECT *list = windowManager->dragObject->parent;
                        UI_EVENT subtractEvent(S_SUBTRACT_OBJECT);
                        subtractEvent.windowObject = windowManager->dragObject;
                        list->Event(subtractEvent);
                        list->Event(UI_EVENT(S_REDISPLAY));
                        windowManager->dragObject->Information(I_GET_TEXT, &text);
                        if (text)
                                Information(I_SET_TEXT, text);
                        delete windowManager->dragObject;
                }
                else
                {
                        if (windowManager->dragObject->Inherited(ID_LIST))
                        {
                                for(windowManager->dragObject->Information(I_GET_FIRST, &object); object; object = object->Next())
                                        if (FlagSet(object->woStatus, WOS_SELECTED))
                                        {
                                                object->Information(I_GET_TEXT, &text);
                                                break;
                                        }
                        }
                        else
                                windowManager->dragObject->Information(I_GET_TEXT, &text);
                        if (text)
                                Information(I_SET_TEXT, text);
                        if (ccode == S_DROP_MOVE_OBJECT)
                                windowManager->dragObject->Information(I_SET_TEXT, &_blankString);
                }
                }
                break;

        case L_SELECT:
                ccode = UserFunction(event, L_SELECT);
                if (parent)
                {
                        if (parent->Inherited(ID_TABLE_RECORD))
                                parent->Event(event);
                        else if (ccode != S_ERROR && !parent->Inherited(ID_COMBO_BOX))
                                Root(TRUE)->Event(UI_EVENT(L_SELECT));
                }
                break;

        case L_HELP:
                if (parent && helpContext == NO_HELP_CONTEXT)
                        parent->Event(event);
                else if (helpSystem)
                        helpSystem->DisplayHelp(windowManager, helpContext);
                break;

        case E_KEY:
                if (event.rawCode == 0xFFFFFFFF)
                        return (0);
                else
                {
#if defined (ZIL_UNICODE)
                        char *temp = MapChar(event.key.value);
                        DefaultCallback(UI_EVENT(E_OS2, screenID, WM_CHAR,
                                MPFROM2SHORT(KC_CHAR, 1), MPFROMSHORT(MAKESHORT((UCHAR)temp[0], (UCHAR)temp[1]))));
#elif defined(ZIL_ISO8859_1)
                        DefaultCallback(UI_EVENT(E_OS2, screenID, WM_CHAR,
                                MPFROM2SHORT(KC_CHAR, 1), MPFROMCHAR((UCHAR)MapChar(event.key.value)[0])));
#else
                        DefaultCallback(UI_EVENT(E_OS2, screenID, WM_CHAR,
                                MPFROM2SHORT(KC_CHAR, 1), MPFROMCHAR((UCHAR)event.key.value)));
#endif
                }
                break;

        default:
                if (event.type == E_OS2)
                        ccode = DefaultCallback(event);
                else
                        ccode = S_UNKNOWN;
                break;
        }

        // Return the control code.
        return (ccode);
}

void UI_WINDOW_OBJECT::Modify(const UI_EVENT &event)
{
        // Make sure we can move the object.
        if (!parent || parent->Inherited(ID_NOTEBOOK) ||
                (event.type == L_SIZE && Inherited(ID_ICON)))
                return;

        ZIL_SCREENID captureID;
        UI_POSITION mousePosition;
        if (event.type == L_MOVE || event.type == L_SIZE)
        {
                ZIL_SCREENID frameID;
                Information(I_GET_FRAMEID, &frameID);
                WinSetFocus(HWND_DESKTOP, frameID);
                captureID = frameID;

                SWP swp;
                POINTL point1, point2;
                WinQueryWindowPos(frameID, &swp);

                if (event.type == L_MOVE)
                {
                        point1.x = swp.x + swp.cx / 2;
                        point1.y = swp.y + swp.cy / 2;
                        point2.x = swp.cx / 2;
                        point2.y = swp.cy / 2;
                        mousePosition.column = trueRegion.left + swp.cx / 2;
                        mousePosition.line = trueRegion.top + swp.cy / 2;
                }
                else
                {
                        point1.x = swp.x + swp.cx;
                        point1.y = swp.y;
                        point2.x = swp.cx;
                        point2.y = 0;
                        mousePosition.column = trueRegion.right;
                        mousePosition.line = trueRegion.bottom;
                }

                WinMapWindowPoints(parent->screenID, HWND_DESKTOP, &point1, 1);
                WinSetPointerPos(HWND_DESKTOP, point1.x, point1.y);
                UI_EVENT os2Event(E_OS2);
                os2Event.message.msg = WM_MOUSEMOVE;
                os2Event.message.mp1 = MPFROM2SHORT(point2.x, point2.y);
                os2Event.message.mp2 = MPFROM2SHORT(0, KC_NONE);
                os2Event.position = mousePosition;
                UI_WINDOW_OBJECT::Event(os2Event);
        }
        else
        {
                captureID = event.message.hwnd;
                mousePosition = event.position;
        }

        // Determine move / size operation.
        ZIL_RAW_CODE sizeFlags = 0;
        int offset = (trueRegion.Width() > 10) ? 5 : 1;
        if (mousePosition.column > trueRegion.right - offset)
                sizeFlags |= M_RIGHT_CHANGE;
        else if (mousePosition.column < trueRegion.left + offset)
                sizeFlags |= M_LEFT_CHANGE;
        if (mousePosition.line > trueRegion.bottom - offset)
                sizeFlags |= M_BOTTOM_CHANGE;
        else if (mousePosition.line < trueRegion.top + offset)
                sizeFlags |= M_TOP_CHANGE;

        if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
        {
                if (Inherited(ID_TABLE_HEADER))
                {
                        THF_FLAGS thFlags;
                        Information(I_GET_FLAGS, &thFlags);
                        if (FlagSet(thFlags, THF_COLUMN_HEADER))
                                sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_RIGHT_CHANGE;
                        else if (FlagSet(thFlags, THF_ROW_HEADER))
                                sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_BOTTOM_CHANGE;
                        else
                                sizeFlags = 0;
                }
                else if (Inherited(ID_TOOL_BAR))
                {
                        WNF_FLAGS wnFlags = WNF_NO_FLAGS;
                        Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
                        if (FlagSet(wnFlags, WNF_NO_WRAP))
                                sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE & ~M_RIGHT_CHANGE;
                        else
                                sizeFlags = 0;
                }
                else if (Inherited(ID_STATUS_BAR))
                        sizeFlags &= ~M_LEFT_CHANGE & ~M_RIGHT_CHANGE & ~M_BOTTOM_CHANGE;
                else
                        sizeFlags = 0;
        }
        else if (Inherited(ID_ICON))
                sizeFlags = 0;
        else if (Inherited(ID_BUTTON))
        {
                BTF_FLAGS btFlags;
                Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
                if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
                        sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
        }
        else if (Inherited(ID_SCROLL_BAR))
        {
                SBF_FLAGS sbFlags = SBF_NO_FLAGS;
                Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
                if (!FlagSet(sbFlags, SBF_VERTICAL))
                        sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
                if (!FlagSet(sbFlags, SBF_HORIZONTAL))
                        sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
        }
        else if (Inherited(ID_TABLE_RECORD))
        {
                sizeFlags &= ~M_LEFT_CHANGE & ~M_TOP_CHANGE;
                if (parent->Inherited(ID_TABLE_HEADER))
                {
                        THF_FLAGS thFlags;
                        parent->Information(I_GET_FLAGS, &thFlags);
                        if (FlagSet(thFlags, THF_COLUMN_HEADER))
                                sizeFlags &= ~M_BOTTOM_CHANGE;
                        else if (FlagSet(thFlags, THF_ROW_HEADER))
                                sizeFlags &= ~M_RIGHT_CHANGE;
                        else
                                sizeFlags = 0;
                }
        }
        else if ((!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX) ||
                Inherited(ID_SPIN_CONTROL)) && searchID < 3000)
                        sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);

        EVENT_TYPE operation = sizeFlags ? L_SIZE : L_MOVE;

        if (operation == L_MOVE)
        {
                if (FlagSet(woFlags, WOF_NON_FIELD_REGION) || Inherited(ID_TABLE_RECORD))
                        return;
                else
                        sizeFlags = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
        }

        // Determine the minimum height and width of the object.
        int minHeight = display->cellHeight - display->preSpace - display->postSpace;
        int minWidth = 2 * display->cellWidth;
        if (Inherited(ID_WINDOW) && !Inherited(ID_SCROLL_BAR) && !Inherited(ID_LIST) &&
                !Inherited(ID_GROUP) && !Inherited(ID_TABLE_HEADER))
                minWidth *= 5;

        // Determine the absolute region.
        UI_REGION absolute;
        parent->Information(I_GET_CLIPREGION, &absolute);

        WinSetCapture(HWND_DESKTOP, captureID);

        UI_POSITION origin = mousePosition;
        UI_REGION oldRegion = trueRegion;
        if (Inherited(ID_COMBO_BOX) && !FlagSet(woStatus, WOS_OWNERDRAW))
                oldRegion.bottom = oldRegion.top + display->cellHeight - display->preSpace - display->postSpace;
        UI_REGION newRegion = oldRegion;

        int xJump;
        int yJump;
        if (FlagSet(woFlags, WOF_MINICELL))
        {
                xJump = display->cellWidth * display->miniNumeratorX / display->miniDenominatorX + 1;
                yJump = display->cellHeight * display->miniNumeratorY / display->miniDenominatorY + 1;
        }
        else if (FlagSet(woFlags, WOF_PIXEL))
                xJump = yJump = 1;
        else
        {
                xJump = display->cellWidth;
                yJump = display->cellHeight;
        }

        ZIL_SCREENID parentID;
        if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
                parent->Information(I_GET_FRAMEID, &parentID);
        else
                parentID = parent->screenID;
        display->VirtualGet(parentID, parent->trueRegion);
        display->Rectangle(parentID, parent->trueRegion.left + newRegion.left,
                parent->trueRegion.top + newRegion.top, parent->trueRegion.left + newRegion.right,
                parent->trueRegion.top + newRegion.bottom, display->xorPalette, 1, FALSE, TRUE);
        EVENT_TYPE ccode = 0;
        int deltaX = 0, deltaY = 0;
        do
        {
                int delta = FALSE;
                UI_EVENT event;
                eventManager->Get(event);
                ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
                switch (ccode)
                {
                case L_BEGIN_SELECT:
                case L_CONTINUE_SELECT:
                case L_VIEW:
                        if (event.position.column > parent->trueRegion.right)
                                event.position.column = parent->trueRegion.right;
                        if (event.position.line > parent->trueRegion.bottom)
                                event.position.line = parent->trueRegion.bottom;
                        deltaX = event.position.column - origin.column;
                        deltaY = event.position.line - origin.line;
                        delta = TRUE;
                        break;

                case L_LEFT:
                case L_RIGHT:
                case L_UP:
                case L_DOWN:
                        {
                        POINTL point;
                        WinQueryPointerPos(HWND_DESKTOP, &point);
                        if (ccode == L_LEFT)
                                point.x -= xJump;
                        else if (ccode == L_RIGHT)
                                point.x += xJump;
                        else if (ccode == L_UP)
                                point.y += yJump;
                        else if (ccode == L_DOWN)
                                point.y -= yJump;
                        WinSetPointerPos(HWND_DESKTOP, point.x, point.y);
                        }
                        break;

                default:
                        if (event.type > 9999 || ccode == L_END_SELECT)
                                windowManager->Event(event);
                        break;
                }

                // Update the new region.
                if (delta)
                {
                        // Check the absolute region.
                        if (FlagSet(sizeFlags, M_LEFT_CHANGE) && trueRegion.left + deltaX < 0)
                                deltaX = -trueRegion.left;
                        if (FlagSet(sizeFlags, M_TOP_CHANGE) && trueRegion.top + deltaY < 0)
                                deltaY = -trueRegion.top;

                        long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
                        long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

                        // Check for a cell boundary move or size.
                        if (FlagSet(woFlags, WOF_MINICELL))
                        {
                                long value = deltaX;
                                int ceil = (value >= 0) ? (int)(miniDX - 1) : int(1 - miniDX);
                                value = (value * miniDX) / (miniNX * display->cellWidth);
                                value = (value * miniNX * display->cellWidth + ceil) / miniDX;
                                deltaX = (int)value;

                                value = deltaY;
                                ceil = (value >= 0) ? (int)(miniDY - 1) : (int)(1 - miniDY);
                                value = (value * miniDY) / (miniNY * display->cellHeight);
                                value = (value * miniNY * display->cellHeight + ceil) / miniDY;
                                deltaY = (int)value;
                        }
                        else if (!FlagSet(woFlags, WOF_PIXEL))
                        {
                                int value = (deltaX > 0) ? deltaX + display->cellWidth / 2 : deltaX - display->cellWidth / 2;
                                deltaX = value / display->cellWidth * display->cellWidth;
                                value = (deltaY > 0) ? deltaY + display->cellHeight / 2 : deltaY - display->cellHeight / 2;
                                deltaY = value / display->cellHeight * display->cellHeight;
                        }

                        if (FlagSet(sizeFlags, M_LEFT_CHANGE))
                                newRegion.left = trueRegion.left + deltaX;
                        if (FlagSet(sizeFlags, M_TOP_CHANGE))
                                newRegion.top = trueRegion.top + deltaY;
                        if (FlagSet(sizeFlags, M_RIGHT_CHANGE))
                                newRegion.right = trueRegion.right + deltaX;
                        if (FlagSet(sizeFlags, M_BOTTOM_CHANGE))
                                newRegion.bottom = trueRegion.bottom + deltaY;

                        if (Inherited(ID_COMBO_BOX) && !FlagSet(woStatus, WOS_OWNERDRAW))
                                newRegion.bottom = newRegion.top + display->cellHeight - display->preSpace - display->postSpace;

                        // Check for minimum size.
                        if (operation == L_SIZE)
                        {
                                if (newRegion.left + minWidth - 1 > newRegion.right)
                                {
                                        newRegion.left = oldRegion.left;
                                        newRegion.right = oldRegion.right;
                                }
                                if (newRegion.top + minHeight - 1 > newRegion.bottom)
                                {
                                        newRegion.top = oldRegion.top;
                                        newRegion.bottom = oldRegion.bottom;
                                }
                        }

                        // Move sizing rectangle.
                        if (newRegion.left != oldRegion.left || newRegion.top != oldRegion.top ||
                                newRegion.right != oldRegion.right || newRegion.bottom != oldRegion.bottom)
                        {
                                display->Rectangle(parentID, parent->trueRegion.left + oldRegion.left,
                                        parent->trueRegion.top + oldRegion.top, parent->trueRegion.left + oldRegion.right,
                                        parent->trueRegion.top + oldRegion.bottom, display->xorPalette, 1, FALSE, TRUE);
                                display->Rectangle(parentID, parent->trueRegion.left + newRegion.left,
                                        parent->trueRegion.top + newRegion.top, parent->trueRegion.left + newRegion.right,
                                        parent->trueRegion.top + newRegion.bottom, display->xorPalette, 1, FALSE, TRUE);
                                oldRegion = newRegion;
                        }
                }
        } while (ccode != L_END_SELECT && ccode != L_SELECT && ccode != L_CANCEL);
        WinSetCapture(HWND_DESKTOP, 0);
        display->Rectangle(parentID, parent->trueRegion.left + oldRegion.left,
                parent->trueRegion.top + oldRegion.top, parent->trueRegion.left + oldRegion.right,
                parent->trueRegion.top + oldRegion.bottom, display->xorPalette, 1, FALSE, TRUE);
        display->VirtualPut(parentID);

        if (ccode == L_CANCEL)
                return;                         // Do not change the object region.

        if (newRegion.left != trueRegion.left || newRegion.right != trueRegion.right ||
                newRegion.top != trueRegion.top || newRegion.bottom != trueRegion.bottom)
        {
                // Save the old region.
                UI_REGION oldRegion = trueRegion;
                if (Inherited(ID_COMBO_BOX) && !FlagSet(woStatus, WOS_OWNERDRAW))
                {
                        oldRegion.bottom = newRegion.bottom = trueRegion.bottom = trueRegion.top +
                                display->cellHeight - display->preSpace - display->postSpace;
                }

                // Reposition the object.
                relative.left += newRegion.left - trueRegion.left;
                relative.top += newRegion.top - trueRegion.top;
                relative.right += newRegion.right - trueRegion.right;
                relative.bottom += newRegion.bottom - trueRegion.bottom;

                if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
                {
                        if (Inherited(ID_STATUS_BAR))
                        {
                                WinEnableWindowUpdate(screenID, FALSE);
                                Event(UI_EVENT(S_SIZE, 0, relative));
                        }
                        else if (Inherited(ID_TABLE_HEADER))
                        {
                                UI_WINDOW_OBJECT *tableRecord;
                                Information(I_GET_FIRST, &tableRecord);
                                if (tableRecord)
                                {
                                        tableRecord->relative.right += newRegion.right - trueRegion.right;
                                        tableRecord->relative.bottom += newRegion.bottom - trueRegion.bottom;
                                }
                        }
                        ZIL_SCREENID frameID;
                        parent->Information(I_GET_FRAMEID, &frameID);
                        WinSendMsg(frameID, WM_UPDATEFRAME, 0, 0);
                }
                else if (Inherited(ID_GROUP) || Inherited(ID_SPIN_CONTROL) || Inherited(ID_COMBO_BOX))
                        Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
                else
                        Event(UI_EVENT(S_SIZE, 0, relative));

                // Update the screen.
                UI_EVENT updateEvent(S_REDISPLAY);
                Event(updateEvent);
                updateEvent.type = S_DISPLAY_ACTIVE;
                updateEvent.region = oldRegion;
                parent->Event(updateEvent);
        }
}

static UI_WINDOW_OBJECT *_initObject = ZIL_NULLP(UI_WINDOW_OBJECT);

MRESULT EXPENTRY ObjectJumpProcedure(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)WinQueryWindowULong(hwnd, QWL_USER);
        UI_EVENT event(E_OS2, hwnd, msg, mp1, mp2);
        return ((void *)object->Event(event));
}

MRESULT EXPENTRY ObjectInitProcedure(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
        _initObject->screenID = hwnd;
        WinSetWindowULong(hwnd, QWL_USER, (ULONG)_initObject);
        WinSetWindowPtr(hwnd, QWP_PFNWP, (PVOID)ObjectJumpProcedure);
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)WinQueryWindowULong(hwnd, QWL_USER);
        return ((void *)object->Event(UI_EVENT(E_OS2, hwnd, msg, mp1, mp2)));
}

EVENT_TYPE UI_WINDOW_OBJECT::DefaultCallback(const UI_EVENT &event)
{
        if (defaultCallback)
                return ((ULONG)(defaultCallback)(event.message.hwnd,
                        event.message.msg, event.message.mp1, event.message.mp2));
        else
                return (S_UNKNOWN);
}

void UI_WINDOW_OBJECT::RegisterObject(char *name, PSZ baseName,
        PFNWP *baseCallback, ZIL_ICHAR *title, void *controlData, ZIL_SCREENID parentID)
{
        // Make sure the field needs to be registered.
        if (screenID)
        {
                if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                {
                        // Set trueRegion coordinates.
                        SWP swp;
                        SWP parentSwp;
                        ZIL_SCREENID frameID;
                        Information(I_GET_FRAMEID, &frameID);
                        WinQueryWindowPos(frameID, &swp);
                        WinQueryWindowPos(parent ? parent->screenID : HWND_DESKTOP, &parentSwp);
                        trueRegion.left = swp.x;
                        trueRegion.top = parentSwp.cy - swp.y - swp.cy;
                        trueRegion.right = swp.x + swp.cx - 1;
                        trueRegion.bottom = parentSwp.cy - swp.y - 1;
                }
                return;
        }

        if (!*baseCallback)
        {
                if (baseName)
                {
                        CLASSINFO classInfo;
                        WinQueryClassInfo(display->hab, baseName, &classInfo);
                        *baseCallback = classInfo.pfnWindowProc;
                        WinRegisterClass(display->hab, (PSZ)name, ObjectInitProcedure,
                                classInfo.flClassStyle & ~CS_PUBLIC | CS_SIZEREDRAW,
                                classInfo.cbWindowData);
                }
                else
                {
                        WinRegisterClass(display->hab, (PSZ)name, ObjectInitProcedure,
                                CS_MOVENOTIFY | CS_SIZEREDRAW, 4);
                        *baseCallback = WinDefWindowProc;
                }
        }
        defaultCallback = *baseCallback;

        // Get a handle to the parent.
        if (!parentID)
        {
                if (parent)
                {
                        if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
                                parent->Information(I_GET_FRAMEID, &parentID);
                        else
                                parentID = parent->screenID;

                        if (!parentID)
                                parentID = HWND_DESKTOP;
                }
                else
                        parentID = HWND_DESKTOP;
        }

        // Compute the window region.
        SWP swp;
        WinQueryWindowPos(parentID, &swp);
        int left = trueRegion.left;
        int bottom = swp.cy - trueRegion.bottom - 1;
        int width = trueRegion.Width();
        int height = trueRegion.Height();

#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
        char *TEXT_ = MapText(title);
#else
#       define TEXT_ title
#endif

        _initObject = this;
        woStatus |= WOS_INTERNAL_ACTION;
        if (parentID == HWND_DESKTOP && !parent)
        {
                if (Inherited(ID_POP_UP_MENU))
                {
                        screenID = WinCreateWindow(windowManager->taskID, (PSZ)name, (PSZ)TEXT_,
                                0, left, bottom, 0, 0, windowManager->taskID, HWND_TOP, FID_MENU,
                                ZIL_NULLP(void), ZIL_NULLP(void));
                }
                else
                {
//                      static int taskList = TRUE;
//                      if (taskList)
//                      {
                                flFlag |= FCF_TASKLIST;
//                              taskList = FALSE;
//                      }

                        FRAMECDATA frame;
                        frame.cb = sizeof(FRAMECDATA);
                        frame.flCreateFlags = flFlag;
                        frame.hmodResources = 0;
                        frame.idResources = 0;

                        ZIL_SCREENID previousID;
                        if (previous)
                                Previous()->Information(I_GET_FRAMEID, &previousID);
                        else
                                previousID = HWND_TOP;
                        screenID = WinCreateWindow(HWND_DESKTOP, (PSZ)name, (PSZ)TEXT_,
                                0, left, bottom, width, height, 0, previousID, 0, &frame,
                                ZIL_NULLP(void));
                        woStatus |= WOS_SYSTEM_OBJECT;
                }
        }
        else if (baseName == WC_FRAME)
        {
                FRAMECDATA frame;
                frame.cb = sizeof(FRAMECDATA);
                frame.flCreateFlags = flFlag;
                frame.hmodResources = 0;
                frame.idResources = numberID;
                screenID = WinCreateWindow(parentID, (PSZ)name, (PSZ)TEXT_,
                        flStyle, left, bottom, width, height, parentID,
                        HWND_TOP, numberID, &frame, ZIL_NULLP(void));
                woStatus |= WOS_SYSTEM_OBJECT;
        }
        else
        {
                screenID = WinCreateWindow(parentID, (PSZ)name, (PSZ)TEXT_,
//                      flStyle | WS_CLIPSIBLINGS, left, bottom, width, height, parentID,
                        flStyle, left, bottom, width, height, parentID,
                        HWND_BOTTOM, numberID, controlData, ZIL_NULLP(void));
                // Fixup for OS/2 create/size "feature".
                woStatus |= WOS_SYSTEM_OBJECT;
                WinSetWindowPos(screenID, 0, left, bottom, width, height,
                        SWP_MOVE | SWP_SIZE | SWP_NOADJUST | SWP_NOREDRAW);
        }
        woStatus &= ~WOS_INTERNAL_ACTION;

#if defined (ZIL_UNICODE) || defined(ZIL_ISO8859_1)
        delete TEXT_;
#endif

        if ((font & 0x0FFF) < ZIL_MAXFONTS && UI_OS2_DISPLAY::fontTable[font & 0x0FFF].lMatch)
        {
                char stringFont[32];
                ::sprintf(stringFont, "%d.%s", UI_OS2_DISPLAY::fontTable[font & 0x0FFF].sNominalPointSize / 10,
                        UI_OS2_DISPLAY::fontTable[font & 0x0FFF].szFacename);
                ULONG length = ::strlen(stringFont) + 1;
                WinSetPresParam(screenID, PP_FONTNAMESIZE, length, stringFont);
        }

        _initObject = ZIL_NULLP(UI_WINDOW_OBJECT);
        return;
}

// ----- OS Specific Functions ----------------------------------------------

void UI_WINDOW_OBJECT::OSInitialize(void)
{
        flStyle = WS_VISIBLE;
        flFlag = 0;
        defaultCallback = ZIL_NULLF(PFNWP);
}

void UI_WINDOW_OBJECT::OSUpdateSettings(ZIL_OBJECTID)
{
        if (screenID && FlagSet(woStatus, WOS_SYSTEM_OBJECT))
        {
                WOS_STATUS saveStatus = woStatus;
                ZIL_SCREENID frameID;
                Information(I_GET_FRAMEID, &frameID);
                WinEnableWindowUpdate(frameID, FALSE);
                UI_EVENT event(S_DEINITIALIZE);
                event.windowObject = this;
                Event(event);
                flStyle = WS_VISIBLE;
                flFlag = 0;
                woStatus = saveStatus | WOS_REDISPLAY;
        }
}
