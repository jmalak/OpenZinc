//	Zinc Interface Library - Z_LIST1.CPP
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


#include "ui_gen.hpp"

// ----- UI_LIST_BLOCK ------------------------------------------------------

UI_LIST_BLOCK::UI_LIST_BLOCK(int _noOfElements,
	int (*_compareFunction)(void *element1, void *element2)) :
	UI_LIST(_compareFunction), noOfElements(_noOfElements),
	elementArray(ZIL_NULLP(void)), freeList()
{
	// No other list block information is initialized.
}

UI_LIST_BLOCK::~UI_LIST_BLOCK(void)
{
	// Establish null lists so it doesn't free up an unknown block.
	first = last = ZIL_NULLP(UI_ELEMENT);
	freeList.first = freeList.last = ZIL_NULLP(UI_ELEMENT);
}

UI_ELEMENT *UI_LIST_BLOCK::Add(UI_ELEMENT *element)
{
	UI_ELEMENT *newElement = freeList.first;

	// Add a new list element to the use list at specified location.
	if (newElement)
	{
		freeList.Subtract(newElement);
		UI_LIST::Add(element, newElement);
	}

	// Return a pointer to the new element.
	return (newElement);
}

UI_ELEMENT *UI_LIST_BLOCK::Add(void)
{
	UI_ELEMENT *newElement = freeList.first;

	// Add a new list element to the use list according to compareFunction.
	if (newElement)
	{
		freeList.Subtract(newElement);
		UI_LIST::Add(newElement);
	}

	// Return a pointer to the new element.
	return (newElement);
}

UI_ELEMENT *UI_LIST_BLOCK::Subtract(UI_ELEMENT *element)
{
	UI_ELEMENT *nextElement = element->Next();

	// Delete the element from the use list.
	UI_LIST::Subtract(element);
	freeList.Add(ZIL_NULLP(UI_ELEMENT), element);

	// Return the next element.
	return (nextElement);
}

