//	Zinc Interface Library - Z_GMGR.CPP
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

static ZIL_ICHAR _stringID[] = { 'N','U','M','I','D','_','G','E','O','M','E','T','R','Y', 0 };

// ----- UI_CONSTRAINT ------------------------------------------------------

UI_CONSTRAINT::UI_CONSTRAINT(UI_WINDOW_OBJECT *_object) :
	object(_object), numberID(0)
{
	UI_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UI_CONSTRAINT::~UI_CONSTRAINT(void)
{
}

void *UI_CONSTRAINT::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object according to numberID.
		// Use Manager() to set the manager before sending I_INITIALIZE_CLASS.
		if (numberID && manager && !object)
			object = (UI_WINDOW_OBJECT *)manager->Information(I_GET_NUMBERID_OBJECT, ZIL_VOIDP(&numberID));
		searchID = ID_CONSTRAINT;
		break;

	case I_GET_OBJECT:
		if (!data)
			return object;
		*(UI_WINDOW_OBJECT **)data = object;
		break;

	case I_SET_OBJECT:
		object = (UI_WINDOW_OBJECT *)data;
		break;

	case I_GET_NUMBERID:
		*(ZIL_NUMBERID *)data = numberID;
		break;

	case I_SET_NUMBERID:
		numberID = *(ZIL_NUMBERID *)data;
		break;

	default:
		data = ZIL_NULLP(void);
		break;
	}

	return data;
}

UI_GEOMETRY_MANAGER *UI_CONSTRAINT::Manager(UI_GEOMETRY_MANAGER *_manager)
{
	if (_manager)
		manager = _manager;
	return (manager);
}

#if defined(ZIL_LOAD)
UI_CONSTRAINT *UI_CONSTRAINT::New(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	if (!objectTable)
		objectTable = UI_WINDOW_OBJECT::objectTable;
	if (!userTable)
		userTable = UI_WINDOW_OBJECT::userTable;
	UI_CONSTRAINT *constraint = ZIL_NULLP(UI_CONSTRAINT);

	// Check for a valid directory and file.
	int tempDirectory = FALSE;
	if (name && !file)
	{
		ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
		ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
		if (!directory)
		{
			extern ZIL_ICHAR *_zilExtName;
			ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
			ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
			directory = new ZIL_STORAGE_READ_ONLY(pathName);
			if (directory->storageError)
			{
				delete directory;
				return (ZIL_NULLP(UI_CONSTRAINT));
			}
			tempDirectory = TRUE;
		}
		if (!file)
		{
			if (directory->storageError)
			{
				delete constraint;
				constraint = ZIL_NULLP(UI_CONSTRAINT);
			}
			if (tempDirectory)
				delete directory;
			return (ZIL_NULLP(UI_CONSTRAINT));
		}
	}

	// Read the constraint based on its identification.
	ZIL_INT16 searchID;
	file->Load(&searchID);
	for (int i = 0; objectTable[i].value != ID_END; i++)
		if (objectTable[i].value == searchID)
		{
#if defined(ZIL_EDIT)
			if (FlagsSet(UI_WINDOW_OBJECT::defaultStatus,
				WOS_EDIT_MODE | WOS_INTERNAL_ACTION) &&
				objectTable[i].flags > 1)
				i = objectTable[i].flags;
			objectTable[i].flags = TRUE;
#endif
			ZIL_NEW_FUNCTION newFunction = (ZIL_NEW_FUNCTION)objectTable[i].data;
			constraint = (UI_CONSTRAINT* )newFunction(ZIL_NULLP(ZIL_ICHAR),
				directory, file, objectTable, userTable);
			constraint->searchID = (ZIL_OBJECTID)objectTable[i].value;
			break;
		}

	// Clean up the file and storage then return the constraint.
	if (tempDirectory)
		delete directory;
	if (constraint)
		return (constraint);

	// Error report for unknown object types.
	return (ZIL_NULLP(UI_CONSTRAINT));
}

UI_CONSTRAINT::UI_CONSTRAINT(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	object(ZIL_NULLP(UI_WINDOW_OBJECT)), numberID(0)
{
	// Initialize the constraint information.
	UI_CONSTRAINT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UI_CONSTRAINT::Load(name, directory, file, objectTable, userTable);
}

void UI_CONSTRAINT::Load(const ZIL_ICHAR *,
	ZIL_STORAGE_READ_ONLY *, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *, UI_ITEM *)
{
	// Load the constraint information.
	file->Load(&numberID);
}
#endif

#if defined(ZIL_STORE)
void UI_CONSTRAINT::Store(const ZIL_ICHAR *, ZIL_STORAGE *,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *)
{
	// Store the constraint information.
	numberID = object ? object->NumberID() : numberID;
	file->Store(numberID);

	// Flag the object as being used.
	if (!objectTable)
		objectTable = UI_WINDOW_OBJECT::objectTable;
	for (int i = 0; objectTable && objectTable[i].value != ID_END; i++)
		if (objectTable[i].value == searchID)
		{
			objectTable[i].flags = TRUE;
			break;
		}
}
#endif

// ----- UI_GEOMETRY_MANAGER ------------------------------------------------

UI_GEOMETRY_MANAGER::UI_GEOMETRY_MANAGER(void) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_SUPPORT_OBJECT | WOF_NON_SELECTABLE, WOAF_NON_CURRENT)
{
	UI_GEOMETRY_MANAGER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UI_GEOMETRY_MANAGER::~UI_GEOMETRY_MANAGER(void)
{
}

UI_CONSTRAINT *UI_GEOMETRY_MANAGER::Add(UI_CONSTRAINT *constraint)
{
	UI_LIST::Add(constraint);
	constraint->Manager(this);
	return constraint;
}

UI_CONSTRAINT *UI_GEOMETRY_MANAGER::Subtract(UI_CONSTRAINT *constraint)
{
	UI_LIST::Subtract(constraint);
	return constraint;
}

EVENT_TYPE UI_GEOMETRY_MANAGER::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	return (FALSE);
}

EVENT_TYPE UI_GEOMETRY_MANAGER::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_GEOMETRY_MANAGER);
	UI_CONSTRAINT *constraint;
	switch (ccode)
	{
	case S_MOVE:
	case S_CHANGED:
	case S_CREATE:
		for (constraint = First(); constraint; constraint = constraint->Next())
			constraint->Modify();
		break;

	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		for (constraint = First(); constraint; constraint = constraint->Next())
			constraint->Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
		break;

	case S_ADD_OBJECT:
		Add((UI_CONSTRAINT *)event.data);
		break;

	case S_SUBTRACT_OBJECT:
		Subtract((UI_CONSTRAINT *)event.data);
		break;

	default:
		ccode = S_UNKNOWN;
		break;
	}
	return ccode;
}

void *UI_GEOMETRY_MANAGER::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_GEOMETRY_MANAGER;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_GEOMETRY_MANAGER;
		numberID = NUMID_GEOMETRY;
		strcpy(stringID, _stringID);
		break;

	case I_GET_NUMBERID_OBJECT:
		// Get the sibling window object from numberID.
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
			return(UI_WINDOW_OBJECT::Information(request, data, objectID));

		woStatus |= WOS_INTERNAL_ACTION;	// Prevent recursive calls.
		data = parent->Information(request, data, objectID);
		woStatus &= ~WOS_INTERNAL_ACTION;
		break;

	case I_GET_FIRST:
		if (!data)
			return first;
		*(UI_CONSTRAINT **)data = First();
		break;

	case I_GET_LAST:
		if (!data)
			return last;
		*(UI_CONSTRAINT **)data = Last();
		break;

	case I_GET_CURRENT:
		if (!data)
			return current;
		*(UI_CONSTRAINT **)data = Current();
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	return data;
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UI_GEOMETRY_MANAGER::NewFunction(void) { return (UI_GEOMETRY_MANAGER::New); }

UI_GEOMETRY_MANAGER::UI_GEOMETRY_MANAGER(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable, const ZIL_ICHAR *) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_SUPPORT_OBJECT | WOF_NON_SELECTABLE, WOAF_NON_CURRENT)
{
	// Initialize the text information.
	if (file)
		UI_GEOMETRY_MANAGER::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UI_GEOMETRY_MANAGER::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UI_GEOMETRY_MANAGER::Load(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable)
{
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);

	// Load the constraint information.
 	ZIL_INT16 noOfConstraints;
	file->Load(&noOfConstraints);
	for (int i = 0; i < noOfConstraints; i++)
	{
//		UI_CONSTRAINT *constraint = (UI_CONSTRAINT *)UI_WINDOW_OBJECT::New(
		UI_CONSTRAINT *constraint = UI_CONSTRAINT::New(
			ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
		if (constraint)
		{
			Add(constraint);
		}
	}
}
#endif

#if defined(ZIL_STORE)
void UI_GEOMETRY_MANAGER::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the geometry management.
	if (!objectTable)
		objectTable = UI_WINDOW_OBJECT::objectTable;
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	// Flag the geometry manager as being used.
	for (int i = 0; objectTable && objectTable[i].value != ID_END; i++)
		if (objectTable[i].value == searchID)
		{
			objectTable[i].flags = TRUE;
			break;
		}

	// Store the constraint.
	ZIL_INT16 noOfConstraints = UI_LIST::Count();
	file->Store(noOfConstraints);
	for (UI_CONSTRAINT *constraint = First(); constraint; constraint = constraint->Next())
	{
		// Store the constraint information.
		ZIL_OBJECTID searchID = constraint->SearchID();
		file->Store(searchID);
		constraint->Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
		// Flag the constraint as being used.
		for (int i = 0; objectTable && objectTable[i].value != ID_END; i++)
			if (objectTable[i].value == searchID)
			{
				objectTable[i].flags = TRUE;
				break;
			}
	}
}
#endif
