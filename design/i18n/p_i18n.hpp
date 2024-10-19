#if defined(USE_HELP_CONTEXTS)
const UI_HELP_CONTEXT HELP_OPT_EDIT                 = 0x0003; // Edit Options
const UI_HELP_CONTEXT HELP_GENERAL                  = 0x0001; // About Defaults Editor
const UI_HELP_CONTEXT HELP_LOC_CURRENCY             = 0x000B; // Currency
const UI_HELP_CONTEXT HELP_LOC_DATE                 = 0x000C; // Date
const UI_HELP_CONTEXT HELP_LOC_NUMBER               = 0x000D; // Number
const UI_HELP_CONTEXT HELP_LOC_TIME                 = 0x000E; // Time
const UI_HELP_CONTEXT HELP_OPT_FILE                 = 0x0002; // File Options
const UI_HELP_CONTEXT HELP_OPT_LANGUAGE             = 0x0004; // Language Options
const UI_HELP_CONTEXT HELP_OPT_LOCALE               = 0x0005; // Locale Options
const UI_HELP_CONTEXT HELP_UIW_BIGNUM               = 0x000F; // Number
const UI_HELP_CONTEXT HELP_UIW_DATE                 = 0x0013; // Date
const UI_HELP_CONTEXT HELP_UIW_INTEGER              = 0x0012; // Integer
const UI_HELP_CONTEXT HELP_UIW_REAL                 = 0x0011; // Real
const UI_HELP_CONTEXT HELP_UIW_SYSTEM_BUTTON        = 0x0014; // System Button
const UI_HELP_CONTEXT HELP_UIW_TIME                 = 0x0010; // Time
const UI_HELP_CONTEXT HELP_UIW_WINDOW               = 0x0015; // Window
const UI_HELP_CONTEXT HELP_ZAF_MSG                  = 0x0016; // Message Window
const UI_HELP_CONTEXT HELP_UI_ERROR_SYSTEM          = 0x0009; // Error Window
const UI_HELP_CONTEXT HELP_UI_HELP_SYSTEM           = 0x000A; // Help Window
const UI_HELP_CONTEXT HELP_ZIL_DAY                  = 0x0007; // Day
const UI_HELP_CONTEXT HELP_ZIL_MONTH                = 0x0006; // Month
const UI_HELP_CONTEXT HELP_ZIL_TIME                 = 0x0008; // Am
#endif

#if defined(USE_ZAF_MESSAGE_WINDOW)
const ZIL_NUMBERID FIELD_2                          = 0x0002;
const ZIL_NUMBERID OK_OPTION                        = 0x0004;
const ZIL_NUMBERID FIELD_5                          = 0x0005;
const ZIL_NUMBERID YES_OPTION                       = 0x000A;
const ZIL_NUMBERID FIELD_3                          = 0x0003;
const ZIL_NUMBERID NO_OPTION                        = 0x0006;
const ZIL_NUMBERID FIELD_7                          = 0x0007;
const ZIL_NUMBERID ABORT_OPTION                     = 0x0008;
const ZIL_NUMBERID FIELD_9                          = 0x0009;
const ZIL_NUMBERID RETRY_OPTION                     = 0x000B;
const ZIL_NUMBERID FIELD_12                         = 0x000C;
const ZIL_NUMBERID IGNORE_OPTION                    = 0x000D;
const ZIL_NUMBERID FIELD_14                         = 0x000E;
const ZIL_NUMBERID CANCEL_OPTION                    = 0x000F;
const ZIL_NUMBERID FIELD_16                         = 0x0010;
const ZIL_NUMBERID HELP_OPTION                      = 0x0011;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0012;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0013;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0014;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0015;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0016;
#endif

#if defined(USE_LOC_DATE)
const ZIL_NUMBERID FIELD_21                         = 0x0015;
const ZIL_NUMBERID SEPARATOR                        = 0x0016;
const ZIL_NUMBERID PROMPT_FORMAT                    = 0x0022;
const ZIL_NUMBERID DATE_STRING_FORMAT               = 0x0023;
const ZIL_NUMBERID PROMPT_DATE_TIME_FORMAT          = 0x0024;
const ZIL_NUMBERID DATE_TIME_STRING_FORMAT          = 0x0025;
const ZIL_NUMBERID DEFAULT_FORMAT                   = 0x001E;
const ZIL_NUMBERID INPUT_FORMAT_OPTIONS             = 0x00BF;
const ZIL_NUMBERID FLAG_SHORT_DAY                   = 0x00AB;
const ZIL_NUMBERID FLAG_DAY_OF_WEEK                 = 0x00A9;
const ZIL_NUMBERID FLAG_SHORT_MONTH                 = 0x01CF;
const ZIL_NUMBERID FLAG_MONTH                       = 0x00AA;
const ZIL_NUMBERID FLAG_SHORT_YEAR                  = 0x01C4;
const ZIL_NUMBERID FLAG_UPPER_CASE                  = 0x021D;
const ZIL_NUMBERID FLAG_ZERO_FILL                   = 0x021E;
const ZIL_NUMBERID FLAG_SYSTEM                      = 0x0229;
const ZIL_NUMBERID SEPARATOR_OPTIONS                = 0x01EA;
const ZIL_NUMBERID FLAG_DEFAULT_SEPARATOR           = 0x01EB;
const ZIL_NUMBERID FLAG_DASH_SEPARATOR              = 0x01EC;
const ZIL_NUMBERID FLAG_SLASH_SEPARATOR             = 0x01F2;
const ZIL_NUMBERID COUNTRY_OPTIONS                  = 0x01F9;
const ZIL_NUMBERID FLAG_EUROPEAN_FORMAT             = 0x01FB;
const ZIL_NUMBERID FLAG_ASIAN_FORMAT                = 0x01FC;
const ZIL_NUMBERID FLAG_MILITARY_FORMAT             = 0x01FD;
const ZIL_NUMBERID FLAG_US_FORMAT                   = 0x01FE;
const ZIL_NUMBERID STORE_BUTTON                     = 0x003D;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x003E;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x003F;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0040;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0041;
#endif

#if defined(USE_LOC_TIME)
const ZIL_NUMBERID FIELD_28                         = 0x001C;
const ZIL_NUMBERID FIELD_39                         = 0x0027;
const ZIL_NUMBERID SEPARATOR                        = 0x001D;
const ZIL_NUMBERID TIME_FORMAT                      = 0x0028;
const ZIL_NUMBERID FIELD_41                         = 0x0029;
const ZIL_NUMBERID TIME_12_FORMAT                   = 0x002A;
const ZIL_NUMBERID DEFAULT_FORMAT                   = 0x0023;
const ZIL_NUMBERID INPUT_FORMAT_OPTIONS             = 0x00BF;
const ZIL_NUMBERID FLAG_HUNDREDTHS                  = 0x00AB;
const ZIL_NUMBERID FLAG_SECONDS                     = 0x00A9;
const ZIL_NUMBERID FLAG_NO_MINUTES                  = 0x01CF;
const ZIL_NUMBERID FLAG_NO_HOURS                    = 0x00AA;
const ZIL_NUMBERID FLAG_LOWER_CASE                  = 0x021D;
const ZIL_NUMBERID FLAG_UPPER_CASE                  = 0x023F;
const ZIL_NUMBERID FLAG_ZERO_FILL                   = 0x021E;
const ZIL_NUMBERID FLAG_SYSTEM                      = 0x0229;
const ZIL_NUMBERID SEPARATOR_OPTIONS                = 0x01EA;
const ZIL_NUMBERID FLAG_DEFAULT_SEPARATOR           = 0x01EB;
const ZIL_NUMBERID FLAG_NO_SEPARATOR                = 0x01F2;
const ZIL_NUMBERID FLAG_COLON_SEPARATOR             = 0x01EC;
const ZIL_NUMBERID COUNTRY_OPTIONS                  = 0x01F9;
const ZIL_NUMBERID FLAG_12_HOUR                     = 0x01FC;
const ZIL_NUMBERID FLAG_24_HOUR                     = 0x01FB;
const ZIL_NUMBERID STORE_BUTTON                     = 0x003E;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x003F;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0040;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0041;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0042;
#endif

#if defined(USE_UIW_DATE)
const ZIL_NUMBERID INVALID                          = 0x0002;
const ZIL_NUMBERID FIELD_7                          = 0x0007;
const ZIL_NUMBERID OUT_OF_RANGE                     = 0x0008;
const ZIL_NUMBERID FIELD_9                          = 0x0009;
const ZIL_NUMBERID VALUE_MISSING                    = 0x000A;
const ZIL_NUMBERID FIELD_11                         = 0x000B;
const ZIL_NUMBERID INVALID_NAME                     = 0x000C;
const ZIL_NUMBERID AMBIGUOUS                        = 0x000D;
const ZIL_NUMBERID FIELD_14                         = 0x000E;
const ZIL_NUMBERID FIELD_1                          = 0x0001;
const ZIL_NUMBERID PROMPT_LESS_THAN                 = 0x0013;
const ZIL_NUMBERID LESS_THAN_RANGE                  = 0x0014;
const ZIL_NUMBERID PROMPT_GREATER_THAN              = 0x0015;
const ZIL_NUMBERID GREATER_THAN_RANGE               = 0x0016;
const ZIL_NUMBERID STORE_BUTTON                     = 0x001B;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x001C;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x001D;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x001E;
const ZIL_NUMBERID HELP_BUTTON                      = 0x001F;
#endif

#if defined(USE_UIW_INTEGER)
const ZIL_NUMBERID PROMPT_INVALID                   = 0x0001;
const ZIL_NUMBERID INVALID                          = 0x0002;
const ZIL_NUMBERID PROMPT_OUT_OF_RANGE              = 0x0007;
const ZIL_NUMBERID OUT_OF_RANGE                     = 0x0008;
const ZIL_NUMBERID PROMPT_LESS_THAN                 = 0x000A;
const ZIL_NUMBERID LESS_THAN_RANGE                  = 0x000B;
const ZIL_NUMBERID PROMPT_GREATER_THAN              = 0x000C;
const ZIL_NUMBERID GREATER_THAN_RANGE               = 0x000D;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0012;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0013;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0014;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0015;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0016;
#endif

#if defined(USE_UIW_REAL)
const ZIL_NUMBERID PROMPT_INVALID                   = 0x0001;
const ZIL_NUMBERID INVALID                          = 0x0002;
const ZIL_NUMBERID PROMPT_OUT_OF_RANGE              = 0x0007;
const ZIL_NUMBERID OUT_OF_RANGE                     = 0x0008;
const ZIL_NUMBERID PROMPT_LESS_THAN                 = 0x000A;
const ZIL_NUMBERID LESS_THAN_RANGE                  = 0x000B;
const ZIL_NUMBERID PROMPT_GREATER_THAN              = 0x000C;
const ZIL_NUMBERID GREATER_THAN_RANGE               = 0x000D;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0012;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0013;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0014;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0015;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0016;
#endif

#if defined(USE_UIW_SYSTEM_BUTTON)
const ZIL_NUMBERID FIELD_1                          = 0x0001;
const ZIL_NUMBERID RESTORE_OPTION                   = 0x0002;
const ZIL_NUMBERID FIELD_7                          = 0x0007;
const ZIL_NUMBERID MOVE_OPTION                      = 0x0008;
const ZIL_NUMBERID FIELD_9                          = 0x0009;
const ZIL_NUMBERID SIZE_OPTION                      = 0x000A;
const ZIL_NUMBERID FIELD_11                         = 0x000B;
const ZIL_NUMBERID MINIMIZE_OPTION                  = 0x000C;
const ZIL_NUMBERID MAXIMIZE_OPTION                  = 0x000D;
const ZIL_NUMBERID FIELD_14                         = 0x000E;
const ZIL_NUMBERID FIELD_16                         = 0x0010;
const ZIL_NUMBERID CLOSE_OPTION                     = 0x0012;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0017;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0018;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0019;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x001A;
const ZIL_NUMBERID HELP_BUTTON                      = 0x001B;
#endif

#if defined(USE_UIW_TIME)
const ZIL_NUMBERID FIELD_1                          = 0x0001;
const ZIL_NUMBERID INVALID                          = 0x0002;
const ZIL_NUMBERID FIELD_7                          = 0x0007;
const ZIL_NUMBERID OUT_OF_RANGE                     = 0x0008;
const ZIL_NUMBERID FIELD_9                          = 0x0009;
const ZIL_NUMBERID VALUE_MISSING                    = 0x000A;
const ZIL_NUMBERID PROMPT_LESS_THAN                 = 0x000F;
const ZIL_NUMBERID LESS_THAN_RANGE                  = 0x0010;
const ZIL_NUMBERID PROMPT_GREATER_THAN              = 0x0011;
const ZIL_NUMBERID GREATER_THAN_RANGE               = 0x0012;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0017;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0018;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0019;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x001A;
const ZIL_NUMBERID HELP_BUTTON                      = 0x001B;
#endif

#if defined(USE_UIW_WINDOW)
const ZIL_NUMBERID PROMPT_ERROR                     = 0x000D;
const ZIL_NUMBERID OBJECT_ERROR                     = 0x000E;
const ZIL_NUMBERID PROMPT_UNKNOWN                   = 0x0001;
const ZIL_NUMBERID UNKNOWN_OBJECT                   = 0x0002;
const ZIL_NUMBERID PROMPT_NOT_FOUND                 = 0x0007;
const ZIL_NUMBERID OBJECT_NOT_FOUND                 = 0x0008;
const ZIL_NUMBERID PROMPT_FILE                      = 0x0009;
const ZIL_NUMBERID FILE_NOT_FOUND                   = 0x000A;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0013;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0014;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0015;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0016;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0017;
#endif

#if defined(USE_UI_ERROR_SYSTEM)
const ZIL_NUMBERID PROMPT_TITLE                     = 0x0001;
const ZIL_NUMBERID TITLE                            = 0x0002;
const ZIL_NUMBERID PROMPT_OK                        = 0x0007;
const ZIL_NUMBERID OK_SELECTION                     = 0x0008;
const ZIL_NUMBERID FIELD_9                          = 0x0009;
const ZIL_NUMBERID CANCEL_SELECTION                 = 0x000A;
const ZIL_NUMBERID FIELD_11                         = 0x000B;
const ZIL_NUMBERID ICONNAME                         = 0x000C;
const ZIL_NUMBERID MOVE_OPTION                      = 0x000D;
const ZIL_NUMBERID FIELD_14                         = 0x000E;
const ZIL_NUMBERID FIELD_19                         = 0x0013;
const ZIL_NUMBERID CLOSE_OPTION                     = 0x0014;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0017;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0018;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0019;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x001A;
const ZIL_NUMBERID HELP_BUTTON                      = 0x001B;
#endif

#if defined(USE_ZAF_LANGUAGE_WINDOW)
#endif

#if defined(USE_ZAF_LOCALE_WINDOW)
#endif

#if defined(USE_ZIL_DAY)
const ZIL_NUMBERID PROMPT_SHORT_DAY                 = 0x0009;
const ZIL_NUMBERID SHORT_DAY_TABLE                  = 0x000D;
const ZIL_NUMBERID PROMPT_ABBREVIATED_DAY           = 0x00B1;
const ZIL_NUMBERID ABBREVIATED_DAY_TABLE            = 0x00B2;
const ZIL_NUMBERID PROMPT_LONG_DAY                  = 0x000A;
const ZIL_NUMBERID LONG_DAY_TABLE                   = 0x001C;
const ZIL_NUMBERID STORE_BUTTON                     = 0x00EF;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x00F0;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x00F1;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x00F2;
const ZIL_NUMBERID HELP_BUTTON                      = 0x00F3;
#endif

#if defined(USE_ZIL_MONTH)
const ZIL_NUMBERID PROMPT_SHORT_MONTH               = 0x000B;
const ZIL_NUMBERID SHORT_MONTH_TABLE                = 0x0013;
const ZIL_NUMBERID PROMPT_ABBREVIATED_MONTHS        = 0x00B9;
const ZIL_NUMBERID ABBREVIATED_MONTH_TABLE          = 0x00BA;
const ZIL_NUMBERID PROMPT_LONG_MONTH                = 0x000C;
const ZIL_NUMBERID LONG_MONTH_TABLE                 = 0x001D;
const ZIL_NUMBERID STORE_BUTTON                     = 0x00DB;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x00DC;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x00DD;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x00DE;
const ZIL_NUMBERID HELP_BUTTON                      = 0x00DF;
#endif

#if defined(USE_ZIL_TIME)
const ZIL_NUMBERID PROMPT_AM                        = 0x0001;
const ZIL_NUMBERID AM_PTR                           = 0x0002;
const ZIL_NUMBERID PROMPT_PM                        = 0x0007;
const ZIL_NUMBERID PM_PTR                           = 0x0008;
const ZIL_NUMBERID PROMPT_TIME_ZONE                 = 0x000A;
const ZIL_NUMBERID TIME_ZONE                        = 0x000B;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0010;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0011;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0012;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0013;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0014;
#endif

#if defined(USE_ZAF_DEFAULTS_EDITOR)
const ZIL_NUMBERID MENU                             = 0x0001;
const ZIL_NUMBERID FILE                             = 0x0002;
const ZIL_NUMBERID FILE_NEW                         = 0x0005;
const ZIL_NUMBERID FILE_OPEN                        = 0x0006;
const ZIL_NUMBERID FILE_SAVE                        = 0x0007;
const ZIL_NUMBERID FILE_SAVE_AS                     = 0x0008;
const ZIL_NUMBERID FILE_CLOSE                       = 0x0011;
const ZIL_NUMBERID FILE_SEP1                        = 0x0009;
const ZIL_NUMBERID FILE_DELETE                      = 0x000A;
const ZIL_NUMBERID FILE_SEP2                        = 0x000B;
const ZIL_NUMBERID FILE_EXIT                        = 0x000C;
const ZIL_NUMBERID TOOLS                            = 0x007E;
const ZIL_NUMBERID LANGUAGE                         = 0x004C;
const ZIL_NUMBERID LANGUAGE                         = 0x0048;
const ZIL_NUMBERID LANGUAGE                         = 0x007C;
const ZIL_NUMBERID LANGUAGE_SEP1                    = 0x0049;
const ZIL_NUMBERID LANGUAGE                         = 0x004A;
const ZIL_NUMBERID LANGUAGE                         = 0x004B;
const ZIL_NUMBERID LANGUAGE                         = 0x004D;
const ZIL_NUMBERID LANGUAGE                         = 0x004E;
const ZIL_NUMBERID LANGUAGE_SEP2                    = 0x004F;
const ZIL_NUMBERID LANGUAGE                         = 0x0068;
const ZIL_NUMBERID LANGUAGE                         = 0x0069;
const ZIL_NUMBERID LANGUAGE                         = 0x0050;
const ZIL_NUMBERID LOCALE                           = 0x0058;
const ZIL_NUMBERID LOCALE                           = 0x0051;
const ZIL_NUMBERID EXPORT                           = 0x007D;
const ZIL_NUMBERID LOCALE_SEP1                      = 0x0052;
const ZIL_NUMBERID LOCALE                           = 0x0066;
const ZIL_NUMBERID LOCALE                           = 0x0053;
const ZIL_NUMBERID LOCALE                           = 0x0054;
const ZIL_NUMBERID LOCALE                           = 0x0055;
const ZIL_NUMBERID LOCALE_SEP2                      = 0x0056;
const ZIL_NUMBERID LOCALE                           = 0x006A;
const ZIL_NUMBERID LOCALE                           = 0x006B;
const ZIL_NUMBERID LOCALE                           = 0x0057;
const ZIL_NUMBERID HELP                             = 0x0004;
const ZIL_NUMBERID HELP_OPT_INDEX                   = 0x0075;
const ZIL_NUMBERID HELP_SEP1                        = 0x0076;
const ZIL_NUMBERID HELP_OPT_FILE                    = 0x0077;
const ZIL_NUMBERID HELP_OPT_LANGUAGE                = 0x0078;
const ZIL_NUMBERID HELP_OPT_LOCALE                  = 0x0079;
const ZIL_NUMBERID HELP_SEP2                        = 0x007A;
const ZIL_NUMBERID HELP_ABOUT                       = 0x007B;
#endif

#if defined(USE_UI_HELP_SYSTEM)
const ZIL_NUMBERID P_HELP_TOPIC                     = 0x0002;
const ZIL_NUMBERID HELPTOPIC                        = 0x0003;
const ZIL_NUMBERID P_HELP_INDEX                     = 0x0004;
const ZIL_NUMBERID HELPINDEX                        = 0x0005;
const ZIL_NUMBERID P_SHOW_TOPIC                     = 0x0006;
const ZIL_NUMBERID SHOWTOPIC                        = 0x0007;
const ZIL_NUMBERID P_SHOW_INDEX                     = 0x0008;
const ZIL_NUMBERID SHOWINDEX                        = 0x0009;
const ZIL_NUMBERID P_SELECT_TITLE                   = 0x000F;
const ZIL_NUMBERID SELECTTITLE                      = 0x0014;
const ZIL_NUMBERID P_SELECT_MESSAGE                 = 0x0010;
const ZIL_NUMBERID SELECTMESSAGE                    = 0x0015;
const ZIL_NUMBERID P_CLOSE                          = 0x0011;
const ZIL_NUMBERID CLOSE                            = 0x0016;
const ZIL_NUMBERID P_ICON_TITLE                     = 0x0012;
const ZIL_NUMBERID ICONTITLE                        = 0x0017;
const ZIL_NUMBERID P_ICON_NAME                      = 0x0013;
const ZIL_NUMBERID ICONNAME                         = 0x0018;
const ZIL_NUMBERID STORE_BUTTON                     = 0x000A;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x000B;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x000C;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x000D;
const ZIL_NUMBERID HELP_BUTTON                      = 0x000E;
#endif

#if defined(USE_LOC_CURRENCY)
const ZIL_NUMBERID FIELD_38                         = 0x0026;
const ZIL_NUMBERID SYMBOL                           = 0x0027;
const ZIL_NUMBERID FIELD_40                         = 0x0028;
const ZIL_NUMBERID INTL_SYMBOL                      = 0x0029;
const ZIL_NUMBERID FIELD_42                         = 0x002A;
const ZIL_NUMBERID DECIMAL_SYMBOL                   = 0x002B;
const ZIL_NUMBERID FIELD_44                         = 0x002C;
const ZIL_NUMBERID THOUSANDS                        = 0x002D;
const ZIL_NUMBERID FIELD_46                         = 0x002E;
const ZIL_NUMBERID GROUPING                         = 0x0031;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0003;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0035;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0004;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0005;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0006;
#endif

#if defined(USE_LOC_NUMBER)
const ZIL_NUMBERID FIELD_58                         = 0x000F;
const ZIL_NUMBERID FRACTIONS_DIGIT                  = 0x0010;
const ZIL_NUMBERID FIELD_60                         = 0x0011;
const ZIL_NUMBERID INT_FRACTIONS_DIGIT              = 0x0012;
const ZIL_NUMBERID FIELD_26                         = 0x0002;
const ZIL_NUMBERID DECIMAL_SYMBOL                   = 0x0004;
const ZIL_NUMBERID FIELD_28                         = 0x0005;
const ZIL_NUMBERID THOUSANDS                        = 0x0006;
const ZIL_NUMBERID FIELD_30                         = 0x0007;
const ZIL_NUMBERID GROUPING                         = 0x0008;
const ZIL_NUMBERID FIELD_37                         = 0x000C;
const ZIL_NUMBERID LEFT_PAREN                       = 0x000D;
const ZIL_NUMBERID FIELD_125                        = 0x002A;
const ZIL_NUMBERID RIGHT_PAREN                      = 0x000E;
const ZIL_NUMBERID FIELD_115                        = 0x0018;
const ZIL_NUMBERID POS_PROMPT1                      = 0x0019;
const ZIL_NUMBERID P_SEP_BY_SPACE                   = 0x001A;
const ZIL_NUMBERID POS_PROMPT2                      = 0x001B;
const ZIL_NUMBERID P_CS_PRECEDES                    = 0x001C;
const ZIL_NUMBERID POS_PROMPT3                      = 0x001D;
const ZIL_NUMBERID P_SIGN_POSN                      = 0x001E;
const ZIL_NUMBERID POS_PROMPT4                      = 0x001F;
const ZIL_NUMBERID POSITIVE_SIGN                    = 0x0020;
const ZIL_NUMBERID FIELD_41                         = 0x0003;
const ZIL_NUMBERID DEFDIGITS                        = 0x0009;
const ZIL_NUMBERID FIELD_43                         = 0x000A;
const ZIL_NUMBERID ALTDIGITS                        = 0x000B;
const ZIL_NUMBERID FIELD_124                        = 0x0021;
const ZIL_NUMBERID NEG_PROMPT1                      = 0x0022;
const ZIL_NUMBERID N_SEP_BY_SPACE                   = 0x0023;
const ZIL_NUMBERID NEG_PROMPT2                      = 0x0024;
const ZIL_NUMBERID N_CS_PRECEDES                    = 0x0025;
const ZIL_NUMBERID NEG_PROMPT3                      = 0x0026;
const ZIL_NUMBERID N_SIGN_POSN                      = 0x0027;
const ZIL_NUMBERID NEG_PROMPT4                      = 0x0028;
const ZIL_NUMBERID NEGATIVE_SIGN                    = 0x0029;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0013;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0014;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0015;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0016;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0017;
#endif

#if defined(USE_UIW_BIGNUM)
const ZIL_NUMBERID PROMPT_INVALID                   = 0x0001;
const ZIL_NUMBERID INVALID                          = 0x0002;
const ZIL_NUMBERID PROMPT_OUT_OF_RANGE              = 0x0007;
const ZIL_NUMBERID OUT_OF_RANGE                     = 0x0008;
const ZIL_NUMBERID PROMPT_LESS_THAN                 = 0x000A;
const ZIL_NUMBERID LESS_THAN_RANGE                  = 0x000B;
const ZIL_NUMBERID PROMPT_GREATER_THAN              = 0x000C;
const ZIL_NUMBERID GREATER_THAN_RANGE               = 0x000D;
const ZIL_NUMBERID STORE_BUTTON                     = 0x0012;
const ZIL_NUMBERID STOREAS_BUTTON                   = 0x0013;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0014;
const ZIL_NUMBERID DEFAULT_BUTTON                   = 0x0015;
const ZIL_NUMBERID HELP_BUTTON                      = 0x0016;
#endif

#if defined(USE_DERIVED_OBJECTS)
#endif

