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


#ifndef AVAILABLEIMAGEFMTS_H
#define AVAILABLEIMAGEFMTS_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <ByExtension.h>
#ifdef HAVE_LIBJPEG
#  include <JPEG.h>
#endif
#ifdef HAVE_LIBPNG
#  include <PNG.h>
#endif
#ifdef HAVE_LIBTIFF
#  include <TIFF.h>
#endif
#include <Postscript.h>
#include <Sun.h>
#include <EPS.h>
#include <XBitmap.h>
#include <PPM.h>
#include <PBM.h>
#include <XPM.h>
#ifdef HAVE_GTS
#  include <GTS.h>
#  include <OOGL.h>
#  include <Inventor.h>
#endif

namespace ImageFormats {
	extern Format* availableFormats[];
	extern size_t numAvailableFormats;
	
	extern int image_formats_data[];
}

#endif //AVAILABLEIMAGEFMTS_H
