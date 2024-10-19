//      Zinc Interface Library - D_SYS.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

EVENT_TYPE UIW_SYSTEM_BUTTON::Event(const UI_EVENT &event)
{
        static int initializedTime = FALSE;
        static ZIL_UTIME lastTime, currentTime;
        if (!initializedTime)
        {
                lastTime.Import();
                initializedTime = TRUE;
        }

        // Switch on the event type.
        EVENT_TYPE ccode = LogicalEvent(event, ID_SYSTEM_BUTTON);
        switch (ccode)
        {
        case S_INITIALIZE:
                // Initialize the generic system button.
                if (FlagSet(syFlags, SYF_GENERIC) && !systemMenu)
                {
                        systemMenu = new UIW_POP_UP_MENU(0, 0, WNF_NO_FLAGS);
                        // Add the pop-up menu items.
                        for (int i = 0; i < myLanguage->noOfElements; i++)
                                systemMenu->Add(new UIW_POP_UP_ITEM(myLanguage->data[i].text, myLanguage->data[i].numberID));
                        systemMenu->woAdvancedFlags |= WOAF_TEMPORARY | WOAF_NO_DESTROY;
                }
                UIW_BUTTON::Event(event);
                break;

        case S_DEINITIALIZE:
                // Check the status of the DOS generic menu.
                if (FlagSet(syFlags, SYF_GENERIC) && systemMenu)
                {
                        delete systemMenu;
                        systemMenu = ZIL_NULLP(UIW_POP_UP_MENU);
                }
                UIW_BUTTON::Event(event);
                break;

        case S_CHANGED:
        case S_CREATE:
                UI_WINDOW_OBJECT::Event(event);
                if (display->isText)
                {
#if !defined(ZIL_GRAPHICS_ONLY)
                        bitmapArray = ZIL_NULLP(ZIL_UINT8);
                        bitmapWidth = bitmapHeight = 0;
                        text = myDecorations->GetText(ZIL_SysButton);
#       if defined(ZIL_SHADOW_BORDER)
                        trueRegion.bottom = trueRegion.top;
                        trueRegion.right = trueRegion.left + display->TextWidth(text, screenID, font) - 1;
#       elif defined(ZIL_STANDARD_BORDER) || defined(ZIL_3D_BORDER)
                        trueRegion.top = trueRegion.bottom = parent->trueRegion.top;
                        trueRegion.left = parent->trueRegion.left;
                        for (UI_WINDOW_OBJECT *object = Previous(); object; object = object->Previous())
                                if (object->Inherited(ID_BORDER))
                                        trueRegion.left++;
                        clip = trueRegion;
                        trueRegion.right = trueRegion.left + 2;
#       endif
#endif
                }
                else
                {
#if !defined(ZIL_TEXT_ONLY)
                        text = ZIL_NULLP(ZIL_ICHAR);
                        ZIL_UINT8 *bitmap = myDecorations->GetBitmap(ZIL_SysButton);
                        bitmapWidth = bitmap[0];
                        bitmapHeight = bitmap[1];
                        bitmapArray = &bitmap[2];
#       if defined(ZIL_MSWINDOWS_STYLE)
                        trueRegion.bottom = --trueRegion.top + (display->cellHeight - display->preSpace - display->postSpace);
                        trueRegion.right = --trueRegion.left + (display->cellHeight - display->preSpace - display->postSpace);
#       elif defined(ZIL_OS2_STYLE)
                        // OS/2 style does not overlap parent region.
                        trueRegion.bottom = trueRegion.top - 1 + (display->cellHeight - display->preSpace - display->postSpace);
                        trueRegion.right = trueRegion.left - 1 + (display->cellHeight - display->preSpace - display->postSpace);
#       elif defined(ZIL_MOTIF_STYLE)
                        // Motif clip region overlaps parent for shadowing.
                        trueRegion.bottom = --trueRegion.top + (display->cellHeight - display->preSpace - display->postSpace);
                        trueRegion.right = --trueRegion.left + (display->cellHeight - display->preSpace - display->postSpace);
                        clip = trueRegion;
#       endif
#endif
                }
                break;

        case S_RESET_DISPLAY:
                ccode = UIW_BUTTON::Event(event);
                menu.Event(event);
                break;

        case S_ADD_OBJECT:
        case S_SUBTRACT_OBJECT:
                menu.Event(event);
                break;

        case L_CONTINUE_SELECT:
                return ccode;

        case L_BEGIN_SELECT:
                {
                currentTime.Import();
                if (currentTime.Difference(lastTime) < doubleClickRate + 25)
                {
                        UI_EVENT tEvent = event;
                        while (ccode != L_END_SELECT)
                        {
                                eventManager->Get(tEvent);
                                ccode = LogicalEvent(tEvent, ID_SYSTEM_BUTTON);
                                // Send all user messages to the window manager.
                                if (tEvent.type > 9999)
                                        windowManager->Event(tEvent);
                        }

                        UIW_POP_UP_MENU *sMenu = FlagSet(syFlags, SYF_GENERIC) ? systemMenu : &menu;
                        int canClose = FALSE;

                        for (UI_WINDOW_OBJECT *object = sMenu->First(); object; object = object->Next())
                                if (object->NumberID() == NUMID_OPT_CLOSE)
                                        canClose = TRUE;

                        if (parent->parent && canClose)
                        {
                                windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
                                eventManager->Put(UI_EVENT(S_CLOSE + S_MDICHILD_EVENT));
                        }
                        else if (!FlagSet(parent->woAdvancedFlags, WOAF_LOCKED) && canClose)
                        {
                                windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
                                eventManager->Put(UI_EVENT(S_CLOSE));
                        }
                        break;
                }
                lastTime = currentTime;
                }
                // Continue to L_SELECT.
        case L_SELECT:
                if (FlagSet(syFlags, SYF_GENERIC) || menu.First())
                {
                        UIW_POP_UP_MENU *sMenu = FlagSet(syFlags, SYF_GENERIC) ? systemMenu : &menu;
                        if (windowManager->Index(sMenu) != -1)
                                break;
                        sMenu->parent = this;
                        sMenu->RegionConvert(sMenu->relative, TRUE);
                        if (FlagSet(parent->woStatus, WOS_MINIMIZED))
                        {
                                sMenu->relative.left = parent->trueRegion.left;
                                sMenu->relative.top = parent->trueRegion.bottom;
                        }
                        else
                        {
                                sMenu->relative.left = trueRegion.left;
                                sMenu->relative.top = trueRegion.bottom;
                        }
                        if (display->isText)
                                sMenu->relative.top++;
                        if (sMenu->Current())
                        {
                                sMenu->Current()->woStatus &= ~WOS_CURRENT;
                                sMenu->SetCurrent(ZIL_NULLP(UI_ELEMENT));
                        }
                        sMenu->trueRegion = sMenu->relative;
                        for (UI_WINDOW_OBJECT *object = sMenu->First(); object; object = object->Next())
                        {
                                switch (object->NumberID())
                                {
                                case NUMID_OPT_RESTORE:
                                        if (FlagSet(parent->woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
                                                object->woFlags &= ~WOF_NON_SELECTABLE;
                                        else
                                                object->woFlags |= WOF_NON_SELECTABLE;
                                        break;

                                case NUMID_OPT_MOVE:
                                        if (FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE) ||
                                                FlagSet(parent->woStatus, WOS_MAXIMIZED))
                                                object->woFlags |= WOF_NON_SELECTABLE;
                                        else
                                                object->woFlags &= ~WOF_NON_SELECTABLE;
                                        break;

                                case NUMID_OPT_SIZE:
                                        if (FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE) ||
                                                FlagSet(parent->woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
                                                object->woFlags |= WOF_NON_SELECTABLE;
                                        else
                                                object->woFlags &= ~WOF_NON_SELECTABLE;
                                        break;

                                case NUMID_OPT_MINIMIZE:
                                case NUMID_OPT_MAXIMIZE:
                                        {
                                        object->woFlags &= ~WOF_NON_SELECTABLE;
                                        if ((FlagSet(parent->woStatus, WOS_MINIMIZED) && object->NumberID() == NUMID_OPT_MINIMIZE) ||
                                                (FlagSet(parent->woStatus, WOS_MAXIMIZED) && object->NumberID() == NUMID_OPT_MAXIMIZE))
                                        {
                                                object->woFlags |= WOF_NON_SELECTABLE;
                                                break;
                                        }
                                        NUMBERID numID = object->NumberID() == NUMID_OPT_MINIMIZE ? NUMID_MINIMIZE : NUMID_MAXIMIZE;
                                        UI_WINDOW_OBJECT *pobject = parent->Get(numID);
                                        if (!pobject)
                                                object->woFlags |= WOF_NON_SELECTABLE;
                                        }
                                        break;

                                case NUMID_OPT_CLOSE:
                                        if (FlagSet(parent->woAdvancedFlags, WOAF_LOCKED))
                                                object->woFlags |= WOF_NON_SELECTABLE;
                                        else
                                                object->woFlags &= ~WOF_NON_SELECTABLE;
                                        break;
                                }
                                
                                if (!sMenu->Current() && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
                                {
                                        object->woStatus |= WOS_CURRENT;
                                        sMenu->SetCurrent(object);
                                }
                        }
                        *windowManager + sMenu;
                }
                else
                        UIW_BUTTON::Event(event);
                break;

        default:
                ccode = UIW_BUTTON::Event(event);
                break;
        }

        // Return the control code.
        return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_SYSTEM_BUTTON::OSUpdateSettings(ZIL_OBJECTID)
{
        // No code necessary.
}
