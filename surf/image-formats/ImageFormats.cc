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
		if(!fmt->isColorFormat()) {
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
		if(!fmt->isDitherFormat()) {
			std::cerr << "You didn't choose a valid dithered file format!\n";
			return false;
		}
		return availableFormats[dff]->saveDitheredImage(filename, data);
	}

	bool save3DImage(const char* filename, Triangulator& data)
	{
		int tdff = ScriptVar::three_d_file_format_data;
		if(tdff > int(numAvailableFormats) || tdff < 0) {
			std::cerr << "three_d_file_format out of range!\n";
			return false;
		}
		Format* fmt = availableFormats[tdff];
		if(!fmt->is3DFormat()) {
			std::cerr << "You didn't choose a valid 3D file format!\n";
			return false;
		}
		return availableFormats[tdff]->save3DImage(filename, data);
	}

} // namespace ImageFormats
