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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */

#include <assert.h>

#include "HornergroupXYZ.h"
#include "CanvasDataStruct.h"
#include "Position.h"
#include "Horner.h"

// ----------------------------------------------------------------------------
// class for trivariate horner polynom -- used for surface
// ----------------------------------------------------------------------------

HornergroupXYZ::HornergroupXYZ( polyxyz* Pol, CanvasDataStruct *C_D_S, Position* position )
	: Polynom( new Polyxyz )   // FIXME: when will this be deleted ???
{
	if( C_D_S->initialized ) {
		state = FALSE;
		*Polynom = C_D_S->pDxyz;     // copy polyxyz to Polyxyz
		Derivate[VARIABLE_X] = *Polynom;       // convert to horner
		*Polynom = C_D_S->pxDyz;     // copy polyxyz to Polyxyz
		Derivate[VARIABLE_Y] = *Polynom;       // convert to horner
		*Polynom = C_D_S->pxyz;      // copy polyxyz to Polyxyz
		Orig = *Polynom;            // convert to horner
		// polynom stays to determine cutting curve
	} else {
		*Polynom = *Pol ;      		// copy polyxyz to Polyxyz  
		state = Polynom->Check();
		if( !state ) {       // check for zero degree

			*Polynom = position->Adjust( *Polynom );  // turn, scale and shift
			Polynom->Norm();                          // norm coeffs
			Orig = *Polynom;                         // convert to horner
			for( int dir = 0; dir < 2; dir++ ) {      // do for x and y

				
				Polyxyz Temp = *Polynom;                  // copy to temp
				Temp.Derive( dir );                       // derive
				Derivate[dir] = Temp;                     // convert to horner
			}
		}
	}
}

HornergroupXYZ::~HornergroupXYZ()
{
}

void HornergroupXYZ::SetRow( double val )
{
	Orig.SetRow( val );
	Derivate[VARIABLE_X].SetRow( val );
	Derivate[VARIABLE_Y].SetRow( val );
}

void HornergroupXYZ::SetVar( double val )
{
	Orig.SetVar( val );
	Derivate[VARIABLE_X].SetVar( val );
	Derivate[VARIABLE_Y].SetVar( val );
}

int HornergroupXYZ::Zero( double a, double b, double* c, double* d, int e )
{
	return Orig.Zero( a,b,c,d,e );
}

// ----------------------------------------------------------------------------
// get estimated deltas for next row resp. col
// ----------------------------------------------------------------------------

double HornergroupXYZ::EstimateDelta( int dir, double Root )
{
	assert(dir==0||dir==1);
	double result = Orig.HornerDerived( Root );
	if( result != 0.0 ) {
		double fd = Derivate[dir].Horner( Root );
		result = fd / result;
	}
	
	return result;
}
