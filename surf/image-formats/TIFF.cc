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

#ifdef HAVE_LIBTIFF

#include <sys/param.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <bit_buffer.h>
#include <FileWriter.h>
#include <ScriptVar.h>

#ifdef NO_GETHOSTNAME_PROTO
extern "C" int gethostname(char*, int);
#endif


#ifdef TIFF_HEADER_34
#include <tiff34/tiffio.h>
#else
#include <tiffio.h>
#endif

#ifndef TIFFDefaultStripSize
#define TIFFDefaultStripSize(tiff, request) (8192/TIFFScanlineSize(tif))
#endif

#include <TIFF.h>

#include<iostream>

namespace ImageFormats {
	
	Tiff imgFmt_TIFF;
	
	bool Tiff::saveDitheredImage(const char* filename, bit_buffer& pixel)
	{
		// check if it's a pipe:

		{
			FileWriter fw(filename);
			if (fw.isWritingToPipe()) {
				std::cerr << "Sorry: TIFF images can't be written to pipes.\n";
				return false;
			}
		}

		passwd* passwd_user = getpwuid(getuid());
		
		char* name_user = passwd_user->pw_name;
		// char    *info_user = passwd_user->pw_gecos;
		
		char hostname[MAXHOSTNAMELEN];
		
		gethostname(hostname, MAXHOSTNAMELEN);
		
		time_t time_local = time(0);
		char* the_time = ctime(&time_local);
		
		const char* title = "algebraic curve/surface (dithered image)";
		
		TIFF* tif;
		
		tif = TIFFOpen(filename, "w");
		if (tif == 0) {
			std::cerr << "Error saving file " << filename << '\n';
			return false;
		}
		TIFFSetField( tif,TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
		
		TIFFSetField( tif,TIFFTAG_IMAGEWIDTH, ScriptVar::main_width_data);
		TIFFSetField( tif,TIFFTAG_IMAGELENGTH, ScriptVar::main_height_data);
		
		TIFFSetField( tif,TIFFTAG_BITSPERSAMPLE, 1);
		
		TIFFSetField( tif,TIFFTAG_COMPRESSION, COMPRESSION_NONE );
                //    TIFFSetField( tif,TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS );
		TIFFSetField( tif,TIFFTAG_SAMPLESPERPIXEL, 1 );
		
		TIFFSetField( tif,TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK );
		TIFFSetField( tif,TIFFTAG_FILLORDER, FILLORDER_MSB2LSB );
		TIFFSetField( tif,TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT );
		TIFFSetField( tif,TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );
		TIFFSetField( tif,TIFFTAG_ROWSPERSTRIP,
			      TIFFDefaultStripSize( tif,(guint32)(-1) ) );
		
		TIFFSetField( tif,TIFFTAG_RESOLUTIONUNIT,(guint16)RESUNIT_INCH );
		int resolution = ScriptVar::print_resolution_array_data[ScriptVar::print_resolution_data];
		TIFFSetField( tif,TIFFTAG_XRESOLUTION,(float)(resolution) );
		TIFFSetField( tif,TIFFTAG_YRESOLUTION,(float)(resolution) );
		TIFFSetField( tif,TIFFTAG_XPOSITION,(float)0.0 );
		TIFFSetField( tif,TIFFTAG_YPOSITION,(float)0.0 );
		
		TIFFSetField( tif,TIFFTAG_SOFTWARE, PACKAGE " " VERSION );
		TIFFSetField( tif,TIFFTAG_ARTIST,name_user );
		TIFFSetField( tif,TIFFTAG_HOSTCOMPUTER,hostname );
		TIFFSetField( tif,TIFFTAG_DOCUMENTNAME, filename);
		TIFFSetField( tif,TIFFTAG_DATETIME,the_time );
		TIFFSetField( tif,TIFFTAG_IMAGEDESCRIPTION,title );
		
		guint8* scanline = new guint8[TIFFScanlineSize(tif)];
		
		if (scanline == 0) {
			fprintf(stderr, "cant alloc ...\n");
			return false;
		}
		
		int px,py,count;
		unsigned char byte;
		
		for (py = 0; py < ScriptVar::main_height_data; py++) {
			for (px = 0, count = 0; px < ScriptVar::main_width_data; px += 8, count++) {
				byte = 255 - pixel.getByte(px,py);
				scanline[count] = byte;
			}
			TIFFWriteScanline(tif, scanline, py, 0);
		}
		
		delete [] scanline;
		TIFFWriteDirectory(tif);
		TIFFClose(tif);
		
		return true;
	}

} // namespace ImageFormats

#endif // HAVE_LIBTIFF
