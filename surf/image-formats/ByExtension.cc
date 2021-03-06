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

#include<iostream>
#include<cstring>

namespace ImageFormats {

	ByExtension imgFmt_ByExtension;

	bool ByExtension::saveColorImage(const char* filename, RgbBuffer& data)
	{
		const char* newfilename;
		Format* fmt = guessFormat(filename, color, &newfilename);

		if(fmt == 0) {
			return false;
		}
		return fmt->saveColorImage(newfilename, data);
	}

	bool ByExtension::saveDitheredImage(const char* filename, bit_buffer& pixel)
	{
		const char* newfilename;
		Format* fmt = guessFormat(filename, dithered, &newfilename);

		if (fmt == 0) {
			return false;
		}
		return fmt->saveDitheredImage(newfilename, pixel);
	}

	bool ByExtension::save3DImage(const char* filename, Triangulator& data)
	{
		const char* newfilename;
		Format* fmt = guessFormat(filename, three_d, &newfilename);

		if (fmt == 0) {
			return false;
		}
		return fmt->save3DImage(newfilename, data);
	}

	Format* ByExtension::guessFormat(const char* filename, Type type,
					 const char** newfilename)
	{
		// check if it's saving to a pipe:
		
		if (filename[0] == '|') {
			*newfilename = filename;
			switch (type) {
			case color:
				return &imgFmt_PPM;
			case dithered:
				return &imgFmt_PBM;
#ifdef HAVE_GTS
			case three_d:
				return &imgFmt_GTS;
#endif
			default:
				return 0;
			}
		}
		
		// just look for "*.extension"
		const char* ext = strrchr(filename, '.');
		if (ext == 0) {
			return 0;
		}
		ext++;

		*newfilename = filename;
		
		for (size_t i = 0; i != numAvailableFormats; ++i) {
			Format* fmt = availableFormats[i];
			if(fmt->isExtension(ext)) {
				if((type == color && fmt->isColorFormat())
				   || (type == dithered && fmt->isDitherFormat())
				   || (type == three_d && fmt->is3DFormat())) {
					return fmt;
				}
				std::cerr << "Unrecognized file extension!\n";
				return 0;
			}
		}

		std::cerr << "Couldn't determine file type by extension.\n";
		return 0;
	}
	
} // namespace ImageFormats
