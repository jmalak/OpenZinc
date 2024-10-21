//      Zinc Interface Library - W_WIN2.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "ui_win.hpp"

#if defined(ZIL_MSWINDOWS_CTL3D)
#       include <ctl3d.h>
#endif

extern ZIL_ICHAR *_genericFieldID;

// ----- UI_WINDOW_OBJECT ---------------------------------------------------
static WNDPROC _objectCallback = ZIL_NULLF(WNDPROC);
static int _objectOffset = GWL_USERDATA;
static int _offset = GWL_USERDATA;
#define ZIL_EXPORT_CLASSABLE

static UI_WINDOW_OBJECT *_object = ZIL_NULLP(UI_WINDOW_OBJECT);
static WORD _controlID = 0;

LRESULT CALLBACK ZIL_EXPORT_CLASSABLE ObjectJumpProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _objectOffset);
        return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}

ATOM zafProcL;
ATOM zafProcH;

LRESULT CALLBACK ZIL_EXPORT_CLASSABLE JumpProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
#if defined(ZIL_UNICODE)
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPWSTR)zafProcL), GetProp(hWnd, (LPWSTR)zafProcH));
#else
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPCSTR)zafProcL), GetProp(hWnd, (LPCSTR)zafProcH));
#endif
        return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}

LRESULT CALLBACK ZIL_EXPORT_CLASSABLE ObjectInitProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
        _object->screenID = hWnd;
        SetWindowLong(hWnd, _offset, (LONG)_object);
        SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ObjectJumpProcedure);
        if (_object->parent && !FlagSet(_object->woAdvancedFlags, WOAF_MDI_OBJECT))
                SetWindowLong(hWnd, GWL_ID, _controlID);
        UI_WINDOW_OBJECT *object = _object;
        _object = ZIL_NULLP(UI_WINDOW_OBJECT);
        return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}

HBRUSH UI_WINDOW_OBJECT::ctlHBrush = 0;

EVENT_TYPE UI_WINDOW_OBJECT::DrawBorder(ZIL_SCREENID screenID, UI_REGION &region,
        int fillRegion, EVENT_TYPE ccode)
{
        display->VirtualGet(screenID, trueRegion);

        // Draw the outer rectangle.
        if (Inherited(ID_BUTTON))
        {
                UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
                display->Rectangle(screenID, region, outline, 1, FALSE);
        }
        else if (Inherited(ID_STATUS_ITEM))
                DrawShadow(screenID, region, -1, fillRegion, ccode);
#if defined(ZIL_MSWINDOWS_CTL3D)
        else if (Inherited(ID_WINDOW) && !Inherited(ID_TABLE_RECORD))
                display->Rectangle(screenID, region, lastPalette, 1, FALSE);
#else
        else if (!Inherited(ID_TABLE_RECORD))
                display->Rectangle(screenID, region, lastPalette, 1, FALSE);
#endif
        else
        {
                // Draw chiseled border.
                //  WARNING: 3D controls draw outside their region.
                if (Inherited(ID_LIST_ITEM))
                        --region;
                UI_PALETTE *shadow = LogicalPalette(ccode, ID_DARK_SHADOW);
                UI_PALETTE *leftTop = LogicalPalette(ccode, ID_BLACK_SHADOW);
                UI_PALETTE *rightBottom = LogicalPalette(ccode, ID_LIGHT_SHADOW);
                UI_PALETTE *highlight = LogicalPalette(ccode, ID_WHITE_SHADOW);
                lastPalette = LogicalPalette(ccode, Inherited(ID_STATUS_ITEM) ?
                        ID_STATUS_ITEM : ID_DEFAULT);
                display->Line(screenID, region.left - 1, region.bottom, region.left - 1,
                        region.top - 1, shadow, 1, FALSE);
                display->Line(screenID, region.left - 1, region.top - 1, region.right,
                        region.top - 1, shadow, 1, FALSE);
                display->Line(screenID, region.left, region.bottom - 1, region.left,
                        region.top, leftTop, 1, FALSE);
                display->Line(screenID, region.left, region.top, region.right - 1,
                        region.top, leftTop, 1, FALSE);
                display->Line(screenID, region.left - 1, region.bottom + 1, region.right + 1,
                        region.bottom + 1, highlight, 1, FALSE);
                display->Line(screenID, region.right + 1, region.bottom + 1, region.right + 1,
                        region.top - 1, highlight, 1, FALSE);
                display->Line(screenID, region.left, region.bottom, region.right,
                        region.bottom, rightBottom, 1, FALSE);
                display->Line(screenID, region.right, region.bottom, region.right,
                        region.top, rightBottom, 1, FALSE);
        }

        --region;

        if (fillRegion)
                display->Rectangle(screenID, region, lastPalette, 0, TRUE);

        display->VirtualPut(screenID);
        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawFocus(ZIL_SCREENID screenID, UI_REGION &region, EVENT_TYPE)
{
        // Focus rect automatic for list items.
        if (!Inherited(ID_LIST_ITEM))
        {
#if defined(ZIL_OPTIMIZE)
                UI_DISPLAY *display = this->display;
#endif
                display->VirtualGet(screenID, trueRegion);

                display->Rectangle(screenID, region, display->xorPalette, 1, FALSE, TRUE);

                display->VirtualPut(screenID);
        }

        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
        return (FALSE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawShadow(ZIL_SCREENID screenID, UI_REGION &region,
        int depth, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
        UI_DISPLAY *display = this->display;
#endif

        display->VirtualGet(screenID, trueRegion);

        // Draw the highlights.
        UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_LIGHT_SHADOW);
        UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
        UI_PALETTE *whiteShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
        UI_PALETTE *blackShadow = LogicalPalette(ccode, ID_BLACK_SHADOW);
        if (depth > 0)
        {
                display->Line(screenID, region.left, region.top + 1, region.left,
                        region.bottom - 1, lightShadow, 1, FALSE);
                display->Line(screenID, region.left, region.top, region.right,
                        region.top, lightShadow, 1, FALSE);
                if (depth > 1)
                {
                        display->Line(screenID, region.right, region.top + 1,
                                region.right, region.bottom, blackShadow, 1, FALSE);
                        display->Line(screenID, region.left, region.bottom,
                                region.right - 1, region.bottom, blackShadow, 1, FALSE);
                        --region;
                        display->Line(screenID, region.left, region.top + 1, region.left,
                                region.bottom - 1, whiteShadow, 1, FALSE);
                        display->Line(screenID, region.left, region.top, region.right,
                                region.top, whiteShadow, 1, FALSE);
                }
                display->Line(screenID, region.right, region.top + 1,
                        region.right, region.bottom, darkShadow, 1, FALSE);
                display->Line(screenID, region.left, region.bottom,
                        region.right - 1, region.bottom, darkShadow, 1, FALSE);
                --region;
        }
        else if (depth < 0)
        {
                display->Line(screenID, region.right, region.top + 1,
                        region.right, region.bottom, whiteShadow, 1, FALSE);
                display->Line(screenID, region.left, region.bottom,
                        region.right - 1, region.bottom, whiteShadow, 1, FALSE);
                if (depth < -1)
                {
                        display->Line(screenID, region.left, region.top + 1, region.left,
                                region.bottom - 1, blackShadow, 1, FALSE);
                        display->Line(screenID, region.left, region.top, region.right,
                                region.top, blackShadow, 1, FALSE);
                        --region;
                        display->Line(screenID, region.right, region.top + 1,
                                region.right, region.bottom, lightShadow, 1, FALSE);
                        display->Line(screenID, region.left, region.bottom,
                                region.right - 1, region.bottom, lightShadow, 1, FALSE);
                }
                display->Line(screenID, region.left, region.top + 1, region.left,
                        region.bottom - 1, darkShadow, 1, FALSE);
                display->Line(screenID, region.left, region.top, region.right,
                        region.top, darkShadow, 1, FALSE);
                --region;
        }
        if (fillRegion)
                display->Rectangle(screenID, region, ZIL_NULLP(UI_PALETTE), 0, TRUE);

        display->VirtualPut(screenID);
        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawText(ZIL_SCREENID screenID, UI_REGION &region,
        const ZIL_ICHAR *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
        UI_DISPLAY *display = this->display;
#endif

        display->VirtualGet(screenID, region);

        // Make sure there is a valid string.
        if (text && text[0])
        {
                // Get the default palette if none specified.
                if (!palette)
                        palette = LogicalPalette(ccode, ID_WINDOW_OBJECT);

                // Fill the region (if so specified).
                if (fillRegion)
                        display->Rectangle(screenID, region, palette, 0, TRUE);

                // Draw the text.
                int textWidth = display->TextWidth(text, screenID, font);
                int textHeight = display->TextHeight(text, screenID, font);

                int left = region.left;
                if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
                        left = region.right - textWidth;
                else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
                        left += (region.Width() - textWidth) / 2;
                region.left = MaxValue(left, region.left);
                region.top += (region.Height() - textHeight) / 2;

                display->Text(screenID, left, region.top, text, palette, -1,
                        FALSE, FALSE, &trueRegion, font);

                region.bottom = region.top + textHeight - 1;
                region.right = MinValue(region.left + textWidth - 1, region.right);
        }

        display->VirtualPut(screenID);
        return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::Event(const UI_EVENT &event)
{
        static int initializedTime = FALSE;
        static ZIL_TIME lastTime;
        if (!initializedTime)
        {
                lastTime.Import();
                initializedTime = TRUE;
        }
        UI_WINDOW_OBJECT *object;

        EVENT_TYPE ccode = S_UNKNOWN;
        ZIL_SCREENID frameID;
        Information(I_GET_FRAMEID, &frameID);
        HWND hwnd = frameID ? frameID : event.message.hwnd;
        UINT message = event.message.message;
        WPARAM wParam = event.message.wParam;
        LPARAM lParam = event.message.lParam;

        //Process messages from system first.
        int processed = FALSE;
        if (event.type == E_MSWINDOWS)
        {
                processed = TRUE;

                // Switch on the windows message.
                ccode = TRUE;
                switch (message)
                {
                case ZM_CONVERTPOINT:
                        {
                        UI_POSITION *position = (UI_POSITION *)lParam;
                        position->column += trueRegion.left;
                        position->line += trueRegion.top;
                        }
                        break;

                case BM_SETSTATE:
                        if (!FlagsSet(dwStyle, BS_OWNERDRAW) ||         // Note: FlagsSet not FlagSet.
                                Inherited(ID_BUTTON) || Inherited(ID_LIST))
                                        processed = FALSE;
                        break;

                case WM_DESTROY:
                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd, message, wParam, lParam);
                        screenID = 0;
                        woStatus &= ~WOS_SYSTEM_OBJECT;
                        break;

                case WM_CLOSE:
                        if (!parent && FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
                                eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY));
                        else if (!parent)
                                eventManager->Put(UI_EVENT(S_CLOSE));
                        else
                                eventManager->Put(UI_EVENT(S_CLOSE + S_MDICHILD_EVENT));
                        break;

                case WM_MEASUREITEM:
                        if (!FlagsSet(GetVersion(), 0x80000003))                // Win32s Bug (lParam corrupt)
                        {
                                MEASUREITEMSTRUCT *item = (MEASUREITEMSTRUCT *)lParam;
                                item->itemWidth = relative.Width();
                                item->itemHeight = relative.Height();
                        }
                        break;

                case WM_PAINT:
                        // See if the object draws its information.
                        if (FlagSet(woStatus, WOS_OWNERDRAW) && !Inherited(ID_LIST))
                        {
                                EVENT_TYPE winCode =
                                        FlagSet(woStatus, WOS_CURRENT) ? S_CURRENT : S_DISPLAY_ACTIVE;
                                RECT winRegion;
                                if (GetUpdateRect(screenID, &winRegion, FALSE) ||
                                        !FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                                {
                                        UI_EVENT tEvent = event;
                                        tEvent.region.left = winRegion.left + trueRegion.left;
                                        tEvent.region.top = winRegion.top + trueRegion.top;
                                        tEvent.region.right = winRegion.right + 1 + trueRegion.left;
                                        tEvent.region.bottom = winRegion.bottom + 1 + trueRegion.top;
                                        lastPalette = LogicalPalette(ccode);

                                        UI_MSWINDOWS_DISPLAY::hDC = BeginPaint(screenID, &UI_MSWINDOWS_DISPLAY::paintStruct);
                                        display->VirtualGet(ID_DIRECT, trueRegion.left, trueRegion.top,
                                                trueRegion.right, trueRegion.bottom);
                                        DrawItem(tEvent, winCode);
                                        display->VirtualPut(ID_DIRECT);
                                        EndPaint(screenID, &UI_MSWINDOWS_DISPLAY::paintStruct);
                                }
                                return (0);
                        }
                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback,
                                event.message.hwnd, message, wParam, lParam);
                        break;

                case WM_DRAWITEM:
                        {
                        DRAWITEMSTRUCT *draw = (DRAWITEMSTRUCT *)lParam;

                        // Check the object's current status.
                        if (parent)
                        {
                                WNF_FLAGS wnFlags = WNF_NO_FLAGS;
                                parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
                                if (FlagsSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
                                {
                                        if (FlagSet(draw->itemState, ODS_SELECTED))
                                                woStatus |= WOS_SELECTED;
                                        else
                                                woStatus &= ~WOS_SELECTED;
                                }
                        }
                        if (FlagSet(draw->itemState, ODS_FOCUS))
                                woStatus |= WOS_CURRENT;
                        else
                                woStatus &= ~WOS_CURRENT;

                        if (draw->CtlType == ODT_LISTBOX && draw->itemAction == ODA_FOCUS)
                                DrawFocusRect(draw->hDC, &draw->rcItem);
                        else
                        {
                                // Recalculate the object's region.
                                UI_MSWINDOWS_DISPLAY::hDC = draw->hDC;
                                if (parent->Inherited(ID_LIST))
                                {
                                        int height = relative.Height() - 1;
                                        trueRegion.left = draw->rcItem.left;
                                        trueRegion.top = draw->rcItem.top;
                                        trueRegion.right = draw->rcItem.right - 1;
                                        trueRegion.bottom = trueRegion.top + height;
                                        display->VirtualGet(ID_DIRECT, 0, 0, trueRegion.Width(), height);
                                }
                                else
                                        display->VirtualGet(ID_DIRECT, trueRegion);

                                EVENT_TYPE winCode =
                                        !FlagSet(draw->itemAction, ODA_FOCUS) ? S_DISPLAY_ACTIVE :
                                        FlagSet(draw->itemState, ODS_FOCUS) ? S_CURRENT : S_NON_CURRENT;
                                UI_EVENT tEvent(winCode, 0 , trueRegion);
                                lastPalette = LogicalPalette(ccode);
                                ccode = DrawItem(tEvent, winCode);
                                display->VirtualPut(ID_DIRECT);
                        }

                        // Check for invalid display object.
                        if (!ccode)
                                processed = FALSE;
                        }
                        break;

                case WM_CTLCOLORBTN:
                case WM_CTLCOLORDLG:
                case WM_CTLCOLOREDIT:
                case WM_CTLCOLORLISTBOX:
                case WM_CTLCOLORMSGBOX:
                case WM_CTLCOLORSCROLLBAR:
                case WM_CTLCOLORSTATIC:
                        {
                        ZIL_SCREENID objectID = (HWND)lParam;
                        HDC objectDC = (HDC)wParam;
                        if (objectID == event.message.hwnd)
                        {
                                if (FlagSet(woStatus, WOS_OWNERDRAW))
                                        ccode = CallWindowProc((WINDOWSPROC)DefWindowProc, hwnd,
//                                      ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                                message, wParam, lParam);
                                else
                                {
                                        UI_WINDOW_OBJECT *fObject = NULL;
                                        for (fObject = this; fObject &&
                                                display->MapColor(fObject->LogicalPalette(S_DISPLAY_ACTIVE), TRUE) == BACKGROUND;
                                                fObject = fObject->parent)
                                                ;
                                        SetTextColor (objectDC, !fObject ? GetSysColor(COLOR_WINDOWTEXT) :
                                                display->MapColor(fObject->LogicalPalette(S_DISPLAY_ACTIVE), TRUE));

                                        UI_WINDOW_OBJECT *bObject = NULL;
                                        for (bObject = this; bObject &&
                                                display->MapColor(bObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE) == BACKGROUND;
                                                bObject = bObject->parent)
                                                ;
                                        SetBkColor (objectDC, !bObject ? GetSysColor(COLOR_WINDOW) :
                                                display->MapColor(bObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE));
        
                                        if (ctlHBrush)
                                                DeleteObject(ctlHBrush);
                                        ctlHBrush = CreateSolidBrush(!bObject ? GetSysColor(COLOR_WINDOW) :
                                                display->MapColor(bObject->LogicalPalette(S_DISPLAY_ACTIVE), FALSE));
                                        UnrealizeObject(ctlHBrush);
                                        POINT p = { 0, 0 };
                                        ClientToScreen(screenID, &p);
                                        POINT oldP;
                                        SetBrushOrgEx(objectDC, p.x, p.y, &oldP);
        
                                        ccode = (LRESULT)ctlHBrush;
                                }
                        }
                        else
                        {
                                ccode = SendMessage(objectID, message, wParam, lParam);
                                if (!ccode)
                                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                                message, wParam, lParam);
                        }
                        }
                        break;

#if !defined(ZIL_EDIT)
                case WM_NCLBUTTONDBLCLK:
                case WM_NCLBUTTONDOWN:
                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback,
                                event.message.hwnd, message, wParam, lParam);
                        break;
#else
                case WM_NCLBUTTONDBLCLK:
                case WM_NCLBUTTONDOWN:
                        if (!FlagSet(woStatus, WOS_EDIT_MODE))
                        {
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback,
                                        event.message.hwnd, message, wParam, lParam);
                                break;
                        }
                        else if (!parent)
                        {
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback,
                                        event.message.hwnd, WM_NCLBUTTONDOWN, wParam, lParam);
                        }
                        // Fall through to WM_LBUTTONDBLCLK.
#endif

                case WM_LBUTTONDBLCLK:
                case WM_LBUTTONDOWN:
#if defined(ZIL_EDIT)
                        if (FlagSet(woStatus, WOS_EDIT_MODE))
                        {
                                ZIL_TIME currentTime;
                                UI_EVENT dEvent;
                                UI_EVENT mEvent;
                                mEvent = event;
                                if (message < WM_MOUSEFIRST || message > WM_MOUSELAST)
                                {
                                        // WM_NC* message.
                                        POINTS ps = MAKEPOINTS(lParam);
                                        POINT p = { ps.x, ps.y };
                                        ScreenToClient(screenID, &p);
                                        mEvent.message.lParam = p.x | ((LPARAM)p.y << 16);
                                        mEvent.position.column = trueRegion.left + p.x;
                                        mEvent.position.line = trueRegion.top + p.y;
                                }
        
                                long elapsedTime = currentTime - lastTime;
                                if (AbsValue(elapsedTime) < doubleClickRate)
                                {
                                        dEvent.type = D_EDIT_OBJECT;
                                        dEvent.rawCode = searchID;
                                        dEvent.data = this;
                                        eventManager->Put(dEvent);
                                        break;
                                }
                                if (parent)
                                {
                                        if (hwnd && GetFocus() != hwnd)
                                                SetFocus(hwnd);
                                        Modify(mEvent);
                                }
                                else if (event.message.message == WM_LBUTTONDOWN ||
                                        event.message.message == WM_LBUTTONDBLCLK)
                                        processed = FALSE;
        
                                // Set the new current object.
                                dEvent.type = D_SET_OBJECT;
                                dEvent.rawCode = searchID;
                                dEvent.windowObject = this;
                                eventManager->Put(dEvent);
                                dEvent.type = D_SET_POSITION;
                                dEvent.rawCode = M_LEFT | M_LEFT_CHANGE;
                                dEvent.position.column = mEvent.position.column;
                                dEvent.position.line = mEvent.position.line;
                                eventManager->Put(dEvent);
                                lastTime = currentTime;
                        }
                        else
#endif
//                              if (!FlagSet(woFlags, WOF_NON_SELECTABLE) &&
//                                      !FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
                                if (!FlagSet(woFlags, WOF_NON_SELECTABLE))
                                                processed = FALSE;
                        break;

#if defined(ZIL_EDIT)
                case WM_NCRBUTTONDBLCLK:
                case WM_NCRBUTTONDOWN:
                case WM_RBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                        if (FlagSet(woStatus, WOS_EDIT_MODE))
                        {
                                UI_EVENT dEvent;
                                dEvent.type = D_SET_POSITION;
                                dEvent.rawCode = M_RIGHT | M_RIGHT_CHANGE;
                                dEvent.position = event.position;
                                eventManager->Put(dEvent);
                                break;
                        }
                        else
                                processed = FALSE;
                        break;
#endif

                case WM_MOUSEACTIVATE:
                        if (FlagSet(woFlags, WOF_NON_SELECTABLE) || FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
                                break;
                        else if (!FlagSet(woStatus, WOS_WINDOWS_ACTION | WOS_CURRENT))
                        {
                                woStatus |= WOS_WINDOWS_ACTION;
                                UI_WINDOW_OBJECT::Event(UI_EVENT(S_ADD_OBJECT, 0));
                                woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd, message, wParam, lParam);
                        break;

                case WM_SETCURSOR:
#if defined(ZIL_EDIT)
                        if (FlagSet(woStatus, WOS_EDIT_MODE) && parent)
                        {
                                SetCursor(LoadCursor(0, IDC_ARROW));
                                ccode = TRUE;
                        }
                        else
#endif
                                if (windowManager->dragObject)
                                        ccode = TRUE;
                                else
                                {
                                        processed = FALSE;
                                        hwnd = event.message.hwnd;
                                }
                        break;

                case WM_SETFOCUS:
                        if (!FlagSet(woFlags, WOF_NON_SELECTABLE)
                                && !FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
                        {
                                if (screenID && FlagSet(woStatus, WOS_OWNERDRAW) && !Inherited(ID_LIST))
                                        InvalidateRect(screenID, ZIL_NULLP(RECT), FALSE);
                                else
                                        processed = FALSE;
                        }
                        break;

                case WM_SYSCHAR:
                        if (parent)
                                ccode = Root()->Event(event);
                        else
                                processed = FALSE;
                        break;

                case WM_CHAR:
                        if (wParam >= ' ' && !Inherited(ID_STRING) && parent &&
                                FlagSet(parent->woAdvancedFlags, WOAF_NORMAL_HOT_KEYS))
                                        ccode = parent->Event(UI_EVENT(E_MSWINDOWS, hwnd, WM_SYSCHAR, wParam, lParam));
                        else
                                processed = FALSE;
                        break;

                case WM_KEYUP:
                case WM_SYSKEYUP:
                        if (windowManager->dragObject)
                        {
                                POINT p;
                                GetCursorPos(&p);
                                SetCursorPos(p.x, p.y);
                        }
                        if (helpSystem && UI_EVENT_MAP::MapEvent(eventMapTable, event, ID_WINDOW_MANAGER) == L_HELP)
                                helpSystem->DisplayHelp(windowManager, NO_HELP_CONTEXT);
                        else
                                processed = FALSE;
                        break;

                // Ambiguous cases.
                case WM_KEYDOWN:
                        if (windowManager->dragObject)
                        {
                                POINT p;
                                GetCursorPos(&p);
                                SetCursorPos(p.x, p.y);
                        }
                        // Fall through to WM_MOUSEMOVE.
                case WM_MOUSEMOVE:
                case WM_LBUTTONUP:
                        processed = FALSE;
                        break;

                case WM_NCHITTEST:
                        if (FlagSet(woFlags, WOF_VIEW_ONLY) &&  FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
                                return (HTTRANSPARENT);
                        else
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                        message, wParam, lParam);
                        break;

                case WM_COMMAND:
                        if ((HWND)lParam == screenID || (ZIL_NUMBERID)LOWORD(event.message.wParam) == numberID)
                                return (0);
                        else
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                        message, wParam, lParam);
                        break;

                case WM_NOTIFY:
                        if (((NMHDR *)lParam)->hwndFrom == screenID || (ZIL_NUMBERID)LOWORD(wParam) == numberID)
                                return (0);
                        else
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                        message, wParam, lParam);
                        break;

                default:
                        if (screenID)
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                        message, wParam, lParam);
                        break;
                }
        }

        if (!processed)
        {
                // Switch on the event type.
                ccode = LogicalEvent(event);
                switch (ccode)
                {
                case S_INITIALIZE:
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

                        // Environment Specific Flag Settings.
                        dwStyle = 0;
                        if (FlagSet(woFlags, WOF_BORDER) && !FlagSet(woStatus, WOS_OWNERDRAW))
                                dwStyle |= WS_BORDER;
                        if (parent)
                                dwStyle |= WS_CHILD | WS_VISIBLE;
                        if (FlagSet(woFlags, WOF_NON_SELECTABLE))
                                dwStyle |= WS_DISABLED;

                        RegionConvert(relative, (!FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
                                parent && !FlagSet(woFlags, WOF_NON_FIELD_REGION)) ? FALSE : TRUE);
                        break;

                case S_DEINITIALIZE:
                        // Clean up non-system type objects (e.g. list items).
                        if (!FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                                screenID = 0;
                        break;

                case S_REGISTER_OBJECT:
                        if (screenID)
                                DestroyWindow(screenID);
                        dwStyle |= BS_OWNERDRAW;
                        RegisterObject("UI_WINDOW_OBJECT", "BUTTON", &_objectCallback);
                        break;

                case S_CHANGED:
                case S_CREATE:
                        if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
                                break;
                        else if (parent)
                        {
                                parent->RegionMax(this);
                                if (parent->Inherited(ID_GROUP))
                                {
                                        trueRegion.top -= display->cellHeight / 4;
                                        trueRegion.bottom -= display->cellHeight / 4;
                                }
                        }
                        else
                                RegionMax(this);
                        break;

                case S_SIZE:
                        relative = event.region;
                        if (parent)
                        {
                                parent->RegionMax(this);
                                if (parent->Inherited(ID_GROUP))
                                {
                                        trueRegion.top -= display->cellHeight / 4;
                                        trueRegion.bottom -= display->cellHeight / 4;
                                }
                        }
                        else
                                RegionMax(this);

                        if (frameID)
                                MoveWindow(frameID, trueRegion.left, trueRegion.top,
                                        trueRegion.Width(), trueRegion.Height(), TRUE);
                        break;

                case S_CURRENT:
                case S_NON_CURRENT:
                        // Check the object status.
                        if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
                        {
                                lastPalette = LogicalPalette(ccode);
                                break;
                        }
                        else if (ccode == S_CURRENT)
                                woStatus |= WOS_CURRENT;
                        else
                                woStatus &= ~WOS_CURRENT;

                        if (Inherited(ID_IMAGE))        // Temporary Fix.
                                UpdateWindow(screenID);
                        // Continue to S_DISPLAY_ACTIVE.

                case S_DISPLAY_ACTIVE:
                case S_DISPLAY_INACTIVE:
                        lastPalette = LogicalPalette(ccode);
                        if (FlagSet(woStatus, WOS_WINDOWS_ACTION) || !screenID)
                                ;
                        else if(!FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                        {
                                if (FlagSet(woStatus, WOS_OWNERDRAW) &&
                                        (!parent || (!parent->Inherited(ID_LIST) &&
                                        FlagSet(parent->woStatus, WOS_OWNERDRAW))))
                                                ccode = DrawItem(event, ccode);
                        }
                        else if (ccode == S_CURRENT)
                        {
                                // Make sure the base window is current on the screen.
                                UI_WINDOW_OBJECT *root = NULL;
                                for (root = this; root->parent; root = root->parent)
                                        if (FlagSet(root->woAdvancedFlags, WOAF_TEMPORARY))
                                                break;
                                if (root != windowManager->First())
                                        break;

                                // Activate the current object.
                                if (!parent)
                                {
                                        woStatus |= WOS_WINDOWS_ACTION;
                                        SendMessage(hwnd, WM_ACTIVATE, 1, 0);
                                        if (FlagSet(woFlags, WOF_AUTO_CLEAR))
                                        {
                                                SendMessage(screenID, EM_SETSEL, 0, 0xFFFFFFFFL);
                                                SendMessage(screenID, EM_SCROLLCARET, 0, 0);
                                        }
                                        SetFocus(screenID);
                                        woStatus &= ~WOS_WINDOWS_ACTION;
                                }
                                else if (FlagSet(woFlags, WOF_AUTO_CLEAR))
                                {
                                        if (Inherited(ID_COMBO_BOX))
                                                SendMessage(screenID, CB_SETEDITSEL, 0, 0x7FFF0000L);
                                        else
                                        {
                                                SendMessage(screenID, EM_SETSEL, 0, 0xFFFFFFFFL);
                                                SendMessage(screenID, EM_SCROLLCARET, 0, 0);
                                        }
                                }
                        }
                        else if (ccode == S_DISPLAY_ACTIVE || ccode == S_DISPLAY_INACTIVE)
                        {
                                if (event.region == trueRegion)
                                {
                                        RECT rect = { 0, 0,
                                                event.region.Width() + 1, event.region.Height() + 1};
                                        SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
                                        InvalidateRect(screenID, &rect, FALSE);
                                }
                                else
                                {
                                        RECT rect = { event.region.left, event.region.top,
                                                event.region.right + 1, event.region.bottom + 1};
                                        SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
                                        InvalidateRect(screenID, &rect, TRUE);
                                }
                        }

                        // Call the associated user or validate function.
                        if (FlagSet(woStatus, WOS_INTERNAL_ACTION) ||
                                (ccode != S_CURRENT && ccode != S_NON_CURRENT))
                                break;

                        ccode = UserFunction(event, ccode);
                        break;

                case S_ADD_OBJECT:
                case S_SUBTRACT_OBJECT:
                        if (FlagSet(woStatus, WOS_WINDOWS_ACTION))
                        {
                                UI_EVENT event(ccode, 0);
                                event.data = this;
                                object = parent ? parent : windowManager;
                                object->woStatus |= WOS_WINDOWS_ACTION;
                                object->Event(event);
                                object->woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        else if (parent)
                                parent->Event(event);
                        break;

                case S_REDISPLAY:
                        if (screenID && parent && parent->Inherited(ID_LIST))
                        {
                                RECT rect;
                                SendMessage(screenID, LB_GETITEMRECT, ListIndex(), (LPARAM)&rect);
                                trueRegion.Assign(rect);
                                InvalidateRect(screenID, &rect, TRUE);
                        }
                        else if  (!FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                                break;
                        else if (screenID)
                        {
                                woStatus |= WOS_REDISPLAY;
                                SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
                                InvalidateRect(frameID, ZIL_NULLP(RECT), TRUE);
                                if (!parent && next && Next()->screenID)
                                {
                                        ZIL_SCREENID prevID = 0;
                                        if (previous)
                                                Previous()->Information(I_GET_FRAMEID, &prevID);
                                        SetWindowPos(frameID, prevID, 0, 0, 0, 0,
                                                SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);
                                }
                        }
                        else if (parent && !FlagSet(parent->woStatus, WOS_INTERNAL_ACTION))
                                return (parent->Event(event));
                        break;

                case S_CLOSE:
                case S_CLOSE_TEMPORARY:
                        if (event.type != ccode)
                                eventManager->Put(UI_EVENT(ccode));
                        else
                                ccode = S_UNKNOWN;
                        break;

                case S_DRAG_DEFAULT:
                        ccode = S_DRAG_MOVE_OBJECT;
                        // Fall through to S_DRAG_MOVE_OBJECT.
                case S_DRAG_MOVE_OBJECT:
                case S_DRAG_COPY_OBJECT:
                        if (!FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP) &&
                                windowManager->dragObject != this)
                                   eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
                        else if (windowManager->dragObject->Inherited(ID_LIST))
                        {
                                int count = 0;
                                UI_WINDOW_OBJECT *object;
                                for (windowManager->dragObject->Information(I_GET_FIRST, &object);
                                        object; object = object->Next())
                                                if (FlagSet(object->woStatus, WOS_SELECTED))
                                                        count++;

                                if (count == 1)
                                        eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
                                                DM_DRAG_MOVE : DM_DRAG_COPY);
                                else
                                   eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
                        }
                        else
                                eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
                                        DM_DRAG_MOVE : DM_DRAG_COPY);
                        break;

                case S_DROP_DEFAULT:
                        ccode = S_DROP_MOVE_OBJECT;
                        // Fall through to S_DROP_MOVE_OBJECT.
                case S_DROP_MOVE_OBJECT:
                case S_DROP_COPY_OBJECT:
                        {
                        ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
                        if (ccode == S_DROP_MOVE_OBJECT &&
                                windowManager->dragObject->Inherited(ID_LIST))
                        {
                                int count = 0;
                                UI_WINDOW_OBJECT *object;
                                UI_WINDOW_OBJECT *sObject;
                                for(windowManager->dragObject->Information(I_GET_FIRST, &object); object; object = object->Next())
                                        if (FlagSet(object->woStatus, WOS_SELECTED))
                                        {
                                                sObject = object;
                                                count++;
                                        }

                                if (count != 1)
                                        break;

                                // Else.
                                UI_EVENT dEvent(S_SUBTRACT_OBJECT);
                                dEvent.windowObject = sObject;
                                windowManager->dragObject->Event(dEvent);
                                windowManager->dragObject->Event(UI_EVENT(S_REDISPLAY));
                                sObject->Information(I_GET_TEXT, &text);
                                if (text)
                                        Information(I_SET_TEXT, text);
                                delete sObject;
                        }
                        else if (ccode == S_DROP_MOVE_OBJECT &&
                                windowManager->dragObject->parent &&
                                windowManager->dragObject->parent->Inherited(ID_LIST))
                        {
                                UI_WINDOW_OBJECT *list = windowManager->dragObject->parent;
                                UI_EVENT dEvent(S_SUBTRACT_OBJECT);
                                dEvent.windowObject = windowManager->dragObject;
                                list->Event(dEvent);
                                list->Event(UI_EVENT(S_REDISPLAY));
                                windowManager->dragObject->Information(I_GET_TEXT, &text);
                                if (text)
                                        Information(I_SET_TEXT, text);
                                delete windowManager->dragObject;
                        }
                        else
                        {
                                windowManager->dragObject->Information(I_GET_TEXT, &text);
                                if (text)
                                        Information(I_SET_TEXT, text);
                                if (ccode == S_DROP_MOVE_OBJECT)
                                        windowManager->dragObject->Information(I_SET_TEXT, _blankString);
                        }
                        }
                        break;

                case L_BEGIN_SELECT:
                case L_BEGIN_COPY_DRAG:
                case L_BEGIN_MOVE_DRAG:
                        if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                                CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                        message, wParam, lParam);
                        if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
                        {
                                SetCapture(screenID);

                                if (ccode == L_BEGIN_SELECT)
                                        ccode = L_BEGIN_MOVE_DRAG;
                                windowManager->dragObject = this;

                                EVENT_TYPE tCode;
                                if (FlagsSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
                                        tCode = (ccode == L_BEGIN_COPY_DRAG) ? DM_DRAG_COPY :
                                                DM_DRAG_MOVE;
                                else if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT))
                                        tCode = DM_DRAG_MOVE;
                                else
                                        tCode = DM_DRAG_COPY;

                                eventManager->DeviceImage(E_MOUSE, tCode);
                        }
                        break;

                case L_CONTINUE_SELECT:
                case L_CONTINUE_MOVE_DRAG:
                case L_CONTINUE_COPY_DRAG:
                        CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                message, wParam, lParam);
                        if (windowManager->dragObject)
                        {
                                // Find who the object overlaps and send them the message.
                                POINT p = { LOWORD(lParam), HIWORD(lParam) };
                                ClientToScreen(event.message.hwnd, &p);
                                HWND hWin = WindowFromPoint(p);

                                // Take care of prompts or groups inside of groups.
                                UI_WINDOW_OBJECT *pObject = NULL;
                                for (pObject = parent; pObject && pObject->Inherited(ID_GROUP);
                                        pObject = pObject->parent)
                                        ;

                                if (!hWin || hWin == GetDesktopWindow() ||
                                        (hWin == frameID && hWin != screenID))
                                                eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
                                else if (hWin == screenID || (windowManager->dragObject != this &&
                                        pObject && hWin == pObject->screenID))
                                {
                                        if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP) ||
                                                windowManager->dragObject == this)
                                        {
                                                EVENT_TYPE tCode;
                                                if (FlagsSet(windowManager->dragObject->woAdvancedFlags,
                                                        WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
                                                        tCode = (ccode == L_CONTINUE_MOVE_DRAG) ?
                                                                S_DRAG_MOVE_OBJECT :
                                                                ccode == L_CONTINUE_COPY_DRAG ?
                                                                S_DRAG_COPY_OBJECT : S_DRAG_DEFAULT;
                                                else if (FlagSet(windowManager->dragObject->woAdvancedFlags,
                                                        WOAF_MOVE_DRAG_OBJECT))
                                                                tCode = S_DRAG_MOVE_OBJECT;
                                                else
                                                                tCode = S_DRAG_COPY_OBJECT;
        
                                                if (windowManager->dragObject == this || !userFunction ||
                                                        UserFunction(event, tCode) == S_UNKNOWN)
                                                                Event(UI_EVENT(tCode, 0, event.position));
                                        }
                                        else
                                                eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
                                }
                                else if (GetWindowTask(hWin) == GetWindowTask(screenID) &&
                                        IsWindowEnabled(hWin))
                                {
                                        ScreenToClient(hWin, &p);
                                        LPARAM tLParam = p.x | ((LPARAM)p.y << 16);
                                        SendMessage(hWin, event.message.message, event.message.wParam,
                                                tLParam);
                                }
                                else
                                        eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
                        }
                        break;

                case L_END_SELECT:
                case L_END_COPY_DRAG:
                case L_END_MOVE_DRAG:
                        if (windowManager->dragObject)
                        {
                                // Find who the object overlaps and send them the message.
                                POINTS ps = MAKEPOINTS(lParam);
                                POINT p = { ps.x, ps.y };
                                ClientToScreen(event.message.hwnd, &p);
                                HWND hWin = WindowFromPoint(p);

                                if (GetCapture() == screenID)
                                {
                                        CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                                message, wParam, lParam);
                                        ReleaseCapture();
                                        if (hWin == screenID)
                                        {
                                                ccode = L_END_SELECT;
                                                windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                                                eventManager->DeviceImage(E_MOUSE, DM_VIEW);
                                                break;  
                                        }
                                }

                                // Take care of prompts or groups inside of groups.
                                UI_WINDOW_OBJECT *pObject = NULL;
                                for (pObject = parent; pObject && pObject->Inherited(ID_GROUP);
                                        pObject = pObject->parent)
                                        ;

                                if (hWin == screenID || (windowManager->dragObject != this &&
                                        pObject && hWin == pObject->screenID))
                                {
                                        if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP))
                                        {
                                                EVENT_TYPE tCode;
                                                if (FlagsSet(windowManager->dragObject->woAdvancedFlags,
                                                        WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT))
                                                        tCode = (ccode == L_END_MOVE_DRAG) ?
                                                                S_DROP_MOVE_OBJECT :
                                                                ccode == L_END_COPY_DRAG ?
                                                                S_DROP_COPY_OBJECT : S_DROP_DEFAULT;
                                                else if (FlagSet(windowManager->dragObject->woAdvancedFlags,
                                                        WOAF_MOVE_DRAG_OBJECT))
                                                                tCode = S_DROP_MOVE_OBJECT;
                                                else
                                                                tCode = S_DROP_COPY_OBJECT;

                                                if (!userFunction || UserFunction(event, tCode) == S_UNKNOWN)
                                                        Event(UI_EVENT(tCode, 0, event.position));
                                        }
                                }
                                else if (hWin == frameID)
                                        ;
                                else if (GetWindowTask(hWin) == GetWindowTask(screenID) &&
                                        IsWindowEnabled(hWin))
                                {
                                        ScreenToClient(hWin, &p);
                                        LPARAM tLParam = p.x | ((LPARAM)p.y << 16);
                                        SendMessage(hWin, event.message.message, event.message.wParam,
                                                tLParam);
                                }
                                
                                windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                                eventManager->DeviceImage(E_MOUSE, DM_VIEW);
                                break;
                        }
                        if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                                CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                        message, wParam, lParam);
                        // Fall through to L_SELECT.

                case L_SELECT:
                        {
                        EVENT_TYPE tCode = ccode;
                        if (event.type == E_MSWINDOWS && ccode == L_END_SELECT)
                                ccode = S_UNKNOWN;
                        else
                        {
                                ccode = UserFunction(event, L_SELECT);
                                if (screenID && Inherited(ID_LIST_ITEM))
                                {
                                        SendMessage(screenID, LB_SETSEL, FlagSet(woStatus, WOS_SELECTED), ListIndex());
                                        ccode = -1;
                                }
                        }
                        if (tCode == L_SELECT && parent && parent->Inherited(ID_TABLE_RECORD))
                                parent->Event(event);
                        else if (tCode == L_SELECT && ccode != S_ERROR)
                                Root(TRUE)->Event(event);
                        }
                        break;

                case L_VIEW:
                        windowManager->dragObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                        ccode = S_UNKNOWN;
                        break;

                case L_HELP:
                        if (parent && helpContext == NO_HELP_CONTEXT)
                                return (S_UNKNOWN);
                        else if (helpSystem)
                                helpSystem->DisplayHelp(windowManager, helpContext);
                        break;

                case L_PREVIOUS:
                case L_NEXT:
                        if (event.type == E_MSWINDOWS)
                                eventManager->Put(UI_EVENT(ccode, 0));
                        else
                                ccode = S_UNKNOWN;
                        break;

                case L_PGUP:
                case L_PGDN:
                        if (parent && !parent->Inherited(ID_TABLE_RECORD) &&
                                !parent->Inherited(ID_TABLE))
                        {
                                ccode = S_UNKNOWN;
                                break;
                        }
                        // Else fall through to L_LEFT.
                case L_LEFT:
                case L_RIGHT:
                case L_UP:
                case L_DOWN:
                        if (parent && !FlagSet(woStatus, WOS_WINDOWS_ACTION))
                                parent->Event(UI_EVENT(ccode));
                        else
                                ccode = S_UNKNOWN;
                        break;

                case E_KEY:
                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd, WM_CHAR, event.key.value, 0);
                        break;

                default:
                        ccode = S_UNKNOWN;
                        break;
                }

                if (ccode == S_UNKNOWN && event.type == E_MSWINDOWS &&
                        FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback, hwnd,
                                        message, wParam, lParam);
        }

        // Return the control code.
        return (ccode);
}

static char *Z_WIN = "UIW_WINDOW";
static char *Z_MDIWIN = "UIW_MDICHILD";

void UI_WINDOW_OBJECT::RegisterObject(char *name, char *baseName,
        WNDPROC *, ZIL_ICHAR *title, HMENU menu)
{
        // Make sure the field needs to be registered.
        if (screenID || FlagSet(woStatus, WOS_INTERNAL_ACTION))
                return;

        if (!baseName)
                baseName = (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && parent) ? Z_MDIWIN : Z_WIN;

#if defined(ZIL_UNICODE)
                ZIL_ICHAR *promotedName = ISOtoUNICODE(name);
                ZIL_ICHAR *promotedBaseName = ISOtoUNICODE(baseName);
#endif

        static int registeredClass = 0;
        static int mdiRegisteredClass = 0;

        if (!registeredClass)
        {

                zafProcL = GlobalAddAtom(ZIL_TEXT("ZAFPROCL"));
                zafProcH = GlobalAddAtom(ZIL_TEXT("ZAFPROCH"));

                WNDCLASS wndClass;
                wndClass.style = 0;
                wndClass.lpfnWndProc = DefWindowProc;
                wndClass.cbClsExtra = 0;
                wndClass.cbWndExtra = 0;
                wndClass.hInstance = display->hInstance;
                wndClass.hIcon = 0;
                wndClass.hCursor = 0;
#       if defined(ZIL_MSWINDOWS_CTL3D)
                wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
#       else
                wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
#       endif
                wndClass.lpszMenuName = 0;
#       if defined(ZIL_UNICODE)
                wndClass.lpszClassName = ISOtoUNICODE(Z_WIN);
#       else
                wndClass.lpszClassName = Z_WIN;
#       endif
                RegisterClass(&wndClass);

                registeredClass = 1;
        }

        _object = this;
        _controlID = numberID;

        // Create the object.
        woStatus |= WOS_INTERNAL_ACTION;
        DWORD exFlags = WS_EX_NOPARENTNOTIFY;
        if (FlagSet(woAdvancedFlags, WOAF_DIALOG_OBJECT))
        {
                dwStyle |= DS_MODALFRAME;
                exFlags |= WS_EX_DLGMODALFRAME;
        }
        if (!parent)
        {
//              ZIL_SCREENID parentId = windowManager->taskID;
                ZIL_SCREENID parentId = 0;
//              ZIL_SCREENID parentId = IsDialog() ? windowManager->taskID : 0;
#if defined(ZIL_UNICODE)
                screenID = CreateWindowEx(exFlags, promotedBaseName, title, dwStyle,
                        trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(),
                        parentId, menu, display->hInstance, ZIL_NULLP(VOID));
#else
                screenID = CreateWindowEx(exFlags, baseName, title, dwStyle,
                        trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(),
                        parentId, menu, display->hInstance, ZIL_NULLP(VOID));
#endif
                if (next && Next()->screenID)
                {
                        ZIL_SCREENID prevID = 0;
                        if (previous)
                Previous()->Information(I_GET_FRAMEID, &prevID);
                        SetWindowPos(screenID, prevID, 0, 0, 0, 0,
                                SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);
                }
                woStatus |= WOS_SYSTEM_OBJECT;
#if defined(ZIL_UNICODE)
                SetProp(screenID, (LPWSTR)zafProcL, (HANDLE)LOWORD(this));
                SetProp(screenID, (LPWSTR)zafProcH, (HANDLE)HIWORD(this));
#else
                SetProp(screenID, (LPCSTR)zafProcL, (HANDLE)LOWORD(this));
                SetProp(screenID, (LPCSTR)zafProcH, (HANDLE)HIWORD(this));
#endif

                defaultCallback = (WNDPROC)SetWindowLong(screenID, GWL_WNDPROC, (LONG)JumpProcedure);

        }
        else if (parent->Inherited(ID_COMBO_BOX))
        {
                screenID = parent->screenID;
                SendMessage(screenID, CB_INSERTSTRING,
                        (!next || !Next()->screenID) ? -1 : ListIndex(),
                        FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LPARAM)this :
                        title ? (LPARAM)title : (LPARAM)_blankString);
                if (FlagSet(GetVersion(), 0x80000000) &&                // Win32s Bug
                        FlagSet(parent->woStatus, WOS_OWNERDRAW))
                {
                        SendMessage(screenID, CB_SETITEMDATA, ListIndex(), (LPARAM)this);
                        SendMessage(screenID, CB_SETITEMHEIGHT, ListIndex(), (LPARAM)relative.Height());
                }
                if (FlagSet(woStatus, WOS_SELECTED))
                        SendMessage(screenID, CB_SETCURSEL, LOWORD(ListIndex()), 0);
        }
        else if (parent->Inherited(ID_LIST))
        {
                screenID = parent->screenID;
                SendMessage(screenID, LB_INSERTSTRING,
                        (!next || !Next()->screenID) ? -1 : ListIndex(),
                        FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LPARAM)this :
                        title ? (LPARAM)title : (LPARAM)_blankString);
                if (FlagSet(GetVersion(), 0x80000000) &&                // Win32s Bug
                        FlagSet(parent->woStatus, WOS_OWNERDRAW))
                {
                        SendMessage(screenID, LB_SETITEMDATA, ListIndex(), (LPARAM)this);
                        SendMessage(screenID, LB_SETITEMHEIGHT, ListIndex(), (LPARAM)relative.Height());
                }
                if (FlagSet(woStatus, WOS_SELECTED))
                        SendMessage(screenID, LB_SETSEL, TRUE, ListIndex());
        }
        else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
        {
                if (!mdiRegisteredClass)
                {
                        WNDCLASS mdiClass;
                        mdiClass.style = 0;
                        mdiClass.lpfnWndProc = DefMDIChildProc;
                        mdiClass.cbClsExtra = 0;
                        mdiClass.cbWndExtra = 0;
                        mdiClass.hInstance = display->hInstance;
                        mdiClass.hIcon = 0;
                        mdiClass.hCursor = 0;
#if defined(ZIL_MSWINDOWS_CTL3D)
                        mdiClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
#else
                        mdiClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
#endif
                        mdiClass.lpszMenuName = 0;
#if defined(ZIL_UNICODE)
                        mdiClass.lpszClassName = promotedBaseName;
#else
                        mdiClass.lpszClassName = baseName;
#endif
                        RegisterClass(&mdiClass);

                        mdiRegisteredClass = 1;
                }

#if defined(ZIL_UNICODE)
                MDICREATESTRUCTW mdiCreate;
                mdiCreate.szClass = promotedName;
#else
                MDICREATESTRUCT mdiCreate;
                mdiCreate.szClass = name;
#endif
                mdiCreate.szTitle = title;
                mdiCreate.hOwner = display->hInstance;
                mdiCreate.style = dwStyle;
                mdiCreate.x = trueRegion.left;
                mdiCreate.y = trueRegion.top;
                mdiCreate.cx = trueRegion.right - trueRegion.left + 1;
                mdiCreate.cy = trueRegion.bottom - trueRegion.top + 1;
                mdiCreate.lParam = 0;
                screenID = (ZIL_SCREENID)SendMessage(parent->screenID, WM_MDICREATE, 0, (LPARAM)&mdiCreate);
                woStatus |= WOS_SYSTEM_OBJECT;

#if defined(ZIL_UNICODE)
                SetProp(screenID, (LPWSTR)zafProcL, (HANDLE)LOWORD(this));
                SetProp(screenID, (LPWSTR)zafProcH, (HANDLE)HIWORD(this));
#else
                SetProp(screenID, (LPCSTR)zafProcL, (HANDLE)LOWORD(this));
                SetProp(screenID, (LPCSTR)zafProcH, (HANDLE)HIWORD(this));
#endif
                defaultCallback = (WNDPROC)SetWindowLong(screenID, GWL_WNDPROC, (LONG)JumpProcedure);

        }
        else
        {
                ZIL_SCREENID parentID;
                if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT) ||
                        !FlagSet(woFlags, WOF_NON_FIELD_REGION))
                        parentID = parent->screenID;
                else
                        parent->Information(I_GET_FRAMEID, &parentID);
#if defined(ZIL_UNICODE)
                screenID = CreateWindowEx(exFlags, promotedBaseName, title, dwStyle,
                        trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(),
                        parentID, (HMENU)numberID, display->hInstance, ZIL_NULLP(VOID));
#else
                screenID = CreateWindowEx(exFlags, baseName, title, dwStyle,
                        trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(),
                        parentID, (HMENU)numberID, display->hInstance, ZIL_NULLP(VOID));
#endif
                woStatus |= WOS_SYSTEM_OBJECT;
                if (FlagSet(woFlags, WOF_NON_SELECTABLE))
                        EnableWindow(screenID, FALSE);
                if (next && Next()->screenID)
                {
                        ZIL_SCREENID prevID = 0;
                        if (previous)
                Previous()->Information(I_GET_FRAMEID, &prevID);
                        SetWindowPos(screenID, prevID, 0, 0, 0, 0,
                                SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);
                }
#if defined(ZIL_UNICODE)
                SetProp(screenID, (LPWSTR)zafProcL, (HANDLE)LOWORD(this));
                SetProp(screenID, (LPWSTR)zafProcH, (HANDLE)HIWORD(this));
#else
                SetProp(screenID, (LPCSTR)zafProcL, (HANDLE)LOWORD(this));
                SetProp(screenID, (LPCSTR)zafProcH, (HANDLE)HIWORD(this));
#endif

#if defined(ZIL_MSWINDOWS_CTL3D)
#       if defined(ZIL_UNICODE)
                ZIL_ICHAR *_Z_WIN = ISOtoUNICODE(Z_WIN);
                ZIL_ICHAR *_Z_MDIWIN = ISOtoUNICODE(Z_MDIWIN);
                if (IsBordered() && strcmp(promotedName, _Z_WIN) && strcmp(promotedName, _Z_MDIWIN))
                        Ctl3dSubclassCtl(screenID);
                delete [] _Z_WIN;
                delete [] _Z_MDIWIN;
#       else
                if (IsBordered() && strcmp(name, Z_WIN) && strcmp(name,Z_MDIWIN))
                        Ctl3dSubclassCtl(screenID);
#       endif
#endif

defaultCallback = (WNDPROC)SetWindowLong(screenID, GWL_WNDPROC, (LONG)JumpProcedure);

        }
        if (FlagSet(woStatus, WOS_SYSTEM_OBJECT) && font != FNT_SYSTEM_FONT)
                SendMessage(screenID, WM_SETFONT, (WPARAM)UI_MSWINDOWS_DISPLAY::fontTable[font & ~FNT_IGNORE_UNDERSCORE], TRUE);

#if defined(ZIL_UNICODE)
        delete promotedName;
        delete promotedBaseName;
#endif

        woStatus &= ~WOS_INTERNAL_ACTION;
}

void UI_WINDOW_OBJECT::Modify(const UI_EVENT &event)
{
        // Make sure we can move the object.
        if (!parent || (event.type == L_SIZE && Inherited(ID_ICON)))
                return;

        WNF_FLAGS wnFlags = WNF_NO_FLAGS;
        Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);

        if (FlagSet(woFlags, WOF_NON_FIELD_REGION) && !(Inherited(ID_STATUS_BAR) ||
                (Inherited(ID_TOOL_BAR) && FlagSet(wnFlags, WNF_NO_WRAP)) ||
                Inherited(ID_TABLE_HEADER)))
                        return;

        ZIL_SCREENID modifyID;
        if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
                parent->Information(I_GET_FRAMEID, &modifyID);
        else
                modifyID = parent->screenID;

        // Determine move / size operation.
        POINT cOrg = { 0, 0 };
        POINT pOrg = { 0, 0 };
        ClientToScreen(screenID, &cOrg);
        ClientToScreen(modifyID, &pOrg);
        int xDiff = cOrg.x - pOrg.x - trueRegion.left;
        int yDiff = cOrg.y - pOrg.y - trueRegion.top;
        RECT sizeBoundary;
        int offset = (trueRegion.Width() > 10) ? 5 : 1;
        sizeBoundary.left = trueRegion.left + offset - xDiff;
        sizeBoundary.top = trueRegion.top + offset - yDiff;
        sizeBoundary.right = trueRegion.right - offset - xDiff;
        sizeBoundary.bottom = trueRegion.bottom - offset - yDiff;

        ZIL_RAW_CODE sizeFlags = 0;
        EVENT_TYPE operation = L_SIZE;
        if (event.type == L_MOVE ||
                (event.position.column >= sizeBoundary.left && event.position.line >= sizeBoundary.top &&
                event.position.column <= sizeBoundary.right && event.position.line <= sizeBoundary.bottom))
        {
                sizeFlags = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
                operation = L_MOVE;
        }
        if (event.position.column < sizeBoundary.left)
                sizeFlags |= M_LEFT_CHANGE;
        else if (event.position.column > sizeBoundary.right)
                sizeFlags |= M_RIGHT_CHANGE;
        if (event.position.line < sizeBoundary.top)
                sizeFlags |= M_TOP_CHANGE;
        else if (event.position.line > sizeBoundary.bottom)
                sizeFlags |= M_BOTTOM_CHANGE;

        // Determine the minimum height and width of the object.
        int minHeight = display->cellHeight;
        if (searchID != ID_WINDOW)
                minHeight -= (display->preSpace + display->postSpace);
        else
                minHeight *= 2;
        int minWidth = 2 * display->cellWidth;
        if (Inherited(ID_WINDOW) && !Inherited(ID_SCROLL_BAR) &&
                !Inherited(ID_LIST) && !Inherited(ID_GROUP) &&
                !Inherited(ID_SPIN_CONTROL) && !Inherited(ID_TABLE_HEADER))
                        minWidth *= 9;

        // Determine the absolute region.
        UI_REGION absolute;
        if (Inherited(ID_TOOL_BAR) || Inherited(ID_STATUS_BAR))
        {
                RECT rect;
                GetClientRect(modifyID, &rect);
                absolute.Assign(rect);
        }
        else
                parent->Information(I_GET_CLIPREGION, &absolute);

        // Adjust size flags if necessary.
        if (operation != L_MOVE)
        {
                if (Inherited(ID_BUTTON))
                {
                        BTF_FLAGS btFlags;
                        Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
                        if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_RADIO_BUTTON | BTF_CHECK_BOX))
                                sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
                }
                else if ((!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX) ||
                        Inherited(ID_SPIN_CONTROL)) && searchID < 3000)
                                sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
                else if (Inherited(ID_TABLE_RECORD))
                {
                        THF_FLAGS thFlags = THF_NO_FLAGS;
                        parent->Information(I_GET_FLAGS, &thFlags, ID_TABLE_HEADER);
                        if (FlagSet(thFlags, THF_COLUMN_HEADER))
                                sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
                        else if (FlagSet(thFlags, THF_ROW_HEADER))
                                sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
                        else if (FlagSet(thFlags, THF_CORNER_HEADER))
                                sizeFlags = 0;
                }
                else if (Inherited(ID_TABLE_HEADER))
                {
                        THF_FLAGS thFlags = THF_NO_FLAGS;
                        Information(I_GET_FLAGS, &thFlags, ID_TABLE_HEADER);
                        if (FlagSet(thFlags, THF_COLUMN_HEADER))
                                sizeFlags &= M_BOTTOM_CHANGE;
                        else if (FlagSet(thFlags, THF_ROW_HEADER))
                                sizeFlags &= M_RIGHT_CHANGE;
                        else
                                sizeFlags = 0;
                }
                else if (Inherited(ID_SCROLL_BAR))
                {
                        SBF_FLAGS sbFlags = SBF_NO_FLAGS;
                        Information(I_GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
                        if (!FlagSet(sbFlags, SBF_VERTICAL))
                        {
                                minHeight = relative.bottom - relative.top + 1;
                                sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
                        }
                        if (!FlagSet(sbFlags, SBF_HORIZONTAL))
                        {
                                minWidth = relative.right - relative.left + 1;
                                sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
                        }
                }
        }
        else if (Inherited(ID_TABLE_RECORD) || Inherited(ID_TOOL_BAR) ||
                Inherited(ID_STATUS_BAR))
                        return;

        if (!sizeFlags)
                return;

        // Convert to client coordinates.
        UI_POSITION origin = event.position;

        RECT newRegion = { trueRegion.left, trueRegion.top, trueRegion.right, trueRegion.bottom };
        RECT oldRegion = newRegion;
        int xJump = display->cellWidth;
        int yJump = display->cellHeight;
        SetCapture(screenID);

        HDC hDC = GetDC(modifyID);
        RECT r = { newRegion.left, newRegion.top, newRegion.right + 1, newRegion.bottom + 1 };
        DrawFocusRect(hDC, &r);
        WORD message;
        EVENT_TYPE ccode = 0;
        do
        {
                UI_EVENT event;
                eventManager->Get(event);
                message = event.message.message;
                int deltaX = 0, deltaY = 0;
                int update = FALSE;
                if (message == WM_LBUTTONDOWN || message == WM_NCLBUTTONDOWN ||
                        (message == WM_MOUSEMOVE && FlagSet(event.message.wParam, MK_LBUTTON)))
                {
                        // Check the absolute region.
                        if (event.position.column < absolute.left)
                                event.position.column = absolute.left;
                        if (event.position.column > absolute.right)
                                event.position.column = absolute.right;
                        if (event.position.line < absolute.top)
                                event.position.line = absolute.top;
                        if (event.position.line > absolute.bottom)
                                event.position.line = absolute.bottom;
                        deltaX =
                                event.position.column - origin.column;
                        deltaY =
                                event.position.line - origin.line;
                        update = TRUE;
                }
                else if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
                {
                        deltaX = newRegion.right - trueRegion.right;
                        deltaY = newRegion.bottom - trueRegion.bottom;
                        ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
                        switch (ccode)
                        {
                        case L_UP:
                        case L_DOWN:
                                if (ccode == L_DOWN)
                                        deltaY += yJump;
                                else
                                        deltaY -= yJump;
                                update = TRUE;
                                break;

                        case L_LEFT:
                        case L_RIGHT:
                                if (ccode == L_RIGHT)
                                        deltaX += xJump;
                                else
                                        deltaX -= xJump;
                                update = TRUE;
                                break;

                        default:
                                if (event.key.value == 0x000D)          // ENTER
                                        ccode = L_SELECT;
                                else if (event.key.value == 0x001B)     // ESCAPE
                                        ccode = L_CANCEL;
                                break;
                        }
                }
                // Send all user messages to the window manager.
                if (event.type > 9999)
                        windowManager->Event(event);

                // Update the new region.
                if (update)
                {
#if defined(ZIL_EDIT)
                        if (FlagSet(woStatus, WOS_EDIT_MODE))
                        {
                                // Check the absolute region.
                                if (FlagSet(sizeFlags, M_LEFT_CHANGE))
                                {
                                        if (trueRegion.left + deltaX < absolute.left)
                                                deltaX = absolute.left - trueRegion.left;
                                }
                                if (FlagSet(sizeFlags, M_TOP_CHANGE))
                                {
                                        if (trueRegion.top + deltaY < absolute.top)
                                                deltaY = absolute.top - trueRegion.top;
                                }
                        }

                        ZIL_INT32 miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
                        ZIL_INT32 miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

                        // Check for a cell boundary move or size.
                        if (FlagSet(woStatus, WOS_EDIT_MODE) && FlagSet(woFlags, WOF_MINICELL))
                        {
                                long value = origin.column + deltaX;
                                value = (int)(((value * miniDX) / display->cellWidth) / miniNX);
                                value = (value * miniNX * display->cellWidth + miniDX - 1) / miniDX;
                                deltaX = (int)(value - origin.column);

                                value = origin.line + deltaY;
                                value = (int)(((value * miniDY) / display->cellHeight) / miniNY);
                                value = (value * miniNY * display->cellHeight + miniDX - 1) / miniDY;
                                deltaY = (int)(value - origin.line);
                        }
                        else if (FlagSet(woStatus, WOS_EDIT_MODE) && !FlagSet(woFlags, WOF_PIXEL))
                        {
                                int value = (deltaX > 0) ? deltaX + display->cellWidth / 2 : deltaX - display->cellWidth / 2;
                                deltaX = value / display->cellWidth * display->cellWidth;
                                value = (deltaY > 0) ? deltaY + display->cellHeight / 2 : deltaY - display->cellHeight / 2;
                                deltaY = value / display->cellHeight * display->cellHeight;
                        }
#endif

                        if (FlagSet(sizeFlags, M_LEFT_CHANGE))
                                newRegion.left = trueRegion.left + deltaX;
                        if (FlagSet(sizeFlags, M_TOP_CHANGE))
                                newRegion.top = trueRegion.top + deltaY;
                        if (FlagSet(sizeFlags, M_RIGHT_CHANGE))
                                newRegion.right = trueRegion.right + deltaX;
                        if (FlagSet(sizeFlags, M_BOTTOM_CHANGE))
                                newRegion.bottom = trueRegion.bottom + deltaY;

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
                                RECT r1 = { oldRegion.left, oldRegion.top, oldRegion.right + 1, oldRegion.bottom + 1 };
                                RECT r2 = { newRegion.left, newRegion.top, newRegion.right + 1, newRegion.bottom + 1 };
                                DrawFocusRect(hDC, &r1);
                                DrawFocusRect(hDC, &r2);
                                oldRegion = newRegion;
                        }
                }
        } while (message != WM_LBUTTONUP && ccode != L_SELECT && ccode != L_CANCEL);
        r.left = newRegion.left;                r.top = newRegion.top;
        r.right = newRegion.right + 1;   r.bottom = newRegion.bottom + 1;
        DrawFocusRect(hDC, &r);
        ReleaseDC(modifyID, hDC);
        ReleaseCapture();

        if (ccode == L_CANCEL)
                return;                         // Do not change the object region.

        if (newRegion.left != trueRegion.left || newRegion.right != trueRegion.right ||
                newRegion.top != trueRegion.top || newRegion.bottom != trueRegion.bottom)
        {
                // Size and redisplay the object.
                relative.left += newRegion.left - trueRegion.left;
                relative.top += newRegion.top - trueRegion.top;
                relative.right += newRegion.right - trueRegion.right;
                relative.bottom += newRegion.bottom - trueRegion.bottom;
                RECT rect = { trueRegion.left - 1, trueRegion.top - 1, trueRegion.right + 2, trueRegion.bottom + 2 };

                if (Inherited(ID_TABLE_HEADER))
                {
                        UI_WINDOW_OBJECT *tableRecord;
                        Information(I_GET_FIRST, &tableRecord);
                        tableRecord->relative.right += newRegion.right - trueRegion.right;
                        tableRecord->relative.bottom += newRegion.bottom - trueRegion.bottom;
                        parent->Information(I_CHANGED_FLAGS, ZIL_NULLP(void));
                        Root()->Event(UI_EVENT(S_CHANGED));
                }
                else
                {
                        if (Inherited(ID_STATUS_BAR))
                                Event(UI_EVENT(S_SIZE, 0, relative));
                        Root()->Event(UI_EVENT(S_CHANGED));
                        if (!Inherited(ID_STATUS_BAR))
                                Event(UI_EVENT(S_CHANGED));
                        if (Inherited(ID_TOOL_BAR) || parent->Inherited(ID_TOOL_BAR))
                                parent->Event(UI_EVENT(S_REDISPLAY));
                        ZIL_SCREENID frameID;
                        Information(I_GET_FRAMEID, &frameID);
                        MoveWindow(frameID, trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(), TRUE);
                        if (parent->Inherited(ID_GROUP) && Inherited(ID_WINDOW))
                                InvalidateRect(screenID, ZIL_NULLP(RECT), FALSE);
                        InvalidateRect(modifyID, &rect, TRUE);
                }
        }
}

// ----- OS Specific Functions ----------------------------------------------

void UI_WINDOW_OBJECT::OSInitialize(void)
{
                dwStyle = 0;
                defaultCallback = ZIL_NULLF(WNDPROC);
}

void UI_WINDOW_OBJECT::OSUpdateSettings(ZIL_OBJECTID)
{
        woStatus &= ~WOS_REDISPLAY;
        if (screenID)
        {
                if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
                {
                        woStatus |= WOS_INTERNAL_ACTION;
                        ZIL_SCREENID frameID;
                        Information(I_GET_FRAMEID, &frameID);
                        Event(UI_EVENT(S_DEINITIALIZE));
                        DestroyWindow(frameID);
                        woStatus |= WOS_REDISPLAY;
                        woStatus &= ~WOS_INTERNAL_ACTION;
                }
        }
}

