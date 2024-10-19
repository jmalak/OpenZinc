//	Zinc Interface Library - Z_BNUM2.CPP
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

const static ZIL_ICHAR numberSeparator[3] = { '.','.',0 };
const static ZIL_ICHAR rangeSeparator[3] = { ',',' ',0 };

#define ZIL_UIW_BIGNUM_DATA
#include "lang_def.cpp"

#define MAX_LENGTH	64

// ----- UIW_BIGNUM ---------------------------------------------------------

UIW_BIGNUM::UIW_BIGNUM(int left, int top, int width, ZIL_BIGNUM *_number,
	const ZIL_ICHAR *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	number(ZIL_NULLP(ZIL_BIGNUM)), nmFlags(_nmFlags)
{
	// Initialize the number information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = new ZIL_BIGNUM;
	else
	{
		text = new ZIL_ICHAR[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = strdup(_range);
	UIW_BIGNUM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BIGNUM::DataSet(_number);
	if (FlagSet(woFlags, WOF_UNANSWERED))
		number->Import(ZIL_NULLP(ZIL_ICHAR));
}

UIW_BIGNUM::~UIW_BIGNUM(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
	// Restore the number information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete number;
	else
		delete text;
	if (range)
		delete range;
}

ZIL_BIGNUM *UIW_BIGNUM::DataGet(void)
{
	// Return the number.
	UIW_STRING::DataGet();
	number->Import(text);
	return (number);
}

void UIW_BIGNUM::DataSet(ZIL_BIGNUM *_number)
{
	// Reset the number.
	if (number == _number || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = _number;
	else if (_number)
		number->Import(*_number);
	else
		number->Import(_blankString);
	if (number->sign < 0)
		woStatus |= WOS_UNANSWERED;
	else
		woStatus &= ~WOS_UNANSWERED;
	number->Export(text, nmFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_BIGNUM::Event(const UI_EVENT &event)
{
	// Check for environment specific messages.
	EVENT_TYPE ccode = event.type;
	if (UIW_BIGNUM::OSEvent(event, &ccode))
		return (ccode);

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_BIGNUM);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			ZIL_BIGNUM tNumber = *number;		// Keep a temporary number in case of error.
			UIW_STRING::DataGet();
			number->Import(text);
			ccode = UIW_STRING::Event(event);	// Call the user or validate function.
			if (ccode == -1)
				*number = tNumber;				// An error occurred, restore the old number.
			else
				if (number->sign < 0)
					woStatus |= WOS_UNANSWERED;
				else
					woStatus &= ~WOS_UNANSWERED;
			number->Export(text, nmFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		number->Export(text, nmFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_BIGNUM::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_BIGNUM;
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
		searchID = WindowID(ID_BIGNUM);

		// Update the internal flag settings.
		UIW_BIGNUM::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_STRING::Information(I_CHANGED_FLAGS, data, ID_BIGNUM);

		// Update the internal flag settings.
		UIW_BIGNUM::UpdateSettings(objectID);
		break;

	case I_GET_VALUE:
		// Get the bignum value.
		if (!data)
			return (UIW_BIGNUM::DataGet());
		*(ZIL_BIGNUM *)data = *(UIW_BIGNUM::DataGet());
		break;

	case I_SET_VALUE:
		// Set the bignum value.
		UIW_BIGNUM::DataSet((ZIL_BIGNUM *)data);
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
		if (objectID && objectID != ID_BIGNUM)
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

	case I_SET_TEXT:
		number->Import((ZIL_ICHAR *)data);
		number->Export(text, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, text);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_BIGNUM::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_BIGNUM::OSUpdateSettings(objectID);
}

int UIW_BIGNUM::Validate(int processError)
{
	// Check for an absolute number error.
	ZIL_BIGNUM currentNumber;
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	NMI_RESULT errorCode = currentNumber.Import(stringNumber);

	// Check for a range error.
	if (range && *range && errorCode == NMI_OK)
		errorCode = NMI_OUT_OF_RANGE;
	int numRanges = 0;
	ZIL_BIGNUM low, high;
	ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
	ZIL_ICHAR rBuffer[1024];				// Localized range string for error messages.
	rBuffer[0] = '\0';
	for (ZIL_ICHAR *tRange = range; tRange && errorCode == NMI_OUT_OF_RANGE; numRanges++)
	{
		tRange = ParseRange(tRange, minNumber, maxNumber);
		low.SetLocale(canonicalLocale->name);
		high.SetLocale(canonicalLocale->name);
		low.Import(minNumber);
		high.Import(maxNumber);
		low.SetLocale(currentNumber.myLocale->name);
		high.SetLocale(currentNumber.myLocale->name);
		if ((!minNumber[0] || currentNumber >= low) && (!maxNumber[0] || currentNumber <= high))
			errorCode = NMI_OK;
		else if (processError)
		{
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minNumber[0])
				low.Export(&rBuffer[strlen(rBuffer)], nmFlags);
			strcat(rBuffer, numberSeparator);
			if (maxNumber[0])
				high.Export(&rBuffer[strlen(rBuffer)], nmFlags);
		}
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == NMI_OK)			// Set up the new number.
	{
		currentNumber.Export(stringNumber, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original number.
	{
		number->Export(stringNumber, nmFlags);
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
				low.Export(rBuffer, nmFlags);
			}
			else if (!minNumber[0] && maxNumber[0])
			{
				errorCode = NMI_GREATER_THAN_RANGE;
				high.Export(rBuffer, nmFlags);
			}
		}
	}
	ZIL_ICHAR *message = myLanguage->GetMessage(errorCode, TRUE);
	if (message != ZIL_NULLP(ZIL_ICHAR))
	{
		WOS_STATUS _woStatus = woStatus;
		woStatus |= WOS_INTERNAL_ACTION;
		woStatus |= WOS_INVALID;
		UIS_STATUS errorStatus = errorSystem->ReportError(windowManager,
			WOS_INVALID, message, stringNumber, rBuffer);
		if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
			woStatus &= ~WOS_INTERNAL_ACTION;
		if (errorStatus == WOS_INVALID)
			return (-1);		// This will cause the number to be restored.
	}
	if (errorCode == NMI_OUT_OF_RANGE ||
		errorCode == NMI_LESS_THAN_RANGE || errorCode == NMI_GREATER_THAN_RANGE)
		return (0);
	return (errorCode);
}

void UIW_BIGNUM::ValueDecrement(ZIL_INT32 delta)
{
	// Decrement the number value.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	ZIL_BIGNUM tNumber(stringNumber);
	ZIL_BIGNUM originalNum(stringNumber);
	tNumber -= delta;
	tNumber.Export(stringNumber, nmFlags);
	UIW_STRING::Information(I_SET_TEXT, stringNumber);
	if (Validate(FALSE) == TMI_OK)
		number->Import(tNumber);
	else
	{
		ZIL_BIGNUM nextRangeMax, max;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		ZIL_BIGNUM rangeMax;
		rangeMax.SetLocale(canonicalLocale->name);
		rangeMax.Import(maxNumber);

		nextRangeMax = max = rangeMax;

		while (tRange)
		{
			tRange = ParseRange(tRange, minNumber, maxNumber);
			rangeMax.SetLocale(canonicalLocale->name);
			rangeMax.Import(maxNumber);

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

		number->Export(stringNumber, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
	}
}

void UIW_BIGNUM::ValueIncrement(ZIL_INT32 delta)
{
	// Increment the number value.
	ZIL_ICHAR *stringNumber = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	ZIL_BIGNUM tNumber(stringNumber);
	ZIL_BIGNUM originalNum(stringNumber);
	tNumber += delta;
	tNumber.Export(stringNumber, nmFlags);
	UIW_STRING::Information(I_SET_TEXT, stringNumber);
	if (Validate(FALSE) == NMI_OK)
		number->Import(tNumber);
	else
	{
		ZIL_BIGNUM nextRangeMin, min;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		ZIL_BIGNUM rangeMin;
		rangeMin.SetLocale(canonicalLocale->name);
		rangeMin.Import(minNumber);

		nextRangeMin = min = rangeMin;

		while (tRange)
		{
			tRange = ParseRange(tRange, minNumber, maxNumber);
			rangeMin.SetLocale(canonicalLocale->name);
			rangeMin.Import(minNumber);

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

		number->Export(stringNumber, nmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringNumber);
	}
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_BIGNUM::NewFunction(void) { return (UIW_BIGNUM::New); }

UIW_BIGNUM::UIW_BIGNUM(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STRING(0, 0, 10, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the number information.
	UIW_BIGNUM::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BIGNUM::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_BIGNUM::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Read the file information.
	UIW_STRING::Load(name, directory, file, objectTable, userTable);
	file->Load(&nmFlags);
#if defined(ZIL_3x_COMPAT)
	if (file->Storage()->Version() < 400)
	{
		ZIL_ICHAR *tmp;
		file->Load(&tmp);
		ZIL_BIGNUM low, high;
		ZIL_ICHAR minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		ZIL_ICHAR rBuffer[1024];
		rBuffer[0] = 0;
		for (ZIL_ICHAR *tRange = tmp; tRange; )
		{
			tRange = ParseRange(tRange, minNumber, maxNumber);
			low.Import(minNumber);
			high.Import(maxNumber);
			low.SetLocale(canonicalLocale->name);
			high.SetLocale(canonicalLocale->name);
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minNumber[0])
				low.Export(&rBuffer[strlen(rBuffer)], NMF_DECIMAL(ZIL_NUMBER_DECIMAL));
			strcat(rBuffer, numberSeparator);
			if (maxNumber[0])
				high.Export(&rBuffer[strlen(rBuffer)], NMF_DECIMAL(ZIL_NUMBER_DECIMAL));
		}
		range = strdup(rBuffer);
		delete tmp;
	}
	else
		file->Load(&range);
	number = new ZIL_BIGNUM;
	if (file->Storage()->Version() < 400)
		number->Import(text);
	else
	{
		const ZIL_LOCALE *save = number->myLocale;
		number->myLocale = canonicalLocale;
		number->Import(text);
		number->myLocale = save;
	}
#else
	file->Load(&range);
	number = new ZIL_BIGNUM;
	// use the canonical decimalString, negativeSign
	const ZIL_LOCALE *save = number->myLocale;
	number->myLocale = canonicalLocale;
	number->Import(text);
	number->myLocale = save;
#endif
	number->Export(text, nmFlags);
}
#endif


#if defined(ZIL_STORE)
void UIW_BIGNUM::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Make sure information is current.
	DataGet();

	// use the canonical decimalString, negativeSign
	const ZIL_LOCALE *save = number->myLocale;
	number->myLocale = canonicalLocale;
	number->Export(text, NMF_DECIMAL(ZIL_NUMBER_DECIMAL));
	number->myLocale = save;

    // Go directly to the base class bypassing DataGet() in UIW_STRING.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(stFlags);
	ZIL_INT16 _value = maxLength; file->Store(_value);
	file->Store(text);

	number->Export(text, nmFlags);
	file->Store(nmFlags);
	file->Store(range);
}
#endif
