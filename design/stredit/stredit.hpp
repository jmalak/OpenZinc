//	Zinc Interface Library - STRING.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_STRING_HPP)
#define ZAF_STRING_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

class ZIL_EXPORT_CLASS ZAF_STRING_EDITOR : public UIW_WINDOW
{
public:
	friend class ZIL_EXPORT_CLASS UIW_HEX_INTEGER;
	friend class ZIL_EXPORT_CLASS UIW_EDIT_STRING;
	friend class ZIL_EXPORT_CLASS UIW_FONT_WINDOW;

	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;

#if defined(__DECCXX)
	// Compiler requires default constructor.
	ZAF_STRING_EDITOR();
#endif
	ZAF_STRING_EDITOR(WOAF_FLAGS woAdvancedFlags, WOS_STATUS woStatus);
	~ZAF_STRING_EDITOR(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

private:
	int page;
	UIW_SPIN_CONTROL *hexField;
	UIW_EDIT_STRING *stringField;
	UI_WINDOW_OBJECT *originalField;
	UIW_FONT_WINDOW *fontField;

	EVENT_TYPE response;
	UI_WINDOW_OBJECT *requestor;
};

#endif

