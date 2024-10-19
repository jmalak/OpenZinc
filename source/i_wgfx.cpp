//	Zinc Interface Library - I_WGFX.CPP
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


#include "ui_evt.hpp"
#define DELAYED_READ

// GFX support routines for Unicode fonts.

#define NEAR
extern "C"
{
#	include "gfx/source/gfx.h"
}
#include "gfx/source/gfx_err.h"
#undef NULL
#include "gfx/source/gfx_src.h"
#include "gfx/source/gfx_pro.h"
typedef struct fc {int x, y;			   /* physical coordinate */
			    int ilog_x, ilog_y;     /* integer logical coordinate */
			    double flog_x, flog_y;  /* floating pt logical coordinate */
			    int delta_x, delta_y;   /* length of string on screen */
			    } FCRSR;
#include "gfx/source/gfxg_src.h"
#include "gfx/source/gfxg_pro.h"
#include "gfx/source/gfxlopro.h"
extern "C"
{
IMPORT LINE_ATTR NEAR gfx_line_attr;
IMPORT CRSR *gfx_crsr;
IMPORT void (_pascal *rs_flog)(void);
IMPORT void  _pascal _gfx_solid_box(int x1, int y1, int x2, int y2, int color);
}

typedef ZIL_UINT32 Offset_t;
const ZIL_UINT16 replaceChar = 0xfffd;

typedef struct _wfont {
	Offset_t **offset_table;	/* bit pointer scan line of characters */
	ZIL_UINT8 **data_table;	/* pointer to the character bitmap */
	ZIL_UINT16 width;		/* the # of bytes in each row of the bitmap */
	ZIL_UINT16 height;		/* the # of rows in the bitmap */
	ZIL_UINT16 lo_char;		/* the first character in the bitmap */
	ZIL_UINT16 hi_char;		/* the last character in the bitmap */
	ZIL_UINT16 widest_cell;	/* the widest character, in bits */
	int ascent_distance;		/* distance from baseline to top of char */
	int underline_loc;		/* distance from baseline to the underline */
	int fixed_width_char;		/* are the characters a fixed width? */
	FONT_ATTR attr;			/* see the structure above */
#if defined(DELAYED_READ)
	int dos_handle;
	long font_data;
	long char_offset_table;
#endif
} WFONT;

WFONT *wFont;
static FCRSR wFont_crsr;
#define MAX_FONTS	5
static WFONT *fonts[MAX_FONTS] = { 0, 0, 0, 0, 0 };
const ZIL_UINT16 SHIFT = 8;
const ZIL_UINT16 MASK  = 0xFF00;
#define REST_OF(x)	((ZIL_UINT16)~(x))

/*~ LO_PRINT.C */

#if defined(DELAYED_READ)
static char ZIL_FARDATA inMemory[REST_OF(MASK) + 1];

static ZIL_ICHAR GetCharPage(ZIL_ICHAR c)
{
	if (inMemory[c >> SHIFT])
		return (c);
	if (UI_DEVICE::eventManager)
		UI_DEVICE::eventManager->DeviceState(E_MOUSE, DM_WAIT);
	ZIL_ICHAR baseChar = c & MASK;
	// Read the offset bit pointers
	gfx_lseek(wFont->dos_handle, wFont->char_offset_table+(unsigned long)baseChar*sizeof(Offset_t), 0);
	gfx_read(wFont->offset_table[baseChar >> SHIFT], (REST_OF(MASK)+1)*sizeof(Offset_t), wFont->dos_handle);
	if (baseChar != MASK)
		gfx_read(wFont->offset_table[(baseChar >> SHIFT)+1], sizeof(Offset_t), wFont->dos_handle);
	// Read the bit maps
	long start = (wFont->offset_table[(baseChar >> SHIFT)][0] / 8);
	long stop  = (baseChar == MASK ? (wFont->offset_table[(baseChar >> SHIFT)][REST_OF(MASK)] / 8) :
		      (wFont->offset_table[(baseChar >> SHIFT) + 1][0]  / 8));
	for (int i=0; i < wFont->height; i++)
	{
		// seek to bitmap;
		gfx_lseek(wFont->dos_handle, wFont->font_data+start+(unsigned long)wFont->width*i, 0);
		// read bitmap;
		gfx_read(&wFont->data_table[i][start], (unsigned) (stop-start+1), wFont->dos_handle);
	}
	inMemory[c >> SHIFT] = TRUE;
	if (UI_DEVICE::eventManager)
		UI_DEVICE::eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (c);
}
#endif

void AFdecl (**gfx_rotate_fcns)(unsigned char *, int, int, int, char *, int, int);
void AFdecl (*gfx_expand_fcn)(unsigned char *, int, int, int, char *, int, int);

#define is_horizontal_run()		(gfx_line_attr.text_dir < TEXT_B_TO_T)

static int med_res_color[] = {0, 0x55, 0xAA, 0xFF};
static char text_dir_CCW[] = {3, 2, 0, 1};
static char text_dir_CW[]  = {2, 3, 1, 0};

int Wgfx_print_font(const ZIL_ICHAR *tp, int max_char)
{
	Offset_t char_bit_offset, next_char_bit_offset;
	int n_char_pels;
	ZIL_UINT16 screen_offset;
	ZIL_UINT8 *char_bitmap;
	ZIL_UINT8 *real_char_bitmap;
	DMEM_HANDLE real_char_bitmap_mc;
	ZIL_ICHAR c;
	char *rotate_buf, *expand_buf;
	DMEM_HANDLE rotate_buf_mc;
	DMEM_HANDLE expand_buf_mc;
	int i, clip_n_top_rows, clip_n_bottom_rows, delta_top, delta_bottom;
	int pt_x, row_y, top_pad_rows, n_dest_rows, n_source_col;
	int diff, tdiff, lpad, rpad, form_width, show_n_rows, n_char_rows, rotate_char;
	int fgnd_color, bkgnd_color, u_color, u_line, reset_x, reset_y, text_dir;
	int expand_rows, expand_width, expand_char, x, y, temp_buf_size;
	int new_form_width, j, k;
	int nonSpacing;

	if (!*tp) return SUCCESS;
	x = wFont_crsr.x;
	y = wFont_crsr.y;
	reset_x = reset_y = 0;
text_dir = gfx_line_attr.text_dir;
rotate_char = wFont->attr.rotate_char;
if (_gfx.sys_flags & ROTATE_SCREEN) {
	if (_gfx.sys_flags & ROTATE_SCREEN_90) {
		text_dir = text_dir_CCW[text_dir];
		rotate_char = (rotate_char + 1) & 3;
          }
	if (_gfx.sys_flags & ROTATE_SCREEN_270) {
		text_dir = text_dir_CW[text_dir];
		rotate_char = (rotate_char + 3) & 3;
		}
	}


	/*  If we need to underline or set a background color for the text,
	 *  or center or end justify it, we'll need to know how long the text
	 *  is.  Furthermore, if we need to justify the text, we'll also need
	 *  to set a new starting coordinate.
	 */

	if (((gfx_line_attr.justify != JUSTIFY_START) && gfx_line_attr.display_char) ||
	    (gfx_line_attr.display_char && ((wFont->attr.bkgnd_color != TRANSPARENT) || gfx_line_attr.display_underline))) {
		gfx_line_attr.display_char = NO;
		Wgfx_print_font(tp, max_char);
		gfx_line_attr.display_char = YES;
		if (wFont_crsr.delta_x + wFont_crsr.delta_y == 0) return SUCCESS;
		if (gfx_line_attr.justify != JUSTIFY_START) {
			reset_x = wFont_crsr.delta_x;
			reset_y = wFont_crsr.delta_y;
			if (gfx_line_attr.justify == JUSTIFY_CENTER) {
				reset_x /= 2;
				reset_y /= 2;
			}
			if (reset_x) {
				x -= reset_x;
				wFont_crsr.x = x + reset_x;
				wFont_crsr.ilog_x += _gfx_xlat_int_coor(reset_x, _gfx.scale.x_denom, _gfx.scale.x_num);
			}
			if (wFont_crsr.delta_y) {
				y -= reset_y;
				wFont_crsr.y = y + reset_y;
				wFont_crsr.ilog_y += _gfx_xlat_int_coor(reset_y, _gfx.scale.y_denom, _gfx.scale.y_num);
			}
		}
	}

	/* Get the alignment adjustment value */

	switch (wFont->attr.align) {
	case TOP_LINE:
		delta_top = 0;
		break;
	case MID_LINE:
		delta_top = wFont->height / 2;
		break;
	case BASE_LINE:
		delta_top = wFont->ascent_distance;
		break;
	case BOTTOM_LINE:
		delta_top = wFont->height - 1;
	}
	delta_bottom = wFont->height - (delta_top + 1);
	expand_rows =	wFont->attr.expand_rows;
	expand_width = wFont->attr.expand_width;
	expand_char = (wFont->attr.expand_rows + wFont->attr.expand_width) - 2;

	/* Get new offsets, depending on the character's rotation */

	switch (wFont->attr.rotate_char) {
	case _0_DEGREES:
		if (!reset_y) y -= delta_top * expand_rows;
		break;
	case _90_DEGREES:
		if (!reset_x) x -= delta_top * expand_rows;
		goto _270;
	case _180_DEGREES:
		if (!reset_y && (text_dir != TEXT_B_TO_T))
			y -= delta_bottom * expand_rows;
		break;
	case _270_DEGREES:
		if (!reset_x) x -= delta_bottom * expand_rows;
	_270:	expand_rows =	wFont->attr.expand_width;
		expand_width = wFont->attr.expand_rows;
		break;
	}

	/*  The following routines will draw the background color or underline
	 *  and only need to be called if characters are going to be written.
	 */

	if (gfx_line_attr.display_char) {
		if (wFont->attr.bkgnd_color != TRANSPARENT) {
			bkgnd_color = _gfx_get_color(wFont->attr.bkgnd_color | FILL_SOLID);
			if (is_horizontal_run())
				_gfx_solid_box(x, y, x+(wFont_crsr.delta_x-1), y+(wFont->height-1)*expand_rows, bkgnd_color);
			else
				_gfx_solid_box(x, y, x+(wFont->height-1)*expand_width, y+(wFont_crsr.delta_y-1), bkgnd_color);
		}
		if (gfx_line_attr.display_underline) {
			u_line = (wFont->underline_loc + wFont->attr.u_pos) * expand_rows;
			u_color = _gfx_get_color(wFont->attr.u_color | FILL_SOLID);
			switch (rotate_char) {
			case _0_DEGREES:
			case _180_DEGREES:
				if (wFont->attr.rotate_char == _180_DEGREES)
					u_line = -u_line;
				_gfx_solid_box(x, y+u_line, x+wFont_crsr.delta_x, y+u_line+wFont->attr.u_thickness-1, u_color);
				break;
			case _90_DEGREES:
			case _270_DEGREES:
				if (wFont->attr.rotate_char == _270_DEGREES)
					u_line = -u_line;
				_gfx_solid_box(x+u_line, y, x+u_line+wFont->attr.u_thickness-1, y+wFont_crsr.delta_y, u_color);
				break;
			}
		}
		if (rotate_char || expand_char) {
			temp_buf_size = ((wFont->widest_cell + 15) / 8) * (wFont->height+1);
			if (rotate_char) {
				if (!gfx_rotate_fcns) return FAILURE;
				if ((rotate_char == _90_DEGREES) ||
				    (rotate_char == _270_DEGREES))
					temp_buf_size = ((wFont->height + 7) / 8) * (wFont->widest_cell + 1);
				temp_buf_size += 100;
				rotate_buf = _gfx_malloc_mc(temp_buf_size, &rotate_buf_mc);
				if (!rotate_buf_mc) return FAILURE;
			}
			if (expand_char) {
				if (!gfx_expand_fcn) return FAILURE;
				expand_buf = _gfx_malloc_mc(temp_buf_size*wFont->attr.expand_rows*wFont->attr.expand_width, &expand_buf_mc);
				if (!expand_buf) {
					if (rotate_char) _gfx_free_mc(rotate_buf_mc);
					return FAILURE;
				}
			}
		}
		fgnd_color = _gfx_get_color(wFont->attr.fgnd_color);
		screen_offset = 0xFFF0;
		turn_on_ega();
	}

	/*  Now to the heart of the matter.  This routine loops until either a
	 *  delimiting NULL is found or the maximum # of characters are written.
	 */

	wFont_crsr.delta_x = wFont_crsr.delta_y = clip_n_top_rows =
		clip_n_bottom_rows = 0;
	temp_buf_size = ((wFont->widest_cell + 15) / 8) * (wFont->height+1);
	real_char_bitmap = (unsigned char *)_gfx_malloc_mc(temp_buf_size, &real_char_bitmap_mc);
	if (!real_char_bitmap) return FAILURE;
	for (i = 0; (i < max_char) && ((c = tp[i]) > 0); i++) {
		if ((ZIL_UINT16)c > replaceChar)
			continue;
		form_width = wFont->width;
		n_char_rows = wFont->height;
		nonSpacing = ZIL_INTERNATIONAL::IsNonspacing(c);

#	if defined(DELAYED_READ)
		c = GetCharPage(c);
#	endif

		if (!wFont->fixed_width_char) {
			char_bit_offset = wFont->offset_table[(c&MASK)>>SHIFT][c&REST_OF(MASK)];
			next_char_bit_offset = wFont->offset_table[((c+1)&MASK)>>SHIFT][(c+1)&REST_OF(MASK)];
			n_char_pels = (int)(next_char_bit_offset - char_bit_offset);
			if (n_char_pels == 0)
			{
#	if defined(DELAYED_READ)
				c = GetCharPage(replaceChar);
#else
				c = replaceChar;
#endif
				char_bit_offset = wFont->offset_table[(c&MASK)>>SHIFT][c&REST_OF(MASK)];
				next_char_bit_offset = wFont->offset_table[((c+1)&MASK)>>SHIFT][(c+1)&REST_OF(MASK)];
				n_char_pels = (int)(next_char_bit_offset - char_bit_offset);
			}
			if (n_char_pels == 0)
				continue;
		}
		else
		{
			char_bit_offset = c * wFont->height * wFont->fixed_width_char;
			next_char_bit_offset = char_bit_offset + wFont->fixed_width_char;
			n_char_pels = (int)(next_char_bit_offset - char_bit_offset);
		}

		// Copy the bitmap to an "easy" buffer
		new_form_width = (int)((next_char_bit_offset / 8) - (char_bit_offset / 8) + 1);
		for (j=0; j < n_char_rows; j++)
		{
			char_bitmap = wFont->data_table[j];
			for (k=0; k < new_form_width; k++)
				real_char_bitmap[j * new_form_width + k] =
					char_bitmap[(int)(char_bit_offset / 8) + k];
		}
		char_bitmap = real_char_bitmap;
		form_width = new_form_width;
		char_bit_offset %= 8;

		/* If a character needs rotation -- do it. */

		if (rotate_char) {
			lpad = (int)(char_bit_offset & 7);
			rpad = top_pad_rows = (8 - ((lpad + n_char_pels) & 7)) & 7;
			char_bit_offset = 0;
			switch (wFont->attr.rotate_char) {
			case _270_DEGREES:
				top_pad_rows = lpad;
				char_bit_offset = (8 - (wFont->height & 7)) & 7;
			case _90_DEGREES:
				n_char_rows = n_char_pels;
				n_char_pels += lpad + rpad;
				n_dest_rows = n_char_pels & 0xF8;
				n_source_col = n_char_pels / 8;
				n_char_pels = wFont->height;
				form_width = (wFont->height + 7) / 8;
				break;
			case _180_DEGREES:
				top_pad_rows = 0;
				char_bit_offset = rpad;
				n_dest_rows = n_char_rows;
				form_width = n_source_col = (lpad + n_char_pels + 7) / 8;
				break;
			}

			if (gfx_line_attr.display_char) {
				(**(gfx_rotate_fcns+wFont->attr.rotate_char))(char_bitmap, wFont->width, wFont->height, n_source_col, rotate_buf, form_width, n_dest_rows);
				char_bitmap = (unsigned char *)rotate_buf + (top_pad_rows * form_width);
			}
		}

		/* If a character needs to be expanded -- do it. */

		if (expand_char) {
			if (gfx_line_attr.display_char) {
				char_bitmap +=  (int)(char_bit_offset / 8);
				char_bit_offset &= 7;
				n_source_col = (int)((char_bit_offset + n_char_pels + 7) / 8);
				(*gfx_expand_fcn)((unsigned char *)char_bitmap, form_width, n_char_rows, n_source_col, expand_buf, expand_width, expand_rows);
				char_bitmap = (unsigned char *)expand_buf;
				char_bit_offset *= expand_width;
				form_width = n_source_col * expand_width;
				while (char_bit_offset > 7){
					char_bit_offset -= 8;
					char_bitmap++;
				}
			}
			n_char_pels *= expand_width;
			n_char_rows *= expand_rows;
		}

		if (gfx_line_attr.text_dir == TEXT_L_TO_R && nonSpacing && i ||
		    gfx_line_attr.text_dir == TEXT_R_TO_L && !nonSpacing)
			wFont_crsr.delta_x -= n_char_pels + wFont->attr.h_space;
		else if (gfx_line_attr.text_dir == TEXT_T_TO_B && nonSpacing && i ||
		    gfx_line_attr.text_dir == TEXT_B_TO_T && !nonSpacing)
			wFont_crsr.delta_y -= n_char_rows + wFont->attr.h_space;

		if (gfx_line_attr.display_char) {

			/* calculate offset of start of row y */

			if (!is_horizontal_run() || (screen_offset == 0xFFF0)) {
				row_y = y + wFont_crsr.delta_y;
				if ((clip_n_top_rows = _gfx.min_y - row_y) > 0)
					row_y += clip_n_top_rows;
				screen_offset = _gfx_set_vidram(0, row_y);
				if (_gfx.bios_mode == MED_RES_COLOR)
					fgnd_color = med_res_color[fgnd_color&0x3];
			}

			/* clip top and bottom rows */

			show_n_rows = n_char_rows;
			if (clip_n_top_rows > 0) {
				char_bitmap += form_width * clip_n_top_rows;
				show_n_rows -= clip_n_top_rows;
			}
			if ((clip_n_bottom_rows = (row_y + wFont_crsr.delta_y + show_n_rows - 1) - _gfx.max_y) > 0)
				show_n_rows -= clip_n_bottom_rows;

			/* clip left and right sides */

			pt_x = x + wFont_crsr.delta_x;
			diff = 0;
			if ((tdiff = (_gfx.min_x - pt_x)) > 0) {
				pt_x = _gfx.min_x;
				char_bit_offset += tdiff;
				diff = tdiff;
			}
			if ((tdiff = (pt_x + n_char_pels - diff) - (_gfx.max_x+1)) > 0)
				diff += tdiff;

			/* write out the character */

			if ((n_char_pels > diff) && (show_n_rows > 0))
				_gfx_write_char(screen_offset, pt_x, fgnd_color,
						(int)char_bit_offset, n_char_pels-diff, show_n_rows,
						form_width, char_bitmap);
		}

		/* move position in x or y direction */

		if (gfx_line_attr.text_dir == TEXT_L_TO_R)
			wFont_crsr.delta_x += n_char_pels + wFont->attr.h_space;
		else if (gfx_line_attr.text_dir == TEXT_T_TO_B)
			wFont_crsr.delta_y += n_char_rows + wFont->attr.h_space;
	}

	/* It's cleanup time. */

	_gfx_free_mc(real_char_bitmap_mc);

	if (gfx_line_attr.display_char) {
		turn_off_ega();
		if (rotate_char) _gfx_free_mc(rotate_buf_mc);
		if (expand_char) _gfx_free_mc(expand_buf_mc);
		if (gfx_line_attr.justify == JUSTIFY_START) {
			wFont_crsr.x += wFont_crsr.delta_x;
			wFont_crsr.y += wFont_crsr.delta_y;
			if (_gfx.xlat_scale == FLOAT_SCALE) {
				_gfx.pt_x += wFont_crsr.delta_x;
				_gfx.pt_y += wFont_crsr.delta_y;
				if (rs_flog) (*rs_flog)();
			}
			else
			{
				if (wFont_crsr.delta_x)
					wFont_crsr.ilog_x += _gfx_xlat_int_coor(wFont_crsr.delta_x, _gfx.scale.x_denom, _gfx.scale.x_num);
				if (wFont_crsr.delta_y)
					wFont_crsr.ilog_y += _gfx_xlat_int_coor(wFont_crsr.delta_y, _gfx.scale.y_denom, _gfx.scale.y_num);
			}
		}
	}
	return SUCCESS;
}

/*~ PRINT.C */

void WPrintFont(int arg, ...)
{
	int *args;
	ZIL_ICHAR *tp;
	CRSR *save_crsr_pointer;

	args = &arg;
	GFX_VVEC(VPRINT_FONT, args);
	save_crsr_pointer = gfx_crsr;
	gfx_crsr = (CRSR *) (&wFont_crsr);
	args += (*_gfx.get_pt)(args);
	gfx_crsr = save_crsr_pointer;
	tp = *(ZIL_ICHAR **)args;
	args += POINTER_ON_STACK;
	Wgfx_print_font(tp, *args);
}


/*~ GETLNLEN.C */

int WGetLineLen(const ZIL_ICHAR *text, int max_n_char)
{
	int delta;

	if (!(*text) || (max_n_char < 1)) return FAILURE;
	gfx_line_attr.display_char = NO;
	Wgfx_print_font(text, max_n_char);
	gfx_line_attr.display_char = YES;
	delta = (gfx_line_attr.text_dir < TEXT_B_TO_T) ? wFont_crsr.delta_x : wFont_crsr.delta_y;
	return (delta < 0) ? -delta : delta;
}


/*~ OPEN_FNT.C */

struct _gem_font
{
	ZIL_INT16 id, size;
	ZIL_INT8 name[32];
	ZIL_ICHAR lo_ade, hi_ade;
	ZIL_INT16 top_dist, ascent_dist, half_dist;
	ZIL_INT16 descent_dist, bottom_dist;
	ZIL_ICHAR widest_char, widest_cell;
	ZIL_INT16 left_offset, right_offset;
	ZIL_INT16 thick, underline_size, v1, v2, v3;
	ZIL_INT32 horiz_table, char_offset_table, font_data;
	Offset_t form_width;
	ZIL_INT16 form_height;
};

int WOpenFont(const ZIL_ICHAR *font_name)
{
	int dos_handle, i;
	struct _gem_font gem;
	WFONT *tFont;

	ZIL_FILE_CHAR *fName = ZIL_INTERNATIONAL::ConvertToFilename(font_name);
	dos_handle = gfx_open(fName, OPEN_READ_FILE);
	delete fName;
	if (dos_handle == FAILURE)
	{
		_gfx_err_number(BAD_FILE_OPEN, OPEN_FONT_FCN);
		return FAILURE;
	}

	gfx_read(&gem, sizeof(struct _gem_font), dos_handle);
	// Allocate tFont and offset_table
	tFont = new WFONT;
	if (tFont == ZIL_NULLP(WFONT))
	{
		gfx_close(dos_handle);
		_gfx_err_number(NO_HEAP_SPACE, OPEN_FONT_FCN);
		return FAILURE;
	}
	tFont->offset_table = new Offset_t *[(MASK >> SHIFT)+1];
	if (tFont->offset_table == ZIL_NULLP(Offset_t *))
	{
		gfx_close(dos_handle);
		_gfx_err_number(NO_HEAP_SPACE, OPEN_FONT_FCN);
		return FAILURE;
	}
	for (i=0; i < (MASK >> SHIFT)+1; i++)
	{
		tFont->offset_table[i] = new Offset_t[(REST_OF(MASK)+1)];
		if (tFont->offset_table[i] == ZIL_NULLP(Offset_t))
		{
			gfx_close(dos_handle);
			_gfx_err_number(NO_HEAP_SPACE, OPEN_FONT_FCN);
			return FAILURE;
		}
	}
	tFont->data_table = new ZIL_UINT8 *[gem.form_height];
	if (tFont->data_table == ZIL_NULLP(ZIL_UINT8 *)) {
		gfx_close(dos_handle);
		_gfx_err_number(NO_HEAP_SPACE, OPEN_FONT_FCN);
		return FAILURE;
	}
	for (i=0; i < gem.form_height; i++)
	{
		tFont->data_table[i] = new ZIL_UINT8[(ZIL_UINT16)gem.form_width];
		if (tFont->data_table[i] == ZIL_NULLP(ZIL_UINT8))
		{
			gfx_close(dos_handle);
			_gfx_err_number(NO_HEAP_SPACE, OPEN_FONT_FCN);
			return FAILURE;
		}
	}
#if defined(DELAYED_READ)
	tFont->dos_handle = dos_handle;
	tFont->font_data = gem.font_data;
	tFont->char_offset_table = gem.char_offset_table;
#else
	if (UI_DEVICE::eventManager)
		UI_DEVICE::eventManager->DeviceState(E_MOUSE, DM_WAIT);
	// Read the offset bit pointers
	gfx_lseek(dos_handle, gem.char_offset_table, 0);
	for (i=0; i < (MASK >> SHIFT)+1; i++)
		gfx_read(tFont->offset_table[i], (REST_OF(MASK)+1)*sizeof(Offset_t), dos_handle);
	// Read the bitmaps
	gfx_lseek(dos_handle, gem.font_data, 0);
	for (i=0; i < gem.form_height; i++)
		gfx_read(tFont->data_table[i], (int) gem.form_width, dos_handle);
	if (UI_DEVICE::eventManager)
		UI_DEVICE::eventManager->DeviceState(E_MOUSE, DM_VIEW);
#endif
	tFont->width = (int) gem.form_width;
	tFont->height = gem.form_height;
	tFont->lo_char = gem.lo_ade;
	tFont->hi_char = gem.hi_ade;
	tFont->widest_cell = gem.widest_cell;
	tFont->ascent_distance = gem.ascent_dist;
	tFont->underline_loc = gem.ascent_dist + gem.descent_dist;
	tFont->fixed_width_char = NO;
	tFont->attr = *GetFontAttr(0);
	tFont->attr.u_thickness = gem.underline_size;
#if !defined(DELAYED_READ)
	gfx_close(dos_handle);
#endif
	for (i=0; i < MAX_FONTS; i++)
		if (fonts[i] == ZIL_NULLP(WFONT))
		{
			fonts[i] = tFont;
			return i+1;
		}
	return FAILURE;
}

/*~ CLOSEFNT.C */

int WCloseFont(int fh)
{
	if (fh < 1 || fh > MAX_FONTS)
		return FAILURE;
	WFONT *tmp = fonts[fh-1];
	if (tmp == ZIL_NULLP(WFONT) || tmp == wFont)
		return FAILURE;
	fonts[fh-1] = ZIL_NULLP(WFONT);

#if defined(DELAYED_READ)
	gfx_close(tmp->dos_handle);
#endif
	int i;
	for (i=0; i < tmp->height; i++)
		delete tmp->data_table[i];
	delete tmp->data_table;
	for (i=0; i < (MASK >> SHIFT)+1; i++)
		delete tmp->offset_table[i];
	delete tmp->offset_table;
	delete tmp;
	return SUCCESS;
}

/*~ FZIL_COLOR.C */

int WFontColor(int fgnd_color, int bkgnd_color)
{
	if (fgnd_color != DFLT)
		wFont->attr.fgnd_color = fgnd_color;
	if (bkgnd_color != DFLT)
		wFont->attr.bkgnd_color = ((unsigned int)bkgnd_color == XPARENT) ? TRANSPARENT : bkgnd_color;
	return SUCCESS;
}

/*~ FALIGN.C */

int WFontAlign(int align)
{
	if (!inrange(TOP_LINE, align, BOTTOM_LINE))
		return _gfx_err_number(BAD_ARG, FONT_ALIGN_FCN);
	wFont->attr.align = align;
	return SUCCESS;
}

void WFontSize(int *width, int *height)
{
	*width = wFont->widest_cell - 1;
	*height = wFont->height - 1;
}

int WFontInRange(ZIL_ICHAR c)
{
	return (wFont->lo_char <= c && c <= wFont->hi_char);
}

int WCurrFont(int fh)
{
	if (fh == 0)
	{
		wFont = ZIL_NULLP(WFONT);
		return SUCCESS;
	}
	if (fh >= 1 && fh <= MAX_FONTS)
	{
		wFont = fonts[fh-1];
		return SUCCESS;
	}
	return FAILURE;
}
