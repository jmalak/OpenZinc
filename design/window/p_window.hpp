#if defined(USE_DEFAULT_SUPPORT)
#endif

#if defined(USE_TEST_WINDOW)
const ZIL_NUMBERID EXIT_TESTING                     = 0x0002;
#endif

#if defined(USE_HELP_CONTEXTS)
const UI_HELP_CONTEXT HELP_GENERAL                  = 0x0001; // About Window Editor
const UI_HELP_CONTEXT HELP_OPT_EDIT                 = 0x0003; // Edit Options
const UI_HELP_CONTEXT HELP_OPT_FILE                 = 0x0002; // File Options
const UI_HELP_CONTEXT HELP_OPT_OBJECT               = 0x002A; // Object Options
const UI_HELP_CONTEXT HELP_OPT_WINDOW               = 0x0004; // Window Options
const UI_HELP_CONTEXT HELP_UIW_BIGNUM               = 0x0005; // Bignum
const UI_HELP_CONTEXT HELP_UIW_BORDER               = 0x0006; // Border
const UI_HELP_CONTEXT HELP_UIW_BUTTON               = 0x0007; // Button
const UI_HELP_CONTEXT HELP_UIW_COMBO_BOX            = 0x0008; // Combo Box
const UI_HELP_CONTEXT HELP_UIW_DATE                 = 0x0009; // Date
const UI_HELP_CONTEXT HELP_UIW_FORMATTED_STRING     = 0x000A; // Formatted String
const UI_HELP_CONTEXT HELP_UIW_GROUP                = 0x000B; // Group
const UI_HELP_CONTEXT HELP_UIW_HZ_LIST              = 0x000C; // Horizontal List
const UI_HELP_CONTEXT HELP_UIW_ICON                 = 0x000D; // Icon
const UI_HELP_CONTEXT HELP_UIW_INTEGER              = 0x000E; // Integer
const UI_HELP_CONTEXT HELP_UIW_MAXIMIZE_BUTTON      = 0x000F; // Maximize Button
const UI_HELP_CONTEXT HELP_UIW_MINIMIZE_BUTTON      = 0x0010; // Minimize Button
const UI_HELP_CONTEXT HELP_UIW_NOTEBOOK             = 0x0011; // Notebook
const UI_HELP_CONTEXT HELP_UIW_POP_UP_ITEM          = 0x0012; // Pop-up Item
const UI_HELP_CONTEXT HELP_UIW_PROMPT               = 0x0014; // Prompt
const UI_HELP_CONTEXT HELP_UIW_PULL_DOWN_ITEM       = 0x0015; // Pull-down Item
const UI_HELP_CONTEXT HELP_UIW_PULL_DOWN_MENU       = 0x0016; // Pull-down Menu
const UI_HELP_CONTEXT HELP_UIW_REAL                 = 0x0017; // Real
const UI_HELP_CONTEXT HELP_UIW_SCROLL_BAR           = 0x0018; // Scroll Bar/Slider
const UI_HELP_CONTEXT HELP_UIW_SPIN_CONTROL         = 0x0019; // Spin Control
const UI_HELP_CONTEXT HELP_UIW_STATUS_BAR           = 0x0025; // Status Bar
const UI_HELP_CONTEXT HELP_UIW_STRING               = 0x001A; // String
const UI_HELP_CONTEXT HELP_UIW_SYSTEM_BUTTON        = 0x001B; // System Button
const UI_HELP_CONTEXT HELP_UIW_TABLE                = 0x002D; // Table
const UI_HELP_CONTEXT HELP_UIW_TABLE_HEADER         = 0x002E; // Table Header
const UI_HELP_CONTEXT HELP_UIW_TABLE_RECORD         = 0x002F; // Table Record
const UI_HELP_CONTEXT HELP_UIW_TEXT                 = 0x001C; // Text
const UI_HELP_CONTEXT HELP_UIW_TIME                 = 0x001D; // Time
const UI_HELP_CONTEXT HELP_UIW_TITLE                = 0x001E; // Title
const UI_HELP_CONTEXT HELP_UIW_TOOL_BAR             = 0x001F; // Tool Bar
const UI_HELP_CONTEXT HELP_UIW_VT_LIST              = 0x0020; // Vertical List
const UI_HELP_CONTEXT HELP_UIW_WINDOW               = 0x0021; // Window
const UI_HELP_CONTEXT HELP_UIW_POP_UP_MENU          = 0x0013; // Pop-up Menu
const UI_HELP_CONTEXT HELP_UI_ADVANCED              = 0x0022; // Advanced Page
const UI_HELP_CONTEXT HELP_UI_GENERAL               = 0x0026; // General Page
const UI_HELP_CONTEXT HELP_UI_OBJECT                = 0x0024; // Subobjects Page
const UI_HELP_CONTEXT HELP_UI_POSITION              = 0x0030; // Position Page
const UI_HELP_CONTEXT HELP_ZAF_DERIVED_OBJECT       = 0x002B; // Derived Object
const UI_HELP_CONTEXT HELP_ZAF_EDIT_GROUP           = 0x0027; // Edit Group
const UI_HELP_CONTEXT HELP_ZAF_GEOMETRY_MANAGER     = 0x002C; // Geometry Page
const UI_HELP_CONTEXT HELP_ZAF_WINDOW_EDITOR        = 0x0029; // Window Editor
const UI_HELP_CONTEXT HELP_ZAF_WINDOW_PREFERENCES   = 0x0028; // Preferences Help
const UI_HELP_CONTEXT HELP_UI_GEOMETRY_MANAGER      = 0x0023; // Geometry
#endif

#if defined(USE_DEFAULT_WINDOW)
#endif

#if defined(USE_OBJ_TABLE)
const ZIL_NUMBERID ZMSG_BORDER                      = 0x0001;
const ZIL_NUMBERID ZMSG_BUTTON                      = 0x0002;
const ZIL_NUMBERID ZMSG_DATE                        = 0x0003;
const ZIL_NUMBERID ZMSG_FORMATTED_STRING            = 0x0004;
const ZIL_NUMBERID ZMSG_ICON                        = 0x0005;
const ZIL_NUMBERID ZMSG_INTEGER                     = 0x0006;
const ZIL_NUMBERID ZMSG_MAXIMIZE_BUTTON             = 0x0007;
const ZIL_NUMBERID ZMSG_MINIMIZE_BUTTON             = 0x0008;
const ZIL_NUMBERID ZMSG_NUMBER                      = 0x0009;
const ZIL_NUMBERID ZMSG_PROMPT                      = 0x000B;
const ZIL_NUMBERID ZMSG_REAL                        = 0x000C;
const ZIL_NUMBERID ZMSG_STRING                      = 0x000D;
const ZIL_NUMBERID ZMSG_TEXT                        = 0x000F;
const ZIL_NUMBERID ZMSG_TIME                        = 0x0010;
const ZIL_NUMBERID ZMSG_TITLE                       = 0x0011;
const ZIL_NUMBERID ZMSG_WINDOW_OBJECT               = 0x0012;
const ZIL_NUMBERID ZMSG_POP_UP_ITEM                 = 0x0013;
const ZIL_NUMBERID ZMSG_IMAGE                       = 0x0014;
const ZIL_NUMBERID ZMSG_GROUP                       = 0x0017;
const ZIL_NUMBERID ZMSG_BIGNUM                      = 0x0018;
const ZIL_NUMBERID ZMSG_CHECK_BOX                   = 0x0019;
const ZIL_NUMBERID ZMSG_RADIO_BUTTON                = 0x001A;
const ZIL_NUMBERID ZMSG_LIST                        = 0x03E9;
const ZIL_NUMBERID ZMSG_POP_UP_MENU                 = 0x03EA;
const ZIL_NUMBERID ZMSG_PULL_DOWN_ITEM              = 0x03EB;
const ZIL_NUMBERID ZMSG_PULL_DOWN_MENU              = 0x03EC;
const ZIL_NUMBERID ZMSG_SCROLL_BAR                  = 0x03ED;
const ZIL_NUMBERID ZMSG_SYSTEM_BUTTON               = 0x03EE;
const ZIL_NUMBERID ZMSG_WINDOW                      = 0x03EF;
const ZIL_NUMBERID ZMSG_COMBO_BOX                   = 0x03F0;
const ZIL_NUMBERID ZMSG_TOOL_BAR                    = 0x03F1;
const ZIL_NUMBERID ZMSG_VT_LIST                     = 0x03F2;
const ZIL_NUMBERID ZMSG_HZ_LIST                     = 0x03F3;
const ZIL_NUMBERID ZMSG_LIST_ITEM                   = 0x03F4;
const ZIL_NUMBERID ZMSG_TABLE                       = 0x03F5;
const ZIL_NUMBERID ZMSG_TABLE_RECORD                = 0x03F6;
const ZIL_NUMBERID ZMSG_STATUS_BAR                  = 0x03F7;
const ZIL_NUMBERID ZMSG_STATUS_ITEM                 = 0x03F8;
const ZIL_NUMBERID ZMSG_SPIN_CONTROL                = 0x03F9;
const ZIL_NUMBERID ZMSG_NOTEBOOK                    = 0x03FA;
const ZIL_NUMBERID ZMSG_CONSTRAINT                  = 0x03FB;
const ZIL_NUMBERID ZMSG_ATTACHMENT                  = 0x03FC;
const ZIL_NUMBERID ZMSG_DIMENSION_CONSTRAINT        = 0x03FD;
const ZIL_NUMBERID ZMSG_RELATIVE_CONSTRAINT         = 0x03FE;
const ZIL_NUMBERID ZMSG_TABLE_HEADER                = 0x03FF;
const ZIL_NUMBERID ZMSG_GEOMETRY_MANAGER            = 0x07D3;
const ZIL_NUMBERID ZMSG_HZ_SLIDER                   = 0x0BB8;
const ZIL_NUMBERID ZMSG_VT_SLIDER                   = 0x0BB9;
const ZIL_NUMBERID ZMSG_DERIVE                      = 0x0BC1;
const ZIL_NUMBERID ZMSG_EDIT_GROUP                  = 0x0BC2;
#endif

#if defined(USE_OBJ_POSITION)
const ZIL_NUMBERID GROUP_POSITION_SIZE              = 0x0002;
const ZIL_NUMBERID COLUMN_PROMPT                    = 0x0003;
const ZIL_NUMBERID LINE_PROMPT                      = 0x0004;
const ZIL_NUMBERID WIDTH_PROMPT                     = 0x0005;
const ZIL_NUMBERID HEIGHT_PROMPT                    = 0x0006;
const ZIL_NUMBERID PIXEL_COORDINATE_FIELD           = 0x0007;
const ZIL_NUMBERID FIELD_PIXEL_COLUMN               = 0x0008;
const ZIL_NUMBERID FIELD_PIXEL_LINE                 = 0x0009;
const ZIL_NUMBERID FIELD_PIXEL_WIDTH                = 0x000A;
const ZIL_NUMBERID FIELD_PIXEL_HEIGHT               = 0x000B;
const ZIL_NUMBERID MINICELL_COORDINATE_FIELD        = 0x000C;
const ZIL_NUMBERID FIELD_MINICELL_COLUMN            = 0x000D;
const ZIL_NUMBERID FIELD_MINICELL_LINE              = 0x000E;
const ZIL_NUMBERID FIELD_MINICELL_WIDTH             = 0x000F;
const ZIL_NUMBERID FIELD_MINICELL_HEIGHT            = 0x0010;
const ZIL_NUMBERID CELL_FIELD                       = 0x0011;
const ZIL_NUMBERID FIELD_CELL_COLUMN                = 0x0012;
const ZIL_NUMBERID FIELD_CELL_LINE                  = 0x0013;
const ZIL_NUMBERID FIELD_CELL_WIDTH                 = 0x0014;
const ZIL_NUMBERID FIELD_CELL_HEIGHT                = 0x0015;
const ZIL_NUMBERID GROUP_BORDER                     = 0x0016;
const ZIL_NUMBERID FLAG_BORDER                      = 0x0018;
const ZIL_NUMBERID FLAG_NO_BORDER                   = 0x0017;
const ZIL_NUMBERID GROUP_REGION                     = 0x0019;
const ZIL_NUMBERID FLAG_FIELD_REGION                = 0x001A;
const ZIL_NUMBERID FLAG_NON_FIELD_REGION            = 0x001B;
const ZIL_NUMBERID GROUP_ALIGNMENT                  = 0x001C;
const ZIL_NUMBERID FLAG_LEFT_JUSTIFY                = 0x001D;
const ZIL_NUMBERID FLAG_CENTER_JUSTIFY              = 0x001E;
const ZIL_NUMBERID FLAG_RIGHT_JUSTIFY               = 0x001F;
const ZIL_NUMBERID BUTTON_OK                        = 0x0020;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0021;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0022;
#endif

#if defined(USE_OBJ_SUBOBJECT)
const ZIL_NUMBERID PROMPT_OBJECTS                   = 0x0001;
const ZIL_NUMBERID LIST_OBJECTS                     = 0x0002;
const ZIL_NUMBERID PROMPT_DIRECTORIES               = 0x0003;
const ZIL_NUMBERID LIST_DIRECTORIES                 = 0x0004;
const ZIL_NUMBERID BUTTON_EDIT                      = 0x0005;
const ZIL_NUMBERID BUTTON_ADD                       = 0x0006;
const ZIL_NUMBERID BUTTON_DELETE                    = 0x0007;
const ZIL_NUMBERID BUTTON_MOVE_UP                   = 0x0008;
const ZIL_NUMBERID BUTTON_MOVE_DOWN                 = 0x0009;
const ZIL_NUMBERID FIELD_ADD_OBJECT                 = 0x000A;
const ZIL_NUMBERID LIST_ADD_OBJECT                  = 0x000B;
const ZIL_NUMBERID BUTTON_OK                        = 0x000C;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000D;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000E;
const ZIL_NUMBERID FIELD_102                        = 0x0066;
#endif

#if defined(USE_ZAF_OBJECT_EDITOR)
#endif

#if defined(USE_ZAF_WINDOW_PREFERENCES)
const ZIL_NUMBERID GROUP_DELTA                      = 0x003E;
const ZIL_NUMBERID FIELD_63                         = 0x003F;
const ZIL_NUMBERID FIELD_DELTA_PATHNAME             = 0x0040;
const ZIL_NUMBERID FIELD_67                         = 0x0043;
const ZIL_NUMBERID FIELD_DELTA_CURRENT              = 0x0044;
const ZIL_NUMBERID FIELD_72                         = 0x0048;
const ZIL_NUMBERID FIELD_LANGUAGE                   = 0x0049;
const ZIL_NUMBERID FIELD_OPEN                       = 0x0041;
const ZIL_NUMBERID FIELD_CLOSE                      = 0x0042;
const ZIL_NUMBERID GROUP_MINICELL                   = 0x0035;
const ZIL_NUMBERID FIELD_54                         = 0x0036;
const ZIL_NUMBERID FIELD_NUMX                       = 0x0037;
const ZIL_NUMBERID FIELD_56                         = 0x0038;
const ZIL_NUMBERID FIELD_DENX                       = 0x0039;
const ZIL_NUMBERID FIELD_58                         = 0x003A;
const ZIL_NUMBERID FIELD_NUMY                       = 0x003B;
const ZIL_NUMBERID FIELD_60                         = 0x003C;
const ZIL_NUMBERID FIELD_DENY                       = 0x003D;
const ZIL_NUMBERID GROUP_FILE                       = 0x002E;
const ZIL_NUMBERID FIELD_47                         = 0x002F;
const ZIL_NUMBERID FIELD_BACKUPS                    = 0x0030;
const ZIL_NUMBERID FIELD_49                         = 0x0031;
const ZIL_NUMBERID FIELD_DEFAULT_EXTENSION          = 0x0032;
const ZIL_NUMBERID OPTION_WRITE_HPP                 = 0x0033;
const ZIL_NUMBERID OPTION_WRITE_CPP                 = 0x0034;
const ZIL_NUMBERID BUTTON_OK                        = 0x000B;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000C;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000D;
#endif

#if defined(USE_ZAF_EDIT_GROUP)
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID INPUT_JUSTIFICATION_OPTIONS      = 0x000E;
const ZIL_NUMBERID FLAG_JUSTIFY_NONE                = 0x006F;
const ZIL_NUMBERID FLAG_JUSTIFY_LEFT                = 0x000F;
const ZIL_NUMBERID FLAG_JUSTIFY_CENTER              = 0x0010;
const ZIL_NUMBERID FLAG_JUSTIFY_RIGHT               = 0x0011;
const ZIL_NUMBERID INPUT_SORTING_OPTION             = 0x0013;
const ZIL_NUMBERID FLAG_POSITION_SORT               = 0x0014;
const ZIL_NUMBERID BUTTON_OK                        = 0x0016;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0017;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0018;
#endif

#if defined(USE_UIW_BIGNUM)
const ZIL_NUMBERID PROMPT_PRECISION_RANGE           = 0x0001;
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0002;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0003;
const ZIL_NUMBERID PROMPT_PRECISION                 = 0x0004;
const ZIL_NUMBERID FIELD_PRECISION                  = 0x0005;
const ZIL_NUMBERID PROMPT_RANGE                     = 0x0006;
const ZIL_NUMBERID FIELD_RANGE                      = 0x0007;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0008;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0009;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x000A;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x000B;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000C;
const ZIL_NUMBERID LIST_FLAGS                       = 0x0010;
const ZIL_NUMBERID INPUT_FORMAT_OPTIONS             = 0x0011;
const ZIL_NUMBERID FLAG_CURRENCY                    = 0x0012;
const ZIL_NUMBERID FLAG_CREDIT                      = 0x0013;
const ZIL_NUMBERID FLAG_COMMA                       = 0x0014;
const ZIL_NUMBERID FLAG_PERCENT                     = 0x0015;
const ZIL_NUMBERID OPTION_FORMAT                    = 0x0072;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x0071;
const ZIL_NUMBERID BUTTON_OK                        = 0x0017;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0018;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0019;
#endif

#if defined(USE_UIW_BORDER)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID BUTTON_OK                        = 0x0003;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0004;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0005;
#endif

#if defined(USE_UIW_BUTTON)
const ZIL_NUMBERID PROMPT_TEXT                      = 0x0001;
const ZIL_NUMBERID FIELD_TEXT                       = 0x0002;
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0003;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0004;
const ZIL_NUMBERID PROMPT_IMAGE                     = 0x0005;
const ZIL_NUMBERID FIELD_IMAGE                      = 0x0006;
const ZIL_NUMBERID LIST_IMAGE                       = 0x0007;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0008;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0009;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x000A;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x000B;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000C;
const ZIL_NUMBERID FLAGS                            = 0x0010;
const ZIL_NUMBERID TYPE_OPTIONS                     = 0x0011;
const ZIL_NUMBERID FLAG_NORMAL_BUTTON               = 0x0012;
const ZIL_NUMBERID FLAG_RADIO_BUTTON                = 0x0014;
const ZIL_NUMBERID FLAG_CHECK_BOX                   = 0x0015;
const ZIL_NUMBERID OPTION_DEFAULTS                  = 0x0024;
const ZIL_NUMBERID FLAG_MESSAGE_BUTTON              = 0x0013;
const ZIL_NUMBERID FLAG_DEFAULT_BUTTON              = 0x0025;
const ZIL_NUMBERID STATE_OPTIONS                    = 0x001C;
const ZIL_NUMBERID FLAG_1_STATE                     = 0x001D;
const ZIL_NUMBERID FLAG_2_STATE                     = 0x001E;
const ZIL_NUMBERID HEIGHT_OPTIONS                   = 0x0016;
const ZIL_NUMBERID FLAG_NORMAL_HEIGHT               = 0x0017;
const ZIL_NUMBERID FLAG_AUTO_SIZE                   = 0x0018;
const ZIL_NUMBERID DEPTH_OPTIONS                    = 0x0019;
const ZIL_NUMBERID FLAG_NO_DEPTH                    = 0x001A;
const ZIL_NUMBERID FLAG_DEPTH_2                     = 0x001B;
const ZIL_NUMBERID ACTION_OPTIONS                   = 0x001F;
const ZIL_NUMBERID FLAG_NORMAL_ACTION               = 0x0020;
const ZIL_NUMBERID FLAG_DOWN_CLICK                  = 0x0021;
const ZIL_NUMBERID FLAG_DOUBLE_CLICK                = 0x0022;
const ZIL_NUMBERID FLAG_REPEAT_ACTION               = 0x0023;
const ZIL_NUMBERID BUTTON_OK                        = 0x0026;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0027;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0028;
#endif

#if defined(USE_UIW_DATE)
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0001;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0002;
const ZIL_NUMBERID PROMPT_RANGE                     = 0x0003;
const ZIL_NUMBERID FIELD_RANGE                      = 0x0004;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID INPUT_FORMAT_OPTIONS             = 0x000E;
const ZIL_NUMBERID FLAG_SHORT_DAY                   = 0x000F;
const ZIL_NUMBERID FLAG_DAY_OF_WEEK                 = 0x0010;
const ZIL_NUMBERID FLAG_SHORT_MONTH                 = 0x0011;
const ZIL_NUMBERID FLAG_MONTH                       = 0x0012;
const ZIL_NUMBERID FLAG_SHORT_YEAR                  = 0x0013;
const ZIL_NUMBERID FLAG_UPPER_CASE                  = 0x0014;
const ZIL_NUMBERID FLAG_ZERO_FILL                   = 0x0015;
const ZIL_NUMBERID FLAG_SYSTEM                      = 0x0016;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x0017;
const ZIL_NUMBERID SEPARATOR_OPTIONS                = 0x0018;
const ZIL_NUMBERID FLAG_DEFAULT_SEPARATOR           = 0x0019;
const ZIL_NUMBERID FLAG_DASH_SEPARATOR              = 0x001A;
const ZIL_NUMBERID FLAG_SLASH_SEPARATOR             = 0x001B;
const ZIL_NUMBERID COUNTRY_OPTIONS                  = 0x001C;
const ZIL_NUMBERID FLAG_DEFAULT_FORMAT              = 0x001D;
const ZIL_NUMBERID FLAG_EUROPEAN_FORMAT             = 0x001E;
const ZIL_NUMBERID FLAG_ASIAN_FORMAT                = 0x001F;
const ZIL_NUMBERID FLAG_MILITARY_FORMAT             = 0x0020;
const ZIL_NUMBERID FLAG_US_FORMAT                   = 0x0021;
const ZIL_NUMBERID BUTTON_OK                        = 0x0022;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0023;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0024;
#endif

#if defined(USE_UIW_FORMATTED_STRING)
const ZIL_NUMBERID PROMPT_TEXT                      = 0x0001;
const ZIL_NUMBERID FIELD_TEXT                       = 0x0002;
const ZIL_NUMBERID PROMPT_EDIT                      = 0x0003;
const ZIL_NUMBERID FIELD_EDIT                       = 0x0004;
const ZIL_NUMBERID PROMPT_DELETE                    = 0x0005;
const ZIL_NUMBERID FIELD_DELETE                     = 0x0006;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0007;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0008;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0009;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x000A;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000B;
const ZIL_NUMBERID FIELD_85                         = 0x0055;
const ZIL_NUMBERID INPUT_CONVERSION_OPTION          = 0x0013;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x0015;
const ZIL_NUMBERID BUTTON_OK                        = 0x000F;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0010;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0011;
#endif

#if defined(USE_UIW_GROUP)
const ZIL_NUMBERID TEXT_PROMPT                      = 0x0001;
const ZIL_NUMBERID TEXT_FIELD                       = 0x0002;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0003;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0004;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0005;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0006;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0007;
const ZIL_NUMBERID OPTION_FLAGS                     = 0x0014;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0009;
const ZIL_NUMBERID FLAG_AUTO_SELECT                 = 0x000A;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x000B;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x005A;
const ZIL_NUMBERID BUTTON_OK                        = 0x0008;
const ZIL_NUMBERID CANCEL_BUTTON                    = 0x0009;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000A;
#endif

#if defined(USE_UIW_ICON)
const ZIL_NUMBERID PROMPT_TITLE                     = 0x0001;
const ZIL_NUMBERID FIELD_TITLE                      = 0x0002;
const ZIL_NUMBERID PROMPT_IMAGE                     = 0x0003;
const ZIL_NUMBERID FIELD_IMAGE                      = 0x0004;
const ZIL_NUMBERID LIST_IMAGE                       = 0x0005;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0006;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0007;
const ZIL_NUMBERID PROMPT_HELP                      = 0x0008;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0009;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000A;
const ZIL_NUMBERID FLAGS                            = 0x000E;
const ZIL_NUMBERID OPTION_ACTION                    = 0x000F;
const ZIL_NUMBERID FLAG_UP_CLICK                    = 0x0010;
const ZIL_NUMBERID FLAG_DOUBLE_CLICK                = 0x0011;
const ZIL_NUMBERID BUTTON_OK                        = 0x0012;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0013;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0014;
#endif

#if defined(USE_UIW_INTEGER)
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0001;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0002;
const ZIL_NUMBERID PROMPT_RANGE                     = 0x0003;
const ZIL_NUMBERID FIELD_RANGE                      = 0x0004;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID INPUT_CONVERSION_OPTION          = 0x000E;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x000F;
const ZIL_NUMBERID BUTTON_OK                        = 0x0010;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0011;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0012;
#endif

#if defined(USE_UIW_MAXIMIZE_BUTTON)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID BUTTON_OK                        = 0x0003;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0004;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0005;
#endif

#if defined(USE_UIW_MINIMIZE_BUTTON)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID BUTTON_OK                        = 0x0003;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0004;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0005;
#endif

#if defined(USE_UIW_NOTEBOOK)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0003;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0004;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0005;
const ZIL_NUMBERID BUTTON_OK                        = 0x000B;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000C;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000D;
#endif

#if defined(USE_UIW_POP_UP_MENU)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0003;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0004;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0005;
const ZIL_NUMBERID LIST_FLAGS                       = 0x0006;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0007;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x0008;
const ZIL_NUMBERID BUTTON_OK                        = 0x0009;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000A;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000B;
#endif

#if defined(USE_UIW_PROMPT)
const ZIL_NUMBERID PROMPT_TEXT                      = 0x0001;
const ZIL_NUMBERID FIELD_TEXT                       = 0x0002;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0003;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0004;
const ZIL_NUMBERID BUTTON_OK                        = 0x0005;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0006;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0007;
#endif

#if defined(USE_UIW_PULL_DOWN_MENU)
const ZIL_NUMBERID PROMPT_INDENTATION               = 0x0006;
const ZIL_NUMBERID FIELD_INDENTATION                = 0x0007;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0003;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0004;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0005;
const ZIL_NUMBERID LIST_FLAGS                       = 0x0008;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0009;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x000A;
const ZIL_NUMBERID BUTTON_OK                        = 0x000D;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000E;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000F;
#endif

#if defined(USE_UIW_REAL)
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0001;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0002;
const ZIL_NUMBERID PROMPT_PRECISION                 = 0x0072;
const ZIL_NUMBERID FIELD_PRECISION                  = 0x0073;
const ZIL_NUMBERID PROMPT_PRECISION_RANGE           = 0x0074;
const ZIL_NUMBERID PROMPT_RANGE                     = 0x0003;
const ZIL_NUMBERID FIELD_RANGE                      = 0x0004;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID INPUT_CONVERSION_OPTION          = 0x000E;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x000F;
const ZIL_NUMBERID FLAG_SCIENTIFIC                  = 0x0075;
const ZIL_NUMBERID BUTTON_OK                        = 0x0010;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0011;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0012;
#endif

#if defined(USE_UIW_SPIN_CONTROL)
const ZIL_NUMBERID PROMPT_SPIN_TYPE                 = 0x0001;
const ZIL_NUMBERID FIELD_SPIN_TYPE                  = 0x0002;
const ZIL_NUMBERID LIST_ADD_OBJECT                  = 0x0003;
const ZIL_NUMBERID PROMPT_DELTA                     = 0x0004;
const ZIL_NUMBERID FIELD_DELTA                      = 0x0005;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0006;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0007;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0008;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0009;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000A;
const ZIL_NUMBERID OPTION_FLAGS                     = 0x00A4;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x00A8;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x00AB;
const ZIL_NUMBERID BUTTON_OK                        = 0x000E;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000F;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0010;
const ZIL_NUMBERID BUTTON_EDIT                      = 0x008F;
#endif

#if defined(USE_UIW_STATUS_BAR)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0003;
const ZIL_NUMBERID HEIGHT_PROMPT                    = 0x0001;
const ZIL_NUMBERID HEIGHT_FIELD                     = 0x0002;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0004;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0005;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0006;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0007;
const ZIL_NUMBERID BUTTON_OK                        = 0x000D;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000E;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000F;
#endif

#if defined(USE_UIW_SYSTEM_BUTTON)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID LIST_FLAGS                       = 0x0003;
const ZIL_NUMBERID OPTIONS_EDIT                     = 0x0004;
const ZIL_NUMBERID FLAG_GENERIC                     = 0x0005;
const ZIL_NUMBERID BUTTON_OK                        = 0x0006;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0007;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0008;
#endif

#if defined(USE_UIW_TABLE)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0004;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0005;
const ZIL_NUMBERID PROMPT_HELP                      = 0x0006;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0007;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0008;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000C;
const ZIL_NUMBERID SUPPORT_FEATURES_OPTION          = 0x0010;
const ZIL_NUMBERID FLAG_VT_SCROLL                   = 0x0014;
const ZIL_NUMBERID FLAG_HZ_SCROLL                   = 0x0015;
const ZIL_NUMBERID FLAG_COL_HEADER                  = 0x0090;
const ZIL_NUMBERID FLAG_ROW_HEADER                  = 0x008F;
const ZIL_NUMBERID OPTION_FEATURES                  = 0x0030;
const ZIL_NUMBERID FLAG_GRID                        = 0x0031;
const ZIL_NUMBERID BUTTON_OK                        = 0x0016;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0017;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0018;
const ZIL_NUMBERID PROMPT_COLUMNS                   = 0x00A1;
const ZIL_NUMBERID FIELD_COLUMNS                    = 0x00A2;
const ZIL_NUMBERID PROMPT_SIZE                      = 0x00A3;
const ZIL_NUMBERID FIELD_REC_SIZE                   = 0x00A4;
#endif

#if defined(USE_UIW_TABLE_HEADER)
const ZIL_NUMBERID PROMPT_HSTRINGID                 = 0x0001;
const ZIL_NUMBERID FIELD_HSTRINGID                  = 0x0002;
const ZIL_NUMBERID BUTTON_HOK                       = 0x0003;
const ZIL_NUMBERID BUTTON_HCANCEL                   = 0x0004;
const ZIL_NUMBERID BUTTON_HHELP                     = 0x0005;
#endif

#if defined(USE_UIW_TIME)
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0001;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0002;
const ZIL_NUMBERID PROMPT_RANGE                     = 0x0003;
const ZIL_NUMBERID FIELD_RANGE                      = 0x0004;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID INPUT_FORMAT_OPTIONS             = 0x000E;
const ZIL_NUMBERID FLAG_HUNDREDTHS                  = 0x000F;
const ZIL_NUMBERID FLAG_SECONDS                     = 0x0010;
const ZIL_NUMBERID FLAG_NO_MINUTES                  = 0x0011;
const ZIL_NUMBERID FLAG_NO_HOURS                    = 0x0012;
const ZIL_NUMBERID FLAG_ZERO_FILL                   = 0x0015;
const ZIL_NUMBERID FLAG_SYSTEM                      = 0x0016;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x0017;
const ZIL_NUMBERID SEPARATOR_OPTIONS                = 0x0018;
const ZIL_NUMBERID FLAG_DEFAULT_SEPARATOR           = 0x0019;
const ZIL_NUMBERID FLAG_NO_SEPARATOR                = 0x001A;
const ZIL_NUMBERID FLAG_COLON_SEPARATOR             = 0x001B;
const ZIL_NUMBERID CASE_OPTION                      = 0x0061;
const ZIL_NUMBERID FLAG_NORMAL_CASE                 = 0x0060;
const ZIL_NUMBERID FLAG_LOWER_CASE                  = 0x0013;
const ZIL_NUMBERID FLAG_UPPER_CASE                  = 0x0014;
const ZIL_NUMBERID COUNTRY_OPTIONS                  = 0x001C;
const ZIL_NUMBERID FLAG_DEFAULT_FORMAT              = 0x001D;
const ZIL_NUMBERID FLAG_24_HOUR                     = 0x001E;
const ZIL_NUMBERID FLAG_12_HOUR                     = 0x001F;
const ZIL_NUMBERID BUTTON_OK                        = 0x0020;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0021;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0022;
#endif

#if defined(USE_UIW_TITLE)
const ZIL_NUMBERID PROMPT_TITLE                     = 0x0001;
const ZIL_NUMBERID FIELD_TITLE                      = 0x0002;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0003;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0004;
const ZIL_NUMBERID BUTTON_OK                        = 0x0005;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0006;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0007;
#endif

#if defined(USE_UIW_TOOL_BAR)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0003;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0004;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0005;
const ZIL_NUMBERID LIST_FLAGS                       = 0x0006;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0007;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x0008;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x0009;
const ZIL_NUMBERID BUTTON_OK                        = 0x000B;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x000C;
const ZIL_NUMBERID BUTTON_HELP                      = 0x000D;
#endif

#if defined(USE_UI_WINDOW_OBJECT)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0004;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0005;
const ZIL_NUMBERID BUTTON_OK                        = 0x0001;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0002;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0003;
#endif

#if defined(USE_ZAF_DERIVED_OBJECT)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0051;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0052;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0053;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0054;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000C;
const ZIL_NUMBERID BUTTON_OK                        = 0x0014;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0015;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0016;
#endif

#if defined(USE_UIW_VT_LIST)
const ZIL_NUMBERID PROMPT_COMPARE_FUNCTION          = 0x0006;
const ZIL_NUMBERID FIELD_COMPARE_FUNCTION           = 0x007E;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0003;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0004;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0005;
const ZIL_NUMBERID LIST_FLAGS                       = 0x0009;
const ZIL_NUMBERID SUPPORT_FEATURE_OPTION           = 0x000A;
const ZIL_NUMBERID FLAG_VERTICAL_SCROLL_BAR         = 0x000B;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x000C;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x000D;
const ZIL_NUMBERID FLAG_BITMAP_CHILDREN             = 0x000E;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x000F;
const ZIL_NUMBERID FLAG_AUTO_SELECT                 = 0x0010;
const ZIL_NUMBERID FLAG_CONTINUE_SELECT             = 0x0011;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x0012;
const ZIL_NUMBERID BUTTON_OK                        = 0x0013;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0014;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0015;
#endif

#if defined(USE_UIW_HZ_LIST)
const ZIL_NUMBERID PROMPT_CELL_WIDTH                = 0x0001;
const ZIL_NUMBERID FIELD_CELL_WIDTH                 = 0x0002;
const ZIL_NUMBERID PROMPT_CELL_HEIGHT               = 0x0003;
const ZIL_NUMBERID FIELD_CELL_HEIGHT                = 0x0004;
const ZIL_NUMBERID PROMPT_COMPARE_FUNCTION          = 0x000A;
const ZIL_NUMBERID FIELD_COMPARE_FUNCTION           = 0x00C3;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID SUPPORT_FEATURE_OPTION           = 0x000E;
const ZIL_NUMBERID FLAG_HORIZONTAL_SCROLL_BAR       = 0x000F;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0010;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x0011;
const ZIL_NUMBERID FLAG_BITMAP_CHILDREN             = 0x0012;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x0013;
const ZIL_NUMBERID FLAG_AUTO_SELECT                 = 0x0014;
const ZIL_NUMBERID FLAG_CONTINUE_SELECT             = 0x0015;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x0016;
const ZIL_NUMBERID BUTTON_OK                        = 0x0017;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0018;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0019;
#endif

#if defined(USE_UIW_WINDOW)
const ZIL_NUMBERID PROMPT_TITLE                     = 0x0001;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0006;
const ZIL_NUMBERID FIELD_TITLE                      = 0x0002;
const ZIL_NUMBERID PROMPT_MINICON                   = 0x0003;
const ZIL_NUMBERID FIELD_MINICON                    = 0x0004;
const ZIL_NUMBERID LIST_MINICON                     = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0007;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0008;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0009;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000A;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000B;
const ZIL_NUMBERID SUPPORT_FEATURES_OPTION          = 0x000C;
const ZIL_NUMBERID FLAG_TITLE                       = 0x012E;
const ZIL_NUMBERID FLAG_BORDER                      = 0x000D;
const ZIL_NUMBERID FLAG_MAX_BUTTON                  = 0x000E;
const ZIL_NUMBERID FLAG_MIN_BUTTON                  = 0x000F;
const ZIL_NUMBERID FLAG_SYS_BUTTON                  = 0x0010;
const ZIL_NUMBERID FLAG_GEOMETRY_MANAGER            = 0x003E;
const ZIL_NUMBERID FLAG_VT_SCROLL                   = 0x0011;
const ZIL_NUMBERID FLAG_HZ_SCROLL                   = 0x0012;
const ZIL_NUMBERID TYPE_OPTION                      = 0x0013;
const ZIL_NUMBERID FLAG_DEFAULT_OBJECT              = 0x0014;
const ZIL_NUMBERID FLAG_DIALOG_OBJECT               = 0x0016;
const ZIL_NUMBERID FLAG_MDI_OBJECT                  = 0x0015;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0017;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x0018;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x0019;
const ZIL_NUMBERID FLAG_CONTINUE_SELECT             = 0x001B;
const ZIL_NUMBERID FLAG_AUTO_SELECT                 = 0x0120;
const ZIL_NUMBERID FLAG_NORMAL_HOTKEYS              = 0x00AA;
const ZIL_NUMBERID FLAG_NO_SIZE                     = 0x001C;
const ZIL_NUMBERID FLAG_NO_MOVE                     = 0x001D;
const ZIL_NUMBERID FLAG_MODAL                       = 0x001E;
const ZIL_NUMBERID FLAG_LOCKED                      = 0x001F;
const ZIL_NUMBERID FLAG_TEMPORARY                   = 0x009B;
const ZIL_NUMBERID FLAG_NO_DESTROY                  = 0x009C;
const ZIL_NUMBERID OPTION_SPECIAL                   = 0x0020;
const ZIL_NUMBERID FLAG_RENUMBER_OBJECTS            = 0x0021;
const ZIL_NUMBERID BUTTON_OK                        = 0x0022;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0023;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0024;
#endif

#if defined(USE_OBJ_ADVANCED)
const ZIL_NUMBERID PROMPT_NUMBERID                  = 0x0001;
const ZIL_NUMBERID FIELD_NUMBERID                   = 0x0002;
const ZIL_NUMBERID PROMPT_CALLBACK                  = 0x000C;
const ZIL_NUMBERID FIELD_CALLBACK                   = 0x00DA;
const ZIL_NUMBERID PROMPT_USER_OBJECT               = 0x0006;
const ZIL_NUMBERID FIELD_USER_OBJECT                = 0x0007;
const ZIL_NUMBERID PROMPT_USER_FLAGS                = 0x0008;
const ZIL_NUMBERID FIELD_USER_FLAGS                 = 0x0009;
const ZIL_NUMBERID PROMPT_USER_STATUS               = 0x000A;
const ZIL_NUMBERID FIELD_USER_STATUS                = 0x000B;
const ZIL_NUMBERID PROMPT_DERIVED_OBJECT            = 0x0003;
const ZIL_NUMBERID FIELD_DERIVED_OBJECT             = 0x00E9;
const ZIL_NUMBERID FLAGS                            = 0x000F;
const ZIL_NUMBERID OPTIONS_EDIT                     = 0x0010;
const ZIL_NUMBERID FLAG_NO_FLAGS                    = 0x0011;
const ZIL_NUMBERID FLAG_VIEW_ONLY                   = 0x0012;
const ZIL_NUMBERID FLAG_NON_CURRENT                 = 0x0014;
const ZIL_NUMBERID FLAG_NON_SELECTABLE_GRAY         = 0x0013;
const ZIL_NUMBERID FLAG_NON_SELECTABLE_BLACK        = 0x00CF;
const ZIL_NUMBERID DATA_OPTIONS                     = 0x00BA;
const ZIL_NUMBERID FLAG_INVALID                     = 0x0016;
const ZIL_NUMBERID FLAG_UNANSWERED                  = 0x0017;
const ZIL_NUMBERID MISC_OPTIONS                     = 0x0018;
const ZIL_NUMBERID FLAG_NO_DEALLOCATE               = 0x0033;
const ZIL_NUMBERID FLAG_MOVE_DRAG_OBJECT            = 0x001A;
const ZIL_NUMBERID FLAG_COPY_DRAG_OBJECT            = 0x00DC;
const ZIL_NUMBERID FLAG_DROP_OBJECT                 = 0x00B9;
const ZIL_NUMBERID FLAG_SUPPORT_OBJECT              = 0x0085;
const ZIL_NUMBERID BUTTON_OK                        = 0x001D;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x001E;
const ZIL_NUMBERID BUTTON_HELP                      = 0x001F;
#endif

#if defined(USE_UIW_TABLE_RECORD)
const ZIL_NUMBERID PROMPT_RSTRINGID                 = 0x0001;
const ZIL_NUMBERID FIELD_RSTRINGID                  = 0x0002;
const ZIL_NUMBERID BUTTON_ROK                       = 0x0003;
const ZIL_NUMBERID BUTTON_RCANCEL                   = 0x0004;
const ZIL_NUMBERID BUTTON_RHELP                     = 0x0005;
const ZIL_NUMBERID OPTION_FLAGS                     = 0x0013;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0017;
const ZIL_NUMBERID FLAG_AUTO_SELECT                 = 0x0018;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x0019;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x001A;
#endif

#if defined(USE_UIW_STRING)
const ZIL_NUMBERID PROMPT_TEXT                      = 0x0001;
const ZIL_NUMBERID FIELD_TEXT                       = 0x0002;
const ZIL_NUMBERID PROMPT_MAXLENGTH                 = 0x0003;
const ZIL_NUMBERID FIELD_MAXLENGTH                  = 0x0004;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID INPUT_FORMAT_OPTIONS             = 0x000E;
const ZIL_NUMBERID FLAG_NO_FLAGS                    = 0x000F;
const ZIL_NUMBERID FLAG_LOWER_CASE                  = 0x0010;
const ZIL_NUMBERID FLAG_STF_UPPER_CASE              = 0x0011;
const ZIL_NUMBERID FLAG_PASSWORD                    = 0x0012;
const ZIL_NUMBERID INPUT_CONVERSION_OPTION          = 0x0013;
const ZIL_NUMBERID FLAG_VARIABLE_NAME               = 0x0014;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x0015;
const ZIL_NUMBERID BUTTON_OK                        = 0x0016;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0017;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0018;
const ZIL_NUMBERID FIELD_115                        = 0x0073;
#endif

#if defined(USE_UIW_TEXT)
const ZIL_NUMBERID PROMPT_TEXT                      = 0x0001;
const ZIL_NUMBERID FIELD_TEXT                       = 0x0002;
const ZIL_NUMBERID PROMPT_MAXLENGTH                 = 0x0003;
const ZIL_NUMBERID FIELD_MAXLENGTH                  = 0x0004;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID FLAGS                            = 0x000D;
const ZIL_NUMBERID SUPPORT_FEATURES                 = 0x000E;
const ZIL_NUMBERID FLAG_VERTICAL_SCROLL_BAR         = 0x000F;
const ZIL_NUMBERID FLAG_HORIZONTAL_SCROLL_BAR       = 0x0010;
const ZIL_NUMBERID INPUT_FORMAT                     = 0x0011;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x0012;
const ZIL_NUMBERID FLAG_AUTO_CLEAR                  = 0x0062;
const ZIL_NUMBERID BUTTON_OK                        = 0x0013;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0014;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0015;
const ZIL_NUMBERID FIELD_133                        = 0x0085;
#endif

#if defined(USE_UIW_PULL_DOWN_ITEM)
const ZIL_NUMBERID PROMPT_TEXT                      = 0x0001;
const ZIL_NUMBERID FIELD_TEXT                       = 0x0002;
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0003;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0004;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x000E;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x000F;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x0011;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x0012;
const ZIL_NUMBERID FLAG_SEND_MESSAGE                = 0x00BC;
const ZIL_NUMBERID BUTTON_OK                        = 0x0015;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0016;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0017;
#endif

#if defined(USE_UIW_SCROLL_BAR)
const ZIL_NUMBERID PROMPT_MINIMUM                   = 0x0039;
const ZIL_NUMBERID FIELD_MINIMUM                    = 0x003C;
const ZIL_NUMBERID PROMPT_MAXIMUM                   = 0x003A;
const ZIL_NUMBERID FIELD_MAXIMUM                    = 0x003D;
const ZIL_NUMBERID PROMPT_CURRENT                   = 0x003B;
const ZIL_NUMBERID FIELD_CURRENT                    = 0x003E;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0001;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0002;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0003;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0004;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0005;
const ZIL_NUMBERID LIST_FLAGS                       = 0x0009;
const ZIL_NUMBERID TYPE_OPTIONS                     = 0x000A;
const ZIL_NUMBERID FLAG_VT_SLIDER                   = 0x008C;
const ZIL_NUMBERID FLAG_HZ_SLIDER                   = 0x000F;
const ZIL_NUMBERID FLAG_VERTICAL_SCROLL_BAR         = 0x000B;
const ZIL_NUMBERID FLAG_HORIZONTAL_SCROLL_BAR       = 0x000C;
const ZIL_NUMBERID FLAG_CORNER_SCROLL_BAR           = 0x000D;
const ZIL_NUMBERID BUTTON_OK                        = 0x0010;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0011;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0012;
#endif

#if defined(USE_MSG_TABLE)
const ZIL_NUMBERID ZMSG_SUPPORT_OBJECTS             = 0x0000;
const ZIL_NUMBERID ZMSG_NORMAL_OBJECTS              = 0x0001;
const ZIL_NUMBERID ZMSG_PIXEL                       = 0x0002;
const ZIL_NUMBERID ZMSG_MINICELL                    = 0x0003;
const ZIL_NUMBERID ZMSG_CELL                        = 0x0004;
const ZIL_NUMBERID ZMSG_ERROR_DRIVE                 = 0x0064;
const ZIL_NUMBERID ZMSG_SAVE_TITLE                  = 0x0065;
const ZIL_NUMBERID ZMSG_SAVE_MESSAGE                = 0x0066;
#endif

#if defined(USE_ZAF_GEOMETRY_MANAGER)
const ZIL_NUMBERID GROUP_OPTIONS                    = 0x0002;
const ZIL_NUMBERID FIELD_SIDE                       = 0x0003;
const ZIL_NUMBERID FIELD_SIDE_LIST                  = 0x0004;
const ZIL_NUMBERID FIELD_SIDE_LEFT                  = 0x0005;
const ZIL_NUMBERID FIELD_SIDE_TOP                   = 0x0006;
const ZIL_NUMBERID FIELD_SIDE_RIGHT                 = 0x0007;
const ZIL_NUMBERID FIELD_SIDE_BOTTOM                = 0x0008;
const ZIL_NUMBERID FIELD_TYPE                       = 0x0009;
const ZIL_NUMBERID FIELD_TYPE_NONE                  = 0x000A;
const ZIL_NUMBERID FIELD_TYPE_ABSOLUTE              = 0x000B;
const ZIL_NUMBERID FIELD_TYPE_RELATIVE              = 0x000C;
const ZIL_NUMBERID PROMPT_ANCHOR                    = 0x000D;
const ZIL_NUMBERID FIELD_ANCHOR                     = 0x000E;
const ZIL_NUMBERID FIELD_LANCHOR                    = 0x000F;
const ZIL_NUMBERID PROMPT_OFFSET                    = 0x0013;
const ZIL_NUMBERID FIELD_OFFSET                     = 0x0014;
const ZIL_NUMBERID FIELD_OPTIONS                    = 0x0015;
const ZIL_NUMBERID FIELD_STRETCH                    = 0x0016;
const ZIL_NUMBERID FIELD_OPPOSITE                   = 0x0017;
const ZIL_NUMBERID FIELD_HZ_CENTER                  = 0x0018;
const ZIL_NUMBERID FIELD_VT_CENTER                  = 0x0019;
const ZIL_NUMBERID GROUP_SIZE                       = 0x001A;
const ZIL_NUMBERID PROMPT_MINX                      = 0x001C;
const ZIL_NUMBERID FIELD_MINX                       = 0x001D;
const ZIL_NUMBERID PROMPT_MAXX                      = 0x001E;
const ZIL_NUMBERID FIELD_MINY                       = 0x001F;
const ZIL_NUMBERID PROMPT_MINY                      = 0x0021;
const ZIL_NUMBERID FIELD_MAXX                       = 0x0022;
const ZIL_NUMBERID PROMPT_MAXY                      = 0x0023;
const ZIL_NUMBERID FIELD_MAXY                       = 0x0024;
const ZIL_NUMBERID FIELD_WIDTH                      = 0x0036;
const ZIL_NUMBERID FIELD_HEIGHT                     = 0x0037;
const ZIL_NUMBERID BUTTON_OK                        = 0x0025;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0026;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0027;
#endif

#if defined(USE_ZAF_WINDOW_EDITOR)
const ZIL_NUMBERID MENU                             = 0x0007;
const ZIL_NUMBERID FILE                             = 0x0001;
const ZIL_NUMBERID FIELD_33                         = 0x0021;
const ZIL_NUMBERID FILE_NEW                         = 0x0022;
const ZIL_NUMBERID FILE_OPEN                        = 0x0023;
const ZIL_NUMBERID FILE_SAVE                        = 0x0024;
const ZIL_NUMBERID FILE_SAVEAS                      = 0x0025;
const ZIL_NUMBERID FILE_CLOSE                       = 0x0073;
const ZIL_NUMBERID FILE_SEP1                        = 0x0026;
const ZIL_NUMBERID FILE_DELETE                      = 0x0027;
const ZIL_NUMBERID FILE_SEP2                        = 0x0028;
const ZIL_NUMBERID FILE_PREFERENCES                 = 0x009E;
const ZIL_NUMBERID FILE_SEP3                        = 0x009F;
const ZIL_NUMBERID FILE_EXIT                        = 0x002B;
const ZIL_NUMBERID EDIT                             = 0x0002;
const ZIL_NUMBERID FIELD_45                         = 0x002D;
const ZIL_NUMBERID EDIT_OBJECT                      = 0x002E;
const ZIL_NUMBERID EDIT_SEP1                        = 0x002F;
const ZIL_NUMBERID EDIT_CUT                         = 0x0030;
const ZIL_NUMBERID EDIT_COPY                        = 0x0031;
const ZIL_NUMBERID EDIT_PASTE                       = 0x0032;
const ZIL_NUMBERID EDIT_DELETE                      = 0x0033;
const ZIL_NUMBERID EDIT_SEP2                        = 0x0000;
const ZIL_NUMBERID EDIT_MOVE                        = 0x0000;
const ZIL_NUMBERID EDIT_SIZE                        = 0x0000;
const ZIL_NUMBERID EDIT_SEP3                        = 0x0000;
const ZIL_NUMBERID EDIT_GROUP_DEFINE                = 0x0000;
const ZIL_NUMBERID EDIT_GROUP_UNDEFINE              = 0x0000;
const ZIL_NUMBERID TOOLS                            = 0x00CA;
const ZIL_NUMBERID WINDOW                           = 0x0068;
const ZIL_NUMBERID WINDOW_IMPORT                    = 0x0097;
const ZIL_NUMBERID WINDOW_EXPORT                    = 0x00AB;
const ZIL_NUMBERID WINDOW_SEP1                      = 0x0099;
const ZIL_NUMBERID WINDOW_CREATE                    = 0x0066;
const ZIL_NUMBERID WINDOW_LOAD                      = 0x0067;
const ZIL_NUMBERID WINDOW_STORE                     = 0x006F;
const ZIL_NUMBERID WINDOW_STOREAS                   = 0x0070;
const ZIL_NUMBERID WINDOW_SEP2                      = 0x0071;
const ZIL_NUMBERID WINDOW_CLEAR                     = 0x009A;
const ZIL_NUMBERID WINDOW_CLEARALL                  = 0x009B;
const ZIL_NUMBERID WINDOW_DELETE                    = 0x0072;
const ZIL_NUMBERID WINDOW_SEP3                      = 0x00AC;
const ZIL_NUMBERID WINDOW_TEST                      = 0x00AD;
const ZIL_NUMBERID OBJECT                           = 0x0095;
const ZIL_NUMBERID FIELD_156                        = 0x009C;
const ZIL_NUMBERID OBJECT_INPUT                     = 0x0074;
const ZIL_NUMBERID UIW_STRING                       = 0x0078;
const ZIL_NUMBERID UIW_FORMATTED_STRING             = 0x0079;
const ZIL_NUMBERID UIW_TEXT                         = 0x007A;
const ZIL_NUMBERID INPUT_SEP1                       = 0x007B;
const ZIL_NUMBERID UIW_DATE                         = 0x007C;
const ZIL_NUMBERID UIW_TIME                         = 0x007D;
const ZIL_NUMBERID INPUT_SEP2                       = 0x007E;
const ZIL_NUMBERID UIW_BIGNUM                       = 0x007F;
const ZIL_NUMBERID UIW_INTEGER                      = 0x0080;
const ZIL_NUMBERID UIW_REAL                         = 0x0081;
const ZIL_NUMBERID OBJECT_CONTROL                   = 0x0000;
const ZIL_NUMBERID UIW_BUTTON                       = 0x0000;
const ZIL_NUMBERID UIW_RADIO_BUTTON                 = 0x0000;
const ZIL_NUMBERID UIW_CHECK_BOX                    = 0x0000;
const ZIL_NUMBERID CONTROL_SEP1                     = 0x0000;
const ZIL_NUMBERID UIW_HZ_SCROLL_BAR                = 0x0000;
const ZIL_NUMBERID UIW_VT_SCROLL_BAR                = 0x0000;
const ZIL_NUMBERID CONTROL_SEP2                     = 0x00B7;
const ZIL_NUMBERID UIW_COMBO_BOX                    = 0x00B8;
const ZIL_NUMBERID UIW_SPIN_CONTROL                 = 0x00B9;
const ZIL_NUMBERID CONTROL_SEP3                     = 0x00BA;
const ZIL_NUMBERID UIW_PULL_DOWN_ITEM               = 0x00BC;
const ZIL_NUMBERID OBJECT_SELECTION                 = 0x0075;
const ZIL_NUMBERID UIW_HZ_LIST                      = 0x0086;
const ZIL_NUMBERID UIW_VT_LIST                      = 0x0087;
const ZIL_NUMBERID CONTROL_SEP1                     = 0x0089;
const ZIL_NUMBERID UIW_TOOL_BAR                     = 0x0088;
const ZIL_NUMBERID CONTROL_SEP2                     = 0x00BD;
const ZIL_NUMBERID UIW_PULL_DOWN_MENU               = 0x008D;
const ZIL_NUMBERID OBJECT_OTHER                     = 0x0077;
const ZIL_NUMBERID UIW_PROMPT                       = 0x0091;
const ZIL_NUMBERID UIW_GROUP                        = 0x0092;
const ZIL_NUMBERID UIW_ICON                         = 0x0094;
const ZIL_NUMBERID OTHER_SEP1                       = 0x0093;
const ZIL_NUMBERID UIW_STATUS_BAR                   = 0x00C0;
const ZIL_NUMBERID UIW_NOTEBOOK                     = 0x00C1;
const ZIL_NUMBERID UIW_TABLE                        = 0x00C2;
const ZIL_NUMBERID OTHER_SEP2                       = 0x00C3;
const ZIL_NUMBERID UIW_WINDOW                       = 0x00BF;
const ZIL_NUMBERID ZAF_DERIVED_OBJECT               = 0x00C6;
const ZIL_NUMBERID OBJECT_SEP1                      = 0x00C9;
const ZIL_NUMBERID OBJECT_CLEAR                     = 0x00C7;
const ZIL_NUMBERID OBJECT_RESET                     = 0x00C8;
const ZIL_NUMBERID HELP                             = 0x0096;
const ZIL_NUMBERID HELP_INDEX                       = 0x00AE;
const ZIL_NUMBERID HELP_SEP1                        = 0x00AF;
const ZIL_NUMBERID HELP_FILE                        = 0x00B0;
const ZIL_NUMBERID HELP_EDIT                        = 0x00B1;
const ZIL_NUMBERID HELP_WINDOW                      = 0x00B2;
const ZIL_NUMBERID HELP_OBJECT                      = 0x00B3;
const ZIL_NUMBERID HELP_SEP2                        = 0x00B4;
const ZIL_NUMBERID HELP_ABOUT                       = 0x00B5;
const ZIL_NUMBERID FIELD_100                        = 0x0064;
const ZIL_NUMBERID STATUS_BAR                       = 0x00A0;
const ZIL_NUMBERID OBJECT_PROMPT                    = 0x00A1;
const ZIL_NUMBERID OBJECT_FIELD                     = 0x01F4;
const ZIL_NUMBERID STRINGID_PROMPT                  = 0x00A3;
const ZIL_NUMBERID STRINGID_FIELD                   = 0x01F5;
const ZIL_NUMBERID POSITION_PROMPT                  = 0x00A5;
const ZIL_NUMBERID POSITION_FIELD                   = 0x01F6;
const ZIL_NUMBERID SIZE_PROMPT                      = 0x00A7;
const ZIL_NUMBERID SIZE_FIELD                       = 0x01F7;
const ZIL_NUMBERID PLACE_OBJECT_PROMPT              = 0x00A9;
const ZIL_NUMBERID PLACE_OBJECT_FIELD               = 0x01F8;
#endif

#if defined(USE_UIW_COMBO_BOX)
const ZIL_NUMBERID PROMPT_LIST_HEIGHT               = 0x0004;
const ZIL_NUMBERID FIELD_LIST_HEIGHT                = 0x0005;
const ZIL_NUMBERID PROMPT_LIST_NAME                 = 0x0006;
const ZIL_NUMBERID FIELD_LIST_NAME                  = 0x0007;
const ZIL_NUMBERID PROMPT_LIST_ID                   = 0x00EC;
const ZIL_NUMBERID FIELD_LIST_ID                    = 0x00ED;
const ZIL_NUMBERID PROMPT_COMPARE_FUNCTION          = 0x0001;
const ZIL_NUMBERID FIELD_COMPARE_FUNCTION           = 0x00C2;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0008;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0009;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x000A;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x000B;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x000C;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID SUPPORT_FEATURE_OPTION           = 0x000E;
const ZIL_NUMBERID FLAG_VERTICAL_SCROLL_BAR         = 0x000F;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x0010;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x0011;
const ZIL_NUMBERID FLAG_BITMAP_CHILDREN             = 0x0012;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x0013;
const ZIL_NUMBERID BUTTON_OK                        = 0x0014;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0015;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0016;
#endif

#if defined(USE_UIW_POP_UP_ITEM)
const ZIL_NUMBERID PROMPT_TEXT                      = 0x0001;
const ZIL_NUMBERID FIELD_TEXT                       = 0x0002;
const ZIL_NUMBERID PROMPT_VALUE                     = 0x0003;
const ZIL_NUMBERID FIELD_VALUE                      = 0x0004;
const ZIL_NUMBERID PROMPT_MENU_NAME                 = 0x00C3;
const ZIL_NUMBERID FIELD_MENU_NAME                  = 0x00C4;
const ZIL_NUMBERID PROMPT_MENU_ID                   = 0x00CD;
const ZIL_NUMBERID FIELD_MENU_ID                    = 0x00CE;
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID PROMPT_HELP_CONTEXT              = 0x0007;
const ZIL_NUMBERID FIELD_HELP_CONTEXT               = 0x0008;
const ZIL_NUMBERID LIST_HELP_CONTEXT                = 0x0009;
const ZIL_NUMBERID LIST_FLAGS                       = 0x000D;
const ZIL_NUMBERID ITEM_OPTIONS                     = 0x0078;
const ZIL_NUMBERID FLAG_SEPARATOR                   = 0x0079;
const ZIL_NUMBERID FLAG_CHECK_MARK                  = 0x0013;
const ZIL_NUMBERID FLAG_SEND_MESSAGE                = 0x0014;
const ZIL_NUMBERID INTERACTION_OPTIONS              = 0x000E;
const ZIL_NUMBERID FLAG_AUTO_SORT                   = 0x000F;
const ZIL_NUMBERID FLAG_NO_WRAP                     = 0x0011;
const ZIL_NUMBERID FLAG_SELECT_MULTIPLE             = 0x0012;
const ZIL_NUMBERID MESSAGE_OPTION                   = 0x0015;
const ZIL_NUMBERID FLAG_MNIF_NORMAL                 = 0x0016;
const ZIL_NUMBERID FLAG_MNIF_ABOUT                  = 0x00A2;
const ZIL_NUMBERID FLAG_MNIF_MAXIMIZE               = 0x0017;
const ZIL_NUMBERID FLAG_MNIF_MINIMIZE               = 0x0018;
const ZIL_NUMBERID FLAG_MNIF_MOVE                   = 0x0019;
const ZIL_NUMBERID FLAG_MNIF_SIZE                   = 0x001A;
const ZIL_NUMBERID FLAG_MNIF_SWITCH                 = 0x001B;
const ZIL_NUMBERID FLAG_MNIF_RESTORE                = 0x001C;
const ZIL_NUMBERID FLAG_MNIF_CLOSE                  = 0x001D;
const ZIL_NUMBERID BUTTON_OK                        = 0x001E;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x001F;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0020;
#endif

#if defined(USE_UI_GEOMETRY_MANAGER)
const ZIL_NUMBERID PROMPT_STRINGID                  = 0x0005;
const ZIL_NUMBERID FIELD_STRINGID                   = 0x0006;
const ZIL_NUMBERID FIELD_CONSTRAINTS                = 0x0007;
const ZIL_NUMBERID BUTTON_OK                        = 0x0002;
const ZIL_NUMBERID BUTTON_CANCEL                    = 0x0003;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0004;
const ZIL_NUMBERID FIELD_35                         = 0x0023;
const ZIL_NUMBERID FIELD_36                         = 0x0024;
const ZIL_NUMBERID FIELD_37                         = 0x0025;
#endif

#if defined(USE_DERIVED_OBJECTS)
#endif

