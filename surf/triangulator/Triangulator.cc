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
#include <ScriptVar.h>
#include <Misc.h>
#include <FileWriter.h>

#include <gts.h>

#include<iostream>
#include<cmath>

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
	g.nx = 100;
	g.ny = 100;
	g.nz = 100;

	g.x = -10.0; g.dx = 20.0/gdouble(g.nx - 1);
	g.y = -10.0; g.dy = 20.0/gdouble(g.ny - 1);
	g.z = -10.0; g.dz = 20.0/gdouble(g.nz - 1);

	surface = gts_surface_new(gts_surface_class(),
				  gts_face_class(),
				  gts_edge_class(),
				  gts_vertex_class());
	gts_isosurface_cartesian(surface, g, _iso_func, 0, 0.0);
}

void Triangulator::write_gts_file()
{
	if(ScriptVar::surface_filename_data == 0) {
		Misc::print_warning("No filename given!\n");
		return;
	}
	
	FileWriter fw(ScriptVar::surface_filename_data);
	FILE* file;
		
	if((file = fw.openFile()) == 0) {
		Misc::print_warning("Could not open file for writing.\n");
		return;
	}
	
	if(surface == 0) {
		Misc::print_warning("There was no triangulated data to save.\n");
	} else {
		gts_surface_write(surface, file);
	}
}

void Triangulator::write_data()
{
	vertex_count = 0;

	std::cout << gts_surface_vertex_number(surface) << ' '
		  << gts_surface_face_number(surface) << '\n';
	gts_surface_foreach_vertex(surface, _vertex_func, this);
	gts_surface_foreach_face(surface, _face_func, this);

	vertex_map.erase(vertex_map.begin(), vertex_map.end());
}

void Triangulator::vertex_func(GtsVertex* v)
{
	vertex_count++;
	vertex_map[v] = vertex_count;
	
	GtsPoint& p = v->p;

	float nx, ny, nz;
	calc_normal(p, nx, ny, nz);

	std::cout << p.x << ' ' << p.y << ' ' << p.z << ' '
		  << nx << ' ' << ny << ' ' << nz << '\n';
}

void Triangulator::face_func(GtsFace* f)
{
	GtsTriangle& t = f->triangle;
	GtsSegment& s1 = t.e1->segment;
	GtsSegment& s2 = t.e2->segment;

	GtsVertex* v1 = s1.v1;
	GtsVertex* v2 = s1.v2;
	GtsVertex* v3;
	if(s2.v1 != s1.v1 && s2.v1 != s1.v2) {
		v3 = s2.v1;
	} else {
		v3 = s2.v2;
	}

	std::cout << vertex_map[v1] << ' '
		  << vertex_map[v2] << ' '
		  << vertex_map[v3] << '\n';
}

void Triangulator::iso_func(gdouble** f, GtsCartesianGrid g, guint k)
{
	gdouble x, y, z = g.z;
	guint i, j;

	for(i = 0, x = g.x; i < g.nx; i++, x += g.dx) {
		for(j = 0, y = g.y; j < g.ny; j++, y += g.dy) {
			f[i][j] = x*x + y*y + z*z - 4.0;
		}
	}
}

void Triangulator::calc_normal(const GtsPoint& p, float& x, float& y, float& z)
{
	x = p.x;
	y = p.y;
	z = p.z;
	float len = x*x + y*y + z*z;
	if(len > 0) {
		float sq = std::sqrt(len);
		x /= sq;
		y /= sq;
		z /= sq;
	}
}



#endif //!HAVE_LIBGTS
