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

#ifndef IMAGEFORMATS_H
#define IMAGEFORMATS_H

#include <string>

class bit_buffer;
class RgbBuffer;

namespace ImageFormats {
	enum ColorType {
		color, dithered, both
	};
	
	class Format {
	public:
		virtual std::string getName() const = 0;
		virtual std::string getID() const = 0;
		virtual bool isExtension(const std::string& ext) const = 0;
		virtual ColorType getColorType() const = 0;
		
		virtual bool saveColorImage(const char* filename, RgbBuffer& data) = 0;
		virtual bool saveDitheredImage(const char* filename, bit_buffer& data) = 0;
	};

	bool saveColorImage(const char* filename, RgbBuffer& data);
	bool saveDitheredImage(const char* filename, bit_buffer& data);
}

#endif //!IMAGEFORMATS_H
