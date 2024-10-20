//	Zinc Interface Library - Z_LIST.CPP
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

// THIS COPYRIGHT NOTICE CANNOT BE REMOVED!
static char *_copyright = "\
COPYRIGHT (C) 1990-1995. \
All Rights Reserved. \
Zinc Software Incorporated.  Pleasant Grove, Utah  USA.";

// ----- UI_ELEMENT & UI_LIST -----------------------------------------------

UI_ELEMENT::UI_ELEMENT(void) : previous(ZIL_NULLP(UI_ELEMENT)), next(ZIL_NULLP(UI_ELEMENT))
{
/* unused parameters */ (void)_copyright;
}

UI_ELEMENT::~UI_ELEMENT(void)
{
}

int UI_ELEMENT::ListIndex(void)
{
	// Get the element index.
	int index = 0;
	for (UI_ELEMENT *element = previous; element; element = element->previous)
		index++;

	// Return the element index.
	return (index);
}

void *UI_ELEMENT::Information(ZIL_INFO_REQUEST, void *, ZIL_OBJECTID)
{
	// No information is processed for this class.
	return (ZIL_NULLP(void));
}

UI_LIST::UI_LIST(ZIL_COMPARE_FUNCTION _compareFunction) :
	first(ZIL_NULLP(UI_ELEMENT)), last(ZIL_NULLP(UI_ELEMENT)),
	current(ZIL_NULLP(UI_ELEMENT)), compareFunction(_compareFunction)
{
}

UI_LIST::~UI_LIST(void)
{
	UI_LIST::Destroy();
}

UI_ELEMENT *UI_LIST::Add(UI_ELEMENT *newElement)
{
	// See if it is a default insertion.
	if (!first || !compareFunction)
		return (UI_LIST::Add(ZIL_NULLP(UI_ELEMENT), newElement));

	// Get the beginning positions.
	int weight;
	UI_ELEMENT *element1, *element2;
	UI_ELEMENT *beginElement = first;
	UI_ELEMENT *endElement = last;

	// Shortcuts for elements in special orders.
	if ((*compareFunction)(endElement, newElement) <= 0)
		beginElement = endElement;
	else if ((*compareFunction)(beginElement, newElement) >= 0)
		beginElement = endElement = ZIL_NULLP(UI_ELEMENT);

	// Find the proper element position.
	while (beginElement != endElement)
	{
		// Get the middle element - The order here is important!
		element1 = beginElement;
		for (element2 = endElement; element1 != element2;
			element2 = element2->previous)
			if (element1->next != element2)
				element1 = element1->next;

		// Increment the proper element position.
		weight = (*compareFunction)(element1, newElement);
		if (weight > 0 && endElement != element1)
			endElement = element1;
		else if (weight < 0 && beginElement != element1)
			beginElement = element1;
		else
			beginElement = endElement = element1;
	}

	// Place the element in the sorted list.
	if (endElement)					// Insert after endElement.
	{
		newElement->previous = endElement;
		newElement->next = endElement->next;
		if (newElement->next)
			newElement->next->previous = newElement;
		endElement->next = newElement;
		if (last == endElement)
			last = newElement;
	}
	else							// Insert at the beginning of the list.
	{
		newElement->previous = ZIL_NULLP(UI_ELEMENT);
		newElement->next = first;
		first->previous = newElement;
		first = newElement;
	}

	// Return a pointer to the element.
	return (newElement);
}

UI_ELEMENT *UI_LIST::Add(UI_ELEMENT *element, UI_ELEMENT *newElement)
{
	// Add the element to the list.
	if (!first)						// Put at the first of the list.
	{
		newElement->previous = newElement->next = ZIL_NULLP(UI_ELEMENT);
		first = last = newElement;
	}
	else if (!element)				// Put at the end of the list.
	{
		newElement->previous = last;
		newElement->next = ZIL_NULLP(UI_ELEMENT);
		last->next = newElement;
		last = newElement;
	}
	else							// Put before the specified element.
	{
		newElement->previous = element->previous;
		newElement->next = element;
		if (!element->previous)
			first = newElement;
		else
			element->previous->next = newElement;
		element->previous = newElement;
	}

	// Return a pointer to the element.
	return (newElement);
}

int UI_LIST::Count(void)
{
	int count = 0;

	// Get the element index.
	for (UI_ELEMENT *element = first; element; element = element->next)
		count++;

	// Return the count.
	return (count);
}

UI_ELEMENT *UI_LIST::Subtract(UI_ELEMENT *element)
{
	// Make sure an element is specified.
	if (!element)
		return(ZIL_NULLP(UI_ELEMENT));

	// Make sure the element is in the list.
	UI_ELEMENT *tElement = element;
	while (tElement->previous)
		tElement = tElement->previous;
	if (tElement != first)
		return (element);

	// Delete the specified element from the list.
	if (element->previous)
		element->previous->next = element->next;
	else
		first = element->next;
	if (element->next)
		element->next->previous = element->previous;
	else
		last = element->previous;
	if (current == element)
		current = ZIL_NULLP(UI_ELEMENT);

	// Return the next element.
	tElement = element->next;
	element->next = element->previous = ZIL_NULLP(UI_ELEMENT);
	return (tElement);
}

void UI_LIST::Destroy(void)
{
	UI_ELEMENT *tElement;

	// Delete all the elements in the list.
	for (UI_ELEMENT *element = first; element; )
	{
		tElement = element;
		element = element->next;
		delete tElement;
	}
	// Reset the list element pointers.
	first = last = current = ZIL_NULLP(UI_ELEMENT);
}

UI_ELEMENT *UI_LIST::Get(int index)
{
	// Get the list element.
	UI_ELEMENT *element = first;
	for (; index > 0 && element; element = element->next)
		index--;

	// Return the matching element.  If no match, element will be NULL.
	return (element);
}

UI_ELEMENT *UI_LIST::Get(int (*findFunction)(void *element1, void *matchData), void *matchData)
{
	// See if the current element matches.
	if (current && !(*findFunction)(current, matchData))
		return (current);

	// Try to find the list element.
	UI_ELEMENT *element = first;
	for (; element && (*findFunction)(element, matchData); element = element->next)
		;

	// Return the matching element.  If no match, element will be NULL.
	return (element);
}

int UI_LIST::Index(const UI_ELEMENT *element)
{
	// Make sure there is an element.
	if (!element)
		return (-1);

	// Get the element index.
	int index = 0;
	UI_ELEMENT *tElement = first;
	for (; tElement && tElement != element; tElement = tElement->next)
		index++;

	// Return the element index.  If no match, return value is -1.
	if (tElement)
		return (index);
	return (-1);
}

void UI_LIST::Sort(void)
{
	// Make sure there are elements in the list and a comparison routine.
	if (!first || !compareFunction)
		return;

	// Start the list with the first element.
	UI_ELEMENT *element;
	UI_ELEMENT *addElement = first;
	first = last = ZIL_NULLP(UI_ELEMENT);

	// Sort the list of elements using a flat binary insertion.
	// (Binary insertion is contained in the UI_LIST::Add() routine.)
	while (addElement)
	{
		element = addElement->next;
		UI_LIST::Add(addElement);
		addElement = element;
	}
}

ZIL_BIT_VECTOR::ZIL_BIT_VECTOR(ZIL_UINT16 _size) :
	size((_size + 7) / 8)
{
	vector = new ZIL_UINT8[size];
	for (int i=0; i < size; i++)
		vector[i] = 0;
}

ZIL_BIT_VECTOR::~ZIL_BIT_VECTOR()
{
	delete vector;
}

void ZIL_BIT_VECTOR::Clear(ZIL_UINT16 pos)
{
	ZIL_UINT16 offset = pos % 8;
	pos /= 8;
	if (pos >= size)
	{
		ZIL_UINT8 *tmpVector = new ZIL_UINT8[pos+1];
		int i = 0;
		for (; i < size; i++)
			tmpVector[i] = vector[i];
		for (i=size; i <= pos; i++)
			tmpVector[i] = 0;
		delete vector;
		vector = tmpVector;
		size = pos+1;
	}
	vector[pos] &= ~(1 << offset);
}

void ZIL_BIT_VECTOR::Set(ZIL_UINT16 pos)
{
	ZIL_UINT16 offset = pos % 8;
	pos /= 8;
	if (pos >= size)
	{
		ZIL_UINT8 *tmpVector = new ZIL_UINT8[pos+1];
		int i = 0;
		for (; i < size; i++)
			tmpVector[i] = vector[i];
		for (i=size; i <= pos; i++)
			tmpVector[i] = 0;
		delete vector;
		vector = tmpVector;
		size = pos+1;
	}
	vector[pos] |= (1 << offset);
}

int ZIL_BIT_VECTOR::Test(ZIL_UINT16 pos, int _default)
{
	ZIL_UINT16 offset = pos % 8;
	pos /= 8;
	if (pos >= size)
		return (_default);
	return (vector[pos] & (1 << offset));
}

#if defined(ZIL_LOAD)
ZIL_BIT_VECTOR::ZIL_BIT_VECTOR(ZIL_STORAGE_OBJECT_READ_ONLY *object)
{
	ZIL_UINT16 _size;

	object->ZIL_STORAGE_OBJECT_READ_ONLY::Load(&_size);
	size = _size;
	vector = new ZIL_UINT8[_size];
	for (int i=0; i < _size; i++)
		object->ZIL_STORAGE_OBJECT_READ_ONLY::Load(&vector[i]);
}
#endif

#if defined(ZIL_STORE)
void ZIL_BIT_VECTOR::Store(ZIL_STORAGE_OBJECT *object)
{
	ZIL_UINT16 _size = size;

	object->ZIL_STORAGE_OBJECT::Store(_size);
	for (int i=0; i < _size; i++)
		object->ZIL_STORAGE_OBJECT::Store(vector[i]);
}
#endif
