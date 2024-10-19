//	Zinc Interface Library - SERVICE.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_SERVICE_HPP)
#	define ZAF_SERVICE_HPP
#	if !defined(UI_WIN_HPP)
#		include <ui_win.hpp>
#	endif

// --- Check for valid compile options --------------------------------------

#if !defined(ZIL_EDIT) || !defined(ZIL_LOAD) || !defined(ZIL_STORE)
???? Zinc Designer must be compiled with the ZIL_EDIT, ZIL_LOAD and ZIL_STORE options set in ui_env.hpp.
#endif

// --------------------------------------------------------------------------
// ----- Definition of common services available from Zinc ------------------
// --------------------------------------------------------------------------

// ----- 6000-6099: General Options -----
const INFO_REQUEST I_GET_CLASSNAME_SERVICE	= 6000;
const INFO_REQUEST I_GET_OBJECTID_SERVICE	= 6001;
const INFO_REQUEST I_SET_REQUESTOR 			= 6002;
const INFO_REQUEST I_SET_REQUEST 			= 6003;
const INFO_REQUEST I_ACTIVATE_SERVICE		= 6004;
const INFO_REQUEST I_STATUS_UPDATE			= 6005;
const INFO_REQUEST I_STATUS_CLEAR			= 6006;
const INFO_REQUEST I_GET_ORIGINAL_TITLE		= 6007;

const EVENT_TYPE OPT_OK 					= 6000;
const EVENT_TYPE OPT_CANCEL 				= 6001;
const EVENT_TYPE OPT_HELP 					= 6002;
const EVENT_TYPE OPT_STORE 					= 6003;
const EVENT_TYPE OPT_STORE_AS				= 6004;
const EVENT_TYPE OPT_CLOSE					= 6005;
const EVENT_TYPE OPT_INVOKE_STREDIT			= 6006;

const EVENT_TYPE OPT_EDIT					= 6010;
const EVENT_TYPE OPT_ADD					= 6011;
const EVENT_TYPE OPT_DELETE					= 6012;
const EVENT_TYPE OPT_MOVE_UP  				= 6013;
const EVENT_TYPE OPT_MOVE_DOWN				= 6014;

const EVENT_TYPE OPT_GET_SERVICE			= 6020;
const EVENT_TYPE OPT_ACTIVATE_SERVICE		= 6021;

// ----- 6100-6199: File Options -----
const INFO_REQUEST I_SET_FILTER				= 6100;

const EVENT_TYPE OPT_FILE_NEW				= 6100;
const EVENT_TYPE OPT_FILE_OPEN				= 6101;
const EVENT_TYPE OPT_FILE_SAVE				= 6102;
const EVENT_TYPE OPT_FILE_SAVEAS			= 6103;
const EVENT_TYPE OPT_FILE_CLOSE				= 6104;
const EVENT_TYPE OPT_FILE_DELETE			= 6105;
const EVENT_TYPE OPT_FILE_PREFERENCES		= 6106;
const EVENT_TYPE OPT_FILE_IMPORT			= 6107;
const EVENT_TYPE OPT_FILE_EXPORT			= 6108;

// ----- 6200-6299: Edit Options -----
const EVENT_TYPE OPT_EDIT_OBJECT			= 6200;
const EVENT_TYPE OPT_EDIT_CUT	 			= 6201;
const EVENT_TYPE OPT_EDIT_COPY	 			= 6202;
const EVENT_TYPE OPT_EDIT_PASTE	 			= 6203;
const EVENT_TYPE OPT_EDIT_DELETE 			= 6204;
const EVENT_TYPE OPT_EDIT_MOVE				= 6205;
const EVENT_TYPE OPT_EDIT_SIZE				= 6206;
const EVENT_TYPE OPT_EDIT_GROUP_DEFINE		= 6207;
const EVENT_TYPE OPT_EDIT_GROUP_UNDEFINE	= 6208;

// ----- 6300-6399: Storage Options -----
const INFO_REQUEST I_GET_STORAGE			= 6300; // ZIL_STORAGE **
const INFO_REQUEST I_SET_STORAGE			= 6301; // ZIL_STORAGE *
const INFO_REQUEST I_GET_REVISIONS			= 6302; // int *
const INFO_REQUEST I_SET_REVISIONS			= 6303; // int *
const INFO_REQUEST I_GET_WRITE_HPP			= 6304; // int *
const INFO_REQUEST I_SET_WRITE_HPP			= 6305; // int *
const INFO_REQUEST I_GENERATE_HPP			= 6306; // void
const INFO_REQUEST I_GET_WRITE_CPP			= 6307; // int *
const INFO_REQUEST I_SET_WRITE_CPP			= 6308; // int *
const INFO_REQUEST I_GENERATE_CPP			= 6309; // void
const INFO_REQUEST I_SET_TRAVERSE_LOCK		= 6310; // int *
const INFO_REQUEST I_GET_DEFAULT_EXTENSION	= 6311; // ZIL_ICHAR **
const INFO_REQUEST I_SET_DEFAULT_EXTENSION	= 6312; // ZIL_ICHAR *

const EVENT_TYPE OPT_RESOURCE_CREATE		= 6300;
const EVENT_TYPE OPT_RESOURCE_LOAD			= 6301;
const EVENT_TYPE OPT_RESOURCE_STORE			= 6302;
const EVENT_TYPE OPT_RESOURCE_STOREAS		= 6303;
const EVENT_TYPE OPT_RESOURCE_DELETE		= 6304;
const EVENT_TYPE OPT_RESOURCE_IMPORT		= 6305;
const EVENT_TYPE OPT_RESOURCE_TEST			= 6306;
const EVENT_TYPE OPT_RESOURCE_CLEAR			= 6307;
const EVENT_TYPE OPT_RESOURCE_CLEARALL		= 6308;
const EVENT_TYPE OPT_RESOURCE_EDIT			= 6309;
const EVENT_TYPE OPT_RESOURCE_EXPORT		= 6310;

// ----- Pre-defined Service Identifications -----

const ZIL_OBJECTID ID_SERVICE_MANAGER		= 3000;

const ZIL_OBJECTID ID_STORAGE_SERVICE		= 3100;
const ZIL_OBJECTID ID_DIRECTORY_SERVICE		= 3101;

const ZIL_OBJECTID ID_WINDOW_EDITOR			= 3200;
const ZIL_OBJECTID ID_WINDOW_WINDOW			= 3201;
const ZIL_OBJECTID ID_OBJECT_EDITOR			= 3202;
const ZIL_OBJECTID ID_IMAGE_EDITOR			= 3205;
const ZIL_OBJECTID ID_IMAGE_WINDOW			= 3206;
const ZIL_OBJECTID ID_I18N_EDITOR			= 3210;
const ZIL_OBJECTID ID_I18N_WINDOW			= 3211;
const ZIL_OBJECTID ID_DATABASE_EDITOR		= 3215;
const ZIL_OBJECTID ID_DATABASE_WINDOW		= 3216;
const ZIL_OBJECTID ID_FILE_EDITOR			= 3220;
const ZIL_OBJECTID ID_FILE_WINDOW			= 3221;
const ZIL_OBJECTID ID_HELP_EDITOR			= 3225;
const ZIL_OBJECTID ID_HELP_WINDOW			= 3226;
const ZIL_OBJECTID ID_MESSAGE_EDITOR		= 3230;
const ZIL_OBJECTID ID_MESSAGE_WINDOW		= 3231;
const ZIL_OBJECTID ID_COLORMAP_EDITOR		= 3235;
const ZIL_OBJECTID ID_COLORMAP_WINDOW		= 3236;
const ZIL_OBJECTID ID_EVENTMAP_EDITOR		= 3240;
const ZIL_OBJECTID ID_EVENTMAP_WINDOW		= 3241;
const ZIL_OBJECTID ID_STRING_EDITOR			= 3243;

// String driver service
const ZIL_DEVICE_TYPE E_STRING_DRIVER		= 98;

// --------------------------------------------------------------------------
// ----- ZAF_SERVICE_MANAGER ------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZAF_SERVICE_MANAGER : public UIW_WINDOW
{
public:
	// compile-time service members.
	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;
	static UI_EVENT_MAP *_eventTable;
	static int _changedData;
	static EVENT_TYPE _queuedEvent;
	static int _testMode;

	// general services.
	ZAF_SERVICE_MANAGER(void);
	~ZAF_SERVICE_MANAGER(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	EVENT_TYPE Control(void);
	UIW_VT_LIST *CtrlList(UIW_WINDOW *window, ZIL_NUMBERID fieldID);
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	UI_WINDOW_OBJECT *Get(const ZIL_ICHAR *className) { return ((UI_WINDOW_OBJECT *)Information(I_GET_CLASSNAME_SERVICE, (void *)className, ID_DEFAULT)); }
	UI_WINDOW_OBJECT *Get(ZIL_OBJECTID objectID) { return ((UI_WINDOW_OBJECT *)Information(I_GET_OBJECTID_SERVICE, (void *)&objectID, ID_DEFAULT)); }
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	void StatusUpdate(ZIL_ICHAR *title = ZIL_NULLP(ZIL_ICHAR), ZIL_ICHAR *format = ZIL_NULLP(ZIL_ICHAR), ...);
	void UpdateToolsMenus(void);

	// general service messages.
	ZIL_ICHAR *ZMSG_allObjects(void);
	ZIL_ICHAR *ZMSG_allFilter(void);
	ZIL_ICHAR *ZMSG_configName(void);
	ZIL_ICHAR *ZMSG_configPath(void);
	ZIL_ICHAR *ZMSG_datFilter(void);
	ZIL_ICHAR *ZMSG_fileOption(void);
	ZIL_ICHAR *ZMSG_mode(void);
	ZIL_ICHAR *ZMSG_msgTableName(void);
	ZIL_ICHAR *ZMSG_newline(void);
	ZIL_ICHAR *ZMSG_none(void);
	ZIL_ICHAR *ZMSG_noFile(void);
	ZIL_ICHAR *ZMSG_tempResourceName(void);
	ZIL_ICHAR *ZMSG_tempFileName(void);
	ZIL_ICHAR *ZMSG_undefined(void);
	ZIL_ICHAR *ZMSG_untitled(void);

	// special environment services.
	static ZIL_STORAGE_READ_ONLY *Storage(ZIL_STORAGE_READ_ONLY **storage, const char *moduleName);

	// list members.
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	UI_WINDOW_OBJECT *Current(void) { return (serviceList.Current()->window); }
	UI_WINDOW_OBJECT *First(void) { return (serviceList.First()->window); }
	UI_WINDOW_OBJECT *Last(void) { return (serviceList.Last()->window); }
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	ZAF_SERVICE_MANAGER &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	ZAF_SERVICE_MANAGER &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// compiler bug prevents following from being private.
	class SERVICE_ELEMENT : public UI_ELEMENT
	{
	public:
		UI_WINDOW_OBJECT *window;
		SERVICE_ELEMENT(UI_WINDOW_OBJECT *_window) { window = _window; }
		~SERVICE_ELEMENT(void) { delete window; }
		SERVICE_ELEMENT *Next(void) { return((ZAF_SERVICE_MANAGER::SERVICE_ELEMENT *)next); }
		SERVICE_ELEMENT *Previous(void) { return((ZAF_SERVICE_MANAGER::SERVICE_ELEMENT *)previous); }
	};

	// compiler bug prevents following from being private.
	class SERVICE_LIST : public UI_LIST
	{
	public:
		SERVICE_ELEMENT *Add(SERVICE_ELEMENT *element) { return ((ZAF_SERVICE_MANAGER::SERVICE_ELEMENT *)UI_LIST::Add(element)); }
		SERVICE_ELEMENT *Current(void) { return((ZAF_SERVICE_MANAGER::SERVICE_ELEMENT *)current); }
		SERVICE_ELEMENT *First(void) { return((ZAF_SERVICE_MANAGER::SERVICE_ELEMENT *)first); }
		SERVICE_ELEMENT *Last(void) { return((ZAF_SERVICE_MANAGER::SERVICE_ELEMENT *)last); }
		SERVICE_ELEMENT *Subtract(SERVICE_ELEMENT *element) { return ((ZAF_SERVICE_MANAGER::SERVICE_ELEMENT *)UI_LIST::Subtract(element)); }
		SERVICE_LIST &operator+(SERVICE_ELEMENT *element) { Add(element); return(*this); }
		SERVICE_LIST &operator-(SERVICE_ELEMENT *element) { Subtract(element); return(*this); }
	};

private:
	static ZIL_ICHAR _allObjects[];
	static ZIL_ICHAR _allFilter[];
	static ZIL_ICHAR _datFilter[];
	static ZIL_ICHAR _fileOption[];
	static ZIL_ICHAR _msgTableName[];
	static ZIL_ICHAR _newline[];
	static ZIL_ICHAR _configName[];
	static ZIL_ICHAR _configPath[];

	ZIL_LANGUAGE msgTable;
	SERVICE_LIST serviceList;

	static EVENT_TYPE Exit(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager);
};

extern ZAF_SERVICE_MANAGER *_serviceManager;

#endif
