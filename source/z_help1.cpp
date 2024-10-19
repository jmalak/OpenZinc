//	Zinc Interface Library - Z_HELP1.CPP
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

#define ZIL_UI_HELP_SYSTEM_DATA
#include "lang_def.cpp"

extern ZIL_ICHAR *_helpDirectory;
extern ZIL_ICHAR *_currentDirectory;
extern ZIL_ICHAR *_parentDirectory;
static ZIL_ICHAR _allObjects[] = { '*', 0 };
static ZIL_ICHAR _topicStringID[] = { 'T','O','P','I','C','_','C','O','N','T','E','X','T', 0 };

// --------------------------------------------------------------------------
// ----- UI_HELP_SYSTEM -----------------------------------------------------
// --------------------------------------------------------------------------

UI_HELP_SYSTEM::UI_HELP_SYSTEM(const ZIL_ICHAR *pathName, UI_WINDOW_MANAGER *, UI_HELP_CONTEXT ) :
	UI_HELP_STUB(),
	UIW_WINDOW(0, 0, 66, 12, WOF_NO_FLAGS, WOAF_MODAL | WOAF_NO_DESTROY),
	context(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the language information.
	if (!defaultInitialized)
	{
		ZIL_LANGUAGE_MANAGER::SetLanguage(_className, STRINGS);
		defaultInitialized = TRUE;
	}
	myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className);

	// Center the window.
	windowManager->Center(this);

	// Open the help file.
	helpFile = pathName ? new ZIL_STORAGE_READ_ONLY(pathName) : ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
	deleteHelpFile = helpFile ? TRUE : FALSE;

	// Create the general support fields.
	*this
		+ (borderField = new UIW_BORDER)
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ (titleField = new UIW_TITLE(myLanguage->GetMessage(MSG_HELP_TOPIC)))
		+ new UIW_ICON(0, 0, myLanguage->GetMessage(MSG_ICONNAME), myLanguage->GetMessage(MSG_ICONTITLE), ICF_MINIMIZE_OBJECT)
		+ (showButton = new UIW_BUTTON(2, 9, 14, myLanguage->GetMessage(MSG_SHOW_INDEX),
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE | BTF_DEFAULT_BUTTON,
			WOF_JUSTIFY_CENTER, ZIL_NULLF(ZIL_USER_FUNCTION), OPT_SHOW_INDEX))
		+ (closeButton = new UIW_BUTTON(48, 9, 14, myLanguage->GetMessage(MSG_CLOSE),
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, ZIL_NULLF(ZIL_USER_FUNCTION), OPT_CLOSE));

	// Create the topic fields.
	topicTitle = new UIW_PROMPT(2, 0, 60, _blankString, WOF_JUSTIFY_CENTER);
	topicContext = new UIW_TEXT(2, 2, 60, 6, context, -1, WNF_NO_FLAGS, WOF_BORDER | WOF_VIEW_ONLY | WOF_NO_ALLOCATE_DATA);
	topicContext->StringID(_topicStringID);
	*topicContext + new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL);
	topicTitle->RegionConvert(topicTitle->relative, FALSE);
	topicContext->RegionConvert(topicContext->relative, FALSE);

	// Create the index fields.
	indexTopic = new UIW_PROMPT(2, 0, 60, myLanguage->GetMessage(MSG_MESSAGE1), WOF_JUSTIFY_CENTER);
	indexInstruction = new UIW_PROMPT(2, 1, 60, myLanguage->GetMessage(MSG_MESSAGE2), WOF_JUSTIFY_CENTER);
	indexTitle = new UIW_STRING(2, 2, 60, _blankString, 100);
	indexContext = new UIW_VT_LIST(2, 3, 60, 5, ZIL_NULLF(ZIL_COMPARE_FUNCTION), WNF_AUTO_SORT | WNF_NO_WRAP);
	*indexContext + new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL);
	indexTopic->RegionConvert(indexTopic->relative, FALSE);
	indexInstruction->RegionConvert(indexInstruction->relative, FALSE);
	indexTitle->RegionConvert(indexTitle->relative, FALSE);
	indexContext->RegionConvert(indexContext->relative, FALSE);
	AddIndexFields();
}

UI_HELP_SYSTEM::~UI_HELP_SYSTEM(void)
{
	// Remove and delete the support windows.
	if (windowManager)
		*windowManager - this;
	if (context)
		delete context;

	*this
		+ indexTopic
		+ indexInstruction
		+ indexContext
		+ indexTitle
		+ topicTitle
		+ topicContext;

	// Restore the language information.
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
}

void UI_HELP_SYSTEM::AddIndexFields(void)
{
	// Load the help contexts.
	if (helpFile && !indexContext->First())
	{
		ZIL_ICHAR title[256];
		helpFile->ChDir(_helpDirectory);
		indexContext->Destroy();
		for (ZIL_ICHAR *entry = helpFile->FindFirstObject(_allObjects); entry; entry = helpFile->FindNextObject())
			if (strcmp(entry, _currentDirectory) && strcmp(entry, _parentDirectory))
			{
				ZIL_STORAGE_OBJECT_READ_ONLY hObject(*helpFile, entry, 0);
				hObject.Load(title, 256);
				UIW_BUTTON *button = new UIW_BUTTON(0, 0, 30, title,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
					ItemCallback);
				button->StringID(entry);
				button->helpContext = hObject.objectID;
				*indexContext + button;
			}
	}

	// Reset the title and show buttons.
	titleField->DataSet(myLanguage->GetMessage(MSG_HELP_INDEX));
	showButton->DataSet(myLanguage->GetMessage(MSG_SHOW_TOPIC));
	showButton->value = OPT_SHOW_TOPIC;

	// Add the support fields.
	*this
		+ indexTopic
		+ indexInstruction
		+ indexContext
		+ indexTitle;
}

void UI_HELP_SYSTEM::AddTopicFields(ZIL_ICHAR *helpTitle, ZIL_ICHAR *helpContext)
{
	// Reset the title and context fields.
	topicTitle->DataSet(helpTitle);
	topicContext->DataSet(helpContext, strlen(helpContext));

	// Reset the title and show buttons.
	titleField->DataSet(myLanguage->GetMessage(MSG_HELP_TOPIC));
	showButton->DataSet(myLanguage->GetMessage(MSG_SHOW_INDEX));
	showButton->value = OPT_SHOW_INDEX;

	// Add the support fields.
	*this
		+ topicTitle
		+ topicContext;
}

void UI_HELP_SYSTEM::ComputeGeometry(void)
{
	// Check for absolute window sizes.
	const int MIN_WIDTH = 50 * display->cellWidth;
	const int MIN_HEIGHT = 8 * display->cellHeight;
	if (!IsMinimized() &&
		(relative.Width() < MIN_WIDTH || relative.Height() < MIN_HEIGHT))
	{
#if defined(ZIL_MSDOS)
		if (relative.Width() < MIN_WIDTH)
			relative.right = relative.left + MIN_WIDTH - 1;
		if (relative.Height() < MIN_HEIGHT)
			relative.bottom = relative.top + MIN_HEIGHT - 1;
#else
		// Return without recomputing the fields until geometry is enhanced.
		return;
#endif
	}

	// Perform geometry management on children.
	int spacing, rightEdge, bottomEdge;
	int buttonHeight = showButton->relative.Height();
	if (!display->isText)
	{
		spacing = showButton->relative.left; // perimeter spacing.
#if defined(ZIL_MACINTOSH)
		rightEdge = relative.Width() - spacing;
		bottomEdge = relative.Height() - borderField->DataGet() - spacing;
#else
		rightEdge = relative.Width() - 2 * borderField->DataGet() - spacing;
		bottomEdge = relative.Height() - 2 * borderField->DataGet() - spacing - titleField->relative.Height();
#endif
	}
	else
	{
		spacing = 1; // perimeter spacing.
		rightEdge = relative.Width() - 2 * borderField->DataGet() - spacing;
		bottomEdge = relative.Height() - 2 * borderField->DataGet() - spacing;
	}
	// right edge
	closeButton->relative.left =
		rightEdge - closeButton->relative.Width() + 1;
	closeButton->woStatus |= WOS_REDISPLAY;
	indexTopic->relative.right =
		indexInstruction->relative.right =
		indexTitle->relative.right =
		indexContext->relative.right =
		topicTitle->relative.right =
		topicContext->relative.right =
		closeButton->relative.right = rightEdge;
	indexTopic->woStatus |= WOS_REDISPLAY;
	// bottom edge
	showButton->relative.top =
		closeButton->relative.top = bottomEdge - buttonHeight + 1;
	showButton->relative.bottom =
		closeButton->relative.bottom = bottomEdge;
	showButton->woStatus |= WOS_REDISPLAY;
	// list items
	indexContext->relative.bottom =
		topicContext->relative.bottom =
		bottomEdge - buttonHeight - spacing;
	indexContext->woStatus |= WOS_REDISPLAY;
	topicContext->woStatus |= WOS_REDISPLAY;
}

void UI_HELP_SYSTEM::DisplayHelp(UI_WINDOW_MANAGER *windowManager, UI_HELP_CONTEXT helpContext)
{
	// Check for a valid help context.
	if (helpContext)
	{
		ZIL_ICHAR title[256];
		helpFile->ChDir(_helpDirectory);
		ZIL_ICHAR *entry = helpFile->FindFirstID(helpContext);
		if (entry && strcmp(entry, _currentDirectory) && strcmp(entry, _parentDirectory))
		{
			ZIL_ICHAR *tContext = context; // save no-allocate data pointer.
			ZIL_STORAGE_OBJECT_READ_ONLY hFile(*helpFile, entry, helpContext);
			hFile.Load(title, 256);
			hFile.Load(&context);
			RemoveIndexFields();
			AddTopicFields(title, context);
			if (tContext)
				delete tContext;
		}
		else
			UI_ERROR_STUB::Beep();
	}
	else
	{
		RemoveTopicFields();
		AddIndexFields();
	}

	// Invoke the index help.
	if (windowManager->Index(this) == -1)
		*windowManager + this;
	else
		Event(S_REDISPLAY);
}

void UI_HELP_SYSTEM::DisplayHelp(UI_WINDOW_MANAGER *windowManager, ZIL_ICHAR *name)
{
	// Check for a valid help context.
	if (name && name[0])
	{
		int length = strlen(name);
		ZIL_ICHAR title[256];
		for (ZIL_ICHAR *entry = helpFile->FindFirstObject(_allObjects); entry; entry = helpFile->FindNextObject())
			if (strcmp(entry, _currentDirectory) && strcmp(entry, _parentDirectory))
			{
				ZIL_STORAGE_OBJECT_READ_ONLY hObject(*helpFile, entry, 0);
				hObject.Load(title, 256);
				if (!strnicmp(name, title, length))
				{
					ZIL_ICHAR *tContext = context; // save no-allocate data pointer.
					hObject.Load(&context);
					RemoveIndexFields();
					AddTopicFields(title, context);
					if (tContext)
						delete tContext;
					// Update the list field.
					for (UI_WINDOW_OBJECT *object = indexContext->First(); object; object = object->Next())
						if (!strcmp(object->StringID(), entry))
						{
							// Make the object current and selected.
							object->woStatus |= WOS_SELECTED;
							indexContext->Information(I_CHECK_SELECTION, object);
							indexContext->Add(object);
							indexTitle->Information(I_SET_TEXT, object->Information(I_GET_TEXT, ZIL_NULLP(void)));
							break;
						}
				}
			}
	}
	else
	{
		RemoveTopicFields();
		AddIndexFields();
	}

	// Invoke the index help.
	if (windowManager->Index(this) == -1)
		*windowManager + this;
	else
		Event(S_REDISPLAY);
}

EVENT_TYPE UI_HELP_SYSTEM::Event(const UI_EVENT &event)
{
	// Switch on the event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CREATE:
	case S_CHANGED:
	case S_SIZE:
		ComputeGeometry();
		ccode = UIW_WINDOW::Event(event);
		break;

	case OPT_CLOSE:
		*windowManager - this;			// Remove the help topic window.
		break;

	case OPT_SHOW_INDEX:
		RemoveTopicFields();
		AddIndexFields();
		Event(S_REDISPLAY);
		break;

	case OPT_SHOW_TOPIC:
		// Display the help context.
		DisplayHelp(windowManager, indexTitle->DataGet());
		break;

	case OPT_SELECT_TOPIC:
		// Display the help context.
		DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case OPT_UPDATE_NAME:
		// Reset the title field.  This message is sent from ItemCallback().
		indexTitle->Information(I_SET_TEXT, event.windowObject->Information(I_GET_TEXT, ZIL_NULLP(void)));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UI_HELP_SYSTEM::ItemCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	// Check for selection of the field.
	if (ccode == L_DOUBLE_CLICK || ccode == L_SELECT)
	{
		// Update the help context selection.
		event.type = OPT_UPDATE_NAME;
		event.windowObject = object;
		object->eventManager->Put(event);
		// Show the associated help.
		if (ccode == L_DOUBLE_CLICK)
		{
			event.type = OPT_SELECT_TOPIC;
			object->eventManager->Put(event);
		}
	}
	return (ccode);
}

void UI_HELP_SYSTEM::RemoveIndexFields(void)
{
	// Remove the support fields.
	*this
		- indexTopic
		- indexInstruction
		- indexTitle
		- indexContext;
}

void UI_HELP_SYSTEM::RemoveTopicFields(void)
{
	// Remove the support fields.
	*this
		- topicTitle
		- topicContext;
}

void UI_HELP_SYSTEM::ResetStorage(ZIL_STORAGE_READ_ONLY *_helpFile, int deleteNewStorage)
{
	// Reset the storage and update the index fields.
	if (_helpFile == helpFile)
		return;
	else if (deleteHelpFile && helpFile)
		delete helpFile;

	helpFile = _helpFile;
	deleteHelpFile = deleteNewStorage;

	indexContext->Destroy();
}

