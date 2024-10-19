//	Zinc Interface Library - PREFER.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "message.hpp"
#define USE_ZAF_MESSAGE_PREFERENCES
#include "p_messag.hpp"

// --------------------------------------------------------------------------
// ----- ZAF_MESSAGE_PREFERENCES ---------------------------------------------
// --------------------------------------------------------------------------

static ZIL_ICHAR _windowName[] = { 'Z','A','F','_','M','E','S','S','A','G','E','_','P','R','E','F','E','R','E','N','C','E','S',0 };

ZAF_MESSAGE_PREFERENCES::ZAF_MESSAGE_PREFERENCES(void) :
	UIW_WINDOW(_windowName, ZAF_MESSAGE_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT), ZAF_MESSAGE_EDITOR::_objectTable,
	ZAF_MESSAGE_EDITOR::_userTable)
{
	// Load the default information.
	LoadOptions();

	// Center the window on the screen.
	windowManager->Center(this);
}

ZAF_MESSAGE_PREFERENCES::~ZAF_MESSAGE_PREFERENCES(void)
{
}

EVENT_TYPE ZAF_MESSAGE_PREFERENCES::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_OK:
		// Save the options.
		StoreOptions();
		// Continue to OPT_CANCEL.
	case OPT_CANCEL:
		// Remove the window from the window manager
		eventManager->Put(S_CLOSE, Q_BEGIN);
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(ZAF_MESSAGE_EDITOR::_storage, FALSE);
		break;

	default:
		// Default to the base window event.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void ZAF_MESSAGE_PREFERENCES::LoadOptions(void)
{
	// revisions
	int value = 0;
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	storageService->Information(I_GET_REVISIONS, &value);
	Get(BACKUP_FIELD)->Information(I_SET_VALUE, &value);
	storageService->Information(I_GET_WRITE_HPP, &value);
	if (value)
		Get(OPTION_WRITE_HPP)->woStatus |= WOS_SELECTED;
}

void ZAF_MESSAGE_PREFERENCES::StoreOptions(void)
{
	// revisions
	int value = 0;
	UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	Get(BACKUP_FIELD)->Information(I_GET_VALUE, &value);
	storageService->Information(I_SET_REVISIONS, &value);
	value = FlagSet(Get(OPTION_WRITE_HPP)->woStatus, WOS_SELECTED) ? TRUE : FALSE;
	storageService->Information(I_SET_WRITE_HPP, &value);
}

