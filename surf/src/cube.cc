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




// ===========================================================================
// File      cube.cc
// Author:   Stephan Endrass
// Address:  endrass@mi.uni-erlangen.de
// Date:     2 march 94
// ============================================================================


#include <math.h>


#include "def.h"
#include "simple.h"
#include "monomarith.h"
#include "polyarith.h"
#include "hornerarith.h"
#include "polylexyacc.h"
#include "gui_config.h"
#include "cube.h"

//  using namespace globals;
// -------------
//  global data
// -------------

cube    clip_cube;

// -----------------------------------
//  rotate the cube around the x-axis
// -----------------------------------

void    cube_rotate_x_axis( cube *c,double alpha )
{
	int     i;
	
	double  ca = cos( alpha );
	double  sa = sin( alpha );
	double  b_new, c_new;

	for( i = 0; i < 6; i++ ) {
		b_new =  c->equations[i][1]*ca + c->equations[i][2]*sa;
		c_new = -c->equations[i][1]*sa + c->equations[i][2]*ca;
		
		c->equations[i][1] = b_new;
		c->equations[i][2] = c_new;
	}
}

// ----------------------------------
//  rotate the cube about the y-axis
// ----------------------------------

void    cube_rotate_y_axis( cube *c,double alpha )
{
	int     i;

	double  ca = cos( alpha );
	double  sa = sin( alpha );
	double  a_new, c_new;
	
	for( i = 0; i < 6; i++ ) {
		a_new =  c->equations[i][0]*ca + c->equations[i][2]*sa;
		c_new = -c->equations[i][0]*sa + c->equations[i][2]*ca;
		
		c->equations[i][0] = a_new;
		c->equations[i][2] = c_new;
	}
}

// ----------------------------------
//  rotate the cube about the z-axis
// ----------------------------------

void    cube_rotate_z_axis( cube *c,double alpha )
{
	int     i;

	double  ca = cos( alpha );
	double  sa = sin( alpha );
	double  a_new, b_new;

	for( i = 0; i < 6; i++ ) {
		a_new =  c->equations[i][0]*ca + c->equations[i][1]*sa;
		b_new = -c->equations[i][0]*sa + c->equations[i][1]*ca;
		
		c->equations[i][0] = a_new;
		c->equations[i][1] = b_new;
	}
}

// -----------------------------------
//  initialize from other global data
// -----------------------------------

void    init_cube( cube *c )
{
	// ------------------------
	//  init center and radius
	// ------------------------
	
	c->center_x = clip_numeric.center_x;
	c->center_y = clip_numeric.center_y;
	c->center_z = clip_numeric.center_z;
	c->radius   = clip_numeric.radius;
	
	// ------------------------------------
	//  init planes x = center_x +- radius
	// ------------------------------------
	
	c->equations[0][0] = 1.0;
	c->equations[0][1] = 0.0;
	c->equations[0][2] = 0.0;
	c->equations[0][3] = -( c->center_x + c->radius );
	
	c->equations[1][0] = 1.0;
	c->equations[1][1] = 0.0;
	c->equations[1][2] = 0.0;
	c->equations[1][3] = -( c->center_x - c->radius );
	
	c->equations[2][0] = 0.0;
	c->equations[2][1] = 1.0;
	c->equations[2][2] = 0.0;
	c->equations[2][3] = -( c->center_y + c->radius );
	
	c->equations[3][0] = 0.0;
	c->equations[3][1] = 1.0;
	c->equations[3][2] = 0.0;
	c->equations[3][3] = -( c->center_y - c->radius );
	
	c->equations[4][0] = 0.0;
	c->equations[4][1] = 0.0;
	c->equations[4][2] = 1.0;
	c->equations[4][3] = -( c->center_z + c->radius );
	
	c->equations[5][0] = 0.0;
	c->equations[5][1] = 0.0;
	c->equations[5][2] = 1.0;
	c->equations[5][3] = -( c->center_z - c->radius );
	
	cube_rotate_y_axis( c,-position_numeric.rot_y );
	cube_rotate_x_axis( c,-position_numeric.rot_x );
	cube_rotate_z_axis( c,-position_numeric.rot_z );
}

// ============================================================================
// End of cube.cc
// ============================================================================
