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




#ifndef IMAGEFORMAT_XPM_H
#define IMAGEFORMAT_XPM_H

#include <ImageFormats.h>

namespace ImageFormats {

	class XPM : public Format {
	public:
		std::string getName() const {
			return "X Pixmap (8 bit)";
		}
		std::string getID() const {
			return "xpm";
		}
		bool isExtension(const std::string& ext) const {
			return ext == "xpm";
		}

		bool isColorFormat() const {
			return true;
		}

		bool saveColorImage(const char* filename, RgbBuffer& data);
	};

	extern XPM imgFmt_XPM;
}

#endif //!IMAGEFORMAT_XPM_H
