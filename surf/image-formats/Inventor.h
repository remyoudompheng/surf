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


#ifndef IMAGEFORMAT_INVENTOR_H
#define IMAGEFORMAT_INVENTOR_H

#include <ImageFormats.h>

#include <gts.h>

#include<map>

class Triangulator;
class SoCoordinate3;
class SoNormal;
class SoIndexedFaceSet;

namespace ImageFormats {

	class OpenInventor : public Format {
	public:
		virtual ~OpenInventor() {}
		
		std::string getName() const {
			return "Inventor";
		}
		std::string getID() const {
			return "inventor";
		}
		bool isExtension(const std::string& ext) const {
			return ext == "iv";
		}

		bool is3DFormat() const {
			return true;
		}

		bool save3DImage(const char* filename, Triangulator& data);

	private:
		Triangulator* tritor;
		
		int vertex_index;
		std::map<GtsVertex*, int> vertex_map;
		
		SoCoordinate3* coords;
		SoNormal* normals;
		SoIndexedFaceSet* faceSets;

		void vertex_func(GtsVertex* f);
		static gint _vertex_func(gpointer f, gpointer This) {
			static_cast<OpenInventor*>(This)->vertex_func(static_cast<GtsVertex*>(f));
			return 0;
		}
		
		void face_func(GtsFace* f);
		static gint _face_func(gpointer f, gpointer This) {
			static_cast<OpenInventor*>(This)->face_func(static_cast<GtsFace*>(f));
			return 0;
		}
	};

	extern OpenInventor imgFmt_Inventor;
}

#endif //!IMAGEFORMAT_INVENTOR_H
