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



#include <CanvasDataStruct.h>
#include <ScriptVar.h>

extern double Y_AXIS_LR_ROTATE;

CanvasDataStruct::CanvasDataStruct()
{
	pxyz = NULLPOLYXYZ;
	pDxyz = NULLPOLYXYZ;
	pxDyz = NULLPOLYXYZ;
	
	hpxyz = hpDxyz = hpxDyz = 0;
	initialized = false;
}

CanvasDataStruct::~CanvasDataStruct()
{
	delete_coeff_polyxyz (&pxyz);
	delete_coeff_polyxyz (&pDxyz);
	delete_coeff_polyxyz (&pxDyz);
	
	delete hpxyz;
	delete hpDxyz;
	delete hpxDyz;
}

bool CanvasDataStruct::initWithSurface (int nr)
{
	int i=nr;

	if ( ScriptVar::main_formula_pxyz_data[i].n<=0
	     || ScriptVar::main_formula_pxyz_data[i].deg < 1
	     || ScriptVar::main_formula_pxyz_data[i].deg >= MAX_DEGREE)
		return false;
	
	return initWith_polyxyz (&ScriptVar::main_formula_pxyz_data[i]);
}

bool CanvasDataStruct::initWith_polyxyz (polyxyz *p)
{
        pxyz = polyxyz_copy (p);
		
	for( int ind = 0; ind < POSITION_SEQUENCE_NUM; ind++ )
        {
		switch( ScriptVar::position_sequence_data[ind] ) {
		case 0:
			polyxyz_shift_self (&pxyz,	
					    ScriptVar::position_numeric.orig_x,
					    ScriptVar::position_numeric.orig_y,
					    ScriptVar::position_numeric.orig_z);
			break;
			
		case 1:
			polyxyz_rotate_self (&pxyz,	   		
					     ScriptVar::position_numeric.rot_x,
					     ScriptVar::position_numeric.rot_y,
					     ScriptVar::position_numeric.rot_z);
			break;
			
		case 2:
			polyxyz_scale_self (&pxyz,
					    ScriptVar::position_numeric.scale_x,
					    ScriptVar::position_numeric.scale_y,
					    ScriptVar::position_numeric.scale_z);
			break;
			
		default:
			break;
		}
	}
	
	// --------------------------------------- 
	//  rotate/shift surface for 3D - picture
	// --------------------------------------- 
	
	if( ScriptVar::display_numeric.stereo_eye ) {
		polyxyz_rotate_self( &pxyz,			
				     0.0, Y_AXIS_LR_ROTATE, 0.0 );
	}
	
	// -------------------------------
	//  adjust surface to perspective
	// -------------------------------
	
	if( ScriptVar::position_perspective_data == ScriptVar::position_perspective_central_data ) {
		polyxyz_perspective_self( &pxyz,		
					  ScriptVar::position_numeric.spectator_z);
	}
	
	polyxyz_norm_self( &pxyz );			
	polyxyz_sort     ( &pxyz );				
	
	
	// -------------------------------
	//  calculate surface derivatives
	// -------------------------------
	
	pDxyz = polyxyz_dx ( &pxyz );		
	pxDyz = polyxyz_dy ( &pxyz );		
	
	polyxyz_sort( &pDxyz );				
	polyxyz_sort( &pxDyz );				
	
	
	// -------------------------
	//  calculate horner scheme
	// -------------------------
	
	hpxyz  = new hornerpolyxyz ( pxyz  );	
	hpDxyz = new hornerpolyxyz ( pDxyz );	
	hpxDyz = new hornerpolyxyz ( pxDyz );	
	
	initialized = true;
	return true;
}
