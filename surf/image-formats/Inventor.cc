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
#ifdef HAVE_INVENTOR

#include <Inventor.h>
#include <Triangulator.h>
#include <ScriptVar.h>
#include <FileWriter.h>
#include <Misc.h>

#ifdef max
#  undef max
#endif

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/actions/SoWriteAction.h>
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

		SoDB::init();

		SoSeparator* root = new SoSeparator;
		root->ref();

                // specify lights:
		for(int i = 0; i != LIGHT_SOURCE_MAX_VALUE; i++) {
			light_data_t& l = ScriptVar::light_data[i];
			int vol = l.volume;
			if(vol == 0) {
				continue;
			}
			SoPointLight* light = new SoPointLight;
			light->location.setValue(l.position[0], l.position[1], l.position[2]);
			light->intensity = vol/100.0;
			light->color.setValue(l.color[0]/255.0, l.color[1]/255.0, l.color[2]/255.0);
			root->addChild(light);
		}

		// specify material:
		SoMaterial* mat = new SoMaterial;
		float r = ScriptVar::color_slider[0].red/255.0;
		float g = ScriptVar::color_slider[0].green/255.0;
		float b = ScriptVar::color_slider[0].blue/255.0;

		int illumination = ScriptVar::light_illumination_data;
		if(illumination & ScriptVar::light_illumination_ambient_data) {
			mat->ambientColor.setValue(r, g, b);
		}
		if(illumination & ScriptVar::light_illumination_diffuse_data) {
			mat->diffuseColor.setValue(r, g, b);
		}
		if(illumination & ScriptVar::light_illumination_reflected_data) {
			mat->specularColor.setValue(1.0, 1.0, 1.0);
			mat->shininess = ScriptVar::light_settings[0].reflected/100.0;
		}
		if(illumination & ScriptVar::light_illumination_transmitted_data) {
			mat->transparency = ScriptVar::light_settings[0].transparence/100.0;
		}
		root->addChild(mat);

		// specify vertex & normal data:
		GtsSurface* surface = data.getSurface();
		guint num_vertices = gts_surface_vertex_number(surface);
		guint num_faces = gts_surface_face_number(surface);
		
		coords = new SoCoordinate3;
		coords->point.setNum(num_vertices);
		normals = new SoNormal;
		normals->vector.setNum(num_vertices);
		faceSets = new SoIndexedFaceSet;
		faceSets->coordIndex.setNum(4*num_faces);

		vertex_index = 0;
		data.initNormals();
		gts_surface_foreach_vertex(surface, _vertex_func, this);
		data.deinitNormals();

		vertex_index = 0;
		gts_surface_foreach_face(surface, _face_func, this);

		root->addChild(coords);
		root->addChild(normals);
		root->addChild(faceSets);

		// write Inventor file:
		SoWriteAction wa;
		SoOutput* out = wa.getOutput();
		out->setFilePointer(file);
		out->setBinary(true);
		wa.apply(root);
		
		vertex_map.erase(vertex_map.begin(), vertex_map.end());
		
		return true;
	}

	void OpenInventor::vertex_func(GtsVertex* v)
	{
		coords->point.set1Value(vertex_index, v->p.x, v->p.y, v->p.z);
		Triangulator::Point n = tritor->getNormal(v);
		normals->vector.set1Value(vertex_index, n.x, n.y, n.z);

		vertex_map[v] = vertex_index++;
	}

	void OpenInventor::face_func(GtsFace* face)
	{
		GtsVertex* v1;
		GtsVertex* v2;
		GtsVertex* v3;
		gts_triangle_vertices(&face->triangle, &v1, &v2, &v3);
		
		faceSets->coordIndex.set1Value(vertex_index++, vertex_map[v1]);
		faceSets->coordIndex.set1Value(vertex_index++, vertex_map[v2]);
		faceSets->coordIndex.set1Value(vertex_index++, vertex_map[v3]);
		faceSets->coordIndex.set1Value(vertex_index++, -1);
	}

} // namespace ImageFormats

#endif // HAVE_INVENTOR
#endif // HAVE_LIBGTS
