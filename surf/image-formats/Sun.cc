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

#include <Sun.h>
#include <FileWriter.h>
#include <RgbBuffer.h>

#include <netinet/in.h>

#include<cstdio>
#include<iostream>


namespace {

// Will be get into trouble if unsigned int isn't 4 bytes?
typedef unsigned int uint32;

void put_long(uint32 hostlong, FILE* file)
{
	uint32 be_long = htonl(hostlong);
	std::fwrite(&be_long, 1, 4, file);
}

const uint32 SUN_MAGIC = 0x59a66a95;
const uint32 RT_STANDARD = 1;
const uint32 RMT_NONE = 0;

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
	
		uint32 width = data.getWidth();
		uint32 height = data.getHeight();
		uint32 length = width*height*3;
		
		// write rasterfile header:
                // (note: use Sun byte order == network order)

		put_long(SUN_MAGIC, file);   // ras_magic
		put_long(width, file);       // ras_width
		put_long(height, file);      // ras_height
		put_long(24, file);          // ras_depth
		put_long(length, file);      // ras_length
		put_long(RT_STANDARD, file); // ras_type
		put_long(RMT_NONE, file);    // ras_maptype
		put_long(0, file);           // ras_maplength

		// write RGB data:
		
		const byte* rdata = data.getRData();
		const byte* gdata = data.getGData();
		const byte* bdata = data.getBData();
		
		for(size_t i = 0; i < length; i++) {
			std::fputc(bdata[i], file);
			std::fputc(gdata[i], file);
			std::fputc(rdata[i], file);
		}

		return true;
	}
		

} // namespace ImageFormats
