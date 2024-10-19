//	Zinc Interface Library - Z_REAL.CPP
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

static ZIL_ICHAR _realleFormat[] 	= { '%','e', 0 };
static ZIL_ICHAR _realdlgFormat[] = { '%','%','.','%','d','g', 0 };
static ZIL_ICHAR _reallgFormat[] 	= { '%','g', 0 };
const static ZIL_ICHAR numSeparator[3] = { '.','.',0 };
const static ZIL_ICHAR rangeSeparator[3] = { ',',' ',0 };

#define ZIL_UIW_REAL_DATA
#include "lang_def.cpp"

#define MAX_LENGTH	64

// ----- UIW_REAL -----------------------------------------------------------

UIW_REAL::UIW_REAL(int left, int top, int width, double *_number,
	const ZIL_ICHAR *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	number(ZIL_NULLP(double)), nmFlags(_nmFlags)
{
	// Initialize the real information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
#if defined(ZIL_MACINTOSH)
		number = new double[1];
#else
		number = new double;
#endif
	else
	{
		text = new ZIL_ICHAR[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = strdup(_range);
	UIW_REAL::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_REAL::DataSet(_number);
}

UIW_REAL::~UIW_REAL(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
	// Restore the real information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete number;
	else
		delete text;
	if (range)
		delete range;
}

double UIW_REAL::DataGet(void)
{
	// Return the double value.
	UIW_STRING::DataGet();
	*number = atof(text);
	return (*number);
}

void UIW_REAL::DataSet(double *_number)
{
	// Reset the double value.
	if (number == _number || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = _number;
	else if (_number)
		*number = *_number;
	else
		*number = 0.0;
	Format(text, *number, nmFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_REAL::Event(const UI_EVENT &event)
{
	// Check for environment specific messages.
	EVENT_TYPE ccode = event.type;
	if (UIW_REAL::OSEvent(event, &ccode))
		return (ccode);

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_REAL);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			double tNumber = *number;			// Keep a temporary value in case of error.
			UIW_STRING::DataGet();
			*number = atof(text);
			ccode = UIW_STRING::Event(event);	// Call the user or valitime function.
			if (ccode == -1)
				*number = tNumber;				// An error occurred, restore the old value.
			else
				woStatus &= ~WOS_UNANSWERED;
			Format(text, *number, nmFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		Format(text, *number, nmFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_REAL::Format(ZIL_ICHAR *text, double number, NMF_FLAGS flags)
{
	ZIL_ICHAR buffer[MAX_LENGTH];

	if (FlagSet(flags, NMF_SCIENTIFIC))
		strcpy(buffer, _realleFormat);
	else if (NMF_DIGITS(flags) >= 0)
		sprintf(buffer, _realdlgFormat, NMF_DIGITS(flags) + 1);
	else
		strcpy(buffer, _reallgFormat);
	sprintf(text, buffer, number);
}

void *UIW_REAL::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_REAL;
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
		searchID = WindowID(ID_REAL);

		// Update the internal flag settings.
		UIW_REAL::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_STRING::Information(I_CHANGED_FLAGS, data, ID_REAL);

		// Update the internal flag settings.
		UIW_REAL::UpdateSettings(objectID);
		break;

	case I_GET_VALUE:
		// Get the double value.
		if (!data)
			return (ZIL_NULLP(void));
		*(double *)data = UIW_REAL::DataGet();
		break;

	case I_SET_VALUE:
		// Set the double value.
		UIW_REAL::DataSet((double *)data);
		break;

	case I_DECREMENT_VALUE:
		// Decrement the integer value.
		ValueDecrement(*(ZIL_INT32 *)data);
		break;

	case I_INCREMENT_VALUE:
		// Increment the integer value.
		ValueIncrement(*(ZIL_INT32 *)data);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_REAL)
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

void UIW_REAL::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_REAL::OSUpdateSettings(objectID);
}

int UIW_REAL::Validate(int processError)
{
	// Check for an absolute number error.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(ZIL_ICHAR));
	double currentNumber = atof(stringNumber);
	NMI_RESULT errorCode = NMI_OK;

	// Check for a range error.
	if (range && *range && errorCode == NMI_OK)
		errorCode = NMI_OUT_OF_RANGE;
	int numRanges = 0;
	double low, high;
	ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
	ZIL_ICHAR rBuffer[1024];				// Localized range string for error messages.
	rBuffer[0] = '\0';
	for (ZIL_ICHAR *tRange = range; tRange && errorCode == NMI_OUT_OF_RANGE; numRanges++)
	{
		tRange = ParseRange(tRange, minNumber, maxNumber);
		low = atof(minNumber);
		high = atof(maxNumber);
		if ((!minNumber[0] || currentNumber >= low) && (!maxNumber[0] || currentNumber <= high))
			errorCode = NMI_OK;
		else if (processError)
		{
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minNumber[0])
				Format(&rBuffer[strlen(rBuffer)], low, nmFlags);
			strcat(rBuffer, numSeparator);
			if (maxNumber[0])
				Format(&rBuffer[strlen(rBuffer)], high, nmFlags);
		}
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == NMI_OK)			// Set up the new number.
	{
		Format(stringNumber, currentNumber, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original number.
	{
		Format(stringNumber, *number, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
		return (errorCode);
	}
	else if (!processError)
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
				Format(rBuffer, low, nmFlags);
			}
			else if (!minNumber[0] && maxNumber[0])
			{
				errorCode = NMI_GREATER_THAN_RANGE;
				Format(rBuffer, high, nmFlags);
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

void UIW_REAL::ValueDecrement(ZIL_INT32 delta)
{
	// Decrement the real value.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	double tNumber = atof(stringNumber);
	double originalNum = tNumber;
	tNumber -= delta;
	Format(stringNumber, tNumber, nmFlags);
	UIW_STRING::Information(I_SET_TEXT, stringNumber);
	if (Validate(FALSE) == NMI_OK)
		*number = tNumber;
	else
	{
		double nextRangeMax, max;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		double rangeMax = atof(maxNumber);

		nextRangeMax = max = rangeMax;

		while (tRange)
		{
			tRange = ParseRange(tRange, minNumber, maxNumber);
			rangeMax = atof(maxNumber);

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

		Format(stringNumber, *number, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
	}
}

void UIW_REAL::ValueIncrement(ZIL_INT32 delta)
{
	// Increment the integer value.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	double tNumber = atof(stringNumber);
	double originalNum = tNumber;
	tNumber += delta;
	Format(stringNumber, tNumber, nmFlags);
	UIW_STRING::Information(I_SET_TEXT, stringNumber);
	if (Validate(FALSE) == NMI_OK)
		*number = tNumber;
	else
	{
		double nextRangeMin, min;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		double rangeMin = atof(minNumber);

		nextRangeMin = min = rangeMin;

		while (tRange)
		{
			tRange = ParseRange(tRange, minNumber, maxNumber);
			rangeMin = atof(minNumber);

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

		Format(stringNumber, *number, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
	}
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_REAL::NewFunction(void) { return (UIW_REAL::New); }

UIW_REAL::UIW_REAL(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STRING(0, 0, 10, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the number information.
	UIW_REAL::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_REAL::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_REAL::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the real information.
	UIW_STRING::Load(name, directory, file, objectTable, userTable);
	file->Load(&nmFlags);
	file->Load(&range);

#if defined(ZIL_MACINTOSH)
	number = new double[1];
#else
	number = new double;
#endif
	*number = atof(text);
}
#endif

#if defined(ZIL_STORE)
void UIW_REAL::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the real information.
	UIW_STRING::Store(name, directory, file, objectTable, userTable);
	file->Store(nmFlags);
	file->Store(range);
}
#endif

