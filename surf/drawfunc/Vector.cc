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

#include "simple.h"

#include "Vector.h"

// ----------------------------------------------------------------------------
// ---------------- normalize vector length -----------------------------------
// ----------------------------------------------------------------------------

void Vector::Unify()
{
	double l = Length();
	if( l > 1e-20 ) 
		*this /= l;
	else 
		l = 0.0;
}

// ----------------------------------------------------------------------------
// ---------------- printout vector -------------------------------------------
// ----------------------------------------------------------------------------

void Vector::Print() const
{
	fprintf(stderr," vector x=%f, y=%f, z=%f, len=%f\n",x,y,z,Length());
}

// ----------------------------------------------------------------------------
// ---------------- vec times vec return double -------------------------------
// ----------------------------------------------------------------------------

double DotProduct(const Vector& V1, const Vector& V2)
{
	return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z; 
}

// ----------------------------------------------------------------------------
// ---------------- return square of vector length ----------------------------
// ----------------------------------------------------------------------------

double SquareLength(const Vector& Vec)
{
	return Vec.x*Vec.x+Vec.y*Vec.y+Vec.z*Vec.z;
}

// ----------------------------------------------------------------------------
// ---------------- return vector with normalized length ----------------------
// ----------------------------------------------------------------------------

Vector Unit(const Vector& V)
{
	Vector Result = V;
	Result.Unify();
	return Result;
}



/* ------------------------------------------------------------------------- */
/*  vectorial comparison:  return  |u-v| < epsilon                           */
/* ------------------------------------------------------------------------- */

int     vector_i_equal( Vector *u,Vector *v )
{
	return  double_equal( u->x,v->x ) &&
		double_equal( u->y,v->y ) &&
		double_equal( u->z,v->z );
}
  
// ----------------------------------------------------------------------------
// --------------- End of file Vector.cc --------------------------------------
// ----------------------------------------------------------------------------
