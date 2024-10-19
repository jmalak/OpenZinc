//      Zinc Interface Library - D_ERROR.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

UIS_STATUS UI_ERROR_SYSTEM::ErrorMessage(UI_WINDOW_MANAGER *windowManager,
        UIS_STATUS errorStatus, ZIL_ICHAR *message, ZIL_ICHAR *titleMessage)
{
        // Beep the system.
        Beep();
        if (!windowManager || !message)
                return (WOS_NO_STATUS);

        const int ERROR_OK = 9900;
        const int ERROR_CANCEL = 9901;

        UIW_BORDER *border = new UIW_BORDER;
        UIW_SYSTEM_BUTTON *sysButton = new UIW_SYSTEM_BUTTON;
        sysButton->Add(new UIW_POP_UP_ITEM(myLanguage->GetMessage(MSG_MOVE), MNIF_MOVE));
        sysButton->Add(new UIW_POP_UP_ITEM(myLanguage->GetMessage(MSG_CLOSE), MNIF_CLOSE));
        UIW_TITLE *title = new UIW_TITLE(titleMessage ? titleMessage : myLanguage->GetMessage(MSG_TITLE));
        extern UI_PALETTE_MAP *_errorPaletteMapTable;
        border->paletteMapTable = sysButton->paletteMapTable =
                title->paletteMapTable = _errorPaletteMapTable;

        UI_DISPLAY *display = windowManager->display;
#if defined(ZIL_3D_BORDER)
        int width = 43;
        int height = 8;
        const int TEXT_WIDTH = 36;
#else
        int width = 43;
        int height = 6;
        const int TEXT_WIDTH = 38;
#endif
        ZIL_ICHAR *text = message;
        int textWidth = display->isText ? TEXT_WIDTH : 30 * display->cellWidth;
        int textHeight = display->TextWidth(text, windowManager->screenID, windowManager->font) / textWidth + 1;
        if (display->isText)
                height += textHeight;
        else
                height += (textHeight * (display->TextHeight(message) + display->preSpace + display->postSpace) / display->cellHeight) + 1;

        // Check for newLine characters.
        for (ZIL_ICHAR *newLine = ZIL_INTERNATIONAL::strchr(text, '\n'); newLine;
                        newLine = ZIL_INTERNATIONAL::strchr(++newLine, '\n'))
                height++;
        
        int left = (display->columns / display->cellWidth - width) / 2;
        int top = (display->lines / display->cellHeight - height) / 2;
        UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height, WOF_NO_FLAGS, WOAF_MODAL | WOAF_NO_SIZE | WOAF_NO_DESTROY);
        *window + border + sysButton + title;
        if (display->isText)
        {
                UIW_TEXT *text = new UIW_TEXT(2, 1, TEXT_WIDTH, height - 6, message, -1, WNF_NO_FLAGS, WOF_VIEW_ONLY);
                window->paletteMapTable = text->paletteMapTable = _errorPaletteMapTable;
                *window + text;
                text->woAdvancedFlags |= WOAF_NON_CURRENT;
        }
        else
        {
#if !defined(ZIL_TEXT_ONLY)
                UIW_TEXT *text = new UIW_TEXT(8, 1, 30, height - 5, message, -1, WNF_NO_FLAGS, WOF_VIEW_ONLY);
                *window
                        + new UIW_ICON(2, 1, myLanguage->GetMessage(MSG_ICONNAME))
                        + text;
                text->woAdvancedFlags |= WOAF_NON_CURRENT;
#endif
        }
        if (FlagSet(errorStatus, WOS_INVALID))
                *window
                        + new UIW_BUTTON(8, height - 3, 12, myLanguage->GetMessage(MSG_OK), BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
                                WOF_JUSTIFY_CENTER, ZIL_NULLF(ZIL_USER_FUNCTION), ERROR_OK)
                        + new UIW_BUTTON(24, height - 3, 12, myLanguage->GetMessage(MSG_CANCEL), BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
                                WOF_JUSTIFY_CENTER, ZIL_NULLF(ZIL_USER_FUNCTION), ERROR_CANCEL);
        else
                *window
                        + new UIW_BUTTON(15, height - 3, 12, myLanguage->GetMessage(MSG_OK), BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
                                WOF_JUSTIFY_CENTER, ZIL_NULLF(ZIL_USER_FUNCTION), ERROR_OK);
        *windowManager + window;

        // Make sure that all VPuts are flushed before going modal
        display->VirtualPut(0);

        // Run the window in a modal fashion.
        UI_EVENT_MANAGER *eventManager = windowManager->eventManager;
        UI_EVENT event;
        EVENT_TYPE ccode = 0;
        do
        {
                eventManager->Get(event, Q_NORMAL);
                ccode = event.type;
                if (ccode == ERROR_OK || ccode == ERROR_CANCEL)
                        *windowManager - window;
                else
                        ccode = windowManager->Event(event);
        } while (ccode != ERROR_OK && windowManager->Index(window) != -1);
        *windowManager - window;
        delete window;
        if (FlagSet(errorStatus, WOS_INVALID))
                return ((ccode == ERROR_OK) ? WOS_INVALID : WOS_NO_STATUS);
        else
                return (WOS_NO_STATUS);
}
