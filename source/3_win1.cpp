//      Zinc Interface Library - W_WIN1.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"

extern ATOM zafProcL;
extern ATOM zafProcH;

// ----- UIW_WINDOW ---------------------------------------------------------

#if defined(ZIL_MSWINDOWS_CTL3D)
#include <ctl3d.h>
#endif

UI_WINDOW_OBJECT *UIW_WINDOW::Add(UI_WINDOW_OBJECT *object)
{
        int newObject = FALSE;

        // See if the new object exists or is already current.
        if (!object)
                return (ZIL_NULLP(UI_WINDOW_OBJECT));

        ZIL_SCREENID objectID;
        object->Information(I_GET_FRAMEID, &objectID);
        UI_WINDOW_OBJECT *cObject = ZIL_NULLP(UI_WINDOW_OBJECT);
        object->Information(I_GET_CURRENT, &cObject);
        if (!cObject && !object->Inherited(ID_GEOMETRY_MANAGER))
                for (object->Information(I_GET_FIRST, &cObject);
                        cObject && (FlagSet(cObject->woFlags, WOF_NON_SELECTABLE) ||
                        FlagSet(cObject->woAdvancedFlags, WOAF_NON_CURRENT));
                        cObject = cObject->Next())
                        ;
        if (cObject && !FlagSet(cObject->woStatus, WOS_SYSTEM_OBJECT))
                cObject = ZIL_NULLP(UI_WINDOW_OBJECT);

        int focus = (IsCurrent()) ? TRUE : FALSE;
        UI_WINDOW_OBJECT *focusParent = parent;

        while (focus && focusParent)
        {
                if (focusParent->IsCurrent())
                        focusParent = focusParent->parent;
                else
                        focus = FALSE;
        }

        if (object == current)
        {
                object->woStatus |= WOS_CURRENT;
                if (objectID && screenID != object->screenID &&
                        !FlagSet(object->woStatus, WOS_INTERNAL_ACTION | WOS_WINDOWS_ACTION) &&
                        !FlagSet(woFlags, WOF_SUPPORT_OBJECT) &&
                        GetFocus() != objectID && focus)        
                        if (cObject)
                        {
                                UI_EVENT tEvent(S_ADD_OBJECT);
                                tEvent.windowObject = cObject;
                                object->Event(tEvent);
                        }
                        else
                                SetFocus(objectID);
                return (object);
        }
        // Add the object to the list.
        else if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
                support.Index(object) == -1)
        {
                newObject = TRUE;
                support.Add(object);
                object->parent = this;
        }
        else if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
                ((!object->Previous() && !object->Next() && object != First()) ||
                 UI_LIST::Index(object) == -1))
        {
                newObject = TRUE;
                UI_LIST::Add(object);
                object->parent = this;

                if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT) || FlagSet(object->woStatus, WOS_CURRENT))
                {
                        if (current)
                        {
                                Current()->Event(UI_EVENT(S_NON_CURRENT, 0, Current()->trueRegion));
                                Current()->woStatus &= ~WOS_CURRENT;
                        }
                        object->woStatus |= WOS_CURRENT;
                        current = object;
                }
        }
        else if (!(FlagSet(object->woFlags, WOF_NON_SELECTABLE) ||
                FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
        {
                UI_WINDOW_OBJECT *tObject = Current();

                // Place MDI objects at the end of the list.
                if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
                {
                        UI_LIST::Subtract(object);
                        UI_LIST::Add(object);
                }
                // Make the old current field non-current.
                current = object;
                if (tObject && tObject != object)
                {
                        if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
                                tObject->woStatus &= ~WOS_CURRENT;
                        else
                                tObject->Event(UI_EVENT(S_NON_CURRENT, 0, object->trueRegion));
                }
                // Update the new current field.
                if (tObject != object)
                {
                        if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
                                object->woStatus |= WOS_CURRENT;
                        else
                        {
                                eventManager->DeviceState(E_CURSOR, D_OFF);
                                object->Event(UI_EVENT(S_CURRENT, 0, object->trueRegion));
                                current = object;
                        }
                        if (objectID && FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
                                object->Event(L_SELECT);
                }
        }

        // Initialize the object if its parent is already shown on the screen.
        if (screenID && !objectID && newObject)
        {
                if (!FlagSet(woStatus, WOS_WINDOWS_ACTION) && !Inherited(ID_MENU))
                {
                        UpdateWindow(screenID);
                        SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
                }
                object->Event(UI_EVENT(S_INITIALIZE, 0));
                object->Event(UI_EVENT(S_CREATE, 0));
                if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
                        clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
                if (!current && !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
                        !FlagSet(object->woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT))
                        current = object;
                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
                        FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
                {
                        SetFocus(objectID);
                        object->Event(UI_EVENT(S_CURRENT, 0, object->trueRegion));
                }
                UI_WINDOW_OBJECT *tDefaultObject;
                Root(TRUE)->Information(I_GET_DEFAULT_OBJECT, &tDefaultObject);
                if (object == tDefaultObject)
                        tDefaultObject->Event(UI_EVENT(S_SHOW_DEFAULT));
        }
        else if (Inherited(ID_MENU) && !screenID && parent && parent->screenID)
        {
                menuID = CreatePopupMenu();
                for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
                        object->Event(UI_EVENT(S_INITIALIZE));
                WORD wFlags = MF_STRING | MF_POPUP;
                if (FlagSet(parent->woFlags, WOF_NON_SELECTABLE))
                        wFlags |= MF_GRAYED;
                ZIL_ICHAR *text;
                parent->Information(I_GET_TEXT, &text);

#               define TEXT_ text
                ModifyMenu(parent->parent->menuID, parent->ListIndex(),
                        wFlags | MF_BYPOSITION, (UINT)screenID, TEXT_);
        }
        else if (objectID && screenID != object->screenID &&
                        !FlagSet(object->woStatus, WOS_INTERNAL_ACTION) &&
                        GetFocus() != objectID && FlagSet(woStatus, WOS_CURRENT))
                {
                        if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
                                FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
                                        SendMessage(screenID, WM_MDIACTIVATE, (WPARAM)objectID, 0);
                        else if (cObject)
                        {
                                UI_EVENT tEvent(S_ADD_OBJECT);
                                tEvent.windowObject = cObject;
                                object->Event(tEvent);
                        }
                        else if (object->Inherited(ID_PULL_DOWN_MENU))
                        {
                                PostMessage(frameID, WM_SYSCOMMAND, SC_KEYMENU, 0);
                        }
                        else
                                SetFocus(objectID);
                }
        else if (screenID && screenID == object->screenID &&
                        !FlagSet(object->woStatus, WOS_INTERNAL_ACTION))
        {
                if (Inherited(ID_COMBO_BOX))
                        SendMessage(screenID, CB_SETCURSEL, Index(object), 0);
                else if (Inherited(ID_LIST))
                        if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
                                SendMessage(screenID, LB_SETCARETINDEX, Index(object), 0);
                        else
                                SendMessage(screenID, LB_SETCURSEL, Index(object), 0);
        }

        // Return a pointer to the object.
        return (object);
}

EVENT_TYPE UIW_WINDOW::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
        ZIL_SCREENID screenID = this->screenID;
        UI_DISPLAY *display = this->display;
#endif
        UI_REGION region = trueRegion;

        lastPalette = LogicalPalette(ccode);

        display->VirtualGet(screenID, region);

        if (FlagSet(woFlags, WOF_BORDER))
                DrawBorder(screenID, region, FALSE, ccode);

        display->Rectangle(screenID, region, lastPalette, 0, TRUE);

        display->VirtualPut(screenID);

        return (TRUE);
}

#if defined(ZIL_WINNT)
static WNDPROC _windowCallback = ZIL_NULLF(WNDPROC);
static WNDPROC _mdiFrameCallback = ZIL_NULLF(WNDPROC);
static WNDPROC _mdiChildCallback = ZIL_NULLF(WNDPROC);
#else
static int _windowOffset = -1;
static FARPROC _windowCallback = (FARPROC)DefWindowProc;
LRESULT FAR PASCAL _export WindowJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
        // Call the user class object event function.
#if defined(ZIL_UNICODE) && defined(ZIL_WINNT)
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPWSTR)zafProcL), GetProp(hWnd, (LPWSTR)zafProcH));
#else
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPCSTR)zafProcL), GetProp(hWnd, (LPCSTR)zafProcH));
#endif
        LRESULT ccode = object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam));

        // Check for a destroyed window.
        if (wMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE &&
                !object->screenID && !object->parent &&
                !FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
                delete object;
        return (ccode);
}
static FARPROC _windowJumpInstance = (FARPROC)WindowJumpProcedure;

static int _mdiFrameOffset = -1;
static FARPROC _mdiFrameCallback = ZIL_NULLF(FARPROC);
LRESULT FAR PASCAL _export MDIFrameJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
        // Call the user class object event function.
#if defined(ZIL_UNICODE) && defined(ZIL_WINNT)
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPWSTR)zafProcL), GetProp(hWnd, (LPWSTR)zafProcH));
#else
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPCSTR)zafProcL), GetProp(hWnd, (LPCSTR)zafProcH));
#endif
        LRESULT ccode = object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam));

        // Check for a destroyed window.
        if (wMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE &&
                !object->screenID && !object->parent &&
                !FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
                delete object;
        return (ccode);
}
static FARPROC _mdiFrameJumpInstance = (FARPROC)MDIFrameJumpProcedure;

static int _mdiChildOffset = -1;
static FARPROC _mdiChildCallback = (FARPROC)DefMDIChildProc;
LRESULT FAR PASCAL _export MDIChildJumpProcedure(HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
#if defined(ZIL_UNICODE) && defined(ZIL_WINNT)
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPWSTR)zafProcL), GetProp(hWnd, (LPWSTR)zafProcH));
#else
        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPCSTR)zafProcL), GetProp(hWnd, (LPCSTR)zafProcH));
#endif
//      UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _mdiChildOffset);
        return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _mdiChildJumpInstance = (FARPROC)MDIChildJumpProcedure;
#endif

static UI_WINDOW_OBJECT *_client;

static int _offset = GWL_USERDATA;
#       define DS_FIXUP

LRESULT FAR PASCAL DS_FIXUP ClientJumpProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
        LRESULT result;
        switch (wMsg)
        {
                case WM_SYSCOMMAND:
                        if ((wParam & 0xFFF0) == SC_VSCROLL ||
                                (wParam & 0xFFF0) == SC_HSCROLL)
                        {
                                DefWindowProc(hWnd, wMsg, wParam, lParam);
                                break;
                        }
                        // Else fall through. (event to frame.)

                case WM_MOUSEMOVE:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_LBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_RBUTTONDBLCLK:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_MBUTTONDBLCLK:
#if defined(ZIL_EDIT)
                case WM_NCLBUTTONDOWN:
#endif

                case ZM_CONVERTPOINT:
                case WM_HSCROLL:
                case WM_VSCROLL:
                case WM_COMMAND:
                case WM_DRAWITEM:
                case WM_ERASEBKGND:
                case WM_CTLCOLORBTN:
                case WM_CTLCOLORDLG:
                case WM_CTLCOLOREDIT:
                case WM_CTLCOLORLISTBOX:
                case WM_CTLCOLORMSGBOX:
                case WM_CTLCOLORSCROLLBAR:
                case WM_CTLCOLORSTATIC:
                case WM_COMPAREITEM:
                case WM_MEASUREITEM:
                case WM_KEYDOWN:
                case WM_KEYUP:
                        {
                        // Pass event to client window.
#if defined(ZIL_UNICODE) 
                        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPWSTR)(zafProcL)), GetProp(hWnd, (LPWSTR)(zafProcH)));
#else
                        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPCSTR)(zafProcL)), GetProp(hWnd, (LPCSTR)(zafProcH)));
#endif
                        if (!object)
                        {
                                UI_ERROR_SYSTEM::Beep();
                                return DefWindowProc(hWnd, wMsg, wParam, lParam);
                        }
                        result = object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam));
                        }
                        break;

                case WM_PAINT:
                        {
                        // Allow OWNERDRAW windows to paint the client.
#if defined(ZIL_UNICODE) 
                        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPWSTR)(zafProcL)), GetProp(hWnd, (LPWSTR)(zafProcH)));
#else
                        UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)MAKELONG(GetProp(hWnd, (LPCSTR)(zafProcL)), GetProp(hWnd, (LPCSTR)(zafProcH)));
#endif
                        if (FlagSet(object->woStatus, WOS_OWNERDRAW))
                        {
                                // Temporary Fix.
                                if (object->Inherited(ID_TABLE))
                                        result = object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam));
                                else
                                        result = object->UI_WINDOW_OBJECT::Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam));
                                break;
                        }
                        // Else fall through to default.
                        }

                default:
                        result = DefWindowProc(hWnd, wMsg, wParam, lParam);
                        break;
        }

        return (result);
}

LRESULT FAR PASCAL DS_FIXUP ClientInitProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
#if defined(ZIL_UNICODE) 
        SetProp(hWnd, (LPWSTR)(zafProcL), (HANDLE)LOWORD(_client));
        SetProp(hWnd, (LPWSTR)(zafProcH), (HANDLE)HIWORD(_client));
#else
        SetProp(hWnd, (LPCSTR)(zafProcL), (HANDLE)LOWORD(_client));
        SetProp(hWnd, (LPCSTR)(zafProcH), (HANDLE)HIWORD(_client));
#endif
        SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ClientJumpProcedure);
        return (DefWindowProc(hWnd, wMsg, wParam, lParam));
}

EVENT_TYPE UIW_WINDOW::Event(const UI_EVENT &event)
{
        UI_WINDOW_OBJECT *object;

        EVENT_TYPE ccode = S_UNKNOWN;
        int processed = FALSE;

        if (event.type == E_MSWINDOWS)
        {
                UINT message = event.message.message;
                WPARAM wParam = event.message.wParam;
                LPARAM lParam = event.message.lParam;

                processed = TRUE;

                // Switch on the windows message.
                switch (message)
                {
                case ZM_CONVERTNCPOINT:
                        {
                        UI_POSITION *position = (UI_POSITION *)lParam;
                        POINT p = { position->column, position->line };
                        ScreenToClient(screenID, &p);
                        position->column = p.x + trueRegion.left;
                        position->line = p.y + trueRegion.top;
                        }
                        break;

                case WM_NCDESTROY:
                        if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                        {
                                ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                        event.message.message, wParam, lParam);
                                screenID = 0;
                                frameID = 0;
                        }
                        else
                                ccode = UI_WINDOW_OBJECT::Event(event);

                        if (!parent &&
                                !FlagSet(woStatus, WOS_INTERNAL_ACTION))        // Avoid Removing on I_CHANGED_FLAGS.
                        {
                                woStatus |= WOS_WINDOWS_ACTION;
                                *windowManager - this;
                                woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        break;

                case WM_DESTROY:
                        {
                        for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First();
                                object; object = object->Next())
                                if (object->Inherited(ID_MENU))
                                {
                                        object->Event(event);
                                        object->screenID = 0;
                                }
                                else if (!FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
                                        object->screenID = 0;
                        for (object = First(); object; object = object->Next())
                                if (!FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
                                        object->screenID = 0;
                        if (parent || ! FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                ccode = UI_WINDOW_OBJECT::Event(event);
                                frameID = 0;
                        }
                        else
                                ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                        event.message.message, wParam, lParam);
                        }
                        break;

                case WM_COMPAREITEM:
                        {
                        COMPAREITEMSTRUCT *compare = (COMPAREITEMSTRUCT *)lParam;
                        UI_WINDOW_OBJECT *item1 = (UI_WINDOW_OBJECT *)compare->itemData1;
                        UI_WINDOW_OBJECT *item2 = (UI_WINDOW_OBJECT *)compare->itemData2;
                        if (item1->parent && item1->parent != this)
                                return (item1->parent->Event(event));
                        else if (compareFunction)
                                ccode = (*compareFunction)((void *)item1, (void *)item2);
                        else
                                return (-1);
                        }
                        break;

                case WM_MEASUREITEM:
                        {
                        MEASUREITEMSTRUCT *measure = (MEASUREITEMSTRUCT *)lParam;
                        for (object = First(); object; object = object->Next())
                        {
                                if (object->numberID == measure->CtlID &&
                                        FlagSet(object->woStatus, WOS_OWNERDRAW))
                                        return (object->Event(event));
                        }
                        processed = FALSE;
                        }
                        break;

                case WM_DRAWITEM:
                        {
                        DRAWITEMSTRUCT *draw = (DRAWITEMSTRUCT *)lParam;
                        if (draw->CtlType == ODT_BUTTON)
                                ccode = SendMessage(draw->hwndItem, WM_DRAWITEM, 0, (LPARAM)draw);
                        else if ((int)draw->itemID >= 0)
                        {
                                object = (UI_WINDOW_OBJECT *)draw->itemData;
                                ccode = object->Event(event);
                        }
                        }
                        break;

                case WM_GETMINMAXINFO:
                        if (parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                POINT *rgpt = (POINT *)lParam;
                                rgpt[1].x = parent->trueRegion.right - parent->trueRegion.left + 1;
                                rgpt[1].y = parent->trueRegion.bottom - parent->trueRegion.top + 1;
                                rgpt[2].x = rgpt[2].y = 0;
                        }
                        else
                                processed = FALSE;
                        break;

                case WM_INITMENUPOPUP:
                        // Check the system-button for invalid options.
                        {
                        HMENU nmenu = (HMENU)wParam;
                        if (nmenu != menu && HIWORD(lParam))
                        {
                                EnableMenuItem(nmenu, SC_RESTORE, MF_BYCOMMAND |
                                        (!FlagSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED) ?
                                        MF_GRAYED : MF_ENABLED));
                                EnableMenuItem(nmenu, SC_MOVE, MF_BYCOMMAND |
                                        ((FlagSet(woAdvancedFlags, WOAF_NO_MOVE) || FlagSet(woStatus, WOS_MAXIMIZED)) ?
                                        MF_GRAYED : MF_ENABLED));
                                EnableMenuItem(nmenu, SC_SIZE, MF_BYCOMMAND |
                                        ((FlagSet(woAdvancedFlags, WOAF_NO_SIZE) || FlagSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED)) ?
                                        MF_GRAYED : MF_ENABLED));
                                EnableMenuItem(nmenu, SC_CLOSE, MF_BYCOMMAND |
                                        (FlagSet(woAdvancedFlags, WOAF_LOCKED) || (parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))) ?
                                        MF_GRAYED : MF_ENABLED);
                        }
                        else
                                processed = FALSE;
                        }
                        break;

                case WM_ERASEBKGND:
                        if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
                        {
                                HDC screenHDC = GetDC(ID_SCREEN);
                                HDC objectHDC = (HDC)wParam;
                                SetBkColor(objectHDC, GetBkColor(screenHDC));
                                SetBkMode(objectHDC, GetBkMode(screenHDC));
                                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                        DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                                WM_ICONERASEBKGND, wParam, lParam);
                                else
                                {
                                        UI_EVENT tEvent = event;
                                        tEvent.message.message = WM_ICONERASEBKGND;
                                        UI_WINDOW_OBJECT::Event(tEvent);
                                }
                                ReleaseDC(ID_SCREEN, screenHDC);
                                ccode = TRUE;
                        }
                        else if (FlagSet(woStatus, WOS_OWNERDRAW))
                                ccode = TRUE;
                        else
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        break;

                case WM_PAINT:
                        if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
                        {
                                PAINTSTRUCT ps;
                                HDC objectHDC = BeginPaint(frameID, &ps);
                                SendMessage(frameID, WM_ERASEBKGND, (WPARAM)objectHDC, 0);
                                RECT rect;
                                GetClientRect(frameID, &rect);
                                rect.left = (rect.right - GetSystemMetrics(SM_CXICON)) / 2;
                                rect.top = (rect.bottom - GetSystemMetrics(SM_CYICON)) / 2;
                                DrawIcon(objectHDC, rect.left, rect.top, icon);
                                EndPaint(frameID, &ps);
                        }
                        else if (!frameID || screenID == frameID)
                                processed = FALSE;
                        else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                        event.message.message, wParam, lParam);
                        else
                                ccode = CallWindowProc((WINDOWSPROC)defaultCallback, frameID, message, wParam, lParam);
                        break;

                case WM_NCPAINT:
                        if (parent && parent->Inherited(ID_NOTEBOOK))
                                ccode = 0;
//-----------------------------------
#if defined(ZIL_MSWINDOWS_CTL3D)
                        else if (FlagSet(woAdvancedFlags, WOAF_DIALOG_OBJECT))
                                ccode = CallWindowProc((WINDOWSPROC)Ctl3dDlgFramePaint,
                                        event.message.hwnd, message, wParam, lParam);
#endif
//-----------------------------------
                        else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                        event.message.message, wParam, lParam);
                        else
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        break;

                case WM_QUERYDRAGICON:
                        if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
                                ccode = (EVENT_TYPE)icon;
                        else
                                processed = FALSE;
                        break;

                case WM_CHILDACTIVATE:
                        UI_WINDOW_OBJECT::Event(event);
                        if (!FlagSet(woStatus, WOS_WINDOWS_ACTION))
                        {
                                woStatus |= WOS_WINDOWS_ACTION;
                                UI_WINDOW_OBJECT::Event(UI_EVENT(S_ADD_OBJECT));
                                woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        break;

                case WM_SETFOCUS:
                        if (!parent)
                        {
                                woStatus |= WOS_WINDOWS_ACTION;
                                windowManager->Add(this);
                                woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        if (current)
                        {
                                Event(UI_EVENT(S_CURRENT));
                                object = Current();
                                if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
                                        UIW_WINDOW::Add((UI_WINDOW_OBJECT *)object->userObject);
                                else if (screenID != object->screenID)
                                {
                                        ZIL_SCREENID objectID;
                                        object->Information(I_GET_FRAMEID, &objectID);
                                        SetFocus(objectID);
                                        break;
                                }
                        }
                        processed = FALSE;
                        break;

                case WM_VSCROLL:
                case WM_HSCROLL:
                        {
                        ZIL_SCREENID scrollBar = (ZIL_SCREENID)lParam;
                        int nPos = HIWORD(wParam);
                        int scrollCode = (int) LOWORD(wParam);
                        if (!scrollBar)
                        {
                                UI_EVENT sEvent(message == WM_VSCROLL ? S_VSCROLL : S_HSCROLL);
                                UI_SCROLL_INFORMATION sInfo = (message == WM_VSCROLL) ?
                                        vScrollInfo : hScrollInfo;
                                int delta = 0;
                                switch(scrollCode)
                                {
                                case SB_LINEUP:
                                        delta = -sInfo.delta;
                                        break;
                                case SB_LINEDOWN:
                                        delta = sInfo.delta;
                                        break;
                                case SB_PAGEUP:
                                        delta = -sInfo.showing;
                                        break;
                                case SB_PAGEDOWN:
                                        delta = sInfo.showing;
                                        break;
                                case SB_THUMBPOSITION:
                                case SB_THUMBTRACK:
                                        delta = nPos - sInfo.current;
                                        break;
                                default:
                                        break;
                                }

                                if (sInfo.current + delta < sInfo.minimum)
                                        delta = sInfo.minimum - sInfo.current;
                                else if (sInfo.current + delta > sInfo.maximum)
                                        delta = sInfo.maximum - sInfo.current;

                                if (delta)
                                {
                                        sEvent.scroll.delta = delta;
                                        Event(sEvent);
                                }
                                break;
                        }
                        NUMBERID tNumberID =
                                (NUMBERID)GetWindowLong(scrollBar, GWL_ID);
                        UI_WINDOW_OBJECT *object =
                                (UI_WINDOW_OBJECT *)Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&tNumberID));
                        if (object)
                                ccode = object->Event(event);
                        }
                        break;

                case WM_COMMAND:
                        {
                        WORD tNumberID = LOWORD(wParam);
                        ZIL_SCREENID tScreenID = (ZIL_SCREENID)lParam;

                        if (tNumberID >= 0xF000)
                                if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                                        processed = FALSE;
                                else
                                        SendMessage(event.message.hwnd, WM_SYSCOMMAND, wParam, lParam);
                        else
                        {
                                object = (UI_WINDOW_OBJECT *)Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&tNumberID));
                                if (object && object != this)
                                {
                                        if (tScreenID)
                                                ccode = object->Event(event);
                                        else
                                        {
                                                // object is a menu item.
                                                UI_EVENT aEvent(S_ADD_OBJECT);
                                                aEvent.windowObject = object;
                                                object->parent->Event(aEvent);
                                                ccode = object->Event(UI_EVENT(L_SELECT, 0));
                                        }
                                }
                                processed = FALSE;
                        }
                        }
                        break;

                case WM_SYSCOMMAND:
                        {
                        processed = FALSE;
                        WORD command = wParam;
                        if (command < 0xF000)
                        {
                                object = (UI_WINDOW_OBJECT *)Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&command));
                                if (object)
                                        ccode = object->Event(UI_EVENT(L_SELECT));
                        }
                        else
                        {
                                command &= 0xFFF0;
                                switch (command)
                                {
                                case SC_RESTORE:
                                        woStatus &= ~(WOS_MINIMIZED | WOS_MAXIMIZED);
                                        woStatus |= WOS_REDISPLAY;
                                        break;

                                case SC_MOVE:
                                        if (FlagSet(woAdvancedFlags, WOAF_NO_MOVE))
                                                processed = TRUE;
                                        break;

                                case SC_SIZE:
                                        if (FlagSet(woAdvancedFlags, WOAF_NO_SIZE))
                                                processed = TRUE;
                                        break;

                                case SC_MINIMIZE:
                                case SC_MAXIMIZE:
                                        restore = relative;
                                        if (FlagSet(woAdvancedFlags, WOAF_MODAL))
                                        {
                                                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                                        ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                                                message, wParam, lParam);
                                                else
                                                        ccode = UI_WINDOW_OBJECT::Event(event);
                                                BringWindowToTop(frameID);
                                                processed = TRUE;
                                        }
                                        break;

                                case SC_CLOSE:
                                        if (screenID == windowManager->screenID)
                                        {
                                                eventManager->Put(UI_EVENT(L_EXIT_FUNCTION, 0));
                                                processed = TRUE;
                                        }
                                        else if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
                                                processed = TRUE;
                                        else if (FlagSet(woAdvancedFlags, WOAF_MODAL))
                                        {
                                                for (UI_WINDOW_OBJECT *tObject = Next(); tObject; tObject = tObject->Next())
                                                {
                                                        ZIL_SCREENID tFrameID;
                                                        tObject->Information(I_GET_FRAMEID, &tFrameID);
                                                        EnableWindow(tFrameID, TRUE);
                                                        if (FlagSet(tObject->woAdvancedFlags, WOAF_MODAL))
                                                                break;
                                                }
                                        }
                                        break;

                                default:
                                        processed = FALSE;
                                }
                        }
                        }
                        break;

                case WM_SYSCHAR:
                case WM_MENUCHAR:
                        {
                        woStatus |= WOS_WINDOWS_ACTION;
                        processed = FALSE;

                        ZIL_RAW_CODE hotCharKey = wParam;
                        hotCharKey = ToLower(hotCharKey);
                        hotCharKey += (ZIL_RAW_CODE)((lParam & 0x20000000L) >> 18);     // Add the ALT key

                        ZIL_LOGICAL_EVENT hotChar = 0;
                        if (hotCharKey & 0x800)
                                for (int i = 0; hotKeyMapTable[i].objectID != ID_END; i++)
                                {
                                        if (hotKeyMapTable[i].rawCode == hotCharKey)
                                                hotChar = (unsigned char)hotKeyMapTable[i].logicalValue;
                                }
                        else if (FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS))                                
                                hotChar = hotCharKey;
                        if (!hotChar)
                        {
                                processed = TRUE;
                                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                        ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                                event.message.message, wParam, lParam);
                                else
                                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback,
                                                event.message.hwnd, event.message.message, wParam, lParam);
                                woStatus &= ~WOS_WINDOWS_ACTION;
                                break;
                        }

                        for (object = (UI_WINDOW_OBJECT *)support.First(); object && !processed; object = object->Next())
                                if (object->hotKey == HOT_KEY_SUB_WINDOW &&
                                        !FlagSet(object->woStatus, WOS_WINDOWS_ACTION))
                                {
                                        ccode = object->Event(event);
                                        if (ccode == 0x00010000L)
                                        {
                                                UIW_WINDOW::Add(object);
                                                processed = TRUE;
                                        }
                                }
                                else if (hotChar == (ZIL_LOGICAL_EVENT)object->hotKey &&
                                        !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
                                {
                                        if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
                                                UIW_WINDOW::Add(object);
                                        if (object->Inherited(ID_BUTTON) || object->Inherited(ID_ICON))
                                                object->Event(UI_EVENT(L_SELECT, 0));
                                        processed = TRUE;
                                        ccode = 0x00010000L;
                                        woStatus &= ~WOS_WINDOWS_ACTION;
                                        break;
                                }
                        for (object = parent || !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? First() : Last();
                                object && !processed;
                                object = parent || !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? object->Next() : object->Previous())
                                        if (object->hotKey == HOT_KEY_SUB_WINDOW &&
                                                !FlagSet(object->woStatus, WOS_WINDOWS_ACTION))
                                        {
                                                ccode = object->Event(event);
                                                if (ccode == 0x00010000L)
                                                {
                                                        UIW_WINDOW::Add(object);
                                                        processed = TRUE;
                                                }
                                        }
                                        else if (hotChar == (ZIL_LOGICAL_EVENT)object->hotKey &&
                                                !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
                                        {
                                                if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
                                                        UIW_WINDOW::Add(object);
                                                if (object->Inherited(ID_BUTTON) || object->Inherited(ID_ICON))
                                                        object->Event(UI_EVENT(L_SELECT, 0));
                                                processed = TRUE;
                                                ccode = 0x00010000L;
                                                woStatus &= ~WOS_WINDOWS_ACTION;
                                                break;
                                        }
                        if (processed)
                        {
                                woStatus &= ~WOS_WINDOWS_ACTION;
                                break;
                        }
                        else if (!parent)
                                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                                        ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                                event.message.message, wParam, lParam);
                                else
                                        ccode = CallWindowProc((WINDOWSPROC)defaultCallback, frameID,
                                                event.message.message, wParam, lParam);
                        else if (parent && !FlagSet(parent->woStatus, WOS_WINDOWS_ACTION))
                                UI_WINDOW_OBJECT::Event(event);
                        processed = TRUE;
                        woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        break;

                case WM_MENUSELECT:
                        {
                        object = Current();
                        if (!object || (!object->Inherited(ID_PULL_DOWN_MENU) && !object->Inherited(ID_SYSTEM_BUTTON)))
                        {
                                for (object = (UI_WINDOW_OBJECT *)support.Last(); object; object = object->Previous())
                                        if (object->Inherited(ID_PULL_DOWN_MENU) || object->Inherited(ID_SYSTEM_BUTTON))
                                        {
                                                object->userObject = current;
                                                object->woStatus |= WOS_INTERNAL_ACTION;
                                                UIW_WINDOW::Add(object);
                                                object->woStatus &= ~WOS_INTERNAL_ACTION;
                                                break;
                                        }
                        }

                        WORD wID = LOWORD(wParam);
                        WORD wMenuFlags = HIWORD(wParam);
                        object = Current();
                        if (wMenuFlags == 0xFFFF && wID == 0 &&
                                object && (object->Inherited(ID_PULL_DOWN_MENU) ||
                                object->Inherited(ID_SYSTEM_BUTTON)))
                                {
                                        UIW_WINDOW::Add((UI_WINDOW_OBJECT *)object->userObject);
                                        // If current was NULL restore it to NULL
                                        if (!object->userObject)
                                        current = (UI_WINDOW_OBJECT *)object->userObject;
                                }
                        else
                                if (object)
                                        object->Event(event);
                        }
                        break;

                case WM_SETCURSOR:                                                                                                              // ENHANCE.General
                        {
                        WORD hiTest = LOWORD(lParam);
                        if (FlagSet(woAdvancedFlags, WOAF_NO_SIZE) &&
                                (hiTest == HTLEFT || hiTest == HTTOP ||
                                hiTest == HTRIGHT || hiTest == HTBOTTOM ||
                                hiTest == HTTOPLEFT || hiTest == HTTOPRIGHT ||
                                hiTest == HTBOTTOMLEFT || hiTest == HTBOTTOMRIGHT))
                                SetCursor(LoadCursor(0, IDC_ARROW));
                        else
                                processed = FALSE;
                        }
                        break;

                case WM_MOVE:
                        if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                processed = FALSE;
                                RECT rect;
                                GetWindowRect(frameID ? frameID : screenID, &rect);
                                if (parent)
                                {
                                        POINT p = { rect.left, rect.top };
                                        ScreenToClient(parent->screenID, &p);
                                        rect.right -= rect.left - p.x;
                                        rect.bottom -= rect.top - p.y;
                                        rect.left = p.x;
                                        rect.top = p.y;
                                }
                                if (rect.left != trueRegion.left || rect.top != trueRegion.top)
                                {
                                        UI_EVENT tEvent(S_MOVE);
                                        tEvent.position.column = rect.left - trueRegion.left;
                                        tEvent.position.line = rect.top - trueRegion.top;
                                        Event(tEvent);
                                        trueRegion.Assign(rect);
                                }

#if defined(ZIL_EDIT)
                                if (FlagSet(woStatus, WOS_EDIT_MODE) && (trueRegion.left < 0 || trueRegion.top < 0))
                                {
                                        int width = trueRegion.Width();
                                        int height = trueRegion.Height();
                                        if (trueRegion.left < 0)
                                                trueRegion.left = 0;
                                        if (trueRegion.top < 0)
                                                trueRegion.top = 0;
                                        trueRegion.right = trueRegion.left + width - 1;
                                        trueRegion.bottom = trueRegion.top + height - 1;
                                        MoveWindow(frameID ? frameID : screenID, trueRegion.left, trueRegion.top,
                                                width, height, TRUE);
                                }
#endif
                                relative = trueRegion;
                        }
                        break;

                case WM_SIZE:
                        if (FlagSet(woStatus, WOS_INTERNAL_ACTION) &&
                                !(parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT)))
                                        break;
                        woStatus &= ~(WOS_MINIMIZED | WOS_MAXIMIZED);
                        if (wParam == SIZEICONIC)
                                woStatus |= WOS_MINIMIZED;
                        else if (wParam == SIZEFULLSCREEN)
                                woStatus |= WOS_MAXIMIZED;
                        if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                RECT rect;
                                GetWindowRect(frameID ? frameID : screenID, &rect);
                                if (parent)
                                {
                                        POINT p = { rect.left, rect.top };
                                        ScreenToClient(parent->screenID, &p);
                                        rect.right -= rect.left - p.x + clip.left;
                                        rect.bottom -= rect.top - p.y + clip.top;
                                        rect.left = p.x - clip.left;
                                        rect.top = p.y - clip.top;
                                }
                                trueRegion.Assign(rect);
                                relative = trueRegion;
                        }
                        else
                        {
                                trueRegion.right = trueRegion.left + LOWORD(lParam) - 1;
                                trueRegion.bottom = trueRegion.top + HIWORD(lParam) - 1;
                        }
                        if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
                                ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                        event.message.message, wParam, lParam);
                                woStatus |= WOS_REDISPLAY;
                        }
                        else
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        if (!FlagSet(woStatus, WOS_MINIMIZED))
                        {
                                Event(UI_EVENT(S_CHANGED, 0));
                                if (FlagSet(woStatus, WOS_REDISPLAY))
                                {
                                        SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
                                        InvalidateRect(frameID ? frameID : screenID, ZIL_NULLP(RECT), TRUE);
                                        woStatus &= ~WOS_REDISPLAY;
                                }
                        }
                        break;

                case WM_LBUTTONDOWN:
                case WM_NCLBUTTONDOWN:
                        Information(I_HIDE_SUBWINDOW, ZIL_NULLP(void)); // See if a combo-box is active.
                        // Continue to WM_LBUTTONDBLCLK.

                case WM_LBUTTONDBLCLK:
                case WM_RBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                        {
#if defined(ZIL_EDIT)
                        if (FlagSet(woStatus, WOS_EDIT_MODE))
                                processed = FALSE;
#endif
                        if (!Inherited(ID_GROUP))
                                processed = FALSE;
                        if (message == WM_NCLBUTTONDOWN)
                                break;

                        UI_POSITION position =
                                { event.position.column - trueRegion.left, event.position.line - trueRegion.top };
                        for (object = First(); object; object = object->Next())
                        {
                                if (object->trueRegion.Overlap(position) &&
                                        (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) || FlagSet(woStatus, WOS_EDIT_MODE)))
                                {
                                        UI_EVENT tEvent = event;
                                        tEvent.message.hwnd = object->screenID;
                                        tEvent.position = position;
                                        tEvent.message.lParam -= object->trueRegion.left | ((LPARAM)object->trueRegion.top << 16);
                                        ccode = object->Event(tEvent);
                                        processed = TRUE;
                                        break;
                                }
                        }
                        }
                        break;

                case WM_CLOSE:
                        ccode = UI_WINDOW_OBJECT::Event(event);
                        break;

                // Amibigous cases.
                case WM_LBUTTONUP:
                case WM_MOUSEMOVE:
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_CHAR:
                        processed = FALSE;
                        break;

                default:
                        if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                        event.message.message, wParam, lParam);
                        else
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        break;
                }
        }

        if (!processed)
        {
                static int clientRegistered = FALSE;

                // Switch on the event type.
                ccode = LogicalEvent(event, ID_WINDOW);
                switch (ccode)
                {
                case S_INITIALIZE:
                        UI_WINDOW_OBJECT::Event(event);

                        // Check the environment specific flag settings.
                        if (screenID && !parent)
                                dwStyle |= title ? WS_OVERLAPPED : WS_POPUP;

                        for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
                                object->Event(event);
                        for (object = First(); object; object = object->Next())
                        {
                                if (FlagSet(object->woStatus, WOS_CURRENT) ||
                                        (!current && !FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
                                        !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
                                {
                                        if (current)
                                                Current()->woStatus &= ~WOS_CURRENT;
                                        if (current && FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
                                                Current()->woStatus &= ~WOS_SELECTED;
                                        current = object;
                                        object->woStatus |= (woStatus & WOS_CURRENT);
                                        if (FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
                                                object->woStatus |= WOS_SELECTED;
                                }
                                else
                                        object->woStatus &= ~WOS_CURRENT;
                                object->Event(event);
                        }
                        if (!parent)
                                dwStyle |= title ? WS_OVERLAPPED : WS_POPUP;

                        break;

                case S_DEINITIALIZE:
                        UI_WINDOW_OBJECT::Event(event);
                        if (FlagSet(woStatus, WOS_MINIMIZED | WOS_MAXIMIZED))
                                relative = trueRegion = restore;
                        menu = 0, icon = 0, title = ZIL_NULLP(UI_WINDOW_OBJECT);
                        for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
                                object->Event(event);
                        for (object = First(); object; object = object->Next())
                                object->Event(event);
                        break;

                case S_CREATE:
                        {
                        ZIL_ICHAR clientString[] = { 'C','L','I','E','N','T', 0 };
                        ZIL_ICHAR clientTitle[] = { 0 };
                        clipList.Destroy();
                        // Specify an absolute minimum size for non-parent and MDI windows.
                        if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                int minWidth = 10 * display->cellWidth;
                                if (relative.Width() < minWidth)
                                        relative.right = relative.left + minWidth - 1;
                                int minHeight = display->cellHeight;
                                minHeight += 2 * UIW_BORDER::width;
                                if (relative.Height() < minHeight)
                                        relative.bottom = relative.top + minHeight - 1;
                        }

                        // Scroll bars need to be added to the client.
                        DWORD fStyle = dwStyle & (WS_VSCROLL | WS_HSCROLL);
                        dwStyle &= ~(WS_VSCROLL | WS_HSCROLL);

                        UI_WINDOW_OBJECT::Event(event);
                        if (FlagSet(woStatus, WOS_MAXIMIZED))
                        {
                                dwStyle |= WS_MAXIMIZE;
                                restore = relative;
                        }
                        else if (FlagSet(woStatus, WOS_MINIMIZED))
                        {
                                dwStyle |= WS_MINIMIZE;
                                restore = relative;
                        }
                        if (parent && parent->Inherited(ID_LIST))
                                SendMessage(parent->screenID, LB_INSERTSTRING, ListIndex(), (LPARAM)this);
                        else if (parent && parent->Inherited(ID_COMBO_BOX))
                                SendMessage(parent->screenID, CB_INSERTSTRING, ListIndex(), (LPARAM)this);
                        else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                        {
                                RegisterObject("UIW_MDIFRAME", ZIL_NULLP(char), &_mdiFrameCallback,
                                        title ? (ZIL_ICHAR *)title->Information(I_GET_TEXT, ZIL_NULLP(void)) : ZIL_NULLP(ZIL_ICHAR), menu);
                                CLIENTCREATESTRUCT clientInfo;
                                clientInfo.hWindowMenu = 0;
                                clientInfo.idFirstChild = 0x1000;

                                wnFlags |= WNF_NO_SCROLL;
                                DWORD mdiStyle = WS_CHILD | WS_VISIBLE | (dwStyle & (SBS_VERT | SBS_HORZ | SBS_SIZEBOX)) | fStyle;

                                frameID = screenID;
#if defined(ZIL_UNICODE)
                                wchar_t *clientStr = ISOtoUNICODE("MDICLIENT");
                                screenID = CreateWindow(clientStr, ZIL_NULLP(ZIL_ICHAR), mdiStyle, 0, 0, 0, 0,
                                        frameID, 0,  display->hInstance, (LPSTR)&clientInfo);
                                delete clientStr;
#else
                                screenID = CreateWindow("MDICLIENT", ZIL_NULLP(char), mdiStyle, 0, 0, 0, 0,
                                        frameID, 0,  display->hInstance, (LPSTR)&clientInfo);
#endif
                        }
                        else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                woStatus |= WOS_WINDOWS_ACTION;
                                RegisterObject("UIW_MDICHILD", ZIL_NULLP(char), &_mdiChildCallback,
                                        title ? (ZIL_ICHAR *)title->Information(I_GET_TEXT, ZIL_NULLP(void)) : ZIL_NULLP(ZIL_ICHAR), menu);

                                woStatus &= ~WOS_WINDOWS_ACTION;

                                // Register the client Area.
                                if (!clientRegistered)
                                {
                                        WNDCLASS wc;
                                        wc.style = 0;
                                        wc.lpfnWndProc = ClientInitProcedure;
                                        wc.cbClsExtra = 0;
                                        wc.cbWndExtra = sizeof(UI_WINDOW_OBJECT *);
                                        wc.hInstance = display->hInstance;
                                        wc.hIcon = ZIL_NULLH(ZIL_ICON_HANDLE);
                                        wc.hCursor = LoadCursor(0, IDC_ARROW);
                                        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
                                        wc.lpszMenuName = 0;
                                        wc.lpszClassName = clientString;
                                        RegisterClass(&wc);
                                        clientRegistered = TRUE;
                                }

                                _client = this;
                                frameID = screenID;
                                screenID = CreateWindowEx(WS_EX_NOPARENTNOTIFY, clientString, clientTitle,
                                        fStyle | WS_VISIBLE | WS_CHILD,
                                        trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(),
                                        frameID, ZIL_NULLH(HMENU), display->hInstance, ZIL_NULLP(ZIL_ICHAR));
                        }
                        else
                        {
                                RegisterObject("UIW_WINDOW", ZIL_NULLP(char), &_windowCallback,
                                        title ? (ZIL_ICHAR *)title->Information(I_GET_TEXT, ZIL_NULLP(void)) : ZIL_NULLP(ZIL_ICHAR), menu);

                                // Register the client Area.
                                if (!clientRegistered)
                                {
                                        WNDCLASS wc;
                                        wc.style = CS_HREDRAW | CS_VREDRAW;
                                        wc.lpfnWndProc = ClientInitProcedure;
                                        wc.cbClsExtra = 0;
                                        wc.cbWndExtra = sizeof(UI_WINDOW_OBJECT *);
                                        wc.hInstance = display->hInstance;
                                        wc.hIcon = ZIL_NULLH(ZIL_ICON_HANDLE);
                                        wc.hCursor = LoadCursor(0, IDC_ARROW);
                                        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
                                        wc.lpszMenuName = 0;
                                        wc.lpszClassName = clientString;
                                        RegisterClass(&wc);
                                        clientRegistered = TRUE;
                                }

                                _client = this;
                                frameID = screenID;
                                screenID = CreateWindowEx(WS_EX_NOPARENTNOTIFY, clientString, clientTitle,
                                        fStyle | WS_VISIBLE | WS_CHILD,
                                        trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(),
                                        frameID, ZIL_NULLH(HMENU), display->hInstance, ZIL_NULLP(ZIL_ICHAR));
                        }
                        for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
                        {
                                object->Event(event);
                                if (FlagSet(object->woStatus, WOS_SYSTEM_OBJECT) &&
                                        FlagSet(object->woFlags, WOF_NON_FIELD_REGION) &&
                                        !object->Inherited(ID_MENU))
                                                clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
                        }
                        if (frameID && screenID != frameID && clipList.First())
                        {
                                UI_REGION region = clipList.First()->region;
                                MoveWindow(screenID, region.left, region.top,
                                        region.Width(), region.Height(), FALSE);
                                clipList.Destroy();
                                region.right = region.Width() - 1;
                                region.left = 0;
                                region.bottom = region.Height() - 1;
                                region.top = 0;
                                clipList.Add(new UI_REGION_ELEMENT(screenID, region));
                        }
                        else if (frameID && screenID != frameID)
                        {
                                RECT cRect;
                                GetClientRect(frameID, &cRect);
                                UI_REGION region;
                                region.Assign(cRect);
                                MoveWindow(screenID, region.left, region.top,
                                        region.Width(), region.Height(), FALSE);
                                clipList.Add(new UI_REGION_ELEMENT(screenID, region));
                        }
                        for (object = First(); object; object = object->Next())
                        {
                                object->Event(event);
                                if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
                                        clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
                        }
                        if (!clipList.First())
                                clipList.Add(new UI_REGION_ELEMENT(screenID, trueRegion));
                        }
                        if (!FlagSet(wnFlags, WNF_NO_SCROLL))
                                ScrollEvent((UI_EVENT &)UI_EVENT(S_SCROLLRANGE));
                        break;

                case S_REGISTER_OBJECT:
                        if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                RegisterObject("UIW_MDIFRAME", ZIL_NULLP(char), &_mdiFrameCallback,
                                        title ? (ZIL_ICHAR *)title->Information(I_GET_TEXT, ZIL_NULLP(void)) : ZIL_NULLP(ZIL_ICHAR), menu);
                        else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                RegisterObject("UIW_MDICHILD", ZIL_NULLP(char), &_mdiChildCallback,
                                        title ? (ZIL_ICHAR *)title->Information(I_GET_TEXT, ZIL_NULLP(void)) : ZIL_NULLP(ZIL_ICHAR), menu);
//                              defaultCallback = _mciChildCallback;
                        }
                        else
                                RegisterObject("UIW_WINDOW", ZIL_NULLP(char), &_windowCallback,
                                        title ? (ZIL_ICHAR *)title->Information(I_GET_TEXT, ZIL_NULLP(void)) : ZIL_NULLP(ZIL_ICHAR), menu);
                        break;

                case S_CHANGED:
                        {
                        if (!screenID || FlagSet(woStatus, WOS_INTERNAL_ACTION))
                                        break;
                        else if (parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                UI_REGION region = trueRegion;
                                ccode = UI_WINDOW_OBJECT::Event(event);
                                if (trueRegion != region)
                                {
                                        ZIL_SCREENID tFrameID = frameID ? frameID : screenID;
                                        SendMessage(tFrameID, WM_SETREDRAW, FALSE, 0);
                                        MoveWindow(tFrameID, trueRegion.left, trueRegion.top,
                                                trueRegion.Width(), trueRegion.Height(), FALSE);
                                        SendMessage(tFrameID, WM_SETREDRAW, TRUE, 0);
                                }
                        }
                        else
                                ccode = S_UNKNOWN;
                        clipList.Destroy();
                        RECT cRect;
                        GetClientRect(frameID ? frameID : screenID, &cRect);
                        UI_REGION region;
                        region.Assign(cRect);
                        clipList.Add(new UI_REGION_ELEMENT(screenID, region));

                        if (!Inherited(ID_LIST))
                        {
                                for(object = (UI_WINDOW_OBJECT *)support.First(); object;
                                        object = object->Next())
                                {
                                        UI_REGION region = object->trueRegion;
                                        object->Event(UI_EVENT(S_CHANGED));
                                        WORD sizeFlags = FlagSet(woStatus, WOS_MINIMIZED) ? SWP_NOSIZE : 0;
                                        if (FlagSet(object->woStatus, WOS_SYSTEM_OBJECT) &&
                                                FlagSet(object->woFlags, WOF_NON_FIELD_REGION) &&
                                                !object->Inherited(ID_MENU))
                                        {
                                                clipList.Split(screenID, object->trueRegion, parent &&
                                                        !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);
                                                if (object->trueRegion != region)
                                                {
                                                        ZIL_SCREENID tFrameID;
                                                        object->Information(I_GET_FRAMEID, &tFrameID);
                                                        SendMessage(tFrameID, WM_SETREDRAW, FALSE, 0);
                                                        SetWindowPos(tFrameID, 0,
                                                                object->trueRegion.left, object->trueRegion.top,
                                                                object->trueRegion.Width(), object->trueRegion.Height(),
                                                                SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW | sizeFlags);
                                                        SendMessage(tFrameID, WM_SETREDRAW, TRUE, 0);

                                                        // Invalidate the old and new object regions.
                                                        woStatus |= WOS_REDISPLAY;
                                                        RECT winRegion;
                                                        winRegion.left = region.left;
                                                        winRegion.top = region.top;
                                                        winRegion.right = region.right + 1;
                                                        winRegion.bottom = region.bottom + 1;
                                                        InvalidateRect(frameID, &winRegion, TRUE);
                                                        InvalidateRect(tFrameID, ZIL_NULLP(RECT), TRUE);

                                                }
                                        }
                                        else if (object->trueRegion != region &&
                                                FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
                                        {
                                                        ZIL_SCREENID tFrameID;
                                                        object->Information(I_GET_FRAMEID, &tFrameID);
                                                        SendMessage(tFrameID, WM_SETREDRAW, FALSE, 0);
                                                        SetWindowPos(tFrameID, 0,
                                                                object->trueRegion.left, object->trueRegion.top,
                                                                object->trueRegion.Width(), object->trueRegion.Height(),
                                                                SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW | sizeFlags);
                                                        SendMessage(tFrameID, WM_SETREDRAW, TRUE, 0);
                                        }
                                }

                                if (!clipList.First())
                                {
                                        UI_REGION client = { -1, -1, -1, -1 };
                                        clipList.Add(new UI_REGION_ELEMENT(screenID, client));
                                }

                                if (frameID && screenID != frameID)
                                {
                                        UI_REGION region = clipList.First()->region;
                                        RECT cRect;
                                        GetWindowRect(screenID, &cRect);
                                        POINT p = { cRect.left, cRect.top };
                                        ScreenToClient(frameID, &p);
                                        cRect.right -= cRect.left - p.x;
                                        cRect.left = p.x;
                                        cRect.bottom -= cRect.top - p.y;
                                        cRect.top = p.y;
                                        UI_REGION cRegion;
                                        cRegion.Assign(cRect);

                                        if (cRegion != region)
                                        {
                                                RECT rect;
                                                GetClientRect(screenID, &rect);
                                                MoveWindow(screenID, region.left, region.top,
                                                        region.Width(), region.Height(), FALSE);
                                                woStatus |= WOS_REDISPLAY;
        
                                                if ((rect.bottom + (hScroll ?
                                                        GetSystemMetrics(SM_CYHSCROLL) : 0)) < region.Height())
                                                {
                                                        RECT tRect = { 0, rect.bottom,
                                                                MinValue(rect.right + 1, region.Width()), region.Height() };
                                                        InvalidateRect(screenID, &tRect, TRUE);
                                                }
                                                else if (hScroll)
                                                {
                                                        RECT tRect = { 0, region.Height() - GetSystemMetrics(SM_CYHSCROLL) - 1,
                                                                MinValue(rect.right + 1, region.Width()), region.Height() };
                                                        InvalidateRect(screenID, &tRect, TRUE);
                                                }
                                                if ((rect.right + (vScroll ?
                                                        GetSystemMetrics(SM_CXVSCROLL) : 0)) < region.Width())
                                                {
                                                        RECT tRect = { rect.right, 0,
                                                                region.Width(), region.Height() };
                                                        InvalidateRect(screenID, &tRect, TRUE);
                                                }
                                                else if (vScroll)
                                                {
                                                        RECT tRect = { region.Width() - GetSystemMetrics(SM_CXVSCROLL) - 1, 0,
                                                                region.Width(), region.Height() };
                                                        InvalidateRect(screenID, &tRect, TRUE);
                                                }
                                        }
        
                                        UI_REGION tRegion = clipList.First()->region;
                                        clipList.Destroy();
                                        tRegion.right = tRegion.Width() - 1;
                                        tRegion.left = 0;
                                        tRegion.bottom = tRegion.Height() - 1;
                                        tRegion.top = 0;
                                        clipList.Add(new UI_REGION_ELEMENT(screenID, tRegion));
                                }

                                for (object = First(); object; object = object->Next())
                                {
                                        UI_REGION region = object->trueRegion;
                                        object->Event(UI_EVENT(S_CHANGED));
                                        if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
                                                clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
                                        if (object->trueRegion != region && FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
                                        {
                                                WORD sizeFlags = FlagSet(woStatus, WOS_MINIMIZED) ? SWP_NOSIZE : 0;
                                                ZIL_SCREENID tFrameID;
                                                object->Information(I_GET_FRAMEID, &tFrameID);
                                                SendMessage(tFrameID, WM_SETREDRAW, FALSE, 0);
                                                SetWindowPos(tFrameID, 0,
                                                        object->trueRegion.left, object->trueRegion.top,
                                                        object->trueRegion.Width(), object->trueRegion.Height(),
                                                        SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW | sizeFlags);
                                                SendMessage(tFrameID, WM_SETREDRAW, TRUE, 0);

                                                // Invalidate the old and new object regions.
                                                woStatus |= WOS_REDISPLAY;
                                                RECT winRegion;
                                                winRegion.left = region.left;
                                                winRegion.top = region.top;
                                                winRegion.right = region.right + 1;
                                                winRegion.bottom = region.bottom + 1;
                                                InvalidateRect(screenID, &winRegion, TRUE);
                                                InvalidateRect(tFrameID, ZIL_NULLP(RECT), TRUE);

                                                // Send the redisplay message to mdi-children.
                                                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && parent)
                                                {
                                                        SendMessage(object->screenID, WM_SETREDRAW, TRUE, 0);
                                                        SendMessage(tFrameID, WM_NCPAINT, TRUE, 0);
                                                        InvalidateRect(tFrameID, ZIL_NULLP(RECT), TRUE);
                                                }
                                        }
                                }

                                if (!FlagSet(wnFlags, WNF_NO_SCROLL))
                                        ScrollEvent((UI_EVENT &)UI_EVENT(S_SCROLLRANGE));
                        }
                        else
                                for(object = First(); object; object = object->Next())
                                                object->Event(UI_EVENT(S_CHANGED));
                        }
                        break;

                case S_MOVE:
                        {
                        UI_POSITION position = { 0, 0 };
                        for(object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
                                object->Event(UI_EVENT(S_MOVE, 0, position));
                        for(object = First(); object; object = object->Next())
                                object->Event(UI_EVENT(S_MOVE, 0, position));
                        }
                        break;

                case S_DROP_DEFAULT:
                        ccode = S_DROP_MOVE_OBJECT;
                        // Fall through to S_DROP_MOVE_OBJECT.
                case S_DROP_MOVE_OBJECT:
                case S_DROP_COPY_OBJECT:
                case S_DISPLAY_ACTIVE:
                case S_DISPLAY_INACTIVE:
                case S_SIZE:
                        ccode = UI_WINDOW_OBJECT::Event(event);
                        break;

                case S_VSCROLL:
                case S_HSCROLL:
                        {
                        UI_EVENT sEvent = event;
                        ccode = ScrollEvent(sEvent);
                        }
                        break;

                case S_CURRENT:
                case S_NON_CURRENT:
                        woStatus |= WOS_WINDOWS_ACTION;
                        UI_WINDOW_OBJECT::Event(event);
                        if (defaultObject)
                                defaultObject->Event(UI_EVENT(ccode == S_CURRENT ? S_SHOW_DEFAULT : S_HIDE_DEFAULT));
                        if (current)
                                ccode = Current()->Event(UI_EVENT(ccode));
                        woStatus &= ~WOS_WINDOWS_ACTION;
                        break;

                case S_MDICHILD_EVENT + S_CLOSE:
                        if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                        {
                                UI_WINDOW_OBJECT *dObject = NULL;
                                for (dObject = Last(); dObject &&
                                        !FlagSet(dObject->woAdvancedFlags, WOAF_MDI_OBJECT);
                                        dObject = dObject->Previous())
                                                ;
                                if (dObject)
                                {
                                        woStatus |= WOS_WINDOWS_ACTION;
                                        Subtract(dObject);
                                        woStatus &= ~WOS_WINDOWS_ACTION;
                                        if (!FlagSet(dObject->woAdvancedFlags, WOAF_NO_DESTROY))
                                                delete dObject;
                                }
                        }
                        break;

                case S_MDICHILD_EVENT + S_RESTORE:
                case S_MDICHILD_EVENT + S_MAXIMIZE:
                case S_MDICHILD_EVENT + S_MINIMIZE:
                        if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&
                                frameID && screenID != frameID && current)
                        {
                                UI_WINDOW_OBJECT *dObject = NULL;
                                for (dObject = Last(); dObject &&
                                        !FlagSet(dObject->woAdvancedFlags, WOAF_MDI_OBJECT);
                                        dObject = dObject->Previous())
                                                ;
                                if (dObject)
                                {
                                        int nCmdShow = SW_RESTORE;
                                        if (ccode == S_MDICHILD_EVENT + S_MAXIMIZE)
                                                nCmdShow = SW_SHOWMAXIMIZED;
                                        else if (ccode == S_MDICHILD_EVENT + S_MINIMIZE)
                                                nCmdShow = SW_SHOWMINIMIZED;
                                        ZIL_SCREENID dFrameID;
                                        dObject->Information(I_GET_FRAMEID, &dFrameID);
                                        ShowWindow(dFrameID, nCmdShow);
                                }
                        }
                        break;

                case S_REDISPLAY:
                        {
                        woStatus |= WOS_INTERNAL_ACTION;
                        for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
                        {
                                object->woStatus |= WOS_WINDOWS_ACTION;
                                object->Event(event);
                                object->woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        for (object = First(); object; object = object->Next())
                        {
                                object->woStatus |= WOS_WINDOWS_ACTION;
                                object->Event(event);
                                object->woStatus &= ~WOS_WINDOWS_ACTION;
                        }
                        woStatus &= ~WOS_INTERNAL_ACTION;
                        ccode = UI_WINDOW_OBJECT::Event(event);
                        if (!current)
                                current = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ?
                                        first : last;
                        Add(Current());
                        }
                        break;

                case S_RESTORE:
                case S_MAXIMIZE:
                case S_MINIMIZE:
                        {
                        int nCmdShow = SW_RESTORE;
                        if (ccode == S_MAXIMIZE)
                                nCmdShow = SW_SHOWMAXIMIZED;
                        else if (ccode == S_MINIMIZE)
                                nCmdShow = SW_SHOWMINIMIZED;
                        ShowWindow(frameID ? frameID : screenID, nCmdShow);
                        }
                        break;

                case S_ADD_OBJECT:
                        Add(event.windowObject);
                        break;

                case S_SUBTRACT_OBJECT:
                        Subtract(event.windowObject);
                        break;

                case S_DRAG_DEFAULT:
                        ccode = S_DRAG_MOVE_OBJECT;
                        // Fall through to S_DRAG_MOVE_OBJECT.
                case S_DRAG_MOVE_OBJECT:
                case S_DRAG_COPY_OBJECT:
                        if (!Inherited(ID_LIST))
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        else if (!FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP))
                        {
                                if (Index(windowManager->dragObject) == -1 &&
                                        windowManager->dragObject != this)
                                                eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
                                else
                                        eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
                                                DM_DRAG_MOVE : DM_DRAG_COPY);
                        }
                        else if (windowManager->dragObject->Inherited(ID_LIST))
                        {
                                int count = 0;
                                UI_WINDOW_OBJECT *object;
                                for (windowManager->dragObject->Information(I_GET_FIRST, &object);
                                        object; object = object->Next())
                                                if (FlagSet(object->woStatus, WOS_SELECTED))
                                                        count++;
                                if (count > 1)
                                        eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
                                                DM_DRAG_MOVE_MULTIPLE : DM_DRAG_COPY_MULTIPLE);
                                else
                                        eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
                                                DM_DRAG_MOVE : DM_DRAG_COPY);
                        }
                        else if (ccode == S_DRAG_COPY_OBJECT || (windowManager->dragObject->parent &&
                                windowManager->dragObject->parent->Inherited(ID_LIST)))
                                        eventManager->DeviceImage(E_MOUSE, ccode == S_DRAG_MOVE_OBJECT ?
                                                DM_DRAG_MOVE : DM_DRAG_COPY);
                        else
                                eventManager->DeviceImage(E_MOUSE, DM_CANCEL);
                        break;

                case L_END_SELECT:
                case L_END_COPY_DRAG:
                case L_END_MOVE_DRAG:
                case L_CONTINUE_SELECT:
                case L_CONTINUE_COPY_DRAG:
                case L_CONTINUE_MOVE_DRAG:
                        {
                        if (FlagSet(woStatus, WOS_MINIMIZED))
                        {
                                ccode = UI_WINDOW_OBJECT::Event(event);
                                break;
                        }
                        UI_POSITION position =
                                { event.position.column - trueRegion.left, event.position.line - trueRegion.top };
                        for (object = First(); object; object = object->Next())
                        {
                                if (object->trueRegion.Overlap(position) && object != windowManager->dragObject)
                                {
                                        UI_EVENT tEvent = event;
                                        tEvent.message.hwnd = object->screenID;
                                        tEvent.position = position;
                                        tEvent.message.lParam -= object->trueRegion.left | ((LPARAM)object->trueRegion.top << 16);
                                        ccode = object->Event(tEvent);
                                        processed = TRUE;
                                        break;
                                }
                        }
                        if (!object)
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        }
                        break;

                case L_HELP:
                        // See if the low level object processes the help.
                        if (current && Current()->Event(event) != S_UNKNOWN)
                                return (ccode);
                        else if (parent && helpContext == NO_HELP_CONTEXT)
                                return (S_UNKNOWN);
                        else if (helpSystem)
                                helpSystem->DisplayHelp(windowManager, helpContext);
                        break;

                case L_FIRST:
                        ccode = current ? Current()->Event(event) : S_UNKNOWN;
                        for (object = First(); object && ccode == S_UNKNOWN; object = object->Next())
                                if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
                                        !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
                                {
                                        wnFlags |= WNF_MOVE_REVERSE;
                                        UIW_WINDOW::Add(object);
                                        wnFlags &= ~WNF_MOVE_REVERSE;
                                        ccode = L_FIRST;
                                }
                        break;

                case L_LAST:
                        ccode = current ? Current()->Event(event) : S_UNKNOWN;
                        for (object = Last(); object && ccode == S_UNKNOWN; object = object->Previous())
                                if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
                                        !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
                                {
                                        wnFlags |= WNF_MOVE_FORWARD;
                                        UIW_WINDOW::Add(object);
                                        wnFlags &= ~WNF_MOVE_FORWARD;
                                        ccode = L_LAST;
                                }
                        break;

                case L_PREVIOUS:
                        {
                        EVENT_TYPE tcode = ccode;
                        ccode = S_UNKNOWN;
                        // Make sure there is a current object.
                        if (!current)
                                UIW_WINDOW::Add(Last());
                        // See if the current object processes the message.
                        else if (Current()->Event(event) != S_UNKNOWN)
                                ccode = tcode;
                        // Don't move if on a support object.
                        else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
                                UI_ERROR_SYSTEM::Beep();
                        // Don't move if on the first field of a non-wrapping window.
                        else if (!Current()->Previous() && FlagSet(wnFlags, WNF_NO_WRAP))
                                break;
                        // Go to the previous field in the window.
                        else
                        {
                                object = Current()->Previous() ? Current()->Previous() : Last();
                                while (object && object != current && ccode == S_UNKNOWN)
                                        if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
                                                !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
                                        {
                                                wnFlags |= WNF_MOVE_REVERSE;
                                                UIW_WINDOW::Add(object);
                                                wnFlags &= ~WNF_MOVE_REVERSE;
                                                ccode = tcode;
                                        }
                                        else if (object->Previous())
                                                object = object->Previous();
                                        else if (FlagSet(wnFlags, WNF_NO_WRAP))
                                                break;
                                        else
                                                object = Last();
                        }
                        }
                        break;

                case L_NEXT:
                        {
                        EVENT_TYPE tcode = ccode;
                        ccode = S_UNKNOWN;
                        // Make sure there is a current object.
                        if (!current)
                                UIW_WINDOW::Add(First());
                        // See if the current object processes the message.
                        else if (Current()->Event(event) != S_UNKNOWN)
                                ccode = tcode;
                        // Don't move if on a support object.
                        else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
                                UI_ERROR_SYSTEM::Beep();
                        // Don't move if on the last field of a non-wrapping window.
                        else if (!Current()->Next() && FlagSet(wnFlags, WNF_NO_WRAP))
                                break;
                        // Go to the next field in the window.
                        else
                        {
                                object = Current()->Next() ? Current()->Next() : First();
                                while (object && object != current && ccode == S_UNKNOWN)
                                        if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
                                                !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
                                        {
                                                wnFlags |= WNF_MOVE_FORWARD;
                                                UIW_WINDOW::Add(object);
                                                wnFlags &= ~WNF_MOVE_FORWARD;
                                                ccode = tcode;
                                        }
                                        else if (object->Next())
                                                object = object->Next();
                                        else if (FlagSet(wnFlags, WNF_NO_WRAP))
                                                break;
                                        else
                                                object = First();
                        }
                        }
                        break;

                case L_UP:
                case L_DOWN:
                case L_LEFT:
                case L_RIGHT:
                        if (current)
                        {
                                if (event.type == E_MSWINDOWS)
                                        ccode = Current()->Event(event);
                                else
                                {
                                        UI_WINDOW_OBJECT *nObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                                        UI_WINDOW_OBJECT *wObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                                        UI_WINDOW_OBJECT *wwObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                                        for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
                                        {
                                                // Is it another object that can get focus.
                                                if (object == Current() ||
                                                        FlagSet(object->woFlags, WOF_NON_SELECTABLE) ||
                                                        FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
                                                                continue;

                                                // Is it in the wrong direction.
                                                if ((ccode == L_LEFT &&
                                                                (object->trueRegion.top > Current()->trueRegion.bottom ||
                                                                (object->trueRegion.bottom > Current()->trueRegion.top &&
                                                                object->trueRegion.left > Current()->trueRegion.left))) ||
                                                        (ccode == L_RIGHT &&
                                                                (object->trueRegion.bottom < Current()->trueRegion.top ||
                                                                (object->trueRegion.top <= Current()->trueRegion.bottom &&
                                                                object->trueRegion.right < Current()->trueRegion.right))) ||
                                                        (ccode == L_UP &&
                                                                (object->trueRegion.left > Current()->trueRegion.right ||
                                                                (object->trueRegion.right >= Current()->trueRegion.left &&
                                                                object->trueRegion.top > Current()->trueRegion.top))) ||
                                                        (ccode == L_DOWN &&
                                                                (object->trueRegion.right < Current()->trueRegion.left ||
                                                                (object->trueRegion.left <= Current()->trueRegion.right &&
                                                                object->trueRegion.bottom < Current()->trueRegion.bottom))))
                                                {
                                                        // Is it farthest so far.
                                                        if (!wwObject ||
                                                                (ccode == L_LEFT &&
                                                                        (object->trueRegion.bottom > wwObject->trueRegion.bottom ||
                                                                        (object->trueRegion.bottom == wwObject->trueRegion.bottom &&
                                                                        object->trueRegion.right > wwObject->trueRegion.right))) ||
                                                                (ccode == L_RIGHT &&
                                                                        (object->trueRegion.top < wwObject->trueRegion.top ||
                                                                        (object->trueRegion.top == wwObject->trueRegion.top &&
                                                                        object->trueRegion.left < wwObject->trueRegion.left))) ||
                                                                (ccode == L_UP &&
                                                                        (object->trueRegion.right > wwObject->trueRegion.right ||
                                                                        (object->trueRegion.right == wwObject->trueRegion.right &&
                                                                        object->trueRegion.bottom > wwObject->trueRegion.bottom))) ||
                                                                (ccode == L_DOWN &&
                                                                        (object->trueRegion.left < wwObject->trueRegion.left ||
                                                                        (object->trueRegion.left == wwObject->trueRegion.left &&
                                                                        object->trueRegion.top < wwObject->trueRegion.top))))
                                                                                wwObject = object;

                                                        continue;
                                                }

                                                // Is it in correct direction.
                                                if ((ccode == L_LEFT && object->trueRegion.bottom > Current()->trueRegion.top) ||
                                                        (ccode == L_RIGHT && object->trueRegion.top < Current()->trueRegion.bottom) ||
                                                        (ccode == L_UP && object->trueRegion.right > Current()->trueRegion.left) ||
                                                        (ccode == L_DOWN && object->trueRegion.left < Current()->trueRegion.right))
                                                {
                                                        // Is it closest so far.
                                                        if (!nObject ||
                                                                (ccode == L_LEFT && object->trueRegion.right > nObject->trueRegion.right) ||
                                                                (ccode == L_RIGHT && object->trueRegion.left < nObject->trueRegion.left) ||
                                                                (ccode == L_UP && object->trueRegion.bottom > nObject->trueRegion.bottom) ||
                                                                (ccode == L_DOWN && object->trueRegion.top < nObject->trueRegion.top))
                                                                        nObject = object;
                                                }
                                                else if (!nObject)
                                                {
                                                        // Is it closest so far.
                                                        if (!wObject ||
                                                                (ccode == L_LEFT &&
                                                                        (object->trueRegion.bottom > wObject->trueRegion.bottom ||
                                                                        (object->trueRegion.bottom == wObject->trueRegion.bottom &&
                                                                        object->trueRegion.right > wObject->trueRegion.right))) ||
                                                                (ccode == L_RIGHT &&
                                                                        (object->trueRegion.top < wObject->trueRegion.top ||
                                                                        (object->trueRegion.top == wObject->trueRegion.top &&
                                                                        object->trueRegion.left < wObject->trueRegion.left))) ||
                                                                (ccode == L_UP &&
                                                                        (object->trueRegion.right > wObject->trueRegion.right ||
                                                                        (object->trueRegion.right == wObject->trueRegion.right &&
                                                                        object->trueRegion.bottom > wObject->trueRegion.bottom))) ||
                                                                (ccode == L_DOWN &&
                                                                        (object->trueRegion.left < wObject->trueRegion.left ||
                                                                        (object->trueRegion.left == wObject->trueRegion.left &&
                                                                        object->trueRegion.top < wObject->trueRegion.top))))
                                                                                wObject = object;
                                                }
                                        }

                                        if (nObject || (!FlagSet(wnFlags, WNF_NO_WRAP) && (wObject || wwObject)))
                                                Add(nObject ? nObject : (wObject ? wObject : wwObject));
                                        else
                                                ccode = UI_WINDOW_OBJECT::Event(event);
                                }
                        }
                        else
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        break;

                case L_SELECT:
                        if (defaultObject)
                                ccode = defaultObject->Event(UI_EVENT(L_SELECT));
                        else
                                ccode = S_UNKNOWN;
                        break;

                // Default operations.
                default:
                        {
                        if (event.type == E_MSWINDOWS && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
                                ccode = DefFrameProc(frameID ? frameID : screenID, frameID ? screenID : 0,
                                        event.message.message, event.message.wParam, event.message.lParam);
                        else if (event.type == E_MSWINDOWS || !current || (ccode = Current()->Event(event)) == S_UNKNOWN)
                                ccode = UI_WINDOW_OBJECT::Event(event);
                        else
                                ccode = S_UNKNOWN;
                        }
                }
        }
        // Return the control code.
        return (ccode);
}

void UIW_WINDOW::RegionMax(UI_WINDOW_OBJECT *object)
{
        // Check to see if it is the top level object.
        if ((object == this && (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY))) ||
                Inherited(ID_MENU))
        {
                trueRegion = clip = relative;
                clipList.Destroy();
                return;
        }
        else if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
        {
                object->trueRegion = object->relative;
                object->clip = clipList.First()->region;
                return;
        }

        // Find the appropriate region element.
        UI_REGION_ELEMENT *element = clipList.First();
        UI_REGION region;
        ZIL_SCREENID parentID = FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) && frameID ?
                        frameID : screenID;
        if (element)                                            // Get first available region.
                region = element->region;
        else if (parentID)                                      // Determine the update region.
        {
                RECT tRegion;
                GetClientRect(parentID, &tRegion);
                region.Assign(tRegion);
                clipList.Add(new UI_REGION_ELEMENT(screenID, region));
        }
        int bottomRegion = FlagsSet(object->woFlags, WOF_NON_FIELD_REGION | WOF_JUSTIFY_RIGHT);
        for ( ; element; element = element->Next())
                if (!bottomRegion && region.top > element->region.top)
                        region = element->region;
                else if (bottomRegion && region.bottom < element->region.bottom)
                        region = element->region;

        // Compute the actual field region.
        object->clip = region;
        if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION) ||
                FlagSet(object->woStatus, WOS_MAXIMIZED))
        {
                object->trueRegion = region;
                object->relative.right = object->relative.right - object->relative.left;
                object->relative.bottom = object->relative.bottom - object->relative.top;
                object->relative.left = object->relative.top = 0;
        }
        else
        {
                object->trueRegion.left = region.left + scroll.left + object->relative.left;
                object->trueRegion.top = region.top + scroll.top + object->relative.top;
                object->trueRegion.right = region.left + scroll.left + object->relative.right;
                object->trueRegion.bottom = region.top + scroll.top + object->relative.bottom;
        }
}


EVENT_TYPE UIW_WINDOW::ScrollEvent(UI_EVENT &event)
{
        EVENT_TYPE ccode = event.type;
        switch(ccode)
        {

        case S_SCROLLRANGE:
                {
                //UI_REGION client = clipList.First() ? clipList.First()->region : trueRegion;
                // sc7 complains about line above and quits with an internal error
                // the code segment below should be equivalent
                UI_REGION client;
                if (clipList.First() != NULL)
                    client = clipList.First()->region;
                else
                    client = trueRegion;
                    
 
                int minx = 32767, miny = 32767, maxx = 0, maxy = 0;
                for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
                        if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
                        {
                                minx = MinValue(minx, object->relative.left);
                                miny = MinValue(miny, object->relative.top);
                                maxx = MaxValue(maxx, object->relative.right);
                                maxy = MaxValue(maxy, object->relative.bottom);
                        }
                maxx += GetSystemMetrics(SM_CXVSCROLL);
                maxy += GetSystemMetrics(SM_CYHSCROLL);

                scroll.right = scroll.left + minx + (maxx - minx);
                scroll.bottom = scroll.top + miny + (maxy - miny);

                // Calculate scroll maximum.
                if (scroll.Height() < client.Height())
                        vScrollInfo.maximum = (vScrollInfo.current != vScrollInfo.minimum ?
                        vScrollInfo.minimum + vScrollInfo.current : vScrollInfo.minimum);
                else
                        vScrollInfo.maximum = MaxValue(vScrollInfo.minimum, vScrollInfo.minimum +
                        scroll.Height() - client.Height() + display->cellWidth);
                if (scroll.Width() < client.Width())
                        hScrollInfo.maximum = (hScrollInfo.current != hScrollInfo.minimum ?
                        hScrollInfo.minimum + hScrollInfo.current : hScrollInfo.minimum);
                else
                        hScrollInfo.maximum = MaxValue(hScrollInfo.minimum, hScrollInfo.minimum +
                        scroll.Width() - client.Width() + display->cellWidth);

                vScrollInfo.delta = hScrollInfo.delta = display->cellHeight;
                vScrollInfo.showing = client.Height();
                hScrollInfo.showing = client.Width();

                event.type = S_VSCROLL_SET;
                event.scroll = vScrollInfo;
                ScrollEvent(event);
                event.type = S_HSCROLL_SET;
                event.scroll = hScrollInfo;
                ScrollEvent(event);
                }
                break;

        case S_VSCROLL_SET:
                // Set the scroll range and thumb position.
                vScrollInfo = event.scroll;
                if (vScroll)
                {
                        SetScrollRange(screenID, SB_VERT, vScrollInfo.minimum, vScrollInfo.maximum, FALSE);
                        SetScrollPos(screenID, SB_VERT, vScrollInfo.current, FALSE);
                }
                break;

        case S_HSCROLL_SET:
                // Set the scroll range and thumb position.
                hScrollInfo = event.scroll;
                if (hScroll)
                {
                        SetScrollRange(screenID, SB_HORZ, hScrollInfo.minimum, hScrollInfo.maximum, FALSE);
                        SetScrollPos(screenID, SB_HORZ, hScrollInfo.current, FALSE);
                }
                break;

        case S_VSCROLL:
        case S_HSCROLL:
                {
                int delta = event.scroll.delta;
                UI_SCROLL_INFORMATION sInfo = ccode == S_VSCROLL ? vScrollInfo : hScrollInfo;
                if ((sInfo.current == sInfo.minimum && delta <= 0) ||
                        (sInfo.current == sInfo.maximum && delta >= 0))
                        return ccode;
                sInfo.current += delta;
                if (sInfo.current < sInfo.minimum)
                {
                        delta -= (sInfo.current - sInfo.minimum);
                        sInfo.current = sInfo.minimum;
                }
                else if (sInfo.current > sInfo.maximum)
                {
                        delta -= (sInfo.current - sInfo.maximum);
                        sInfo.current = sInfo.maximum;
                }

                // Update scroll information and thumb button.
                event.scroll = sInfo;
                event.scroll.delta = delta;
                if (ccode == S_VSCROLL && delta)
                {
                        vScrollInfo = sInfo;
                        if (vScroll && frameID && screenID != frameID)
                                SetScrollPos(screenID, SB_VERT, vScrollInfo.current, TRUE);
                        event.type = S_VSCROLL_WINDOW;
                        ScrollEvent(event);
                }
                else if (ccode == S_HSCROLL && delta)
                {
                        hScrollInfo = sInfo;
                        if (hScroll && frameID && screenID != frameID)
                                SetScrollPos(screenID, SB_HORZ, hScrollInfo.current, TRUE);
                        event.type = S_HSCROLL_WINDOW;
                        ScrollEvent(event);
                }
                }
                break;

        case S_VSCROLL_WINDOW:
        case S_HSCROLL_WINDOW:
                if (!FlagSet(wnFlags, WNF_NO_SCROLL))
                {
                        if (ccode == S_VSCROLL_WINDOW)
                        {
                                scroll.top -= event.scroll.delta;
                                scroll.bottom -= event.scroll.delta;
                        }
                        else
                        {
                                scroll.left -= event.scroll.delta;
                                scroll.right -= event.scroll.delta;
                        }

                        SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
                        if (parent && FlagSet(woFlags, WOF_NON_FIELD_REGION))
                                parent->Event(UI_EVENT(S_CHANGED));
                        else
                                Event(UI_EVENT(S_CHANGED));
                        SendMessage(screenID, WM_SETREDRAW, TRUE, 0);

                        // Scroll the window contents
                        RECT rect;
                        GetClientRect(screenID, &rect);
                        if (ccode == S_VSCROLL_WINDOW)
                                ScrollWindow(screenID, 0, -event.scroll.delta, &rect, 0);
                        else
                                ScrollWindow(screenID, -event.scroll.delta, 0, &rect, 0);
                        UpdateWindow(screenID);
                        }
                        break;

                default:
                        ccode = S_UNKNOWN;
        }
        return ccode;
}



UI_WINDOW_OBJECT *UIW_WINDOW::Subtract(UI_WINDOW_OBJECT *object)
{
        UI_WINDOW_OBJECT *nextObject = object ? object->Next() : ZIL_NULLP(UI_WINDOW_OBJECT);

        // Remove the object from the list.
        if (!object)
                return (nextObject);
        else if (screenID && !FlagSet(woStatus, WOS_WINDOWS_ACTION) &&
                !Inherited(ID_MENU))
        {
                UpdateWindow(screenID);
                SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
        }

        if (UI_LIST::Index(object) != -1 ||
                support.Index(object) != -1)
        {
                UI_EVENT event;
                if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
                        ;
                else if (object == vScroll)
                        vScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
                else if (object == hScroll)
                        hScroll = ZIL_NULLP(UI_WINDOW_OBJECT);
                else if (object == title)
                        title = ZIL_NULLP(UI_WINDOW_OBJECT);
                else if (object->Inherited(ID_MENU))
                {
                        DestroyMenu(object->menuID);
                        object->menuID = 0;
                        menu = ZIL_NULLH(HMENU);
                }
                else if (FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
                {
                        woStatus |= WOS_INTERNAL_ACTION;
                        ZIL_SCREENID tFrameID;
                        object->Information(I_GET_FRAMEID, &tFrameID);
                        object->Event(UI_EVENT(S_DEINITIALIZE));
                        if (parent || !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                                DestroyWindow(tFrameID);
                        else
                                SendMessage(screenID, WM_MDIDESTROY, (int)tFrameID, 0);
                        object->woStatus &= ~WOS_SYSTEM_OBJECT;
                        woStatus &= ~WOS_INTERNAL_ACTION;
                }
                else if (menuID && Inherited(ID_MENU))
                {
                        DeleteMenu(menuID, UI_LIST::Index(object), MF_BYPOSITION);
                        object->menuID = 0;
                }
                else
                {
                        woStatus |= WOS_INTERNAL_ACTION;
                        object->Event(UI_EVENT(S_DEINITIALIZE));
                        event.type = S_SUBTRACT_OBJECT;
                        event.data = object;
                        Event(event);
                        woStatus &= ~WOS_INTERNAL_ACTION;
                }

                if (UI_LIST::Index(object) != -1)       // Make sure the object is in the list.
                {
                        if (current && Current()->Inherited(ID_PULL_DOWN_MENU)
                                && object == Current()->userObject)
                                        Current()->userObject = ZIL_NULLP(UI_WINDOW_OBJECT);
                        UI_LIST::Subtract(object);
                }
                else if (support.Index(object) != -1)
                {
                        if (object == current)  // Check for a current support object.
                                current = First();
                        support.Subtract(object);
                }
        }

        // Return a pointer to the next object.
        return (nextObject);
}


// ----- OS Specific Functions ----------------------------------------------

void UIW_WINDOW::CheckSelection(UI_WINDOW_OBJECT *selectedObject)
{
        // Check for proper single-select status.
        if (!selectedObject)
                selectedObject = Current();
        if (!selectedObject || !FlagSet(selectedObject->woStatus, WOS_SELECTED) ||
                FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
                return;

        // Make sure other items are not selected.
        int position = 0;
        for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next(), position++)
        {
                if (object != selectedObject && FlagSet(object->woStatus, WOS_SELECTED))
                {
                        object->woStatus &= ~WOS_SELECTED;
                        if (screenID && object->Inherited(ID_POP_UP_ITEM))
                                CheckMenuItem(menuID, position, MF_BYPOSITION | MF_UNCHECKED);
                        else if (screenID && Inherited(ID_LIST))
                                SendMessage(screenID, LB_SETSEL, 0, position);
                        else if (screenID == object->screenID)
                                ;
                        else if (object->screenID && object->Inherited(ID_BUTTON))
                                SendMessage(object->screenID, BM_SETCHECK, 0, 0);
                }
        }
}

void UIW_WINDOW::OSDestroy(void)
{
        // Remove the window objects from the system.
        if (screenID)
        {
                SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
                for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
                        if (object->screenID)
                        {
                                ZIL_SCREENID frameID;
                                object->Information(I_GET_FRAMEID, &frameID);
                                DestroyWindow(frameID);
                        }
        }
}

void UIW_WINDOW::OSUpdateSettings(ZIL_OBJECTID objectID)
{
        // See if the window needs to be re-computed.
        if (objectID == ID_WINDOW && FlagSet(woStatus, WOS_REDISPLAY))
        {
                Event(UI_EVENT(S_INITIALIZE));
                Event(UI_EVENT(S_CREATE));
                if (!parent)
                {
                        ZIL_SCREENID insertAfter = HWND_TOP;
                        if (previous)
                                Previous()->Information(I_GET_FRAMEID, &insertAfter);
                        SetWindowPos(frameID, insertAfter, 0, 0, 0, 0,
                                SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE |
                                (insertAfter == HWND_TOP ? 0 : SWP_NOACTIVATE));
                }
                else
                {
                        ShowWindow(frameID, SW_SHOWNA);
                }
                if (!parent && windowManager->Index(this) &&
                        FlagSet(windowManager->First()->woAdvancedFlags, WOAF_MODAL))
                                EnableWindow(frameID, FALSE);
        }
}

void UIW_WINDOW::OSInitialize(void)
{
                hScroll = vScroll = title = ZIL_NULLP(UI_WINDOW_OBJECT);
                title = ZIL_NULLP(UI_WINDOW_OBJECT);
                frameID = 0;
                menu = 0;
                icon = 0;
                mdiCallback = ZIL_NULLF(LRESULT (FAR PASCAL *)(HWND, HWND, UINT, WPARAM, LPARAM));
}

