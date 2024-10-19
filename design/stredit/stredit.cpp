//	Zinc Interface Library - STREDIT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_RAW_KEYS
#include "stredit.hpp"
#define USE_ZAF_STRING_EDITOR
#include "p_string.hpp"

const INFO_REQUEST I_SHOW_WINDOW		= 0x0300;
const INFO_REQUEST I_GET_INDEX			= 0x0301;
const INFO_REQUEST I_SET_INDEX			= 0x0302;

const EVENT_TYPE OPT_PUT_KEY			= 7000;

class ZIL_EXPORT_CLASS UIW_EDIT_STRING : public UIW_STRING
{
public:
	UIW_EDIT_STRING(int left, int top, int width);
	EVENT_TYPE Event(const UI_EVENT &event);
	void HexToChar(ZIL_ICHAR *text);
};

class ZIL_EXPORT_CLASS UIW_HEX_INTEGER : public UIW_STRING
{
public:
	UIW_HEX_INTEGER(int left, int top, int width);
	EVENT_TYPE Event(const UI_EVENT &event);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
};

class ZIL_EXPORT_CLASS UIW_FONT_WINDOW : public UI_WINDOW_OBJECT
{
public:
	UIW_FONT_WINDOW(int left, int top, int width, int height);
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE Event(const UI_EVENT &event);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

private:
	int cellWidth, cellHeight;
	UI_REGION active;
};

// --------------------------------------------------------------------------
// ----- STRING_DRIVER ------------------------------------------------------
// --------------------------------------------------------------------------
// This device allows the user to invoke the string editor window anytime
// during the application by pressing the <F12> key.  The string driver
// device watches the event queue to see if the <F12> key is pressed, and if
// so, removes the key from the queue and invokes the string editor.

class STRING_DRIVER : public UI_DEVICE
{
public:
	UIW_WINDOW *stringEditor;
	STRING_DRIVER(UIW_WINDOW *stringEditor);
	EVENT_TYPE Event(const UI_EVENT &event);
	void Poll(void);
};

STRING_DRIVER::STRING_DRIVER(UIW_WINDOW *_stringEditor) :
	UI_DEVICE(E_STRING_DRIVER, D_ON)
{
	stringEditor = _stringEditor;
}

EVENT_TYPE STRING_DRIVER::Event(const UI_EVENT &event)
{
	// Switch on the type.
	if (event.type == D_OFF || event.type == D_ON)
		state = event.type;
	// Return the keyboard state.
	return (state);
}

void STRING_DRIVER::Poll(void)
{
	// Check for an active string driver.
	if (state != D_ON || ZAF_SERVICE_MANAGER::_testMode)
		return;

	// Provide the mapping for the <F12> key.
	static UI_EVENT_MAP ZIL_FARDATA _eventTable[] =
	{
#	if defined(ZIL_MSDOS)
	{ ID_WINDOW,	OPT_INVOKE_STREDIT,	E_KEY, 		F12,		0 },
#	elif defined(ZIL_MSWINDOWS)
	{ ID_WINDOW,	OPT_INVOKE_STREDIT,	WM_KEYUP, 	F12,		0 },
#	elif defined(ZIL_OS2)
	{ ID_WINDOW,	OPT_INVOKE_STREDIT,	WM_CHAR, 	F12,		0 },
#	elif defined(ZIL_MOTIF)
	{ ID_WINDOW,	OPT_INVOKE_STREDIT,	KeyPress, 	XK_F12,		0 },
#	elif defined(ZIL_CURSES)
#	if defined(ZIL_PC_KEYBRD)
	{ ID_WINDOW,	OPT_INVOKE_STREDIT,	E_KEY, 		F12,		0 },
#	else
	{ ID_WINDOW,	OPT_INVOKE_STREDIT,	E_KEY, 		F12,		0 },
#	endif
#	elif defined(ZIL_MACINTOSH)
	{ ID_WINDOW,	OPT_INVOKE_STREDIT,	keyDown, 	F12,		0 },
#	endif
	{ ID_END, 0, 0, 0 }	// End of array.
	};

	// See if the string editor should be invoked.
	UI_EVENT event;
	EVENT_TYPE ccode = eventManager->Get(event, Q_NO_BLOCK | Q_NO_DESTROY | Q_NO_POLL);
	if (ccode != -1)
	{
		// Check for the matching key and active (non-test mode) string editor.
		ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, ID_WINDOW);
		if (ccode != OPT_INVOKE_STREDIT ||
			!_serviceManager->Get(ID_STRING_EDITOR))
			return;
		eventManager->Get(event, Q_NO_BLOCK | Q_NO_POLL); // remove the event.

		// Try to find an edit window.
		UI_WINDOW_OBJECT *requestor = stringEditor->windowManager->First();
		if (!requestor || requestor == stringEditor)
		{
			UI_ERROR_STUB::Beep();
			return;
		}

		// Traverse down edit window to find a requestor.
		while (requestor)
		{
			UI_WINDOW_OBJECT *child = ZIL_NULLP(UI_WINDOW_OBJECT);
			requestor->Information(I_GET_CURRENT, &child);
			if (!child)
				break;
			requestor = child;
		}

		// Set the requestor then invoke the string editor.
		if (FlagSet(requestor->woFlags, WOS_EDIT_MODE) ||
			(!FlagSet(requestor->woFlags, WOF_VIEW_ONLY) &&
			 (requestor->Inherited(ID_STRING) ||
			  requestor->Inherited(ID_TEXT))))
		{
			stringEditor->Information(I_SET_REQUESTOR, requestor);
			stringEditor->Information(I_ACTIVATE_SERVICE, ZIL_NULLP(void));
		}
		else
			UI_ERROR_STUB::Beep();
	}
}

// --------------------------------------------------------------------------
// ----- ZAF_STRING_EDITOR --------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__DECCXX)
ZAF_STRING_EDITOR::ZAF_STRING_EDITOR() : UIW_WINDOW(0, 0, 0, 0) {}
#endif

ZAF_STRING_EDITOR::ZAF_STRING_EDITOR(WOAF_FLAGS _woAdvancedFlags,
	WOS_STATUS _woStatus) :
	// base initialization
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_string"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable),
	// member initialization
	page(0x00), requestor(ZIL_NULLP(UI_WINDOW_OBJECT)), response(0)
{
	// Add the edit device (to invoke the editor);
	woStatus |= _woStatus;
	woAdvancedFlags |= _woAdvancedFlags;
	*eventManager + new STRING_DRIVER(this);

	// This const allows/disallows paging in the font editor.
#	if defined(ZIL_UNICODE)
	const WOF_FLAGS woFlags = WOF_NO_FLAGS;
#	else
	const WOF_FLAGS woFlags = WOF_NON_SELECTABLE;
#	endif

	// Set the window fields.
	searchID = ID_STRING_EDITOR;
	*this
		+ (stringField = new UIW_EDIT_STRING(11, 1, 55))
		+ (hexField = new UIW_SPIN_CONTROL(11, 2, 10, new UIW_HEX_INTEGER(11, 2, 10), 1, WNF_NO_FLAGS, woFlags))
		+ (fontField = new UIW_FONT_WINDOW(1, 3, 65, 11));

	// Center the window on the screen.
	windowManager->Center(this);
	originalField = Get(FIELD_ORIGINAL);
	Add(stringField);
}

ZAF_STRING_EDITOR::~ZAF_STRING_EDITOR(void)
{
	// Remove the storage.
	if (_storage)
		delete _storage;
}

EVENT_TYPE ZAF_STRING_EDITOR::Event(const UI_EVENT &event)
{
	// Check for non editor-events.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPT_OK:
		if (requestor)
		{
			// Set the requestor text data and mark field as changed.
			requestor->Information(I_SET_TEXT, stringField->DataGet());
			requestor->woStatus |= WOS_CHANGED;
			if (response)
			{
				UI_EVENT rEvent(-response);
				requestor->Event(rEvent); // NULL data response tells requestor we're done.
			}
		}
		// Continue to OPT_CANCEL.
	case OPT_CANCEL:
		if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
		{
			// Minimize the window.
			UI_EVENT minimizeEvent(L_MINIMIZE);
			minimizeEvent.windowObject = this;
			Event(minimizeEvent);
		}
		else
			eventManager->Put(S_CLOSE); // Close the window.
		// Make sure the string editor does not remain current.
		if (windowManager->First() == this && Next())
			*windowManager + Next();
		requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
		response = 0;
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case I_INCREMENT_VALUE:
	case I_DECREMENT_VALUE:
		Information((INFO_REQUEST)ccode, ZIL_NULLP(void));
		break;

	case S_CURRENT:
		if (!requestor)
		{
			// Try to find an edit window.
			requestor = (windowManager->First() == this) ? Next() : windowManager->First();
			while (requestor)
				if (FlagSet(requestor->woStatus, WOS_EDIT_MODE))
					break;
				else
					requestor = requestor->Next();

			// Traverse down edit window to find a requestor.
			while (requestor)
			{
				UI_WINDOW_OBJECT *child = ZIL_NULLP(UI_WINDOW_OBJECT);
				requestor->Information(I_GET_CURRENT, &child);
				if (!child)
					break;
				requestor = child;
			}

			// Reset the string information for the request object.
			if (requestor)
			{
				ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
				requestor->Information(I_GET_TEXT, &text);
				originalField->Information(I_SET_TEXT, text);
				stringField->Information(I_SET_TEXT, text);
			}
		}
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(_storage, FALSE);
		break;

	case S_CREATE:
		if (requestor)
		{
			// reset the string information for the request object.
			ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
			requestor->Information(I_GET_TEXT, &text);
			originalField->Information(I_SET_TEXT, text);
			stringField->Information(I_SET_TEXT, text);
		}
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_NON_CURRENT:
		// reset the requestor.
		ccode = UIW_WINDOW::Event(event);
		requestor = ZIL_NULLP(UI_WINDOW_OBJECT);
		response = 0;
		break;

	case OPT_PUT_KEY:
		// Make sure the string field is current.
		if (current != stringField)
			Add(stringField);
		stringField->Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	return (ccode);
}

void *ZAF_STRING_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the storage service.
		*windowManager + this;
		if (FlagSet(woStatus, WOS_MINIMIZED))
		{
			// Restore the minimized window.
			UI_EVENT restoreEvent(L_RESTORE);
			restoreEvent.windowObject = this;
			Event(restoreEvent);
		}
		break;

	case I_SET_REQUESTOR:
		requestor = (UI_WINDOW_OBJECT *)data;
		if (requestor)
		{
			ZIL_ICHAR *text = ZIL_NULLP(ZIL_ICHAR);
			requestor->Information(I_GET_TEXT, &text);
			originalField->Information(I_SET_TEXT, text);
			stringField->Information(I_SET_TEXT, text);
		}
		else if (windowManager->First() != this)
		{
			UI_WINDOW_OBJECT *child = windowManager->First();
			while (child)
			{
				requestor = child;
				child = ZIL_NULLP(UI_WINDOW_OBJECT);
				requestor->Information(I_GET_CURRENT, &child);
			}
		}
		Add(stringField); // Make sure the string field is current.
		break;

	case I_SET_REQUEST:
		// Set the information request.
		response = *(EVENT_TYPE *)data;
		break;

	// ----- String editor specific requests -----
	case I_GET_INDEX:
		*(int *)data = page;
		break;

	case I_SET_INDEX:
	case I_INCREMENT_VALUE:
	case I_DECREMENT_VALUE:
		// The index can only be changed for unicode character sets.
		if (request == I_SET_INDEX && page != *(int *)data)
			page = *(int *)data;
		else if (request == I_DECREMENT_VALUE && page > 0x00)
			page--;
		else if (request == I_INCREMENT_VALUE && page < 0xFF)
			page++;
		else
			break;
		fontField->Event(S_REDISPLAY);
		break;

	// ----- Base class requests -----
	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// --------------------------------------------------------------------------
// ----- UIW_HEX_INTEGER ----------------------------------------------------
// --------------------------------------------------------------------------

static ZIL_ICHAR _xFormat[] = { '%','x', 0 };

UIW_HEX_INTEGER::UIW_HEX_INTEGER(int left, int top, int width) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), 1024)
{
	sprintf(text, _xFormat, 0);
}

EVENT_TYPE UIW_HEX_INTEGER::Event(const UI_EVENT &event)
{
	// Check for Zinc specific messages.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STRING);
	switch (ccode)
	{
	case L_SELECT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			UIW_STRING::DataGet();
			int page = (ZIL_ICHAR)strtol(text, ZIL_NULLP(ZIL_ICHAR *), 0x10);
			Root()->Information(I_SET_INDEX, &page);
			ccode = UIW_STRING::Event(event);
			sprintf(text, _xFormat, page);
			UIW_STRING::DataSet(text);
			eventManager->Put(L_PREVIOUS); // Move to the string field.
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			woStatus |= WOS_INTERNAL_ACTION;
			UIW_STRING::DataGet();
			int page; Root()->Information(I_GET_INDEX, &page);
			sprintf(text, _xFormat, page);
			UIW_STRING::DataSet(text);
			woStatus &= ~WOS_INTERNAL_ACTION;
		}
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_HEX_INTEGER::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INCREMENT_VALUE:
	case I_DECREMENT_VALUE:
		{
		// Page count is kept by the root object.
		Root()->Information(request, data, objectID);
		int page; Root()->Information(I_GET_INDEX, &page);
		// Reformat the page information.
		sprintf(text, _xFormat, page);
		UIW_STRING::DataSet(text);
		}
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// --------------------------------------------------------------------------
// ----- UIW_EDIT_STRING ----------------------------------------------------
// --------------------------------------------------------------------------

UIW_EDIT_STRING::UIW_EDIT_STRING(int left, int top, int width) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), 100, STF_NO_FLAGS, WOF_BORDER)
{
}

EVENT_TYPE UIW_EDIT_STRING::Event(const UI_EVENT &event)
{
	// Check for Zinc specific messages.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STRING);
	switch (ccode)
	{
	case L_SELECT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			UIW_STRING::DataGet();
			HexToChar(text);
			ccode = UIW_STRING::Event(event);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case OPT_PUT_KEY:
		// Process mouse selected characters.
		{
		UI_EVENT keyEvent = event;
		keyEvent.type = E_KEY;
		ccode = UIW_STRING::Event(keyEvent);
		}
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_EDIT_STRING::HexToChar(ZIL_ICHAR *text)
{
	const int HEX_RADIX = 0x10;
	static const ZIL_ICHAR HEX_INDEX[] = { '0','x', 0 };
	const int MAX_HEX_LENGTH = 6;

	ZIL_ICHAR sValue[MAX_HEX_LENGTH+1];
	ZIL_ICHAR *start = strstr(text, HEX_INDEX);
	while (start)
	{
		strncpy(sValue, start, MAX_HEX_LENGTH);
		int i;
		for (i = 2; i < MAX_HEX_LENGTH; i++)
			if (!IsXDigit(sValue[i]))
				break;
		sValue[i] = '\0';
		*start = (ZIL_ICHAR)strtoul(sValue, ZIL_NULLP(ZIL_ICHAR *), HEX_RADIX);
		strcpy(start + 1, start + strlen(sValue));
		start = strstr(start, HEX_INDEX);
	}
}

// --------------------------------------------------------------------------
// ----- UIW_FONT_WINDOW ----------------------------------------------------
// --------------------------------------------------------------------------

UIW_FONT_WINDOW::UIW_FONT_WINDOW(int left, int top, int width, int height) :
	UI_WINDOW_OBJECT(left, top, width, height, WOF_NO_FLAGS, WOAF_NO_FLAGS)
{
	woFlags |= WOF_BORDER;
	woStatus |= WOS_OWNERDRAW;
	woAdvancedFlags |= WOAF_NON_CURRENT;
}

EVENT_TYPE UIW_FONT_WINDOW::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	static ZIL_ICHAR _text[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	static ZIL_ICHAR _testText[2] = { '0',0 };
	int i, j;
	UI_REGION region = trueRegion;
	display->VirtualGet(screenID, region);
	lastPalette = &display->colorMap[(int)WHITE];
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);

	UI_PALETTE *darkLine = &display->colorMap[(int)BLACK];
	UI_PALETTE *lightLine = &display->colorMap[(int)LIGHTGRAY];

	if (!display->isText)
	{
		int column = region.left + display->cellHeight + 1;
		int line = region.top + display->cellHeight + 1;
		int _textXOffset = cellWidth / 2 - display->cellWidth;
		int _textYOffset = (display->cellHeight - display->TextHeight(_testText)) / 2;
		for (i = 0; i < 16; i++, column += cellWidth, line += cellHeight)
		{
			display->Text(screenID, column + _textXOffset, region.top + _textYOffset, &_text[i], lightLine, 1,
				FALSE, FALSE, ZIL_NULLP(UI_REGION), FNT_DIALOG_FONT | FNT_IGNORE_UNDERSCORE);
			display->Text(screenID, region.left + _textXOffset, line, &_text[i], lightLine, 1,
				FALSE, FALSE, ZIL_NULLP(UI_REGION), FNT_DIALOG_FONT | FNT_IGNORE_UNDERSCORE);
		}
		region.left += display->cellHeight, region.top += display->cellHeight;

		display->Rectangle(screenID, region, darkLine, 1); --region;
		active = region;

		_textYOffset = 0;
		int page; Root()->Information(I_GET_INDEX, &page);
		for (i = 0, line = region.top + cellHeight - 1; i < 15; i++, line += cellHeight)
 			display->Line(screenID, region.left, line, region.right, line, lightLine, 1);
		for (i = 0, line = region.top + cellHeight - 1; i < 16; i++, line += cellHeight)
		{
			ZIL_ICHAR value = (page << 8) + i;
			line -= cellHeight - 1;
			for (j = 0, column = region.left; j < 16; j++, column += cellWidth, value += 0x10)
				display->Text(screenID, column + _textXOffset, line + _textYOffset, &value, darkLine, 1,
					FALSE, FALSE, ZIL_NULLP(UI_REGION), FNT_DIALOG_FONT | FNT_IGNORE_UNDERSCORE);
			line += cellHeight - 1;
		}
		for (j = 0, column = region.left + cellWidth - 1; j < 15; j++, column += cellWidth)
			display->Line(screenID, column, region.top, column, region.bottom, lightLine, 1);
	}
	else // text-mode
	{
		// Draw the text-mode information.
		int column = region.left + 1;
		int line = region.top + 1;
		int _textXOffset = cellWidth / 2 - 1;
		int page; Root()->Information(I_GET_INDEX, &page);
		for (i = 0; i < 16; i++, column += cellWidth, line += cellHeight)
		{
			display->Text(screenID, column + 1, region.top, &_text[i], lightLine, 1, FALSE);
			display->Text(screenID, region.left, line, &_text[i], lightLine, 1, FALSE);
		}
		for (i = 0, line = region.top + 1; i < 16; i++, line++)
		{
			ZIL_ICHAR value = (page << 8) + i;
			for (j = 0, column = region.left + 1; j < 16; j++, column += cellWidth, value += 0x10)
				display->Text(screenID, column + _textXOffset, line, &value, darkLine, 1,
					FALSE, FALSE, ZIL_NULLP(UI_REGION), FNT_DIALOG_FONT | FNT_IGNORE_UNDERSCORE);
		}

		active = region;
		active.left++;
		active.top++;
	}

	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_FONT_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);
	if (event.InputType() == E_MOUSE && event.rawCode == M_LEFT_CHANGE)
		ccode = L_END_SELECT;
	switch (ccode)
	{
	case S_CREATE:
	case S_CHANGED:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (!display->isText)
		{
			cellHeight = (relative.Height() - display->cellHeight) / 16;
			cellWidth = (relative.Width() - display->cellHeight) / 16;
		}
		else
		{
			cellHeight = 1;
			cellWidth = (relative.Width() - display->cellHeight) / 16;
		}
		trueRegion.right = trueRegion.left + display->cellHeight + cellWidth * 16;
		trueRegion.bottom = trueRegion.top + display->cellHeight + cellHeight * 16;
		if (ccode == S_CREATE)
			UI_WINDOW_OBJECT::Event(S_REGISTER_OBJECT);
		break;

	case L_BEGIN_SELECT:
		{
		UI_WINDOW_OBJECT::Event(event);
		UI_POSITION position = event.position;
		NormalizePosition(this, event, position);
		if (active.Overlap(position))
		{
			int column = (position.column - active.left) / cellWidth;
			int line = (position.line - active.top) / cellHeight;
			if (column > 0x0F || line > 0x0F)
				break;
			int page; Root()->Information(I_GET_INDEX, &page);
			UI_EVENT keyEvent(OPT_PUT_KEY);
			keyEvent.rawCode = (page << 8) + (column << 4) + line;
			keyEvent.key.value = keyEvent.rawCode;
			keyEvent.key.shiftState = 0;
			eventManager->Put(keyEvent);
		}
		}
		break;

	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_FONT_WINDOW::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- Static tables ------------------------------------------------------

ZIL_ICHAR ZAF_STRING_EDITOR::_className[] = { 'Z','A','F','_','S','T','R','I','N','G','_','E','D','I','T','O','R',0 };

ZIL_STORAGE_READ_ONLY *ZAF_STRING_EDITOR::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_STRING_EDITOR::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_ICON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_ICON::New), UIW_ICON::_className, 0 },
	{ ID_MAXIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MAXIMIZE_BUTTON::New), UIW_MAXIMIZE_BUTTON::_className, 0 },
	{ ID_MINIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MINIMIZE_BUTTON::New), UIW_MINIMIZE_BUTTON::_className, 0 },
	{ ID_PROMPT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PROMPT::New), UIW_PROMPT::_className, 0 },
	{ ID_STRING, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_STRING_EDITOR::_objectTable = __objectTable;
