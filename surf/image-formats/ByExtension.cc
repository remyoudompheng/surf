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



#include "ByExtension.h"

#include "Misc.h"

namespace ImageFormats {

	ByExtension imgFmt_ByExtension;

	bool ByExtension::saveColorImage(const char* filename,
					 guint8* rdata, guint8* gdata, guint8* bdata,
					 int width, int height, bool fromDlg)
	{
		Format* fmt = guessFormat(filename, color);

		if (fmt == 0) {
			Misc::alert("Couldn't determine file type by extension.");
			return false;
		}
		if (fmt->getColorType() == dithered) {
			Misc::alert("You must choose a color image file format.");
			return false;
		}
		return fmt->saveColorImage(filename, rdata, gdata, bdata, width, height,fromDlg);
	}

	bool ByExtension::saveDitheredImage(const char* filename,
					    bit_buffer& pixel,
					    int paper_width, int paper_height, int resolution,
					    bool fromDlg)
	{
		Format* fmt = guessFormat(filename, dithered);

		if (fmt == 0) {
			Misc::alert("Couldn't determine file type by extension.");
			return false;
		}
		if (fmt->getColorType() == color) {
			Misc::alert("You must choose a dithered image file format.");
			return false;
		}
		return fmt->saveDitheredImage(filename, pixel, paper_width, paper_height, resolution, fromDlg);
	}

} // namespace ImageFormats
