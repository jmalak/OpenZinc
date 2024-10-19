//	Zinc Interface Library - HELP.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "help.hpp"
#define USE_UIW_HELP_WINDOW
#include "p_help.hpp"

extern ZIL_ICHAR *_helpDirectory;

// ----- UIW_HELP_WINDOW ----------------------------------------------------

static ZIL_ICHAR _helpWindow[] = { 'U','I','W','_','H','E','L','P','_','W','I','N','D','O','W',0 };

UIW_HELP_WINDOW::UIW_HELP_WINDOW(ZIL_ICHAR *name) :
	UIW_WINDOW(_helpWindow, ZAF_HELP_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), ZAF_HELP_EDITOR::_objectTable,
	ZAF_HELP_EDITOR::_userTable), helpID(NO_HELP_CONTEXT)
{
	// Set the help context title and identification.
	searchID = ID_HELP_WINDOW;
	if (!name)
	{
		StringID(_serviceManager->ZMSG_tempResourceName());
		Information(I_SET_TEXT, StringID());
	}
	else
		Information(I_SET_TEXT, StringID(name));

	// Center the window on the screen.
	windowManager->Center(this);
}

UIW_HELP_WINDOW::~UIW_HELP_WINDOW(void)
{
}

EVENT_TYPE UIW_HELP_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_STORE:
		// Let the help editor handle the store request.
		_serviceManager->Get(ID_HELP_EDITOR)->Event(OPT_RESOURCE_STORE);
		break;

	case OPT_STORE_AS:
		// Let the help editor handle the store-as request.
		_serviceManager->Get(ID_HELP_EDITOR)->Event(OPT_RESOURCE_STOREAS);
		break;

	case OPT_CLOSE:
		// Close the window.
		eventManager->Put(S_CLOSE);
		break;

	case OPT_HELP:
		// Call the help system.
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_DEINITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (!ZAF_SERVICE_MANAGER::_queuedEvent)
			_serviceManager->Get(ID_HELP_EDITOR)->Information(I_STATUS_CLEAR, ZIL_NULLP(void));
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(ZAF_HELP_EDITOR::_storage, FALSE);
		_serviceManager->Get(ID_HELP_EDITOR)->Information(I_STATUS_UPDATE, this);
		break;

	default:
		// Default to the base UIW_WINDOW class.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_HELP_WINDOW::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *, UI_ITEM *)
{
	ZIL_ICHAR *text;
	if (name)
		file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, name, 0);
	else
	{
		Information(I_GET_TEXT, &text);
		file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, text, 0);
	}

	helpID = file->objectID;
	int contextID = helpID;
	Get(FIELD_CONTEXTID)->Information(I_SET_VALUE, &contextID);

	file->Load(&text);
	Get(FIELD_TITLE)->Information(I_SET_TEXT, text);
	delete text;

	file->Load(&text);
	Get(FIELD_MESSAGE)->Information(I_SET_TEXT, text);
	delete text;

	delete file;
}

void UIW_HELP_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *, UI_ITEM *)
{
	// Check for an invalid file name.
	if (name)
		Information(I_SET_TEXT, StringID(name));
	else
		name = StringID();

	// Move to the help directory.
	if (directory->ChDir(_helpDirectory))
	{
		directory->MkDir(_helpDirectory);
		directory->ChDir(_helpDirectory);
	}
	// Get the help ID.
	if (helpID == NO_HELP_CONTEXT || !directory->FindFirstObject(name))
	{
		UI_WINDOW_OBJECT *helpEditor = _serviceManager->Get(ID_HELP_EDITOR);
		if (helpEditor)
			helpEditor->Information(GET_HELP_IDENTIFICATION, &helpID);
		int contextID = helpID;
		Get(FIELD_CONTEXTID)->Information(I_SET_VALUE, &contextID);
	}

	// Create the help context object.
	file = new ZIL_STORAGE_OBJECT(*directory, name, helpID, UIS_CREATE | UIS_READWRITE);

	ZIL_ICHAR *text;
	Get(FIELD_TITLE)->Information(I_GET_TEXT, &text);
	file->Store(text);

	Get(FIELD_MESSAGE)->Information(I_GET_TEXT, &text);
	file->Store(text);

	delete file;
}

