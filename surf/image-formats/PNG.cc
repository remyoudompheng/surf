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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_PNG

#include <FileWriter.h>
#include <RgbBuffer.h>
#include <ScriptVar.h>
#include <IO.h>
#include <PNG.h>

#include <png.h>

#include<iostream>

namespace ImageFormats {

	PNG imgFmt_PNG;

	bool PNG::saveColorImage(const char* filename, RgbBuffer& buffer)
	{
		FileWriter fw(filename);
		FILE* file;
		
		if((file = fw.openFile()) == 0) {
		        std::cerr << "Could not open file for writing.\n";
			return false;
		}

		if(ScriptVar::colormap_output_data == ScriptVar::colormap_output_netscape_data) {
			buffer.NetscapeColor();
		} else {
			buffer.OptimizedColor(ScriptVar::display_color_dither_data, ScriptVar::display_dither_value_data);
		}

		
                // allocate PNG structs:
		
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		if(png_ptr == 0) {
			IO::print_warning("Allocation of PNG write buffer failed. Saving aborted!");
			return false;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr) {
			IO::print_warning("Allocation of PNG info buffer failed. Saving aborted!");
			png_destroy_write_struct(&png_ptr, 0);
			return false;
		}

		png_init_io(png_ptr, file);


		// header:

		int width = buffer.getWidth();
		int height = buffer.getHeight();
		png_set_IHDR(png_ptr, info_ptr, width, height, 8,
			     PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_ADAM7,
			     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);


		// palette:

		size_t numcols = buffer.getNumCols();
		png_colorp palette = new png_color[numcols];
		const byte* rmap = buffer.getRMap();
		const byte* gmap = buffer.getGMap();
		const byte* bmap = buffer.getBMap();
		for(size_t i = 0; i < numcols; i++) {
			palette[i].red = rmap[i];
			palette[i].green = gmap[i];
			palette[i].blue = bmap[i];
		}
		png_set_PLTE(png_ptr, info_ptr, palette, numcols);

		
		// rows:

		png_bytep ptr = const_cast<png_bytep>(buffer.getMap());
		png_bytepp row_pointers = new png_bytep[height];
		for(int i = 0; i != height; i++) {
			row_pointers[i] = ptr;
			ptr += width;
		}
		png_set_rows(png_ptr, info_ptr, row_pointers);

		
                // write everything:
		
		png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);

		return true;
	}

} // namespace ImageFormats

#endif // HAVE_PNG
