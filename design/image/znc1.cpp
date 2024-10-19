//	Zinc Application Framework - ZNC1.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::ImportZNC(ZIL_FILE *file)
{
	// Load the image.
	ZIL_INT16 _value; file->Read(&_value); bitmapWidth = _value;
	file->Read(&_value); bitmapHeight = _value;
	bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	file->Read(bitmapArray, bitmapWidth * bitmapHeight, sizeof(ZIL_UINT8));

	// Return success.
	return (file->GetError() ? SetError(ERROR_FILE) : ERROR_NONE);
}

