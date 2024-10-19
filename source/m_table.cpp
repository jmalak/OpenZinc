//	Zinc Interface Library - M_TABLE.CPP
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


#include <Xm/BulletinB.h>
#include <Xm/ScrolledW.h>
#if defined(__DVX__)
	// Needed for XtConfigureWidget().
	struct _XtStateRec;
#	include <X11/IntrinsP.h>
#else
#	if defined(__QNX__)
	struct _XtStateRec;
#	endif
#	include <X11/IntrinsicP.h>
#endif

EVENT_TYPE UIW_TABLE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_REGION updateRegion;
	UI_REGION client;
	Information(I_GET_CLIPREGION, &client);
	if (!event.region.Overlap(client, updateRegion))
		return ccode;

	display->VirtualGet(screenID, updateRegion);

	int recordWidth = 0;
	int recordHeight = 0;

	if (virtualRecord)
	{
		recordWidth = virtualRecord->relative.Width();
		recordHeight = virtualRecord->relative.Height();
		for (int i = topRecord; i < records; i++)
		{
			if (i - topRecord > client.Height() / recordHeight)
				break;
			else if (i != currentRecord)
				DrawRecord(i);
		}
	}

#if 0
	if (FlagSet(tblFlags, TBLF_GRID))
	{
		int column = client.left + scroll.left + (updateRegion.left -
			(client.left + scroll.left) + recordWidth - 1) / recordWidth *
			recordWidth - 1;
		while (column <= updateRegion.right)
		{
			display->Line(screenID, column, updateRegion.top, column,
				updateRegion.bottom, &display->colorMap[DARKGRAY], 1,
				FALSE, &clip);
			column += recordWidth;
		}
		int row = client.top + scroll.top + (updateRegion.top -
			(client.top + scroll.top) + recordHeight - 1) / recordHeight *
			recordHeight- 1;
		while (row <= updateRegion.bottom)
		{
			display->Line(screenID, updateRegion.left, row, updateRegion.right,
				row, &display->colorMap[DARKGRAY], 1, FALSE, &clip);
			row += recordHeight;
		}
	}
#endif

	display->VirtualPut(screenID);
	return (ccode);
}

void UIW_TABLE::DrawRecord(int recordNum)
{
	// Set the new position.
	int column = recordNum % columns;
	int row = recordNum / columns;
	UI_REGION newRegion;
	newRegion.left = column * virtualRecord->trueRegion.Width() + scroll.left + 2;
	newRegion.top = (row + scroll.top) * virtualRecord->trueRegion.Height() + 2;
	newRegion.right = newRegion.left + virtualRecord->relative.Width() - 1;
	newRegion.bottom = newRegion.top + virtualRecord->relative.Height() - 1;
	virtualRecord->relative = newRegion;
	virtualRecord->Event(UI_EVENT(S_CREATE));
	virtualRecord->Event(UI_EVENT(S_REDISPLAY));

	// Set the new data.
	UI_EVENT dataEvent(S_SET_DATA);
	dataEvent.rawCode = recordNum;
	dataEvent.data = GetRecord(recordNum);
	virtualRecord->Event(dataEvent);

	UI_EVENT updateEvent(S_DISPLAY_ACTIVE);
	updateEvent.region = virtualRecord->trueRegion;
	virtualRecord->woStatus |= WOS_REDISPLAY;
	virtualRecord->woStatus &= ~WOS_CURRENT;
	virtualRecord->UI_WINDOW_OBJECT::Event(updateEvent);
}

EVENT_TYPE UIW_TABLE::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TABLE);
	switch (ccode)
	{
		case S_INITIALIZE:
			currentRecord = topRecord = 0;
			UIW_WINDOW::Event(event);
			if (virtualRecord)
			{
				virtualRecord->parent = this;
				virtualRecord->Event(event);
			}
			break;

		case S_REGISTER_OBJECT:
		case S_CREATE:
			currentRecord = topRecord = 0;
			vScrollInfo.delta = 1;
			hScrollInfo.delta = virtualRecord ? MaxValue(virtualRecord->trueRegion.Width(),display->cellWidth) : display->cellWidth;
			if (tableRecord)
			{
				int height = tableRecord->relative.Height();
				tableRecord->relative.top = 0;
				tableRecord->relative.bottom = height - 1;

				UI_EVENT dataEvent(S_SET_DATA);
				dataEvent.rawCode = currentRecord;
				dataEvent.data = GetRecord(currentRecord);
				tableRecord->Event(dataEvent);
			}
			// Continue.

		case S_CHANGED:
			{
			UI_REGION tRegion = trueRegion;
			UI_WINDOW_OBJECT *object;
			nargs = 0;
			XtSetArg(args[nargs], XmNmarginWidth, 0); nargs++;
			XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
			XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;
			if (Inherited(ID_TABLE_HEADER))
			{
				if (ccode != S_CHANGED)
					RegisterObject(xmBulletinBoardWidgetClass,
						ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION),
						ccode, TRUE, TRUE, parent->screenID);
				if (ccode == S_CHANGED)
					XtConfigureWidget(screenID,
						MaxValue(trueRegion.left, 0), MaxValue(trueRegion.top, 0),
						MaxValue(trueRegion.Width(), 1),
						MaxValue(trueRegion.Height(), 1), 0);
			}
			else
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				tRegion = trueRegion;
				if (FlagSet(woFlags, WOF_BORDER))
				{
					XtSetArg(args[nargs], XmNshadowThickness, 2); nargs++;
					XtSetArg(args[nargs], XmNshadowType, XmSHADOW_IN);
						nargs++;
				}

				// create the BB for the window
				if (ccode != S_CHANGED)
					RegisterObject(xmBulletinBoardWidgetClass,
						ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode,
						TRUE, TRUE);
				if (ccode == S_CHANGED)
					XtConfigureWidget(XtParent(XtParent(screenID)),
						MaxValue(trueRegion.left, 0), MaxValue(trueRegion.top, 0),
						MaxValue(trueRegion.Width(), 1),
						MaxValue(trueRegion.Height(), 1), 0);

				clipList.Destroy();
				{
				UI_REGION region;
				region.left = 2; region.top = 2;
				region.right = trueRegion.Width() - 4;
				region.bottom = trueRegion.Height() - 4;
				clipList.Add(new UI_REGION_ELEMENT(screenID, region));
				}
				for (object = (UI_WINDOW_OBJECT *)support.First(); object;
						object = object->Next())
				{
					if (object->Inherited(ID_SCROLL_BAR))
						continue;
					object->Event(event);
					if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
						clipList.Split(screenID, object->trueRegion, TRUE);
				}
		
				if (vScroll || hScroll)
				{
					if (ccode == S_CHANGED)
						XtVaSetValues(XtParent(XtParent(screenID)),
							XmNwidth, trueRegion.Width(),
							XmNheight, trueRegion.Height(), NULL);
					UI_REGION region = trueRegion;
					Information(I_GET_CLIPREGION, &trueRegion);
					if (ccode != S_CHANGED)
					{
						nargs = 0;
						// XmAPPLICATION_DEFINED scrolling.
						XtSetArg(args[nargs], XmNscrollingPolicy,
							XmAPPLICATION_DEFINED); nargs++;
						XtSetArg(args[nargs], XmNscrollBarDisplayPolicy,
						XmSTATIC); nargs++;
						RegisterObject(ZIL_NULLP(_WidgetClassRec),
							XmCreateScrolledWindow, ccode, TRUE, TRUE, screenID);

						// Remove unneeded scroll bars.
						Widget vertScroll = ZIL_NULLP(_WidgetRec),
							horizScroll = ZIL_NULLP(_WidgetRec);
						nargs = 0;
						XtSetArg(args[nargs], XmNhorizontalScrollBar,
							&horizScroll); nargs++;
						XtSetArg(args[nargs], XmNverticalScrollBar,
							&vertScroll); nargs++;
						XtGetValues(screenID, args, nargs);
						if (hScroll && horizScroll)
						{
							XtVaSetValues(horizScroll, XmNtraversalOn, FALSE, NULL);
							hScroll->screenID = horizScroll;
						}
						else if (horizScroll)
							XtUnmanageChild(horizScroll);
						if (vScroll && vertScroll)
						{
							XtVaSetValues(vertScroll, XmNtraversalOn, FALSE, NULL);
							vScroll->screenID = vertScroll;
						}
						else if (vertScroll)
							XtUnmanageChild(vertScroll);
					}
					if (ccode == S_CHANGED)
						XtConfigureWidget(XtParent(screenID),
							MaxValue(trueRegion.left, 0), MaxValue(trueRegion.top, 0),
							MaxValue(trueRegion.Width(), 1), MaxValue(trueRegion.Height(), 1), 0);

					trueRegion.right -= trueRegion.left;
					trueRegion.bottom -= trueRegion.top;
					trueRegion.left = trueRegion.top = 0;
					if (vScroll)
						trueRegion.right -= 24;
					if (hScroll)
						trueRegion.bottom -= 24;

					if (ccode != S_CHANGED)
					{
						nargs = 0;
						XtSetArg(args[nargs], XmNmarginWidth, 0); nargs++;
						XtSetArg(args[nargs], XmNmarginHeight, 0); nargs++;
						XtSetArg(args[nargs], XmNresizePolicy, XmRESIZE_NONE); nargs++;
						RegisterObject(xmBulletinBoardWidgetClass,
							ZIL_NULLF(ZIL_MOTIF_CONVENIENCE_FUNCTION), ccode,
							TRUE, TRUE, screenID);
					}

					if (ccode == S_CHANGED)
						XtConfigureWidget(screenID, MaxValue(trueRegion.left, 0), MaxValue(trueRegion.top, 0),
							MaxValue(trueRegion.Width(), 1), MaxValue(trueRegion.Height(), 1), 0);
				}

				for (object = (UI_WINDOW_OBJECT *)support.First(); object;
						object = object->Next())
				{
					if (!object->Inherited(ID_SCROLL_BAR))
						continue;
					object->Event(event);
					if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
						clipList.Split(screenID, object->trueRegion, TRUE);
				}
			}

//			clipList.Destroy();
			for (object = First(); object; object = object->Next())
			{
				object->Event(event);
				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion, TRUE);
			}

			if (vScroll || hScroll)
			{
				// hook Bulletin Board to scrolled win
				nargs = 0;
				XtSetArg(args[nargs], XmNworkWindow, screenID); nargs++;
				XtSetValues(XtParent(screenID), args, nargs);

				if (ccode == S_CHANGED && records - topRecord <=
					trueRegion.Height() / tableRecord->trueRegion.Height())
				{
					// Last record is at bottom, and window was enlarged.
					UI_EVENT tEvent(S_VSCROLL);
					tEvent.scroll.delta = MinValue(vScrollInfo.showing -
						trueRegion.Height() / tableRecord->trueRegion.Height(), 0);
					if (topRecord + tEvent.scroll.delta < 0)
						tEvent.scroll.delta = -topRecord;
					ScrollEvent(tEvent);
					scroll.top -= tEvent.scroll.delta;
					if (rowHeader)
						((UIW_TABLE *)rowHeader)->topRecord = topRecord;
					if (rowHeader)
						((UIW_TABLE *)rowHeader)->scroll.top = scroll.top;
				}
			}

			UI_EVENT range(S_SCROLLRANGE);
			ScrollEvent(range);

			if (ccode != S_CHANGED && virtualRecord)
				virtualRecord->Event(event);
			trueRegion = tRegion;
			}
			break;

		case L_LEFT:
		case L_RIGHT:
		case L_UP:
		case L_DOWN:
			if (tableRecord)
			{
				int column = currentRecord % columns;
				int row = currentRecord / columns;
				if (ccode == L_LEFT && column > 0)
					column -= 1;
				else if (ccode == L_RIGHT && column < columns - 1)
					column += 1;
				else if (ccode == L_UP && row > 0)
					row -= 1;
				else if (ccode == L_DOWN && (row + 1) * columns + column <= records - 1)
					row += 1;
				else
					break;

				SetCurrent(row * columns + column);
				Event(UI_EVENT(S_REDISPLAY));
			}
			break;

	case L_PGUP:
	case L_PGDN:
		if (tableRecord)
		{
			int recordHeight = virtualRecord->trueRegion.Height();
			int deltaRows = (ccode == L_PGUP ?
				-vScrollInfo.showing : vScrollInfo.showing);
			int newRecord = currentRecord + (deltaRows * columns);
			if (newRecord < 0)
				newRecord = currentRecord % columns;
			if (newRecord > records - 1)
				newRecord = records - columns + currentRecord % columns;
			if (currentRecord != newRecord)
				SetCurrent(newRecord);
		}
		break;

		case L_BEGIN_SELECT:
			if (event.type != E_MOTIF)
			{
				SetCurrent((int)event.rawCode);

				// Remove this line to diminish flash but allow dropout.
				Event(UI_EVENT(S_REDISPLAY));
				break;
			}

			if (!FlagSet(woFlags, WOF_NON_SELECTABLE) &&
				!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			{
				UI_REGION client = trueRegion;
				UI_POSITION position = event.position;
				int recordWidth = tableRecord->trueRegion.Width();
				int recordHeight = tableRecord->trueRegion.Height();
				int newCurrent = topRecord + position.line / recordHeight *
					columns + position.column / recordWidth;

				if (newCurrent < records && newCurrent != currentRecord)
				{
					// Route an event thru the queue.
					UI_EVENT rEvent(S_ROUTE_EVENT);
					rEvent.event = new UI_EVENT(ccode);
					rEvent.event->rawCode = newCurrent;
					rEvent.routeTarget = this;
					eventManager->Put(rEvent);
				}
			}
			UIW_WINDOW::Event(event);
			break;

		case S_VSCROLL:
		case S_HSCROLL:
		case S_VSCROLL_CHECK:
		case S_HSCROLL_CHECK:
		case S_SCROLLRANGE:
		case S_VSCROLL_WINDOW:
		case S_HSCROLL_WINDOW:
			{
			UI_EVENT sEvent = event;
			ccode = ScrollEvent(sEvent);
			}
			break;

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
	}
	return(ccode);
}

void UIW_TABLE::SetCurrent(int recordNum)
{
	UI_EVENT recordEvent;

	tableRecord->Event(UI_EVENT(S_NON_CURRENT));
	int column = recordNum % columns;
	int row = recordNum / columns;

	UI_REGION client = clipList.First()->region;
	if (row < topRecord)
	{
		UI_EVENT scrollEvent(S_VSCROLL);
		scrollEvent.scroll.delta = row - topRecord;
		ScrollEvent(scrollEvent);
	}
	else if (row > topRecord + vScrollInfo.showing - 1)
	{
		UI_EVENT scrollEvent(S_VSCROLL);
		scrollEvent.scroll.delta = row - (topRecord + vScrollInfo.showing - 1);
		ScrollEvent(scrollEvent);
	}
	else if (tableRecord->trueRegion.left + 2 < client.left)
	{
		UI_EVENT scrollEvent(S_HSCROLL);
		scrollEvent.scroll.delta = (tableRecord->trueRegion.left - client.left -
			hScrollInfo.delta + 1) / hScrollInfo.delta * hScrollInfo.delta;
		ScrollEvent(scrollEvent);
	}
	else if (tableRecord->trueRegion.right > client.right &&
		tableRecord->trueRegion.left > client.left)
	{
		UI_EVENT scrollEvent(S_HSCROLL);
		scrollEvent.scroll.delta = (tableRecord->trueRegion.right - client.right +
			hScrollInfo.delta - 1) / hScrollInfo.delta * hScrollInfo.delta;
		ScrollEvent(scrollEvent);
	}

	UI_REGION newRegion;
	newRegion.left = (column + scroll.left) * tableRecord->trueRegion.Width();
	newRegion.top = (row + scroll.top) * tableRecord->trueRegion.Height();
	newRegion.right = newRegion.left + tableRecord->relative.Width() - 1;
	newRegion.bottom = newRegion.top + tableRecord->relative.Height() - 1;
	tableRecord->relative = newRegion;

	currentRecord = recordNum;
	UI_EVENT dataEvent(S_SET_DATA);
	dataEvent.rawCode = currentRecord;
	dataEvent.data = GetRecord(currentRecord);

	tableRecord->Event(dataEvent);
	tableRecord->Event(UI_EVENT(S_CREATE));

	recordEvent.type = S_CURRENT;
	recordEvent.region = tableRecord->trueRegion;
	tableRecord->Event(recordEvent);

}

EVENT_TYPE UIW_TABLE::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_SCROLLRANGE:
		if (tableRecord)
		{
			UI_REGION client;
			Information(I_GET_CLIPREGION, &client);
			int recordWidth = tableRecord->trueRegion.Width();
			int recordHeight = tableRecord->trueRegion.Height();

			hScrollInfo.delta = (recordWidth < client.Width()) ?
				recordWidth : display->cellWidth;
			hScrollInfo.minimum = 0;
			hScrollInfo.showing =
				client.Width() / hScrollInfo.delta * hScrollInfo.delta;
			hScrollInfo.maximum = MaxValue(columns * recordWidth /
				hScrollInfo.delta * hScrollInfo.delta -	hScrollInfo.showing, 0);
//			hScrollInfo.current = -scroll.left;

			vScrollInfo.delta = 1;
			vScrollInfo.minimum = 0;
			vScrollInfo.showing = client.Height() / recordHeight;
			vScrollInfo.maximum = MaxValue(records - vScrollInfo.showing, 0);

			UI_EVENT scrollEvent;
			if (vScroll)
			{
				scrollEvent.type = S_VSCROLL_SET;
				scrollEvent.scroll = vScrollInfo;
				vScroll->Event(scrollEvent);
			}
			if (hScroll)
			{
				scrollEvent.type = S_HSCROLL_SET;
				scrollEvent.scroll = hScrollInfo;
				hScroll->Event(scrollEvent);
			}
		}
		scroll.top = -topRecord;
		scroll.left = 0;
		scroll.bottom = vScrollInfo.maximum - topRecord;
		scroll.right = hScrollInfo.maximum;
		if (rowHeader)
			rowHeader->Event(event);
		if (columnHeader)
			columnHeader->Event(event);
		break;

	case S_VSCROLL:
		if (rowHeader)
			rowHeader->Event(event);

		topRecord += event.scroll.delta / vScrollInfo.delta * columns;

		{
		UI_REGION region;
		Information(I_GET_CLIPREGION, &region);
		int visible = region.Height() / tableRecord->trueRegion.Height();

		if ((currentRecord < topRecord || currentRecord >= topRecord + visible)
				&& First()->screenID)
			First()->Event(UI_EVENT(S_DEINITIALIZE));
		else if ((currentRecord >= topRecord &&
				currentRecord < topRecord + visible) && !First()->screenID)
			First()->Event(UI_EVENT(S_CREATE));
		else if (First()->screenID)
			First()->Event(UI_EVENT(S_CHANGED));
		}

		UIW_WINDOW::ScrollEvent(event);
		break;

	case S_HSCROLL:
		if (columnHeader)
			columnHeader->Event(event);

		UIW_WINDOW::ScrollEvent(event);
		break;

	case S_VSCROLL_CHECK:
		{
		UI_REGION client;
		UIW_WINDOW::Information(I_GET_CLIPREGION, &client);

		int height = tableRecord->trueRegion.Height();
		int clientHeight = client.Height();
		int row = currentRecord / columns;
		int top = row * height;
		int bottom = top + height - 1;

		int scrollDelta = 0;
		if (scroll.top + top < 0)
			scrollDelta = scroll.top + top;
		else if (scroll.top + bottom >= clientHeight)
			scrollDelta = scroll.top + bottom - clientHeight + height - 1;

		if (scrollDelta)
		{
			UI_EVENT scrollEvent(S_VSCROLL);
			scrollEvent.scroll.delta = scrollDelta;
			ScrollEvent(scrollEvent);
		}
		}
		break;

	case S_HSCROLL_CHECK:
		{
		UI_REGION client;
		UIW_WINDOW::Information(I_GET_CLIPREGION, &client);

		int width = tableRecord->trueRegion.Width();
		int clientWidth = client.Width();
		int column = currentRecord % columns;
		int left = column * width;
		int right = left + width - 1;

		int scrollDelta = 0;
		if (scroll.left + left < 0)
			scrollDelta = scroll.left + left;
		else if (scroll.left + right >= clientWidth)
			scrollDelta = scroll.left + right - clientWidth + width - 1;

		if (scrollDelta)
		{
			UI_EVENT scrollEvent(S_HSCROLL);
			scrollEvent.scroll.delta = scrollDelta;
			ScrollEvent(scrollEvent);
		}
		}
		break;

	default:
		ccode = UIW_WINDOW::ScrollEvent(event);
		break;
	}
	return (ccode);
}

void UIW_TABLE::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);
	if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) )
	{
		object->trueRegion = object->relative;
	}
	if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
	{
		object->trueRegion.bottom += FlagSet(object->woFlags, WOF_BORDER) ? 4 : 2;
		object->trueRegion.right += FlagSet(object->woFlags, WOF_BORDER) ? 4 : 2;
	}
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TABLE::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TABLE && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
	}
}
