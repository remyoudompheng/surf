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
#include <assert.h>

#include "HornergroupXY.h"
#include "HornergroupXYZ.h"
#include "RationalHornerXY.h"

#include "Horner.h"
#include "MultiVariatePolynom.h"
#include "Position.h"

// ----------------------------------------------------------------------------
// class for bivariate horner polynom -- used for curve
// ----------------------------------------------------------------------------

HornergroupXY::HornergroupXY( HornergroupXYZ* Surface, 
			      RationalHornerXY* Plane )
{
	Polyxy* Funct = Plane->Subst( Surface->Polynom );
	state = Funct->Check();

	if( !state ) {
		Funct->Norm();                 // norm coeffs
		Polyxy Temp = *Funct;          // copy to temporary
		Orig[VARIABLE_X] = Temp;       // convert to hornerpoly
		Temp.Derive( VARIABLE_X );     // derive to x
		Derivate[VARIABLE_X] = Temp;   // convert to hornerpoly
		Temp = *Funct;                 // recopy temporary
		Temp.SwapXY();                 // swap x and y
		Orig[VARIABLE_Y] = Temp;       // convert to hornerpoly
		Temp.Derive( VARIABLE_X );     // derive to y since swapped
		Derivate[VARIABLE_Y] = Temp;   // convert to hornerpoly
	}
}

HornergroupXY::HornergroupXY( polyxyz* Pol, Position* position)
{
	Polyxy Funct = *Pol;                     // copy polyxyz to Polyxy 
	state = Funct.Check();
	if( !state ) {                             // check for zero degree etc.
		if (position)
			Funct = position->Adjust( Funct );   // turn, scale and shift
		Funct.Norm();                        // norm coeffs
		Polyxy Temp = Funct;                 // copy to temporary
		Orig[VARIABLE_X] = Temp;             // convert to horner
		Temp.Derive( VARIABLE_X );           // derive to x
		Derivate[VARIABLE_X] = Temp;         // convert to horner
		Temp = Funct;                        // recopy from temp
		Temp.SwapXY();                       // swap x and y
		Orig[VARIABLE_Y] = Temp;             // convert to horner
		Temp.Derive( VARIABLE_X );           // derive to y since swapped
		Derivate[VARIABLE_Y] = Temp;         // convert to horner
	}
}

HornergroupXY::HornergroupXY (Polyxy Funct, Position *pos)
{
	state = Funct.Check();
	if( !state ) {				// check for zero degree etc.
		if (pos)
			Funct=pos->Adjust( Funct );   // turn, scale and shift

		Funct.Norm();                        // norm coeffs
		Polyxy Temp = Funct;                 // copy to temporary
		Orig[VARIABLE_X] = Temp;             // convert to horner
		Temp.Derive( VARIABLE_X );           // derive to x
		Derivate[VARIABLE_X] = Temp;         // convert to horner
		Temp = Funct;                        // recopy from temp
		Temp.SwapXY();                       // swap x and y
		Orig[VARIABLE_Y] = Temp;             // convert to horner
		Temp.Derive( VARIABLE_X );           // derive to y since swapped
		Derivate[VARIABLE_Y] = Temp;         // convert to horner
	}

}
// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------

HornergroupXY::~HornergroupXY()
{
// 	delete [] Orig;
// 	delete [] Derivate;
}

void HornergroupXY::SetVar( int dir, double val )
{
	assert(dir == 0 || dir == 1);

	Orig[dir].SetVar( val );
	Derivate[dir].SetVar( val );
}


int HornergroupXY::Zero( int dir, double a, double b,
			 double* c, double* d, int e )
{
	assert(dir==0 || dir == 1);
	return Orig[dir].Zero( a,b,c,d,e );
}

double HornergroupXY::EstimateDelta( int dir, double val )
{
	assert(dir == 0 || dir == 1);

	double res = Orig[dir].HornerDerived( val );
	if( res != 0.0 ) {
		double tmp = Derivate[dir].Horner( val );
		res = tmp / res;
	}
  
	return res;
}
