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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_LIBJPEG

#include <FileWriter.h>
#include <RgbBuffer.h>
#include <ScriptVar.h>

// these two includes and the 'extern "C" { }' are needed for jpeglib.h
// otherwise I get errors here :-/  (stdio.h is neede anyway..)
#include <ctype.h>
#include <stdio.h>
extern "C" {
#include <jpeglib.h>
}

#include <JPEG.h>

#include<iostream>

namespace ImageFormats {

	JPEG imgFmt_JPEG;

	bool JPEG::saveColorImage(const char* filename, RgbBuffer& buffer)
	{
		FileWriter fw(filename);
		FILE* file;
		
		if((file = fw.openFile()) == 0) {
		        std::cerr << "Could not open file for writing.\n";
			return false;
		}

		int width = buffer.getWidth();
		int height = buffer.getHeight();

		// init cinfo struct:

		jpeg_compress_struct cinfo;
		jpeg_error_mgr jerr;
		
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, file);
		cinfo.image_width = width;
		cinfo.image_height = height;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, ScriptVar::jpeg_quality, true);
		jpeg_start_compress(&cinfo, true);
		
		
		// compress and write to file:
		
		int row_stride = width*3;
		JSAMPLE* row = new JSAMPLE[row_stride];
		JSAMPROW rowpointer[1] = { row };
		JSAMPLE* rptr = buffer.getRData();
		JSAMPLE* gptr = buffer.getGData();
		JSAMPLE* bptr = buffer.getBData();
		
		while(cinfo.next_scanline < unsigned(height)) {
			JSAMPLE* ptr = row;
			for(int i = 0; i < width; ++i) {
				*ptr++ = *rptr++;
				*ptr++ = *gptr++;
				*ptr++ = *bptr++;
			}
			jpeg_write_scanlines(&cinfo, rowpointer, 1);
		}
		
		delete [] row;
		
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);

		return true;
	}

} // namespace ImageFormats

#endif // HAVE_LIBJPEG
