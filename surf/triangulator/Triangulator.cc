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
#include <Script.h>
#include <Misc.h>
#include <FileWriter.h>
#include <polyarith.h>

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
	polyxyz f = polyxyz_copy(&ScriptVar::main_formula_pxyz_data[0]);
	polyxyz_sort(&f);
	hf = new hornerpolyxyz(f);
	
	GtsCartesianGrid g;
	g.nx = 64;
	g.ny = 64;
	g.nz = 64;

	g.x = -10.0; g.dx = 20.0/gdouble(g.nx - 1);
	g.y = -10.0; g.dy = 20.0/gdouble(g.ny - 1);
	g.z = -10.0; g.dz = 20.0/gdouble(g.nz - 1);

	surface = gts_surface_new(gts_surface_class(),
				  gts_face_class(),
				  gts_edge_class(),
				  gts_vertex_class());
	gts_isosurface_cartesian(surface, g, _iso_func, this, 0.0);
	gdouble maxcost = 0.1;
	gts_surface_coarsen(surface,
			    0, 0,
			    0, 0,
			    gts_coarsen_stop_cost, &maxcost, 0);

	delete hf;

	if(Script::isKernelMode()) {
		write_data();
	}
}

void Triangulator::write_data()
{
	std::cout << "triangulate_surface\n";
	std::cout.flush();
	
	polyxyz f = polyxyz_copy(&ScriptVar::main_formula_pxyz_data[0]);
	polyxyz_sort(&f);
	polyxyz dx = polyxyz_dx(&f);
	polyxyz_sort(&dx);
	hdx = new hornerpolyxyz(dx);
	polyxyz dy = polyxyz_dy(&f);
	polyxyz_sort(&dy);
	hdy = new hornerpolyxyz(dy);
	polyxyz dz = polyxyz_dz(&f);
	polyxyz_sort(&dz);
	hdz = new hornerpolyxyz(dz);

	vertex_count = 0;

	std::cout << gts_surface_vertex_number(surface) << ' '
		  << gts_surface_face_number(surface) << '\n';
	gts_surface_foreach_vertex(surface, _vertex_func, this);
	gts_surface_foreach_face(surface, _face_func, this);

	vertex_map.erase(vertex_map.begin(), vertex_map.end());

	std::cout << "end\n";
	std::cout.flush();

	delete hdx;
	delete hdy;
	delete hdz;
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
	gdouble z = g.z;

	gdouble y = g.y;
	for(guint i = 0; i < g.ny; i++) {
		hf->setRow(y);

		gdouble x = g.x;
		for(guint j = 0; j < g.nx; j++) {
			hf->setColumn(x);

			f[j][i] = hf->pZ.horner(z);

			x += g.dx;
		}
		
		y += g.dy;
	}
}

void Triangulator::calc_normal(const GtsPoint& p, float& x, float& y, float& z)
{
	hdx->setRow(p.y);
	hdx->setColumn(p.x);
	x = hdx->pZ.horner(p.z);
	hdy->setRow(p.y);
	hdy->setColumn(p.x);
	y = hdy->pZ.horner(p.z);
	hdz->setRow(p.y);
	hdz->setColumn(p.x);
	z = hdz->pZ.horner(p.z);

	// normalize:
	float len = x*x + y*y + z*z;
	if(len > 0) {
		float sq = std::sqrt(len);
		x /= sq;
		y /= sq;
		z /= sq;
	}
}



#endif //!HAVE_LIBGTS
