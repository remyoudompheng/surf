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


#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <hornerarith.h>

#ifdef HAVE_LIBGTS

#include <gts.h>

#include<map>

class Triangulator {
private:
	Triangulator(const Triangulator&);
	void operator=(const Triangulator&);
	
public:
	Triangulator();
	virtual ~Triangulator();

	void triangulate();
	void write_gts_file();
	void write_data();

private:
	void vertex_func(GtsVertex* v);
	static gint _vertex_func(gpointer v, gpointer This) {
		reinterpret_cast<Triangulator*>(This)->vertex_func(static_cast<GtsVertex*>(v));
		return 0;
	}
	guint vertex_count;
	std::map<GtsVertex*, guint> vertex_map;
	void face_func(GtsFace* f);
	static gint _face_func(gpointer f, gpointer This) {
		reinterpret_cast<Triangulator*>(This)->face_func(static_cast<GtsFace*>(f));
		return 0;
	}
	void iso_func(gdouble** f, GtsCartesianGrid g, guint k);
	static void _iso_func(gdouble** f, GtsCartesianGrid g, guint k, gpointer This) {
		reinterpret_cast<Triangulator*>(This)->iso_func(f, g, k);
	}
	void calc_normal(const GtsPoint& p, float& x, float& y, float& z);

	hornerpolyxyz* hf;
	hornerpolyxyz* hdx;
	hornerpolyxyz* hdy;
	hornerpolyxyz* hdz;
		
	GtsSurface* surface;
};

#endif //HAVE_LIBGTS

#endif //!TRIANGULATOR_H
