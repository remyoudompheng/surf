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



#include "AvailableImageFormats.h"
#include "Misc.h"

namespace ImageFormats {

	ByExtension imgFmt_ByExtension;

	bool ByExtension::saveColorImage(const char* filename, RgbBuffer& data, bool fromDlg)
	{
		const char* newfilename;
		Format* fmt = guessFormat(filename, color, &newfilename);

		if (fmt == 0) {
			return false;
		}
		return fmt->saveColorImage(newfilename, data, fromDlg);
	}

	bool ByExtension::saveDitheredImage(const char* filename,
					    bit_buffer& pixel,
					    int paper_width, int paper_height, int resolution,
					    bool fromDlg)
	{
		const char* newfilename;
		Format* fmt = guessFormat(filename, dithered, &newfilename);

		if (fmt == 0) {
			return false;
		}
		return fmt->saveDitheredImage(newfilename, pixel, paper_width, paper_height, resolution, fromDlg);
	}

	Format* ByExtension::guessFormat(const char* filename, ColorType type,
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
			default: // suppress warning
				return 0;
			}
		}
		
		// check if filename is of type "<fmt>|<command>"
		
		const int maxExtLen = 32;
		int len = strlen(filename);
		
		if (len > maxExtLen) {
			len = maxExtLen;
		}
		
		bool pipe = false;
		char ext[maxExtLen];
		
		for (int i = 0; i != len; ++i) {
			if (filename[i] == '|') {
				pipe = true;
				std::strncpy(ext, filename, i);
				ext[i] = 0;
				*newfilename = filename + i;
				break;
			}
		}
		
		if (!pipe) { // just look for "*.extension"
			char* extPtr = strrchr(filename, '.');
			if (extPtr == 0) {
				return 0;
			}
			strncpy(ext, extPtr + 1, maxExtLen);
			*newfilename = filename;
		}

		return findFormatByExt(ext, type);
	}

	Format* ByExtension::findFormatByExt(const char* ext, ColorType type)
	{
		for (int i = 0; i != numAvailableFormats; ++i) {
			if (availableFormats[i]->isExtension(ext)) {
				ColorType fmtType = availableFormats[i]->getColorType();
				if (fmtType == both || fmtType == type) {
					return availableFormats[i];
				} else {
					Misc::alert("You chose the wrong extension.");
					return 0;
				}
			}
		}
		Misc::alert("Couldn't determine file type by extension.");
		return 0;
	}
	
} // namespace ImageFormats
