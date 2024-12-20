//      Zinc Interface Library - UI_ENV.HPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#ifndef UI_ENV_HPP
#define UI_ENV_HPP

// ----- Version information ------------------------------------------------

#define ZIL_VERSION             420
#define ZIL_REVISION    0

// ----- General Zinc Switches ----------------------------------------------

// Optimization switches for various compiler problems.
#define ZIL_LOAD                        // Zinc persistence.
#define ZIL_STORE                       // Zinc persistence.
#define ZIL_STORAGE_IMAGE               // Zinc storage.
#define ZIL_OPTIMIZE                    // Size & speed optimization for members.
#define ZIL_EDIT                                // WOS_EDIT_MODE objects.
//#define ZIL_OLD_DEFS                  // Use old ZIL definitions.

// Presentation switches for the library.
//#define ZIL_TEXT_ONLY                 // Run Zinc ONLY in text mode.
//#define ZIL_GRAPHICS_ONLY             // Run Zinc ONLY in graphics mode.

#define ZIL_SHADOW_BORDER               // Shadow the border and button objects (text).
//#define ZIL_STANDARD_BORDER           // Standard double/single border (text).
//#define ZIL_3D_BORDER                 // Use the visual basic presentation (text).

#define ZIL_MSWINDOWS_STYLE             // Use the MSWindows presentation (graphics).
//#define ZIL_OS2_STYLE                 // Use the OS/2 presentation (graphics).
//#define ZIL_MOTIF_STYLE               // Use the Motif presentation (graphics).

//#define ZIL_MSWINDOWS_CTL3D           // Use CTL3DV2.DLL (3D look). For Windows only.

// Switches for the international language versions.
#define ZIL_ISO8859_1                   // Standard support for ISO 8859-1
//#define ZIL_UNICODE                   // Support for UNICODE
//#define ZIL_DECOMPOSE                 // Support to decompose compound Unicode
//#define ZIL_HARDWARE                  // Support for Non-AT MS-DOS machines.
#if 0
/* TODO!
 * temporarily disabled, it is implementation dependant
 * OW 2.0 use new layout need update appropriate code
 */
#define ZIL_REARRANGEARGS               // Support for argument rearrangement in
                                        // sprintf() and sscanf().
#endif
#define ZIL_DO_FILE_I18N                // Support for Internationalization
#define ZIL_3x_COMPAT                   // Compatibility for 3x .dat files.
#define ZIL_DO_OS_I18N                  // Do possibly broken OS i18n support

// ----- Compiler/Environment Default Dependencies --------------------------

#define ZIL_EXPORT_CLASS
#define ZIL_EXPORT_CLASS_FUNCTION
//#define ZIL_LITTLEENDIAN              // Byte ordering.  Opposite of ZIL_BIGENDIAN.
#define ZIL_LINKBUG
//#define ZIL_WORD_SIZE         16
//#define ZIL_SIGNED_CHAR

// ----- ZIL_NULLP, ZIL_NULLF, ZIL_NULLH ----------------------------------

#define ZIL_NULLP(type)         ((type *)0)
#define ZIL_NULLF(type)         ((type)0)
#define ZIL_NULLH(type)         ((type)0)

// --------------------------------------------------------------------------
// ----- BORLAND ------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#       define ZIL_SIGNED_CHAR
#       define ZIL_VOIDF(function)      ((void *)(function))
#       define ZIL_VOIDP(pointer)       ((void *)(pointer))
#       if defined(ZIL_UNICODE) && __BORLANDC__ > 0x0410
#               define ICHAR_T  wchar_t
#       endif
#       if defined(__OS2__)
#               define ZIL_OS2          20
#       elif defined(DOSX286) || defined(DOS16PP) || defined(DOS16M)
#               define ZIL_MSDOS        20
#               pragma option -zEZIL_FARDATA
#               define ZIL_FARDATA      far
#               define ZIL_WORD_SIZE    16
#       elif defined(DOS32PP)
#               define ZIL_MSDOS        20
#       elif defined(_WINDOWS) || defined(_Windows)
#               define ZIL_MSWINDOWS    WINVER
#               if defined(__WIN32__)
#                       define ZIL_WINNT        __WIN32__
#               else
#                       pragma option -zEZIL_FARDATA
#                       define ZIL_FARDATA      far
#                       define ZIL_WORD_SIZE    16
#               endif
#               ifdef __DLL__
#                       undef ZIL_EXPORT_CLASS
#                       define ZIL_EXPORT_CLASS _export
#               else
#                       if !defined(ZIL_WINNT)
#                               undef ZIL_EXPORT_CLASS
#                               define ZIL_EXPORT_CLASS huge
#                       endif
#               endif
#       else
#               define ZIL_MSDOS                20
#               pragma option -zEZIL_FARDATA
#               define ZIL_FARDATA              far
#               define ZIL_WORD_SIZE            16
#       endif
#endif

// --------------------------------------------------------------------------
// ----- GLOCKENSPIEL -------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__GLOCKENSPIEL__)
#       define ZIL_OS2          21
#       define ZIL_SIGNED_CHAR
#endif

// --------------------------------------------------------------------------
// ----- METAWARE -----------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__HIGHC__)
#       define ZIL_SIGNED_CHAR
#       if defined(_WINDOWS) || defined(_Windows) || defined(WIN32)
#               define ZIL_MSWINDOWS    WINVER
#               if defined      WIN32
#                       define ZIL_WINNT        WIN32
#               else
#                       define ZIL_FARDATA      far
#                       define ZIL_WORD_SIZE    16
#               endif
#               ifdef __DLL__
#                       undef ZIL_EXPORT_CLASS
#                       if defined(ZIL_WINNT)
#                               define ZIL_EXPORT_CLASS __export
#                       else
#                               define ZIL_EXPORT_CLASS _export
#                       endif
#               endif
#       else
#               define ZIL_MSDOS        20
#               define ZIL_WORD_SIZE    16
#       endif
#endif

// --------------------------------------------------------------------------
// ----- MICROSOFT ----------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(_MSC_VER) && !defined(__WATCOMC__)
#       define ZIL_SIGNED_CHAR
#       define ZIL_VOIDP(pointer)       ((void *)pointer)
#       if defined(ZIL_UNICODE)
#               include <stddef.h>
#               define ICHAR_T wchar_t
#       endif
#       if defined(_WINDOWS) || defined(_Windows) || defined(WIN32)
#               define ZIL_MSWINDOWS    WINVER
#               if defined(WIN32)
#                       define ZIL_WINNT        WIN32
#               else
#                       define ZIL_FARDATA      far
#                       define ZIL_WORD_SIZE    16
#               endif
#               ifdef __DLL__
#                       undef ZIL_EXPORT_CLASS
#                       if defined(ZIL_WINNT)
#                               define ZIL_EXPORT_CLASS __export
#                       else
#                               define ZIL_EXPORT_CLASS _export
#                       endif
#               endif
#       else
#               define ZIL_MSDOS        20
#               if defined(DOS386)
#               else
#                       define ZIL_FARDATA      far
#                       define ZIL_WORD_SIZE    16
#               endif
#       endif
#endif

// --------------------------------------------------------------------------
// ----- IBM ----------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__IBMCPP__)
#       define ZIL_OS2          21
#       define ZIL_SIGNED_CHAR
#       if defined(ZIL_UNICODE)
#               define ICHAR_T  wchar_t
#       endif
#endif

// --------------------------------------------------------------------------
// ----- SYMANTEC & ZORTECH -------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__SC__)
#       define ZIL_VOIDP(pointer)       ((void *)(pointer))
#       if defined(ZIL_UNICODE)
#               define ICHAR_T  wchar_t
#       endif
#       if defined(__OS2__)
#               define ZIL_OS2          20
#               define ZIL_SIGNED_CHAR
#       elif defined(_WINDOWS) || defined(_Windows) || defined(__NT__)
#               define ZIL_MSWINDOWS    WINVER
#               define ZIL_SIGNED_CHAR
#               if defined (__NT__)
#                       define ZIL_WINNT        __NT__
#               else
#                       define ZIL_FARDATA      far
#                       define ZIL_WORD_SIZE    16
#               endif
#               ifdef __DLL__
#                       undef ZIL_EXPORT_CLASS
#                       if defined(ZIL_WINNT)
#                               define ZIL_EXPORT_CLASS __export
#                       else
#                               define ZIL_EXPORT_CLASS _export
#                       endif
#               endif
#       elif defined(macintosh)
#               if defined(ZIL_UNICODE)
#                       define ZIL_MACINTOSH    71
#               else
#                       define ZIL_MACINTOSH    70
#               endif
#               define ZIL_EXPORT_CLASS
#               define ZIL_BIGENDIAN
#               undef ZIL_LINKBUG
#               define ZIL_OLDIO
#               include <Printing.h>
#               include <Mac #Includes.cpp>
#               if !defined(__powerc)
#                       if !defined(c2pstr)
#                               define c2pstr   CtoPstr
#                       endif
#                       if !defined(p2pstr)
#                               define p2cstr   PtoCstr
#                       endif
#               endif
#       else
#               define ZIL_MSDOS                20
#               if defined(DOS386)
#               else
#                       define ZIL_WORD_SIZE    16
#               endif
#       endif
#endif

// --------------------------------------------------------------------------
// ----- METROWERKS ---------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__MWERKS__)
#       if defined(ZIL_UNICODE)
#               define ZIL_MACINTOSH    71
#       else
#               define ZIL_MACINTOSH    70
#       endif
#       define ZIL_EXPORT_CLASS
#       define ZIL_BIGENDIAN
#       undef ZIL_LINKBUG
#       if defined(__powerc) || (__fourbyteints__ == 1)
#       else
#               define ZIL_WORD_SIZE    16
#       endif
#       define ZIL_OLDIO
#       if !defined(_NO_FAST_STRING_INLINES_)
#               define _NO_FAST_STRING_INLINES_ 1       // For Universal Headers version 2.0.1 and greater.
#       endif
#       include <MacHeaders.c>
#       if defined(__powerc)
#               pragma precompile_target "UI_Win_MPPC++"
#       else
#               pragma precompile_target "UI_Win_M68K++"
#       endif
#       undef TRUE
#       undef FALSE
#endif

// --------------------------------------------------------------------------
// ----- APPLE --------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(applec)
#       if defined(ZIL_UNICODE)
#               define ZIL_MACINTOSH    71
#       else
#               define ZIL_MACINTOSH    70
#       endif
#       define ZIL_EXPORT_CLASS
#       define ZIL_BIGENDIAN
#       undef ZIL_LINKBUG
#       define ZIL_OLDIO
#       define ZIL_NO_GEN_INIT
#       include <Files.h>
#       include <Fonts.h>
#       include <Lists.h>
#       include <Strings.h>
#       include <ToolUtils.h>
#       include <Types.h>
#       include <QuickDraw.h>
#       include <Scrap.h>
#       include <Resources.h>
#       include <IntlResources.h>
#       include <Desk.h>
#       include <DiskInit.h>
#       include <Printing.h>

        // Some things not defined by Apple C++
#       define topLeft(r)               (((Point *) &(r))[0])
#       define botRight(r)              (((Point *) &(r))[1])
#endif

// --------------------------------------------------------------------------
// ----- WATCOM -------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__WATCOMC__)
#       define ZIL_SIGNED_CHAR
#       define ZIL_VOIDP(pointer)       ((void *)(pointer))
#       if defined(ZIL_UNICODE)
#               include <stddef.h>
#               define ICHAR_T          wchar_t
#       endif
#       if defined(__OS2__)
#               define ZIL_OS2          20
#       elif defined(__WINDOWS__) ||  defined(__NT__) || defined(WIN32)  // last or added by jdh
#               define ZIL_MSWINDOWS    WINVER
#               undef ZIL_MSWINDOWS_CTL3D
#               if defined(WIN32)
#                       define ZIL_WINNT        WIN32
#               elif defined (__NT__)
#                       define ZIL_WINNT        __NT__
#               else
#                       define ZIL_FARDATA      far
#                       define ZIL_WORD_SIZE    16
#               endif
#               ifdef __DLL__
#                       undef ZIL_EXPORT_CLASS
#                       if defined(ZIL_WINNT)
#                               define ZIL_EXPORT_CLASS __export
#                       else
#                               define ZIL_EXPORT_CLASS _export
#                       endif
#               endif
#       elif defined(__DVX__)
#               define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#               define MSDOS
#       elif defined(__QNX__)
#               define ZIL_POSIX
#               define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
                struct _XDisplay;
                struct _XrmHashBucketRec;
                struct _XPrivate;
#       elif defined(__LINUX__)
#               define ZIL_LINUX
#               define ZIL_POSIX
#               if defined(ZIL_CURSES)
#                       include <curses.h>
#               else
#                       define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#               endif
#       else
#               define ZIL_MSDOS        20
#       endif
#endif

// --------------------------------------------------------------------------
// ----- DJGPP, GNU C++ port DOS (1.08) -------------------------------------
// --------------------------------------------------------------------------

#if defined(__GNUC__)
#       if defined(MSDOS)
#               define ZIL_SIGNED_CHAR
#               if defined(__DVX__)
#                       define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
                        struct _XDisplay;
                        struct XKeytrans;
                        struct _XrmHashBucketRec;
                        struct _XSQEvent;

                        extern "C"
                        {
#                       define LC_ALL      0
#                       define LC_COLLATE  1
#                       define LC_CTYPE    2
#                       define LC_MONETARY 3
#                       define LC_NUMERIC  4
#                       define LC_TIME     5
                        struct lconv
                        {
                                char *decimal_point;
                                char *thousands_sep;
                                char *grouping;
                                char *int_curr_symbol;
                                char *currency_symbol;
                                char *mon_decimal_point;
                                char *mon_thousands_sep;
                                char *mon_grouping;
                                char *positive_sign;
                                char *negative_sign;
                                char int_frac_digits;
                                char frac_digits;
                                char p_cs_precedes;
                                char p_sep_by_space;
                                char n_cs_precedes;
                                char n_sep_by_space;
                                char p_sign_posn;
                                char n_sign_posn;
                        };

                        struct lconv *localeconv( void );
                        char *setlocale( int category, const char *locale );
                        };
#               else
#                       define ZIL_MSDOS        20
#               endif
#       endif
#endif

// --------------------------------------------------------------------------
// ----- HP-UX, CC (cfront from HP) and Motif -------------------------------
// --------------------------------------------------------------------------

#if defined(__hpux)
#       define ZIL_POSIX
#       define ZIL_BIGENDIAN
#       if defined(ZIL_CURSES)
#               include <curses.h>
#               undef ZIL_SHADOW_BORDER
#               define ZIL_STANDARD_BORDER      // Standard double/single border (text).
#       else
#               define ZIL_LOAD_MOTIF           // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       endif
#endif

// --------------------------------------------------------------------------
// ----- IBM RS6000 ---------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(_IBM_RS6000)
#       define ZIL_POSIX
#       define ZIL_BIGENDIAN
#       if defined(ZIL_UNICODE)
#               define ICHAR_T wchar_t
#       endif
#       if defined(ZIL_CURSES)
#               define _AIX32_CURSES
//#             define ZIL_PC_KEYBRD
#               include <curses.h>
//               _VR3_COMPAT_CODE is defined by the 4.1 curses.h
#               if !defined(_VR3_COMPAT_CODE)
#                       define ACS_ULCORNER     '-'
#                       define ACS_URCORNER     '-'
#                       define ACS_LLCORNER     '-'
#                       define ACS_LRCORNER     '-'
#                       define ACS_HLINE        '-'
#                       define ACS_VLINE        '|'
#                       define KEY_CANCEL       -1      // KEY_BAD
#                       define KEY_HELP         -1      // KEY_BAD
#                       define KEY_MARK         -1      // KEY_BAD
#                       define KEY_RESUME       -1      // KEY_BAD
#                       define KEY_MOVE         -1      // KEY_BAD
#                       define KEY_SMOVE        -1      // KEY_BAD
#                       define KEY_COMMAND      -1      // KEY_BAD
#                       define KEY_SEXIT        -1      // KEY_BAD
#                       define KEY_SHELP        -1      // KEY_BAD
#                       define KEY_CLOSE        -1      // KEY_BAD
#                       define KEY_REFRESH      -1      // KEY_BAD
#                       define KEY_SNEXT        -1      // KEY_BAD
                        extern "C" nodelay(WINDOW * stdscr, int ch);
#               endif
                extern "C" beep();
                extern "C" WINDOW *initscr();
                extern "C" wclear(WINDOW * stdscr);
                extern "C" endwin();
                extern "C" waddch(WINDOW * stdscr, unsigned long tmp);
                extern "C" wrefresh(WINDOW * stdscr);
                extern "C" wmove(WINDOW * stdscr, int y, int x);
                extern "C" cbreak();
                extern "C" noecho();
                extern "C" nonl();
                extern "C" keypad(WINDOW * stdscr, int ch);
                extern "C" int wgetch(WINDOW * stdscr);
#               undef ZIL_SHADOW_BORDER
#               define ZIL_STANDARD_BORDER      // Standard double/single border (text).
#       else
#               define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       endif
#endif

// --------------------------------------------------------------------------
// ----- MS-DOS, Quarterdeck DESQview/X with Motif, DJGPP G++ ---------------
// --------------------------------------------------------------------------

#if defined(QDeckX)
#       define ZIL_POSIX
#       define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       define ZIL_SIGNED_CHAR
        extern "C"
        {
#               define LC_ALL      0
#               define LC_COLLATE  1
#               define LC_CTYPE    2
#               define LC_MONETARY 3
#               define LC_NUMERIC  4
#               define LC_TIME     5
        struct lconv
        {
                char *decimal_point;
                char *thousands_sep;
                char *grouping;
                char *int_curr_symbol;
                char *currency_symbol;
                char *mon_decimal_point;
                char *mon_thousands_sep;
                char *mon_grouping;
                char *positive_sign;
                char *negative_sign;
                char int_frac_digits;
                char frac_digits;
                char p_cs_precedes;
                char p_sep_by_space;
                char n_cs_precedes;
                char n_sep_by_space;
                char p_sign_posn;
                char n_sign_posn;
        };

        struct lconv *localeconv( void );
        char *setlocale( int category, const char *locale );
        };
#endif

// --------------------------------------------------------------------------
// ----- SCO UNIX 3.2 with Motif or Curses ----------------------------------
// --------------------------------------------------------------------------

#if defined(SCO_UNIX)
#       define ZIL_POSIX
#       if defined(ZIL_CURSES)
#               if defined(__ghs)
#                       define _cplusplus 1
#               endif
#               include <curses.h>
#               define ZIL_COLOR_CURSES
#               if defined(__ghs)
#                       undef columns
#                       undef lines
#                       if defined(clear)
#                               undef clear
#                       endif
#               endif
#               undef ZIL_SHADOW_BORDER
#               define ZIL_STANDARD_BORDER      // Standard double/single border (text).
#       else
#               define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       endif
#endif

// --------------------------------------------------------------------------
// ----- Silicon Graphics and Motif -----------------------------------------
// --------------------------------------------------------------------------

#if defined(__sgi)
#       define ZIL_POSIX
#       define ZIL_BIGENDIAN
#       define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       define ZIL_SIGNED_CHAR
#endif

// --------------------------------------------------------------------------
// ----- Solaris 2.x, CC (cfront from SunPro) and Motif ---------------------
// --------------------------------------------------------------------------

#if defined(sun)
#       define ZIL_POSIX
#       define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       define ZIL_SIGNED_CHAR
#       if defined(i386)
                // Solaris X86 for Intel.
#       else
#               define ZIL_BIGENDIAN
#               if defined(_SUNOS4)
//#                     define XtPointer caddr_t
#                       define XTFUNCPROTO
#                       define CFRONT
#                       define ZIL_OLDIO
#                       undef ZIL_SIGNED_CHAR
#               endif
#       endif
#endif

// --------------------------------------------------------------------------
// ----- Siemens/Nixdorf SINIX and Motif --------------------------
// --------------------------------------------------------------------------

#if defined(_SINIX)
#       define ZIL_POSIX
#       define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       if defined(mips)
#               include <stdlib.h>
#               define ZIL_BIGENDIAN
#               undef ZIL_SIGNED_CHAR
#       endif
#endif


// --------------------------------------------------------------------------
// ----- DEC 4000 OSF/1 3.2 -------------------------------------------------
// --------------------------------------------------------------------------

#if defined(__alpha) && !defined(WIN32)
#       define ZIL_POSIX
#       define ZIL_WORD_SIZE 64
#       if defined(ZIL_CURSES)
//#             define ZIL_PC_KEYBRD
#               include <curses.h>
#               define KEY_CANCEL       -1      // KEY_BAD
#               define KEY_MARK         -1      // KEY_BAD
#               define KEY_RESUME       -1      // KEY_BAD
#               define KEY_MOVE         -1      // KEY_BAD
#               define KEY_SMOVE        -1      // KEY_BAD
#               define KEY_SEXIT        -1      // KEY_BAD
#               define KEY_SHELP        -1      // KEY_BAD
#               define KEY_CLOSE        -1      // KEY_BAD
#               define KEY_REFRESH      -1      // KEY_BAD
#               define KEY_SNEXT        -1      // KEY_BAD
#               define ACS_LARROW       '<'
#               define ACS_RARROW       '>'
#               define ACS_DARROW       'v'
#               define ACS_UARROW       '^'
#               define ACS_BLOCK        '#'
                extern "C" nodelay(WINDOW * stdscr, int ch);
                extern "C" beep();
                extern "C" WINDOW *initscr();
                extern "C" wclear(WINDOW * stdscr);
                extern "C" endwin();
                extern "C" waddch(WINDOW * stdscr, unsigned long tmp);
                extern "C" wrefresh(WINDOW * stdscr);
                extern "C" wmove(WINDOW * stdscr, int y, int x);
                extern "C" cbreak();
                extern "C" noecho();
                extern "C" nonl();
                extern "C" keypad(WINDOW * stdscr, int ch);
                extern "C" int wgetch(WINDOW * stdscr);
#               undef ZIL_SHADOW_BORDER
#               define ZIL_STANDARD_BORDER      // Standard double/single border (text).
#       else
#               define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#       endif
#endif

// --------------------------------------------------------------------------
// ----- NEXTSTEP  3.x -------------------------------------------------
// --------------------------------------------------------------------------
#if defined(__NeXT__)
#       define ZIL_POSIX
#       define ZIL_NEXTSTEP
#       if defined(__BIG_ENDIAN__)
#               define ZIL_BIGENDIAN
#       endif
#       define ZIL_SIGNED_CHAR
#       define ZIL_MAXPATHLEN   1024
#       define ZIL_PATHSEP      ':'
#       define ZIL_DIRSEP       '/'
#       define ZIL_DIRSEPSTR    "/"
#       define ZIL_EXT  ".dat"
#       define ZIL_BAK  ".bk?"
//  Compatibility with 3.5 Beta
#       define ZIL_UNIX
#endif

//-------------------------------------------------------------------------
//-------------Linux-------------------------------------------------------
//---section added by jdh--------------------------------------------------

// following define added by jdh to allow compilation under linux
#if defined(__linux__)
#       define ZIL_LINUX
#       define ZIL_POSIX
#       define ZIL_LOAD_MOTIF   // Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#endif



// ----- Compiler/Environment Default Dependencies --------------------------
#if !defined(ZIL_FARDATA)
#       define ZIL_FARDATA
#endif
#if !defined(ZIL_BIGENDIAN)
#       define ZIL_LITTLEENDIAN
#endif
#if !defined(ZIL_WORD_SIZE)
#       define ZIL_WORD_SIZE    32
#endif

#if defined(DOSX286) || defined(DOS16PP) || defined(DOS16M)
#       define ZIL_DOSEXTENDED16
#endif
#if defined(DOS32PP) || defined(DOS386)
#       define ZIL_DOSEXTENDED32
#endif
#if defined(ZIL_DOSEXTENDED16) || defined(ZIL_DOSEXTENDED32)
#       define ZIL_DOSEXTENDED
#endif

#if !defined(ZIL_VOIDF)
#       define ZIL_VOIDF(function)      (function)
#endif
//as is this macro requires specific casts for files generated by designer
//this can be circumvented by alternate macros or inline functions
//something like
//inline UI_WINDOW_OBJECT* ZIL_VOIDF(&functionx){return funtionx;};
//altough this has a syntax error and won't get past the compiler
//needs work, area for study
#if !defined(ZIL_VOIDP)
#       define ZIL_VOIDP(pointer)       (pointer)
#endif

#if defined(ZIL_MSDOS)
#       define ZIL_MAXPATHLEN   128
#       define ZIL_MAXNAMELEN   64
#       define ZIL_PATHSEP              ';'
#       define ZIL_EXT                  ".DAT"
#       define ZIL_BAK                  ".BK?"
#endif

#if defined(ZIL_MSWINDOWS)
#       if defined(ZIL_WINNT) && defined(ZIL_UNICODE)
#               define UNICODE
#       endif
#       if !(defined(__BCPLUSPLUS__) && __BORLANDC__ >= 0X0500)
extern "C"
{
#       endif
#       if defined(ZIL_WIN32)
#               define WINVER 0x0400
#               include <windows.h>
#               if __BORLANDC__ >= 0x0500
#                       include <commctrl.h>
#               else
#                       include <z_comctl.h>
#               endif
#       else
#               include <windows.h>
#       endif
#       if !(defined(__BCPLUSPLUS__) && __BORLANDC__ >= 0X0500)
}
#       endif
#       if defined(ZIL_WINNT) && defined(__WATCOMC__)
#               undef _MSC_VER
#       endif
#       if defined(STRICT)
#               define WINDOWSPROC      WNDPROC
#       else
#               define WINDOWSPROC      FARPROC
#       endif
#       define ZIL_MAXPATHLEN   128
#       define ZIL_MAXNAMELEN   64
#       define ZIL_PATHSEP              ';'
#       define ZIL_EXT                  ".DAT"
#       define ZIL_BAK                  ".BK?"
#endif

#if defined(ZIL_OS2)
#       define HIWORD(arg) HIUSHORT(arg)
#       define LOWORD(arg) LOUSHORT(arg)
        extern "C"
        {
#       include <os2.h>
        }
#       define ZIL_MAXPATHLEN   128
#       define ZIL_MAXNAMELEN   64
#       define ZIL_PATHSEP              ';'
#       define ZIL_EXT                  ".DAT"
#       define ZIL_BAK                  ".BK?"
#endif

#if defined(ZIL_POSIX)
        extern "C"
        {
                void cursorRemove(void);
                void cursorPosition(int, int, int);
        }
#       define ZIL_MAXPATHLEN   1024
#       define ZIL_MAXNAMELEN   64
#       define ZIL_PATHSEP              ':'
#       define ZIL_EXT                  ".dat"
#       define ZIL_BAK                  ".bk?"
//  Compatibility with 3.5 Beta
#   define ZIL_UNIX
#endif

#if defined(__DVX__)
#       define ZIL_MAXPATHLEN   128
#       define ZIL_MAXNAMELEN   64
#       define ZIL_PATHSEP              ';'
#       define ZIL_EXT                  ".dat"
#       define ZIL_BAK                  ".bk?"
#endif

#if defined(ZIL_MACINTOSH)
#       if defined(Length)
#               undef Length                                    // Length() is used in ZIL_FILE, and in Universal Headers 2.0.
#       endif
#       define ZIL_MAXPATHLEN   1023
#       define ZIL_MAXNAMELEN   64
#       define ZIL_PATHSEP              ';'
#       define ZIL_EXT                  ".dat"
#       define ZIL_BAK                  ".bk?"

#       if defined(ZIL_NO_GEN_INIT)
                // Use these defines if compiler has no General Initializer support.
#               define ZIL_DEFINE_CELL(name, pV, pH) \
                        Cell name; \
                        name.v = (pV); \
                        name.h = (pH);
#               define ZIL_DEFINE_POINT(name, pV, pH) \
                        Point name; \
                        name.v = (pV); \
                        name.h = (pH);
#               define ZIL_DEFINE_RECT(name, rTop, rLeft, rBottom, rRight) \
                        Rect name; \
                        name.top = (rTop); \
                        name.left = (rLeft); \
                        name.bottom = (rBottom); \
                        name.right = (rRight);
#       else
#               define ZIL_DEFINE_CELL(name, pV, pH) \
                        Cell name = { pV, pH };
#               define ZIL_DEFINE_POINT(name, pV, pH) \
                        Point name = { pV, pH };
#               define ZIL_DEFINE_RECT(name, rTop, rLeft, rBottom, rRight) \
                        Rect name = { rTop, rLeft, rBottom, rRight };
#       endif
#endif

// ----- Compiler/Hardware Typedef Sizes ------------------------------------
#if (ZIL_WORD_SIZE == 16)
        typedef short                   ZIL_INT16;
        typedef unsigned short  ZIL_UINT16;
        typedef long                    ZIL_INT32;
        typedef unsigned long   ZIL_UINT32;
#elif (ZIL_WORD_SIZE == 32)
        typedef short                   ZIL_INT16;
        typedef unsigned short  ZIL_UINT16;
        typedef long                    ZIL_INT32;
        typedef unsigned long   ZIL_UINT32;
//#define       ZIL_UINT32      unsigned long;
#elif (ZIL_WORD_SIZE == 64)
        typedef short                   ZIL_INT16;
        typedef unsigned short  ZIL_UINT16;
        typedef int                             ZIL_INT32;
        typedef unsigned int    ZIL_UINT32;
#endif

#if defined(ZIL_SIGNED_CHAR)
        // Undefine ZIL_SIGNED_CHAR if your compiler doesn't like 'signed char foo'.
        typedef signed char             ZIL_INT8;
#else
        typedef char                    ZIL_INT8;
#endif
        typedef unsigned char   ZIL_UINT8;

#if defined(ICHAR_T)
        typedef ICHAR_T                 ZIL_ICHAR;
#elif defined(ZIL_UNICODE)
        typedef ZIL_UINT16              ZIL_ICHAR;
#else
        typedef char                    ZIL_ICHAR;
#endif

// ----- Motif Setup --------------------------------------------------------
#if defined (ZIL_LOAD_MOTIF)
        extern "C"
        {
#       include <Xm/Xm.h>
        }

#       if defined(XmVersion)
#               define ZIL_MOTIF        XmVersion
#       else
#               define ZIL_MOTIF        1001
#       endif
#       undef   ZIL_LOAD_MOTIF
#endif

// ----- X and Xt Setup -----------------------------------------------------
#if defined(ZIL_MOTIF)
#       if defined(XlibSpecificationRelease)
#               define ZIL_X11          XlibSpecificationRelease
#       elif defined(__linux__)
#               define ZIL_X11          6
#       else
#               define ZIL_X11          4
#       endif
#       if defined(XtSpecificationRelease)
#               define ZIL_XT           XtSpecificationRelease
#       elif defined(__linux__)
#               define ZIL_XT           6
#       else
#               define ZIL_XT           4
#       endif
#endif

#if defined(ZIL_X11)
        typedef unsigned long ZIL_RAW_CODE;
#else
        typedef unsigned ZIL_RAW_CODE;
#endif

#define ZIL_HOTMARK                     '&'
#define ZIL_HOTMARKSTR          "&"

// ----- TRUE/FALSE ---------------------------------------------------------

#if !defined(TRUE)
        const int TRUE = 1;
        const int FALSE = 0;
#endif

// ----- UIF_FLAGS ----------------------------------------------------------

typedef ZIL_UINT16 UIF_FLAGS;
const UIF_FLAGS UIF_NO_FLAGS                    = 0x0000;

// ----- UIS_STATUS ---------------------------------------------------------

typedef ZIL_UINT16 UIS_STATUS;
const UIS_STATUS UIS_NO_FLAGS                   = 0x0000;

// ----- Macros -------------------------------------------------------------

#define FlagSet(flag1, flag2) ((flag1) & (flag2))
#define FlagsSet(flag1, flag2) (((flag1) & (flag2)) == (flag2))
#define MaxValue(arg1, arg2) (((arg1) > (arg2)) ? (arg1) : (arg2))
#define MinValue(arg1, arg2) (((arg1) < (arg2)) ? (arg1) : (arg2))
#define AbsValue(arg) ((arg) > 0 ? (arg) : -(arg))
#if defined(ZIL_UNICODE)
#       define ZIL_TEXT(x)      L ## x
#else
#       define ZIL_TEXT(x)      x
#endif

// ----- DOS Multiple hardware support --------------------------------------
#if defined(ZIL_HARDWARE) && defined(ZIL_MSDOS)
#       define _i2DoName(a, b)          a ## b
#       define _i1DoName(a, b)          _i2DoName(a, b)
#       define I_MAKENAME(name)         _i1DoName(ZIL_MODULE, name)
#       define MAKE_SETFUNCTIONS        \
        void I_MAKENAME(I_SetFunctions)(void) \
        { \
                _textDriverName = I_MAKENAME(_textDriverName); \
                I_CheckLeadIn = I_MAKENAME(I_CheckLeadIn); \
                I_ScreenOpen = I_MAKENAME(I_ScreenOpen); \
                I_ScreenClose = I_MAKENAME(I_ScreenClose); \
                I_ScreenPut = I_MAKENAME(I_ScreenPut); \
                I_CursorRemove = I_MAKENAME(I_CursorRemove); \
                I_CursorPosition = I_MAKENAME(I_CursorPosition); \
                I_GetCodePage = I_MAKENAME(I_GetCodePage); \
                I_Beep = I_MAKENAME(I_Beep); \
                \
                I_KeyboardOpen = I_MAKENAME(I_KeyboardOpen); \
                I_KeyboardClose = I_MAKENAME(I_KeyboardClose); \
                I_KeyboardRead = I_MAKENAME(I_KeyboardRead); \
                I_KeyboardQuery = I_MAKENAME(I_KeyboardQuery); \
                \
                I_MouseOpen = I_MAKENAME(I_MouseOpen); \
                I_MouseClose = I_MAKENAME(I_MouseClose); \
                \
                I_PrintDotMatrixChar = I_MAKENAME(I_PrintDotMatrixChar); \
        }
        extern ZIL_ICHAR *_textDriverName;
        extern int *(*I_CheckLeadIn)(void);
        extern void (*I_ScreenOpen)(int *, int *, int *);
        extern void (*I_ScreenClose)(void);
        extern void (*I_ScreenPut)(int, int, int, int, void *);
        extern void (*I_CursorRemove)(void);
        extern void (*I_CursorPosition)(int, int, int);
        extern int (*I_GetCodePage)(void);
        extern void (*I_Beep)(void);

        extern void (*I_KeyboardOpen)(void);
        extern void (*I_KeyboardClose)(void);
        extern void (*I_KeyboardRead)(unsigned *, unsigned *, unsigned *);
        extern int (*I_KeyboardQuery)(unsigned *);

        extern int (*I_MouseOpen)(void);
        extern void (*I_MouseClose)(void);
        extern void (*mouseQueueEvent)(unsigned, int *, int *);
        extern void (*I_PrintDotMatrixChar)(char);
#elif defined(ZIL_MSDOS) || defined(ZIL_CURSES)
#       define I_MAKENAME(name)         name
#       define MAKE_SETFUNCTIONS
        extern ZIL_ICHAR _textDriverName[];
        extern int *I_CheckLeadIn(void);
        extern void I_ScreenOpen(int *, int *, int *);
        extern void I_ScreenClose(void);
        extern void I_ScreenPut(int, int, int, int, void *);
        extern void I_CursorRemove(void);
        extern void I_CursorPosition(int, int, int);
        extern int I_GetCodePage(void);
        extern void I_Beep(void);

        extern void I_KeyboardOpen(void);
        extern void I_KeyboardClose(void);
        extern void I_KeyboardRead(unsigned *, unsigned *, unsigned *);
        extern int I_KeyboardQuery(unsigned *);

        extern int I_MouseOpen(void);
        extern void I_MouseClose(void);
        extern void (*mouseQueueEvent)(unsigned, int *, int *);
        extern void I_PrintDotMatrixChar(char);
#endif

// --- Version 3.6, 3.5, 3.0 compatibility ---
#if defined(ZIL_OLD_DEFS)
#       if !defined(NULL) && (defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__))
#       define NULL    0
#       elif !defined(NULL)
#       define NULL    0L
#       endif
#define NULLP   ZIL_NULLP
#define NULLF   ZIL_NULLF
#define NULLH   ZIL_NULLH
#define VOIDF   ZIL_VOIDF
#define VOIDP   ZIL_VOIDP
#define EXPORT  ZIL_EXPORT_CLASS

#define int8    ZIL_INT8
#define int16   ZIL_INT16
#define int32   ZIL_INT32
#define uint8   ZIL_UINT8
#define uint16  ZIL_UINT16
#define uint32  ZIL_UINT32
#define ichar_t ZIL_ICHAR

#define Max             MaxValue
#define Min             MinValue
#define Abs             AbsValue

#       if !defined(UCHAR)
                typedef unsigned char UCHAR;
#       endif
#       if !defined(USHORT)
                typedef unsigned short USHORT;
#       endif
#       if !defined(ULONG)
                typedef unsigned long ULONG;
#       endif
#endif

#       if !defined(__MWERKS__)
        extern "C"
        {
#       endif
#               include <string.h>
#       if !defined(__MWERKS__)
        }
#       endif
#endif
