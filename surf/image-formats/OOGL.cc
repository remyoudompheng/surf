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

#ifdef HAVE_GTS

#include <OOGL.h>
#include <Triangulator.h>
#include <FileWriter.h>
#include <Triangulator.h>
#include <ScriptVar.h>
#include <IO.h>

namespace ImageFormats {

	OOGL imgFmt_OOGL;

	bool OOGL::save3DImage(const char* filename, Triangulator& data)
	{
		tritor = &data;
		
		FileWriter fw(filename);
		
		if((file = fw.openFile()) == 0) {
		        IO::print_warning("Could not open file for writing.");
			return false;
		}

		GtsSurface* surface = data.getSurface();		
		if(surface == 0) {
			IO::print_warning("There was no triangulated data to save.");
			return false;
		}

		fputs("{\n", file);
		
		// write appearance:
		float r = ScriptVar::color_slider[0].red/255.0;
		float g = ScriptVar::color_slider[0].green/255.0;
		float b = ScriptVar::color_slider[0].blue/255.0;
		float br = ScriptVar::color_slider[0].inside_red/255.0;
		float bg = ScriptVar::color_slider[0].inside_green/255.0;
		float bb = ScriptVar::color_slider[0].inside_blue/255.0;
		int illumination = ScriptVar::light_illumination_data;
		fputs("appearance {\n"
		      "  shading smooth\n"
		      "  material {\n", file);
		if(illumination & ScriptVar::light_illumination_ambient_data) {
			fprintf(file, "    ka %f\n", ScriptVar::light_settings[0].ambient/100.0);
			fprintf(file, "    ambient %f %f %f\n", r, g, b);
		}
		if(illumination & ScriptVar::light_illumination_diffuse_data) {
			fprintf(file, "    kd %f\n", ScriptVar::light_settings[0].diffuse/100.0);
		}
		fprintf(file, "    diffuse %f %f %f\n", r, g, b);
		fprintf(file, "    backdiffuse %f %f %f\n", br, bg, bb);
		if(illumination & ScriptVar::light_illumination_reflected_data) {
			fprintf(file, "    ks %f\n", ScriptVar::light_settings[0].reflected/100.0);
			fputs("    specular 1 1 1\n", file);
		}
		if(illumination & ScriptVar::light_illumination_transmitted_data) {
			fprintf(file, "    alpha %f\n", 1.0 - ScriptVar::light_settings[0].transparence/100.0);
		}
		fputs("  }\n"
		      "  lighting {\n"
		      "    replacelights\n"
		      "    ambient .3 .3 .3\n", file); // hm. should we?

		for(int i = 0; i != LIGHT_SOURCE_MAX_VALUE; i++) {
			light_data_t& l = ScriptVar::light_data[i];
			int vol = l.volume;
			if(vol == 0) {
				continue;
			}
			fputs("    light {\n", file);
			fprintf(file, "      color %f %f %f\n", l.getColorValue(0), l.getColorValue(1), l.getColorValue(2));
			fprintf(file, "      position %f %f %f 0\n", l.position[0], l.position[1], l.position[2]);
			fputs("    }\n", file);
		}
		
		fputs("  }\n"
		      "}\n", file);

		// write vertices/faces:
		fputs("NOFF\n", file);
		fprintf(file, "%d %d %d\n",
			gts_surface_vertex_number(surface),
			gts_surface_face_number(surface),
			gts_surface_edge_number(surface));

		num_vertices = 0;
		data.initNormals();
		gts_surface_foreach_vertex(surface, _vertex_func, this);
		data.deinitNormals();
		
		gts_surface_foreach_face(surface, _face_func, this);

		// end file:
		fputs("}\n", file);

		vertex_map.erase(vertex_map.begin(), vertex_map.end());

		return true;
	}

	void OOGL::vertex_func(GtsVertex* vertex)
	{
		vertex_map[vertex] = num_vertices++;

		Triangulator::Point p = {
			vertex->p.x, vertex->p.y, vertex->p.z
		};
		Triangulator::Point n = tritor->getNormal(p);
		
		fprintf(file, "%f %f %f %f %f %f\n",
			p.x, p.y, p.z, n.x, n.y, n.z);
	}

	void OOGL::face_func(GtsFace* face)
	{
		GtsVertex* v1;
		GtsVertex* v2;
		GtsVertex* v3;
		gts_triangle_vertices(&face->triangle, &v1, &v2, &v3);
		fprintf(file, "3 %d %d %d\n", vertex_map[v1], vertex_map[v2], vertex_map[v3]);
	}

} // namespace ImageFormats

#endif // HAVE_GTS
