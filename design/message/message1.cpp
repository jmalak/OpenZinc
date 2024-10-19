//	Zinc Interface Library - MESSAGE1.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "message.hpp"
#define USE_UIW_MESSAGE_WINDOW
#define USE_UIW_MESSAGE_INFORMATION
#include "p_messag.hpp"

extern ZIL_ICHAR *_hppDirectory;
extern ZIL_ICHAR *_messageDirectory;

static ZIL_ICHAR _msgFormat[] = { 'c','o','n','s','t',' ','Z','I','L','_','N','U','M','B','E','R','I','D',' ','%','-','3','2','s',' ','=',' ','0','x','%','0','4','X',';', 0 };

const int BUFFER_SIZE = 10;

const INFO_REQUEST I_GET_MESSAGE		= 7000;
const INFO_REQUEST I_SET_MESSAGE		= 7001;

class UIW_MESSAGE_ITEM : public UIW_BUTTON
{
public:
	ZIL_LANGUAGE_ELEMENT *message;
	UIW_MESSAGE_ITEM(ZIL_LANGUAGE_ELEMENT *message);
	~UIW_MESSAGE_ITEM(void);
#if defined(ZIL_LINUX)
	void SetButtonTitle(void);
#else
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
#endif
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static EVENT_TYPE ItemCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

class ZAF_MESSAGE_INFORMATION : public UIW_WINDOW
{
public:
	ZIL_LANGUAGE_ELEMENT *message;
	UI_WINDOW_OBJECT *item;
	ZAF_MESSAGE_INFORMATION(ZIL_LANGUAGE_ELEMENT *message, UI_WINDOW_OBJECT *item);
	~ZAF_MESSAGE_INFORMATION(void);
	EVENT_TYPE Event(const UI_EVENT &event);
};

// ----- UIW_MESSAGE_ITEM ---------------------------------------------------

UIW_MESSAGE_ITEM::UIW_MESSAGE_ITEM(ZIL_LANGUAGE_ELEMENT *_message) :
	UIW_BUTTON(0, 0, 30, ZIL_NULLP(ZIL_ICHAR), BTF_NO_3D | BTF_DOUBLE_CLICK,
	WOF_NO_FLAGS, ItemCallback),
	message(_message)
{

#if defined(ZIL_LINUX)
	font = 4;
	SetButtonTitle();
#else
	woStatus |= WOS_OWNERDRAW;
#endif

}

UIW_MESSAGE_ITEM::~UIW_MESSAGE_ITEM(void)
{
}

#if defined(ZIL_LINUX)

void UIW_MESSAGE_ITEM::SetButtonTitle(void)
{
	ZIL_ICHAR labelBuffer[66];
	//fill with 66 blanks and null terminate
	strcpy(labelBuffer, "                                                                 ");
	ZIL_ICHAR sValue[32];
	itoa(message->numberID, sValue, 10);
	//copy non null terminated string to labelBuffer
	strncpy(labelBuffer, sValue, strlen(sValue));
	strncpy(&labelBuffer[6], message->stringID, strlen(message->stringID));
	strncpy(&labelBuffer[36], message->text, strlen(message->text) < 29 ? strlen(message->text) : 29); 
	DataSet(labelBuffer);
}

#else

EVENT_TYPE UIW_MESSAGE_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	const int BUTTON_OFFSET = 4;
	const int FIELD1_OFFSET = 8;
	const int FIELD2_OFFSET = 30;

	UI_REGION region = trueRegion;
	display->VirtualGet(screenID, region);
	lastPalette = LogicalPalette(ccode, ID_LIST_ITEM);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);

	region.left += BUTTON_OFFSET;
	region.right = trueRegion.left + FIELD1_OFFSET * display->cellWidth - 1;
	ZIL_ICHAR sValue[32];
	itoa(message->numberID, sValue, 10);
	DrawText(screenID, region, sValue, lastPalette, FALSE, ccode);

	region = trueRegion;
	region.left += FIELD1_OFFSET * display->cellWidth;
	region.right = trueRegion.left + FIELD2_OFFSET * display->cellWidth - 1;
	DrawText(screenID, region, message->stringID, lastPalette, FALSE, ccode);

	ZIL_ICHAR *text = message->text;
	if (text)
	{
		region = trueRegion;
		region.left += FIELD2_OFFSET * display->cellWidth;
		DrawText(screenID, region, text, lastPalette, FALSE, ccode);
	}

	// Draw the focus rectangle.
	if (ccode == S_CURRENT)
		DrawFocus(screenID, trueRegion, ccode);

	display->VirtualPut(screenID);
	return (TRUE);
}

#endif

void *UIW_MESSAGE_ITEM::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	case I_GET_MESSAGE:
		*(ZIL_LANGUAGE_ELEMENT **)data = message;
		break;

	case I_SET_MESSAGE:
		message = (ZIL_LANGUAGE_ELEMENT *)data;
		break;

	default:
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	return (data);
}

EVENT_TYPE UIW_MESSAGE_ITEM::ItemCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		ZIL_LANGUAGE_ELEMENT *message;
		object->Information(I_GET_MESSAGE, &message);
		object->windowManager->Add(new ZAF_MESSAGE_INFORMATION(message, object));
	}
	return (ccode);
}

// ----- ZAF_MESSAGE_INFORMATION --------------------------------------------

static ZIL_ICHAR _messageInformation[] = { 'U','I','W','_','M','E','S','S','A','G','E','_','I','N','F','O','R','M','A','T','I','O','N',0 };

ZAF_MESSAGE_INFORMATION::ZAF_MESSAGE_INFORMATION(ZIL_LANGUAGE_ELEMENT *_message, UI_WINDOW_OBJECT *_item) :
	UIW_WINDOW(_messageInformation, ZAF_MESSAGE_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT), ZAF_MESSAGE_EDITOR::_objectTable,
	ZAF_MESSAGE_EDITOR::_userTable),
	message(_message), item(_item)
{
	// Set the message identification.
	searchID = ID_MESSAGE_WINDOW;

	// Set the message information: number, stringID, text.
	if (message)
	{
		int numberID = (int)message->numberID;
		Get(FIELD_NUMBERID)->Information(I_SET_VALUE, &numberID);

		ZIL_ICHAR *stringID = message->stringID;
		Get(FIELD_STRINGID)->Information(I_SET_TEXT, stringID);

		ZIL_ICHAR *messageText = message->text;
		Get(FIELD_MESSAGE)->Information(I_SET_TEXT, messageText);
	}

	// Center the window on the screen.
	windowManager->Center(this);
}

ZAF_MESSAGE_INFORMATION::~ZAF_MESSAGE_INFORMATION(void)
{
}

EVENT_TYPE ZAF_MESSAGE_INFORMATION::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_OK:
		if (message)
		{
			// Save the message information: numberID, stringID, text.
			UI_WINDOW_OBJECT *field = Get(FIELD_NUMBERID);
			if (FlagSet(field->woStatus, WOS_CHANGED))
			{
				int numberID;
				field->Information(I_GET_VALUE, &numberID);
				message->numberID = (NUMBERID)numberID;
			}
			field = Get(FIELD_STRINGID);
			if (FlagSet(field->woStatus, WOS_CHANGED))
			{
				ZIL_ICHAR *stringID;
				field->Information(I_GET_TEXT, &stringID);
				strcpy(message->stringID, stringID);
			}
			field = Get(FIELD_MESSAGE);
			if (FlagSet(field->woStatus, WOS_CHANGED))
			{
				ZIL_ICHAR *messageText;
				field->Information(I_GET_TEXT, &messageText);
				if (message->text)
					delete message->text;
				message->text = strdup(messageText);
			}

			// Redisplay the affected message item.
			UI_EVENT dspEvent(S_DISPLAY_ACTIVE);
			dspEvent.region = item->parent->trueRegion;
			UIW_MESSAGE_ITEM *messageItem = (UIW_MESSAGE_ITEM *)item;
#if defined(ZIL_LINUX)
			messageItem->SetButtonTitle();
#endif
			item->parent->Event(dspEvent);
		}
		eventManager->Put(S_CLOSE); // Close the window.
		break;

	case OPT_CANCEL:
		eventManager->Put(S_CLOSE); // Close the window.
		break;

	case OPT_HELP:
		helpSystem->ResetStorage(ZAF_MESSAGE_EDITOR::_storage, FALSE);
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- UIW_MESSAGE_WINDOW -------------------------------------------------

static ZIL_ICHAR _messageWindow[] = { 'U','I','W','_','M','E','S','S','A','G','E','_','W','I','N','D','O','W',0 };

UIW_MESSAGE_WINDOW::UIW_MESSAGE_WINDOW(ZIL_ICHAR *name) :
	UIW_WINDOW(_messageWindow, ZAF_MESSAGE_EDITOR::_storage,
	ZIL_NULLP(ZIL_STORAGE_OBJECT), ZAF_MESSAGE_EDITOR::_objectTable,
	ZAF_MESSAGE_EDITOR::_userTable),
	messageList(name, 0)
{
	// Set the help context title.
	searchID = ID_MESSAGE_WINDOW;
	if (!name)
	{
		StringID(_serviceManager->ZMSG_tempResourceName());
		Information(I_SET_TEXT, StringID());
	}
	else
		Information(I_SET_TEXT, StringID(name));

	// Center the window on the screen.
	windowManager->Center(this);

	// Set the list pointer.
	list = _serviceManager->CtrlList(this, LIST_MESSAGES);
}

UIW_MESSAGE_WINDOW::~UIW_MESSAGE_WINDOW(void)
{
}

EVENT_TYPE UIW_MESSAGE_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_STORE:
		// Let the message editor handle the store request.
		_serviceManager->Get(ID_MESSAGE_EDITOR)->Event(OPT_RESOURCE_STORE);
		break;

	case OPT_STORE_AS:
		// Let the message editor handle the store-as request.
		_serviceManager->Get(ID_MESSAGE_EDITOR)->Event(OPT_RESOURCE_STOREAS);
		break;

	case OPT_CLOSE:
		// Close the window.
		eventManager->Put(S_CLOSE);
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case OPT_EDIT:
		// Edit the current message by sending the L_SELECT command.
		if (list->Current())
			list->Current()->Event(L_SELECT);
		break;

	case OPT_ADD:
		// Add an item to the list and message table.
		{
		// Check for a full message table.
		if (messageList.noOfElements % BUFFER_SIZE == 0)
		{
			// Reallocate the message table and reset the list pointers.
			ZIL_LANGUAGE_ELEMENT *message = messageList.data;
			messageList.data = new ZIL_LANGUAGE_ELEMENT[messageList.noOfElements + BUFFER_SIZE];
			int i;
			for (i = 0; i < messageList.noOfElements; i++)
			{
				// Null out the record.
				messageList.data[i].text = ZIL_NULLP(ZIL_ICHAR);
				messageList.data[i].numberID = 0;
				messageList.data[i].stringID[0] = '\0';
			}

			UI_WINDOW_OBJECT *object = list->First();
			for (i = 0; i < messageList.noOfElements && object; i++)
			{
				messageList.data[i].SwapData(message[i]);
				object->Information(I_SET_MESSAGE, &messageList.data[i]);
				object = object->Next();
			}
			if (message)
				delete message;
		}
		// Set the message table information.
		ZIL_LANGUAGE_ELEMENT *message = &messageList.data[messageList.noOfElements];
		messageList.noOfElements++;

		// Add the new message.
		ZIL_ICHAR stringID[32];
		static ZIL_ICHAR _format[] = { 'Z','M','S','G','_','%','d',0 };
		message->numberID = 0;
		sprintf(stringID, _format, message->numberID);
		strcpy(message->stringID, stringID);
		message->text = strdup(_serviceManager->ZMSG_undefined());
		*list + new UIW_MESSAGE_ITEM(message);

		// Redisplay the list of messages.
		list->Event(S_REDISPLAY);
		}
		break;

	case OPT_DELETE:
		// Delete an item from the list and message table.
		if (list->Current())
		{
			// Move the message item and reset the list pointers.
			UI_WINDOW_OBJECT *cObject = list->Current();
			UI_WINDOW_OBJECT *nObject = cObject->Next();
			while (nObject)
			{
				ZIL_LANGUAGE_ELEMENT *cMessage, *nMessage;
				cObject->Information(I_GET_MESSAGE, &cMessage);
				nObject->Information(I_GET_MESSAGE, &nMessage);
				cMessage->SwapData(*nMessage);
				cObject = nObject;
				nObject = cObject->Next();
			}

			// Clean up the deleted message and message list.
			int i = --messageList.noOfElements;
			if (messageList.data[i].text)
				delete messageList.data[i].text;
			*list - list->Last();
			if (!list->Current())
				*list + list->Last(); // Last() was Current().

			// Redisplay the list of messages.
			list->Event(S_REDISPLAY);
			UI_EVENT scrollEvent(S_VSCROLL_CHECK);
			list->ScrollEvent(scrollEvent);
		}
		break;

	case OPT_MOVE_UP:
		// Move the current list item up in the list.
		if (list->Current() && list->Current() != list->First())
		{
			// Move the message on the visible message list.
			UI_WINDOW_OBJECT *cObject = list->Current();
			UI_WINDOW_OBJECT *pObject = cObject->Previous();
			list->UI_LIST::Subtract(cObject);
			list->UI_LIST::Add(pObject, cObject);
			list->Add(cObject);

			// Move the message in internal message table.
			ZIL_LANGUAGE_ELEMENT *cMessage, *pMessage;
			cObject->Information(I_GET_MESSAGE, &cMessage);
			pObject->Information(I_GET_MESSAGE, &pMessage);
			cMessage->SwapData(*pMessage);
			cObject->Information(I_SET_MESSAGE, pMessage);
			pObject->Information(I_SET_MESSAGE, cMessage);

			// Redisplay the list to reflect the moved items.
			list->Event(S_REDISPLAY);
			UI_EVENT scrollEvent(S_VSCROLL_CHECK);
			list->ScrollEvent(scrollEvent);
		}
		break;

	case OPT_MOVE_DOWN:
		// Move the current list item down in the list.
		if (list->Current() && list->Current() != list->Last())
		{
			// Move the message on the visible message list.
			UI_WINDOW_OBJECT *cObject = list->Current();
			UI_WINDOW_OBJECT *nObject = cObject->Next()->Next();
			list->UI_LIST::Subtract(cObject);
			list->UI_LIST::Add(nObject, cObject);
			list->Add(cObject);
			UI_WINDOW_OBJECT *pObject = cObject->Previous();

			// Move the message in internal message table.
			ZIL_LANGUAGE_ELEMENT *cMessage, *pMessage;
			cObject->Information(I_GET_MESSAGE, &cMessage);
			pObject->Information(I_GET_MESSAGE, &pMessage);
			cMessage->SwapData(*pMessage);
			cObject->Information(I_SET_MESSAGE, pMessage);
			pObject->Information(I_SET_MESSAGE, cMessage);

			// Redisplay the list to reflect the moved items.
			list->Event(S_REDISPLAY);
			UI_EVENT scrollEvent(S_VSCROLL_CHECK);
			list->ScrollEvent(scrollEvent);
		}
		break;

	case S_DEINITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (!ZAF_SERVICE_MANAGER::_queuedEvent)
			_serviceManager->Get(ID_MESSAGE_EDITOR)->Information(I_STATUS_CLEAR, ZIL_NULLP(void));
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(ZAF_MESSAGE_EDITOR::_storage, FALSE);
		_serviceManager->Get(ID_MESSAGE_EDITOR)->Information(I_STATUS_UPDATE, this);
		break;

	default:
		// Default to the base UIW_WINDOW class.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_MESSAGE_WINDOW::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
	ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *, UI_ITEM *)
{
	// Set the list information.
	ZIL_ICHAR *title;
	Information(I_GET_TEXT, &title);
	messageList.Load(name ? name : title, file, object);

	// Fix-up the table to buffer extra messages, then construct the messages.
	ZIL_LANGUAGE_ELEMENT *message = messageList.data;
	messageList.data = new ZIL_LANGUAGE_ELEMENT[messageList.noOfElements + BUFFER_SIZE];
	for (int i = 0; i < messageList.noOfElements; i++)
	{
		// Null out the record.
		messageList.data[i].text = ZIL_NULLP(ZIL_ICHAR);
		messageList.data[i].numberID = 0;
		messageList.data[i].stringID[0] = '\0';
		// Swap with the valid data.
		messageList.data[i].SwapData(message[i]);
		*list + new UIW_MESSAGE_ITEM(&messageList.data[i]);
	}
	if (message)
		delete message;
}

void UIW_MESSAGE_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
	ZIL_STORAGE_OBJECT *object, UI_ITEM *, UI_ITEM *)
{
	// Check for an invalid file name.
	if (name)
		Information(I_SET_TEXT, StringID(name));
	else
		name = StringID();

	// Move to the message directory.
	if (file->ChDir(_messageDirectory))
	{
		file->MkDir(_messageDirectory);
		file->ChDir(_messageDirectory);
	}

	// Store the message table.
	messageList.Store(name, file, object);

	// Store the HPP entries.
	if (file->ChDir(_hppDirectory))
	{
		file->MkDir(_hppDirectory);
		file->ChDir(_hppDirectory);
	}
	ZIL_STORAGE_OBJECT hppEntry(*file, name, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
	for (int i = 0; i < messageList.noOfElements; i++)
		if (messageList.data[i].stringID[0])
		{
			ZIL_ICHAR line[256];
			sprintf(line, _msgFormat, messageList.data[i].stringID, messageList.data[i].numberID);
			hppEntry.Store(line);	// End of HPP string entries.
		}
	hppEntry.Store(_blankString);	// End of HPP string entries.
	file->ChDir(_messageDirectory);
}
