//	Zinc Interface Library - UI_MAP.HPP
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


// ----- Include file and environment definition ----------------------------

#if !defined(UI_MAP_HPP)
#	define UI_MAP_HPP
//#	if !defined(UI_DSP_HPP)
#		if defined(__OS2__)
#			if !defined(INCL_WININPUT)
#				define INCL_WININPUT
#			endif
#			if !defined(INCL_WINMESSAGEMGR)
#				define INCL_WINMESSAGEMGR
#			endif
#		endif
#		include <ui_dsp.hpp>
//#	endif

// ----- Compiler/Environment Dependencies ----------------------------------

// ----- Special hotkey values ----------------------------------------------

const ZIL_UINT8 HOT_KEY_MAXIMIZE			= 252;
const ZIL_UINT8 HOT_KEY_MINIMIZE			= 253;
const ZIL_UINT8 HOT_KEY_SYSTEM				= 254;
const ZIL_UINT8 HOT_KEY_SUB_WINDOW			= 255;

// --------------------------------------------------------------------------
// ----- ZIL_MSDOS ----------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSDOS) && defined(USE_RAW_KEYS)
const ZIL_RAW_CODE ESCAPE					= 0x011B;	// Miscellaneous keys
const ZIL_RAW_CODE ENTER					= 0x1C0D;
const ZIL_RAW_CODE CTRL_ENTER				= 0x1C0A;
const ZIL_RAW_CODE SPACE					= 0x3920;
const ZIL_RAW_CODE BACKSPACE				= 0x0E08;
const ZIL_RAW_CODE CTRL_BACKSPACE			= 0x0E7F;
const ZIL_RAW_CODE ALT_BACKSPACE			= 0x0E00;
const ZIL_RAW_CODE TAB						= 0x0F09;
const ZIL_RAW_CODE CTRL_TAB					= 0x9400;
const ZIL_RAW_CODE BACKTAB					= 0x0F00;

const ZIL_RAW_CODE CTRL_BREAK				= 0x0000;
const ZIL_RAW_CODE CTRL_C					= 0x2E03;

const ZIL_RAW_CODE ALT_ESCAPE				= 0x0100;
const ZIL_RAW_CODE ALT_PERIOD				= 0x3400;
const ZIL_RAW_CODE ALT_SPACE				= 0x3920;
const ZIL_RAW_CODE ALT_WHITE_MINUS			= 0x8200;
const ZIL_RAW_CODE ALT_WHITE_PLUS			= 0x8300;

const ZIL_RAW_CODE ALT_A					= 0x1E00;
const ZIL_RAW_CODE ALT_B					= 0x3000;
const ZIL_RAW_CODE ALT_C					= 0x2E00;
const ZIL_RAW_CODE ALT_D					= 0x2000;
const ZIL_RAW_CODE ALT_E					= 0x1200;
const ZIL_RAW_CODE ALT_F					= 0x2100;
const ZIL_RAW_CODE ALT_G					= 0x2200;
const ZIL_RAW_CODE ALT_H					= 0x2300;
const ZIL_RAW_CODE ALT_I					= 0x1700;
const ZIL_RAW_CODE ALT_J					= 0x2400;
const ZIL_RAW_CODE ALT_K					= 0x2500;
const ZIL_RAW_CODE ALT_L					= 0x2600;
const ZIL_RAW_CODE ALT_M					= 0x3200;
const ZIL_RAW_CODE ALT_N					= 0x3100;
const ZIL_RAW_CODE ALT_O					= 0x1800;
const ZIL_RAW_CODE ALT_P					= 0x1900;
const ZIL_RAW_CODE ALT_Q					= 0x1000;
const ZIL_RAW_CODE ALT_R					= 0x1300;
const ZIL_RAW_CODE ALT_S					= 0x1F00;
const ZIL_RAW_CODE ALT_T					= 0x1400;
const ZIL_RAW_CODE ALT_U					= 0x1600;
const ZIL_RAW_CODE ALT_V					= 0x2F00;
const ZIL_RAW_CODE ALT_W					= 0x1100;
const ZIL_RAW_CODE ALT_X					= 0x2D00;
const ZIL_RAW_CODE ALT_Y					= 0x1500;
const ZIL_RAW_CODE ALT_Z					= 0x2C00;

const ZIL_RAW_CODE ALT_1					= 0x7800;
const ZIL_RAW_CODE ALT_2					= 0x7900;
const ZIL_RAW_CODE ALT_3					= 0x7A00;
const ZIL_RAW_CODE ALT_4					= 0x7B00;
const ZIL_RAW_CODE ALT_5					= 0x7C00;
const ZIL_RAW_CODE ALT_6					= 0x7D00;
const ZIL_RAW_CODE ALT_7					= 0x7E00;
const ZIL_RAW_CODE ALT_8					= 0x7F00;
const ZIL_RAW_CODE ALT_9					= 0x8000;
const ZIL_RAW_CODE ALT_0					= 0x8100;

const ZIL_RAW_CODE GRAY_ENTER				= 0xE00D;	// Grey keys
const ZIL_RAW_CODE GRAY_CTRL_ENTER	  	= 0xE00A;
const ZIL_RAW_CODE GRAY_UP_ARROW			= 0x48E0;
const ZIL_RAW_CODE GRAY_DOWN_ARROW			= 0x50E0;
const ZIL_RAW_CODE GRAY_LEFT_ARROW			= 0x4BE0;
const ZIL_RAW_CODE GRAY_RIGHT_ARROW			= 0x4DE0;
const ZIL_RAW_CODE GRAY_INSERT				= 0x52E0;
const ZIL_RAW_CODE GRAY_DELETE				= 0x53E0;
const ZIL_RAW_CODE GRAY_HOME				= 0x47E0;
const ZIL_RAW_CODE GRAY_END					= 0x4FE0;
const ZIL_RAW_CODE GRAY_PGUP				= 0x49E0;
const ZIL_RAW_CODE GRAY_PGDN				= 0x51E0;
const ZIL_RAW_CODE GRAY_DIVIDE				= 0xE02F;
const ZIL_RAW_CODE GRAY_MULTIPLY			= 0x372A;
const ZIL_RAW_CODE GRAY_PLUS				= 0x4E2B;
const ZIL_RAW_CODE GRAY_MINUS				= 0x4A2D;

const ZIL_RAW_CODE CTRL_GRAY_UP_ARROW		= 0x8DE0;
const ZIL_RAW_CODE CTRL_GRAY_DOWN_ARROW		= 0x91E0;
const ZIL_RAW_CODE CTRL_GRAY_LEFT_ARROW		= 0x73E0;
const ZIL_RAW_CODE CTRL_GRAY_RIGHT_ARROW	= 0x74E0;
const ZIL_RAW_CODE CTRL_GRAY_INSERT			= 0x92E0;
const ZIL_RAW_CODE CTRL_GRAY_DELETE			= 0x93E0;
const ZIL_RAW_CODE CTRL_GRAY_HOME			= 0x77E0;
const ZIL_RAW_CODE CTRL_GRAY_END			= 0x75E0;
const ZIL_RAW_CODE CTRL_GRAY_PGUP			= 0x84E0;
const ZIL_RAW_CODE CTRL_GRAY_PGDN			= 0x76E0;
const ZIL_RAW_CODE CTRL_GRAY_DIVIDE			= 0x9500;
const ZIL_RAW_CODE CTRL_GRAY_MULTIPLY		= 0x9600;
const ZIL_RAW_CODE CTRL_GRAY_PLUS			= 0x9000;
const ZIL_RAW_CODE CTRL_GRAY_MINUS			= 0x8E00;

const ZIL_RAW_CODE ALT_GRAY_UP_ARROW		= 0x9800;
const ZIL_RAW_CODE ALT_GRAY_DOWN_ARROW		= 0xA000;
const ZIL_RAW_CODE ALT_GRAY_LEFT_ARROW		= 0x9B00;
const ZIL_RAW_CODE ALT_GRAY_RIGHT_ARROW		= 0x9D00;
const ZIL_RAW_CODE ALT_GRAY_INSERT			= 0xA200;
const ZIL_RAW_CODE ALT_GRAY_DELETE			= 0xA300;
const ZIL_RAW_CODE ALT_GRAY_HOME			= 0x9700;
const ZIL_RAW_CODE ALT_GRAY_END				= 0x9F00;
const ZIL_RAW_CODE ALT_GRAY_PGUP			= 0x9900;
const ZIL_RAW_CODE ALT_GRAY_PGDN			= 0xA100;
const ZIL_RAW_CODE ALT_GRAY_DIVIDE			= 0xA400;
const ZIL_RAW_CODE ALT_GRAY_MULTIPLY		= 0x3700;
const ZIL_RAW_CODE ALT_GRAY_PLUS			= 0x4E00;
const ZIL_RAW_CODE ALT_GRAY_MINUS			= 0x4A00;

const ZIL_RAW_CODE WHITE_UP_ARROW			= 0x4800;	// White keys
const ZIL_RAW_CODE WHITE_DOWN_ARROW			= 0x5000;
const ZIL_RAW_CODE WHITE_LEFT_ARROW			= 0x4B00;
const ZIL_RAW_CODE WHITE_RIGHT_ARROW		= 0x4D00;
const ZIL_RAW_CODE WHITE_INSERT				= 0x5200;
const ZIL_RAW_CODE WHITE_DELETE				= 0x5300;
const ZIL_RAW_CODE WHITE_HOME				= 0x4700;
const ZIL_RAW_CODE WHITE_END				= 0x4F00;
const ZIL_RAW_CODE WHITE_PGUP				= 0x4900;
const ZIL_RAW_CODE WHITE_PGDN				= 0x5100;
const ZIL_RAW_CODE WHITE_CENTER				= 0x4C00;

const ZIL_RAW_CODE SHIFT_WHITE_UP_ARROW			= 0x4838;
const ZIL_RAW_CODE SHIFT_WHITE_DOWN_ARROW		= 0x5032;
const ZIL_RAW_CODE SHIFT_WHITE_LEFT_ARROW		= 0x4B34;
const ZIL_RAW_CODE SHIFT_WHITE_RIGHT_ARROW		= 0x4D36;
const ZIL_RAW_CODE SHIFT_WHITE_INSERT			= 0x5230;
const ZIL_RAW_CODE SHIFT_WHITE_DELETE			= 0x532E;
const ZIL_RAW_CODE SHIFT_WHITE_HOME				= 0x4737;
const ZIL_RAW_CODE SHIFT_WHITE_END				= 0x4F31;
const ZIL_RAW_CODE SHIFT_WHITE_PGUP				= 0x4939;
const ZIL_RAW_CODE SHIFT_WHITE_PGDN				= 0x5133;

const ZIL_RAW_CODE CTRL_WHITE_UP_ARROW		= 0x8D00;
const ZIL_RAW_CODE CTRL_WHITE_DOWN_ARROW	= 0x9100;
const ZIL_RAW_CODE CTRL_WHITE_LEFT_ARROW	= 0x7300;
const ZIL_RAW_CODE CTRL_WHITE_RIGHT_ARROW	= 0x7400;
const ZIL_RAW_CODE CTRL_WHITE_INSERT		= 0x9200;
const ZIL_RAW_CODE CTRL_WHITE_DELETE		= 0x9300;
const ZIL_RAW_CODE CTRL_WHITE_HOME			= 0x7700;
const ZIL_RAW_CODE CTRL_WHITE_END			= 0x7500;
const ZIL_RAW_CODE CTRL_WHITE_PGUP			= 0x8400;
const ZIL_RAW_CODE CTRL_WHITE_PGDN			= 0x7600;
const ZIL_RAW_CODE CTRL_WHITE_CENTER		= 0x8F00;

const ZIL_RAW_CODE F1						= 0x3B00;	// Function keys
const ZIL_RAW_CODE F2						= 0x3C00;
const ZIL_RAW_CODE F3						= 0x3D00;
const ZIL_RAW_CODE F4						= 0x3E00;
const ZIL_RAW_CODE F5						= 0x3F00;
const ZIL_RAW_CODE F6						= 0x4000;
const ZIL_RAW_CODE F7						= 0x4100;
const ZIL_RAW_CODE F8						= 0x4200;
const ZIL_RAW_CODE F9						= 0x4300;
const ZIL_RAW_CODE F10						= 0x4400;
const ZIL_RAW_CODE F11						= 0x8500;
const ZIL_RAW_CODE F12						= 0x8600;

const ZIL_RAW_CODE SHIFT_F1					= 0x5400;
const ZIL_RAW_CODE SHIFT_F2					= 0x5500;
const ZIL_RAW_CODE SHIFT_F3					= 0x5600;
const ZIL_RAW_CODE SHIFT_F4					= 0x5700;
const ZIL_RAW_CODE SHIFT_F5					= 0x5800;
const ZIL_RAW_CODE SHIFT_F6					= 0x5900;
const ZIL_RAW_CODE SHIFT_F7					= 0x5A00;
const ZIL_RAW_CODE SHIFT_F8					= 0x5B00;
const ZIL_RAW_CODE SHIFT_F9					= 0x5C00;
const ZIL_RAW_CODE SHIFT_F10				= 0x5D00;
const ZIL_RAW_CODE SHIFT_F11				= 0x8700;
const ZIL_RAW_CODE SHIFT_F12				= 0x8800;

const ZIL_RAW_CODE CTRL_F1					= 0x5E00;
const ZIL_RAW_CODE CTRL_F2					= 0x5F00;
const ZIL_RAW_CODE CTRL_F3					= 0x6000;
const ZIL_RAW_CODE CTRL_F4					= 0x6100;
const ZIL_RAW_CODE CTRL_F5					= 0x6200;
const ZIL_RAW_CODE CTRL_F6					= 0x6300;
const ZIL_RAW_CODE CTRL_F7					= 0x6400;
const ZIL_RAW_CODE CTRL_F8					= 0x6500;
const ZIL_RAW_CODE CTRL_F9					= 0x6600;
const ZIL_RAW_CODE CTRL_F10					= 0x6700;
const ZIL_RAW_CODE CTRL_F11					= 0x8900;
const ZIL_RAW_CODE CTRL_F12					= 0x8A00;

const ZIL_RAW_CODE ALT_F1					= 0x6800;
const ZIL_RAW_CODE ALT_F2					= 0x6900;
const ZIL_RAW_CODE ALT_F3					= 0x6A00;
const ZIL_RAW_CODE ALT_F4					= 0x6B00;
const ZIL_RAW_CODE ALT_F5					= 0x6C00;
const ZIL_RAW_CODE ALT_F6					= 0x6D00;
const ZIL_RAW_CODE ALT_F7					= 0x6E00;
const ZIL_RAW_CODE ALT_F8					= 0x6F00;
const ZIL_RAW_CODE ALT_F9					= 0x7000;
const ZIL_RAW_CODE ALT_F10					= 0x7100;
const ZIL_RAW_CODE ALT_F11					= 0x8B00;
const ZIL_RAW_CODE ALT_F12					= 0x8C00;
const ZIL_RAW_CODE Z_DONT_USE				= 0xFFFF;
#endif

// --------------------------------------------------------------------------
// ----- ZIL_MSWINDOWS ------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MSWINDOWS) && defined(USE_RAW_KEYS)
const ZIL_RAW_CODE ESCAPE					= 0x001B;	// Miscellaneous keys
const ZIL_RAW_CODE ENTER					= 0x000D;
const ZIL_RAW_CODE CTRL_ENTER				= 0x040A;
const ZIL_RAW_CODE SPACE					= 0x0020;
const ZIL_RAW_CODE SHIFT_SPACE				= 0x0320;
const ZIL_RAW_CODE CTRL_SPACE				= 0x0420;
const ZIL_RAW_CODE BACKSPACE				= 0x0008;
const ZIL_RAW_CODE CTRL_BACKSPACE			= 0x0408;
const ZIL_RAW_CODE ALT_BACKSPACE			= 0x0808;
const ZIL_RAW_CODE TAB						= 0x0009;
const ZIL_RAW_CODE CTRL_TAB					= 0x0409;
const ZIL_RAW_CODE BACKTAB					= 0x0309;

const ZIL_RAW_CODE CTRL_BREAK				= 0x0403;
const ZIL_RAW_CODE CTRL_C					= 0x0443;

const ZIL_RAW_CODE ALT_ESCAPE				= 0x081B;
const ZIL_RAW_CODE ALT_PERIOD				= 0x08BE;
const ZIL_RAW_CODE ALT_SPACE				= 0x0820;
const ZIL_RAW_CODE ALT_WHITE_MINUS			= 0x082D;
const ZIL_RAW_CODE ALT_WHITE_PLUS			= 0x082B;

const ZIL_RAW_CODE ALT_A					= 0x0861;
const ZIL_RAW_CODE ALT_B					= 0x0862;
const ZIL_RAW_CODE ALT_C					= 0x0863;
const ZIL_RAW_CODE ALT_D					= 0x0864;
const ZIL_RAW_CODE ALT_E					= 0x0865;
const ZIL_RAW_CODE ALT_F					= 0x0866;
const ZIL_RAW_CODE ALT_G					= 0x0867;
const ZIL_RAW_CODE ALT_H					= 0x0868;
const ZIL_RAW_CODE ALT_I					= 0x0869;
const ZIL_RAW_CODE ALT_J					= 0x086A;
const ZIL_RAW_CODE ALT_K					= 0x086B;
const ZIL_RAW_CODE ALT_L					= 0x086C;
const ZIL_RAW_CODE ALT_M					= 0x086D;
const ZIL_RAW_CODE ALT_N					= 0x086E;
const ZIL_RAW_CODE ALT_O					= 0x086F;
const ZIL_RAW_CODE ALT_P					= 0x0870;
const ZIL_RAW_CODE ALT_Q					= 0x0871;
const ZIL_RAW_CODE ALT_R					= 0x0872;
const ZIL_RAW_CODE ALT_S					= 0x0873;
const ZIL_RAW_CODE ALT_T					= 0x0874;
const ZIL_RAW_CODE ALT_U					= 0x0875;
const ZIL_RAW_CODE ALT_V					= 0x0876;
const ZIL_RAW_CODE ALT_W					= 0x0877;
const ZIL_RAW_CODE ALT_X					= 0x0878;
const ZIL_RAW_CODE ALT_Y					= 0x0879;
const ZIL_RAW_CODE ALT_Z					= 0x087A;

const ZIL_RAW_CODE ALT_A_GRAVE				= 0x08E0;
const ZIL_RAW_CODE ALT_A_ACUTE				= 0x08E1;
const ZIL_RAW_CODE ALT_A_CIRCUMFLEX			= 0x08E2;
const ZIL_RAW_CODE ALT_A_TILDE				= 0x08E3;
const ZIL_RAW_CODE ALT_A_DIARESIS			= 0x08E4;
const ZIL_RAW_CODE ALT_A_RING				= 0x08E5;
const ZIL_RAW_CODE ALT_A_E					= 0x08E6;
const ZIL_RAW_CODE ALT_C_CEDILLA			= 0x08E7;
const ZIL_RAW_CODE ALT_E_GRAVE				= 0x08E8;
const ZIL_RAW_CODE ALT_E_ACUTE				= 0x08E9;
const ZIL_RAW_CODE ALT_E_CIRCUMFLEX			= 0x08EA;
const ZIL_RAW_CODE ALT_E_DIARESIS			= 0x08EB;
const ZIL_RAW_CODE ALT_I_GRAVE				= 0x08EC;
const ZIL_RAW_CODE ALT_I_ACUTE				= 0x08ED;
const ZIL_RAW_CODE ALT_I_CIRCUMFLEX			= 0x08EE;
const ZIL_RAW_CODE ALT_I_DIARESIS			= 0x08EF;
const ZIL_RAW_CODE ALT_ETH					= 0x08F0;
const ZIL_RAW_CODE ALT_N_TILDE				= 0x08F1;
const ZIL_RAW_CODE ALT_O_GRAVE				= 0x08F2;
const ZIL_RAW_CODE ALT_O_ACUTE				= 0x08F3;
const ZIL_RAW_CODE ALT_O_CIRCUMFLEX			= 0x08F4;
const ZIL_RAW_CODE ALT_O_TILDE				= 0x08F5;
const ZIL_RAW_CODE ALT_O_DIARESIS			= 0x08F6;
const ZIL_RAW_CODE ALT_O_SLASH				= 0x08F8;
const ZIL_RAW_CODE ALT_U_GRAVE				= 0x08F9;
const ZIL_RAW_CODE ALT_U_ACUTE				= 0x08FA;
const ZIL_RAW_CODE ALT_U_CIRCUMFLEX			= 0x08FB;
const ZIL_RAW_CODE ALT_U_DIARESIS			= 0x08FC;
const ZIL_RAW_CODE ALT_Y_ACUTE				= 0x08FD;
const ZIL_RAW_CODE ALT_THORN				= 0x08FE;

const ZIL_RAW_CODE ALT_1					= 0x0831;
const ZIL_RAW_CODE ALT_2					= 0x0832;
const ZIL_RAW_CODE ALT_3					= 0x0833;
const ZIL_RAW_CODE ALT_4					= 0x0834;
const ZIL_RAW_CODE ALT_5					= 0x0835;
const ZIL_RAW_CODE ALT_6					= 0x0836;
const ZIL_RAW_CODE ALT_7					= 0x0837;
const ZIL_RAW_CODE ALT_8					= 0x0838;
const ZIL_RAW_CODE ALT_9					= 0x0839;
const ZIL_RAW_CODE ALT_0					= 0x0840;

const ZIL_RAW_CODE GRAY_ENTER				= 0x000D;	// Grey keys
const ZIL_RAW_CODE GRAY_UP_ARROW			= 0x0026;
const ZIL_RAW_CODE GRAY_DOWN_ARROW			= 0x0028;
const ZIL_RAW_CODE GRAY_LEFT_ARROW			= 0x0025;
const ZIL_RAW_CODE GRAY_RIGHT_ARROW			= 0x0027;
const ZIL_RAW_CODE GRAY_INSERT				= 0x802D;
const ZIL_RAW_CODE GRAY_DELETE				= 0x002E;
const ZIL_RAW_CODE GRAY_HOME				= 0x0024;
const ZIL_RAW_CODE GRAY_END					= 0x0023;
const ZIL_RAW_CODE GRAY_PGUP				= 0x0021;
const ZIL_RAW_CODE GRAY_PGDN				= 0x0022;
const ZIL_RAW_CODE GRAY_DIVIDE				= 0x006F;
const ZIL_RAW_CODE GRAY_MULTIPLY			= 0x006A;
const ZIL_RAW_CODE GRAY_PLUS				= 0x006B;
const ZIL_RAW_CODE GRAY_MINUS				= 0x006D;

const ZIL_RAW_CODE CTRL_GRAY_UP_ARROW		= 0x0426;
const ZIL_RAW_CODE CTRL_GRAY_DOWN_ARROW		= 0x0428;
const ZIL_RAW_CODE CTRL_GRAY_LEFT_ARROW		= 0x0425;
const ZIL_RAW_CODE CTRL_GRAY_RIGHT_ARROW	= 0x0427;
const ZIL_RAW_CODE CTRL_GRAY_INSERT			= 0x842D;
const ZIL_RAW_CODE CTRL_GRAY_DELETE			= 0x042E;
const ZIL_RAW_CODE CTRL_GRAY_HOME			= 0x0424;
const ZIL_RAW_CODE CTRL_GRAY_END			= 0x0423;
const ZIL_RAW_CODE CTRL_GRAY_PGUP			= 0x0421;
const ZIL_RAW_CODE CTRL_GRAY_PGDN			= 0x0422;
const ZIL_RAW_CODE CTRL_GRAY_DIVIDE			= 0x046F;
const ZIL_RAW_CODE CTRL_GRAY_MULTIPLY		= 0x046A;
const ZIL_RAW_CODE CTRL_GRAY_PLUS			= 0x046B;
const ZIL_RAW_CODE CTRL_GRAY_MINUS			= 0x046D;

const ZIL_RAW_CODE ALT_GRAY_UP_ARROW		= 0x0826;
const ZIL_RAW_CODE ALT_GRAY_DOWN_ARROW		= 0x0828;
const ZIL_RAW_CODE ALT_GRAY_LEFT_ARROW		= 0x0825;
const ZIL_RAW_CODE ALT_GRAY_RIGHT_ARROW		= 0x0827;
const ZIL_RAW_CODE ALT_GRAY_INSERT			= 0x882D;
const ZIL_RAW_CODE ALT_GRAY_DELETE			= 0x082E;
const ZIL_RAW_CODE ALT_GRAY_HOME			= 0x0824;
const ZIL_RAW_CODE ALT_GRAY_END				= 0x0823;
const ZIL_RAW_CODE ALT_GRAY_PGUP			= 0x0821;
const ZIL_RAW_CODE ALT_GRAY_PGDN			= 0x0822;
const ZIL_RAW_CODE ALT_GRAY_DIVIDE			= 0x086F;
const ZIL_RAW_CODE ALT_GRAY_MULTIPLY		= 0x086A;
const ZIL_RAW_CODE ALT_GRAY_PLUS			= 0x086B;
const ZIL_RAW_CODE ALT_GRAY_MINUS			= 0x086D;

const ZIL_RAW_CODE WHITE_UP_ARROW			= 0x0026;	// White keys
const ZIL_RAW_CODE WHITE_DOWN_ARROW			= 0x0028;
const ZIL_RAW_CODE WHITE_LEFT_ARROW			= 0x0025;
const ZIL_RAW_CODE WHITE_RIGHT_ARROW		= 0x0027;
const ZIL_RAW_CODE WHITE_INSERT				= 0x802D;
const ZIL_RAW_CODE WHITE_DELETE				= 0x002E;
const ZIL_RAW_CODE WHITE_HOME				= 0x0024;
const ZIL_RAW_CODE WHITE_END				= 0x0023;
const ZIL_RAW_CODE WHITE_PGUP				= 0x0021;
const ZIL_RAW_CODE WHITE_PGDN				= 0x0022;
const ZIL_RAW_CODE WHITE_CENTER				= 0x002A;

const ZIL_RAW_CODE CTRL_WHITE_UP_ARROW		= 0x0426;
const ZIL_RAW_CODE CTRL_WHITE_DOWN_ARROW	= 0x0428;
const ZIL_RAW_CODE CTRL_WHITE_LEFT_ARROW	= 0x0425;
const ZIL_RAW_CODE CTRL_WHITE_RIGHT_ARROW	= 0x0427;
const ZIL_RAW_CODE CTRL_WHITE_INSERT		= 0x842D;
const ZIL_RAW_CODE CTRL_WHITE_DELETE		= 0x042E;
const ZIL_RAW_CODE CTRL_WHITE_HOME			= 0x0424;
const ZIL_RAW_CODE CTRL_WHITE_END			= 0x0423;
const ZIL_RAW_CODE CTRL_WHITE_PGUP			= 0x0421;
const ZIL_RAW_CODE CTRL_WHITE_PGDN			= 0x0422;
const ZIL_RAW_CODE CTRL_WHITE_CENTER		= 0x042A;

const ZIL_RAW_CODE F1						= 0x0070;	// Function keys
const ZIL_RAW_CODE F2						= 0x0071;
const ZIL_RAW_CODE F3						= 0x0072;
const ZIL_RAW_CODE F4						= 0x0073;
const ZIL_RAW_CODE F5						= 0x0074;
const ZIL_RAW_CODE F6						= 0x0075;
const ZIL_RAW_CODE F7						= 0x0076;
const ZIL_RAW_CODE F8						= 0x0077;
const ZIL_RAW_CODE F9						= 0x0078;
const ZIL_RAW_CODE F10						= 0x0079;
const ZIL_RAW_CODE F11						= 0x007A;
const ZIL_RAW_CODE F12						= 0x007B;

const ZIL_RAW_CODE SHIFT_F1					= 0x0370;
const ZIL_RAW_CODE SHIFT_F2					= 0x0371;
const ZIL_RAW_CODE SHIFT_F3					= 0x0372;
const ZIL_RAW_CODE SHIFT_F4					= 0x0373;
const ZIL_RAW_CODE SHIFT_F5					= 0x0374;
const ZIL_RAW_CODE SHIFT_F6					= 0x0375;
const ZIL_RAW_CODE SHIFT_F7					= 0x0376;
const ZIL_RAW_CODE SHIFT_F8					= 0x0377;
const ZIL_RAW_CODE SHIFT_F9					= 0x0378;
const ZIL_RAW_CODE SHIFT_F10				= 0x0379;
const ZIL_RAW_CODE SHIFT_F11				= 0x037A;
const ZIL_RAW_CODE SHIFT_F12				= 0x037B;

const ZIL_RAW_CODE CTRL_F1					= 0x0470;
const ZIL_RAW_CODE CTRL_F2					= 0x0471;
const ZIL_RAW_CODE CTRL_F3					= 0x0472;
const ZIL_RAW_CODE CTRL_F4					= 0x0473;
const ZIL_RAW_CODE CTRL_F5					= 0x0474;
const ZIL_RAW_CODE CTRL_F6					= 0x0475;
const ZIL_RAW_CODE CTRL_F7					= 0x0476;
const ZIL_RAW_CODE CTRL_F8					= 0x0477;
const ZIL_RAW_CODE CTRL_F9					= 0x0478;
const ZIL_RAW_CODE CTRL_F10					= 0x0479;
const ZIL_RAW_CODE CTRL_F11					= 0x047A;
const ZIL_RAW_CODE CTRL_F12					= 0x047B;

const ZIL_RAW_CODE ALT_F1					= 0x0870;
const ZIL_RAW_CODE ALT_F2					= 0x0871;
const ZIL_RAW_CODE ALT_F3					= 0x0872;
const ZIL_RAW_CODE ALT_F4					= 0x0873;
const ZIL_RAW_CODE ALT_F5					= 0x0874;
const ZIL_RAW_CODE ALT_F6					= 0x0875;
const ZIL_RAW_CODE ALT_F7					= 0x0876;
const ZIL_RAW_CODE ALT_F8					= 0x0877;
const ZIL_RAW_CODE ALT_F9					= 0x0878;
const ZIL_RAW_CODE ALT_F10					= 0x0879;
const ZIL_RAW_CODE ALT_F11					= 0x087A;
const ZIL_RAW_CODE ALT_F12					= 0x087B;
#endif

// --------------------------------------------------------------------------
// ----- ZIL_OS2 ------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_OS2) && defined(USE_RAW_KEYS)
const ZIL_RAW_CODE ESCAPE					= 0x01000000;
const ZIL_RAW_CODE ENTER					= 0x1C000000;
const ZIL_RAW_CODE SPACE					= 0x39000000;
const ZIL_RAW_CODE BACKSPACE	   			= 0x0E000000;
const ZIL_RAW_CODE CTRL_BACKSPACE			= 0x0E000004;
const ZIL_RAW_CODE ALT_BACKSPACE			= 0x0E000008;
const ZIL_RAW_CODE TAB						= 0x0F000000;
const ZIL_RAW_CODE CTRL_TAB					= 0x0F000004;
const ZIL_RAW_CODE BACKTAB					= 0x0F000003;

const ZIL_RAW_CODE CTRL_BREAK				= 0x5F000004;
const ZIL_RAW_CODE CTRL_C					= 0x2E000004;

const ZIL_RAW_CODE ALT_ESCAPE				= 0x01000008;
const ZIL_RAW_CODE ALT_PERIOD				= 0x34000008;
const ZIL_RAW_CODE ALT_SPACE	   			= 0x39000008;
const ZIL_RAW_CODE ALT_WHITE_MINUS			= 0x0C000008;
const ZIL_RAW_CODE ALT_WHITE_PLUS			= 0x0D00000B;

const ZIL_RAW_CODE ALT_A					= 0x1E000008;
const ZIL_RAW_CODE ALT_B					= 0x30000008;
const ZIL_RAW_CODE ALT_C					= 0x2E000008;
const ZIL_RAW_CODE ALT_D					= 0x20000008;
const ZIL_RAW_CODE ALT_E					= 0x12000008;
const ZIL_RAW_CODE ALT_F					= 0x21000008;
const ZIL_RAW_CODE ALT_G					= 0x22000008;
const ZIL_RAW_CODE ALT_H					= 0x23000008;
const ZIL_RAW_CODE ALT_I					= 0x17000008;
const ZIL_RAW_CODE ALT_J					= 0x24000008;
const ZIL_RAW_CODE ALT_K					= 0x25000008;
const ZIL_RAW_CODE ALT_L					= 0x26000008;
const ZIL_RAW_CODE ALT_M					= 0x32000008;
const ZIL_RAW_CODE ALT_N					= 0x31000008;
const ZIL_RAW_CODE ALT_O					= 0x18000008;
const ZIL_RAW_CODE ALT_P					= 0x19000008;
const ZIL_RAW_CODE ALT_Q					= 0x10000008;
const ZIL_RAW_CODE ALT_R					= 0x13000008;
const ZIL_RAW_CODE ALT_S					= 0x1F000008;
const ZIL_RAW_CODE ALT_T					= 0x14000008;
const ZIL_RAW_CODE ALT_U					= 0x16000008;
const ZIL_RAW_CODE ALT_V					= 0x2F000008;
const ZIL_RAW_CODE ALT_W					= 0x11000008;
const ZIL_RAW_CODE ALT_X					= 0x2D000008;
const ZIL_RAW_CODE ALT_Y					= 0x15000008;
const ZIL_RAW_CODE ALT_Z					= 0x2C000008;

const ZIL_RAW_CODE ALT_1					= 0x02000008;
const ZIL_RAW_CODE ALT_2					= 0x03000008;
const ZIL_RAW_CODE ALT_3					= 0x04000008;
const ZIL_RAW_CODE ALT_4					= 0x05000008;
const ZIL_RAW_CODE ALT_5					= 0x06000008;
const ZIL_RAW_CODE ALT_6					= 0x07000008;
const ZIL_RAW_CODE ALT_7					= 0x08000008;
const ZIL_RAW_CODE ALT_8					= 0x09000008;
const ZIL_RAW_CODE ALT_9					= 0x0A000008;
const ZIL_RAW_CODE ALT_0					= 0x0B000008;

const ZIL_RAW_CODE GRAY_ENTER				= 0x5A000000;
const ZIL_RAW_CODE GRAY_UP_ARROW			= 0x61000000;
const ZIL_RAW_CODE GRAY_DOWN_ARROW			= 0x66000000;
const ZIL_RAW_CODE GRAY_LEFT_ARROW			= 0x63000000;
const ZIL_RAW_CODE GRAY_RIGHT_ARROW			= 0x64000000;
const ZIL_RAW_CODE GRAY_INSERT				= 0x68000000;
const ZIL_RAW_CODE GRAY_DELETE				= 0x69000000;
const ZIL_RAW_CODE GRAY_HOME	    	  	= 0x60000000;
const ZIL_RAW_CODE GRAY_END					= 0x65000000;
const ZIL_RAW_CODE GRAY_PGUP	      		= 0x62000000;
const ZIL_RAW_CODE GRAY_PGDN	      		= 0x67000000;
const ZIL_RAW_CODE GRAY_DIVIDE				= 0x5C000000;
const ZIL_RAW_CODE GRAY_MULTIPLY			= 0x37000000;
const ZIL_RAW_CODE GRAY_PLUS	      		= 0x4E000000;
const ZIL_RAW_CODE GRAY_MINUS				= 0x4A000000;
											
const ZIL_RAW_CODE CTRL_GRAY_UP_ARROW		= 0x61000004;
const ZIL_RAW_CODE CTRL_GRAY_DOWN_ARROW		= 0x66000004;
const ZIL_RAW_CODE CTRL_GRAY_LEFT_ARROW		= 0x63000004;
const ZIL_RAW_CODE CTRL_GRAY_RIGHT_ARROW	= 0x64000004;
const ZIL_RAW_CODE CTRL_GRAY_INSERT			= 0x68000004;
const ZIL_RAW_CODE CTRL_GRAY_DELETE			= 0x69000004;
const ZIL_RAW_CODE CTRL_GRAY_HOME			= 0x60000004;
const ZIL_RAW_CODE CTRL_GRAY_END			= 0x65000004;
const ZIL_RAW_CODE CTRL_GRAY_PGUP			= 0x62000004;
const ZIL_RAW_CODE CTRL_GRAY_PGDN			= 0x67000004;
const ZIL_RAW_CODE CTRL_GRAY_DIVIDE			= 0x5C000004;
const ZIL_RAW_CODE CTRL_GRAY_MULTIPLY		= 0x37000004;
const ZIL_RAW_CODE CTRL_GRAY_PLUS			= 0x4E000004;
const ZIL_RAW_CODE CTRL_GRAY_MINUS			= 0x4A000004;

const ZIL_RAW_CODE ALT_GRAY_UP_ARROW      	= 0x61000008;
const ZIL_RAW_CODE ALT_GRAY_DOWN_ARROW		= 0x66000008;
const ZIL_RAW_CODE ALT_GRAY_LEFT_ARROW		= 0x63000008;
const ZIL_RAW_CODE ALT_GRAY_RIGHT_ARROW		= 0x64000008;
const ZIL_RAW_CODE ALT_GRAY_INSERT			= 0x68000008;
const ZIL_RAW_CODE ALT_GRAY_DELETE			= 0x69000008;
const ZIL_RAW_CODE ALT_GRAY_HOME			= 0x60000008;
const ZIL_RAW_CODE ALT_GRAY_END				= 0x65000008;
const ZIL_RAW_CODE ALT_GRAY_PGUP			= 0x62000008;
const ZIL_RAW_CODE ALT_GRAY_PGDN			= 0x67000008;
const ZIL_RAW_CODE ALT_GRAY_DIVIDE			= 0x5C000008;
const ZIL_RAW_CODE ALT_GRAY_MULTIPLY      	= 0x37000008;
const ZIL_RAW_CODE ALT_GRAY_PLUS			= 0x4E000008;
const ZIL_RAW_CODE ALT_GRAY_MINUS			= 0x4A000008;

const ZIL_RAW_CODE WHITE_UP_ARROW			= 0x48000000;
const ZIL_RAW_CODE WHITE_DOWN_ARROW			= 0x50000000;
const ZIL_RAW_CODE WHITE_LEFT_ARROW			= 0x4B000000;
const ZIL_RAW_CODE WHITE_RIGHT_ARROW      	= 0x4D000000;
const ZIL_RAW_CODE WHITE_INSERT				= 0x52000000;
const ZIL_RAW_CODE WHITE_DELETE				= 0x53000000;
const ZIL_RAW_CODE WHITE_HOME				= 0x47000000;
const ZIL_RAW_CODE WHITE_END	      		= 0x4F000000;
const ZIL_RAW_CODE WHITE_PGUP				= 0x49000000;
const ZIL_RAW_CODE WHITE_PGDN				= 0x51000000;
const ZIL_RAW_CODE WHITE_CENTER				= 0x4C000000;

const ZIL_RAW_CODE CTRL_WHITE_UP_ARROW		= 0x48000004;
const ZIL_RAW_CODE CTRL_WHITE_DOWN_ARROW	= 0x50000004;
const ZIL_RAW_CODE CTRL_WHITE_LEFT_ARROW	= 0x4B000004;
const ZIL_RAW_CODE CTRL_WHITE_RIGHT_ARROW	= 0x4D000004;
const ZIL_RAW_CODE CTRL_WHITE_INSERT		= 0x52000004;
const ZIL_RAW_CODE CTRL_WHITE_DELETE		= 0x53000004;
const ZIL_RAW_CODE CTRL_WHITE_HOME			= 0x47000004;
const ZIL_RAW_CODE CTRL_WHITE_END			= 0x4F000004;
const ZIL_RAW_CODE CTRL_WHITE_PGUP			= 0x49000004;
const ZIL_RAW_CODE CTRL_WHITE_PGDN			= 0x51000004;
const ZIL_RAW_CODE CTRL_WHITE_CENTER		= 0x4C000004;

const ZIL_RAW_CODE F1						= 0x3B000000;
const ZIL_RAW_CODE F2						= 0x3C000000;
const ZIL_RAW_CODE F3						= 0x3D000000;
const ZIL_RAW_CODE F4						= 0x3E000000;
const ZIL_RAW_CODE F5						= 0x3F000000;
const ZIL_RAW_CODE F6						= 0x40000000;
const ZIL_RAW_CODE F7						= 0x41000000;
const ZIL_RAW_CODE F8						= 0x42000000;
const ZIL_RAW_CODE F9						= 0x43000000;
const ZIL_RAW_CODE F10						= 0x44000000;
const ZIL_RAW_CODE F11						= 0x57000000;
const ZIL_RAW_CODE F12						= 0x58000000;

const ZIL_RAW_CODE SHIFT_F1					= 0x3B000003;
const ZIL_RAW_CODE SHIFT_F2					= 0x3C000003;
const ZIL_RAW_CODE SHIFT_F3					= 0x3D000003;
const ZIL_RAW_CODE SHIFT_F4					= 0x3E000003;
const ZIL_RAW_CODE SHIFT_F5					= 0x3F000003;
const ZIL_RAW_CODE SHIFT_F6					= 0x40000003;
const ZIL_RAW_CODE SHIFT_F7					= 0x41000003;
const ZIL_RAW_CODE SHIFT_F8					= 0x42000003;
const ZIL_RAW_CODE SHIFT_F9					= 0x43000003;
const ZIL_RAW_CODE SHIFT_F10				= 0x44000003;
const ZIL_RAW_CODE SHIFT_F11				= 0x57000003;
const ZIL_RAW_CODE SHIFT_F12				= 0x58000003;

const ZIL_RAW_CODE CTRL_F1					= 0x3B000004;
const ZIL_RAW_CODE CTRL_F2					= 0x3C000004;
const ZIL_RAW_CODE CTRL_F3					= 0x3D000004;
const ZIL_RAW_CODE CTRL_F4					= 0x3E000004;
const ZIL_RAW_CODE CTRL_F5					= 0x3F000004;
const ZIL_RAW_CODE CTRL_F6					= 0x40000004;
const ZIL_RAW_CODE CTRL_F7					= 0x41000004;
const ZIL_RAW_CODE CTRL_F8					= 0x42000004;
const ZIL_RAW_CODE CTRL_F9					= 0x43000004;
const ZIL_RAW_CODE CTRL_F10					= 0x44000004;
const ZIL_RAW_CODE CTRL_F11					= 0x57000004;
const ZIL_RAW_CODE CTRL_F12					= 0x58000004;

const ZIL_RAW_CODE ALT_F1					= 0x3B000008;
const ZIL_RAW_CODE ALT_F2					= 0x3C000008;
const ZIL_RAW_CODE ALT_F3					= 0x3D000008;
const ZIL_RAW_CODE ALT_F4					= 0x3E000008;
const ZIL_RAW_CODE ALT_F5					= 0x3F000008;
const ZIL_RAW_CODE ALT_F6					= 0x40000008;
const ZIL_RAW_CODE ALT_F7					= 0x41000008;
const ZIL_RAW_CODE ALT_F8					= 0x42000008;
const ZIL_RAW_CODE ALT_F9					= 0x43000008;
const ZIL_RAW_CODE ALT_F10					= 0x44000008;
const ZIL_RAW_CODE ALT_F11					= 0x57000008;
const ZIL_RAW_CODE ALT_F12					= 0x58000008;
#endif

// --------------------------------------------------------------------------
// ----- ZIL_X11 ------------------------------------------------------------
// --------------------------------------------------------------------------

#if defined (ZIL_X11) && defined(USE_RAW_KEYS)
#include <X11/keysym.h>
const ZIL_RAW_CODE ESCAPE					= XK_Escape;	// Miscellaneous keys
const ZIL_RAW_CODE ENTER					= XK_Return;
const ZIL_RAW_CODE BACKSPACE				= XK_BackSpace;
const ZIL_RAW_CODE TAB						= XK_Tab;

const ZIL_RAW_CODE GRAY_ENTER				= XK_KP_Enter;	// Grey keys
const ZIL_RAW_CODE GRAY_UP_ARROW			= XK_Up;
const ZIL_RAW_CODE GRAY_DOWN_ARROW			= XK_Down;
const ZIL_RAW_CODE GRAY_LEFT_ARROW			= XK_Left;
const ZIL_RAW_CODE GRAY_RIGHT_ARROW			= XK_Right;
const ZIL_RAW_CODE GRAY_DIVIDE				= XK_KP_Divide;
const ZIL_RAW_CODE GRAY_MULTIPLY			= XK_KP_Multiply;
const ZIL_RAW_CODE GRAY_PLUS				= XK_KP_Add;
const ZIL_RAW_CODE GRAY_MINUS				= XK_KP_Subtract;

const ZIL_RAW_CODE WHITE_UP_ARROW			= XK_Up;	// White keys
const ZIL_RAW_CODE WHITE_DOWN_ARROW			= XK_Down;
const ZIL_RAW_CODE WHITE_LEFT_ARROW			= XK_Left;
const ZIL_RAW_CODE WHITE_RIGHT_ARROW		= XK_Right;

const ZIL_RAW_CODE F1						= XK_F1;	// Function keys
const ZIL_RAW_CODE F2						= XK_F2;
const ZIL_RAW_CODE F3						= XK_F3;
const ZIL_RAW_CODE F4						= XK_F4;
const ZIL_RAW_CODE F5						= XK_F5;
const ZIL_RAW_CODE F6						= XK_F6;
const ZIL_RAW_CODE F7						= XK_F7;
const ZIL_RAW_CODE F8						= XK_F8;
const ZIL_RAW_CODE F9						= XK_F9;
const ZIL_RAW_CODE F10						= XK_F10;
const ZIL_RAW_CODE F11						= XK_F11;
const ZIL_RAW_CODE F12						= XK_F12;
#endif

// --------------------------------------------------------------------------
// ----- ZIL_CURSES ---------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_CURSES) && defined(USE_RAW_KEYS)
const ZIL_RAW_CODE ESCAPE					= 0x1B;	
const ZIL_RAW_CODE ENTER                    = 0x0D;     
const ZIL_RAW_CODE SPACE					= 0x20;
const ZIL_RAW_CODE TAB						= 0x09;

const ZIL_RAW_CODE BACKSPACE				= KEY_BACKSPACE;
//const ZIL_RAW_CODE CTRL_BACKSPACE		    = 0x0E7F;
//const ZIL_RAW_CODE CTRL_TAB				= 0x9400;
const ZIL_RAW_CODE BACKTAB					= KEY_BTAB;

const ZIL_RAW_CODE CTRL_BREAK				= KEY_BREAK;

const ZIL_RAW_CODE UP_ARROW				    = KEY_UP;
const ZIL_RAW_CODE DOWN_ARROW				= KEY_DOWN;
const ZIL_RAW_CODE LEFT_ARROW				= KEY_LEFT;
const ZIL_RAW_CODE RIGHT_ARROW				= KEY_RIGHT;
const ZIL_RAW_CODE INSERT					= KEY_IC;
const ZIL_RAW_CODE DELETE					= KEY_DC;
const ZIL_RAW_CODE HOME						= KEY_HOME;
#if defined(SCO_UNIX)
const ZIL_RAW_CODE END						= KEY_END;
#endif
const ZIL_RAW_CODE PGUP						= KEY_PPAGE;
const ZIL_RAW_CODE PGDN						= KEY_NPAGE;
const ZIL_RAW_CODE DIVIDE					= '/';
const ZIL_RAW_CODE MULTIPLY				    = '*';
const ZIL_RAW_CODE PLUS						= '+';
const ZIL_RAW_CODE MINUS					= '-';

const ZIL_RAW_CODE WHITE_END				= KEY_C1;
const ZIL_RAW_CODE WHITE_PGUP				= KEY_A3;
const ZIL_RAW_CODE WHITE_PGDN				= KEY_C3;
const ZIL_RAW_CODE WHITE_CENTER			    = KEY_B2;

const ZIL_RAW_CODE F1						= KEY_F(1);	// Function keys
const ZIL_RAW_CODE F2						= KEY_F(2);
const ZIL_RAW_CODE F3						= KEY_F(3);
const ZIL_RAW_CODE F4						= KEY_F(4);
const ZIL_RAW_CODE F5						= KEY_F(5);
const ZIL_RAW_CODE F6						= KEY_F(6);
const ZIL_RAW_CODE F7						= KEY_F(7);
const ZIL_RAW_CODE F8						= KEY_F(8);
const ZIL_RAW_CODE F9						= KEY_F(9);
const ZIL_RAW_CODE F10						= KEY_F(10);
const ZIL_RAW_CODE F11						= KEY_F(11);
const ZIL_RAW_CODE F12						= KEY_F(12);

#if defined(ZIL_PC_KEYBRD)
const ZIL_RAW_CODE ALT_KEY					= '`';
const ZIL_RAW_CODE CTRL_KEY				    = '~';
#else
const ZIL_RAW_CODE ALT_KEY					= 0x1B;
#endif

const ZIL_RAW_CODE CTRL_A					= 0x01;	// down-action scan codes
const ZIL_RAW_CODE CTRL_B					= 0x02;
const ZIL_RAW_CODE CTRL_C					= 0x03;
const ZIL_RAW_CODE CTRL_D					= 0x04;
const ZIL_RAW_CODE CTRL_E					= 0x05;
const ZIL_RAW_CODE CTRL_F					= 0x06;
const ZIL_RAW_CODE CTRL_G					= 0x07;
const ZIL_RAW_CODE CTRL_H					= 0x08;
const ZIL_RAW_CODE CTRL_I					= 0x09;
const ZIL_RAW_CODE CTRL_J					= 0x0A;
const ZIL_RAW_CODE CTRL_K					= 0x0B;
const ZIL_RAW_CODE CTRL_L					= 0x0C;
const ZIL_RAW_CODE CTRL_M					= 0x0D;
const ZIL_RAW_CODE CTRL_N					= 0x0E;
const ZIL_RAW_CODE CTRL_O					= 0x0F;
const ZIL_RAW_CODE CTRL_P					= 0x10;
const ZIL_RAW_CODE CTRL_Q					= 0x11;
const ZIL_RAW_CODE CTRL_R					= 0x12;
const ZIL_RAW_CODE CTRL_S					= 0x13;
const ZIL_RAW_CODE CTRL_T					= 0x14;
const ZIL_RAW_CODE CTRL_U					= 0x15;
const ZIL_RAW_CODE CTRL_V					= 0x16;
const ZIL_RAW_CODE CTRL_W					= 0x17;
const ZIL_RAW_CODE CTRL_X					= 0x18;
const ZIL_RAW_CODE CTRL_Y					= 0x19;
const ZIL_RAW_CODE CTRL_Z					= 0x1A;
#endif

// --------------------------------------------------------------------------
// ----- ZIL_MACINTOSH ------------------------------------------------------
// --------------------------------------------------------------------------

// These are always declared for Metrowerks and Symantec to facilitate
// a single pre-compiled header.
#if defined(ZIL_MACINTOSH) && (defined(USE_RAW_KEYS) || defined(__MWERKS__) || defined(__ZTC__))
const ZIL_RAW_CODE ESCAPE					= 0x3500;			// Miscellaneous keys
const ZIL_RAW_CODE ENTER					= 0x2400;
const ZIL_RAW_CODE SPACE					= 0x3100;
const ZIL_RAW_CODE BACKSPACE				= 0x3300;
const ZIL_RAW_CODE TAB						= 0x3000;

const ZIL_RAW_CODE GRAY_ENTER				= 0x4C00;			// Grey keys
const ZIL_RAW_CODE GRAY_UP_ARROW			= 0x7E00;
const ZIL_RAW_CODE GRAY_DOWN_ARROW			= 0x7D00;
const ZIL_RAW_CODE GRAY_LEFT_ARROW			= 0x7B00;
const ZIL_RAW_CODE GRAY_RIGHT_ARROW			= 0x7C00;
const ZIL_RAW_CODE GRAY_INSERT				= 0x7200;
const ZIL_RAW_CODE GRAY_DELETE				= 0x7500;
const ZIL_RAW_CODE GRAY_HOME				= 0x7300;
const ZIL_RAW_CODE GRAY_END					= 0x7700;
const ZIL_RAW_CODE GRAY_PGUP				= 0x7400;
const ZIL_RAW_CODE GRAY_PGDN				= 0x7900;
const ZIL_RAW_CODE GRAY_DIVIDE				= 0x4B00;
const ZIL_RAW_CODE GRAY_MULTIPLY			= 0x4300;
const ZIL_RAW_CODE GRAY_PLUS				= 0x4500;
const ZIL_RAW_CODE GRAY_MINUS				= 0x4E00;

const ZIL_RAW_CODE WHITE_UP_ARROW			= GRAY_UP_ARROW;	// White keys
const ZIL_RAW_CODE WHITE_DOWN_ARROW			= GRAY_DOWN_ARROW;
const ZIL_RAW_CODE WHITE_LEFT_ARROW			= GRAY_LEFT_ARROW;
const ZIL_RAW_CODE WHITE_RIGHT_ARROW		= GRAY_RIGHT_ARROW;
const ZIL_RAW_CODE WHITE_INSERT				= GRAY_INSERT;
const ZIL_RAW_CODE WHITE_DELETE				= GRAY_DELETE;
const ZIL_RAW_CODE WHITE_HOME				= GRAY_HOME;
const ZIL_RAW_CODE WHITE_END				= GRAY_END;
const ZIL_RAW_CODE WHITE_PGUP				= GRAY_PGUP;
const ZIL_RAW_CODE WHITE_PGDN				= GRAY_PGDN;
const ZIL_RAW_CODE WHITE_QUESTION			= 0x2C00;
const ZIL_RAW_CODE WHITE_X					= 0x0700;
const ZIL_RAW_CODE WHITE_C					= 0x0800;
const ZIL_RAW_CODE WHITE_V					= 0x0900;
const ZIL_RAW_CODE WHITE_Q					= 0x0C00;
const ZIL_RAW_CODE WHITE_W					= 0x0D00;

const ZIL_RAW_CODE F1						= 0x7A00;			// Function keys
const ZIL_RAW_CODE F2						= 0x7800;
const ZIL_RAW_CODE F3						= 0x6300;
const ZIL_RAW_CODE F4						= 0x7600;
const ZIL_RAW_CODE F5						= 0x6000;
const ZIL_RAW_CODE F6						= 0x6100;
const ZIL_RAW_CODE F7						= 0x6200;
const ZIL_RAW_CODE F8						= 0x6400;
const ZIL_RAW_CODE F9						= 0x6500;
const ZIL_RAW_CODE F10						= 0x6D00;
const ZIL_RAW_CODE F11						= 0x6700;
const ZIL_RAW_CODE F12						= 0x6F00;
const ZIL_RAW_CODE F13						= 0x6900;
const ZIL_RAW_CODE F14						= 0x6B00;
const ZIL_RAW_CODE F15						= 0x7100;
#endif

// --------------------------------------------------------------------------
// ----- ZIL_NEXTSTEP -------------------------------------------------------
// --------------------------------------------------------------------------

#if defined (ZIL_NEXTSTEP)
const ZIL_RAW_CODE ESCAPE				= 0x001b;	// Miscellaneous keys
const ZIL_RAW_CODE ENTER				= 0x000d;
const ZIL_RAW_CODE BACKSPACE				= 0x007f;
const ZIL_RAW_CODE TAB					= 0x0009;

const ZIL_RAW_CODE GRAY_ENTER				= 0x000d;	// Grey keys
const ZIL_RAW_CODE GRAY_UP_ARROW			= 0x00ad;
const ZIL_RAW_CODE GRAY_DOWN_ARROW			= 0x00af;
const ZIL_RAW_CODE GRAY_LEFT_ARROW			= 0x00ac;
const ZIL_RAW_CODE GRAY_RIGHT_ARROW			= 0x00ae;
const ZIL_RAW_CODE GRAY_DIVIDE				= 0x002f;
const ZIL_RAW_CODE GRAY_MULTIPLY			= 0x002a;
const ZIL_RAW_CODE GRAY_PLUS				= 0x002b;
const ZIL_RAW_CODE GRAY_MINUS				= 0x002d;

const ZIL_RAW_CODE WHITE_UP_ARROW			= 0x00ad;	// White keys
const ZIL_RAW_CODE WHITE_DOWN_ARROW			= 0x00af;
const ZIL_RAW_CODE WHITE_LEFT_ARROW			= 0x00ac;
const ZIL_RAW_CODE WHITE_RIGHT_ARROW			= 0x00ae;

// NEXTSTEP does not define function keys.
const ZIL_RAW_CODE F1						= 0;// Function keys
const ZIL_RAW_CODE F2						= 0;
const ZIL_RAW_CODE F3						= 0;
const ZIL_RAW_CODE F4						= 0;
const ZIL_RAW_CODE F5						= 0;
const ZIL_RAW_CODE F6						= 0;
const ZIL_RAW_CODE F7						= 0;
const ZIL_RAW_CODE F8						= 0;
const ZIL_RAW_CODE F9						= 0;
const ZIL_RAW_CODE F10						= 0;
const ZIL_RAW_CODE F11						= 0;
const ZIL_RAW_CODE F12						= 0;
#endif

#if defined(ZIL_OLD_DEFS)
// --- Version 3.6 compatibility ---
#define RAW_CODE				ZIL_RAW_CODE
#endif

#endif

