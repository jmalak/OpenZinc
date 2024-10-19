//	Zinc Interface Library - D_TABLE2.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

/*       This file is a part of OpenZinc

          OpenZinc is free software; you can redistribute it and/or modify it under
          the terms of the GNU Lessor General Public License as published by
          the Free Software Foundation, either version 3 of the License, or (at
          your option) any later version

	OpenZinc is distributed in the hope that it will be useful, but WITHOUT
          ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
          or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
          General Public License for more details.

          You should have received a copy of the GNU Lessor General Public License
	 along with OpenZinc. If not, see <http://www.gnu.org/licenses/>                          */


#include <ui_win.hpp>

EVENT_TYPE UIW_TABLE_HEADER::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_REGION region = trueRegion;
	UIW_TABLE::DrawItem(event, ccode);
	if (!display->isText)
	{
		display->VirtualGet(screenID, region);
		if (FlagSet(thFlags, THF_COLUMN_HEADER | THF_CORNER_HEADER))
			display->Line(screenID, region.left, region.bottom, region.right, region.bottom,
				lastPalette, 1, FALSE, &clip);
		if (FlagSet(thFlags, THF_ROW_HEADER | THF_CORNER_HEADER))
			display->Line(screenID, region.right, region.top, region.right, region.bottom,
				lastPalette, 1, FALSE, &clip);
		display->VirtualPut(screenID);
	}
	woStatus &= ~WOS_REDISPLAY;
	return (ccode);
}

EVENT_TYPE UIW_TABLE_HEADER::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TABLE);
	switch (ccode)
	{
		case S_INITIALIZE:
			woFlags &= ~WOF_BORDER;
			ccode = UIW_TABLE::Event(event);

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
			{
				columns = 1;
				records = 1;
			}
			break;

		case S_CREATE:
		case S_CHANGED:
			UIW_TABLE::Event(event);
			{
			int width, height;
			int offset = display->isText ? 0 : 2;

			if (FlagSet(thFlags, THF_COLUMN_HEADER))
			{
				Information(I_GET_HEIGHT, &height);
				trueRegion.bottom = trueRegion.top + offset + height - 1;
			}
			else if (FlagSet(thFlags, THF_ROW_HEADER))
  			{
				Information(I_GET_WIDTH, &width);
				trueRegion.right = trueRegion.left + offset + width - 1;
			}
			else
			{
				parent->Information(I_GET_CORNER_WIDTH, &width);
				parent->Information(I_GET_CORNER_HEIGHT, &height);
				trueRegion.right = trueRegion.left + offset + width - 1;
				trueRegion.bottom = trueRegion.top + offset + height - 1;
				if (tableRecord)
				{
					tableRecord->relative.right = trueRegion.Width() - (display->isText ? 1 : 4);
					tableRecord->relative.bottom = trueRegion.Height() - (display->isText ? 1 : 2);
					tableRecord->Event(event);
				}
			}
			clipList.Destroy();
			UI_REGION region = trueRegion;
			clipList.Add(new UI_REGION_ELEMENT(screenID, region));
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
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}
