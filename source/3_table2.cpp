//      Zinc Interface Library - W_TABLE2.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"

EVENT_TYPE UIW_TABLE_HEADER::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
        UI_REGION region = trueRegion;
        UIW_TABLE::DrawItem(event, ccode);
        display->VirtualGet(screenID, region);
        if (FlagSet(thFlags, THF_COLUMN_HEADER | THF_CORNER_HEADER))
                display->Line(screenID, region.left, region.bottom, region.right, region.bottom,
                        lastPalette, 1, FALSE, &clip);
        if (FlagSet(thFlags, THF_ROW_HEADER | THF_CORNER_HEADER))
                display->Line(screenID, region.right, region.top, region.right, region.bottom,
                        lastPalette, 1, FALSE, &clip);
        woStatus &= ~WOS_REDISPLAY;

        display->VirtualPut(screenID);
        return (ccode);
}

EVENT_TYPE UIW_TABLE_HEADER::Event(const UI_EVENT &event)
{
        // Switch on the event type.
        EVENT_TYPE ccode = LogicalEvent(event, ID_TABLE);
        switch (ccode)
        {
                case S_INITIALIZE:
                        if (FlagSet(thFlags, THF_COLUMN_HEADER))
                        {
                                parent->Information(I_SET_COL_HEADER, this);
                                parent->Information(I_GET_COLUMNS, &columns);
                                parent->Information(I_GET_RECORDS, &records);
                                records = MinValue(records, columns);
                                columns = MaxValue(records, 1);
                        }
                        else if (FlagSet(thFlags, THF_ROW_HEADER))
                        {
                                parent->Information(I_SET_ROW_HEADER, this);
                                int _columns, _records;
                                parent->Information(I_GET_COLUMNS, &_columns);
                                parent->Information(I_GET_RECORDS, &_records);
                                columns = 1;
                                records = (_records + _columns - 1) / _columns;
                        }
                        else if (FlagSet(thFlags, THF_CORNER_HEADER))
                                columns = records = 1;
                        ccode = UIW_TABLE::Event(event);
                        dwStyle &= ~WS_BORDER;
                        break;

                case S_CREATE:
                case S_CHANGED:
                        {
                        UI_WINDOW_OBJECT::Event(event);
                        if (FlagSet(thFlags, THF_COLUMN_HEADER))
                        {
                                int height;
                                Information(I_GET_HEIGHT, &height);
                                trueRegion.bottom = trueRegion.top + height - 1;
                        }
                        else if (FlagSet(thFlags, THF_ROW_HEADER))
                        {
                                int width;
                                Information(I_GET_WIDTH, &width);
                                trueRegion.right = trueRegion.left + width - 1;
                        }
                        else
                        {
                                int width, height;
                                parent->Information(I_GET_CORNER_WIDTH, &width);
                                parent->Information(I_GET_CORNER_HEIGHT, &height);
                                trueRegion.right = trueRegion.left + width - 1;
                                trueRegion.bottom = trueRegion.top + height - 1;
                                if (tableRecord)
                                {
                                        tableRecord->relative.right = trueRegion.Width() - 4;
                                        tableRecord->relative.bottom = trueRegion.Height() - 2;
                                }
                        }
                        clipList.Destroy();
                        UI_REGION region = { 0, 0, trueRegion.Width() - 1, trueRegion.Height() - 1 };
                        clipList.Add(new UI_REGION_ELEMENT(screenID, region));

                        if (ccode == S_CREATE)
                        {
                                if (tableRecord)
                                {
                                        UI_EVENT dataEvent(S_SET_DATA);
                                        dataEvent.rawCode = currentRecord;
                                        dataEvent.data = GetRecord(currentRecord);
                                        tableRecord->Event(dataEvent);
                                }
                                UIW_TABLE::Event(UI_EVENT(S_REGISTER_OBJECT));
                                if (tableRecord)
                                {
                                        UI_EVENT dataEvent(S_SET_DATA);
                                        dataEvent.rawCode = currentRecord;
                                        dataEvent.data = GetRecord(currentRecord);
                                        tableRecord->Event(dataEvent);
                                        tableRecord->Event(event);
                                }
                                if (virtualRecord)
                                {
                                        virtualRecord->Event(event);
                                        ZIL_SCREENID virtualID;
                                        virtualRecord->Information(I_GET_FRAMEID, &virtualID);
                                        SendMessage(virtualID, WM_SETREDRAW, (WPARAM)FALSE, 0);
                                }
                                ScrollEvent((UI_EVENT &)UI_EVENT(S_SCROLLRANGE));
                        }
                        }
                        break;

                default:
                        ccode = UIW_TABLE::Event(event);
                        break;
        }
        return(ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TABLE_HEADER::OSUpdateSettings(ZIL_OBJECTID objectID)
{
        // See if the field needs to be re-computed.
        if (objectID == ID_TABLE_HEADER && FlagSet(woStatus, WOS_REDISPLAY))
        {
                Event(UI_EVENT(S_INITIALIZE));
                Event(UI_EVENT(S_CREATE));
        }
}

