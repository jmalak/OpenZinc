//	Zinc Interface Library - Z_DATE1.CPP
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

const static ZIL_ICHAR dateSeparator[3] = { '.','.',0 };
const static ZIL_ICHAR rangeSeparator[3] = { ',',' ',0 };

#define ZIL_UIW_DATE_DATA
#include "lang_def.cpp"

#define MAX_LENGTH	64

// ----- UIW_DATE -----------------------------------------------------------

UIW_DATE::UIW_DATE(int left, int top, int width, ZIL_DATE *_date,
	const ZIL_ICHAR *_range, DTF_FLAGS _dtFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	date(ZIL_NULLP(ZIL_DATE)), dtFlags(_dtFlags)
{
	// Initialize the date information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		date = new ZIL_DATE;
	else
	{
		text = new ZIL_ICHAR[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = strdup(_range);
	UIW_DATE::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_DATE::DataSet(_date);
	if (FlagSet(woFlags, WOF_UNANSWERED))
		date->Import(-1);
}

UIW_DATE::~UIW_DATE(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
	// Restore the date information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete date;
	else
		delete text;
	if (range)
		delete range;
}

ZIL_DATE *UIW_DATE::DataGet(void)
{
	// Return the date.
	UIW_STRING::DataGet();
	date->Import(text, dtFlags);
	return (date);
}

void UIW_DATE::DataSet(ZIL_DATE *_date)
{
	// Reset the date.
	if (date == _date || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		date = _date;
	else if (_date)
		date->Import(*_date);
	else
		date->Import();
	if (date->jday == 0 && date->usec < 0)
		woStatus |= WOS_UNANSWERED;
	else
		woStatus &= ~WOS_UNANSWERED;
	date->Export(text, dtFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_DATE::Event(const UI_EVENT &event)
{
	// Check for environment specific messages.
	EVENT_TYPE ccode = event.type;
	if (UIW_DATE::OSEvent(event, &ccode))
		return (ccode);

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_DATE);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			ZIL_DATE tDate = *date;				// Keep a temporary date in case of error.
			UIW_STRING::DataGet();
			date->Import(text, dtFlags);
			ccode = UIW_STRING::Event(event);	// Call the user or validate function.
			if (ccode == -1)
				*date = tDate;					// An error occurred, restore the old date.
			else
				if (date->jday == 0 && date->usec < 0)
					woStatus |= WOS_UNANSWERED;
				else
					woStatus &= ~WOS_UNANSWERED;
			date->Export(text, dtFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		date->Export(text, dtFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_DATE::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_DATE;
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
		searchID = WindowID(ID_DATE);

		// Update the internal flag settings.
		UIW_DATE::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_STRING::Information(I_CHANGED_FLAGS, data, ID_DATE);

		// Update the internal flag settings.
		UIW_DATE::UpdateSettings(objectID);
		break;

	case I_GET_VALUE:
		// Get the date value.
		if (!data)
			return (UIW_DATE::DataGet());
		*(ZIL_DATE *)data = *(UIW_DATE::DataGet());
		break;

	case I_SET_VALUE:
		// Set the date value.
		UIW_DATE::DataSet((ZIL_DATE *)data);
		break;

	case I_DECREMENT_VALUE:
		// Decrement the date value.
		ValueDecrement(*(ZIL_INT32 *)data);
		break;

	case I_INCREMENT_VALUE:
		// Increment the date value.
		ValueIncrement(*(ZIL_INT32 *)data);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_DATE)
			data = UIW_STRING::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &dtFlags;
		else if (request == I_GET_FLAGS)
			*(DTF_FLAGS *)data = dtFlags;
		else if (request == I_SET_FLAGS)
			dtFlags |= *(DTF_FLAGS *)data;
		else
			dtFlags &= ~(*(DTF_FLAGS *)data);
		break;

	case I_SET_TEXT:
		date->Import((ZIL_ICHAR *)data, dtFlags);
		date->Export(text, dtFlags);
		UIW_STRING::Information(I_SET_TEXT, text);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_DATE::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_DATE::OSUpdateSettings(objectID);
}

int UIW_DATE::Validate(int processError)
{
	// Check for an absolute date error.
	ZIL_DATE currentDate;
	ZIL_ICHAR *stringDate = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	DTI_RESULT errorCode = currentDate.Import(stringDate, dtFlags);

	// Check for a range error.
	if (range && *range && errorCode == DTI_OK)
		errorCode = DTI_OUT_OF_RANGE;
	int numRanges = 0;
	ZIL_DATE low, high;
	ZIL_ICHAR minDate[MAX_LENGTH], maxDate[MAX_LENGTH];
	ZIL_ICHAR rBuffer[1024];				// Localized range string for error messages.
	rBuffer[0] = '\0';
	for (ZIL_ICHAR *tRange = range; tRange && errorCode == DTI_OUT_OF_RANGE; numRanges++)
	{
		tRange = ParseRange(tRange, minDate, maxDate);
		low.SetLocale(canonicalLocale->name);
		high.SetLocale(canonicalLocale->name);
		low.Import(minDate, DTF_NO_FLAGS);
		high.Import(maxDate, DTF_NO_FLAGS);
		low.SetLocale(currentDate.myLocale->name);
		high.SetLocale(currentDate.myLocale->name);
		if ((!minDate[0] || currentDate >= low) && (!maxDate[0] || currentDate <= high))
			errorCode = DTI_OK;
		else if (processError)
		{
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minDate[0])
				low.Export(&rBuffer[strlen(rBuffer)], dtFlags);
			strcat(rBuffer, dateSeparator);
			if (maxDate[0])
				high.Export(&rBuffer[strlen(rBuffer)], dtFlags);
		}
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == DTI_OK)			// Set up the new date.
	{
		currentDate.Export(stringDate, dtFlags);
		UIW_STRING::Information(I_SET_TEXT, stringDate);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original date.
	{
		date->Export(stringDate, dtFlags);
		UIW_STRING::Information(I_SET_TEXT, stringDate);
		return (errorCode);
	}
	else if (!processError)
		return (errorCode);

	// Generate the error message and wait for a response.
	if (errorCode == DTI_OUT_OF_RANGE)
	{
		if (!range || !*range)
			errorCode = DTI_INVALID;
		else if (numRanges == 1)
		{
			if (minDate[0] && !maxDate[0])
			{
				errorCode = DTI_LESS_THAN_RANGE;
				low.Export(rBuffer, dtFlags);
			}
			else if (!minDate[0] && maxDate[0])
			{
				errorCode = DTI_GREATER_THAN_RANGE;
				high.Export(rBuffer, dtFlags);
			}
		}
	}
	ZIL_ICHAR *errStr = myLanguage->GetMessage(errorCode, TRUE);
	if (errStr)
	{
		WOS_STATUS _woStatus = woStatus;
		woStatus |= WOS_INTERNAL_ACTION;
		woStatus |= WOS_INVALID;
		UIS_STATUS errorStatus = errorSystem->ReportError(windowManager,
			WOS_INVALID, errStr, stringDate, rBuffer);
		if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
			woStatus &= ~WOS_INTERNAL_ACTION;
		if (errorStatus == WOS_INVALID)
			return (-1);		// This will cause the date to be restored.
	}
	if (errorCode == DTI_INVALID || errorCode == DTI_OUT_OF_RANGE ||
		errorCode == DTI_LESS_THAN_RANGE || errorCode == DTI_GREATER_THAN_RANGE)
		return (0);
	return (errorCode);
}

void UIW_DATE::ValueDecrement(ZIL_INT32 delta)
{
	// Decrement the date value.
	ZIL_ICHAR *stringDate = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	ZIL_DATE tDate(stringDate, dtFlags);
    ZIL_DATE originalDate(stringDate, dtFlags);
	tDate -= delta;
	tDate.Export(stringDate, dtFlags);
	UIW_STRING::Information(I_SET_TEXT, stringDate);
	if (Validate(FALSE) == DTI_OK)
		date->Import(tDate);
	else
	{
		ZIL_DATE nextRangeMax, max;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minDate[MAX_LENGTH], maxDate[MAX_LENGTH];
		tRange = ParseRange(tRange, minDate, maxDate);
		ZIL_DATE rangeMax;
		rangeMax.SetLocale(canonicalLocale->name);
		rangeMax.Import(maxDate, DTF_NO_FLAGS);

		nextRangeMax = max = rangeMax;

		while (tRange)
		{
			tRange = ParseRange(tRange, minDate, maxDate);
			rangeMax.SetLocale(canonicalLocale->name);
			rangeMax.Import(maxDate, DTF_NO_FLAGS);

			if (!maxDate[0])
				wrap = FALSE;
			else
			{
				// Find the largest allowed value.
				if (rangeMax > max)
					max = rangeMax;

				// Find the maximum value less than the current value.
				if (rangeMax > nextRangeMax && rangeMax < originalDate)
					nextRangeMax = rangeMax;
				else if (nextRangeMax > originalDate && rangeMax < nextRangeMax)
					nextRangeMax = rangeMax;
			}
		}

		// If can go lower, then do so.
		if (nextRangeMax < originalDate)
			*date = nextRangeMax;
		// Otherwise, we are off the bottom of the ranges and need to wrap.
		else if (wrap && parent && parent->Inherited(ID_SPIN_CONTROL))
		{
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
				*date = max;
		}

		date->Export(stringDate, dtFlags);
		UIW_STRING::Information(I_SET_TEXT, stringDate);
	}
}

void UIW_DATE::ValueIncrement(ZIL_INT32 delta)
{
	// Increment the date value.
	ZIL_ICHAR *stringDate = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	ZIL_DATE tDate(stringDate, dtFlags);
    ZIL_DATE originalDate(stringDate, dtFlags);
	tDate += delta;
	tDate.Export(stringDate, dtFlags);
	UIW_STRING::Information(I_SET_TEXT, stringDate);
	if (Validate(FALSE) == NMI_OK)
		date->Import(tDate);
	else
	{
		ZIL_DATE nextRangeMin, min;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minDate[MAX_LENGTH], maxDate[MAX_LENGTH];
		tRange = ParseRange(tRange, minDate, maxDate);
		ZIL_DATE rangeMin;
		rangeMin.SetLocale(canonicalLocale->name);
		rangeMin.Import(minDate, DTF_NO_FLAGS);

		nextRangeMin = min = rangeMin;

		while (tRange)
		{
			tRange = ParseRange(tRange, minDate, maxDate);
			rangeMin.SetLocale(canonicalLocale->name);
			rangeMin.Import(minDate, DTF_NO_FLAGS);

			if (!minDate[0])
				wrap = FALSE;
			else
			{
				// Find the smallest allowed value.
				if (rangeMin < min)
					min = rangeMin;

				// Find the minimum value greater than the current value.
				if (rangeMin < nextRangeMin && rangeMin > originalDate)
					nextRangeMin = rangeMin;
				else if (nextRangeMin < originalDate && rangeMin > nextRangeMin)
					nextRangeMin = rangeMin;
			}
		}

		// If can go higher, then do so.
		if (nextRangeMin > originalDate)
			*date = nextRangeMin;
		// Otherwise, we are off the top of the ranges and need to wrap.
		else if (wrap && parent && parent->Inherited(ID_SPIN_CONTROL))
		{
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
				*date = min;
		}

		date->Export(stringDate, dtFlags);
		UIW_STRING::Information(I_SET_TEXT, stringDate);
	}
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_DATE::NewFunction(void) { return (UIW_DATE::New); }

UIW_DATE::UIW_DATE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STRING(0, 0, 15, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the date information.
	UIW_DATE::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_DATE::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_DATE::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the date information.
	UIW_STRING::Load(name, directory, file, objectTable, userTable);
	file->Load(&dtFlags);
#if defined(ZIL_3x_COMPAT)
	if (file->Storage()->Version() < 400)
	{
		ZIL_ICHAR *tmp;
		file->Load(&tmp);
		ZIL_DATE low, high;
		ZIL_ICHAR minDate[MAX_LENGTH], maxDate[MAX_LENGTH];
		ZIL_ICHAR rBuffer[1024];
		rBuffer[0] = 0;
		for (ZIL_ICHAR *tRange = tmp; tRange; )
		{
			tRange = ParseRange(tRange, minDate, maxDate);
			low.Import(minDate, rangeFlags | DTF_SHORT_YEAR);
			high.Import(maxDate, rangeFlags | DTF_SHORT_YEAR);
			low.SetLocale(canonicalLocale->name);
			high.SetLocale(canonicalLocale->name);
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minDate[0])
				low.Export(&rBuffer[strlen(rBuffer)], DTF_NO_FLAGS);
			strcat(rBuffer, dateSeparator);
			if (maxDate[0])
				high.Export(&rBuffer[strlen(rBuffer)], DTF_NO_FLAGS);
		}
		range = strdup(rBuffer);
		delete tmp;
	}
	else
		file->Load(&range);
	date = new ZIL_DATE;
	if (!FlagSet(dtFlags, DTF_SYSTEM))
	{
		if (file->Storage()->Version() < 400)
			date->Import(text);	// Set the date according to the loaded text.
		else
		{
			const ZIL_LOCALE *save = date->myLocale;
			date->myLocale = canonicalLocale;
			date->Import(text);	// Set the date according to the loaded text.
			date->myLocale = save;
		}
	}
#else
	file->Load(&range);
	date = new ZIL_DATE;
	if (!FlagSet(dtFlags, DTF_SYSTEM))
	{
		const ZIL_LOCALE *save = date->myLocale;
		date->myLocale = canonicalLocale;
		date->Import(text);	// Set the date according to the loaded text.
		date->myLocale = save;
	}
#endif
	date->Export(text, dtFlags);	// Set the text date
}
#endif

#if defined(ZIL_STORE)
void UIW_DATE::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Make sure information is current.
	DataGet();

	// Store the date information.
	const ZIL_LOCALE *save = date->myLocale;
	date->myLocale = canonicalLocale;
	date->Export(text, DTF_NO_FLAGS);	// Set the text to canonical locale

    // Go directly to the base class bypassing DataGet() in UIW_STRING.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(stFlags);
	ZIL_INT16 _value = maxLength; file->Store(_value);
	file->Store(text);

	date->myLocale = save;
	date->Export(text, dtFlags);	// Set the text to default locale.
	file->Store(dtFlags);
	file->Store(range);
}
#endif
