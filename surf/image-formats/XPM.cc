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


#include <FileWriter.h>
#include <RgbBuffer.h>
#include <ScriptVar.h>
#include <XPM.h>

#include<iostream>

namespace ImageFormats {

	XPM imgFmt_XPM;

	bool XPM::saveColorImage(const char* filename, RgbBuffer& buffer)
	{
		FileWriter fw(filename);
		FILE* file;
		
		if((file = fw.openFile()) == 0) {
		        std::cerr << "Could not open file for writing.\n";
			return false;
		}

		if(ScriptVar::colormap_output_data == ScriptVar::colormap_output_optimized_data) {
			buffer.OptimizedColor(ScriptVar::display_color_dither_data, ScriptVar::display_dither_value_data);
		} else {
			buffer.NetscapeColor();
		}

		// write header

		fputs("/* XPM */\n"
		      "static char* surf_image[] = {\n\n", file);

		int width = buffer.getWidth();
		int height = buffer.getHeight();
		size_t nmap = buffer.getNumCols();

		fprintf(file, "\"%d %d %d 2\",\n\n", width, height, nmap);
		
		// write colors
		
		const byte* rmap = buffer.getRMap();
		const byte* gmap = buffer.getGMap();
		const byte* bmap = buffer.getBMap();

		for(size_t i = 0; i < nmap; i++) {
			fprintf(file, "\"%.2x c #%.2x%.2x%.2x\",\n", i, rmap[i], gmap[i], bmap[i]);
		}

		// write pixels

		const byte* ptr = buffer.getMap();

		for(int i = 0; i < height; i++) {
			fputc('\"', file);
			for(int j = 0; j < width; j++) {
				fprintf(file, "%.2x", *ptr++);
			}
			fputs("\",\n", file);
		}

		fputs("\n};\n", file);

		return true;
	}

} // namespace ImageFormats
