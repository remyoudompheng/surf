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




#include <AvailableImageFormats.h>
#include <ScriptVar.h>

#include<iostream>

namespace ImageFormats {

	bool saveColorImage(const char* filename, RgbBuffer& data)
	{
		int cff = ScriptVar::color_file_format_data;
		if(cff > int(numAvailableFormats) || cff < 0) {
			std::cerr << "color_file_format out of range!\n";
			return false;
		}
		Format* fmt = availableFormats[cff];
		if(fmt->getColorType() == dithered) {
			std::cerr << "You didn't choose a valid color file format!\n";
			return false;
		}
		return availableFormats[cff]->saveColorImage(filename, data);
	}

	bool saveDitheredImage(const char* filename, bit_buffer& data)
	{
		int dff = ScriptVar::dither_file_format_data;
		if(dff > int(numAvailableFormats) || dff < 0) {
			std::cerr << "dither_file_format out of range!\n";
			return false;
		}
		Format* fmt = availableFormats[dff];
		if(fmt->getColorType() == color) {
			std::cerr << "You didn't choose a valid dithered file format!\n";
			return false;
		}
		return availableFormats[dff]->saveDitheredImage(filename, data);
	}

	
	// some file format implementations could use this utility function:
	// Write a long word to a file respecting endianess:
	void put_long(long word, FILE* file, int order)
	{
#if __BYTE_ORDER == LITTLE_ENDIAN
		if (order == BIG_ENDIAN) {
#else
	        if (order == LITTLE_ENDIAN) {
#endif
			fputc((word >> 24) & 0x0ff, file);
			fputc((word >> 16) & 0x0ff, file);
			fputc((word >> 8) & 0x0ff, file);
			fputc(word & 0x0ff, file);
		} else {
			fputc(word & 0x0ff, file);
			fputc((word >> 8) & 0x0ff, file);
			fputc((word >> 16) & 0x0ff, file);
			fputc((word >> 24) & 0x0ff, file);
		}
	}
	
	
	void swapshort(char* bp, unsigned int n)
	{
		char c;
		char* ep = bp + n;
		
		while (bp < ep) {
			c = *bp;
			*bp = *(bp + 1);
			bp++;
			*bp++ = c;
		}
	}

	void swaplong(char* bp, unsigned int n)
	{
		char c;
		char *ep = bp + n;
		char *sp;
		
		while (bp < ep) {
			sp = bp + 3;
			c = *sp;
			*sp = *bp;
			*bp++ = c;
			sp = bp + 1;
			c = *sp;
			*sp = *bp;
			*bp++ = c;
			bp += 2;
		}
	}



} // namespace ImageFormats
