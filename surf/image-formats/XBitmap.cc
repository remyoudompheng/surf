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
#include <limits.h>

#include <bit_buffer.h>
#include <FileWriter.h>
#include <ScriptVar.h>
#include <XBitmap.h>

#include<iostream>

namespace ImageFormats {

	XBitmap imgFmt_XBitmap;
	
	inline unsigned char rotateByte(unsigned char c)
	{
		unsigned char retval = 0;
		for (int i = 0; i< CHAR_BIT; i++) {
			retval <<= 1;
			retval |= c & 1;
			c >>= 1;
		}
		return retval;
	}
	
	
	bool XBitmap::saveDitheredImage(const char* filename, bit_buffer& pixel)
	{
		FileWriter fw(filename);
		FILE* file;

		if ((file = fw.openFile()) == 0) {
			std::cerr << "Couldn't open file for writing.\n";
			return false;
		}
		
		int bytes_per_line = ScriptVar::main_width_data/8 + (ScriptVar::main_width_data%8 == 0 ? 0 : 1);
		int bits_per_line = bytes_per_line*8;
		
		fprintf(file,"#define %s_width %d\n", filename, bits_per_line);
		fprintf(file,"#define %s_height %d\n", filename, ScriptVar::main_height_data);
		fprintf(file,"static char %s_bits[] = {\n", filename);
		
		int komma_end = ScriptVar::main_width_data*bytes_per_line - 1;
		int count = 0;

		for(int py = 0; py < ScriptVar::main_height_data; py++) {
			fprintf(file, "   ");		
			
			for(int px = 0; px < ScriptVar::main_width_data; px += 8) {
				int byte = rotateByte(pixel.getByte(px,py));
				fprintf(file, " 0x%.2x",byte);
				count++;
				
				if (count < komma_end) {
					fprintf(file, ",");
				}
			}
		}
		fprintf(file, "};\n");
		
		return true;
	}

} // namespace ImageFormats

