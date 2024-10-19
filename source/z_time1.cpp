//	Zinc Interface Library - Z_TIME1.CPP
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

const static ZIL_ICHAR timeSeparator[3] = { '.','.',0 };
const static ZIL_ICHAR rangeSeparator[3] = { ',',' ',0 };

#define ZIL_UIW_TIME_DATA
#include "lang_def.cpp"

#define MAX_LENGTH	64

// ----- UIW_TIME -----------------------------------------------------------

UIW_TIME::UIW_TIME(int left, int top, int width, ZIL_TIME *_time,
	const ZIL_ICHAR *_range, TMF_FLAGS _tmFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	time(ZIL_NULLP(ZIL_TIME)), tmFlags(_tmFlags)
{
	// Initialize the time information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		time = new ZIL_TIME;
	else
	{
		text = new ZIL_ICHAR[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = strdup(_range);
	UIW_TIME::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TIME::DataSet(_time);
	if (FlagSet(woFlags, WOF_UNANSWERED))
		time->Import(-1);
}

UIW_TIME::~UIW_TIME(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
	// Restore the time information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete time;
	else
		delete text;
	if (range)
		delete range;
}

ZIL_TIME *UIW_TIME::DataGet(void)
{
	// Return the time.
	UIW_STRING::DataGet();
	time->Import(text, tmFlags);
	return (time);
}

void UIW_TIME::DataSet(ZIL_TIME *_time)
{
	// Reset the time.
	if (time == _time || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		time = _time;
	else if (_time)
		time->Import(*_time);
	else
		time->Import();
	if (time->jday == 0 && time->usec < 0)
		woStatus |= WOS_UNANSWERED;
	else
		woStatus &= ~WOS_UNANSWERED;
	time->Export(text, tmFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_TIME::Event(const UI_EVENT &event)
{
	// Check for environment specific messages.
	EVENT_TYPE ccode = event.type;
	if (UIW_TIME::OSEvent(event, &ccode))
		return (ccode);

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_TIME);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			ZIL_TIME tTime = *time;				// Keep a temporary time in case of error.
			UIW_STRING::DataGet();
			time->Import(text, tmFlags);
			ccode = UIW_STRING::Event(event);	// Call the user or validate function.
			if (ccode == -1)
				*time = tTime;					// An error occurred, restore the old time.
			else
				if (time->jday == 0 && time->usec < 0)
					woStatus |= WOS_UNANSWERED;
				else
					woStatus &= ~WOS_UNANSWERED;
			time->Export(text, tmFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		time->Export(text, tmFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_TIME::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_TIME;
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
		searchID = WindowID(ID_TIME);

		// Update the internal flag settings.
		UIW_TIME::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_STRING::Information(I_CHANGED_FLAGS, data, ID_TIME);

		// Update the internal flag settings.
		UIW_TIME::UpdateSettings(objectID);
		break;

	case I_GET_VALUE:
		// Get the time value.
		if (!data)
			return (UIW_TIME::DataGet());
		*(ZIL_TIME *)data = *(UIW_TIME::DataGet());
		break;

	case I_SET_VALUE:
		// Set the time value.
		UIW_TIME::DataSet((ZIL_TIME *)data);
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
		if (objectID && objectID != ID_TIME)
			data = UIW_STRING::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &tmFlags;
		else if (request == I_GET_FLAGS)
			*(TMF_FLAGS *)data = tmFlags;
		else if (request == I_SET_FLAGS)
			tmFlags |= *(TMF_FLAGS *)data;
		else
			tmFlags &= ~(*(TMF_FLAGS *)data);
		break;

	case I_SET_TEXT:
		time->Import((ZIL_ICHAR *)data, tmFlags);
		time->Export(text, tmFlags);
		UIW_STRING::Information(I_SET_TEXT, text);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_TIME::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_TIME::OSUpdateSettings(objectID);
}

int UIW_TIME::Validate(int processError)
{
	// Check for an absolute time error.
	ZIL_TIME currentTime;
	ZIL_ICHAR *stringTime = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	TMI_RESULT errorCode = currentTime.Import(stringTime, tmFlags);

	// Check for a range error.
	if (range && *range && errorCode == TMI_OK)
		errorCode = TMI_OUT_OF_RANGE;
	int numRanges = 0;
	ZIL_TIME low, high;
	ZIL_ICHAR minTime[MAX_LENGTH], maxTime[MAX_LENGTH];
	ZIL_ICHAR rBuffer[1024];				// Localized range string for error messages.
	rBuffer[0] = '\0';
	for (ZIL_ICHAR *tRange = range; tRange && errorCode == TMI_OUT_OF_RANGE; numRanges++)
	{
		tRange = ParseRange(tRange, minTime, maxTime);
		low.SetLocale(canonicalLocale->name);
		high.SetLocale(canonicalLocale->name);
		low.Import(minTime, TMF_NO_FLAGS);
		high.Import(maxTime, TMF_NO_FLAGS);
		low.SetLocale(currentTime.myLocale->name);
		high.SetLocale(currentTime.myLocale->name);
		if ((!minTime[0] || currentTime >= low) && (!maxTime[0] || currentTime <= high))
			errorCode = TMI_OK;
		else if (processError)
		{
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minTime[0])
				low.Export(&rBuffer[strlen(rBuffer)], tmFlags);
			strcat(rBuffer, timeSeparator);
			if (maxTime[0])
				high.Export(&rBuffer[strlen(rBuffer)], tmFlags);
		}
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == TMI_OK)			// Set up the new time.
	{
		currentTime.Export(stringTime, tmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringTime);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original time.
	{
		time->Export(stringTime, tmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringTime);
		return (errorCode);
	}
	else if (!processError)
		return (errorCode);

	// Generate the error message and wait for a response.
	if (errorCode == TMI_OUT_OF_RANGE)
	{
		if (!range || !*range)
			errorCode = TMI_INVALID;
		else if (numRanges == 1)
		{
			if (minTime[0] && !maxTime[0])
			{
				errorCode = TMI_LESS_THAN_RANGE;
				low.Export(rBuffer, tmFlags);
			}
			else if (!minTime[0] && maxTime[0])
			{
				errorCode = TMI_GREATER_THAN_RANGE;
				high.Export(rBuffer, tmFlags);
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
			WOS_INVALID, errStr, stringTime, rBuffer);
		if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
			woStatus &= ~WOS_INTERNAL_ACTION;
		if (errorStatus == WOS_INVALID)
			return (-1);		// This will cause the time to be restored.
	}
	if (errorCode == TMI_INVALID || errorCode == TMI_OUT_OF_RANGE ||
		errorCode == TMI_LESS_THAN_RANGE || errorCode == TMI_GREATER_THAN_RANGE)
		return (0);
	return (errorCode);
}

void UIW_TIME::ValueDecrement(ZIL_INT32 delta)
{
	// Decrement the time value.
	if (!FlagSet(tmFlags, TMF_HUNDREDTHS))
		delta *= 100;
	if (!FlagSet(tmFlags, TMF_HUNDREDTHS | TMF_SECONDS))
		delta *= 60;
	if (FlagSet(tmFlags, TMF_NO_MINUTES) && !FlagSet(tmFlags, TMF_HUNDREDTHS | TMF_SECONDS))
		delta *= 60;
	ZIL_ICHAR *stringTime = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
	ZIL_TIME tTime(stringTime, tmFlags);
	ZIL_TIME originalTime(stringTime, tmFlags);
	tTime -= delta;
	tTime.Export(stringTime, tmFlags);
	UIW_STRING::Information(I_SET_TEXT, stringTime);
	if (Validate(FALSE) == TMI_OK)
		time->Import(tTime);
	else
	{
		ZIL_TIME nextRangeMax, max;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minTime[MAX_LENGTH], maxTime[MAX_LENGTH];
		tRange = ParseRange(tRange, minTime, maxTime);
		ZIL_TIME rangeMax;
		rangeMax.SetLocale(canonicalLocale->name);
		rangeMax.Import(maxTime, TMF_NO_FLAGS);

		nextRangeMax = max = rangeMax;

		while (tRange)
		{
			tRange = ParseRange(tRange, minTime, maxTime);
			rangeMax.SetLocale(canonicalLocale->name);
			rangeMax.Import(maxTime, TMF_NO_FLAGS);

			if (!maxTime[0])
				wrap = FALSE;
			else
			{
				// Find the largest allowed value.
				if (rangeMax > max)
					max = rangeMax;

				// Find the maximum value less than the current value.
				if (rangeMax > nextRangeMax && rangeMax < originalTime)
					nextRangeMax = rangeMax;
				else if (nextRangeMax > originalTime && rangeMax < nextRangeMax)
					nextRangeMax = rangeMax;
			}
		}

		// If can go lower, then do so.
		if (nextRangeMax < originalTime)
			*time = nextRangeMax;
		// Otherwise, we are off the bottom of the ranges and need to wrap.
		else if (wrap && parent && parent->Inherited(ID_SPIN_CONTROL))
		{
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
				*time = max;
		}

		time->Export(stringTime, tmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringTime);
	}
}

void UIW_TIME::ValueIncrement(ZIL_INT32 delta)
{
	// Increment the time value.
	if (!FlagSet(tmFlags, TMF_HUNDREDTHS))
		delta *= 100;
	if (!FlagSet(tmFlags, TMF_HUNDREDTHS | TMF_SECONDS))
		delta *= 60;
	if (FlagSet(tmFlags, TMF_NO_MINUTES) && !FlagSet(tmFlags, TMF_HUNDREDTHS | TMF_SECONDS))
		delta *= 60;
	ZIL_ICHAR *stringTime = (ZIL_ICHAR *)UIW_STRING::Information(I_GET_TEXT, ZIL_NULLP(void));
  	ZIL_TIME tTime(stringTime, tmFlags);
	ZIL_TIME originalTime(stringTime, tmFlags);
	tTime += delta;
	tTime.Export(stringTime, tmFlags);
	UIW_STRING::Information(I_SET_TEXT, stringTime);
	if (Validate(FALSE) == NMI_OK)
		time->Import(tTime);
	else
	{
		ZIL_TIME nextRangeMin, min;
		int wrap = TRUE;

		ZIL_ICHAR *tRange = range;
		ZIL_ICHAR minTime[MAX_LENGTH], maxTime[MAX_LENGTH];
		tRange = ParseRange(tRange, minTime, maxTime);
		ZIL_TIME rangeMin;
		rangeMin.SetLocale(canonicalLocale->name);
		rangeMin.Import(minTime, TMF_NO_FLAGS);

		nextRangeMin = min = rangeMin;

		while (tRange)
		{
			tRange = ParseRange(tRange, minTime, maxTime);
			rangeMin.SetLocale(canonicalLocale->name);
			rangeMin.Import(minTime, TMF_NO_FLAGS);

			if (!minTime[0])
				wrap = FALSE;
			else
			{
				// Find the smallest allowed value.
				if (rangeMin < min)
					min = rangeMin;

				// Find the minimum value greater than the current value.
				if (rangeMin < nextRangeMin && rangeMin > originalTime)
					nextRangeMin = rangeMin;
				else if (nextRangeMin < originalTime && rangeMin > nextRangeMin)
					nextRangeMin = rangeMin;
			}
		}

		// If can go higher, then do so.
		if (nextRangeMin > originalTime)
			*time = nextRangeMin;
		// Otherwise, we are off the top of the ranges and need to wrap.
		else if (wrap && parent && parent->Inherited(ID_SPIN_CONTROL))
		{
			WNF_FLAGS wnFlags;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (!FlagSet(wnFlags, WNF_NO_WRAP))
				*time = min;
		}

		time->Export(stringTime, tmFlags);
		UIW_STRING::Information(I_SET_TEXT, stringTime);
	}
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_TIME::NewFunction(void) { return (UIW_TIME::New); }

UIW_TIME::UIW_TIME(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STRING(0, 0, 10, ZIL_NULLP(ZIL_ICHAR), MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the time information.
	UIW_TIME::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TIME::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_TIME::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Read the file information.
	UIW_STRING::Load(name, directory, file, objectTable, userTable);
	file->Load(&tmFlags);
#if defined(ZIL_3x_COMPAT)
	if (file->Storage()->Version() < 400)
	{
		ZIL_ICHAR *tmp;
		file->Load(&tmp);
		ZIL_TIME low, high;
		ZIL_ICHAR minTime[MAX_LENGTH], maxTime[MAX_LENGTH];
		ZIL_ICHAR rBuffer[1024];
		rBuffer[0] = 0;
		for (ZIL_ICHAR *tRange = tmp; tRange; )
		{
			tRange = ParseRange(tRange, minTime, maxTime);
			low.Import(minTime, rangeFlags);
			high.Import(maxTime, rangeFlags);
			low.SetLocale(canonicalLocale->name);
			high.SetLocale(canonicalLocale->name);
			if (rBuffer[0])
				strcat(rBuffer, rangeSeparator);
			if (minTime[0])
				low.Export(&rBuffer[strlen(rBuffer)], TMF_NO_FLAGS);
			strcat(rBuffer, timeSeparator);
			if (maxTime[0])
				high.Export(&rBuffer[strlen(rBuffer)], TMF_NO_FLAGS);
		}
		range = strdup(rBuffer);
		delete tmp;
	}
	else
		file->Load(&range);
	time = new ZIL_TIME;
	if (!FlagSet(tmFlags, TMF_SYSTEM))
	{
		if (file->Storage()->Version() < 400)
			time->Import(text);	// Set the time according to the loaded text.
		else
		{
			const ZIL_LOCALE *save = time->myLocale;
			time->myLocale = canonicalLocale;
			time->Import(text);	// Set the time according to the loaded text.
			time->myLocale = save;
		}
	}
#else
	file->Load(&range);
	time = new ZIL_TIME;
	if (!FlagSet(tmFlags, TMF_SYSTEM))
	{
		const ZIL_LOCALE *save = time->myLocale;
		time->myLocale = canonicalLocale;
		time->Import(text);	// Set the time according to the loaded text.
		time->myLocale = save;
	}
#endif
	time->Export(text, tmFlags);	// set the text time
}
#endif

#if defined(ZIL_STORE)
void UIW_TIME::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Make sure information is current.
	DataGet();

	// Store the time information.
	const ZIL_LOCALE *save = time->myLocale;
	time->myLocale = canonicalLocale;
	time->Export(text, TMF_NO_FLAGS);	// Set the text to canonical locale

    // Go directly to the base class bypassing DataGet() in UIW_STRING.
    UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(stFlags);
	ZIL_INT16 _value = maxLength; file->Store(_value);
	file->Store(text);

	time->myLocale = save;
	time->Export(text, tmFlags);	// Set the text to default locale.
	file->Store(tmFlags);
	file->Store(range);
}
#endif
