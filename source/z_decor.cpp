//	Zinc Interface Library - Z_DECOR.CPP
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


#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#	include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_gen.hpp"

static ZIL_ICHAR _pathDecoration[] =
{
	'D','E','C','O','R','A','T','I','O','N',
	0
};
ZIL_DECORATION_MANAGER decorationManager;
static ZIL_ICHAR _defaultName[12] = { 0 };
static ZIL_ICHAR _defaultOSName[12] = { 0 };

#define ZIL_DECORATION_DATA
#include "img_def.cpp"

// ----- ZIL_DECORATION ---------------------------------------------------

ZIL_DECORATION::ZIL_DECORATION(void) : ZIL_I18N()
{
	noOfTextElements = 0;
	text = ZIL_NULLP(ZIL_TEXT_ELEMENT);
	noOfBitmapElements = 0;
	bitmap = ZIL_NULLP(ZIL_BITMAP_ELEMENT);
	pathName = _pathDecoration;
}

ZIL_DECORATION::~ZIL_DECORATION(void)
{
	ZIL_DECORATION::DeleteData();
}

void ZIL_DECORATION::AssignData(const ZIL_I18N *_data)
{
	noOfTextElements = ((ZIL_DECORATION *)_data)->noOfTextElements;
	text = ((ZIL_DECORATION *)_data)->text;
	noOfBitmapElements = ((ZIL_DECORATION *)_data)->noOfBitmapElements;
	bitmap = ((ZIL_DECORATION *)_data)->bitmap;
}

void ZIL_DECORATION::DeleteData(void)
{
	if (allocated)
	{
		int i = 0;
		for (; i < noOfTextElements; i++)
			delete text[i].text;
		delete []text;
		for (i=0; i < noOfBitmapElements; i++)
			delete bitmap[i].bitmap;
		delete []bitmap;
	}
}

ZIL_ICHAR *ZIL_DECORATION::GetText(ZIL_NUMBERID numberID, int useDefault) const
{
	for (int i = 0; i < noOfTextElements; i++)
		if (text[i].numberID == numberID)
			return (text[i].text);
	return (useDefault ? text[0].text : ZIL_NULLP(ZIL_ICHAR));
}

ZIL_UINT8 *ZIL_DECORATION::GetBitmap(ZIL_NUMBERID numberID, int useDefault) const
{
	for (int i = 0; i < noOfBitmapElements; i++)
		if (bitmap[i].numberID == numberID)
			return (bitmap[i].bitmap);
	return (useDefault ? bitmap[0].bitmap : ZIL_NULLP(ZIL_UINT8));
}

//--- Language Setup ---------------------------------------------------------

ZIL_DECORATION_MANAGER::ZIL_DECORATION_MANAGER(void) : ZIL_I18N_MANAGER()
{
	initialized = TRUE;
	defaultName = _defaultName;
	defaultOSName = _defaultOSName;
}

void ZIL_DECORATION_MANAGER::SetDecorations(const ZIL_ICHAR *className,
		ZIL_TEXT_ELEMENT *defaultText,
		ZIL_BITMAP_ELEMENT *defaultBitmap)
{
	if (_defaultName[0] == 0)
	{
		ZIL_INTERNATIONAL::strcpy(_defaultName, isoImageName);
	}
	if (ZIL_INTERNATIONAL::streq(_defaultName, isoImageName) != 0)
	{
		UseDecorations(className, _defaultName);
		return;
	}

	ZIL_DECORATION *def = new ZIL_DECORATION;
	def->className = className;
	ZIL_INTERNATIONAL::strcpy(def->name, isoImageName);
	int i = 0;
	for (; defaultText[i].text; i++)
		;
	def->noOfTextElements = i;
	def->text = defaultText;
	for (i=0; defaultBitmap[i].bitmap; i++)
		;
	def->noOfBitmapElements = i;
	def->bitmap = defaultBitmap;
	def->allocated = FALSE;
	def->defaults = TRUE;
	def->useCount = 0;
	decorationManager.Add(def);
}

//----------------------------------------------------------------------------

#if defined(ZIL_LOAD)
void ZIL_DECORATION::ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *file)
{
	// Load in data from the current directory.
	ZIL_UINT16 count; file->Load(&count); noOfTextElements = count;
	text = new ZIL_TEXT_ELEMENT[noOfTextElements];
	int i = 0;
	for (; i < noOfTextElements && !file->objectError; i++)
	{
		file->Load(&text[i].numberID);
		file->Load(text[i].stringID, ZIL_STRINGID_LEN);
		file->Load(&text[i].text);
		if (!text[i].text)
			text[i].text = ZIL_INTERNATIONAL::strdup(ZIL_INTERNATIONAL::_blankString);
	}
	file->Load(&count); noOfBitmapElements = count;
	bitmap = new ZIL_BITMAP_ELEMENT[noOfBitmapElements];
	for (i = 0; i < noOfBitmapElements && !file->objectError; i++)
	{
		file->Load(&bitmap[i].numberID);
		file->Load(bitmap[i].stringID, ZIL_STRINGID_LEN);
		ZIL_UINT16 width, height;
		file->Load(&width);
		file->Load(&height);
		bitmap[i].bitmap = new ZIL_UINT8[width*height+2];
		bitmap[i].bitmap[0] = (ZIL_UINT8)width;
		bitmap[i].bitmap[1] = (ZIL_UINT8)height;
		file->Load(&bitmap[i].bitmap[2], sizeof(ZIL_UINT8), width*height);
	}
	error = file->objectError;
}
#endif

#if defined(ZIL_STORE)
void ZIL_DECORATION::ClassStoreData(ZIL_STORAGE_OBJECT *file)
{
	// Store data to the current directory.
	ZIL_UINT16 count = noOfTextElements; file->Store(count);
	int i = 0;
	for (; i < noOfTextElements && !file->objectError; i++)
	{
		file->Store(text[i].numberID);
		file->Store(text[i].stringID);
		file->Store(text[i].text);
	}
	count = noOfBitmapElements; file->Store(count);
	for (i = 0; i < noOfBitmapElements && !file->objectError; i++)
	{
		file->Store(bitmap[i].numberID);
		file->Store(bitmap[i].stringID);
		ZIL_UINT16 width = bitmap[i].bitmap[0],
			height = bitmap[i].bitmap[1];
		file->Store(width);
		file->Store(height);
		file->Store(&bitmap[i].bitmap[2], sizeof(ZIL_UINT8), width*height);
	}
	error = file->objectError;
}
#endif

