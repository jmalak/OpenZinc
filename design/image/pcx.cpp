//	Zinc Application Framework - PCX.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::LoadPCX(ZIL_FILE *file)
{
	return (ZNC_LOAD_IMAGE::LoadZNC(file));
}

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::StorePCX(ZIL_FILE *file)
{
	return (ZNC_STORE_IMAGE::StoreZNC(file));
}

