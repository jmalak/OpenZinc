/************************************************/
/* Copyright (c) C Source, Inc 1989, 1990, 1991 */
/************************************************/

#if !defined (__GFX_PRO_)
#define __GFX_PRO_

#if defined (__cplusplus)
extern "C" {
#endif

#if defined (USE_PAS_M2)

			/*******************************************
			 *	  PROTOTYPES FOR PAS/M2 INTERFACE	   *
			 *******************************************/

			/*******************************************
			 *	   GFX GRAPHICS LIBRARY FUNCTIONS	   *
			 *******************************************/


	/* Setting the video 'playing field' */

extern  void Color(int fgnd, int bkgnd, int palette);
extern  int  CopyVideoPages(int from_page, int to_page);
extern  void InitGFXStruct(int bios_nmbr);
extern  int  Screen(int mode);
extern  void SetVideoResolution(int res_x, int res_y);
extern  int  SetExtendedMode(int x_res, int y_res, int is_color, int mode_n);
extern  int  SetVideoPages(int vpage, int wpage);
extern  unsigned int  GetCardMonitor(void);
extern  int  MapEGAPalette(int index, int color);
extern  int  RotateScreen(int rotate_dir);

	/* Functions getting various screen coordinates. */

extern  void ConvertCoorPt(COOR_PT *cpt, int type);
extern  void GetCAP(COOR_PT *cpt);
extern  void ViewOrigin(COOR_PT *cpt);
extern  void ViewLrCorner(COOR_PT *cpt);
extern  void ViewUlCorner(COOR_PT *cpt);

	/* Pattern setting functions */

extern  void SetLinePat(unsigned int pat);
extern  void SetFillPat(unsigned char *pat, int color, int bkgnd_color);
extern  void SetFillSolid(int color);

	/* General purpose */

extern  void Cls( void );
extern  void AFdecl MoveTo(int, int);
extern  void AFdecl MoveToRel(int, int);
extern  void AFdecl MoveToF(double, double);
extern  void AFdecl MoveToRelF(double, double);
extern  int  AFdecl Point(int, int);
extern  int  AFdecl PointRel(int, int);
extern  int  AFdecl PointF(double, double);
extern  int  AFdecl PointRelF(double, double);
extern  void SetVideoAspectRatio(int, int);
extern  void SetMallocHook(void *(* malloc_hook)(), void (* free_hook)());
extern  void SetFarmallocHook(VOID_FAR *(* malloc_hook)(), void (* free_hook)());
extern  int  UseLogicalCoor(void);
extern  int  UseAbsoluteCoor(void);

	/* Drawing functions */

extern  void AFdecl Box(int, int, int, int, int);
extern  void AFdecl BoxTo(int, int, int);
extern  void AFdecl BoxToRel(int, int, int);
extern  void AFdecl BoxF(double, double, double, double, int);
extern  void AFdecl BoxToF(double, double, int);
extern  void AFdecl BoxToRelF(double, double, int);

extern  void AFdecl Circle(int, int, int, int);
extern  void AFdecl CircleRel(int, int, int, int);
extern  void AFdecl CircleF(double, double, double, int);
extern  void AFdecl CircleRelF(double, double, double, int);

extern  void AFdecl CircleArc(int, int, int, int, int, int);
extern  void AFdecl CircleArcRel(int, int, int, int, int, int);
extern  void AFdecl CircleArcF(double, double, double, int, int, int);
extern  void AFdecl CircleArcRelF(double, double, double, int, int, int);

extern  int  Draw(char *fmt, ... );

extern  void AFdecl DrawLine(int, int, int, int, int);
extern  void AFdecl DrawLineTo(int, int, int);
extern  void AFdecl DrawLineToRel(int, int, int);
extern  void AFdecl DrawLineF(double, double, double, double, int);
extern  void AFdecl DrawLineToF(double, double, int);
extern  void AFdecl DrawLineToRelF(double, double, int);

extern  void AFdecl Oval(int, int, int, int, int);
extern  void AFdecl OvalRel(int, int, int, int, int);
extern  void AFdecl OvalF(double, double, double, double, int);
extern  void AFdecl OvalRelF(double, double, double, double, int);

extern  void AFdecl OvalArc(int, int, int, int, int, int, int);
extern  void AFdecl OvalArcRel(int, int, int, int, int, int, int);
extern  void AFdecl OvalArcF(double, double, double, double, int, int, int);
extern  void AFdecl OvalArcRelF(double, double, double, double, int, int, int);

extern  void AFdecl PieSlice(int, int, int, int, int, int);
extern  void AFdecl PieSliceRel(int, int, int, int, int, int);
extern  void AFdecl PieSliceF(double, double, double, int, int, int);
extern  void AFdecl PieSliceRelF(double, double, double, int, int, int);

extern  void AFdecl OvalPieSlice(int, int, int, int, int, int, int);
extern  void AFdecl OvalPieSliceRel(int, int, int, int, int, int, int);
extern  void AFdecl OvalPieSliceF(double, double, double, double, int, int, int);
extern  void AFdecl OvalPieSliceRelF(double, double, double, double, int, int, int);

extern  int AFdecl  PolyLine(int, int, int *, int, int);
extern  int AFdecl  PolyLineRel(int, int, int *, int, int);
extern  int AFdecl  PolyLineF(double, double, double *, int, int);
extern  int AFdecl  PolyLineRelF(double, double, double *, int, int);

extern  int AFdecl  PolyPoint(int, int, int *, int, int);
extern  int AFdecl  PolyPointRel(int, int, int *, int, int);
extern  int AFdecl  PolyPointF(double, double, double *, int, int);
extern  int AFdecl  PolyPointRelF(double, double, double *, int, int);

extern  void AFdecl Pset(int, int, int);
extern  void AFdecl PsetRel(int, int, int);
extern  void AFdecl PsetF(double, double, int);
extern  void AFdecl PsetRelF(double, double, int);

extern  void AFdecl SwapPel(int, int, int);
extern  void AFdecl SwapPelRel(int, int, int);
extern  void AFdecl SwapPelF(double, double, int);
extern  void AFdecl SwapPelRelF(double, double, int);

	/* Seed fill functions */

extern  int AFdecl  Fastfill(int, int, int, int);
extern  int AFdecl  FastfillRel(int, int, int, int);
extern  int AFdecl  FastfillF(double, double, int, int);
extern  int AFdecl  FastfillRelF(double, double, int, int);
extern  int AFdecl  Paint(int, int, int, int);
extern  int AFdecl  PaintRel(int, int, int, int);
extern  int AFdecl  PaintF(double, double, int, int);
extern  int AFdecl  PaintRelF(double, double, int, int);

	/* Pic & FPic functions */

extern  int  CloseFpic(int pic_handle);
extern  int  CompressFpicFile(char * file_name, char *temp_file_name);
extern  int  CreateFpic(char *fname, int max_fpics);
extern  int  DeleteFpic(int fpic_index, int fpic_handle);
extern  int  FreePic(PIC *);
extern  int  OpenFpic(char *fname, int update_file);
extern  PIC *ReadFpic(int fpic_index, int fpic_handle);
extern  int  WriteFpic(PIC *pic, int fpic_index, int fpic_handle);

extern  int AFdecl  GetFpic(int, int, int, int, int, int);
extern  int AFdecl  GetFpicTo(int, int, int, int);
extern  int AFdecl  GetFpicToRel(int, int, int, int);
extern  int AFdecl  GetFpicF(double, double, double, double, int, int);
extern  int AFdecl  GetFpicToF(double, double, int, int);
extern  int AFdecl  GetFpicToRelF(double, double, int, int);

extern  PIC * AFdecl  GetPic(int, int, int, int);
extern  PIC * AFdecl  GetPicTo(int, int);
extern  PIC * AFdecl  GetPicToRel(int, int);
extern  PIC * AFdecl  GetPicF(double, double, double, double);
extern  PIC * AFdecl  GetPicToF(double, double);
extern  PIC * AFdecl  GetPicToRelF(double, double);

extern  int AFdecl  PutFpic(int, int, int, int, int);
extern  int AFdecl  PutFpicRel(int, int, int, int, int);
extern  int AFdecl  PutFpicF(double, double, int, int, int);
extern  int AFdecl  PutFpicRelF(double, double, int, int, int);

extern  int AFdecl  PutPic(int, int, PIC *, int);
extern  int AFdecl  PutPicRel(int, int, PIC *, int);
extern  int AFdecl  PutPicF(double, double, PIC *, int);
extern  int AFdecl  PutPicRelF(double, double, PIC *, int);

	/* Viewport control functions */

extern  int  ClearView(int view_n, int color);
extern  int  CloseView(int view_n, int hide_view);
extern  int  CloseViewFile(void );
extern  int  CreateViewFile(char *fpic_name);
extern  int  DeleteViewFile(void );
extern  void LogicalIntView(int x_ul, int y_ul, int x_lr, int y_lr);
extern  void LogicalFloatView(double x_ul, double y_ul, double x_lr, double y_lr);
extern  int  SwitchView(int new_view_n, int hide_view);

extern  int AFdecl  MoveView(int, int);
extern  int AFdecl  MoveViewRel(int, int);
extern  int AFdecl  MoveViewF(double, double);
extern  int AFdecl  MoveViewRelF(double, double);

extern  int AFdecl  OpenView(int, int, int, int, int, int);
extern  int AFdecl  OpenViewTo(int, int, int, int);
extern  int AFdecl  OpenViewToRel(int, int, int, int);
extern  int AFdecl  OpenViewF(double, double, double, double, int, int);
extern  int AFdecl  OpenViewToF(double, double, int, int);
extern  int AFdecl  OpenViewToRelF(double, double, int, int);

	/* Now obsolete functions */
extern  int  Line(int, int, int, int, int, int);
extern  int  Locate(int, int );
extern  void ScaleIntView(int x_scale, int y_scale, int x_org, int y_org, int x_dir, int y_dir);
extern  void ScaleFloatView(double x_scale, double y_scale, double x_org, double y_org, int x_dir, int y_dir);

	/* NEW GFX 3.0 FUNCTIONS */

extern  int  IdentifySVGA(void);
extern  int  SetSVGAMode(int);
extern  int  GetSVGAInfo(int **svga_modes, int vram);

extern  int  GetGFXError(void);
extern  int  SetGFXErrorReporting(int flags, int (*hook)(int, int));

extern  void GetArcCoor(ARC_COOR *);
extern  int AFdecl SetViewClipping(int, ... );
extern  void GetViewClipping(BOX_COOR *);
extern  void GetVideoAspectRatio(int *, int *);

extern  int  ReadPCXHeader(char *, PCX_HDR *);
extern  int  OpenPCXFile(char *, PIC *, PCX_HDR *);
extern  int  ReadPCXFile(char *, PIC *, PCX_HDR *);
extern  int  GetMemPCXImage(PIC *user_pic, PCX_HDR *mem_PCX);

extern  int  GetPCXFile(int, int, int, int, char *, int);
extern  int  GetPCXFileTo(int, int, char *, int);
extern  int  GetPCXFileToRel(int, int, char *, int);
extern  int  GetPCXFileF(double, double, double, double, char *, int);
extern  int  GetPCXFileToF(double, double, char *, int);
extern  int  GetPCXFileToRelF(double, double, char *, int);

extern  int  AFdecl PutPCXFile(int, int, char *, int);
extern  int  PutPCXFileRel(int, int, char *, int);
extern  int  PutPCXFileF(double, double, char *, int);
extern  int  PutPCXFileRelF(double, double, char *, int);

extern  int  MapVGAPalette(int, int, unsigned char *);
extern  int  ReadVGAPalette(int, int, unsigned char *);
extern  void MapVGAGreyScale(int brightness);
extern  void Reset256PalRange(int lo, int hi);
extern  void Remap256ColorTable(unsigned char *color_buf, int n_colors,
     unsigned char *source_DACs, unsigned char *dest_DACs);

extern  unsigned int AllocGFXVideoRam(unsigned int);
extern  int AllocGFXWorkspace(int, VOID_FAR *);
extern  unsigned int AllocGFXSystemRam(unsigned int);
extern  unsigned int AllocGFXDiskMem(char *, unsigned int);

extern  void FreeGFXWorkspace(void);
extern  void FreeGFXSystemRam(void);
extern  void FreeGFXDiskMem(void);


extern  int  OpenBlobFont(int, int);
extern  int  GetBlobNameIndex(char *, int);
extern  int  InitBlobNameList(char *, int, int, int *);
extern  int  GetBlobNameList(char *, int, int, char **, int);
extern  void SortBlobNameList(char *[], int, int);
extern  int  GetBlobItemName(char *, int, int);
extern  int  SetBlobItemName(char *, int, int);
extern  int  SliceFileFromBlob(char *, int, int, int);
extern  int  AddFileToBlob(char *, int, int, int, int);
extern  UTINY_FAR *LoadBlobItem(UTINY_FAR *, long *, int, int);

extern  int  DeleteBlobItem(int, int);
extern  int  GetBlobItemInfo(BLOB_ITEM *, int, int);
extern  int  CloseBlobFile(int);
extern  int  OpenBlobFile(char *, int);
extern  int  CreateBlobFile(char *, int, int);

extern  PIC *GetDefaultPicStructPtr(void);
extern  void SetDefaultPicUserFlags(int);
extern  int  InitPicStructForBlob(PIC *, int, int, int);
extern  void InitPicStruct(PIC *, int);

extern  int AFdecl PutImage(int, int, PIC *, int);
extern  int AFdecl PutImageRel(int, int, PIC *, int);
extern  int AFdecl PutImageF(double, double, PIC *, int);
extern  int AFdecl PutImageRelF(double, double, PIC *, int);

extern  int AFdecl GetImage(int, int, int, int, PIC *);
extern  int AFdecl GetImageTo(int, int, PIC *);
extern  int AFdecl GetImageToRel(int, int, PIC *);
extern  int AFdecl GetImageF(double, double, double, double, PIC *);
extern  int AFdecl GetImageToF(double, double, PIC *);
extern  int AFdecl GetImageToRelF(double, double, PIC *);

extern  long AFdecl GetImageSize(int, int, int, int, PIC *);
extern  long AFdecl GetImageSizeTo(int, int, PIC *);
extern  long AFdecl GetImageSizeToRel(int, int, PIC *);
extern  long AFdecl GetImageSizeF(double, double, double, double, PIC *);
extern  long AFdecl GetImageSizeToF(double, double, PIC *);
extern  long AFdecl GetImageSizeToRelF(double, double, PIC *);

extern  int  ReadImage(PIC *, int, int);
extern  int  WriteImage(PIC *, int, int);

extern  void SetGFXMallocData(int safe_type, MALLOC_DATA *mdata);
extern  int  InitGFXMalloc(int orig_n_kbytes, int bump_n_kbytes, void *heap_ptr);

			/*******************************************
			 *	GFX FONTS & MENUS LIBRARY FUNCTIONS   *
			 *******************************************/


	/* font functions */

int CloseFont(int fh);
int CurrFont(int fh);
int FontAlign(int fh, int alignment);
int FontColor(int fh, int fgnd, int bkgnd);
int FontMagnify(int fh, int width, int height);
int FontRotate(int fh, int rotation_angle);
int FontSpacing(int fh, int delta_space);
int FontUnderline(int fh, int location, int thickness, int color);
void AFdecl FprintFont(char *, ... );
struct _fontattr *GetFontAttr(int fh);
struct _lineattr *GetLineAttr(void);
int	GetFontHeight(int fh);
int	GetLineLen(int fh, char * text, int max_n_char);
int	LineDirection(int text_dir);
int	LineJustify(int justify);
int	LineUnderline(int on_or_off);
int	OpenFont(char * text);
int	OpenMemFont(FONT *);
int  OpenBGIStroke_font(char *font_name);
int  FastROMFont(int, int, int);
void AFdecl PrintFont(int x, int y, char * text, int max_len);
void AFdecl PrintFontRel(int dx, int dy, char * text, int max_len);
void AFdecl PrintFontF(double x, double y, char * text, int max_len);
void AFdecl PrintFontRelF(double dx, double dy, char * text, int max_len);
void GetFontCursor(COOR_PT *cpt);

	/* menu functions */

void SetEventTextMacro(char *tp);
void SetDoubleClick(int n_ticks);
void SetAutoRepeat(int pause, int pause2);
void SetAutoRepeat(int p1, int p2);
void SetMouseEvent(int mouse_event_mask);
void SetExitHook(EVENT * (*exit_hook_fcn)(), unsigned int mask_for_exit_hook);
void SetMenuHook(int (*menu_hook_fcn)(EVENT *));
void SetLoopHook(void (*event_vec)());
void SetDBOXHook(void (*hook_fcn)(int, int, ULTD_DBOX *));
void SetEventTimer(int n_ticks, int auto_repeat);
EVENT *GetEvent(int event_mask);
void PositionRootMenu(int root_top);
void SetRootMenu(ROOT_MENU *new_root);
void ClearMenus(void);
EVENT *SetBoxWrtCenter(int dx, int dy, GFX_BOX *gbox);
EVENT * AFdecl SetBox(int x, int y, GFX_BOX *gbox);
EVENT * AFdecl SetBoxToRel(int dx, int dy, GFX_BOX *gbox);
EVENT * AFdecl SetBoxF(double x, double y, GFX_BOX *gbox);
EVENT * AFdecl SetBoxToRelF(double dx, double dy, GFX_BOX *gbox);
void InitGFXMenu(int n_box_layers, int dflt_fh, int orig_x_res, int orig_y_res);
void InitVideoStorage(unsigned used_vid_mem, UTINY_FAR *buf, unsigned int n_para);
void ClearBox(void);
EVENT *RevisitBox(void);

EVENT *PeekEvent(void);
int	PushEvent(EVENT *event);
void FlushEventQ(void);
void HaltQ(void);
void SetCursorRegions(int n_regions, CURSOR_REGION *crsr_regions);
int	PushHotSpot(HOT_SPOT *hs);
int	PopHotSpots(int n_hot_spots);
void SleepAllHotSpots(void);
void WakeHotSpots(void);
void ModHotSpotsStatus(int first_hs_n, int n_hot_spots, int type, int status);
int	Qprint(int x, int y, int fh, int color, int justify, char *tp, int max_n_char);
int	QstringLen(int  fh, char  *tp);
void ShadeBox(int x, int y, int x_width, int y_height, int pattern, int color);
void ResetLayout(int n_layouts, LAYOUT *layouts[]);
int	ScaleDim(int val, int x_or_y);
void ResetColorScheme(int n_cs, COLOR_SCHEME *csa[], int n_items, COLOR_MAP *color_map);

int	InitAutoBox(int type, GFX_BOX *dflt_gbox);
GFX_BOX *BuildAutoBox(int type, char *box_title, int button_n, int n_items, char **items);
EVENT   *SetBoxWrtMenu(int justify, int delta_x, int delta_y, GFX_BOX *gbox);
int	GetDirectory(char *asciiz, int flags, unsigned char ***items);
int	InitMouse(void);
void InitMultiSelections(int c, int fh, int color, int max_n, int *choices);
void SetTextCursor(int type, int color, int crsr_flash);

void InitAutoGFXMenu(int used_video_pages, int n_sys_para, int n_box_layers, int dflt_fh, int x_res, int y_res);
void InitAutoButtons(int n_button_sets, BUTTON_SET **buttons);
void ResetAutoCsLayout(int save_orig_settings);
void SetDfltMenuColors(int text_c, int bkgnd_c, int btn_bkgnd, int border_type);
void UseGFXMouseCursor(void);

/*
void  _gfx_color_box(int x1, int y1, int x2, int y2, int color);
void  _gfx_solid_box(int x1, int y1, int x2, int y2, int color);
*/

/* Low level mouse functions */

int	MouseReset(void);
void MouseShowCursor(void);
void MouseHideCursor(void);
void MouseGetCursor(int crsr[3]);
void MouseSetCursor(int x, int y);
int	MouseGetButtonPressInfo(int button_n, int crsr[3]);
int	MouseGetButtonReleaseInfo(int button_n, int crsr[3]);
void MouseSetBounds(int min_x, int min_y, int max_x, int max_y);
void MouseDefineCursorShape(CURSOR_SHAPE *crsr_shape);
void MouseReadMotionCounters(int *h_count, int *v_count);
void MouseSetMickeyToPixelRatio(int x_ratio, int y_ratio);
void MouseAreaToHideCursor(int min_x, int min_y, int max_x, int max_y);
void MouseSetSpeedThreshold(int mickeys_per_sec);

/* The lower case names are also available */

int	AFdecl mouse_reset(void);
void AFdecl mouse_show_cursor(void);
void AFdecl mouse_show_crsr(void);
void AFdecl mouse_hide_cursor(void);
void AFdecl mouse_hide_crsr(void);
void AFdecl mouse_get_cursor(int crsr[3]);
void AFdecl mouse_set_cursor(int x, int y);
int	AFdecl mouse_get_button_press_info(int button_n, int crsr[3]);
int	AFdecl mouse_get_button_release_info(int button_n, int crsr[3]);
void AFdecl mouse_set_bounds(int min_x, int min_y, int max_x, int max_y);
void AFdecl mouse_define_cursor_shape(CURSOR_SHAPE *crsr_shape);
void AFdecl mouse_read_motion_counters(int *h_count, int *v_count);
void AFdecl mouse_set_mickey_to_pixel_ratio(int x_ratio, int y_ratio);
void AFdecl mouse_area_to_hide_cursor(int min_x, int min_y, int max_x, int max_y);
void AFdecl mouse_set_speed_threshold(int mickeys_per_sec);


			/*******************************************
			 *	 GFX SCREEN DUMP LIBRARY FUNCTIONS    *
			 *******************************************/


int	SetMemPrinterRec(int dev_id, char *port_name);
int	SetDiskPrinterRec(int dev_id, char *port_name);
void	FreeDiskPrinterRec(void);
int	SetResidentPrinterRec(int dev_id, int resolution, char *port_name);
void SetPrxOptions(int pic, int clear, int white, int eject, int reset);
int	SetPrxMonitorAspect(int x_asp, int y_asp);
int	GetPrinterStatus(int n);
void SetPrxSwathHook(int (*hook_fcn)(int, int, int, int));
void SetPrxSolidOutput(unsigned char solid_pat[16]);
void SetPrxShading(unsigned char *usr_pat_data);
void SetPrxColorTable(unsigned char *usr_color_data);
int	SetPrxOutputScale(int type);
void SetPrxTableName(char *table_name);
int  AFdecl PrintScreen(int width, int height, int lmargin, int tmargin);
int	AFdecl PrintArea(int x1, int y1, int x2, int y2, int width, int height, int lm, int tm);
int	AFdecl PrintAreaTo(int x2, int y2, int width, int height, int lm, int tm);
int	AFdecl PrintAreaToRel(int dx, int dy, int width, int height, int lm, int tm);
void LinkAllPrinterTypes(void);
void LinkDotMatrixPrinters(void);
void LinkLaserPrinters(void);
int  PrintPCXFile(char *fname, int x1, int y1, int x2, int y2,
                int width, int height, int lmargin, int tmargin);
void SetPrxImageInputHook(int (*usr_fcn)(UTINY_FAR *, int, int, int));


#elif defined (USE_UPPER_C)

			/*******************************************
			 *	PROTOTYPES FOR UPPER CASE INTERFACE   *
			 *******************************************/

			/*******************************************
			 *	   GFX GRAPHICS LIBRARY FUNCTIONS	   *
			 *******************************************/


	/* Setting the video 'playing field' */

extern  void COLOR(int fgnd, int bkgnd, int palette, ...);
extern  int  COPY_VIDEO_PAGES(int from_page, int to_page);
extern  void INIT_GFX_STRUCT(int bios_nmbr);
extern  int  SCREEN(int mode);
extern  void SET_VIDEO_RESOLUTION(int res_x, int res_y);
extern  int  SET_EXTENDED_MODE(int x_res, int y_res, int is_color, int mode_n);
extern  int  SET_VIDEO_PAGES(int vpage, int wpage);
extern  unsigned int  GET_CARD_MONITOR(void);
extern  int  MAP_EGA_PALETTE(int index, int color);
extern  int  ROTATE_SCREEN(int rotate_dir);

	/* Functions getting various screen coordinates. */

extern  void CONVERT_COOR_PT(COOR_PT *cpt, int type);
extern  void GET_CURR_PT(COOR_PT *cpt);
extern  void VIEW_ORIGIN(COOR_PT *cpt);
extern  void VIEW_LR_CORNER(COOR_PT *cpt);
extern  void VIEW_UL_CORNER(COOR_PT *cpt);

	/* Pattern setting functions */

extern  void SET_LINE_PAT(unsigned int pat);
extern  void SET_FILL_PAT(unsigned char *pat, int color, int bkgnd_color);
extern  void SET_FILL_SOLID(int color);

	/* General purpose */

extern  void CLS( void );
extern  void MOVE_TO(int, ... );
extern  int  POINT(int, ... );
extern  void SET_VIDEO_ASPECT_RATIO(int, int);
extern  void SET_MALLOC_HOOK(void *(* malloc_hook)(), void (* free_hook)());
extern  void SET_FARMALLOC_HOOK(UTINY_FAR *(* malloc_hook)(), void (* free_hook)());
extern  int  USE_LOGICAL_COOR(void);
extern  int  USE_ABSOLUTE_COOR(void);

	/* Drawing functions */

extern  void BOX(int, ... );
extern  void CIRCLE(int, ... );
extern  void CIRCLE_ARC(int, ... );
extern  int  DRAW(char *fmt, ... );
extern  int  DRAW_LINE(int, ... );
extern  void OVAL(int, ... );
extern  void OVAL_ARC(int, ... );
extern  int  PIE_SLICE(int, ... );
extern  int  OVAL_PIE_SLICE(int, ... );
extern  void POLY_POINT(int, ... );
extern  int  POLY_LINE(int, ... );
extern  int  PSET(int, ... );
extern  int  SWAP_PEL(int, ... );

	/* Seed fill functions */

extern  int  FASTFILL(int, ... );
extern  int  PAINT(int, ... );

	/* Pic & FPic functions */

extern  int  CLOSE_FPIC(int pic_handle);
extern  int  COMPRESS_FPIC_FILE(char *file_name, char *temp_file_name);
extern  int  CREATE_FPIC(char *fname, int max_fpics);
extern  int  DELETE_FPIC(int fpic_index, int fpic_handle);
extern  int  GET_FPIC(int, ... );
extern  int  FREE_PIC(PIC *);
extern  PIC *GET_PIC(int, ... );
extern  int  OPEN_FPIC(char *fname, int update_file);
extern  int  PUT_FPIC(int, ... );
extern  int  PUT_PIC(int, ... );
extern  PIC *READ_FPIC(int fpic_index, int fpic_handle);
extern  int  WRITE_FPIC(PIC *pic, int fpic_index, int fpic_handle);

	/* Viewport control functions */

extern  int  CLEAR_VIEW(int view_n, int color);
extern  int  CLOSE_VIEW(int view_n, int hide_view);
extern  int  CLOSE_VIEW_FILE(void );
extern  int  CREATE_VIEW_FILE(char *fpic_name);
extern  void LOGICAL_INT_VIEW(int x_ul, int y_ul, int x_lr, int y_lr);
extern  void LOGICAL_FLOAT_VIEW(double x_ul, double y_ul, double x_lr, double y_lr);
extern  int  MOVE_VIEW(int, ... );
extern  int  OPEN_VIEW(int, ... );
extern  int  SWITCH_VIEW(int new_view_n, int hide_view);

	/* Now obsolete functions */
extern  int  LINE(int, ... );		/* replaced by DRAW_LINE & BOX */
extern  int  LOCATE(int, int );
extern  void SCALE_INT_VIEW(int x_scale, int y_scale, int x_org, int y_org, int x_dir, int y_dir);
extern  void SCALE_FLOAT_VIEW(double x_scale, double y_scale, double x_org, double y_org, int x_dir, int y_dir);


/* Version 3.0 Functions */

extern int IDENTIFY_SVGA(void);
extern int SET_SVGA_MODE(int);

extern  int  GET_GFX_ERROR(void);
extern  int  SET_GFX_ERROR_REPORTING(int flags, int (*hook)(int, int));

extern  void GET_ARC_COOR(ARC_COOR *);
extern  int AFdecl SET_VIEW_CLIPPING(int type, ...);
extern  void GET_VIEW_CLIPPING(BOX_COOR *);
extern  void GET_VIDEO_ASPECT_RATIO(int *, int *);

extern int  PUT_PCX_FILE(int arg, ...);
extern int  GET_PCX_FILE(int arg, ... );
extern int  READ_PCX_HEADER(char *file_name, PCX_HDR *pcx_hdr);
extern int  GET_MEM_PCX_IMAGE(PIC *user_pic, PCX_HDR *mem_PCX);

extern  int  MAP_VGA_PALETTE(int, int, unsigned char *);
extern  int  READ_VGA_PALETTE(int, int, unsigned char *);
extern  void MAP_VGA_GREY_SCALE(int brightness);
extern  void RESET_256_PAL_RANGE(int lo, int hi);
extern  void REMAP_256_COLOR_TABLE(unsigned char *color_buf, int n_colors,
   unsigned char *source_DACs, unsigned char *dest_DACs);

extern  unsigned int ALLOC_GFX_VIDEO_RAM(unsigned int);
extern  int ALLOC_GFX_WORKSPACE(int, VOID_FAR *);
extern  unsigned int ALLOC_GFX_SYSTEM_RAM(unsigned int);
extern  unsigned int ALLOC_GFX_DISK_MEM(int, unsigned int);

extern  void FREE_GFX_WORKSPACE(void);
extern  void FREE_GFX_SYSTEM_RAM(void);
extern  void FREE_GFX_DISK_MEM(void);

extern  int  GET_BLOB_NAME_INDEX(char *, int);
extern  int  INIT_BLOB_NAME_LIST(char *, int, int, int *);
extern  int  GET_BLOB_NAME_LIST(char *, int, int, char **, int);
extern  int  OPEN_BLOB_FONT(int, int);
extern  void SORT_BLOB_NAME_LIST(char *blob_names[], int, int);
extern  int  GET_BLOB_ITEM_NAME(char *, int, int);
extern  int  SET_BLOB_ITEM_NAME(char *, int, int);
extern  int  SLICE_FILE_FROM_BLOB(char *, int, int, int);
extern  int  ADD_FILE_TO_BLOB(char *, int, int, int, int);
extern  UTINY_FAR *LOAD_BLOB_ITEM(UTINY_FAR *, long *, int, int);
extern  int  PUT_IMAGE(int arg, ...);
extern  int  GET_IMAGE(int arg, ...);
extern  long GET_IMAGE_SIZE(int arg, ...);
extern  int  OPEN_PCX_FILE(char *, PIC *, PCX_HDR *);
extern  int  READ_PCX_FILE(char *, PIC *, PCX_HDR *);
extern  PIC *GET_DEFAULT_PIC_STRUCT_PTR(void);
extern  void SET_DEFAULT_PIC_USER_FLAGS(int);
extern  int  INIT_PIC_STRUCT_FOR_BLOB(PIC *, int, int);
extern  void INIT_PIC_STRUCT(PIC *,  int);
extern  int  DELETE_BLOB_ITEM(int, int);
extern  int  GET_BLOB_ITEM_INFO(BLOB_ITEM *, int, int);
extern  int  CLOSE_BLOB_FILE(int);
extern  int  OPEN_BLOB_FILE(char *, int);
extern  int  CREATE_BLOB_FILE(char *, int, int);
extern  void SET_GFX_MALLOC_DATA(int safe_type, MALLOC_DATA *mdata);
extern  int  INIT_GFX_MALLOC(int orig_n_kbytes, int bump_n_kbytes, void *heap_ptr);

			/*******************************************
			 *	GFX FONTS & MENUS LIBRARY FUNCTIONS   *
			 *******************************************/

int CLOSE_FONT(int fh);
int CURR_FONT(int fh);
int FONT_ALIGN(int fh, int alignment);
int FONT_COLOR(int fh, int fgnd, int bkgnd);
int FONT_MAGNIFY(int fh, int width, int height);
int FONT_ROTATE(int fh, int rotation_angle);
int FONT_SPACING(int fh, int delta_space);
int FONT_UNDERLINE(int fh, int location, int thickness, int color);
void FPRINT_FONT(char *, ... );
struct _fontattr *GET_FONT_ATTR(int fh);
struct _lineattr *GET_LINE_ATTR(void);
int GET_FONT_HEIGHT(int fh);
int GET_LINE_LEN(int fh, char * text, int max_n_char);
int LINE_DIRECTION(int text_dir);
int LINE_JUSTIFY(int justify);
int LINE_UNDERLINE(int on_or_off);
int OPEN_FONT(char * text);
int OPEN_MEM_FONT(FONT *);
int OPEN_BGI_STROKE_FONT(char *font_name);
void PRINT_FONT(int, ... );
void GET_FONT_CURSOR(COOR_PT *cpt);

#endif


			/*******************************************
			 *	PROTOTYPES FOR LOWER CASE INTERFACE   *
			 *******************************************/

			/*******************************************
			 *	   GFX GRAPHICS LIBRARY FUNCTIONS	   *
			 *******************************************/

#if defined (USE_LOWER_C)

	/* Setting the video 'playing field' */

extern  void AFdecl color(int fgnd, int bkgnd, int palette, ...);
extern  int  copy_video_pages(int from_page, int to_page);
extern  void init_GFX_struct(int bios_nmbr);
extern  int  screen(int mode);
extern  void set_video_resolution(int res_x, int res_y);
extern  int  set_extended_mode(int x_res, int y_res, int is_color, int mode_n);
extern  int  set_video_pages(int vpage, int wpage);
extern  unsigned int  get_card_monitor(void);
extern  int  map_EGA_palette(int index, int color);
extern  int  rotate_screen(int rotate_dir);

	/* Functions getting various screen coordinates. */

extern  void convert_coor_pt(COOR_PT *cpt, int type);
extern  void get_CAP(COOR_PT *cpt);
extern  void view_origin(COOR_PT *cpt);
extern  void view_lr_corner(COOR_PT *cpt);
extern  void view_ul_corner(COOR_PT *cpt);

	/* Pattern setting functions */

extern  void set_line_pat(unsigned int pat);
extern  void set_fill_pat(unsigned char *pat, int color, int bkgnd_color);
extern  void set_fill_solid(int color);

	/* General purpose */

extern  void cls( void );
extern  void AFdecl move_to(int, ... );
extern  int  AFdecl point(int, ... );
extern  void AFdecl move_to_f(double, ... );
extern  int  AFdecl point_f(double, ... );
extern  void set_video_aspect_ratio(int, int);
extern  void set_malloc_hook(void *(* malloc_hook)(), void (* free_hook)());
extern  void set_farmalloc_hook(VOID_FAR *(* malloc_hk)(), void (* free_hk)());
extern  int  use_logical_coor(void);
extern  int  use_absolute_coor(void);

	/* Drawing functions for integer viewports */

#if !defined (__GRAPHX_DEF_)		/* circle clashes w/ borland C's circle() */
extern  void AFdecl circle(int, ... );
#endif
extern  void AFdecl box(int, ... );
extern  void AFdecl circle_arc(int, ... );
extern  int  AFdecl draw(char *fmt, ... );
extern  int  AFdecl draw_line(int, ... );
extern  void AFdecl oval(int, ... );
extern  void AFdecl oval_arc(int, ... );
extern  int  AFdecl pie_slice(int, ... );
extern  int  AFdecl oval_pie_slice(int, ... );
extern  void AFdecl poly_point(int, ... );
extern  int  AFdecl poly_line(int, ... );
extern  int  AFdecl pset(int, ... );
extern  int  AFdecl swap_pel(int, ... );

	/* Drawing functions for floating-point viewports */

extern  void AFdecl box_f(double, ... );
extern  void AFdecl circle_f(double, ... );
extern  void AFdecl circle_arc_f(double, ... );
extern  int  AFdecl draw_line_f(double, ... );
extern  void AFdecl oval_f(double, ... );
extern  void AFdecl oval_arc_f(double, ... );
extern  int  AFdecl pie_slice_f(double, ... );
extern  int  AFdecl oval_pie_slice_f(double, ... );
extern  void AFdecl poly_point_f(double, ... );
extern  int  AFdecl poly_line_f(double, ... );
extern  int  AFdecl pset_f(double, ... );
extern  int  AFdecl swap_pel_f(double, ... );

	/* Seed fill functions */
				/* integer viewports */
extern  int  AFdecl fastfill(int, ... );
extern  int  AFdecl paint(int, ... );

				/* floating-point viewports */
extern  int  AFdecl fastfill_f(double, ... );
extern  int  AFdecl paint_f(double, ... );


	/* Pic & FPic functions */

extern  int  close_fpic(int pic_handle);
extern  int  compress_fpic_file(char *file_name, char *temp_file_name);
extern  int  create_fpic(char *fname, int max_fpics);
extern  int  delete_fpic(int fpic_index, int fpic_handle);
extern  int  free_pic(PIC *);
extern  int  open_fpic(char *fname, int update_file);
extern  PIC *read_fpic(int fpic_index, int fpic_handle);
extern  int  write_fpic(PIC *pic, int fpic_index, int fpic_handle);

				/* functions for integer viewports */
extern  int  AFdecl get_fpic(int, ... );
extern  PIC * AFdecl get_pic(int, ... );
extern  int  AFdecl put_fpic(int, ... );
extern  int  AFdecl put_pic(int, ... );

				/* functions for floating-point viewports */
extern  int  AFdecl get_fpic_f(double, ... );
extern  PIC * AFdecl get_pic_f(double, ... );
extern  int  AFdecl put_fpic_f(double, ... );
extern  int  AFdecl put_pic_f(double, ... );

	/* Viewport control functions */

extern  int  clear_view(int view_n, int color);
extern  int  close_view(int view_n, int hide_view);
extern  int  close_view_file(void );
extern  int  delete_view_file(void );
extern  int  create_view_file(char *fpic_name);
extern  void logical_int_view(int x_ul, int y_ul, int x_lr, int y_lr);
extern  void logical_float_view(double x_ul, double y_ul, double x_lr, double y_lr);
extern  int  switch_view(int new_view_n, int hide_view);

				/* integer viewports */
extern  int  AFdecl move_view(int, ... );
extern  int  AFdecl open_view(int, ... );

				/* floating-point viewports */
extern  int  AFdecl move_view_f(double, ... );
extern  int  AFdecl open_view_f(double, ... );

	/* Now obsolete functions */
extern  int  locate(int, int );
extern  void scale_int_view(int x_scale, int y_scale, int x_org, int y_org, int x_dir, int y_dir);
extern  void scale_float_view(double x_scale, double y_scale, double x_org, double y_org, int x_dir, int y_dir);

	/* NEW 3.0 FUNCTIONS */

extern  int  identify_SVGA(void);
extern  int  set_SVGA_mode(int);
extern  int  get_SVGA_info(int **svga_modes, int vram);

extern  int  get_GFX_error(void);
extern  int  set_GFX_error_reporting(int flags, int (*hook)(int, int));

extern  int  AFdecl put_PCX_file(int arg, ...);
extern  int  AFdecl get_PCX_file(int arg, ... );

extern  int  read_PCX_header(char *, PCX_HDR *);
extern  int  open_PCX_file(char *, PIC *, PCX_HDR *);
extern  int  read_PCX_file(char *, PIC *, PCX_HDR *);
extern  int  get_mem_PCX_image(PIC *user_pic, PCX_HDR *mem_PCX);

extern  void get_arc_coor(ARC_COOR *);
extern  int  AFdecl set_view_clipping(int clip_type, ...);
extern  void get_view_clipping(BOX_COOR *);
extern  void get_video_aspect_ratio(int *, int *);

extern  int  map_VGA_palette(int, int, unsigned char *);
extern  int  read_VGA_palette(int, int, unsigned char *);
extern  void map_VGA_grey_scale(int brightness);
extern  void reset_256_pal_range(int lo, int hi);
extern  void remap_256_color_table(unsigned char *color_buf, int n_colors,
		    unsigned char *source_DACs, unsigned char *dest_DACs);

extern  unsigned int alloc_GFX_video_ram(unsigned int);
extern  int alloc_GFX_workspace(int, VOID_FAR *);
extern  unsigned int alloc_GFX_system_ram(unsigned int);
extern  unsigned int alloc_GFX_disk_mem(char *, unsigned int);

extern  void free_GFX_workspace(void);
extern  void free_GFX_system_ram(void);
extern  void free_GFX_disk_mem(void);

extern  int  get_blob_name_index(char *, int);
extern  int  init_blob_name_list(char *, int, int, int *);
extern  int  get_blob_name_list(char *, int, int, char **, int);
extern  int  open_blob_font(int, int);
extern  void sort_blob_name_list(char *[], int, int);
extern  int  get_blob_item_name(char *, int, int);
extern  int  set_blob_item_name(char *, int, int);
extern  int  slice_file_from_blob(char *, int, int, int);
extern  int  add_file_to_blob(char *, int, int, int, int);
extern  UTINY_FAR *load_blob_item(UTINY_FAR *, long *, int, int);
extern  PIC *get_default_pic_struct_ptr(void);
extern  void set_default_pic_user_flags(int);
extern  int  init_pic_struct_for_blob(PIC *, int, int, int);
extern  void init_pic_struct(PIC *,  int);
extern  int  delete_blob_item(int, int);
extern  int  get_blob_item_info(BLOB_ITEM *, int, int);
extern  int  close_blob_file(int);
extern  int  open_blob_file(char *, int);
extern  int  create_blob_file(char *, int, int);
extern  long AFdecl get_image_size(int, ...);
extern  long AFdecl get_image_size_f(double, ...);
extern  int  AFdecl put_image(int, ...);
extern  int  AFdecl put_image_f(double, ...);
extern  int  AFdecl get_image(int, ...);
extern  int  AFdecl get_image_f(double, ...);
extern  int  read_image(PIC *, int, int);
extern  int  write_image(PIC *, int, int);
extern  void set_GFX_malloc_data(int safe_type, MALLOC_DATA *mdata);
extern  int  init_GFX_malloc(int orig_n_kbytes, int bump_n_kbytes, void *heap_ptr);

	/* NEW functions for BGI interface */

extern void BGIGFX_graphdefaults(void);
extern int  BGIGFX_set_image_compression(int type);


			/*******************************************
			 *	GFX FONTS & MENUS LIBRARY FUNCTIONS   *
			 *******************************************/

	/* font functions */

extern int close_font(int);
extern int curr_font(int);
extern int font_align(int, int);
extern int font_color(int, int, int);
extern int font_magnify(int, int, int);
extern int font_rotate(int, int);
extern int font_spacing(int, int);
extern int font_underline(int, int, int, int);
extern void AFdecl fprint_font(char *, ...);
extern struct _fontattr *get_font_attr(int);
struct _lineattr *get_line_attr(void);
extern int get_font_height(int);
extern int get_line_len(int , char *,int);
extern int line_direction(int);
extern int line_justify(int);
extern int line_underline(int);
extern int open_font(char *);
extern int open_mem_font(FONT *);
extern int open_BGI_stroke_font(char *font_name);
extern int fast_ROM_font(int, int, int);
extern void AFdecl print_font(int, ...);
extern void AFdecl print_font_f(double, ...);
extern void get_font_cursor(COOR_PT *cpt);
#endif


#if defined (USE_UPPER_C) || (USE_LOWER_C)

	/* menu functions */

extern void  set_event_text_macro(char *tp);
extern void  set_double_click(int n_ticks);
extern void  set_auto_repeat(int pause, int pause2);
extern void  set_mouse_event(int mouse_event_mask);
extern void  init_exit_hook(EVENT *(*exit_hook_fcn)(), unsigned int mask_for_exit_hook);
extern void  init_loop_hook(void  (*event_vec)());
extern void  set_exit_hook(EVENT *(*exit_hook_fcn)(), unsigned int mask_for_exit_hook);
extern void  set_loop_hook(void  (*event_vec)());
extern void  set_menu_hook(int (*menu_hook_fcn)(EVENT *));
extern void  set_DBOX_hook(void (*hook_fcn)(int, int, ULTD_DBOX *));
extern void  init_event_timer(int n_ticks, int auto_repeat);
extern EVENT *peek_event(void);
extern int   push_event(EVENT *event);
extern void  flush_event_q(void);
extern void  halt_q(void);
extern void  set_cursor_regions(int n_regions, CURSOR_REGION *crsr_regions);
extern EVENT *get_event(int event_mask);
extern int   qprint(int x, int y, int fh, int color, int justify, char *_tp, int max_n_char);
extern int   qstring_len(int	fh, char	*tp);
extern int   push_hot_spot(HOT_SPOT *hs);
extern int   pop_hot_spots(int n_hot_spots);
extern void  sleep_all_hot_spots(void);
extern void  wake_hot_spots(void);
extern void  mod_hot_spots_status(int first_hs_n, int n_hot_spots, int type, int status);
extern void  set_root_menu(ROOT_MENU *new_root);
extern void  position_root_menu(int root_top);
extern void  menu_title(char *title, int color, int fh, int r_pad);
extern void  clear_menus(void);
extern void  init_GFX_menu(int n_box_layers, int dflt_fh, int orig_x_res, int orig_y_res);
extern void  init_video_storage(unsigned int used_vid_mem, UTINY_FAR *buf, unsigned int n_para);
extern void  set_dflt_menu_colors(int text_c, int bkgnd_c, int btn_bkgnd, int border_type);
extern void  reset_auto_cs_layout(int save_orig_settings);
extern void  reset_color_scheme(int n_cs, COLOR_SCHEME **csa, int n_items, COLOR_MAP *color_map);
extern void  reset_layout(int n_layouts, LAYOUT **layouts);
extern void  init_auto_GFX_menu(int used_video_pages, int n_sys_para, int n_box_layers, int dflt_fh, int x_res, int y_res);
extern void  init_auto_gfx_menu(int used_video_pages, int n_sys_para, int n_box_layers, int dflt_fh, int x_res, int y_res);
extern EVENT *set_box_wrt_menu(int justify, int delta_x, int delta_y, GFX_BOX *gbox);
extern EVENT *set_box_wrt_center(int dx, int dy, GFX_BOX *gbox);
extern void  clear_box(void);
extern int   curr_box(int box_n);
extern EVENT * AFdecl set_box(int args, ...);
extern EVENT *_set_box(int x, int y, GFX_BOX *gbox);
extern EVENT *revisit_box(void);
extern int   init_auto_box(int type, GFX_BOX *dflt_gbox);
extern void  init_auto_buttons(int n_button_sets, BUTTON_SET **buttons);
extern int   init_mouse(void);
extern void  init_multi_selections(int c, int fh, int color, int max_n, int *choices);
extern void  set_text_cursor(int type, int color, int crsr_flash);

extern GFX_BOX *build_auto_box(int type, char *box_title, int button_n, int n_items, char **items);
extern int   get_directory(char *asciiz, int flags, unsigned char ***items);
extern void  shade_box(int x, int y, int x_width, int y_height, int pattern, int color);
extern int   scale_dim(int val, int x_or_y);
extern void  use_GFX_mouse_cursor(void);

/*
extern void  _gfx_color_box(int x1, int y1, int x2, int y2, int color);
extern void  _gfx_solid_box(int x1, int y1, int x2, int y2, int color);
*/
/* Low level mouse functions */

extern int  AFdecl mouse_reset(void);
extern void AFdecl mouse_show_cursor(void);
extern void AFdecl mouse_show_crsr(void);
extern void AFdecl mouse_hide_cursor(void);
extern void AFdecl mouse_hide_crsr(void);
extern void AFdecl mouse_get_cursor(int crsr[3]);
extern void AFdecl mouse_set_cursor(int x, int y);
extern int  AFdecl mouse_get_button_press_info(int button_n, int crsr[3]);
extern int  AFdecl mouse_get_button_release_info(int button_n, int crsr[3]);
extern void AFdecl mouse_set_bounds(int min_x, int min_y, int max_x, int max_y);
extern void AFdecl mouse_define_cursor_shape(CURSOR_SHAPE *crsr_shape);
extern void AFdecl mouse_read_motion_counters(int *h_count, int *v_count);
extern void AFdecl mouse_set_mickey_to_pixel_ratio(int x_ratio, int y_ratio);
extern void AFdecl mouse_area_to_hide_cursor(int min_x, int min_y, int max_x, int max_y);
extern void AFdecl mouse_set_speed_threshold(int mickeys_per_sec);


			/*******************************************
			 *	 GFX SCREEN DUMP LIBRARY FUNCTIONS    *
			 *******************************************/

extern int  set_mem_printer_rec(int dev_id, char *port_name);
extern int  set_disk_printer_rec(int dev_id, char *port_name);
extern void free_disk_printer_rec(void);
extern int  set_resident_printer_rec(int dev_id, int resolution, char *port_name);
extern void set_prx_options(int pic, int clear, int white, int eject, int reset);
extern int  set_prx_monitor_aspect(int x_asp, int y_asp);
extern int  get_printer_status(int n);
extern void set_prx_swath_hook(int (*hook_fcn)(int, int, int, int));
extern int  AFdecl print_screen(int width, int height, int lmargin, int tmargin);
extern int  AFdecl print_area(int arg, ...);
extern int  AFdecl print_area_f(double arg, ...);
extern void set_prx_solid_output(unsigned char solid_pat[16]);
extern void set_prx_shading(unsigned char *usr_pat_data);
extern void set_prx_color_table(unsigned char *usr_color_data);
extern int  set_prx_output_scale(int type);
extern void set_prx_table_name(char *table_name);
extern void link_all_printer_types(void);
extern void link_dot_matrix_printers(void);
extern void link_laser_printers(void);
extern int  print_PCX_file(char *fname, int x1, int y1, int x2, int y2,
                int width, int height, int lmargin, int tmargin);
extern void set_prx_image_input_hook(int (*usr_fcn)(UTINY_FAR *, int, int, int));
#endif

#if defined (__cplusplus)
	}
#endif

#endif




