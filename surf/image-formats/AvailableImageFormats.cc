/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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


#include <AvailableImageFormats.h>

namespace ImageFormats {
	Format* availableFormats[] = {
		&imgFmt_ByExtension,
		&imgFmt_PPM,
		&imgFmt_PBM,
		&imgFmt_Postscript,
		&imgFmt_EPS,

#ifdef HAVE_LIBJPEG
		&imgFmt_JPEG,
#endif

#ifdef HAVE_LIBPNG
		&imgFmt_PNG,
#endif

#ifdef HAVE_LIBTIFF
		&imgFmt_TIFF,
#endif
		
                &imgFmt_XPM,
		&imgFmt_XBitmap,
		&imgFmt_Sun,

#ifdef HAVE_LIBGTS
		&imgFmt_GTS,
#endif

		0 // last element is 0
	};

	size_t numAvailableFormats = sizeof(availableFormats)/sizeof(Format) - 1;

	int image_formats_data[sizeof(availableFormats)/sizeof(Format) - 1];
}
