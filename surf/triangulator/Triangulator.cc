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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_LIBGTS

#include <Triangulator.h>

#include <assert.h>
#include <stdio.h>
#include <gts.h>

namespace {
void sphere(gdouble** f, GtsCartesianGrid g, guint k, gpointer data)
{
	gdouble x, y, z = g.z;
	guint i, j;

	for(i = 0, x = g.x; i < g.nx; i++, x += g.dx) {
		for(j = 0, y = g.y; j < g.ny; j++, y += g.dy) {
			f[i][j] = x*x + y*y + z*z - 0.8;
		}
	}
}
}

Triangulator::Triangulator()
	: surface(0)
{
}

Triangulator::~Triangulator()
{
	if(surface != 0) {
		gts_object_destroy(reinterpret_cast<GtsObject*>(surface));
	}
}

void Triangulator::triangulate()
{
	// FIXME :o)

	GtsCartesianGrid g;
	g.nx = 10;
	g.ny = 10;
	g.nz = 10;

	g.x = -1.0; g.dx = 2.0/gdouble(g.nx - 1);
	g.y = -1.0; g.dy = 2.0/gdouble(g.ny - 1);
	g.z = -1.0; g.dz = 2.0/gdouble(g.nz - 1);

	surface = gts_surface_new(gts_surface_class(),
				  gts_face_class(),
				  gts_edge_class(),
				  gts_vertex_class());
	gts_isosurface_cartesian(surface, g, sphere, 0, 0.0);
}

void Triangulator::write_gts_file(FILE* file)
{
	if(file == 0) {
		file = stdout;
	}

	gts_surface_write(surface, file);
}

#endif //!HAVE_LIBGTS
