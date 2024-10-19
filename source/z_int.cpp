//	Zinc Interface Library - Z_INT.CPP
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


#include "ui_win.hpp"

const static ZIL_ICHAR numSeparator[3] = { '.','.',0 };
const static ZIL_ICHAR rangeSeparator[3] = { ',',' ',0 };

#define ZIL_UIW_INT_DATA
#include "lang_def.cpp"

#define MAX_LENGTH	64

// ----- UIW_INTEGER --------------------------------------------------------

UIW_INTEGER::UIW_INTEGER(int left, int top, int width, int *_number,
	const ZIL_ICHAR *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	number(ZIL_NULLP(int)), nmFlags(_nmFlags)
{
	// Initialize the integer information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
#if defined(ZIL_MACINTOSH)
		number = new int[1];
#else
		number = new int;
#endif
	else
	{
		text = new ZIL_ICHAR[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = strdup(_range);
	UIW_INTEGER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_INTEGER::DataSet(_number);
}

UIW_INTEGER::~UIW_INTEGER(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
	// Restore the integer information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete number;
	else
		delete text;
	if (range)
		delete range;
}

int UIW_INTEGER::DataGet(void)
{
	// Return the integer value.
	UIW_STRING::DataGet();
	*number = atoi(text);
	return (*number);
}

void UIW_INTEGER::DataSet(int *_number)
{
	// Reset the integer value.
	if (number == _number || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = _number;
	else if (_number)
		*number = *_number;
	else
		*number = 0;
	itoa(*number, text, 10);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_INTEGER::Event(const UI_EVENT &event)
{
	// Check for environment specific messages.
	EVENT_TYPE ccode = event.type;
	if (UIW_INTEGER::OSEvent(event, &ccode))
		return (ccode);

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_INTEGER);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			int tNumber = *number;				// Keep a temporary value in case of error.
			UIW_STRING::DataGet();
			*number = atoi(text);
			ccode = UIW_STRING::Event(event);	// Call the user or valitime function.
			if (ccode == -1)
				*number = tNumber;				// An error occurred, restore the old value.
			else
				woStatus &= ~WOS_UNANSWERED;
			itoa(*number, text, 10);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		itoa(*number, text, 10);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_INTEGER::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_INTEGER;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Check the default initialization.
		if (!defaultInitialized)
		{
			ZIL_LANGUAGE_MANAGER::SetLanguage(_className, STRINGS);
			defaultInitialized = TRUE;
		}
		myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className);

		// Set the object identification and variables.
		WindowID(ID_NUMBER);
		searchID = WindowID(ID_INTEGER);

		// Update the internal flag settings.
		UIW_INTEGER::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_STRING::Information(I_CHANGED_FLAGS, data, ID_INTEGER);

		// Update the internal flag settings.
		UIW_INTEGER::UpdateSettings(objectID);
		break;

	case I_GET_VALUE:
		// Get the integer value.
		if (!data)
			return (ZIL_NULLP(void));
		*(int *)data = UIW_INTEGER::DataGet();
		break;

	case I_SET_VALUE:
		// Set the integer value.
		UIW_INTEGER::DataSet((int *)data);
		break;

	case I_DECREMENT_VALUE:
		// Decrement the integer value.
		ValueDecrement(*(int *)data);
		break;

	case I_INCREMENT_VALUE:
		// Increment the integer value.
		ValueIncrement(*(int *)data);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_INTEGER)
			data = UIW_STRING::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &nmFlags;
		else if (request == I_GET_FLAGS)
			*(NMF_FLAGS *)data = nmFlags;
		else if (request == I_SET_FLAGS)
			nmFlags |= *(NMF_FLAGS *)data;
		else
			nmFlags &= ~(*(NMF_FLAGS *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_INTEGER::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_INTEGER::OSUpdateSettings(objectID);
}

int UIW_INTEGER::Validate(int processError)
{
	// Check for an absolute number error.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	int currentNumber = atoi(stringNumber);
	NMI_RESULT errorCode = NMI_OK;

	// Check for a range error.
	if (range && *range && errorCode == NMI_OK)
		errorCode = NMI_OUT_OF_RANGE;
	int numRanges = 0;
	int low, high;
	ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
	ZIL_ICHAR rBuffer[1024];				// Localized range string for error messages.
	rBuffer[0] = '\0';
	for (ZIL_ICHAR *tRange = range; tRange && errorCode == NMI_OUT_OF_RANGE; numRanges++)
	{
		tRange = ParseRange(tRange, minNumber, maxNumber);
		low = atoi(minNumber);
		high = atoi(maxNumber);
		if ((!minNumber[0] || currentNumber >= low) && (!maxNumber[0] || currentNumber <= high))
			errorCode = NMI_OK;
		else if (processError)
		{
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minNumber[0])
				itoa(low, &rBuffer[strlen(rBuffer)], 10);
			strcat(rBuffer, numSeparator);
			if (maxNumber[0])
				itoa(high, &rBuffer[strlen(rBuffer)], 10);
		}
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == NMI_OK)			// Set up the new number.
	{
		itoa(currentNumber, stringNumber, 10);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
		return (errorCode);
	}
	if (!errorSystem)				// Restore the original number.
	{
		itoa(*number, stringNumber, 10);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
		return (errorCode);
	}
	if (!processError)
		return (errorCode);

	// Generate the error message and wait for a response.
	if (errorCode == NMI_OUT_OF_RANGE)
	{
		if (!range || !*range)
			errorCode = NMI_INVALID;
		else if (numRanges == 1)
		{
			if (minNumber[0] && !maxNumber[0])
			{
				errorCode = NMI_LESS_THAN_RANGE;
				itoa(low, rBuffer, 10);
			}
			else if (!minNumber[0] && maxNumber[0])
			{
				errorCode = NMI_GREATER_THAN_RANGE;
				itoa(high, rBuffer, 10);
			}
		}
	}
	ZIL_ICHAR *errStr = myLanguage->GetMessage(errorCode);
	if (errStr)
	{
		WOS_STATUS _woStatus = woStatus;
		woStatus |= WOS_INTERNAL_ACTION;
		UIS_STATUS errorStatus = errorSystem->ReportError(windowManager,
			WOS_INVALID, errStr, stringNumber, rBuffer);
		if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
			woStatus &= ~WOS_INTERNAL_ACTION;
		if (errorStatus == WOS_INVALID)
			return (-1);		// This will cause the number to be restored.
		woStatus |= WOS_INVALID;
	}
	if (errorCode == NMI_OUT_OF_RANGE ||
		errorCode == NMI_LESS_THAN_RANGE || errorCode == NMI_GREATER_THAN_RANGE)
		return (0);
	return (errorCode);
}

void UIW_INTEGER::ValueDecrement(int delta)
{
	// Decrement the integer value.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	int tNumber = atoi(stringNumber);
	int originalNum = tNumber;
	tNumber -= delta;
	itoa(tNumber, stringNumber, 10);
	UIW_STRING::Information(I_SET_TEXT, stringNumber);
	if (Validate(FALSE) == NMI_OK)
		*number = tNumber;
	else
	{
		int nextRangeMax, max;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		int rangeMax = atoi(maxNumber);

		nextRangeMax = max = rangeMax;

		while (tRange)
		{
			tRange = ParseRange(tRange, minNumber, maxNumber);
			rangeMax = atoi(maxNumber);

			if (!maxNumber[0])
				wrap = FALSE;
			else
			{
				// Find the largest allowed value.
				if (rangeMax > max)
					max = rangeMax;

				// Find the maximum value less than the current value.
				if (rangeMax > nextRangeMax && rangeMax < originalNum)
					nextRangeMax = rangeMax;
				else if (nextRangeMax > originalNum && rangeMax < nextRangeMax)
					nextRangeMax = rangeMax;
			}
		}

		// If can go lower, then do so.
		if (nextRangeMax < originalNum)
			*number = nextRangeMax;
		// Otherwise, we are off the bottom of the ranges and need to wrap.
		else if (wrap && parent && parent->Inherited(ID_SPIN_CONTROL))
		{
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
				*number = max;
		}

		itoa(*number, stringNumber, 10);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
	}
}

void UIW_INTEGER::ValueIncrement(int delta)
{
	// Increment the integer value.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	int tNumber = atoi(stringNumber);
	int originalNum = tNumber;
	tNumber += delta;
	itoa(tNumber, stringNumber, 10);
	UIW_STRING::Information(I_SET_TEXT, stringNumber);
	if (Validate(FALSE) == NMI_OK)
		*number = tNumber;
	else
	{
		int nextRangeMin, min;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		int rangeMin = atoi(minNumber);

		nextRangeMin = min = rangeMin;

		while (tRange)
		{
			tRange = ParseRange(tRange, minNumber, maxNumber);
			rangeMin = atoi(minNumber);

			if (!minNumber[0])
				wrap = FALSE;
			else
			{
				// Find the smallest allowed value.
				if (rangeMin < min)
					min = rangeMin;

				// Find the minimum value greater than the current value.
				if (rangeMin < nextRangeMin && rangeMin > originalNum)
					nextRangeMin = rangeMin;
				else if (nextRangeMin < originalNum && rangeMin > nextRangeMin)
					nextRangeMin = rangeMin;
			}
		}

		// If can go higher, then do so.
		if (nextRangeMin > originalNum)
			*number = nextRangeMin;
		// Otherwise, we are off the top of the ranges and need to wrap.
		else if (wrap && parent && parent->Inherited(ID_SPIN_CONTROL))
		{
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
				*number = min;
		}

		itoa(*number, stringNumber, 10);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
	}
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_INTEGER::NewFunction(void) { return (UIW_INTEGER::New); }

UIW_INTEGER::UIW_INTEGER(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STRING(0, 0, 10, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the number information.
	UIW_INTEGER::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_INTEGER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_INTEGER::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the integer information.
	UIW_STRING::Load(name, directory, file, objectTable, userTable);
	file->Load(&nmFlags);
	file->Load(&range);

#if defined(ZIL_MACINTOSH)
	number = new int[1];
#else
	number = new int;
#endif
	*number = atoi(text);
}
#endif

#if defined(ZIL_STORE)
void UIW_INTEGER::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the integer information.
	UIW_STRING::Store(name, directory, file, objectTable, userTable);
	file->Store(nmFlags);
	file->Store(range);
}
#endif
