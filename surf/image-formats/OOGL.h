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


#ifndef IMAGEFORMAT_OOGL_H
#define IMAGEFORMAT_OOGL_H

#include <ImageFormats.h>

#include <gts.h>

#include <stdio.h>

#include<map>

class Triangulator;

namespace ImageFormats {

	class OOGL : public Format {
	public:
		OOGL() : num_vertices(0) {}
		virtual ~OOGL() {}
		
		std::string getName() const {
			return "OOGL/OFF (Geomview)";
		}
		std::string getID() const {
			return "oogl";
		}
		bool isExtension(const std::string& ext) const {
			return ext == "off" || ext == "oogl";
		}

		bool is3DFormat() const {
			return true;
		}

		bool save3DImage(const char* filename, Triangulator& data);

	private:
		FILE* file;
		
		Triangulator* tritor;
		int num_vertices;
		std::map<GtsVertex*, int> vertex_map;

		void vertex_func(GtsVertex* v);
		static gint _vertex_func(gpointer v, gpointer This) {
			static_cast<OOGL*>(This)->vertex_func(static_cast<GtsVertex*>(v));
			return 0;
		}
		void face_func(GtsFace* f);
		static gint _face_func(gpointer f, gpointer This) {
			static_cast<OOGL*>(This)->face_func(static_cast<GtsFace*>(f));
			return 0;
		}
	};

	extern OOGL imgFmt_OOGL;
}

#endif //!IMAGEFORMAT_OOGL_H
