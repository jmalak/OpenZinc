                         /*****************************************/
                         /*     Copyright C Source, Inc. 1987 - 1993 */
                         /*     All Rights Reserved                       */
                         /*****************************************/

#if !defined (__GFX_)
#define __GFX_

   /* The following next 3 #defines define the library's function
    * interface. Only one of the elements should not be commented out.
    */

#define USE_PAS_M2              1
/* #define USE_LOWER_C          1 */
/* #define USE_UPPER_C          1 */

        /*  The next macro specifies Watcom C with functions that use
         *  the stack based argument passing convention.
         */
#define WATCOM_STACK_PASSING    0


                    /*********************************************
                        ****              IMPORTANT                      ****
                        ****                                                     ****
                        **** This file contains the basic header ****
                        **** definitions for the GFX Graphics,   ****
                        **** Fonts & Menus and Printer Screen    ****
                        **** Dump Libraries.                             ****
                        *********************************************/


    /*********************************************************
        *  Function declarations for internal GFX functions      *
        *  that you might want to use. The function prototypes  *
        *  are in the GFX?_SRC.H header files.                           *
        *********************************************************/


        /* This set is for assembly language functions */

#if defined(__WATCOMC__) || defined(__EXPRESSC__)
#define AFdecl  cdecl
#else
#define AFdecl
#endif

#if defined (_MSC_VER) && defined (_M_IX86)
#define __MSC_32__   ( (_MSC_VER >= 800) && (_M_IX86 >= 300) )
#else
#define __MSC_32__   0
#endif

        /* Forces packed structures for Metaware High C Ver 3.0+ */

#if defined (__HIGHC__)
#define PACKED _packed
#else
#define PACKED
#endif

#if defined (__HIGHC__) || (__MSC_32__ == 1)
#define _pascal
#define pascal
#define _near    
#endif

#if (defined(__TURBOC__) && (__TURBOC__ >= 0x290))
#define _pascal pascal
#define _near           near
#endif

#if defined (_I386) || defined (__386__) || (defined(__ZTC__) && (__I86__ >= 3)) || (__MSC_32__ > 0)
typedef void VOID_FAR;
typedef unsigned char UTINY_FAR;
#elif defined (__HIGHC__)
typedef _far void VOID_FAR;
typedef _far unsigned char UTINY_FAR;
#elif (defined(__TURBOC__) && (__TURBOC__ <= 0x201)) || defined(_QC)
#define VOID_FAR   void far
#define UTINY_FAR  unsigned char far
#else
typedef void far VOID_FAR;
typedef unsigned char far  UTINY_FAR;
#endif

#define MOBILE_DMEM             0

#if (MOBILE_DMEM == 0)
typedef void *    DMEM_HANDLE;
#if defined (_I386) || defined (__386__) || defined (DOS386)
typedef void *  FAR_DMEM_HANDLE;
#else
typedef UTINY_FAR *  FAR_DMEM_HANDLE;
#endif
#define NULL_DMEM_HANDLE                (void *) 0
#else
typedef long  DMEM_HANDLE;
typedef long  FAR_DMEM_HANDLE;
#define NULL_DMEM_HANDLE                0L
#endif


            /********************************************************
                ******* THE GFX GRAPHICS DEFINITIONS BEGIN HERE ********
                ********************************************************/

         /* Bit flags -- most are used in a function's color argument */

#define PAT                     0x0400
#define LINE_PAT                0x0400
#define RECORD_PTS              0x0800  /* Bit settings for the _gfx.color_flags */
#define FILL_PAT                0x1000
#define XOR_PEL         0x2000
#define FILL_SOLID              0x4000
#define XPARENT         0x8000
#define NO_FRAME                0x8000
#define COLOR_FLAGS             0xFC00

#define DFLT                    -14232      /* value for integer default */
#define CAP                     -1347       /* relative integer coordinates */
#define STEP                    -2415

#define FLT_DFLT                -723943.914     /* value for floating point default */
#define FLT_CAP         -275864.312     /* relative floating point coordinates */
#define FLT_STEP                -982132.001

#define DISPLAY_ERROR   0x100           /* flag for set_GFX_error_reporting() */


           /* These values specify the type of card & monitor in use.
            * The card and monitor are stored in _gfx.card_monitor. Note
            * that the type of display is in the low byte and the type
            * of card is in the high byte. The hardware is automatically
            * identified and the values are set when screen() is called.
            */

#define MONO_DISPLAY    0x1
#define COLOR_DISPLAY   0x2
#define EGA_DISPLAY     0x4
#define VGA_DISPLAY     0x8
#define MDA_CARD                0x100
#define CGA_CARD                0x200
#define EGA_CARD                0x400
#define VGA_CARD                0x800
#define EGA_VGA_CARD    (EGA_CARD | VGA_CARD)
#define HERC_CARD               0x1000
#define EGA_HERC                (EGA_VGA_CARD | HERC_CARD)


                /* The next set of values are definitions for the various
                 * standard bios modes and are stored in _gfx.bios_mode.
                 * Mode #s 8 & 9 are for Hercules and AT&T graphics modes
                 * and only exist for GFX products, since these modes
                 * are not supported by the IBM ROM bios.
                 */
#define NOT_SET         0xFFFF  /* Mode is not yet set */
                                                        /* Text modes */
#define BW_40_TEXT              0               /* 40 col 2 color */
#define COLOR_40_TEXT   1               /* 40 col 16 color */
#define BW_80_TEXT              2               /* 80 col 2 color */
#define COLOR_80_TEXT   3               /* 80 col 16 color */

#define GRAPHICS                4               /* Graphics Modes  */
#define MED_RES_COLOR   4               /* 320x200x04 color */
#define MED_RES_BW              5               /* 320x200x02 color */
#define HI_RES_BW               6               /* 640x200x02 color */
#define MONO_TEXT               7               /* MONOCHROME TEXT 80x25x2 color */
#define ATT_GFX         8               /* 640x400x02 color */
#define HERC_GFX                9               /* 720x348x02 color */
#define EGA_MED_RES     0xD             /* 320x200x16 color */
#define EGA_HI_RES              0xE             /* 640x200x16 color */
#define EGA_MONO                0xF             /* 640x350x02 color */
#define EGA_COLOR               0x10            /* 640x350x16 color */
#define VGA_MONO                0x11            /* 640x480x02 color */
#define VGA_COLOR               0x12    /* 640x480x16 color */


                /* These values are used by the screen() function
                 * to force a particular video mode.
                 */
#define FORCE_BIOS_MODE         0x8000
#define SVGA_256_RES            0x4000
#define FORCE_ATT                       FORCE_BIOS_MODE + ATT_GFX
#define FORCE_HERC                      FORCE_BIOS_MODE + HERC_GFX
#define FORCE_MED_RES_COLOR     FORCE_BIOS_MODE + MED_RES_COLOR
#define FORCE_HI_RES_BW         FORCE_BIOS_MODE + HI_RES_BW
#define FORCE_EGA_MED_RES       FORCE_BIOS_MODE + EGA_MED_RES
#define FORCE_EGA_HI_RES        FORCE_BIOS_MODE + EGA_HI_RES
#define FORCE_EGA_MONO          FORCE_BIOS_MODE + EGA_MONO
#define FORCE_EGA_COLOR         FORCE_BIOS_MODE + EGA_COLOR
#define FORCE_VGA_MONO          FORCE_BIOS_MODE + VGA_MONO
#define FORCE_VGA_COLOR         FORCE_BIOS_MODE + VGA_COLOR

#define OR_MED_RES_COLOR                0x400
#define OR_HI_RES_BW            0x600
#define OR_HERC                 0x900
#define OR_EGA_MED_RES          0xD00
#define OR_EGA_HI_RES           0xE00
#define OR_EGA_MONO             0xF00
#define OR_EGA_COLOR            0x1000
#define OR_VGA_MONO             0x1100
#define OR_VGA_COLOR            0x1200
#define NO_MODE_CHANGE          0x7300


                /* The following constants and macros are used by the
                 * functions using the COOR_PT structure that can
                 * translate between logical and absolute values.
                 */
#define FROM_ABS_TO_LOG                 0x100
#define FROM_LOG_TO_ABS                 0x200

#define X_POINT                         1
#define X_DISTANCE                              2
#define Y_POINT                         4
#define Y_DISTANCE                              8


#if defined (USE_LOWER_C)

#define abs_to_log(cpt)                 convert_coor_pt(cpt, FROM_ABS_TO_LOG | X_POINT | Y_POINT)
#define log_to_abs(cpt)                 convert_coor_pt(cpt, FROM_LOG_TO_ABS | X_POINT | Y_POINT)

#elif defined (USE_PAS_M2)

#define AbsToLog(cpt)                   ConvertCoorPt(cpt, FROM_ABS_TO_LOG | X_POINT | Y_POINT)
#define LogToAbs(cpt)                   ConvertCoorPt(cpt, FROM_LOG_TO_ABS | X_POINT | Y_POINT)

#elif defined (USE_UPPER_C)

#define ABS_TO_LOG(cpt)                 CONVERT_COOR_PT(cpt, FROM_ABS_TO_LOG | X_POINT | Y_POINT)
#define LOG_TO_ABS(cpt)                 CONVERT_COOR_PT(cpt, FROM_LOG_TO_ABS | X_POINT | Y_POINT)

#endif

typedef struct _cpt { int abs_x, abs_y;
                                  int ilog_x, ilog_y;
                                  double flog_x, flog_y;
                                } COOR_PT;


                /* The ICOOR structure can hold an (x,y) coordinate pair.
                 * You may find it useful for listing an array of integer
                 * coordinate pairs to send to poly_line() or poly_point().
                 */
typedef struct ic {int x, y;} ICOOR;

typedef struct _acr {int x_center, y_center;
                                 int x_start, y_start;
                                 int x_end, y_end;
                                 int x_radius, y_radius;
                                } ARC_COOR;

typedef struct _crgn { int min_x, min_y, max_x, max_y; } CLIP_REGION;

typedef struct _pic { int  user_flags;
                                  int  encoding;
                                  int  matte_color;
                                  int  promote_color;
                                  int  stretch_x_pels, stretch_y_rows;
                                  UTINY_FAR *xlat_256_pal;

                                  int  image_type;
                                  int  mem_type;

                                  int  signature;
                                  int  sys_flags;
                                  int  clip_image;
                                  int  blob_handle;
                                  int  item_index;
                                  int  file_handle;
                                  long image_offset;
                                  long image_size;
                                  long blob_size;
                                  long max_buf_sz;
                                  UTINY_FAR *ram_buf;
                                  UTINY_FAR *pal_data;
                                  UTINY_FAR *image_buf;
                                  int  pal_data_size;
                                  FAR_DMEM_HANDLE ram_mc;
                                  DMEM_HANDLE pic_mc;
                                  int  x1, y1;
                                  int  x_pels, y_rows;
                                  int  x_bytes_per_line;
                                  int  n_planes, n_orig_colors;
                                  int  y_rows_moved;
                                } PIC;


typedef struct _gfx_scale { int x_num, x_denom, y_num, y_denom;
                                           double x_flt, y_flt; } SCALE_VAL;

typedef struct _vu {

        unsigned int bkgnd;      /* background color */
        unsigned int fgnd;               /* forground color */
        unsigned int font_handle; /* current font for view */
        unsigned int auto_scale;  /* use auto-scaling */
        unsigned int xlat_scale;  /* the auto-scaling factor */

        int view_ul_x, view_ul_y; /* Viewport's upper-left coordinates */
        int view_lr_x, view_lr_y; /* Viewport's upper-left coordinates */

        int min_x, min_y;               /* clip coor for upper left corner */
        int max_x, max_y;               /* clip coor for lower right corner */
        int pt_x, pt_y;         /* current x,y coordinate pair */
        int ilog_x, ilog_y;     /* logical x,y pair for integer scaling */
        double flog_x, flog_y;  /* logical x,y pair for floating point scaling */
        int x_org, y_org;               /* viewport's origin */
        int x_dir, y_dir;               /* direction in which coordinates grow */
        SCALE_VAL scale;
        int (_pascal *get_pt)(int *);
        int vstatus;
        PIC *view_pic;
        int fpic_index;
        } VIEW;


typedef struct {

        /* general parameters for setting rom bios */

                unsigned int gfx_mode;      /* graphics mode info */
                unsigned int card_monitor;   /* type of card and monitor */
                unsigned int bios_mode;     /* crt mode */
                unsigned int vpage_n;       /* active display page */
                unsigned int wpage_n;       /* active write page */
                unsigned int err_number;     /* function error number */
                unsigned int show_gfx_err;   /* display the error number */
                unsigned int screen_x_res;   /* screen's x-resolution */
                unsigned int screen_y_res;   /* screen's y-resolution */
                unsigned int orig_bios_mode; /* original bios mode # */

        /* Info for writing text to the screen */

                unsigned int n_colors;    /* # of colors for current video mode */
                unsigned int screen_base;  /* segment of base of video ram */
                unsigned int screen_offset;/* offset of base of video ram  */

        /* Graphics info */

                unsigned int view_nmbr;
                unsigned int sys_flags;
                int bytes_per_row;              /* 80 is standard*/
                int last_video_row;
                int color_flags;

                unsigned int bkgnd;     /* background color */
                unsigned int fgnd;              /* forground color */
                unsigned int font_handle;/* font for writing */
                unsigned int auto_scale; /* use auto-scaling */
                unsigned int xlat_scale; /* the auto-scaling factor */

                int view_ul_x, view_ul_y; /* Viewport's upper-left coordinates */
                int view_lr_x, view_lr_y; /* Viewport's upper-left coordinates */

                int min_x, min_y;               /* viewport's upper left corner */
                int max_x, max_y;               /* viewport's lower right corner */
                int pt_x, pt_y;         /* current x,y coordinate pair */
                int ilog_x, ilog_y;     /* logical x,y pair for integer scaling */
                double flog_x, flog_y;  /* logical x,y pair for floating point scaling */
                int x_org, y_org;               /* viewport's origin */
                int x_dir, y_dir;               /* direction in which coordinates grow */
                SCALE_VAL scale;
                int     (_pascal *get_pt)(int *);
                int vstatus;
                PIC *view_pic;
                int fpic_index;
                int vpic_handle;

                }   GFX_STATUS;

                /* Fix for Watcom Ver 9.5 patch A and later */
#if defined(__SW_3S) || (WATCOM_STACK_PASSING == 1)
#define _gfx                    __gfx
#endif

#if defined (M_I86) && !defined (__ZTC__) && !defined(__WATCOMC__)
extern GFX_STATUS near _gfx;
#else
extern GFX_STATUS _gfx;
#endif


typedef struct _box_coor {int x1, y1, x2, y2;} BOX_COOR;

#define GET_GFX_VAL             1
#define SET_GFX_VAL             2

#define GET_GFX_STATUS(a, b)            set_get_gfx_status_val(GET_GFX_VAL, a, b)
#define get_gfx_status(a, b)            set_get_gfx_status_val(GET_GFX_VAL, a, b)
#define SET_GFX_STATUS(a, b)            set_get_gfx_status_val(SET_GFX_VAL, a, b)
#define set_gfx_status(a, b)            set_get_gfx_status_val(SET_GFX_VAL, a, b)

#define GFX_GFX_MODE                    1
#define GFX_CARD_MONITOR                        2
#define GFX_BIOS_MODE                   3
#define GFX_VPAGE_N                             4
#define GFX_WPAGE_N                             5
#define GFX_ERR_NUMBER                  6
#define GFX_SHOW_GFX_ERR                        7
#define GFX_USE_ANSI                    8
#define GFX_ANSI_IS_LOADED              9
#define GFX_PAINT_STACK_SZ              10
#define GFX_ATTR                                11
#define GFX_SCREEN_BASE                 12
#define GFX_WIDTH                               13
#define GFX_VIEW_NMBR                   14
#define GFX__PALLETE                    15
#define GFX_BYTES_PER_ROW               16
#define GFX_LAST_VIDEO_ROW              17
#define GFX_XOR_PEL                             18
#define GFX_BKGND                               19
#define GFX_FGND                                20
#define GFX_FONT_HANDLE                 21
#define GFX_AUTO_SCALE                  22
#define GFX_XLAT_SCALE                  23
#define GFX_V_START_BYTE                        24
#define GFX_V_NBYTES_WIDE               25
#define GFX_N_PELS_HIGH                 26
#define GFX_N_PELS_WIDE                 27
#define GFX_MIN_X                               28
#define GFX_MIN_Y                               29
#define GFX_MAX_X                               30
#define GFX_MAX_Y                               31
#define GFX_PT_X                                32
#define GFX_PT_Y                                33
#define GFX_ILOG_X                              34
#define GFX_ILOG_Y                              35

#define GFX_FLOG_X                              36
#define GFX_FLOG_Y                              37
#define GFX_X_ORG                               38
#define GFX_Y_ORG                               39
#define GFX_X_DIR                               40
#define GFX_Y_DIR                               41
#define GFX_SCALE_X_NUM                 42
#define GFX_SCALE_X_DENOM               43
#define GFX_SCALE_Y_NUM                 44
#define GFX_SCALE_Y_DENOM               45
#define GFX_SCALE_X_FLT                 46
#define GFX_SCALE_Y_FLT                 47
#define GFX_VSTATUS                             48
#define GFX_VIEW_PIC                    49
#define GFX_FPIC_INDEX                  50
#define GFX_VPIC_HANDLE                 51
#define GFX_VPIC_CB                             52
#define GFX_FPIC_CB                             53


                /* Archaic values, preserved for compatability
                 *  with previous versions of the library
                 */

#define  _DRAW_LINE  0                  /* values for LINE() */
#define  FILL_BOX        1
#define  EMPTY_BOX       2
#define  PAT_BOX         3

#define GFX_PALETTE -1                  /* set GFX PALETTE with COLOR() */

#define CURR_PT  -1347          /* relative coordinates */
#define FLT_CURR_PT -275864.312
#define PI              3.141593                /* for setting angles in ARC() */
#define ROUND           -1.1            /* for specifying a circular ARC() */



            /********************************************************
                ****** THE FONTS & MENUS DEFINITIONS BEGIN HERE ********
                ********************************************************/


        /* The LINE_ATTR structure has the elements set by the LINE_* functions */

typedef struct _lineattr
                            { int display_char, display_underline;
                                 int text_dir, justify, rotation, flags;
                            } LINE_ATTR;


typedef struct _fontattr
                           { int align, h_space, rotate_char;
                                int fgnd_color, bkgnd_color;
                                int expand_width, expand_rows;
                                int u_thickness, u_pos, u_color;
                                int fixed_spacing;
                           } FONT_ATTR;


        /*  The FONT structure contains the descriptive information about
         *  a font.  It is initialized when a font is read from the disk
         *  or, in the case of the ROM fonts, is pre-set.  Only the
         *  elements in the FONT_ATTR attr; member are modified as the
         *  program progresses.
         */

typedef PACKED struct _font {
   unsigned short int  *offset_table;   /* pointer to table of character offsets */
   UTINY_FAR *data_table;           /* pointer to the character bitmap */
   unsigned int width;              /* the # of bytes in each row of the bitmap */
   unsigned int height;             /* the # of rows in the bitmap */
   unsigned int lo_char;            /* the first character in the bitmap */
   unsigned int hi_char;            /* the last character in the bitmap */
   unsigned int widest_cell;        /* the widest character, in bits */
   int ascent_distance;             /* distance from baseline to top of char */
   int underline_loc;               /* distance from baseline to the underline */
   int fixed_width_char;            /* are the characters a fixed width? */
   FONT_ATTR attr;                          /* see the structure above */
   int  flags;                                  /* defining flags for the font */
   int  id;                                     /* defining id # -- usually 0 */
   UTINY_FAR *width_table;              /* pointer to width table */
   UTINY_FAR *fast_table;               /* pointer to fast offsets */
   int (_pascal *write_text)(unsigned char *, int);
   int (_pascal *text_len)(int, unsigned char *, int);
   } FONT;

#define STROKE_FONT                     1               /* for flags member */
#define COMPILED_FONT           2

#define is_stroke_font(f)       (f & STROKE_FONT)


        /* The following macro constants are discussed in the manual */

#define ROM_8x8                 1
#define ROM_8x14                        2
#define ROM_8x16                        3

#define TRANSPARENT                     -1

#define _0_DEGREES                      0
#define _90_DEGREES                     1
#define _180_DEGREES            2
#define _270_DEGREES            3

#define TOP_LINE                        0
#define MID_LINE                        1
#define BASE_LINE                       2
#define BOTTOM_LINE                     3
#define BGI_TOP_LINE            4
#define BGI_MID_LINE            5

#define TEXT_R_TO_L                     0
#define TEXT_L_TO_R                     1
#define TEXT_B_TO_T                     2
#define TEXT_T_TO_B                     3

#define JUSTIFY_START           0
#define JUSTIFY_CENTER          1
#define JUSTIFY_END                     2

#define FAST_FONTS                      1

#if defined (USE_LOWER_C)
#define FONT_ALIGN(fh, a)       font_align(fh, a)
#define FONT_ROTATE(fh,d)       font_rotate(fh,d)
#define LINE_DIRECTION(d)       line_direction(d)
#define LINE_JUSTIFY(j)         line_justify(j)
#define LINE_UNDERLINE(j)       line_underline(j)
#elif defined (USE_PAS_M2)
#define FONT_ALIGN(fh, a)       FontAlign(fh, a)
#define FONT_ROTATE(fh,d)       FontRotate(fh,d)
#define LINE_DIRECTION(d)       LineDirection(d)
#define LINE_JUSTIFY(j)         LineJustify(j)
#define LINE_UNDERLINE(j)       LineUnderline(j)
#endif

#define ALIGN_TOP(fh)           FONT_ALIGN(fh, TOP_LINE)
#define ALIGN_BOTTOM(fh)                FONT_ALIGN(fh, BOTTOM_LINE)
#define ALIGN_CENTER(fh)                FONT_ALIGN(fh, MID_LINE)
#define ALIGN_BASE(fh)          FONT_ALIGN(fh, BASE_LINE)

#define ROT_0(fh)                       FONT_ROTATE(fh, _0_DEGREES)
#define ROT_90(fh)                      FONT_ROTATE(fh, _90_DEGREES)
#define ROT_180(fh)                     FONT_ROTATE(fh, _180_DEGREES)
#define ROT_270(fh)                     FONT_ROTATE(fh, _270_DEGREES)

#define LD_RIGHT()                      LINE_DIRECTION(TEXT_L_TO_R)
#define LD_LEFT()                       LINE_DIRECTION(TEXT_R_TO_L)
#define LD_UP()                 LINE_DIRECTION(TEXT_B_TO_T)
#define LD_DOWN()                       LINE_DIRECTION(TEXT_T_TO_B)
#define LD_DFLT()                       LINE_DIRECTION(DFLT)

#define LJ_START()                      LINE_JUSTIFY(JUSTIFY_START)
#define LJ_CENTER()             LINE_JUSTIFY(JUSTIFY_CENTER)
#define LJ_END()                        LINE_JUSTIFY(JUSTIFY_END)
#define LJ_DFLT()                       LINE_JUSTIFY(DFLT)

#define LU_ON()                 LINE_UNDERLINE(1)
#define LU_OFF()                        LINE_UNDERLINE(0)
#define LU_DFLT()                       LINE_UNDERLINE(DFLT)

#define FONT_OFFSET_TABLE                       1
#define FONT_DATA_TABLE                         2
#define FONT_WIDTH                                      3
#define FONT_HEIGHT                                     4
#define FONT_LO_CHAR                            5
#define FONT_HI_CHAR                            6
#define FONT_WIDEST_CELL                                7
#define FONT_ASCENT_DISTANCE                    8
#define FONT_UNDERLINE_LOC                      9
#define FONT_FIXED_WIDTH_CHAR                   10
#define FONT_ATTR_ALIGN                         11
#define FONT_ATTR_H_SPACE                       12
#define FONT_ATTR_ROTATE_CHAR                   13
#define FONT_ATTR_FGND_COLOR                    14
#define FONT_ATTR_BKGND_COLOR                   15
#define FONT_ATTR_EXPAND_WIDTH          16
#define FONT_ATTR_EXPAND_ROWS                   17
#define FONT_ATTR_U_THICKNESS                   18
#define FONT_ATTR_U_POS                         19
#define FONT_ATTR_U_COLOR                       20

#define LINE_ATTR_DISPLAY_CHAR          51
#define LINE_ATTR_DISPLAY_UNDERLINE     52
#define LINE_ATTR_TEXT_DIR                      53
#define LINE_ATTR_JUSTIFY                       54

#define GET_FONT_VAL                            1
#define SET_FONT_VAL                            2

#define GET_FONT_STATUS(a, b)           set_get_font_status_val(GET_FONT_VAL, a, b)
#define get_font_status(a, b)           set_get_font_status_val(GET_FONT_VAL, a, b)
#define SET_FONT_STATUS(a, b)           set_get_font_status_val(SET_FONT_VAL, a, b)
#define set_font_status(a, b)           set_get_font_status_val(SET_FONT_VAL, a, b)

#define GET_LINE_STATUS(a, b)           set_get_font_status_val(GET_FONT_VAL, a, b)
#define get_line_status(a, b)           set_get_font_status_val(GET_FONT_VAL, a, b)
#define SET_LINE_STATUS(a, b)           set_get_font_status_val(SET_FONT_VAL, a, b)
#define set_line_status(a, b)           set_get_font_status_val(SET_FONT_VAL, a, b)


                 /**********************************************************
                  ****** THE MENU MACROS AND DEFINITIONS BEGIN HERE ********
                  **********************************************************/

#define NO_WAIT         0x8000
#define KYBD_EVENT              0x2                     /* EVENT TYPES */
#define MOUSE_EVENT     0x4
#define REGION_EVENT    0x8
#define TIMER_EVENT     0x10
#define MENU_EVENT              0x20
#define DB_CLICK_EVENT  0x100
#define DBLCLICK_EVENT  0x100
#define MACRO_EVENT     0x200


typedef PACKED struct _ev { unsigned int ascii_code, scan_code;
                                           unsigned int kybd_shift_flags, bios_time;
                                           unsigned char mouse_buttons, mouse_event;
                                           int x, y;
                                           unsigned int event_type;
                                         } EVENT;


typedef PACKED struct _eq_cb {int max_n_q_events, n_events_in_q, head, tail;
                                                unsigned char q_mouse_event;
                                                unsigned char db_click_pause;

                                                void (*dummy2)();

                                                void (*event_vec)(void);

                                                EVENT *event_q;

                                                unsigned int pause1, pause2;

                                                char *event_macro;
                                                int macro_char_n;

                                                EVENT * (*event_exit_hook)(EVENT * ev);
                                                unsigned int mask_for_event_exit_hook;

                                                int     timer, curr_timer;
                                                unsigned int last_timer_count;

                                           } EQ_CB;


typedef PACKED struct _sh { short int x, y;
                                           unsigned short int color1_mask[16];
                                           unsigned short int color2_mask[16];
                                         } CURSOR_SHAPE;


typedef struct _cb { int min_x, min_y, max_x, max_y, ascii_code;
                                 CURSOR_SHAPE *cursor_shape;
                            } CURSOR_REGION;


typedef PACKED struct _cm { unsigned char buttons, event;
                                           int x, y, visible;
                                           int n_cursor_regions, curr_cursor_region;
                                           CURSOR_REGION *cursor_regions;
                                         } MOUSE_STATE;

                /* Fix for Watcom Ver 9.5 patch A and later */
#if defined(__SW_3S) || (WATCOM_STACK_PASSING == 1)
#define  mouse_state     _mouse_state
#endif

#if defined (M_I86) && !defined (__ZTC__) && !defined(__WATCOMC__)
extern MOUSE_STATE near mouse_state;
#else
extern MOUSE_STATE mouse_state;
#endif


#define ENABLE_HOT_SPOT         1               /* HOT SPOT STATUS BITS */
#define HILIGHT_HOT_SPOT        2
#define SPOT_IS_HILIGHTED       4
#define RB_CLASS                        8
#define BLOCK_HOT_SPOT          0x10
#define TOGGLE_HOT_SPOT         (HILIGHT_HOT_SPOT + ENABLE_HOT_SPOT)

#define is_live_hot_spot(hs)    ((hs->status & (ENABLE_HOT_SPOT+BLOCK_HOT_SPOT)) == ENABLE_HOT_SPOT)

#define USR_BIT1                        0x40
#define USR_BIT2                        0x80
#define USR_BITS                        (USR_BIT1 + USR_BIT2)



#define SUB_MENU_ROOT_DEF       0x200           /* ROOT BAR STATUS FLAG */
#define MARK_IS_MOD                     0x400

#define DISABLE_CHOICE          0x100           /* Both Pull-down & Menu Flag */

#define SHOW_CHECK_MARK         0x200           /* PULL DOWN MENU FLAGS */
#define CHECK_MARK_IS_ON        0x400
#define SUB_MENU_VECTOR         0x800
#define JC_MENU                 0x80
#define JR_MENU                 0x40
#define IS_SUB_MENU_VECTOR      0x8C0
#define ADD_BAR                 0x1000
#define GRP1_RBTN                       0x2010
#define GRP2_RBTN                       0x2020
#define GRP3_RBTN                       0x2030


#define FIRST_EXT_SCAN_CODE             214
#define CROSS_REGION_SCAN_CODE  214
#define ROOT_MENU_SCAN_CODE             215
#define PULL_DOWN_SCAN_CODE             216
#define SELECT_SCAN_CODE                217
#define BUTTON_SCAN_CODE                218
#define SCROLL_BAR_SCAN_CODE            219
#define TIMER_SCAN_CODE                 220


#define SLIDE_BAR                       248
#define SCROLL_BAR_1_RIGHT      250
#define SCROLL_BAR_1_DOWN       250
#define SCROLL_BAR_1_LEFT       251
#define SCROLL_BAR_1_UP         251

#define VERT_SCROLL_BAR         1
#define HORIZONTAL_SCROLL_BAR 2

#define SCROLL_UP                       248
#define SCROLL_DOWN             249
#define NEXT_PAGE                       250
#define PREV_PAGE                       251
#define HOME_PAGE                       252
#define END_PAGE                        253

#define HORIZONTAL_POS                  0xF
#define VERTICAL_POS                    0xF0
#define OUTSIDE_BOTTOM                  1
#define BOTTOM_SIDE                     2
#define TOP_SIDE                                4
#define OUTSIDE_TOP                     8
#define OUTSIDE_RIGHT                   0x10
#define RIGHT_SIDE                              0x20
#define LEFT_SIDE                               0x40
#define OUTSIDE_LEFT                    0x80

#define DOWN_ARROW              80
#define UP_ARROW                72
#define LEFT_ARROW              75
#define RIGHT_ARROW     77
#define HOME                    71
#define END                     79
#define PG_UP                   73
#define PG_DOWN         81

#define NEW_PAGE                12
#define NEW_PAGE_MARKER "\014"
#define NEW_HELP_PAGE   2

#define TURN_ON 0x100
#define TURN_OFF        0

#define OK_BUTTON               1               /* values for buttons */
#define CANCEL_BUTTON   0

typedef struct _hs      { int  min_x, min_y;
                                  int  max_x, max_y;
                                  int  ascii_code, scan_code;
                                  unsigned char xor_color, status;
                                  unsigned int  xlat_keystroke;
                                } HOT_SPOT;

typedef struct _hsh { int  min_x, min_y;
                                  int  max_x, max_y;
                                  int  max_n_hot_spots, curr_base_hot_spot;
                                  int  n_hot_spots, curr_hot_spot;
                                  int  grow_stack_n_spots;
                                  HOT_SPOT *hs;
                                } HOT_SPOT_CB;

#define mod_rm_hot_spots_status(on_off, status) mod_hot_spots_status(0, 200, on_off | ROOT_MENU_SCAN_CODE, status)
#define first_hot_spot_ptr()            ((HOT_SPOT *) (hs_cb.hs))
#define curr_hot_spot_ptr()             ((HOT_SPOT *) (hs_cb.hs+hs_cb.curr_hot_spot))
#define curr_base_hot_spot_ptr()        ((HOT_SPOT *) (hs_cb.hs+hs_cb.curr_base_hot_spot))
#define next_hot_spot_ptr()             ((HOT_SPOT *) (hs_cb.hs+hs_cb.n_hot_spots))
#define hot_spot_ptr(i)                 ((HOT_SPOT *) (hs_cb.hs+i))

#define curr_hot_spot_n()               (hs_cb.curr_hot_spot)
#define base_hot_spot_n()               (hs_cb.curr_base_hot_spot)
#define n_hot_spots()                   (hs_cb.n_hot_spots)

#define set_curr_hot_spot_n(val)        (hs_cb.curr_hot_spot = val)
#define in_hot_spot(hs, x, y)   (inrange((hs)->min_x, x, (hs)->max_x) && inrange((hs)->min_y, y, (hs)->max_y))

#define is_keystroke(ev)                ((ev)->event_type & KYBD_EVENT)
#define is_mouse_event(ev)              ((ev)->event_type & MOUSE_EVENT)

#define get_border_width(cs)            ((cs)->border_type & 0xF)

        /* The next four macros are used for simple name changes */

#define set_loop_hook(vec)              init_loop_hook(vec)
#define set_event_text_macro(tp)        init_event_text_macro(tp)
#define set_exit_hook(vec, mask)        init_exit_hook(vec, mask)
#define set_event_timer(t1, t2) init_event_timer(t1, t2)

#define NO_SHADOW               0

#define LIGHT_HT                 6              /* Half tones */
#define MED_HT           7
#define HEAVY_HT                 8
#define SOLID_SHADOW     9

#define BRICK_PAT               10              /* Repeating patterns */
#define TRELLIS_PAT     11
#define SQUARE_PAT              12
#define STRIPE_PAT              13

#define CHECK_14                14              /* Check Marks */
#define CHECK_08                15

#define LEFT_SHADOW             0x80

#define SINGLE_BORDER           0x100
#define DOUBLE_BORDER           0x200
#define _3D_BORDER                      0x400

#define ULINE                           0x1000

#define DBOX0                           0x100           /* DIALOG Box */
#define SBOX0                           0x200           /* SELECTION Box */
#define FBOX0                           0x400           /* FORM Box   */

#define NO_BOX_DRAW             0x01            /* GFX_BOX 'type' flags */
#define IMMEDIATE_RETURN        0x02
#define ONE_KEY_RETURN          0x04
#define FORCE_POSITION          0x08
                                                                        /* DISPLAY_PAGE types */
#define CLEAN_SLATE             0x10
#define SHOW_SELECTION          0x20
#define SELECT_ICON             0x40            /* ICON type flag */
#define USR_VALIDATE_FORM       0x80
#define OVERSTRIKE_MODE         0x100
#define SHOW_DEFAULT            0x200
#define REFILL_DBOX                     0x400
#define SELECT_MULTI_ITEMS      0x800
#define SELECT_USER_INPUT       0x1000
#define NO_FORM_UNDERLINE       0x2000


#define HILIGHT_BUTTONS         0x40                    /* BUTTON_SET flag */

typedef struct _csc { int text, bkgnd, quick_key, xorInt;
                                  int border_type, border_color, border_thickness;
                                  int shadow_type, shadow_color, shadow_thickness;
                                } COLOR_SCHEME;

typedef struct _lay { int x_border_pels, y_border_rows;
                                  int x_pad_pels, y_pad_rows, min_width;
                                } LAYOUT;

typedef struct _btn { unsigned int xlat_keystroke, ascii_code;
                                  char *text;
                                } BUTTON_DATA;

typedef struct _mdf { int n_items;
                                  char *items;
                                  LAYOUT *layout;
                                  COLOR_SCHEME *cs;
                                  int fh, justify, disable_color, dummy;
                                } MENU_DEF;

typedef struct _tdf { int n_items;
                                  char **items;
                                  LAYOUT *layout;
                                  COLOR_SCHEME *cs;
                                  int fh, justify, type, dummy;
                                } DISPLAY_PAGE;

typedef struct _ttf { int n_items;
                                  char **items;
                                  LAYOUT *layout;
                                  COLOR_SCHEME *cs;
                                  int fh, justify, dummy1, dummy2;
                                } TITLE_BOX;

typedef struct _bdf { int n_buttons;
                                  BUTTON_DATA **buttons;
                                  LAYOUT *layout;
                                  COLOR_SCHEME *cs;
                                  int fh, n_buttons_per_row, flags, pos;
                                } BUTTON_SET;


        /*      Macros useful for initializing
         *      the n_items & items members in
         *      the DISPLAY_PAGE and similar
         *      structures above.
         */

#define n_arr_items(arr, type)   (sizeof(arr)/sizeof(type))
#define arr_sz(arr)     (sizeof(arr)/sizeof(char *))
#define sbox_init(sa)   (sizeof(sa)/sizeof(char *)), ((char **)sa)
#define ibox_init(tp)    sizeof(tp), ((char **)tp)
#define fbox_init(fa)   (sizeof(fa)/sizeof(FORM_ENTRY)), ((char **)fa)
#define dbox_init(db)   (sizeof(db)/sizeof(char *)), ((char **)db)

#define CURSOR_BOX              2
#define I_BEAM                  0

typedef struct _cmp { unsigned int xlat_color;
                                  int color[4];
                                 } COLOR_MAP;

typedef struct _sdf { COLOR_SCHEME *cs;
                                  int halftone, thickness;
                                  int n_col, n_rows, scroll_n_rows, pos;
                                } SCROLL;

typedef struct _sentry { char *prompt, *response;
                                        int     resp_len;
                                   } FORM_ENTRY;

typedef struct _ventry { unsigned char *prompt, *response;
                                        int     resp_len, validate;
                                   } VFORM_ENTRY;


typedef struct _vc { int x, y, n_pels_wide, n_lines_high;
                                 int color, type, mode;
                            } TEXT_CURSOR;

typedef struct _bdm { int x, y, width, height; } BOX_DIM;
#define INIT_BDIM               0, 0, 0, 0


typedef struct _hbx { int n_items;
                                  char **items;
/*                                struct _gbx *hbox; */
                                } HELP_BOX;


typedef struct _gbx { int type, return_value, fh;
                                  LAYOUT *layout;
                                  COLOR_SCHEME *cs;

                                  DISPLAY_PAGE *display_page;
                                  BUTTON_SET *buttons;
                                  TITLE_BOX *title_bar;         /* optional */
                                  SCROLL *scroll;                       /* optional */
                                  HELP_BOX *help;                       /*  optional */

                                  BOX_DIM *bdim;                        /* optional */

                                } GFX_BOX;


typedef struct _pdef { unsigned int status, xlat_keystroke;
                                   char *name;
                                 } PDM_DEF;

typedef struct _rdef { unsigned int status, xlat_keystroke;
                                   unsigned int first_choice, n_choices;
                                   char *root_menu_title;
                                   BOX_DIM bdim;
                                 } ROOT_DEF;

typedef struct _rmnu { int type;
                                   MENU_DEF *root_bar;
                                   MENU_DEF *pull_down_menus;
                                   HELP_BOX *help;
                                   BOX_DIM  *bdim;
                                 } ROOT_MENU;

typedef struct _vbx { int usr_id;
                                  int top_vline_n;
                                  int n_vlines_in_buf;
                                  int n_items;
                                  char **items;
                                } ULTD_DBOX;

#define CENTER_X                                -31923          /* values for set_box() */
#define CENTER_Y                                -31924

#define LINE_JUSTIFY_START              0x534A          /* JS */
#define LINE_JUSTIFY_END                0x454A          /* JE */
#define LINE_JUSTIFY_CENTER             0x434A          /* JC */
#define LINE_JUSTIFY_ABSOLUTE   0x414A          /* JA */

#define NO_LINE_FEED                    0x4C4E          /* NL */

#define JUSTIFY_ABSOLUTE                0x1000


#define NO_HELP                 ((HELP_BOX *) 0)
#define NO_BDIM                 ((BOX_DIM *) 0)
#define NO_EVENT                        ((EVENT *) 0)
#define NO_TITLE                        ((TITLE_BOX *) 0)
#define NO_SCROLL                       ((SCROLL *) 0)
#define NO_HOT_SPOTS            ((HOT_SPOT *) 0)
#define NO_BUTTON_SET           ((BUTTON_SET *) 0)
#define NO_BUTTONS                      ((BUTTON_SET *) 0)
#define NO_LAYOUT                       ((LAYOUT *) 0)
#define NO_COLOR_SCHEME         ((COLOR_SCHEME *) 0)
#define NO_BORDER                       0


/*  The following declarations are for boxes and box elements
 *  defined in the AUTO_MNU.C file that you may find useful.
 */

                                        /* Some commonly used COLOR_SCHEMEs
                                         */
extern COLOR_SCHEME dflt_cs;
extern COLOR_SCHEME basic_cs;
extern COLOR_SCHEME help_cs;
extern COLOR_SCHEME icon_cs;

extern COLOR_SCHEME button_cs;
extern COLOR_SCHEME scroll_bar_cs;
extern COLOR_SCHEME title_bar_cs;
extern COLOR_SCHEME icon_cs;

                                        /* Some commonly used LAYOUTs
                                         */
extern LAYOUT gbox_layout;
extern LAYOUT title_bar_layout;
extern LAYOUT std_btn_layout;
extern LAYOUT dbox_layout;
extern LAYOUT fbox_layout;
extern LAYOUT ibox_layout;
extern LAYOUT root_layout;
extern LAYOUT pd_layout;

                                         /*     Structures describing
                                          *     inidvidual buttons.
                                          */
extern BUTTON_DATA ok_btn;
extern BUTTON_DATA cancel_btn;
extern BUTTON_DATA next_pg_btn;
extern BUTTON_DATA prev_pg_btn;

extern BUTTON_DATA form_ok_btn;
extern BUTTON_DATA form_cncl_btn;
extern BUTTON_DATA form_next_pg_btn;
extern BUTTON_DATA form_prev_pg_btn;


                                        /*  Arrays of pointers to buttons.
                                         */
extern BUTTON_DATA *std_1btn[];
extern BUTTON_DATA *std_2btn[];
extern BUTTON_DATA *std_3btn[];
extern BUTTON_DATA *frm_2btn[];
extern BUTTON_DATA *frm_3btn[];


                                        /*  Structures fully describing
                                         *  possible sets of buttons for a box.
                                         */
extern BUTTON_SET one_button;
extern BUTTON_SET ok_cncl_button;
extern BUTTON_SET mpage_button;
extern BUTTON_SET form_ok_cncl_button;
extern BUTTON_SET form_mpage_button;


                                        /*  Structures of the Auto boxes
                                         */
extern TITLE_BOX auto_title;
extern GFX_BOX auto_dbox;
extern GFX_BOX auto_fbox;
extern GFX_BOX auto_sbox;
extern GFX_BOX auto_ibox;


         /**************************************************************
          ****** THE PRINTER SCREEN DUMP DEFINITIONS BEGIN HERE ********
          **************************************************************/

/* function argument values for set_prx_options() */
#define OMIT            -1
#define RESET           -2
#define PORTRAIT         1
#define LANDSCAPE        0

/* function return codes */
#define  SUCCESS  1
#define  FAILURE  0

/* output scale types */
#define USE_CENTIMETERS         1
#define USE_INCHES                      2
#define USE_DOTS                        3
#define USE_ABS_SCALE           3

/* printer status values */

#define PRT_TIME_OUT            0x01
#define PRT_IO_ERROR            0x08
#define PRT_SELECTED            0x10
#define PRT_NO_PAPER            0x20
#define PRT_ACK                 0x40
#define PRT_BUSY                        0x80


        /* Names for resident printer definitions */

#define HP_LASER                                        10
#define HP_LASER_III                            11
#define HP_LASER_IV                             17

#define HP_PAINT_JET                            12
#define HP_PAINT_JET_XL                         13
#define HP_PAINT_JET_XL_300                     14

#define HP_DESK_JET                                     15
#define HP_DESK_JET_500C                                16

#define EPSON_9PIN                                      01
#define EPSON_9PIN_COLOR                                02
#define EPSON_24PIN                                     03
#define EPSON_24PIN_COLOR                       04

#define PJ_DFLT_PAL                                     0
#define PJ_USER_PAL                                     1
#define PJ_XL_VGA_PAL_BLACK_ZERO                2
#define PJ_XL_VGA_PAL_WHITE_ZERO                3



#if defined (USE_PAS_M2)
#define Pat                     0x0400
#define LinePat         0x0400
#define RecordPts               0x0800  /* Bit settings for the _gfx.color_flags */
#define FillPat         0x1000
#define XorPel                  0x2000
#define FillSolid               0x4000
#define Xparent         0x8000
#define NoFrame         0x8000
#define ColorFlags              0xFC00
#define Dflt                    -14232      /* value for integer default */
#define Cap                     -1347       /* relative integer coordinates */
#define Step                    -2415

#define FltDflt         -723943.914     /* value for floating point default */
#define FltCap                  -275864.312     /* relative floating point coordinates */
#define FltStep         -982132.001

#define MonoDisplay             0x1
#define ColorDisplay    0x2
#define EgaDisplay              0x4
#define VgaDisplay              0x8
#define MdaCard         0x100
#define CgaCard         0x200
#define EgaCard         0x400
#define VgaCard         0x800
#define EgaVgaCard              (EGA_CARD | VGA_CARD)
#define HercCard                0x1000
#define EgaHerc         (EGA_VGA_CARD | HERC_CARD)

#define ForceBiosMode           0x8000
#define ForceAtt                        ForceBiosMode + ATT_GFX
#define ForceHerc                       ForceBiosMode + HERC_GFX
#define ForceMedResColor                ForceBiosMode + MED_RES_COLOR
#define ForceHiResBw            ForceBiosMode + HI_RES_BW
#define ForceEgaMedRes          ForceBiosMode + EgaMedRes
#define ForceEgaHiRes           ForceBiosMode + EgaHiRes
#define ForceEgaMono            ForceBiosMode + EgaMono
#define ForceEgaColor           ForceBiosMode + EgaColor
#define ForceVgaMono            ForceBiosMode + VgaMono
#define ForceVgaColor           ForceBiosMode + VgaColor

#define OrMedResColor           0x400
#define OrHiResBw                       0x600
#define OrHerc                          0x900
#define OrEgaMedRes             0xD00
#define OrEgaHiRes                      0xE00
#define OrEgaMono                       0xF00
#define OrEgaColor                      0x1000
#define OrVgaMono                       0x1100
#define OrVgaColor                      0x1200
#define NoModeChange            0x7300

#define XPoint                                  1
#define XDistance                               2
#define YPoint                                  4
#define YDistance                               8
#endif


                            /***********************************/
                            /****   ADDITIONS FOR VER 3.0        ****/
                            /***********************************/

#define N_EXTD_MODES            8

typedef PACKED struct _svga { int       chipset;        /* #defined above */
                                                int     bios;      /* #defined above */
                                                int     vram;           /* in 64K blocks, 0=don't know */
                                                int     VESA_is_supported;
                                                char set_mode_with_VESA;
                                                char switch_banks_with_VESA;
                                                int     modes[N_EXTD_MODES];
                                                void (AFdecl *init_chipset)(void);
                                                char future_use[16];
                                           } SVGA;

typedef struct _vr {int max_x, max_y, n_colors, vram_sz; } VESA_RES;

#define USE_VESA_SWITCHING              0x4000

#define ZSOFT_PCX               0x0A
#define GFX_PCX         0xB3
#define GFX_MEM_PCX             0xC4

typedef PACKED struct _pcxhdr {
                                        unsigned char mfg, version, encoding, bits_per_pixel;
                                        short int min_x, min_y;
                                        short int max_x, max_y;
                                        short int hres, vres;
                                        unsigned char colormap[16][3], reserved, n_planes;
                                        short int bytes_per_line, palette_type;
                                        char  filler1[42];
                                        long     image_size;
                                        char  filler2[12];
                                   } PCX_HDR;


/* Low level prototypes for version 3.0 */

                /* action switches for get_pcx_file */

#define SAVE_PAL                        0x100
#define GREY_SCALE                      0x200
#define SAVE_VIDEO_COOR         0x400

#define ENCODE_PCX                      0x001
#define ENCODE_RLE                      0x002
#define ENCODE_LZW                      0x003
#define ENCODE_BEST                     0x007
#define ENCODE_MATTE_MASK       0x010
#define GET_PIC_FLAGS           0x31F

                /* action switches for put_pcx_file() */

#define USE_VIDEO_COOR          0x0400
#define REMAP_256_COLOR         0x0800
#define ONE_GULP                        0x1000
#define NO_PCX_PAL                      0x2000
#define CENTER_IMAGE            0x4000
#define NO_GREY_SCALE           0x8000
#define PUT_PIC_FLAGS           0xFC00

#define PCX_256_PAL_SZ          (256 * 3)
#define PCX_16_PAL_SZ           (16 * 3)

#define SAVE_256_PCX_PAL                0x0080          /* pic.sys_flags */
#define FREE_PIC_STRUCT         0x0100
#define FREE_IMAGE_MEM          0x0200
#define FREE_HANDLE                     0x0400
#define PCX_SHELL                       0x0800
#define STRETCH_IMAGE           0x1000
#define PAL_16_INFO                     0x2000
#define PAL_256_INFO            0x4000

#define PCX_FILE                        0x100           /* pic.image_type */
#define IMAGE_BLOB                      0x200
#define IMAGE_RAM                       0x400
#define FONT_BLOB                       0x800

#define NO_MEM                  0x00                    /* pic.mem_type */
#define VIDEO_RAM                       0x01
#define XMS_RAM                 0x02
#define EMS_RAM                 0x03
#define SYSTEM_RAM                      0x04
#define DISK_MEM                        0x05
#define FILE_MEM                        0x10
#define N_MEM_TYPES                     7
#define RAM_FILE                        0x100

typedef PACKED struct _blobindx { short int type;
                                                    long        blob_pos;
                                                    long        blob_size;
                                                  } BLOB_ITEM;

/* 'safe' malloc structure and defines */

#define MALLOC_SIGNATURE                0xC4000000L
#define FARMALLOC_SIGNATURE     0xA8000000L

#define SAFE_MALLOC                     1
#define MARK_MALLOC                     2
#define MARK_FREE                       4
#define MALLOC_MARK_BYTE                0xC1

typedef struct _mldt { unsigned int safe_type;
                                   unsigned int total_n_blocks;
                                   unsigned int n_blocks_in_use;
                                   long largest_small_block;
                                   long largest_far_block;
                                   long n_bytes_in_use;
                                   long max_malloc_bytes;
                                 } MALLOC_DATA;

#endif
