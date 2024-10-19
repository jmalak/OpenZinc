//	Zinc Application Framework - ZNC2.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ExportZNC(ZIL_FILE *file)
{
	// Store the image.
	ZIL_INT16 _value = (ZIL_INT16)bitmapWidth; file->Write(_value);
	_value = (ZIL_INT16)bitmapHeight; file->Write(_value);
	file->Write(bitmapArray, bitmapWidth * bitmapHeight, sizeof(ZIL_UINT8));

	// Return success.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

