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

/// Namespace which contains general functions to save images and special classes for each image format.
namespace ImageFormats {

	/**
         * Save color image.
         * Determines current color file format and calls the appropriate
         * image format class to save the image in this format.
         * @param filename Filename of the image file to be written
         * @param data The RGB data of the curve/surface
         * @return false if something went wrong, otherwise true
         */
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

        /**
         * Save dithered image.
         * Determines current dither file format and calls the appropriate
         * image format class to save the image in this format.
         * @param filename Filename of the image file to be written
         * @param data The bitplane data of the dithered curve/surface
         * @return false if something went wrong, otherwise true
         */
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

        /**
         * Save 3D image.
         * Determines 3D file format and saves the triangulated data of the
         * surface by calling the appropriate image format class.
         * @param filename Filename to save the data to.
         * @param data Tringulated data of the surface.
         * @return false: error, true: success
         */
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
