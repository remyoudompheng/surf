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

#include <Inventor.h>
#include <Triangulator.h>
#include <ScriptVar.h>
#include <FileWriter.h>
#include <Misc.h>

#include <gts.h>

namespace ImageFormats {

	OpenInventor imgFmt_Inventor;

	bool OpenInventor::save3DImage(const char* filename, Triangulator& data)
	{
		tritor = &data;
		
		FileWriter fw(filename);
		FILE* file;
		
		if((file = fw.openFile()) == 0) {
		        Misc::print_warning("Could not open file for writing.\n");
			return false;
		}

		sstr << "#Inventor V2.1 ascii\n"
                        "\n"
                        "Separator {\n";

                // specify lights:
		for(int i = 0; i != LIGHT_SOURCE_MAX_VALUE; i++) {
			light_data_t& l = ScriptVar::light_data[i];
			int vol = l.volume;
			if(vol == 0) {
				continue;
			}
			sstr << "    PointLight {\n"
                                      "\tintensity\t" << vol/100.0 << "\n"
				      "\tcolor\t" << l.color[0]/255.0 << ' ' << l.color[1]/255.0 << ' ' << l.color[2]/255.0 << "\n"
                                      "\tlocation\t" << l.position[0] << ' ' << l.position[1] << ' ' << l.position[2] << "\n"
                                "    }\n";
		}

		// specify material:
		sstr << "    Material {\n";
		float r = ScriptVar::color_slider[0].red/255.0;
		float g = ScriptVar::color_slider[0].green/255.0;
		float b = ScriptVar::color_slider[0].blue/255.0;
		int illumination = ScriptVar::light_illumination_data;
		if(illumination & ScriptVar::light_illumination_ambient_data) {
			sstr << "\tambientColor\t" << r << ' ' << g << ' ' << b << '\n';
		}
		if(illumination & ScriptVar::light_illumination_diffuse_data) {
			sstr << "\tdiffuseColor\t" << r << ' ' << g << ' ' << b << '\n';
		}
		if(illumination & ScriptVar::light_illumination_reflected_data) {
			sstr << "\tspecularColor\t1 1 1\n"
                                "\tshininess\t" << ScriptVar::light_settings[0].reflected/100.0 << '\n';
		}
		if(illumination & ScriptVar::light_illumination_transmitted_data) {
			sstr << "\ttransparency\t" << ScriptVar::light_settings[0].transparence/100.0 << '\n';
		}
		sstr << "    }\n";

		// specify vertex & normal data:
		sstr << "    Coordinate3 {\n"
                              "\tpoint [\n";
		GtsSurface* surface = data.getSurface();

		num_vertices = gts_surface_vertex_number(surface);
		num_faces = gts_surface_face_number(surface);
		
		vertex_index = 0;
		gts_surface_foreach_vertex(surface, _vertex_func, this);
		sstr <<       "\t]\n"
                        "    }\n"
                        "    Normal {\n"
                              "\tvector [\n";
		
		vertex_index = 0;
		data.initNormals();
		gts_surface_foreach_vertex(surface, _normal_func, this);
		data.deinitNormals();
		sstr <<       "\t]\n"
                        "    }\n"
                        "    IndexedFaceSet {\n"
			      "\tcoordIndex [\n";

		vertex_index = 0;
		gts_surface_foreach_face(surface, _face_func, this);
                sstr <<       "\t]\n"
                        "    }\n"
                        "}\n";

		std::string s = sstr.str();
		fwrite(s.c_str(), s.length(), 1, file);
		
		vertex_map.erase(vertex_map.begin(), vertex_map.end());
		
		return true;
	}

	void OpenInventor::vertex_func(GtsVertex* v)
	{
		sstr << "\t\t" << v->p.x << ' ' << v->p.y << ' ' << v->p.z;
		vertex_map[v] = vertex_index++;
		if(vertex_index != num_vertices) {
			sstr << ',';
		}
		sstr << '\n';
	}

	void OpenInventor::normal_func(GtsVertex* v)
	{
		Triangulator::Point n = tritor->getNormal(v);
		sstr << "\t\t" << n.x << ' ' << n.y << ' ' << n.z;

		vertex_index++;
		if(vertex_index != num_vertices) {
			sstr << ',';
		}
		sstr << '\n';
	}

	void OpenInventor::face_func(GtsFace* face)
	{
		GtsVertex* v1;
		GtsVertex* v2;
		GtsVertex* v3;
		gts_triangle_vertices(&face->triangle, &v1, &v2, &v3);
		sstr << "\t\t"
		     << vertex_map[v1] << ", "
		     << vertex_map[v2] << ", "
		     << vertex_map[v3] << ", -1";

		vertex_index++;
		if(vertex_index != num_faces) {
			sstr << ',';
		}
		sstr << '\n';
	}

} // namespace ImageFormats

#endif // HAVE_GTS
