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




#ifndef IMAGEFORMAT_BYEXTENSION_H
#define IMAGEFORMAT_BYEXTENSION_H

#include "ImageFormats.h"

#include <cstring>

namespace ImageFormats {

	class ByExtension : public Format {
	public:
		const char* getName() {
			return "By Extension";
		}
		
		ColorType getColorType() {
			return both;
		}

		bool isExtension(const char* ext) {
			return false;
		}

		bool saveColorImage(const char* filename, RgbBuffer& data, bool fromDlg);
		
		bool saveDitheredImage(const char* filename, bit_buffer& data, int paper_width, int paper_height, int resolution, bool fromDlg);

	private:
		Format* guessFormat(const char* filename, ColorType type, const char** newfilename);
		Format* findFormatByExt(const char* ext, ColorType type);
	};

	extern ByExtension imgFmt_ByExtension;

}

#endif //!IMAGEFORMAT_BYEXTENSION_H