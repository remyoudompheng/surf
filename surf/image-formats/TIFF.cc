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



#ifdef HAVE_LIBTIFF

#include <sys/param.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "bit_buffer.h"
#include "Misc.h"
#include "FileWriter.h"

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

#include "TIFF.h"

namespace ImageFormats {
	
	Tiff imgFmt_TIFF;
	
	bool Tiff::saveDitheredImage(const char* filename,
				     bit_buffer& pixel,
				     int paper_width, int paper_height, int resolution,
				     bool fromDlg)
	{
		// check if it's a pipe:

		{
			FileWriter fw(filename);
			if (fw.isWritingToPipe()) {
				Misc::alert("TIFF images can't be written to pipes.");
				return false;
			}
		}

		// --------------------
		//  User name and info
		// --------------------
		
		passwd* passwd_user;
		
		passwd_user = getpwuid(getuid());
		
		char* name_user = passwd_user->pw_name;
		// char    *info_user = passwd_user->pw_gecos;
		
		// ----------
		//  Hostname
		// ----------
		
		char hostname[MAXHOSTNAMELEN];
		
		gethostname(hostname, MAXHOSTNAMELEN);
		
		// ---------------
		//  Time and date
		// ---------------
		
		time_t  time_local;
		char* the_time;
		
		time_local = time(0);
		the_time = ctime(&time_local);
		
		// ---------------
		//  Picture title
		// ---------------
		
		const char* title = "algebraic curve/surface (dithered image)";
		
		TIFF* tif;
		
		tif = TIFFOpen(filename, "w");
		if (tif == 0) {
			ostrstream ostr;
			ostr << "Error saving file " << filename;
			Misc::alert(ostr);
			return false;
		}
		TIFFSetField( tif,TIFFTAG_FILLORDER,(guint16)FILLORDER_MSB2LSB );
		
		TIFFSetField( tif,TIFFTAG_IMAGEWIDTH ,(guint32)paper_width );
		TIFFSetField( tif,TIFFTAG_IMAGELENGTH,(guint32)paper_height );
		
		TIFFSetField( tif,TIFFTAG_BITSPERSAMPLE,(guint16)1 );
		
		TIFFSetField( tif,TIFFTAG_COMPRESSION,(guint16)COMPRESSION_NONE );
                //    TIFFSetField( tif,TIFFTAG_COMPRESSION,(guint16)COMPRESSION_PACKBITS );
		TIFFSetField( tif,TIFFTAG_SAMPLESPERPIXEL,(guint16)1 );
		
		TIFFSetField( tif,TIFFTAG_PHOTOMETRIC,(guint16)PHOTOMETRIC_MINISBLACK );
		TIFFSetField( tif,TIFFTAG_FILLORDER,(guint16)FILLORDER_MSB2LSB );
		TIFFSetField( tif,TIFFTAG_ORIENTATION,(guint16)ORIENTATION_TOPLEFT );
		TIFFSetField( tif,TIFFTAG_PLANARCONFIG,(guint16)PLANARCONFIG_CONTIG );
		TIFFSetField( tif,TIFFTAG_ROWSPERSTRIP,
			      TIFFDefaultStripSize( tif,(guint32)(-1) ) );
		
		TIFFSetField( tif,TIFFTAG_RESOLUTIONUNIT,(guint16)RESUNIT_INCH );
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
		
		for (py = 0; py < paper_height; py++) {
			for (px = 0, count = 0; px < paper_width; px += 8, count++) {
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
