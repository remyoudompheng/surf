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

#ifdef HAVE_GTS

#include <gts.h>

class Triangulator {
private:
	Triangulator(const Triangulator&);
	void operator=(const Triangulator&);
	
public:
	Triangulator() : surface(0) {}
	virtual ~Triangulator();

	void triangulate();

	GtsSurface* getSurface() const {
		return surface;
	}

	struct Point {
		gdouble x;
		gdouble y;
		gdouble z;
	};

	void initNormals();
	Point getNormal(const Point& vertex);
	Point getNormal(GtsVertex* v) {
		Point p = { v->p.x, v->p.y, v->p.z };
		return getNormal(p);
	}
	void deinitNormals();

private:
	void clip();
	
	void iso_func(gdouble** f, GtsCartesianGrid g, guint k);
	static void _iso_func(gdouble** f, GtsCartesianGrid g, guint k, gpointer This) {
		reinterpret_cast<Triangulator*>(This)->iso_func(f, g, k);
	}
	
	hornerpolyxyz* hf;
	hornerpolyxyz* hdx;
	hornerpolyxyz* hdy;
	hornerpolyxyz* hdz;
		
	GtsSurface* surface;
};

#endif //HAVE_GTS

#endif //!TRIANGULATOR_H
