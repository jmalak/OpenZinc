//      Zinc Interface Library - D_ICON.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"

// ----- UIW_ICON -----------------------------------------------------------

EVENT_TYPE UIW_ICON::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
        ZIL_SCREENID screenID = this->screenID;
        UI_DISPLAY *display = this->display;
        UI_REGION clip = this->clip;
        UI_PALETTE *lastPalette = this->lastPalette;
#endif
        UI_REGION region = trueRegion;

        /* unused parameters */ (void)event;

        if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && parent && parent->parent)
        {
                UI_WINDOW_OBJECT *currentObject;
                if (FlagSet(parent->parent->woAdvancedFlags, WOAF_MDI_OBJECT))
                        parent->parent->Information(I_GET_LAST, &currentObject);
                else
                        parent->parent->Information(I_GET_CURRENT, &currentObject);
                if (parent != currentObject)
                        lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_ANY, ID_WINDOW);
                else
                        lastPalette = LogicalPalette(ccode);
        }

        // Check for text mode operation.
        if (display->isText && title)
        {
                DrawText(screenID, region, title, lastPalette, TRUE, ccode);
                return (ccode);
        }

        // Draw the icon.
#if !defined(ZIL_TEXT_ONLY)
        int listItem = parent->Inherited(ID_LIST);
        int statusItem = Inherited(ID_STATUS_ITEM);
        if (!listItem && !statusItem)
        {
                region = iconRegion;
                if (FlagSet(woFlags, WOF_BORDER))
                        DrawBorder(screenID, region, FALSE, ccode);
                if (iconArray || icon.colorBitmap)
                {
                        if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && parent && parent->parent)
                                display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
                        else if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                                display->Rectangle(screenID, region, display->backgroundPalette, 0, TRUE, FALSE, &clip);
                        display->Bitmap(screenID, region.left, region.top, iconWidth,
                                iconHeight, iconArray, ZIL_NULLP(UI_PALETTE),
                                &clip, &icon.colorBitmap, &icon.monoBitmap);
                }
                else
                        display->Rectangle(screenID, region, lastPalette,
                                FlagSet(icFlags, ICF_MINIMIZE_OBJECT) ? 1 : 0, TRUE, FALSE, &clip);
                if (title)
                {
                        region = titleRegion;
                        DrawText(screenID, region, title, lastPalette, TRUE, ccode);
                }
                return (ccode);
        }

        // Draw the list or status item.
        int iconOffset = listItem ? 4 : 0;
        if (parent->Inherited(ID_COMBO_BOX))
                if (ccode == S_CURRENT)
                        woStatus |= WOS_SELECTED;
                else if (ccode == S_NON_CURRENT)
                        woStatus &= ~WOS_SELECTED;

        lastPalette = LogicalPalette(ccode);

        // Increase region so border doesn't touch bitmap.
        if (Inherited(ID_STATUS_ITEM))
                region.right++;

        display->VirtualGet(screenID, region);
        if (FlagSet(woFlags, WOF_BORDER))
                DrawBorder(screenID, region, FALSE, ccode);
        display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);

        // Compute the draw region.
        region.left += iconOffset;
        if (parent->Inherited(ID_VT_LIST))
                region.left += relative.left;
        if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
                region.right -= iconOffset;
        int fieldWidth = region.Width();
        int fieldHeight = region.Height();
        int left = region.left;
        if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
                left = region.right - iconWidth + 1;
        else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
                left = region.left + (fieldWidth - iconWidth) / 2;
        int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && title) ?
                region.top + 2 : region.top + (fieldHeight - iconHeight) / 2;

        // Draw the icon.
        if (iconArray || icon.colorBitmap)
                display->Bitmap(screenID, left, top, iconWidth, iconHeight,
                        iconArray, ZIL_NULLP(UI_PALETTE), &clip, &icon.colorBitmap, &icon.monoBitmap);

        // Draw the title.
        if (title)
        {
                WOF_FLAGS flags = woFlags;
                if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
                        region.top = region.bottom - display->cellHeight + 4 + display->preSpace + display->postSpace;
                else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
                {
                        region.right -= iconWidth - iconOffset;
                        woFlags &= ~(WOF_JUSTIFY_RIGHT);
                }
                else
                        region.left += iconWidth + iconOffset;
                region.bottom--;
                DrawText(screenID, region, title, lastPalette, FALSE, ccode);
                woFlags = flags;
        }

        // Draw Focus rectangle if current list item.
        if (Inherited(ID_LIST_ITEM) && ccode == S_CURRENT && !display->isText)
        {
                UI_REGION focus = trueRegion;
                if (parent->Inherited(ID_COMBO_BOX) && !FlagSet(parent->woAdvancedFlags, WOAF_TEMPORARY))
                        --focus;
                DrawFocus(screenID, focus, ccode);
        }

        // Restore the display.
        display->VirtualPut(screenID);

        // Return the control code.
#if defined(ZIL_OPTIMIZE)
        this->lastPalette = lastPalette;
#endif
#endif

        return (ccode);
}

EVENT_TYPE UIW_ICON::Event(const UI_EVENT &event)
{
        static int initializedTime = FALSE;
        static ZIL_UTIME lastTime, currentTime;
        static UI_REGION mouseRegion;
        if (!initializedTime)
        {
                lastTime.Import();
                initializedTime = TRUE;
        }

        // Switch on the event type.
        EVENT_TYPE ccode = LogicalEvent(event, ID_ICON);
        switch (ccode)
        {
        case S_INITIALIZE:
                woStatus &= ~WOS_OWNERDRAW;
                iconHeight = iconWidth = 32;
                if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
                        icFlags |= ICF_MINIMIZE_OBJECT;
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                {
                        woFlags |= WOF_JUSTIFY_CENTER;
                        if (!display->isText && !FlagSet(woStatus, WOS_GRAPHICS))
                        {
                                relative.left = relative.left * display->cellWidth;
                                relative.top = relative.top * display->cellHeight;
                                woStatus |= WOS_GRAPHICS;
                        }
                        parent->Information(I_SET_ICON, this);
                }
                else
                        UI_WINDOW_OBJECT::Event(event);
                break;

        case S_CHANGED:
        case S_CREATE:
                if (display->isText)
                {
                        relative.bottom = relative.top;
                        if (!Inherited(ID_LIST_ITEM))
                                relative.right = relative.left + (title ? display->TextWidth(title) - 1 : 0);
                }
#if !defined(ZIL_TEXT_ONLY)
                else if (Inherited(ID_LIST_ITEM))
                {
                        relative.bottom = relative.top + iconHeight + 2;
                        if (title && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
                        {
                                relative.bottom += display->cellHeight / 4;
                                relative.bottom += display->TextHeight(title, screenID, font);
                        }
                }
                else
                {
                        relative.right = relative.left + iconWidth + 2;
                        relative.bottom = relative.top + iconHeight + 2;
                        if (title)
                        {
                                relative.bottom += display->cellHeight / 4;
                                relative.bottom += display->TextHeight(title, screenID, font);
                                int width = display->TextWidth(title, screenID, font);
                                if (width > iconWidth)
                                        relative.right = relative.left + width + display->cellWidth;
                        }
                }
#endif
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                {
                        screenID = parent->screenID;
                        RegionConvert(relative, TRUE);
                        trueRegion.left = clip.left + relative.left;
                        trueRegion.top = clip.top + relative.top;
                        trueRegion.right = trueRegion.left + relative.Width() - 1;
                        trueRegion.bottom = trueRegion.top + relative.Height() - 1;
                }
                else
                        ccode = UI_WINDOW_OBJECT::Event(event);

#if !defined(ZIL_TEXT_ONLY)
                if (!display->isText)
                {
                        iconRegion.left = (trueRegion.right - trueRegion.left > iconWidth) ?
                                (trueRegion.left + trueRegion.right - iconWidth) / 2 : trueRegion.left;
                        iconRegion.top = trueRegion.top;
                        iconRegion.right = iconRegion.left + iconWidth - 1;
                        iconRegion.bottom = iconRegion.top + iconHeight - 1;
                        if (title)
                        {
                                titleRegion = trueRegion;
                                titleRegion.top = titleRegion.bottom - display->TextHeight(title, screenID, font);
                        }
                }
                if (iconArray && !icon.colorBitmap && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
                {
                        display->IconArrayToHandle(screenID, iconWidth, iconHeight,
                                iconArray, ZIL_NULLP(UI_PALETTE), &icon);
                        if (icon.colorBitmap)
                        {
                                delete iconArray;
                                iconArray = ZIL_NULLP(ZIL_UINT8);
                        }
                }
#endif
                break;

        case S_RESET_DISPLAY:
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && !display->isText &&
                        FlagSet(woStatus, WOS_GRAPHICS))
                {
                        UI_REGION region = relative;
                        region.left /= display->cellWidth;
                        region.top /= display->cellHeight;
                        region.right = region.left + strlen(title);
                        region.bottom = region.top;
                        woStatus &= ~WOS_GRAPHICS;
                        woStatus |= WOS_REDISPLAY;
                        relative = region;
                }
                else
                        ccode = UI_WINDOW_OBJECT::Event(event);
                break;

        case S_REGION_DEFINE:
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                {
                        UI_REGION region = (display->isText ? trueRegion : iconRegion);
                        region.left = MaxValue(region.left, clip.left);
                        region.top = MaxValue(region.top, clip.top);
                        region.right = MinValue(region.right, clip.right);
                        region.bottom = MinValue(region.bottom, clip.bottom);
                        display->RegionDefine(screenID, region);
                        if (title && !display->isText)
                        {
                                region.left = MaxValue(titleRegion.left, clip.left);
                                region.top = MaxValue(titleRegion.top, clip.top);
                                region.right = MinValue(titleRegion.right, clip.right);
                                region.bottom = MinValue(titleRegion.bottom, clip.bottom);
                                display->RegionDefine(screenID, region);
                        }
                }
                break;

        case S_CURRENT:
        case S_NON_CURRENT:
        case S_DISPLAY_ACTIVE:
        case S_DISPLAY_INACTIVE:
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                {
                        if (FlagSet(parent->woStatus, WOS_MINIMIZED))
                                woFlags &= ~WOF_NON_SELECTABLE;
                        else
                                woFlags |= WOF_NON_SELECTABLE;
                }
                UI_WINDOW_OBJECT::Event(event);
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && !FlagSet(parent->woStatus, WOS_MINIMIZED))
                        woStatus &= ~WOS_REDISPLAY;
                else if (FlagSet(woStatus, WOS_REDISPLAY))
                {
                        DrawItem(event, ccode);
                        woStatus &= ~WOS_REDISPLAY;
                }
                break;

        case L_BEGIN_SELECT:
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && ((display->isText &&
                        trueRegion.Overlap(event.position)) || (!display->isText &&
                        (iconRegion.Overlap(event.position) || titleRegion.Overlap(event.position)))))
                {
                        // Make sure system menu is closed.
                        if (!FlagSet(parent->woAdvancedFlags, WOAF_TEMPORARY))
                                windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
                        currentTime.Import();
                        if (currentTime.Difference(lastTime) < doubleClickRate + 25 &&
                                mouseRegion == trueRegion)
                                parent->Event(UI_EVENT(L_RESTORE, 0x0001));
                        else
                        {
                                lastTime = currentTime;
                                mouseRegion = trueRegion;
                                if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE))
                                        parent->Event(UI_EVENT(L_MOVE, 0x0001, event.position));
                        }
                }
                else if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                        return (S_UNKNOWN);
                else
                        ccode = UI_WINDOW_OBJECT::Event(event);
                break;

        case L_END_SELECT:
#if defined(ZIL_EDIT)
                if (FlagSet(woStatus, WOS_EDIT_MODE))
                        return (UI_WINDOW_OBJECT::Event(event));
#endif
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && mouseRegion == trueRegion)
                {
                        // Let the window display it's system menu.
                        UI_EVENT tEvent(E_KEY, 0x3920);       // SPACEBAR
                        tEvent.key.shiftState |= S_ALT;
                        tEvent.key.value = ' ';
                        parent->Event(tEvent);
                }
                if ((display->isText && trueRegion.Overlap(event.position)) || (!display->isText &&
                        (iconRegion.Overlap(event.position) || titleRegion.Overlap(event.position))))
                {
                        currentTime.Import();
                        if (FlagSet(icFlags, ICF_DOUBLE_CLICK &&
                                currentTime.Difference(lastTime) < doubleClickRate))
                        {
                                UI_EVENT uEvent = event;
                                uEvent.type = L_DOUBLE_CLICK;
                                Event(uEvent);
                        }
                        else
                        {
                                ccode = UserFunction(UI_EVENT(L_END_SELECT), L_SELECT);
                                if (!FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                                        lastTime = currentTime;
                        }
                }
                break;

        case L_UP:
        case L_DOWN:
        case L_LEFT:
        case L_RIGHT:
                if (!FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                        ccode = UI_WINDOW_OBJECT::Event(event);
                break;

        case L_SELECT:
                if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
                {
                        UI_EVENT tEvent(L_RESTORE);
                        tEvent.data = parent;
                        parent->Event(tEvent);
                }
                else
                        UI_WINDOW_OBJECT::Event(event);
                break;

        default:
                ccode = UI_WINDOW_OBJECT::Event(event);
                break;
        }

        // Return the control code.
        return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_ICON::OSDataSet(void)
{
        // Redisplay the string.
        if (screenID)
        {
                if (!display->isText && title)
                {
#if !defined(ZIL_TEXT_ONLY)
                        titleRegion.bottom = trueRegion.bottom;
                        titleRegion.top = titleRegion.bottom - display->TextHeight(title, screenID, font);
                        int center = (trueRegion.left + trueRegion.right) /2;
                        int width = display->TextWidth(title, screenID, font);
                        titleRegion.left = center - width/2 ;
                        titleRegion.right = center + width/2;
                        trueRegion.left = MinValue(titleRegion.left, trueRegion.left);
                        trueRegion.right = MaxValue(titleRegion.right, trueRegion.right);
#endif
                }
                Event(UI_EVENT(S_REDISPLAY));
        }
}

void UIW_ICON::OSImageInitialize(void)
{
        if (iconArray && !icon.colorBitmap)
        {
                display->IconArrayToHandle(screenID, iconWidth, iconHeight,
                        iconArray, ZIL_NULLP(UI_PALETTE), &icon);
                if (icon.colorBitmap)
                {
                        delete iconArray;
                        iconArray = ZIL_NULLP(ZIL_UINT8);
                }
        }
}

void UIW_ICON::OSSystemImage(void)
{
}

void UIW_ICON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
        // See if the field needs to be re-computed.
        if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && parent)
                parent->Information(I_SET_ICON, &icon);
        else if (objectID == ID_ICON && FlagSet(woStatus, WOS_REDISPLAY))
        {
                UI_EVENT event(S_INITIALIZE, 0);
                Event(event);
                event.type = S_CREATE;
                Event(event);
                event.type = S_REDISPLAY;
                Event(event);
        }
}
