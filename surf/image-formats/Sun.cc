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




#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include <FileWriter.h>
#include <RgbBuffer.h>

#include <Sun.h>

#include <errno.h>
#include <string.h>
#include <netinet/in.h>

#include<iostream>


#ifdef SUN
int fread ( char*,int,int,FILE* );
int fwrite ( char*,int,int,FILE* );
int fclose ( FILE* );
#endif /* SUN */

struct RASHDR
{
	long ras_magic;
	long ras_width;
	long ras_height;
	long ras_depth;
	long ras_length;
	long ras_type;
	long ras_maptype;
	long ras_maplength;
};

#define SUN_MAGIC        0x59a66a95
#define RT_OLD	         0
#define RT_STANDARD      1
#define RT_BYTE_ENCODED  2
#define RT_RGB_FORMAT    3
#define RT_TIFF_FORMAT   4
#define RT_IFF_FORMAT    5

#define RMT_NONE	  0
#define RMT_EQUAL_RGB	  1
#define RMT_RAW	  2

namespace {
	void put_long(uint32_t hostlong, FILE* file)
	{
		uint32_t be_long = htonl(hostlong);
		fwrite(&be_long, 1, sizeof(uint32_t), file);
	}
}

namespace ImageFormats {

	Sun imgFmt_Sun;

	bool Sun::saveColorImage(const char* filename, RgbBuffer& data)
	{
		FileWriter fw(filename);
		FILE *file;
		
		if((file = fw.openFile()) == 0) {
			std::cerr << "Could not open file for writing.\n";
			return false;
		}
	
		
		RASHDR rhdr;
		int width = data.getWidth();
		int height = data.getHeight();
		size_t length = width*height;
		
		/* Initialize Sun raster header */
		
		rhdr.ras_magic     = SUN_MAGIC;
		rhdr.ras_width     = width;
		rhdr.ras_height    = height;
		rhdr.ras_depth     = 24;
		rhdr.ras_length    = length*3;
		rhdr.ras_type      = RT_STANDARD;
		rhdr.ras_maptype   = RMT_NONE;
		rhdr.ras_maplength = 0;

		/* Write rasterfile header
                   note: use Sun byte order == network order */
		
		put_long(rhdr.ras_magic, file);
		put_long(rhdr.ras_width, file);
		put_long(rhdr.ras_height, file);
		put_long(rhdr.ras_depth, file);
		put_long(rhdr.ras_length, file);
		put_long(rhdr.ras_type, file);
		put_long(rhdr.ras_maptype, file);
		put_long(rhdr.ras_maplength, file);
		
		const byte* rdata = data.getRData();
		const byte* gdata = data.getGData();
		const byte* bdata = data.getBData();
		
		for(size_t i = 0; i < length; i++) {
			fputc(bdata[i], file);
			fputc(gdata[i], file);
			fputc(rdata[i], file);
		}

		return true;
	}
		

} // namespace ImageFormats
