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

#include <debug.h>
#include <Script.h>
#include <ScriptVar.h>
#include <float_buffer.h>
#include <DrawFunc.h>
#include <Position.h>
#include <UniVariatePolynom.h>
#include <Hornergroup.h>
#include <DrawfuncData.h>
#include <Clip.h>
#include <init_parser.h>
#include <SurfaceDataStruct.h>
#include <SurfaceCalc.h>
#include <resultant.h>
#include <RgbBuffer.h>

#include<iostream>
#include <stdio.h>
#include <math.h>

namespace {
Position* GetPosition();
Clip* GetClip(Position*);
}

// ----------------------------------------------------------------------------
// ---------------- draw a curve ----------------------------------------------
// ----------------------------------------------------------------------------

void draw_func_draw()
{
	const int width = ScriptVar::main_width_data;
	const int height = ScriptVar::main_height_data;
	
	Script::checkVariables();
      
	Position* position = GetPosition();
	
	Clip *clip = GetClip(position);
	
	Polyx::SetStatics(ScriptVar::numeric_epsilon_data,
			  ScriptVar::numeric_iterations_data,
			  ScriptVar::numeric_root_finder_data,
			  true);
	
	HornergroupXY* Curve = new HornergroupXY(&ScriptVar::draw_func_function_data,
						 position);
		
	if(!Curve->getState()) {
		Script::getZBuffer()->Realloc(width, height);
		DrawfuncData drawfuncdata(Script::getBuffer(), Script::getZBuffer(),
					  0, 0, Curve, clip, position,
					  ScriptVar::curve_width_data);

		drawfuncdata.setGeometry(WindowGeometry(width, height));
		drawfuncdata.SetBorders(0, width, 0, height);

		drawfuncdata.PrintCurve(1);

		drawfuncdata.PrintCurve(0);

		RgbBuffer* buffer = Script::getBuffer();
		buffer->AddCurve(ScriptVar::curve_color_slider_data[red],
				 ScriptVar::curve_color_slider_data[green],
				 ScriptVar::curve_color_slider_data[blue]);

		// output as PPM to stdout if we're in kernel mode
		if(Script::isKernelMode()) {
			Script::ppm_to_stdout();
		}
	}
	
	delete position;
	delete clip;
}

// ----------------------------------------------------------------------------
// ------------- cut with a plane ---------------------------------------------
// sk1:multisurfaces
// sk2:calculate curve, like drawing it on the surface
// ----------------------------------------------------------------------------

void draw_func_cut( void )
{
	SurfaceCalc sc;
	Script::getZBuffer()->Realloc(ScriptVar::main_width_data, ScriptVar::main_height_data);

	Position* position = GetPosition();
	Clip* clip = GetClip(position);
	
 	Polyx::SetStatics(ScriptVar::numeric_epsilon_data, ScriptVar::numeric_iterations_data,
 			  ScriptVar::numeric_root_finder_data, true);
	
	RationalHornerXY* plane = new RationalHornerXY(&ScriptVar::draw_func_plane_data,
						       position);
	
	bool surfnr_ok = (ScriptVar::curve_surface_nr_data >= 1 && ScriptVar::curve_surface_nr_data <= MAIN_SURFACE_AMOUNT_NUM);
	
	
	if(!plane->getState() && surfnr_ok) {
		
		HornergroupXYZ* surface = new HornergroupXYZ(&ScriptVar::main_formula_pxyz_data[ScriptVar::curve_surface_nr_data],
							     sc.sf_ds.getFormula(ScriptVar::curve_surface_nr_data), position);
		
		if(!surface->getState()) {      
			HornergroupXY* curve = new HornergroupXY(surface, plane);

			if(!curve->getState()) {
				const int width = ScriptVar::main_width_data;
				const int height = ScriptVar::main_height_data;
				
				DrawfuncData drawfuncdata(Script::getBuffer(), Script::getZBuffer(),
							  plane, surface, curve,
							  clip, position,
							  ScriptVar::curve_width_data);
				drawfuncdata.setGeometry(WindowGeometry(width, height));
				drawfuncdata.SetBorders(0, width, 0, height);

				drawfuncdata.PrintCurve(1);
				drawfuncdata.PrintCurve(0);
				
				sc.CalculateCurveOnSurface(0, 0, width, height, *Script::getBuffer(), *Script::getZBuffer());
			}
		}
	}
	
	delete position;
	delete clip;
}

namespace {
Position* GetPosition()
{
	int perspec = (ScriptVar::position_perspective_data
		       == ScriptVar::position_perspective_central_data);
      
	return new Position(ScriptVar::position_numeric,
			    ScriptVar::position_sequence_data,
			    perspec);
}

Clip* GetClip(Position* position)
{
	return Clip::create(ScriptVar::position_perspective_data,
			    ScriptVar::clip_data,
			    ScriptVar::clip_numeric,
			    WindowGeometry(ScriptVar::main_width_data, ScriptVar::main_height_data),
			    ScriptVar::position_numeric.spectator_z,
			    position,
			    0, ScriptVar::main_width_data, 
			    0, ScriptVar::main_height_data);
}
}
