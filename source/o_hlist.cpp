//      Zinc Interface Library - O_HLIST.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINWINDOWMGR               // OS/2 window manager.
#define INCL_WINSCROLLBARS
#include "ui_win.hpp"

// ----- UIW_HZ_LIST --------------------------------------------------------

EVENT_TYPE UIW_HZ_LIST::Event(const UI_EVENT &event)
{
        // OS/2 specific messages.
        EVENT_TYPE ccode = event.type;
        if (ccode == E_OS2)
        {
                if (event.message.hwnd != screenID)
                        return (UIW_WINDOW::Event(event));

                int processed = TRUE;
                switch (event.message.msg)
                {
                case WM_CHAR:
                case WM_BUTTON1DOWN:
                case WM_BUTTON1UP:
                case WM_BUTTON1DBLCLK:
                case WM_MOUSEMOVE:
                        processed = FALSE;
                        break;

                case WM_PAINT:
                        if (!FlagSet(woStatus, WOS_OWNERDRAW) || !DrawItem(event, FlagSet(woStatus, WOS_CURRENT) ?
                                        S_CURRENT : S_DISPLAY_ACTIVE))
                        {
                                RECTL windowRect;
                                RECTL updateRect;
                                WinQueryWindowRect(screenID, &windowRect);
                                UI_OS2_DISPLAY::hps = WinBeginPaint(screenID, ZIL_NULLH(HPS), &updateRect);
                                display->VirtualGet(ID_DIRECT, 0, 0, windowRect.xRight - windowRect.xLeft - 1,
                                        windowRect.yTop - windowRect.yBottom - 1);

                                UI_REGION updateRegion;
                                int windowHeight = windowRect.yTop - windowRect.yBottom;
                                updateRegion.left = updateRect.xLeft;
                                updateRegion.top = windowHeight - updateRect.yTop;
                                updateRegion.right = updateRect.xRight;
                                updateRegion.bottom = windowHeight - updateRect.yBottom;

                                if (topObject)
                                {
                                        UI_REGION backgroundRegion;
                                        backgroundRegion.left = backgroundRegion.top = 0;
                                        backgroundRegion.right = scroll.left + topObject->relative.right;
                                        backgroundRegion.bottom = windowHeight - 1;

                                        // Draw the Children.
                                        UI_WINDOW_OBJECT *object;
                                        for (object = topObject; object; object = object->Next())
                                        {
                                                // Calculate Regions.
                                                UI_REGION region;
                                                region.left = scroll.left + object->relative.left;
                                                region.top = object->relative.top;
                                                region.right = scroll.left + object->relative.right;
                                                region.bottom = object->relative.bottom;
                                                object->trueRegion = region;

                                                if (region.left > backgroundRegion.left)
                                                {
                                                        if (backgroundRegion.Overlap(updateRegion))
                                                                display->Rectangle(screenID, backgroundRegion, lastPalette, 0, TRUE);
                                                        backgroundRegion.left = region.left;
                                                        backgroundRegion.right = region.right;
                                                }
                                                backgroundRegion.top = region.bottom + 1;

                                                // Test for completion.
                                                if (region.left > updateRegion.right)
                                                        break;
                                                else if (region.Overlap(updateRegion))
                                                {
                                                        // Draw the child.
                                                        if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
                                                        {
                                                                UI_EVENT os2Event;
                                                                os2Event.type = E_OS2;
                                                                os2Event.message.hwnd = object->screenID;
                                                                os2Event.message.msg = WM_DRAWITEM;
                                                                os2Event.message.mp1 = this;
                                                                os2Event.message.mp2 = object;
                                                                object->Event(os2Event);
                                                        }
                                                        else
                                                        {
                                                                // Draw the text.
                                                                UI_REGION drawRegion = region;
                                                                ZIL_ICHAR *text = (ZIL_ICHAR *)object->Information(I_GET_TEXT, ZIL_NULLP(void));
                                                                DrawText(screenID, drawRegion, text, object->LogicalPalette(S_DISPLAY_ACTIVE),
                                                                        TRUE, S_DISPLAY_ACTIVE);

                                                                // Draw Focus Rectangle.
                                                                if (FlagSet(woStatus, WOS_CURRENT) && FlagSet(object->woStatus, WOS_CURRENT))
                                                                        DrawFocus(screenID, region, S_CURRENT);
                                                        }
                                                }
                                        }
                                        if (backgroundRegion.Overlap(updateRegion))
                                                display->Rectangle(screenID, backgroundRegion, lastPalette, 0, TRUE);
                                        if (!object)
                                        {
                                                backgroundRegion.top = 0;
                                                backgroundRegion.left += Last()->trueRegion.Width();
                                                backgroundRegion.right = windowRect.xRight - 1;
                                                if (backgroundRegion.Overlap(updateRegion))
                                                        display->Rectangle(screenID, backgroundRegion, lastPalette, 0, TRUE);
                                        }
                                }
                                else
                                        display->Rectangle(screenID, updateRegion, lastPalette, 0, TRUE);
                                WinEndPaint(UI_OS2_DISPLAY::hps);
                                display->VirtualPut(ID_DIRECT);
                        }
                        return (0);

                default:
                        ccode = UIW_WINDOW::Event(event);
                        break;
                }

                if (processed)
                        return (ccode);
        }

        // Zinc specific messages.
        ccode = LogicalEvent(event, ID_HZ_LIST);
        switch (ccode)
        {
        case S_INITIALIZE:
                UIW_WINDOW::Event(event);
                topObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                break;

        case S_CREATE:
        case S_CHANGED:
        case S_DISPLAY_ACTIVE:
        case S_REDISPLAY:
                {
                if (!topObject || Index(topObject) == -1)
                        topObject = First();

                if (ccode == S_CREATE)
                        UIW_WINDOW::Event(event);
                else if (screenID)
                {
                        UI_EVENT scrollEvent(S_SCROLLRANGE);
                        ScrollEvent(scrollEvent);
                }
                else
                        break;

                // Set the child object regions.
                int top = 0, left = 0;
                int itemWidth = cellWidth * display->cellWidth;
                int itemHeight = cellHeight * (display->cellHeight - display->preSpace - display->postSpace + 1);
                UI_REGION clientRegion = clipList.First()->region;
                for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
                {
                        if (top + itemHeight > clientRegion.Height())
                        {
                                top = 0;
                                left += itemWidth;
                        }
                        object->trueRegion.left = object->relative.left = left;
                        object->trueRegion.top = object->relative.top = top;
                        object->trueRegion.right = object->relative.right = left + itemWidth - 1;
                        top += itemHeight;
                        object->trueRegion.bottom = object->relative.bottom = top - 1;

                        object->trueRegion.left += scroll.left;
                        object->trueRegion.right += scroll.right;
                }

                if (ccode != S_CREATE)
                        UI_WINDOW_OBJECT::Event(event);
                }
                break;

        case S_REGISTER_OBJECT:
                {
                UIW_WINDOW::Event(event);
                for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
                {
                        object->screenID = screenID;
                        object->woStatus |= WOS_OWNERDRAW;
                }
                }
                break;

        case L_LEFT:
        case L_RIGHT:
        case L_UP:
        case L_DOWN:
        case L_PGUP:
        case L_PGDN:
                if (current)
                {
                        UI_REGION clientRegion = clipList.First()->region;
                        int itemWidth = cellWidth * display->cellWidth;
                        int itemHeight = cellHeight * (display->cellHeight - display->preSpace - display->postSpace + 1);
                        int rows = clientRegion.Height() / itemHeight;
                        int columns = clientRegion.Width() / itemWidth;

                        UI_WINDOW_OBJECT *newCurrent = Current();
                        if (ccode == L_LEFT)
                        {
                                for (int i = 0; newCurrent->Previous() && i < rows; i++)
                                        newCurrent = newCurrent->Previous();
                        }
                        else if (ccode == L_RIGHT)
                        {
                                for (int i = 0; newCurrent->Next() && i < rows; i++)
                                        newCurrent = newCurrent->Next();
                        }
                        else if (ccode == L_UP && newCurrent->Previous())
                                newCurrent = newCurrent->Previous();
                        else if (ccode == L_DOWN && newCurrent->Next())
                                newCurrent = newCurrent->Next();
                        else if (ccode == L_PGUP)
                        {
                                for (int i = 0; newCurrent->Previous() && i < rows * columns; i++)
                                        newCurrent = newCurrent->Previous();
                        }
                        else if (ccode == L_PGDN)
                        {
                                for (int i = 0; newCurrent->Next() && i < rows * columns; i++)
                                        newCurrent = newCurrent->Next();
                        }

                        Add(newCurrent);
                        UI_EVENT scrollEvent(S_HSCROLL_CHECK);
                        ScrollEvent(scrollEvent);
                }
                break;


        case L_TOP:
        case L_BOTTOM:
                if (First())
                {
                        Add(ccode == L_TOP ? First() : Last());
                        UI_EVENT scrollEvent(S_HSCROLL_CHECK);
                        ScrollEvent(scrollEvent);
                }
                break;

        case L_SELECT:
                if (current)
                {
                        Current()->UserFunction(event, L_SELECT);
                        Current()->Event(UI_EVENT(S_REDISPLAY, 0));
                }
                break;

        case L_BEGIN_SELECT:
        case L_CONTINUE_SELECT:
        case L_END_SELECT:
        case L_DOUBLE_CLICK:
                {
                UI_WINDOW_OBJECT::Event(event);
#if defined (ZIL_EDIT)
                if (FlagSet(woStatus, WOS_EDIT_MODE))
                        break;
#endif
                if (First())
                {
                        UI_POSITION mousePos = event.position;
                        mousePos.column -= trueRegion.left;
                        mousePos.line -= trueRegion.top;

                        UI_REGION clientRegion = clipList.First()->region;
                        for (UI_WINDOW_OBJECT *object = topObject; object; object = object->Next())
                        {
                                if (object->trueRegion.Overlap(mousePos))
                                {
                                        if (FlagSet(object->woFlags, WOF_NON_SELECTABLE))
                                                break;

                                        static int discardUpClick = TRUE;
                                        UI_WINDOW_OBJECT *oldCurrent = Current();
                                        BTF_FLAGS btFlags = 0;
                                        object->Information(I_GET_FLAGS, &btFlags, ID_BUTTON);

                                        if (ccode == L_BEGIN_SELECT || ccode == L_CONTINUE_SELECT)
                                        {
                                                if (ccode == L_BEGIN_SELECT)
                                                        discardUpClick = FALSE;
                                                if (object != oldCurrent)
                                                        Add(object);

                                                if (ccode == L_BEGIN_SELECT && FlagSet(btFlags, BTF_DOWN_CLICK))
                                                {
                                                        object->UserFunction(event, L_SELECT);
                                                        object->woStatus |= WOS_REDISPLAY;
                                                }
                                        }
                                        else if (ccode == L_DOUBLE_CLICK)
                                        {
                                                if (FlagSet(btFlags, BTF_DOUBLE_CLICK))
                                                {
                                                        discardUpClick = TRUE;
                                                        object->UserFunction(event, L_DOUBLE_CLICK);
                                                        object->woStatus |= WOS_REDISPLAY;
                                                }
                                                else if (FlagSet(btFlags, BTF_DOWN_CLICK))
                                                {
                                                        object->UserFunction(event, L_SELECT);
                                                        object->woStatus |= WOS_REDISPLAY;
                                                }
                                        }
                                        else
                                        {
                                                if (!discardUpClick && !FlagSet(btFlags, BTF_DOWN_CLICK))
                                                {
                                                        object->UserFunction(event, L_SELECT);
                                                        object->woStatus |= WOS_REDISPLAY;
                                                }
                                                discardUpClick = TRUE;
                                        }

                                        if (FlagSet(object->woStatus, WOS_REDISPLAY))
                                                object->Event(UI_EVENT(S_REDISPLAY, 0));
                                }
                                if (object->trueRegion.left > clientRegion.Width() - 1)
                                        break;
                        }
                }
                }
                break;

        case S_CURRENT:
        case S_NON_CURRENT:
                if (ccode == S_CURRENT)
                        woStatus |= WOS_CURRENT;
                else
                        woStatus &= ~WOS_CURRENT;
                UIW_WINDOW::Event(event);
                break;

        case S_DRAG_DEFAULT:
        case S_DRAG_MOVE_OBJECT:
                if (!windowManager->dragObject->Inherited(ID_LIST) &&
                        !(windowManager->dragObject->parent &&
                        windowManager->dragObject->parent->Inherited(ID_LIST)))
                {
                        if (ccode == S_DRAG_DEFAULT)
                                ccode = S_DRAG_COPY_OBJECT;
                        else
                                return (S_ERROR);
                }
                // Continue.

        case S_DRAG_COPY_OBJECT:
                eventManager->DeviceState(E_MOUSE, ccode == S_DRAG_COPY_OBJECT ? DM_DRAG_COPY : DM_DRAG_MOVE);
                return (0);

        case S_DROP_DEFAULT:
        case S_DROP_MOVE_OBJECT:
        case S_DROP_COPY_OBJECT:
                if (ccode == S_DRAG_DEFAULT)
                {
                        if (!windowManager->dragObject->Inherited(ID_LIST) &&
                                !(windowManager->dragObject->parent &&
                                windowManager->dragObject->parent->Inherited(ID_LIST)))
                                ccode = S_DROP_COPY_OBJECT;
                        else
                                ccode = S_DROP_MOVE_OBJECT;
                }
                if (ccode == S_DROP_MOVE_OBJECT)
                {
                        if (windowManager->dragObject->parent &&
                                windowManager->dragObject->parent == this)
                                        break;
                        else if (windowManager->dragObject->Inherited(ID_LIST))
                        {
                                UI_WINDOW_OBJECT *object;
                                windowManager->dragObject->Information(I_GET_FIRST, &object);
                                while (object)
                                {
                                        UI_WINDOW_OBJECT *nextObject = object->Next();
                                        if (FlagSet(object->woStatus, WOS_SELECTED))
                                        {
                                                UI_EVENT moveEvent(S_SUBTRACT_OBJECT);
                                                moveEvent.windowObject = object;
                                                windowManager->dragObject->Event(moveEvent);
                                                object->woStatus &= ~WOS_CURRENT;
                                                moveEvent.type = S_ADD_OBJECT;
                                                Event(moveEvent);
                                        }
                                        object = nextObject;
                                }
                                windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
                        }
                        else if (windowManager->dragObject->parent &&
                                windowManager->dragObject->parent->Inherited(ID_LIST) &&
                                windowManager->dragObject->parent != this)
                        {
                                UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
                                UI_WINDOW_OBJECT *dragParent = dragObject->parent;
                                UI_EVENT moveEvent(S_SUBTRACT_OBJECT);
                                moveEvent.windowObject = dragObject;
                                dragParent->Event(moveEvent);
                                dragParent->Event(UI_EVENT(S_REDISPLAY));
                                dragObject->relative.right = dragObject->relative.Width() - 1;
                                dragObject->relative.left = 0;
                                moveEvent.type = S_ADD_OBJECT;
                                moveEvent.windowObject->woStatus &= ~WOS_CURRENT;
                                Event(moveEvent);
                        }
                        Event(S_REDISPLAY);
                }
#if defined(ZIL_LOAD) && defined(ZIL_STORE)
                else
                {
                        if (windowManager->dragObject->Inherited(ID_LIST))
                        {
                                UI_WINDOW_OBJECT *object;
                                windowManager->dragObject->Information(I_GET_FIRST, &object);
                                while (object)
                                {
                                        UI_WINDOW_OBJECT *nextObject = object->Next();
                                        if (FlagSet(object->woStatus, WOS_SELECTED))
                                        {
                                                ZIL_ICHAR name[] = { 'd','r','a','g',0 };
                                                ZIL_NEW_FUNCTION newFunction = object->NewFunction();
                                                ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
                                                ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
                                                object->Store(name, &directory, &file);
                                                file.Seek(0);
                                                object = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
                                                UI_EVENT addEvent(S_ADD_OBJECT);
                                                addEvent.windowObject = object;
                                                Event(addEvent);
                                        }
                                        object = nextObject;
                                }
                                windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
                        }
                        else
                        {
                                ZIL_ICHAR name[] = { 'd','r','a','g',0 };
                                UI_WINDOW_OBJECT *dragObject = windowManager->dragObject;
                                ZIL_NEW_FUNCTION newFunction = dragObject->NewFunction();
                                ZIL_STORAGE directory(name, UIS_CREATE | UIS_READWRITE | UIS_TEMPORARY | UIS_COPY);
                                ZIL_STORAGE_OBJECT file(directory, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
                                dragObject->Store(name, &directory, &file);
                                file.Seek(0);
                                dragObject = (newFunction)(name, &directory, &file, ZIL_NULLP(UI_ITEM), ZIL_NULLP(UI_ITEM));
                                dragObject->relative.right = dragObject->relative.Width() - 1;
                                dragObject->relative.left = 0;
                                UI_EVENT dEvent(S_ADD_OBJECT);
                                dEvent.windowObject = dragObject;
                                Event(dEvent);
                        }
                        Event(S_REDISPLAY);
                }
#endif
                break;

        default:
                ccode = UIW_WINDOW::Event(event);
                break;
        }

        // Return the control code.
        return (ccode);
}

EVENT_TYPE UIW_HZ_LIST::ScrollEvent(UI_EVENT &event)
{
        UI_REGION clientRegion = clipList.First()->region;
        int itemHeight = cellHeight * (display->cellHeight - display->preSpace - display->postSpace + 1);
        int itemWidth = cellWidth * display->cellWidth;
        int rows = clientRegion.Height() / itemHeight;

        EVENT_TYPE ccode = event.type;
        switch (ccode)
        {
        case S_SCROLLRANGE:
                // Set the scroll bar information.
                hScrollInfo.showing = clientRegion.Width() / itemWidth * itemWidth;
                hScrollInfo.minimum = 0;
                if (rows)
                {
                        hScrollInfo.maximum = MaxValue((Count() + rows - 1) / rows * itemWidth - hScrollInfo.showing, 0);
                        hScrollInfo.current = topObject ? Index(topObject) / rows * itemWidth : 0;
                }
                hScrollInfo.delta = itemWidth;

                if (hScroll)
                {
                        UI_EVENT sEvent(S_HSCROLL_SET);
                        sEvent.scroll = hScrollInfo;
                        hScroll->Event(sEvent);
                }
                break;

        case S_HSCROLL:
                {
                int itemWidth = cellWidth * display->cellWidth;
                int delta = event.scroll.delta / itemWidth;
                event.scroll.delta = delta * itemWidth;

                if (delta)
                {
                        int rows = clientRegion.Height() / itemHeight;
                        if (delta > 0)
                        {
                                for (int i = 0; i < delta * rows; i++)
                                        topObject = topObject->Next();
                        }
                        else
                        {
                                for (int i = 0; i < -delta * rows; i++)
                                        topObject = topObject->Previous();
                        }

                        for (UI_WINDOW_OBJECT *object = topObject; object; object = object->Next())
                        {
                                if (scroll.left - event.scroll.delta + object->relative.left > clientRegion.Width())
                                        break;

                                object->trueRegion.left = scroll.left - event.scroll.delta + object->relative.left;
                                object->trueRegion.right = scroll.left - event.scroll.delta + object->relative.right;
                        }
                        UIW_WINDOW::ScrollEvent(event);
                }
                }
                break;

        case S_HSCROLL_CHECK:
                if (current)
                {
                        int scrollDelta = 0;
                        int itemWidth = cellWidth * display->cellWidth;
                        if (Current()->relative.left + scroll.left < 0)
                                scrollDelta = scroll.left + Current()->relative.left;
                        else if (Current()->relative.right + scroll.left > clientRegion.Width() - 1)
                                scrollDelta = (Current()->relative.right + scroll.left - clientRegion.Width() +
                                        itemWidth - 1) / itemWidth * itemWidth;

                        if (scrollDelta)
                        {
                                UI_EVENT sEvent(S_HSCROLL);
                                sEvent.scroll.delta = scrollDelta;
                                ScrollEvent(sEvent);
                        }
                }
                break;

        default:
                ccode = UIW_WINDOW::ScrollEvent(event);
        }
        return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_HZ_LIST::OSDestroy(void)
{
        if (screenID)
        {
                WinEnableWindowUpdate(screenID, FALSE);
                UIW_WINDOW::Destroy();
                topObject = ZIL_NULLP(UI_WINDOW_OBJECT);
        }
}

void UIW_HZ_LIST::OSUpdateSettings(ZIL_OBJECTID objectID)
{
        // See if the field needs to be re-computed.
        if (objectID == ID_HZ_LIST && FlagSet(woStatus, WOS_REDISPLAY))
        {
                UI_EVENT event(S_INITIALIZE, 0);
                Event(event);
                event.type = S_CREATE;
                Event(event);
        }
}

void UIW_HZ_LIST::OSSort(void)
{
}
