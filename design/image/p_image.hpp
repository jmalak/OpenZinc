#if defined(USE_HELP_CONTEXTS)
const UI_HELP_CONTEXT HELP_BITMAP_WINDOW            = 0x0007; // Bitmap Creation Window
const UI_HELP_CONTEXT HELP_GENERAL                  = 0x0001; // About Image Editor
const UI_HELP_CONTEXT HELP_ICON_WINDOW              = 0x0008; // Icon Creation Window
const UI_HELP_CONTEXT HELP_MOUSE_WINDOW             = 0x0009; // Mouse Cursor Creation Window
const UI_HELP_CONTEXT HELP_OPT_BITMAP               = 0x0004; // Bitmap Options
const UI_HELP_CONTEXT HELP_OPT_EDIT                 = 0x0003; // Edit Options
const UI_HELP_CONTEXT HELP_OPT_FILE                 = 0x0002; // File Options
const UI_HELP_CONTEXT HELP_OPT_ICON                 = 0x0005; // Icon Options
const UI_HELP_CONTEXT HELP_OPT_MOUSE                = 0x0006; // Mouse Cursor Options
#endif

#if defined(USE_UI_BITMAP_WINDOW)
const ZIL_NUMBERID PROMPT_WIDTH                     = 0x0016;
const ZIL_NUMBERID FIELD_WIDTH                      = 0x0065;
const ZIL_NUMBERID PROMPT_HEIGHT                    = 0x0017;
const ZIL_NUMBERID FIELD_HEIGHT                     = 0x0066;
const ZIL_NUMBERID BUTTON_STORE                     = 0x0001;
const ZIL_NUMBERID BUTTON_STORE_AS                  = 0x0020;
const ZIL_NUMBERID BUTTON_CLOSE                     = 0x0002;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0003;
const ZIL_NUMBERID BUTTON_SIZE                      = 0x001E;
#endif

#if defined(USE_UI_ICON_WINDOW)
const ZIL_NUMBERID PROMPT_WIDTH                     = 0x0016;
const ZIL_NUMBERID FIELD_WIDTH                      = 0x0065;
const ZIL_NUMBERID PROMPT_HEIGHT                    = 0x0017;
const ZIL_NUMBERID FIELD_HEIGHT                     = 0x0066;
const ZIL_NUMBERID BUTTON_STORE                     = 0x0001;
const ZIL_NUMBERID BUTTON_STORE_AS                  = 0x0020;
const ZIL_NUMBERID BUTTON_CLOSE                     = 0x0002;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0003;
const ZIL_NUMBERID BUTTON_SIZE                      = 0x001E;
#endif

#if defined(USE_UI_MOUSE_WINDOW)
const ZIL_NUMBERID PROMPT_WIDTH                     = 0x0016;
const ZIL_NUMBERID FIELD_WIDTH                      = 0x0065;
const ZIL_NUMBERID PROMPT_HEIGHT                    = 0x0017;
const ZIL_NUMBERID FIELD_HEIGHT                     = 0x0066;
const ZIL_NUMBERID PROMPT_HOT_COLUMN                = 0x0025;
const ZIL_NUMBERID FIELD_HOT_COLUMN                 = 0x0026;
const ZIL_NUMBERID PROMPT_HOT_LINE                  = 0x0027;
const ZIL_NUMBERID FIELD_HOT_LINE                   = 0x0028;
const ZIL_NUMBERID BUTTON_STORE                     = 0x0001;
const ZIL_NUMBERID BUTTON_STORE_AS                  = 0x0020;
const ZIL_NUMBERID BUTTON_CLOSE                     = 0x0002;
const ZIL_NUMBERID BUTTON_HELP                      = 0x0003;
const ZIL_NUMBERID BUTTON_SIZE                      = 0x001E;
#endif

#if defined(USE_MSG_TABLE)
const ZIL_NUMBERID ZMSG_IMPORT_TITLE                = 0x00C8;
const ZIL_NUMBERID ZMSG_IMPORT_MESSAGE              = 0x00C9;
const ZIL_NUMBERID ZMSG_LEFT                        = 0x00CA;
const ZIL_NUMBERID ZMSG_RIGHT                       = 0x00CB;
const ZIL_NUMBERID ZMSG_SCREEN                      = 0x00CC;
const ZIL_NUMBERID ZMSG_MOUSE_CLICK                 = 0x00CD;
const ZIL_NUMBERID ZMSG_MOUSE_OPT_CLICK             = 0x00CE;
#endif

#if defined(USE_ZAF_IMAGE_EDITOR)
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
const ZIL_NUMBERID EDIT                             = 0x0003;
const ZIL_NUMBERID EDIT_GRID                        = 0x0022;
const ZIL_NUMBERID EDIT_ROLLER                      = 0x0023;
const ZIL_NUMBERID ROLLER_1X1                       = 0x0025;
const ZIL_NUMBERID ROLLER_1X2                       = 0x0026;
const ZIL_NUMBERID ROLLER_1X3                       = 0x0027;
const ZIL_NUMBERID ROLLER_1X4                       = 0x0028;
const ZIL_NUMBERID ROLLER_1X5                       = 0x0029;
const ZIL_NUMBERID EDIT_PATTERN                     = 0x0042;
const ZIL_NUMBERID PATTERN_PENCIL                   = 0x003B;
const ZIL_NUMBERID PATTERN_ROLLER                   = 0x003C;
const ZIL_NUMBERID PATTERN_LINE                     = 0x0043;
const ZIL_NUMBERID PATTERN_RECTANGLE                = 0x003D;
const ZIL_NUMBERID PATTERN_RECTANGLE                = 0x003E;
const ZIL_NUMBERID PATTERN_ELLIPSE                  = 0x003F;
const ZIL_NUMBERID PATTERN_FILL_ELLIPSE             = 0x0040;
const ZIL_NUMBERID PATTERN_FILL                     = 0x0041;
const ZIL_NUMBERID EDIT_ERASE                       = 0x008C;
const ZIL_NUMBERID EDIT_SEP1                        = 0x0071;
const ZIL_NUMBERID EDIT_CUT                         = 0x006E;
const ZIL_NUMBERID EDIT_COPY                        = 0x006F;
const ZIL_NUMBERID EDIT_PASTE                       = 0x0070;
const ZIL_NUMBERID EDIT_DELETE                      = 0x0046;
const ZIL_NUMBERID EDIT_SEP2                        = 0x0072;
const ZIL_NUMBERID EDIT_GROUP_DEFINE                = 0x0073;
const ZIL_NUMBERID EDIT_GROUP_UNDEFINE              = 0x0074;
const ZIL_NUMBERID TOOLS                            = 0x008D;
const ZIL_NUMBERID BITMAP                           = 0x004C;
const ZIL_NUMBERID UI_BITMAP                        = 0x0048;
const ZIL_NUMBERID UI_BITMAP                        = 0x007F;
const ZIL_NUMBERID BITMAP_SEP1                      = 0x0049;
const ZIL_NUMBERID UI_BITMAP                        = 0x004A;
const ZIL_NUMBERID UI_BITMAP                        = 0x004B;
const ZIL_NUMBERID UI_BITMAP                        = 0x004D;
const ZIL_NUMBERID UI_BITMAP                        = 0x004E;
const ZIL_NUMBERID BITMAP_SEP2                      = 0x004F;
const ZIL_NUMBERID UI_BITMAP                        = 0x0068;
const ZIL_NUMBERID UI_BITMAP                        = 0x0069;
const ZIL_NUMBERID UI_BITMAP                        = 0x0050;
const ZIL_NUMBERID ICON                             = 0x0058;
const ZIL_NUMBERID UI_ICON                          = 0x0051;
const ZIL_NUMBERID UI_ICON                          = 0x0080;
const ZIL_NUMBERID ICON_SEP1                        = 0x0052;
const ZIL_NUMBERID UI_ICON                          = 0x0066;
const ZIL_NUMBERID UI_ICON                          = 0x0053;
const ZIL_NUMBERID UI_ICON                          = 0x0054;
const ZIL_NUMBERID UI_ICON                          = 0x0055;
const ZIL_NUMBERID ICON_SEP2                        = 0x0056;
const ZIL_NUMBERID UI_ICON                          = 0x006A;
const ZIL_NUMBERID UI_ICON                          = 0x006B;
const ZIL_NUMBERID UI_ICON                          = 0x0057;
const ZIL_NUMBERID MOUSE                            = 0x0065;
const ZIL_NUMBERID UI_MOUSE                         = 0x005E;
const ZIL_NUMBERID UI_MOUSE                         = 0x0081;
const ZIL_NUMBERID MOUSE_SEP1                       = 0x005F;
const ZIL_NUMBERID UI_MOUSE                         = 0x0067;
const ZIL_NUMBERID UI_MOUSE                         = 0x0060;
const ZIL_NUMBERID UI_MOUSE                         = 0x0061;
const ZIL_NUMBERID UI_MOUSE                         = 0x0062;
const ZIL_NUMBERID MOUSE_SEP2                       = 0x0063;
const ZIL_NUMBERID UI_MOUSE                         = 0x006C;
const ZIL_NUMBERID UI_MOUSE                         = 0x006D;
const ZIL_NUMBERID UI_MOUSE                         = 0x0064;
const ZIL_NUMBERID HELP                             = 0x0004;
const ZIL_NUMBERID HELP_INDEX                       = 0x0083;
const ZIL_NUMBERID HELP_SEP1                        = 0x0084;
const ZIL_NUMBERID HELP_FILE                        = 0x0085;
const ZIL_NUMBERID HELP_EDIT                        = 0x0086;
const ZIL_NUMBERID HELP_BITMAP                      = 0x0087;
const ZIL_NUMBERID HELP_ICON                        = 0x0088;
const ZIL_NUMBERID HELP_MOUSE                       = 0x0089;
const ZIL_NUMBERID HELP_SEP2                        = 0x008A;
const ZIL_NUMBERID HELP_ABOUT                       = 0x008B;
const ZIL_NUMBERID FIELD_PATTERNS                   = 0x0059;
const ZIL_NUMBERID OPT_PENCIL                       = 0x0033;
const ZIL_NUMBERID OPT_ROLLER                       = 0x0034;
const ZIL_NUMBERID OPT_LINE                         = 0x0035;
const ZIL_NUMBERID OPT_RECTANGLE                    = 0x0036;
const ZIL_NUMBERID OPT_FILL_RECTANGE                = 0x0037;
const ZIL_NUMBERID OPT_ELLIPSE                      = 0x0038;
const ZIL_NUMBERID OPT_FILL_ELLIPSE                 = 0x0039;
const ZIL_NUMBERID OPT_FILL                         = 0x003A;
#endif

#if defined(USE_DERIVED_OBJECTS)
#endif

