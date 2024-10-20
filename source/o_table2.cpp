//	Zinc Interface Library - O_TABLE2.CPP
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
	display->VirtualGet(screenID, trueRegion);
	UIW_TABLE::DrawItem(event, ccode);
	if (FlagSet(thFlags, THF_COLUMN_HEADER | THF_CORNER_HEADER))
		display->Line(screenID, trueRegion.left, trueRegion.bottom, trueRegion.right, trueRegion.bottom,
			lastPalette, 1, FALSE);
	if (FlagSet(thFlags, THF_ROW_HEADER | THF_CORNER_HEADER))
		display->Line(screenID, trueRegion.right, trueRegion.top, trueRegion.right, trueRegion.bottom,
			lastPalette, 1, FALSE);
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
				records = columns;
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
			break;

		case S_CHANGED:
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
					tableRecord->relative.right = tableRecord->relative.left + width - 4;
					tableRecord->relative.bottom = tableRecord->relative.top + height - 4;
					tableRecord->trueRegion.right = tableRecord->trueRegion.left + width - 2;
					tableRecord->trueRegion.bottom = tableRecord->trueRegion.top + height - 2;
					if (tableRecord->screenID)
						WinInvalidateRect(tableRecord->screenID, ZIL_NULLP(RECTL), TRUE);
				}
			}
			break;

		case S_REGISTER_OBJECT:
			{
			UI_WINDOW_OBJECT::Event(event);
			ULONG value = SYSCLR_BUTTONMIDDLE;
			WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(value), &value);
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
	}
}