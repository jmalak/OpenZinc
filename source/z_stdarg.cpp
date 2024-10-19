//      Zinc Interface Library - Z_STDARG.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#       include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_gen.hpp"
#include "z_stdarg.hpp"

enum type
{
        Z_INT, Z_LONG, Z_DOUBLE, Z_CHAR_STAR
};

typedef struct
{
        enum type type;
        int newPos, oldPos;
} rdata;

#if defined(__ZTC__) && (__ZTC__ >= 0x310) && !defined(ZIL_MACINTOSH)
        static int __CLIB rdatacmp(const void *a, const void *b)
#elif defined(__ZTC__)
        extern "C" {
        static int rdatacmp(const void *a, const void *b)
#elif defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
        extern "C" {
        int rdatacmp(const void *a, const void *b)
#else
        static int rdatacmp(const void *a, const void *b)
#endif
        {
                return ((rdata *)a)->newPos - ((rdata *)b)->newPos;
        }
#if (defined(__ZTC__) && ((__ZTC__ < 0x310) || defined(ZIL_MACINTOSH))) || defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
        }
#endif

void ZIL_STDARG::RearrangeArgs(int isScanf, void *newBuffer,
                              const ZIL_ICHAR *format, const va_list args,
                              ZIL_ICHAR *newFormat, va_list *newArgs)
{
        int i, j, islonger;
        rdata *tbl = new rdata[ZIL_MAXPARAMLEN];
        int orgpos, first, last;
        const ZIL_ICHAR *fmtStart;

        orgpos = 0;
        last = first = 0;
        for (i=0; i < ZIL_MAXPARAMLEN; i++)
        {
                tbl[i].type = Z_CHAR_STAR;
                tbl[i].oldPos = i;
                tbl[i].newPos = -1;
        }
        i = 0;
        while (*format)
        {
                if (*format == '%' && format[1] == '%')
                        newFormat[i++] = *format++;
                else if (*format == '%')
                {
                        fmtStart = format;
                        format++;
                        /* scan out the position */
                        j = 0;
                        while (IsDigit(*format))
                        {
                                j *= 10;
                                j += *format++ - '0';
                        }
                        j--;    // The position starts at 1
                        if (j < 0 || j >= ZIL_MAXPARAMLEN || *format != '%')
                        {
                                // no (valid) positional information
                                j = first++;
                                format = fmtStart;
                        }
                        newFormat[i++] = *format++;
                        while (*format == '-' || *format == '+' ||
                               *format == '#' || *format == ' ')
                                newFormat[i++] = *format++;
                        if (*format == '*')
                        {
                                newFormat[i++] = *format++;
                                tbl[j].type = Z_INT;
                                tbl[j].newPos = orgpos;
                                tbl[j].oldPos = j;
                                if (j > last) last = j;
                                orgpos++;
                                j++;
                                first++;
                        }
                        else if (IsDigit(*format))
                        {
                                while (IsDigit(*format))
                                        newFormat[i++] = *format++;
                        }
                        if (*format == '.')
                        {
                                newFormat[i++] = *format++;
#if defined(TESTING)
                                if (!IsDigit(*format))
                                        abort();        /*raise(badformat)*/
#endif
                                if (*format == '*')
                                {
                                        newFormat[i++] = *format++;
                                        tbl[j].type = Z_INT;
                                        tbl[j].newPos = orgpos;
                                        tbl[j].oldPos = j;
                                        if (j > last) last = j;
                                        orgpos++;
                                        j++;
                                        first++;
                                }
                                else
                                        while (IsDigit(*format))
                                                newFormat[i++] = *format++;
                        }
                        islonger = 0;
                rflag:  switch (*format)
                        {
                        case 'l':
                                islonger = 1;
                                // Fall through
                        case 'L':
                        case 'h':
                        case 'a':
                                newFormat[i++] = *format++;
                                goto rflag;
                        }
                        /*???? I think I only care about 'l' and 'L' */
                        /* and either F or N depending on memory model */
                        switch (*format)
                        {
                        case 'd': case 'i': case 'o': case 'u': case 'x':
                        case 'X': case 'c':
                                tbl[j].type = (islonger ? Z_LONG : Z_INT);
                                tbl[j].newPos = orgpos;
                                tbl[j].oldPos = j;
                                if (j > last) last = j;
                                orgpos++;
                                break;
                        case 'f': case 'e': case 'g': case 'E': case 'G':
                                tbl[j].type = Z_DOUBLE;
                                tbl[j].newPos = orgpos;
                                tbl[j].oldPos = j;
                                if (j > last) last = j;
                                orgpos++;
                                break;
                        case 's': case 'n': case 'p':
                                tbl[j].type = Z_CHAR_STAR;
                                tbl[j].newPos = orgpos;
                                tbl[j].oldPos = j;
                                if (j > last) last = j;
                                orgpos++;
                                break;
                        case '[':
                                if (isScanf)
                                {
                                        // Copy across the [...] string
                                        newFormat[i++] = *format++;
                                        if (*format == '^')
                                                newFormat[i++] = *format++;
                                        // special for special ]
                                        if (*format == ']')
                                                newFormat[i++] = *format++;
                                        while (*format != ']')
                                                newFormat[i++] = *format++;

                                        tbl[j].type = Z_CHAR_STAR;
                                        tbl[j].newPos = orgpos;
                                        tbl[j].oldPos = j;
                                        if (j > last) last = j;
                                        orgpos++;
                                        break;
                                }
                                break;
                        default:
                                break;
                        }
                }
                newFormat[i++] = *format++;
        }
        last++;
        newFormat[i] = '\0';
        // If this is scanf(), all parameters are (should be) pointers
        if (isScanf)
                for (i=0; i < last; i++)
                        tbl[i].type = Z_CHAR_STAR;

#if defined(__ZTC__) && (__ZTC__ >= 0x0310) && !defined(ZIL_MACINTOSH)
 //       extern void __CLIB qsort(void *,size_t,size_t, int (__CLIB *)(const void *,const void *));
#endif
        qsort(tbl, last, sizeof(tbl[0]), rdatacmp);
#if defined(TESTING)
        for (i=1; i < last; i++)
                if (tbl[i].newPos == tbl[i-1].newPos-1)
                        abort();        /*raise(badformat)*/
#endif
#if defined(__WATCOMC__)
        va_list toVlist;
        toVlist[0] = (*newArgs)[0] = (char *)newBuffer;
#elif defined(__DECCXX)
        va_list toVlist;
        toVlist._a0 = (char **)((char *)newBuffer + sizeof(va_list));
        toVlist._offset = 0;
        *((va_list *)newBuffer) = toVlist;
        *newArgs = toVlist;
#elif _ALPHA_ == 1
        va_list toVlist;
        toVlist.a0 = (char *)newBuffer + sizeof(va_list);
        toVlist.offset = 0;
        *((va_list *)newBuffer) = toVlist;
        *newArgs = toVlist;
#else
        va_list toVlist = *newArgs = (va_list)newBuffer;

        // This handles those machines (like the PA-RISC) that runs varargs
        // from high to low memory rather than from low to high memory.
        (void) va_arg(toVlist, int);
        if (toVlist < *newArgs)
        {
                *newArgs = (va_list)((char *)newBuffer + ZIL_MAXPARAMLEN);
        }
        toVlist = *newArgs;
#endif
        for (i=0; i < last; i++)
        {
                if (tbl[i].newPos < 0) continue;
#if defined(__WATCOMC__)
                va_list fromVlist;
                fromVlist[0] = args[0];
#else
                va_list fromVlist = args;
#endif
                for (j=0; j < last; j++)
                {
                        if (tbl[j].oldPos >= tbl[i].oldPos) continue;
                        switch (tbl[j].type)
                        {
                        case Z_INT:
                                (void) va_arg(fromVlist, int);
                                break;
                        case Z_LONG:
                                (void) va_arg(fromVlist, long);
                                break;
                        case Z_DOUBLE:
                                (void) va_arg(fromVlist, double);
                                break;
                        case Z_CHAR_STAR:
                                (void) va_arg(fromVlist, ZIL_ICHAR *);
                                break;
                        };
                }
                switch (tbl[i].type)
                {
                case Z_INT:
                        {
#if defined(__GNUC__)
                                memcpy(toVlist, fromVlist, sizeof(int));
                                (void)va_arg(toVlist, int);
                                (void)va_arg(fromVlist, int);
#else
                                va_arg(toVlist, int) = va_arg(fromVlist, int);
#endif
                        }
                        break;
                case Z_LONG:
                        {
#if defined(__GNUC__)
                                memcpy(toVlist, fromVlist, sizeof(long));
                                (void)va_arg(toVlist, long);
                                (void)va_arg(fromVlist, long);
#else
                                va_arg(toVlist, long) = va_arg(fromVlist, long);
#endif
                        }
                        break;
                case Z_DOUBLE:
#if (defined (ZIL_MSDOS) && !defined (__WATCOMC__)) || defined(__GNUC__)
                        memcpy(toVlist, fromVlist, sizeof(double));
                        (void)va_arg(toVlist, double);
                        (void)va_arg(fromVlist, double);
#else
                        va_arg(toVlist, double) = va_arg(fromVlist, double);
#endif
                        break;
                case Z_CHAR_STAR:
                        {
#if defined(__GNUC__)
                                memcpy(toVlist, fromVlist, sizeof(ZIL_ICHAR *));
                                (void)va_arg(toVlist, ZIL_ICHAR *);
                                (void)va_arg(fromVlist, ZIL_ICHAR *);
#else
                                long ptr = (long) va_arg(fromVlist, ZIL_ICHAR *);
                                va_arg(toVlist, ZIL_ICHAR *) = (ZIL_ICHAR *)ptr;
#endif
                        }
                        break;
                };
        }
        delete []tbl;
}
