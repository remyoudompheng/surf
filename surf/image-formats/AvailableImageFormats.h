/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */



#ifndef AVAILABLEIMAGEFMTS_H
#define AVAILABLEIMAGEFMTS_H

#include "ByExtension.h"
#include "JPEG.h"
#include "TIFF.h"
#include "Postscript.h"
#include "Sun.h"
#include "EPS.h"
#include "XWD.h"
#include "XBitmap.h"
#include "PPM.h"
#include "PBM.h"

#ifndef NO_GUI
#include "Pipe.h"
#endif

namespace ImageFormats {
	extern ByExtension imgFmt_ByExtension;
	extern JPEG imgFmt_JPEG;
	extern Tiff imgFmt_TIFF;
	extern Postscript imgFmt_Postscript;
	extern Sun imgFmt_Sun;
	extern EPS imgFmt_EPS;
	extern XWD imgFmt_XWD;
	extern XBitmap imgFmt_XBitmap;
	extern PPM imgFmt_PPM;
	extern PBM imgFmt_PBM;

#ifndef NO_GUI
	extern Pipe imgFmt_Pipe;
#endif

	Format* availableFormats[] = {
		&imgFmt_ByExtension,
		&imgFmt_PPM,
		&imgFmt_PBM,
		&imgFmt_Postscript,
		&imgFmt_EPS,

#ifdef HAVE_LIBJPEG
		&imgFmt_JPEG,
#endif

#ifdef HAVE_LIBTIFF
		&imgFmt_TIFF,
#endif
		
		&imgFmt_XBitmap,
		&imgFmt_XWD,
		&imgFmt_Sun,
#ifndef NO_GUI
		&imgFmt_Pipe,
#endif
		0 // last element is 0
	};

	int numAvailableFormats = sizeof(availableFormats) / sizeof(Format) - 1;

}

#endif //AVAILABLEIMAGEFMTS_H
