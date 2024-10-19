//	Zinc Interface Library - Z_HELP.CPP
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


#include "ui_win.hpp"

// --------------------------------------------------------------------------
// ----- UI_HELP_STUB -------------------------------------------------------
// --------------------------------------------------------------------------

UI_HELP_STUB::UI_HELP_STUB(void)
{
	deleteHelpFile = FALSE;
	helpFile = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
}

UI_HELP_STUB::~UI_HELP_STUB(void)
{
	if (helpFile && deleteHelpFile)
		delete helpFile;
}

void UI_HELP_STUB::DisplayHelp(UI_WINDOW_MANAGER *, UI_HELP_CONTEXT)
{
}

void UI_HELP_STUB::ResetStorage(ZIL_STORAGE_READ_ONLY *_helpFile, int deleteNewStorage)
{
	// Reset the storage.
	if (_helpFile == helpFile)
		return;
	else if (deleteHelpFile && helpFile)
		delete helpFile;

	helpFile = _helpFile;
	deleteHelpFile = deleteNewStorage;
}
