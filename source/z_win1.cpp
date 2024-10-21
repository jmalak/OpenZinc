//	Zinc Interface Library - Z_WIN1.CPP
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

#define ZIL_UIW_WINDOW_DATA
#include "lang_def.cpp"

extern ZIL_ICHAR *_windowDirectory;
extern ZIL_ICHAR *_hppDirectory;
extern ZIL_ICHAR *_cppDirectory;
extern ZIL_ICHAR *_objectIDName;
extern ZIL_ICHAR *_userFunctionName;
extern ZIL_ICHAR *_compareFunctionName;
extern ZIL_ICHAR *_zilExtName;

// ----- UIW_WINDOW ---------------------------------------------------------

UIW_WINDOW::UIW_WINDOW(int left, int top, int width, int height,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags,
	UI_HELP_CONTEXT _helpContext, UI_WINDOW_OBJECT *minObject) :
	UI_WINDOW_OBJECT(left, top, width, height, _woFlags, _woAdvancedFlags),
	UI_LIST(), support(), clipList(), wnFlags(WNF_NO_FLAGS),
	defaultObject(ZIL_NULLP(UI_WINDOW_OBJECT)), myLanguage(ZIL_NULLP(ZIL_LANGUAGE))
{
	// Initialize the window information.
	compareFunctionName = ZIL_NULLP(ZIL_ICHAR);
	helpContext = _helpContext;
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	if (minObject)
	{
		minObject->woFlags |= WOF_SUPPORT_OBJECT;
		UIW_WINDOW::Add(minObject);
	}
}

UIW_WINDOW::~UIW_WINDOW(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
	if (compareFunctionName)
		delete compareFunctionName;
}

void UIW_WINDOW::Destroy(void)
{
	// Destroy the list elements (high level).
	UIW_WINDOW::OSDestroy();

	// Destroy the list elements (low level).
	UI_LIST::Destroy();

	defaultObject = ZIL_NULLP(UI_WINDOW_OBJECT);
}

void *UIW_WINDOW::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	UI_WINDOW_OBJECT *object;
	if (objectID == ID_DEFAULT)
		objectID = ID_WINDOW;
	switch (request)
	{
	case I_INITIALIZE_CLASS_FIRST:
		if (!defaultInitialized)
		{
			ZIL_LANGUAGE_MANAGER::SetLanguage(_className, STRINGS);
			defaultInitialized = TRUE;
		}
		myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className);
		break;

	case I_INITIALIZE_CLASS:
		if (!myLanguage)
			UIW_WINDOW::Information(I_INITIALIZE_CLASS_FIRST, ZIL_NULLP(void));

		// Set the object identification and variables.
		searchID = WindowID(ID_WINDOW);
		font = FNT_SYSTEM_FONT;
		if (FlagSet(wnFlags, WNF_AUTO_SORT))
			compareFunction = UIW_WINDOW::StringCompare;
		vScrollInfo.minimum = vScrollInfo.maximum = vScrollInfo.current = 0;
		hScrollInfo.minimum = hScrollInfo.maximum = hScrollInfo.current = 0;
		scroll.top = scroll.left = 0;

		// Initialize the default system settings.
		UIW_WINDOW::OSInitialize();

		// Update the internal flag settings.
		UIW_WINDOW::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
#if defined (ZIL_OS2)
		if (parent && parent->Inherited(ID_NOTEBOOK))
			return(parent->Information(request, data, objectID));
#elif defined(ZIL_MOTIF)
		// Send request to parent if non-field region object.
		if (parent && FlagSet(woFlags, WOF_NON_FIELD_REGION) &&
			!FlagSet(woStatus, WOS_EDIT_MODE))
			return(parent->Information(request, data, objectID));
#endif
		// Check the base class flag settings.
		UI_WINDOW_OBJECT::Information(I_CHANGED_FLAGS, data, ID_WINDOW);
		
		// Update the internal flag settings.
		UIW_WINDOW::UpdateSettings(objectID);
		break;

	case I_DESTROY_LIST:
		Destroy();
		break;

	case I_CHECK_SELECTION:
		CheckSelection((UI_WINDOW_OBJECT *)data);
		break;

	case I_GET_NUMBERID_OBJECT:
	case I_GET_STRINGID_OBJECT:
		{
		void *match = UI_WINDOW_OBJECT::Information(request, data, objectID);
		// Special support numbers should only go 1 level deep.
		if (request == I_GET_NUMBERID_OBJECT && *(ZIL_NUMBERID *)data >= (ZIL_NUMBERID)0xFF00)
		{
			for (object = (UI_WINDOW_OBJECT *)support.First(); object && !match; object = object->Next())
				match = object->UI_WINDOW_OBJECT::Information(request, data, objectID);
			for (object = First(); object && !match; object = object->Next())
				match = object->UI_WINDOW_OBJECT::Information(request, data, objectID);
		}
		// All others are depth first searches.
		else
		{
			for (object = (UI_WINDOW_OBJECT *)support.First(); object && !match; object = object->Next())
				match = object->Information(request, data, objectID);
			for (object = First(); object && !match; object = object->Next())
				match = object->Information(request, data, objectID);
		}
		data = match;
		}
		break;

	case I_SET_TEXT:
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (object->Inherited(ID_TITLE) || object->Inherited(ID_ICON))
				object->Information(request, data, objectID);
		break;

	case I_GET_TEXT:
	case I_COPY_TEXT:
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (object->Inherited(ID_TITLE) || object->Inherited(ID_ICON))
				return object->Information(request, data, objectID);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_WINDOW)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &wnFlags;
		else if (request == I_GET_FLAGS)
			*(WNF_FLAGS *)data = wnFlags;
		else if (request == I_SET_FLAGS)
			wnFlags |= *(WNF_FLAGS *)data;
		else
			wnFlags &= ~(*(WNF_FLAGS *)data);
		break;

	case I_GET_FIRST:
		if (!data)
			return first;
		*(UI_WINDOW_OBJECT **)data = First();
		break;

	case I_GET_LAST:
		if (!data)
			return last;
		*(UI_WINDOW_OBJECT **)data = Last();
		break;

	case I_GET_CURRENT:
		if (!data)
			return current;
		*(UI_WINDOW_OBJECT **)data = Current();
		break;

	case I_GET_SUPPORT_FIRST:
		if (!data)
			return support.First();
		*(UI_WINDOW_OBJECT **)data = (UI_WINDOW_OBJECT *)support.First();
		break;

	case I_GET_SUPPORT_LAST:
		if (!data)
			return support.Last();
		*(UI_WINDOW_OBJECT **)data = (UI_WINDOW_OBJECT *)support.Last();
		break;

	case I_GET_SUPPORT_CURRENT:
		if (!data)
			return support.Current();
		*(UI_WINDOW_OBJECT **)data = (UI_WINDOW_OBJECT *)support.Current();
		break;

	case I_GET_CLIPREGION:
		if (clipList.First())
			*(UI_REGION *)data = clipList.First()->region;
		else
		{
			*(UI_REGION *)data = trueRegion;
#if defined(ZIL_MSWINDOWS)
			if (screenID)
			{
				UI_REGION *region = (UI_REGION *)data;
				RECT rect;
				GetClientRect(frameID ? frameID : screenID, &rect);
				region->Assign(rect);
			}
#elif defined(ZIL_MOTIF)
			UI_REGION *region = (UI_REGION *)data;
			region->right -= region->left;
			region->bottom -= region->top;
			region->left = region->top = 0;
#endif
		}
		break;

	case I_SET_VSCROLL:	vScroll = (UI_WINDOW_OBJECT *)data;		break;
	case I_SET_HSCROLL:	hScroll = (UI_WINDOW_OBJECT *)data;		break;

	case I_GET_DEFAULT_OBJECT:
		if (!data)
			data = defaultObject;
		else
			*(UI_WINDOW_OBJECT **)data = defaultObject;
		break;

	case I_SET_DEFAULT_OBJECT:
		defaultObject = (UI_WINDOW_OBJECT *)data;
		break;

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	case I_GET_ICON:
		if (!data)
			return icon;
		*(UI_WINDOW_OBJECT **)data = icon;
		break;

	case I_SET_ICON:		icon = (UI_WINDOW_OBJECT *)data;		break;
#elif defined(ZIL_MSWINDOWS)
	case I_GET_FRAMEID:
		*(ZIL_SCREENID *)data = frameID ? frameID : screenID;		break;
	case I_SET_FRAMEID:
		frameID = *(ZIL_SCREENID *)data;							break;

	case I_GET_MENU:		*(HMENU *)data = menu;					break;
	case I_SET_MENU:		menu = *(HMENU *)data;					break;

	case I_SET_TITLE:		title = (UI_WINDOW_OBJECT *)data;		break;
	case I_SET_ICON:		icon = *(ZIL_ICON_HANDLE *)data;					break;

	case I_HIDE_SUBWINDOW:
		if (!current)
			break;
		else if (Current()->Inherited(ID_COMBO_BOX))
			SendMessage(Current()->screenID, CB_SHOWDROPDOWN, FALSE, 0);
		else
			Current()->Information(request, data, objectID);
		break;

#elif defined(ZIL_OS2)
	case I_SET_TITLE:		title = (UI_WINDOW_OBJECT *)data;		break;
	case I_GET_FRAMEID:
		*(ZIL_SCREENID *)data = frameID ? frameID : screenID;		break;
#elif defined(ZIL_MOTIF)
	case I_GET_DECORATIONS:	*(ZIL_UINT32 *)data = supportDecorations;	break;
	case I_SET_DECORATIONS:	supportDecorations |= *(ZIL_UINT32 *)data;	break;
#elif defined(ZIL_MACINTOSH)
	case I_GET_MENU:
		if (!data)
			return menu;
		*(UI_WINDOW_OBJECT **)data = menu;
		break;

	case I_SET_MENU:		menu = (UI_WINDOW_OBJECT *)data;		break;
#endif

#if defined(ZIL_LOAD) || defined(ZIL_STORE)
	case I_PRINT_INFORMATION:
	case I_PRINT_USER_FUNCTION:
	case I_PRINT_COMPARE_FUNCTION:
		if (request == I_PRINT_COMPARE_FUNCTION && compareFunctionName)
		{
			ZIL_STORAGE_OBJECT *cppEntry = (ZIL_STORAGE_OBJECT *)data;
			cppEntry->Store(compareFunctionName);
		}
		else if (request != I_PRINT_COMPARE_FUNCTION)
			UI_WINDOW_OBJECT::Information(request, data, objectID);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Information(request, data, objectID);
		for (object = First(); object; object = object->Next())
			object->Information(request, data, objectID);
		break;
#endif

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_WINDOW::StringCompare(void *object1, void *object2)
{
	ZIL_ICHAR *string1 = ZIL_NULLP(ZIL_ICHAR), *string2 = ZIL_NULLP(ZIL_ICHAR);
	((UI_WINDOW_OBJECT *)object1)->Information(I_GET_TEXT, &string1);
	if (!string1)
		return (-1);
	((UI_WINDOW_OBJECT *)object2)->Information(I_GET_TEXT, &string2);
	if (!string2)
		return (1);
#if defined(ZIL_NEXTSTEP) || (defined(__ZTC__) && !defined(ZIL_MACINTOSH))
	return (strcmp(string1,string2));
#else
	return (strcoll(string1, string2));
#endif
}

void UIW_WINDOW::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_WINDOW::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)

ZIL_NEW_FUNCTION UIW_WINDOW::NewFunction(void) { return (UIW_WINDOW::New); }

UIW_WINDOW::UIW_WINDOW(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 40, 10, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	UI_LIST(), support(), clipList(), defaultObject(ZIL_NULLP(UI_WINDOW_OBJECT)),
	myLanguage(ZIL_NULLP(ZIL_LANGUAGE))
{
	compareFunctionName = ZIL_NULLP(ZIL_ICHAR);
	if (name && !file)
	{
		ZIL_INT32 miniNumeratorX = display->miniNumeratorX;
		ZIL_INT32 miniDenominatorX = display->miniDenominatorX;
		ZIL_INT32 miniNumeratorY = display->miniNumeratorY;
		ZIL_INT32 miniDenominatorY = display->miniDenominatorY;
		UIW_WINDOW::Information(I_INITIALIZE_CLASS_FIRST, ZIL_NULLP(void));
		Load(name, directory, file, objectTable, userTable);
		WOS_STATUS woStatusSave = woStatus;
		UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		if (FlagSet(woStatusSave, WOS_READ_ERROR))
			return;
		display->miniNumeratorX = miniNumeratorX;
		display->miniDenominatorX = miniDenominatorX;
		display->miniNumeratorY = miniNumeratorY;
		display->miniDenominatorY = miniDenominatorY;
	}
	else
	{
		UIW_WINDOW::Information(I_INITIALIZE_CLASS_FIRST, ZIL_NULLP(void));
		Load(name, directory, file, objectTable, userTable);
		if (!FlagSet(woStatus, WOS_READ_ERROR))
		{
			UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
			UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		}
	}
}

void UIW_WINDOW::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Check for a valid directory and file.
	int tempDirectory = FALSE, tempFile = FALSE;
	if (name && !file)
	{
		ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
		ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
		if (!directory)
		{
			tempDirectory = TRUE;
			ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
			ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
			directory = new ZIL_STORAGE_READ_ONLY(pathName);
			if (directory->storageError)
			{
				UIW_WINDOW::Error(WOS_READ_ERROR, myLanguage->GetMessage(ZIL_NO_FILE), fileName);
				goto LOAD_ERROR;
			}
		}
		if (!file)
		{
			tempFile = TRUE;
			if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
				;
			else
				directory->ChDir(_windowDirectory);
			if (objectName[0] == '\0')
				strcpy(objectName, fileName);
			file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, objectName, ID_WINDOW);
			if (file->objectError)
			{
				Error(WOS_READ_ERROR, myLanguage->GetMessage(ZIL_NO_OBJ), objectName);
				goto LOAD_ERROR;
			}
			file->Load(&display->miniNumeratorX);	// miniNumeratorX
			file->Load(&display->miniDenominatorX);	// miniDenominatorX
			file->Load(&display->miniNumeratorY);	// miniNumeratorY
			file->Load(&display->miniDenominatorY);	// miniDenominatorY
		}
	}

	// Load the window information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);

	// Load the object information.
	{
 	ZIL_INT16 noOfObjects;
	file->Load(&noOfObjects);
	for (int i = 0; i < noOfObjects; i++)
	{
		UI_WINDOW_OBJECT *object = UI_WINDOW_OBJECT::New(ZIL_NULLP(ZIL_ICHAR),
			directory, file, objectTable, userTable);
		if (!object)
		{
			UIW_WINDOW::Error(WOS_READ_ERROR, myLanguage->GetMessage(ZIL_UNK_OBJ));
			goto LOAD_ERROR;
		}
		Add(object);
	}
	file->Load(&wnFlags);
	file->Load(&compareFunctionName);
	if (!userTable)
		 userTable = UI_WINDOW_OBJECT::userTable;
	if (compareFunctionName && userTable)
	{
		for (int i = 0; !compareFunction && userTable[i].data; i++)
			if (stricmp(userTable[i].text, compareFunctionName) == 0)
				compareFunction = (ZIL_COMPARE_FUNCTION)userTable[i].data;
	}
	}

LOAD_ERROR:
	// Clean up the file and storage.
	if (tempFile)
		delete file;
	if (tempDirectory)
		delete directory;
}
#endif

#if defined(ZIL_STORE)
void UIW_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Check for a valid directory and file.
	int tempDirectory = FALSE, tempFile = FALSE;
	if (!objectTable)
		objectTable = UI_WINDOW_OBJECT::objectTable;
	if (name && !file)
	{
		ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
		ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
		if (!directory)
		{
			ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
			ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
			directory = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
			if (directory->storageError)
			{
				delete directory;
				return;
			}
			tempDirectory = TRUE;
		}
		if (!file)
		{
			if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
				;
			else if (directory->ChDir(_windowDirectory))
			{
				directory->MkDir(_windowDirectory);
				directory->ChDir(_windowDirectory);
			}
			if (objectName[0] == '\0')
				strcpy(objectName, fileName);
			StringID(objectName);
			file = new ZIL_STORAGE_OBJECT(*directory, objectName, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
			file->Store(display->miniNumeratorX);	// miniNumeratorX
			file->Store(display->miniDenominatorX);	// miniDenominatorX
			file->Store(display->miniNumeratorY);	// miniNumeratorY
			file->Store(display->miniDenominatorY);	// miniDenominatorY
			tempFile = TRUE;
			// Reset the object table status.
			if (objectTable)
			{
				for (int i = 0; objectTable[i].value != ID_END; i++)
					objectTable[i].flags = FALSE;
			}
		}
	}

	// Store the object information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	ZIL_INT16 noOfObjects = support.Count() + UI_LIST::Count();
	file->Store(noOfObjects);
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First();
	for (; object; object = object->Next())
	{
		file->Store(object->searchID);
		object->Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
	}
	for (object = First(); object; object = object->Next())
	{
		file->Store(object->searchID);
		object->Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
	}
	file->Store(wnFlags);
	file->Store(compareFunctionName);

	// Generate the persistent table information.
	if (!parent && stringID[0] != '\0')
	{
		// Create the HPP entries.
		if (directory->ChDir(_hppDirectory))
		{
			directory->MkDir(_hppDirectory);
			directory->ChDir(_hppDirectory);
		}
		ZIL_STORAGE_OBJECT hppEntry(*directory, stringID, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Information(I_PRINT_INFORMATION, &hppEntry);
		for (object = First(); object; object = object->Next())
			object->Information(I_PRINT_INFORMATION, &hppEntry);
		hppEntry.Store(_blankString);	// End of HPP string entries.

		// Create the CPP entries.
		if (directory->ChDir(_cppDirectory))
		{
			directory->MkDir(_cppDirectory);
			directory->ChDir(_cppDirectory);
		}
		if (directory->ChDir(stringID))
		{
			directory->MkDir(stringID);
			directory->ChDir(stringID);
		}

		if (objectTable)
		{
			ZIL_OBJECTID objectID;
			ZIL_STORAGE_OBJECT cppEntry(*directory, _objectIDName, 0, UIS_CREATE | UIS_READWRITE);
			for (int i = 0; objectTable[i].value != ID_END; i++)
				if (objectTable[i].flags)
				{
					objectID = (ZIL_OBJECTID)objectTable[i].value;
					cppEntry.Store(objectID);
				}
				objectID = 0; cppEntry.Store(objectID);	// End of CPP object entries.
		}

		ZIL_STORAGE_OBJECT userEntry(*directory, _userFunctionName, 0, UIS_CREATE | UIS_READWRITE);
		Information(I_PRINT_USER_FUNCTION, &userEntry);
		userEntry.Store(_blankString);	// End of CPP user-function entries.

		ZIL_STORAGE_OBJECT compareEntry(*directory, _compareFunctionName, 0, UIS_CREATE | UIS_READWRITE);
		Information(I_PRINT_COMPARE_FUNCTION, &compareEntry);
		compareEntry.Store(_blankString);	// End of CPP compare-function entries.
	}

	// Cleanup the temporary file and directory.
	if (tempFile)
		delete file;
	if (tempDirectory)
	{
		directory->Save();
		delete directory;
	}
}
#endif

// ----- ERROR_HANDLING -----------------------------------------------------

#if defined(ZIL_DEBUG)
void UIW_WINDOW::Error(UIS_STATUS status, const ZIL_ICHAR *format, ...)
{
	ZIL_ICHAR *message = new char[256];
	va_list arguments;
   	va_start(arguments, format);
	ZIL_STDARG::vsprintf(message, format, arguments);
	va_end(arguments);

	// Display the error message before we abort.
#	if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	if (display)
		delete display;
	printf(message);
#	elif defined(ZIL_MSWINDOWS)
	MessageBox(0, message, ZIL_NULLP(ZIL_ICHAR), MB_OK);
#	elif defined(ZIL_OS2)
	WinMessageBox(HWND_DESKTOP, windowManager->First() ?
		windowManager->First()->screenID : HWND_DESKTOP, (PSZ)message,
		0, 0, flStyle);
#	elif defined(ZIL_MOTIF) || defined(ZIL_MACINTOSH)
	format = format;
	printf(message);
#	endif

	delete message;
	abort();
}
#else
void UIW_WINDOW::Error(UIS_STATUS status, const ZIL_ICHAR *, ...)
{
	// Flag parents with the error.
	for (UI_WINDOW_OBJECT *object = this; object; object = object->parent)
		object->woStatus |= status;
}
#endif
