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




#ifndef IMAGEFORMAT_SUN_H
#define IMAGEFORMAT_SUN_H

#include <ImageFormats.h>

namespace ImageFormats {

	class Sun : public Format {
	public:
		std::string getName() const {
			return "Sun Rasterfile";
		}
		std::string getID() const {
			return "sun";
		}
		ColorType getColorType() const {
			return color;
		}
		bool isExtension(const std::string& ext) const {
			return ext == "ras";
		}

		bool saveColorImage(const char* filename, RgbBuffer& data);
		
		bool saveDitheredImage(const char* filename, bit_buffer& data) {
			return false;
		}
	};

	extern Sun imgFmt_Sun;

}

#endif //!IMAGEFORMAT_SUN_H
