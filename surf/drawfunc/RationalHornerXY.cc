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

#include <RationalHornerXY.h>
#include <Position.h>

// ----------------------------------------------------------------------------
// class for rational horner polynom -- used for the plane
// ----------------------------------------------------------------------------

RationalHornerXY::RationalHornerXY( polyxyz* Pol, Position* position )
// 	: Poly(new Polyxyz[2]),
// 	  HPoly(new Hornerxy[2]) 
{
	Polyxyz Temp = *Pol;        // copy polyxyz to Polyxyz 
	state = Temp.Check();
	if( !state ) {                    // check for zero degree etc.
	
		Temp = position->Adjust( Temp );  // turn, scale and shift
		// get denominator and nominator of plane polynom solved to z
		Poly[0] = Temp.CounterOfSolve(VARIABLE_Z);  
		Poly[1] = Temp.NominatorOfSolve(VARIABLE_Z);
		// transfer both to horner
		HPoly[0] = Poly[0];
		HPoly[1] = Poly[1];
	}
}

RationalHornerXY::~RationalHornerXY()
{
// 	delete [] Poly;
// 	delete [] HPoly;
}

// ----------------------------------------------------------------------------
// substitute rational polynom for z in surface
// ----------------------------------------------------------------------------

Polyxy* RationalHornerXY::Subst( Polyxyz* Surface ) 
{
	Polyxy* Curve = new Polyxy;        // alloc memory for new polynom
	// and substitute the rational poly plane in surface to get curve
	*Curve = Surface->Subst( Poly[0], Poly[1], VARIABLE_Z );
	return Curve;
}

// ----------------------------------------------------------------------------
// --------------- get z value of a point x y on plane ------------------------
// ----------------------------------------------------------------------------

double RationalHornerXY::ZValue( double x, double y )
{
	// the plane polynom is solved to z 
	// this results (maybe) in a rational polynom in x and y
	HPoly[1].SetVar( x );
	double nom = HPoly[1].Horner( y );
	if( nom != 0.0 ) {
		HPoly[0].SetVar( x );
		double cnt = HPoly[0].Horner( y );
		nom = cnt / nom;
	}
	return nom;
}
