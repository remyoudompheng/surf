/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
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



#include <stdio.h>
#include <math.h>
#include <iostream.h>

#include "Script.h"

#include "float_buffer.h"
#include "DrawFunc.h"
#include "Position.h"
#include "UniVariatePolynom.h"
#include "Hornergroup.h"
#include "DrawfuncData.h"
#include "Clip.h"
#include "gui_config.h"
#include "stop.h"
#include "init_parser.h"
#include "SurfaceDataStruct.h"
#include "SurfaceCalc.h"
#include "resultant.h"

#define DEBUG
#include "debug.h"

static Position* GetPosition ();
static Clip* GetClip (Position*);


// ----------------------------------------------
// sk :Curve Panel data and other datas 
// ----------------------------------------------

static polyxyz draw_func_function_data = NULLPOLYXYZ;  // "curve"
static polyxyz draw_func_plane_data    = NULLPOLYXYZ;  // "plane"

// ----------------------------------------------------------------------------
// --------------- add curves keywords ----------------------------------------
// ----------------------------------------------------------------------------

void draw_func_init_parser( void )
{
	symtab_add_surface_name( "curve", SYM_POLYXYZ, FALSE,
				 &draw_func_function_data);

	symtab_add_surface_name( "plane", SYM_POLYXYZ, FALSE,
				 &draw_func_plane_data);
	
	addCommand ("draw_curve", draw_func_draw);
	addCommand ("cut_with_plane", draw_func_cut);
}

// ----------------------------------------------------------------------------
// ---------------- draw a curve ----------------------------------------------
// ----------------------------------------------------------------------------


void draw_func_draw( void )
{
	const int width = main_width_data;
	const int height = main_height_data;
	
	if (stop)
		return;

	Script::checkVariables();
      
	Position *position = GetPosition();
	
	Clip *clip = GetClip (position );
	
	Polyx::SetStatics( numeric_epsilon_data,
			   numeric_iterations_data,
			   numeric_root_finder_data,
			   true );
	
	HornergroupXY* Curve = new HornergroupXY( &draw_func_function_data, 
						  position );
		
	if( !Curve->getState() ) {
		Script::getBuffer()->Realloc  (width,height);
		Script::getZBuffer()->Realloc (width, height);
		DrawfuncData drawfuncdata( Script::getBuffer(), Script::getZBuffer(),
					   NULL,
					   NULL,
					   Curve,
					   clip,
					   position,
					   curve_width_data );

		drawfuncdata.setGeometry (WindowGeometry(width, height));
		drawfuncdata.SetBorders (0, width, 0, height);

		drawfuncdata.PrintCurve(1);

		drawfuncdata.PrintCurve(0);

		if (Script::getDisplay()) {
			Script::getDisplay()->showColorAreaWindow();
			Script::getDisplay()->setSize (width, height);

			Script::getDisplay()->drawRgbBuffer (*Script::getBuffer(), true, 
							     curve_color_slider_data[red],
							     curve_color_slider_data[green],
							     curve_color_slider_data[blue]);
		} else {
			Script::getBuffer()->AddCurve (curve_color_slider_data[red],
						       curve_color_slider_data[green],
						       curve_color_slider_data[blue]);
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
	if (stop)
		return;
	SurfaceCalc sc;
	sc.setDisplay(Script::getDisplay());
	Script::getZBuffer()->Realloc(main_width_data, main_height_data);


	Position *position = GetPosition ();
	Clip     *clip     = GetClip (position);
	
 	Polyx::SetStatics( numeric_epsilon_data, numeric_iterations_data,
 			   numeric_root_finder_data, TRUE );
	
	RationalHornerXY* Plane = new RationalHornerXY( &draw_func_plane_data,
							position );
	
	bool surfnr_ok = (curve_surface_nr_data >= 1 && curve_surface_nr_data <= MAIN_SURFACE_AMOUNT_NUM) ;
	
	
	if( !Plane->getState() && surfnr_ok ) {
		
		HornergroupXYZ* Surface = new HornergroupXYZ(
			&main_formula_pxyz_data[curve_surface_nr_data],
			sc.sf_ds.getFormula(curve_surface_nr_data),position );
		
		
		if( !Surface->getState() ) {      
			HornergroupXY* Curve = new HornergroupXY( Surface, Plane );
			
				
			if( !Curve->getState() ) {
				DrawfuncData drawfuncdata( Script::getBuffer(), Script::getZBuffer(),
							   Plane,Surface,Curve,
							   clip,position,
							   curve_width_data );
				drawfuncdata.setGeometry(WindowGeometry(main_width_data, main_height_data));
				drawfuncdata.SetBorders(0,main_width_data,0,main_height_data);

				drawfuncdata.PrintCurve(1);
				drawfuncdata.PrintCurve(0);
				
				sc.CalculateCurveOnSurface(0,0,main_width_data,main_height_data,*Script::getBuffer(), *Script::getZBuffer() );
			}
		}
	}
	
	delete position;
	delete clip;
}

Position* GetPosition( void )
{
	int Perspec = ( position_perspective_data
			== position_perspective_central_data );
      
	return new Position( position_numeric,
			     position_sequence_data,
			     Perspec );
}

Clip* GetClip (Position* position )
{
	return Clip::create( position_perspective_data,
			     clip_data,
			     clip_numeric,
			     WindowGeometry (main_width_data, main_height_data),
			     position_numeric.spectator_z,
			     position,
			     0,main_width_data, 
			     0,main_height_data);
}
