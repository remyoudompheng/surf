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




#include <FileWriter.h>
#include <RgbBuffer.h>
#include <PPM.h>

#include<iostream>

namespace ImageFormats {

	PPM imgFmt_PPM;

	bool PPM::saveColorImage(const char* filename, RgbBuffer& data)
	{
		FileWriter fw(filename);
		FILE *file;

		if((file = fw.openFile()) == 0) {
			std::cerr << "Could not open file for writing.\n";
			return false;
		}
	
		int width = data.getWidth();
		int height = data.getHeight();
		
		fprintf (file, "P6\n%d %d\n255\n", width, height);
		
		const guint8* rdata = data.getRData();
		const guint8* gdata = data.getGData();
		const guint8* bdata = data.getBData();
		
		size_t n = width*height;

		for(size_t i = 0; i < n; i++) {
			fprintf (file, "%c%c%c", rdata[i], gdata[i], bdata[i]);
		}
		
		return true;
	}

} // namespace ImageFormats
