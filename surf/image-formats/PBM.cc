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



#include "Misc.h"
#include "FileWriter.h"
#include "bit_buffer.h"

#include "PBM.h"

namespace ImageFormats {

	PBM imgFmt_PBM;
	
	
	bool PBM::saveDitheredImage(const char* filename,
				    bit_buffer& pixel,
				    int paper_width, int paper_height, int resolution,
				    bool fromDlg)
	{
		FileWriter fw(filename);
		FILE* file;
		
		if ((file = fw.openFile()) == 0) {
			Misc::alert("Couldn't open file for writing.");
			return false;
		}
		
		fprintf (file, "P4\n%d %d\n", pixel.getWidth(), pixel.getHeight());
		fwrite (pixel.getBuffer(), 1, pixel.getSize(), file);

		return true;
	}

} // namespace ImageFormats

