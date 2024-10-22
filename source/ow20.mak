# Zinc Application Framework 4.0 library makefile
# Uses Open Watcom C/C++ 2.0 with Watcom WMAKE.EXE
#
# Compiles Zinc for 32 bit DOS (using DOS4GW), 16 bit Windows, Windows NT
#  (including WIN32s), OS2 2.x.
#    wmake -f ow20.mak dos32
#    wmake -f ow20.mak windows
#    wmake -f ow20.mak winnt
#    wmake -f ow20.mak win32
#    wmake -f ow20.mak os2
#    wmake -f ow20.mak curses
#    wmake -f ow20.mak motif
#
# ----- General Definitions -------------------------------------------------
VERSION=ow20
exename=test

template_lib_objs = &
	z_bnum$(oext) &
	z_bnum1$(oext) &
	z_bnum2$(oext) &
	z_border$(oext) &
	z_button$(oext) &
	z_combo$(oext) &
	z_cursor$(oext) &
	z_date$(oext) &
	z_date1$(oext) &
	z_decor$(oext) &
	z_device$(oext) &
	z_dialog$(oext) &
	z_dsp$(oext) &
	z_error$(oext) &
	z_error1$(oext) &
	z_event$(oext) &
	z_file$(oext) &
	z_fmtstr$(oext) &
	z_gmgr$(oext) &
	z_gmgr1$(oext) &
	z_gmgr2$(oext) &
	z_gmgr3$(oext) &
	z_group$(oext) &
	z_help$(oext) &
	z_help1$(oext) &
	z_hlist$(oext) &
	z_icon$(oext) &
	z_image$(oext) &
	z_i18n$(oext) &
	z_int$(oext) &
	z_intl$(oext) &
	z_lang$(oext) &
	z_list$(oext) &
	z_list1$(oext) &
	z_locale$(oext) &
	z_map1$(oext) &
	z_map2$(oext) &
	z_max$(oext) &
	z_min$(oext) &
	z_msgwin$(oext) &
	z_path$(oext) &
	z_plldn$(oext) &
	z_plldn1$(oext) &
	z_popup$(oext) &
	z_popup1$(oext) &
	z_printf$(oext) &
	z_prompt$(oext) &
	z_real$(oext) &
	z_region$(oext) &
	z_sbar$(oext) &
	z_scanf$(oext) &
	z_scroll$(oext) &
	z_spin$(oext) &
	z_stdarg$(oext) &
	z_stored$(oext) &
	z_storer$(oext) &
	z_storew$(oext) &
	z_string$(oext) &
	z_sys$(oext) &
	z_table$(oext) &
	z_table1$(oext) &
	z_table2$(oext) &
	z_tbar$(oext) &
	z_text$(oext) &
	z_time$(oext) &
	z_time1$(oext) &
	z_timer$(oext) &
	z_title$(oext) &
	z_utils$(oext) &
	z_utime$(oext) &
	z_utime1$(oext) &
	z_vlist$(oext) &
	z_win$(oext) &
	z_win1$(oext) &
	z_win2$(oext) &
	z_win3$(oext) &
	z_win4$(oext) &
	g_dsp$(oext) &
	g_event$(oext) &
	g_evt$(oext) &
	g_gen$(oext) &
	g_i18n$(oext) &
	g_jump$(oext) &
	g_lang$(oext) &
	g_lang1$(oext) &
	g_loc$(oext) &
	g_loc1$(oext) &
	g_mach$(oext) &
	g_pnorm$(oext) &
	g_win$(oext) &
	i_file$(oext) &
	i_map$(oext) &
	i_str1$(oext) &
	i_str2$(oext) &
	i_str3$(oext) &
	i_str4$(oext) &
	i_str5$(oext) &
	i_type$(oext)

template_spec_objs = &
	$(prfx)_bnum$(oext) &
	$(prfx)_border$(oext) &
	$(prfx)_button$(oext) &
	$(prfx)_combo$(oext) &
	$(prfx)_cursor$(oext) &
	$(prfx)_date$(oext) &
	$(prfx)_error$(oext) &
	$(prfx)_error1$(oext) &
	$(prfx)_event$(oext) &
	$(prfx)_fmtstr$(oext) &
	$(prfx)_group$(oext) &
	$(prfx)_hlist$(oext) &
	$(prfx)_icon$(oext) &
	$(prfx)_image$(oext) &
	$(prfx)_int$(oext) &
	$(prfx)_intl$(oext) &
	$(prfx)_keybrd$(oext) &
	$(prfx)_max$(oext) &
	$(prfx)_min$(oext) &
	$(prfx)_mouse$(oext) &
	$(prfx)_notebk$(oext) &
	$(prfx)_plldn$(oext) &
	$(prfx)_plldn1$(oext) &
	$(prfx)_popup$(oext) &
	$(prfx)_popup1$(oext) &
	$(prfx)_prompt$(oext) &
	$(prfx)_real$(oext) &
	$(prfx)_sbar$(oext) &
	$(prfx)_scroll$(oext) &
	$(prfx)_spin$(oext) &
	$(prfx)_string$(oext) &
	$(prfx)_sys$(oext) &
	$(prfx)_table$(oext) &
	$(prfx)_table1$(oext) &
	$(prfx)_table2$(oext) &
	$(prfx)_tbar$(oext) &
	$(prfx)_text$(oext) &
	$(prfx)_time$(oext) &
	$(prfx)_title$(oext) &
	$(prfx)_vlist$(oext) &
	$(prfx)_win$(oext) &
	$(prfx)_win1$(oext) &
	$(prfx)_win2$(oext)

d32_extra_objs = i_wccat.o32 d_tdsp.o32 z_notebk.o32
win_extra_objs = z_app.obw w_dsp.obw w_print.obw z_notebk.obw
wnt_extra_objs = z_app.obn 3_dsp.obn 3_print.obn z_notebk.obn
w32_extra_objs = z_app.ob9 9_dsp.ob9 9_print.ob9 9_comctl.ob9 z_notebk.ob9
os2_extra_objs = z_app.obo o_dsp.obo o_print.obo
crs_extra_objs = z_app.oc d_dsp.oc d_print.oc z_notebk.oc
mtf_extra_objs = z_app.om m_dsp.om m_print.om z_notebk.om

prfx=d
oext=.o32
d32_lib_objs=$+$(template_lib_objs) $(template_spec_objs)$- $(d32_extra_objs)

prfx=w
oext=.obw
win_lib_objs=$+$(template_lib_objs) $(template_spec_objs)$- $(win_extra_objs)

prfx=3
oext=.obn
wnt_lib_objs=$+$(template_lib_objs) $(template_spec_objs)$- $(wnt_extra_objs)

prfx=9
oext=.ob9
w32_lib_objs=$+$(template_lib_objs) $(template_spec_objs)$- $(w32_extra_objs)

prfx=o
oext=.obo
os2_lib_objs=$+$(template_lib_objs) $(template_spec_objs)$- $(os2_extra_objs)

prfx=d
oext=.oc
crs_lib_objs=$+$(template_lib_objs) $(template_spec_objs)$- $(crs_extra_objs)

prfx=m
oext=.om
mtf_lib_objs=$+$(template_lib_objs) $(template_spec_objs)$- $(mtf_extra_objs)

d32_gfx_objs = &
	d_gfxdsp.o32 &
	d_gfxprn.o32 &
	i_gfx.o32 &
	z_appgfx.o32 &
	ISO_smal.o32 &
	ISO_dial.o32 &
	ISO_syst.o32 &
	OEM_smal.o32 &
	OEM_dial.o32 &
	OEM_syst.o32

.EXTENSIONS:
.EXTENSIONS: .lib .rew .re9 .ren .reo .o32 .obw .obn .ob9 .obo .oc .om .o .rc .cpp .c

CXX16=wpp -zq
CXX=wpp386 -zq
LINK=wlink op q,map
LIBRARIAN=wlib -q
RC=wrc -q

CXX_OPTS=-oaxt -w4 -j
LIB_OPTS=-pa -n
RC_OPTS=-r

# ----- DOS 32 bit (DOS4GW extender) compiler options -----------------------
D32_CXX_OPTS=-bt=dos -dDOS386 -I"gfx/source"
D32_LINK_OPTS=SYSTEM dos4g OP stack=20000 DISA 1124
D32_OBJS=
# --- Use the next line for UI_WCC_DISPLAY ---
D32_LIBS=d32_wcc.lib
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
#D32_LIBS=d32_gfx.lib wc_32gfx.lib

# ----- 16 bit Windows compiler options -------------------------------------
WIN_CXX_OPTS=-zW -zc -ml -zt=100
WIN_LINK_OPTS=SYSTEM windows OP heapsize=16k OP stack=24k
WIN_RC_OPTS=-bt=windows
WIN_OBJS=
WIN_LIBS=

# ----- Windows NT (and WIN32s) compiler options ----------------------------
WNT_CXX_OPTS=-bt=nt
WNT_LINK_OPTS=SYSTEM nt_win
WNT_RC_OPTS=-bt=nt
WNT_OBJS=
WNT_LIBS=

# ----- 32 bit Windows compiler options -------------------------------------
W32_CXX_OPTS=-bt=nt -DZIL_WIN32
W32_LINK_OPTS=SYSTEM nt_win
W32_RC_OPTS=-bt=nt
W32_OBJS=
W32_LIBS=

# ----- OS/2 2.x compiler options -------------------------------------------
OS2_CXX_OPTS=-bt=os2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=96000
OS2_RC_OPTS=-bt=os2
OS2_OBJS=
OS2_LIBS=

# ----- 32 bit Linux compiler options -------------------------------------
LNX_CXX_OPTS=-bt=linux
LNX_LINK_OPTS=SYSTEM linux
LNX_RC_OPTS=
LNX_OBJS=
LNX_LIBS=

.cpp.o32:
	$(CXX) $(CXX_OPTS) $(D32_CXX_OPTS) -fo=$@ $<

.cpp.obw:
	$(CXX16) $(CXX_OPTS) $(WIN_CXX_OPTS) -fo=$@ $<

.cpp.obn:
	$(CXX) $(CXX_OPTS) $(WNT_CXX_OPTS) -fo=$@ $<

.cpp.ob9:
	$(CXX) $(CXX_OPTS) $(W32_CXX_OPTS) -fo=$@ $<

.cpp.obo:
	$(CXX) $(CXX_OPTS) $(OS2_CXX_OPTS) -fo=$@ $<

.cpp.oc:
	$(CXX) $(CXX_OPTS) $(LNX_CXX_OPTS) -DZIL_CURSES -fo=$@ $<

.cpp.om:
	$(CXX) $(CXX_OPTS) $(LNX_CXX_OPTS) -DZIL_MOTIF -fo=$@ $<

.rc.rew:
	$(RC) $(RC_OPTS) $(WIN_RC_OPTS) $< -fo=$@

.rc.ren:
	$(RC) $(RC_OPTS) $(WNT_RC_OPTS) $< -fo=$@

.rc.re9:
	$(RC) $(RC_OPTS) $(W32_RC_OPTS) $< -fo=$@

.rc.reo:
	$(RC) $(RC_OPTS) $(OS2_RC_OPTS) $< -fo=$@

# ----- Usage --------------------------------------------------------------
usage: .SYMBOLIC
	@echo ...........
	@echo ...........
	@echo To generate the library modules for specific environments type:
	@echo wmake -f ow20.mak dos32
	@echo wmake -f ow20.mak windows
	@echo wmake -f ow20.mak winnt
        @echo wmake -f ow20.mak win32
	@echo wmake -f ow20.mak os2
	@echo wmake -f ow20.mak curses
	@echo wmake -f ow20.mak motif
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	@rm -f foo *.bak *.bk? *.ob? *.rbj *.map *.exe *.tc tc*.* *.dsk *.dpr
	@rm -f *.sav *.sv? *.cfg *.$$$$$$ *.lib *.fil *.re? *.sym *.err *.zip *.ovl
	@rm -f *.o16 *.o32 *.oc *.om *.p16 *.p32 __tmp.rsp

# ----- Copy files ----------------------------------------------------------
init: ../lib/$(VERSION) .SYMBOLIC
#	%copy ui_dsn.hpp ../include
	%copy ui_dsp.hpp ../include
	%copy ui_env.hpp ../include
	%copy ui_evt.hpp ../include
	%copy ui_gen.hpp ../include
	%copy ui_map.hpp ../include
	%copy ui_win.hpp ../include
	%copy z_comctl.h ../include
#	%copy z_clean.bat ../bin
	%copy gfx/source/gfx.h ../include
	%copy gfx/source/gfx_pro.h ../include

../lib/$(VERSION):
!ifdef __UNIX__
        mkdir -p $@
!else
        mkdir $@
!endif

# ----- DOS 32 bit (DOS4GW extender) Libraries and Programs -----------------
dos32: init $(exename).exe .SYMBOLIC

#$(exename).exe: $(exename).o32 d32_gfx.lib d32_wcc.lib wc_32gfx.lib
$(exename).exe: $(exename).o32 d32_wcc.lib d32_zil.lib
	$(LINK) $(D32_LINK_OPTS) N $@ F {$(exename).o32 $(D32_OBJS)} L {$(D32_LIBS) d32_zil.lib}

#d32_zil.lib : d32_gfx.lib wc_32gfx.lib $(d32_lib_objs)
d32_zil.lib : $(d32_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $(d32_lib_objs)
	%copy $@ ../lib/$(VERSION)

d32_wcc.lib : d_wccdsp.o32 d_wccprn.o32 z_appwcc.o32
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

d32_gfx.lib : $(d32_gfx_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

wc_32gfx.lib : .SYMBOLIC
	@cd gfx
	@cd source
	wmake -f $(VERSION).mak dos32
	@cd ..
	@cd ..

# ----- Windows 16 bit Libraries and Programs -------------------------------
windows: init w$(exename).exe .SYMBOLIC

w$(exename).exe: $(exename).obw win_zil.lib
	$(LINK) $(WIN_LINK_OPTS) N w$(exename) F {$(exename).obw $(WIN_OBJS)} L {$(WIN_LIBS) win_zil.lib}

win_zil.lib : $(win_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

# ----- Windows NT (and WIN32s) Libraries and Programs ----------------------
winnt: init n$(exename).exe .SYMBOLIC

n$(exename).exe: $(exename).obn wnt_zil.lib
	$(LINK) $(WNT_LINK_OPTS) N n$(exename) F {$(exename).obn $(WNT_OBJS)} L {$(WNT_LIBS) wnt_zil.lib}

wnt_zil.lib : $(wnt_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

# ----- Windows 32 bit Libraries and Programs -------------------------------
win32: init 9$(exename).exe .SYMBOLIC

9$(exename).exe: $(exename).ob9 w32_zil.lib
	$(LINK) $(W32_LINK_OPTS) N 9$(exename) F {$(exename).ob9 $(W32_OBJS)} L {$(W32_LIBS) w32_zil.lib}

w32_zil.lib : $(w32_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

# ----- OS/2 2.x Libraries and Programs -------------------------------------
os2: init o$(exename).exe .SYMBOLIC

o$(exename).exe: $(exename).obo os2_zil.lib
	$(LINK) $(OS2_LINK_OPTS) N o$(exename).exe F {$(exename).obo $(OS2_OBJS)} L {$(OS2_LIBS) os2_zil.lib}

os2_zil.lib : $(os2_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

# ----- Linux Libraries and Programs (Curses) --------------------------------
curses: init c$(exename).exe .SYMBOLIC

c$(exename).exe: $(exename).oc crs_zil.lib
	$(LINK) $(LNX_LINK_OPTS) N c$(exename).exe F {$(exename).oc $(LNX_OBJS)} L {$(LNX_LIBS) crs_zil.lib}

crs_zil.lib : $(crs_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

# ----- Linux Libraries and Programs (Motif) --------------------------------
motif: init m$(exename).exe .SYMBOLIC

m$(exename).exe: $(exename).om mtf_zil.lib
	$(LINK) $(LNX_LINK_OPTS) N m$(exename).exe F {$(exename).om $(LNX_OBJS)} L {$(LNX_LIBS) mtf_zil.lib}

mtf_zil.lib : $(mtf_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy $@ ../lib/$(VERSION)

# --- Done with libraries ----------------------------------------------------

z_appwcc.o32: z_app.cpp
	$(CXX) $(CXX_OPTS) -dWCC $(D32_CXX_OPTS) -fo=$@ $?

z_appgfx.o32: z_app.cpp
	$(CXX) $(CXX_OPTS) -dGFX $(D32_CXX_OPTS) -fo=$@ $?

z_apptxt.o32: z_app.cpp
	$(CXX) $(CXX_OPTS) -dTEXT $(D32_CXX_OPTS) -fo=$@ $?

d_wccprn.o32: d_print.cpp
	$(CXX) $(CXX_OPTS) -dWCC $(D32_CXX_OPTS) -fo=$@ $?

d_gfxprn.o32: d_print.cpp
	$(CXX) $(CXX_OPTS) -dGFX $(D32_CXX_OPTS) -fo=$@ $?

OEM_dial.cpp: gfx/OEM_dial.cpp
	%copy $< $@

OEM_smal.cpp: gfx/OEM_smal.cpp
	%copy $< $@

OEM_syst.cpp: gfx/OEM_syst.cpp
	%copy $< $@

ISO_dial.cpp: gfx/ISO_dial.cpp
	%copy $< $@

ISO_smal.cpp: gfx/ISO_smal.cpp
	%copy $< $@

ISO_syst.cpp: gfx/ISO_syst.cpp
	%copy $< $@
