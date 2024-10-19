//	Zinc Interface Library - Z_HELP.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_UI_HELP_SYSTEM
#include "p_i18n.hpp"

// ----- UI_HELP_SYSTEM -----------------------------------------------------

void ZAF_LANGUAGE_WINDOW::UI_HELP_SYSTEM_DataLoad(void)
{
	// Load the object data.
	SetText(HELPTOPIC, MSG_HELP_TOPIC);
	SetText(HELPINDEX, MSG_HELP_INDEX);
	SetText(SHOWTOPIC, MSG_SHOW_TOPIC);
	SetText(SHOWINDEX, MSG_SHOW_INDEX);
	SetText(SELECTTITLE, MSG_MESSAGE1);
	SetText(SELECTMESSAGE, MSG_MESSAGE2);
	SetText(CLOSE, MSG_CLOSE);
	SetText(ICONTITLE, MSG_ICONTITLE);
	SetText(ICONNAME, MSG_ICONNAME);
}

void ZAF_LANGUAGE_WINDOW::UI_HELP_SYSTEM_DataStore(void)
{
	// Store the object data.
	GetText(HELPTOPIC, MSG_HELP_TOPIC);
	GetText(HELPINDEX, MSG_HELP_INDEX);
	GetText(SHOWTOPIC, MSG_SHOW_TOPIC);
	GetText(SHOWINDEX, MSG_SHOW_INDEX);
	GetText(SELECTTITLE, MSG_MESSAGE1);
	GetText(SELECTMESSAGE, MSG_MESSAGE2);
	GetText(CLOSE, MSG_CLOSE);
	GetText(ICONTITLE, MSG_ICONTITLE);
	GetText(ICONNAME, MSG_ICONNAME);
}

