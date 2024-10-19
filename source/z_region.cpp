//	Zinc Interface Library - Z_REGION.CPP
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


#include "ui_dsp.hpp"

// ----- UI_REGION_ELEMENT & UI_REGION_LIST ---------------------------------

UI_REGION_ELEMENT::UI_REGION_ELEMENT(ZIL_SCREENID _screenID, const UI_REGION &_region) :
	screenID(_screenID)
{
	region.left = _region.left,
		region.top = _region.top,
		region.right = _region.right,
		region.bottom = _region.bottom;
}
UI_REGION_ELEMENT::UI_REGION_ELEMENT(ZIL_SCREENID _screenID, int _left,
	int _top, int _right, int _bottom) : screenID(_screenID)
{
	region.left = _left,
		region.top = _top,
		region.right = _right,
		region.bottom = _bottom;
}

UI_REGION_ELEMENT::~UI_REGION_ELEMENT(void)
{
}

UI_REGION_LIST::UI_REGION_LIST(void)
{
}

UI_REGION_LIST::~UI_REGION_LIST(void)
{
}

void UI_REGION_LIST::Split(ZIL_SCREENID screenID, const UI_REGION &region,
	int allocateBelow)
{
	UI_REGION tRegion, sRegion;
	UI_REGION_ELEMENT *dRegion, *t_dRegion;

	// Split any overlapping regions.
	for (dRegion = First(); dRegion; dRegion = t_dRegion)
	{
		// Preset the previous region element.
		t_dRegion = dRegion->Next();

		// Object encompasses the whole region.
		if (dRegion->region.Encompassed(region))
		{
			UI_LIST::Subtract(dRegion);
			delete dRegion;
		}

		// Object overlaps the region.
		else if (dRegion->region.Overlap(region, tRegion))
		{
			screenID = dRegion->screenID;
			tRegion = dRegion->region;
			sRegion = region;

			// Region is split at a maximum shown by the following set
			// of regions:
			//		 _________
			//		|_________|		1
			//		|__|___|__|		2,3,4
			//		|_________|		5
			//

			// Check for a top region (region 1 above).
			if (region.top > tRegion.top)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, tRegion.left,
					tRegion.top, tRegion.right, region.top - 1));
			else
				sRegion.top = tRegion.top;

			// Check for a bottom region (region 5 above).
			if (region.bottom < tRegion.bottom)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, tRegion.left,
					region.bottom + 1, tRegion.right, tRegion.bottom));
			else
				sRegion.bottom = tRegion.bottom;

			// Check for a left region (region 2 above).
			if (region.left > tRegion.left)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, tRegion.left,
					sRegion.top, region.left - 1, sRegion.bottom));

			// Check for a right region (region 4 above).
			if (region.right < tRegion.right)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region.right + 1,
					sRegion.top, tRegion.right, sRegion.bottom));

			// Region 3 is the object's region.
			UI_LIST::Subtract(dRegion);
			delete dRegion;
		}
	}

	// Check for new allocation space.
	if (!First() && allocateBelow)
		UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region.left,
			region.bottom + 10, region.right, region.bottom + 20));
}
