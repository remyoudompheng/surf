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

#ifdef HAVE_LIBTIFF

#include <bit_buffer.h>
#include <RgbBuffer.h>
#include <FileWriter.h>
#include <ScriptVar.h>
#include <Misc.h>

#ifdef TIFF_HEADER_34
#include <tiff34/tiffio.h>
#else
#include <tiffio.h>
#endif

#include <TIFF.h>

#include<iostream>

namespace ImageFormats {
	
	Tiff imgFmt_TIFF;
	
	bool Tiff::saveColorImage(const char* filename, RgbBuffer& buffer)
	{
		TIFF* tiff = static_cast<TIFF*>(do_preps(filename));
		if(tiff == 0) {
			return false;
		}

		TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 3);

		// write scanlines:
		
		byte* scanline = new byte[TIFFScanlineSize(tiff)];
		if(scanline == 0) {
			Misc::print_warning("Couldn't allocate scanline buffer. Saving aborted!\n");
			return false;
		}

		byte* rdata = buffer.getRData();
		byte* gdata = buffer.getGData();
		byte* bdata = buffer.getBData();

		for(int py = 0; py < ScriptVar::main_height_data; py++) {

			byte* ptr = scanline;

			for(int px = 0, count = 0; px < ScriptVar::main_width_data; px++) {
				*ptr++ = *rdata++;
				*ptr++ = *gdata++;
				*ptr++ = *bdata++;
			}
			
			TIFFWriteScanline(tiff, scanline, py, 0);
		}
		
		delete [] scanline;

		TIFFClose(tiff);
		
		return true;
	}
	
	bool Tiff::saveDitheredImage(const char* filename, bit_buffer& pixel)
	{
		TIFF* tiff = static_cast<TIFF*>(do_preps(filename));
		if(tiff == 0) {
			return false;
		}
		
		TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 1);
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);

		TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

		TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
		float resolution = ScriptVar::print_resolution_array_data[ScriptVar::print_resolution_data];
		TIFFSetField(tiff, TIFFTAG_XRESOLUTION, resolution);
		TIFFSetField(tiff, TIFFTAG_YRESOLUTION, resolution);
		
		
		// write scanlines:
		
		byte* scanline = new byte[TIFFScanlineSize(tiff)];
		if(scanline == 0) {
			Misc::print_warning("Couldn't allocate scanline buffer. Saving aborted!\n");
			return false;
		}
		
		for(int py = 0; py < ScriptVar::main_height_data; py++) {
			for(int px = 0, count = 0; px < ScriptVar::main_width_data; px += 8, count++) {
				scanline[count] = 255 - pixel.getByte(px, py);
			}
			TIFFWriteScanline(tiff, scanline, py, 0);
		}
		
		delete [] scanline;

		TIFFClose(tiff);
		
		return true;
	}

	void* Tiff::do_preps(const char* filename)
	{
		// check if it's a pipe:
		{
			FileWriter fw(filename);
			if (fw.isWritingToPipe()) {
				std::cerr << "Sorry: TIFF images can't be written to pipes.\n";
				return 0;
			}
		}

		TIFF* tiff = TIFFOpen(filename, "w");
		if(tiff == 0) {
			std::cerr << "Error saving file " << filename << '\n';
			return 0;
		}
		
		TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, ScriptVar::main_width_data);
		TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, ScriptVar::main_height_data);
		
                TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
		
		TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		
		TIFFSetField(tiff, TIFFTAG_SOFTWARE, PACKAGE " " VERSION);

		TIFFSetField(tiff, TIFFTAG_IMAGEDESCRIPTION, "surf image");

		return tiff;
	}

} // namespace ImageFormats

#endif // HAVE_LIBTIFF
