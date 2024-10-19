//	Zinc Interface Library - LANG_EN.CPP
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


// In error statements, the %1%s is the data, the %2%s is the range.
// NOTE: '&' is the hotkey specifier.

#if defined(ZIL_LANGUAGE_DATA)
#	define langCodeset	CP_ISO8859_1
	// Language definitions for ENGLISH (EN).
	static ZIL_ICHAR ZIL_FARDATA isoLanguageName[] = { 'e','n',0 };
#endif

#if defined(ZIL_UTIME_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA DAY_STRINGS[] =
{
	{ "", 0xffff, { 0 } },
	{ "Sunday",	ZIL_DAY_1, { 0 } },
	{ "Monday",	ZIL_DAY_2, { 0 } },
	{ "Tuesday",	ZIL_DAY_3, { 0 } },
	{ "Wednesday",	ZIL_DAY_4, { 0 } },
	{ "Thursday",	ZIL_DAY_5, { 0 } },
	{ "Friday",	ZIL_DAY_6, { 0 } },
	{ "Saturday",	ZIL_DAY_7, { 0 } },
	{ "Sun",	ZIL_SHDAY_1, { 0 } },
	{ "Mon",	ZIL_SHDAY_2, { 0 } },
	{ "Tue",	ZIL_SHDAY_3, { 0 } },
	{ "Wed",	ZIL_SHDAY_4, { 0 } },
	{ "Thu",	ZIL_SHDAY_5, { 0 } },
	{ "Fri",	ZIL_SHDAY_6, { 0 } },
	{ "Sat",	ZIL_SHDAY_7, { 0 } },
	{ "Sun.",	ZIL_ABDAY_1, { 0 } },
	{ "Mon.",	ZIL_ABDAY_2, { 0 } },
	{ "Tues.",	ZIL_ABDAY_3, { 0 } },
	{ "Wed.",	ZIL_ABDAY_4, { 0 } },
	{ "Thurs.",	ZIL_ABDAY_5, { 0 } },
	{ "Fri.",	ZIL_ABDAY_6, { 0 } },
	{ "Sat.",	ZIL_ABDAY_7, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA MONTH_STRINGS[] =
{
	{ "", 0xffff, { 0 } },
	{ "January",	ZIL_MON_1, { 0 }  },
	{ "February",	ZIL_MON_2, { 0 }  },
	{ "March",	ZIL_MON_3, { 0 }  },
	{ "April",	ZIL_MON_4, { 0 }  },
	{ "May",	ZIL_MON_5, { 0 }  },
	{ "June",	ZIL_MON_6, { 0 }  },
	{ "July",	ZIL_MON_7, { 0 }  },
	{ "August",	ZIL_MON_8, { 0 }  },
	{ "September",	ZIL_MON_9, { 0 }  },
	{ "October",	ZIL_MON_10, { 0 } },
	{ "November",	ZIL_MON_11, { 0 } },
	{ "December",	ZIL_MON_12, { 0 } },
	{ "Jan",	ZIL_SHMON_1, { 0 }  },
	{ "Feb",	ZIL_SHMON_2, { 0 }  },
	{ "Mar",	ZIL_SHMON_3, { 0 }  },
	{ "Apr",	ZIL_SHMON_4, { 0 }  },
	{ "May",	ZIL_SHMON_5, { 0 }  },
	{ "Jun",	ZIL_SHMON_6, { 0 }  },
	{ "Jul",	ZIL_SHMON_7, { 0 }  },
	{ "Aug",	ZIL_SHMON_8, { 0 }  },
	{ "Sep",	ZIL_SHMON_9, { 0 }  },
	{ "Oct",	ZIL_SHMON_10, { 0 } },
	{ "Nov",	ZIL_SHMON_11, { 0 } },
	{ "Dec",	ZIL_SHMON_12, { 0 } },
	{ "Jan.",	ZIL_ABMON_1, { 0 }  },
	{ "Feb.",	ZIL_ABMON_2, { 0 }  },
	{ "Mar.",	ZIL_ABMON_3, { 0 }  },
	{ "Apr.",	ZIL_ABMON_4, { 0 }  },
	{ "May",	ZIL_ABMON_5, { 0 }  },
	{ "June",	ZIL_ABMON_6, { 0 }  },
	{ "July",	ZIL_ABMON_7, { 0 }  },
	{ "Aug.",	ZIL_ABMON_8, { 0 }  },
	{ "Sept.",	ZIL_ABMON_9, { 0 }  },
	{ "Oct.",	ZIL_ABMON_10, { 0 } },
	{ "Nov.",	ZIL_ABMON_11, { 0 } },
	{ "Dec.",	ZIL_ABMON_12, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA TIME_STRINGS[] =
{
	{ "", 0xffff, { 0 } },
	{ "a.m.", ZIL_AM, { 0 } },
	{ "p.m.", ZIL_PM, { 0 } },
	{ "xxxx", ZIL_TZ, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_UIW_BIGNUM_DATA) || defined(ZIL_UIW_INT_DATA) || defined(ZIL_UIW_REAL_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "The number %s is invalid.", NMI_INVALID, { 0 } },
	{ "The number %s is not in the range %s.", NMI_OUT_OF_RANGE, { 0 } },
	{ "The number %s is less than %s.", NMI_LESS_THAN_RANGE, { 0 } },
	{ "The number %s is greater than %s.", NMI_GREATER_THAN_RANGE, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_UIW_DATE_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "The date %s is invalid.", DTI_INVALID, { 0 } },
	{ "The date %s has an ambiguous month name.", DTI_AMBIGUOUS, { 0 } },
	{ "The date %s has an invalid month name.", DTI_INVALID_NAME, { 0 } },
	{ "A date value must be entered.", DTI_VALUE_MISSING, { 0 } },
	{ "The date %s is not in the range %s.", DTI_OUT_OF_RANGE, { 0 } },
	{ "The date %s is before %s.", DTI_LESS_THAN_RANGE, { 0 } },
	{ "The date %s is after %s.", DTI_GREATER_THAN_RANGE, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_UIW_TIME_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "The time %s is invalid.", TMI_INVALID, { 0 } },
	{ "A time value must be entered.", TMI_VALUE_MISSING, { 0 } },
	{ "The time %s is not in the range %s.", TMI_OUT_OF_RANGE, { 0 } },
	{ "The time %s is before %s.", TMI_LESS_THAN_RANGE, { 0 } },
	{ "The time %s is after %s.", TMI_GREATER_THAN_RANGE, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_UI_ERROR_SYSTEM_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "Error", MSG_TITLE, { 0 } },
	{ "&OK", MSG_OK, { 0 } },
	{ "&Cancel", MSG_CANCEL, { 0 } },
	{ "ASTERISK", MSG_ICONNAME, { 0 } },
	{ "&Move", MSG_MOVE, { 0 } },
	{ "&Close\tAlt+F4", MSG_CLOSE, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_UI_HELP_SYSTEM_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "Help Topic", MSG_HELP_TOPIC, { 0 } },
	{ "Help Index", MSG_HELP_INDEX, { 0 } },
	{ "Show Topic", MSG_SHOW_TOPIC, { 0 } },
	{ "Show Index", MSG_SHOW_INDEX, { 0 } },
	{ "Select A Topic", MSG_MESSAGE1, { 0 } },
	{ "Type a word, or select one from the list.", MSG_MESSAGE2, { 0 } },
	{ "Close", MSG_CLOSE, { 0 } },
	{ "Help Information", MSG_ICONTITLE, { 0 } },
	{ "QUESTION", MSG_ICONNAME, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_UIW_SYSTEM_BUTTON_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "&Restore", MNIF_RESTORE, { 0 } },	// S_RESTORE
	{ "&Move", MNIF_MOVE, { 0 } },			// S_MOVE
	{ "&Size", MNIF_SIZE, { 0 } },			// S_SIZE
	{ "Mi&nimize", MNIF_MINIMIZE, { 0 } },	// S_MINIMIZE
	{ "Ma&ximize", MNIF_MAXIMIZE, { 0 } },	// S_MAXIMIZE
	{ "", MNIF_SEPARATOR, { 0 } },
	{ "&Close", MNIF_CLOSE, { 0 } },		// S_CLOSE
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_UIW_WINDOW_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "ERROR", 0xffff, { 0 } },
	{ "Unknown window object.", ZIL_UNK_OBJ, { 0 } },
	{ "Object %s was not found.", ZIL_NO_OBJ, { 0 } },
	{ "File %s was not found.", ZIL_NO_FILE, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

#if defined(ZIL_ZAF_MESSAGE_WINDOW_DATA)
static ZIL_PRIVATE_LANGUAGE_ELEMENT ZIL_FARDATA STRINGS[] =
{
	{ "&OK", ZIL_MSG_OK, { 0 }},
	{ "&Yes", ZIL_MSG_YES, { 0 } },
	{ "&No", ZIL_MSG_NO, { 0 } },
	{ "&Abort", ZIL_MSG_ABORT, { 0 } },
	{ "&Retry", ZIL_MSG_RETRY, { 0 } },
	{ "&Ignore", ZIL_MSG_IGNORE, { 0 } },
	{ "&Cancel", ZIL_MSG_CANCEL, { 0 } },
	{ "&Help", ZIL_MSG_HELP, { 0 } },
	{ ZIL_NULLP(char), 0xffff, { 0 } }
};
#endif

