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




#ifndef IMAGEFORMAT_BYEXTENSION_H
#define IMAGEFORMAT_BYEXTENSION_H

#include <ImageFormats.h>

#include<iostream>

namespace ImageFormats {

	class ByExtension : public Format {
	public:
		std::string getName() const {
			return "By Extension";
		}
	        std::string getID() const {
			return "auto";
		}
		bool isExtension(const std::string& ext) const {
			return false;
		}

		bool isColorFormat() const {
			return true;
		}
		bool isDitherFormat() const {
			return true;
		}
		bool is3DFormat() const {
			return true;
		}

		bool saveColorImage(const char* filename, RgbBuffer& data);
		bool saveDitheredImage(const char* filename, bit_buffer& data);
		bool save3DImage(const char* filename, Triangulator& data);

	private:
		enum Type {
			color, dithered, three_d
		};
		Format* guessFormat(const char* filename, Type type, const char** newfilename);
	};

	extern ByExtension imgFmt_ByExtension;
}

#endif //!IMAGEFORMAT_BYEXTENSION_H
