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

#include "def.h"
#include "simple.h"
#include "monomarith.h"
#include "polyarith.h"
#include "hornerarith.h"
#include "roots.h"
#include "polylexyacc.h"
#include "color.h"               //sk24
#include "gui_enum.h"
#include "MultiVariatePolynom.h"
#include "Vector.h"
#include "MappingMatrix.h"
#include "Position.h"

extern double Y_AXIS_LR_ROTATE;
extern double  display_numeric_data[DISPLAY_NUMERIC_NUM];

// ----------------------------------------------------------------------------
// -------------- constructor of positioning class ----------------------------
// ----------------------------------------------------------------------------

Position::Position( position_numeric_t Data, int *PRSD, int PRSP )
	: Turn(Data.rot_x,
	       Data.rot_y,
	       Data.rot_z),
	  Scale(Data.scale_x,
		Data.scale_y,
		Data.scale_z),
	  Shift(Data.orig_x,
		Data.orig_y,
		Data.orig_z),
	  SpecZ( Data.spectator_z),
	  Sequence( PRSD ),
	  Perspective( PRSP ),

	  PosXY (1.0),
	  PosXYZ (1.0),
	  Rotation (1.0)
{
	Rotation.Turn( Turn.y, 1 );
	Rotation.Turn( Turn.x, 0 );
	Rotation.Turn( Turn.z, 2 );

	for( int i = 0; i < 3; i++ ) {
		switch( Sequence[i] ) {
		case 0:
			PosXY.Translate( Shift.x,Shift.y,0.0 );
			PosXYZ.Translate( Shift.x,Shift.y,Shift.z );
			break;

		case 1:
			PosXY.Turn( Turn.z, 2 );
			PosXYZ.Turn( Turn.y, 1 );
			PosXYZ.Turn( Turn.x, 0 );
			PosXYZ.Turn( Turn.z, 2 );
			break;

		case 2:
			PosXY.Scale( Scale.x, 0 );
			PosXY.Scale( Scale.y, 1 );
			PosXYZ.Scale( Scale.x, 0 );
			PosXYZ.Scale( Scale.y, 1 );
			PosXYZ.Scale( Scale.z, 2 );
			break;

		default:
			break;
		}
	}
	
	if( display_numeric.stereo_eye) {
		PosXYZ.Turn( Y_AXIS_LR_ROTATE,1 );
	}
}

// ----------------------------------------------------------------------------
// ------------------ destructor -- cleanup -----------------------------------
// ----------------------------------------------------------------------------

Position::~Position()
{
}

// ----------------------------------------------------------------------------
// ------------- position a bivariate polynom ---------------------------------
// ----------------------------------------------------------------------------

Polyxy Position::Adjust( const Polyxy &Pol )
{
	Polyxy Result = Pol;
	Polyxy SubPol[3];

	for( int i = 0; i < 2; i++ ) {
		SubPol[i] = PosXY.CreateSubstPolyxy( i );
	}
	SubPol[2] = 1.0;
	Result.Subst( SubPol );

	return Result;
}

// ----------------------------------------------------------------------------
// ------------- position a trivariate polynom --------------------------------
// ----------------------------------------------------------------------------

Polyxyz Position::Adjust( const Polyxyz &Pol )
{
	Polyxyz Result = Pol;
	Polyxyz SubPol[3];

	for( int i = 0; i < 3; i++ ) {
		SubPol[i] = PosXYZ.CreateSubstPolyxyz( i );
	}
	Result.Subst( SubPol );

	if( Perspective ) {
		Result.Perspective( SpecZ );
	}

	return Result;
}

// ----------------------------------------------------------------------------
// ------------- position a vector rotate and scale if needed -----------------
// ----------------------------------------------------------------------------

Vector  Position::Adjust( const Vector &Vec )
{ 
	Vector Result = Vec;
	if( Sequence[1] == 0 ) {
		Result = Vec * Rotation;
	}
	Result.Scale( Scale );
	return Result;
}

// ----------------------------------------------------------------------------
// --------------- position a vector -- rotate only ---------------------------
// ----------------------------------------------------------------------------

Vector  Position::Rotate( const Vector &Vec )
{
	return Vec * Rotation;
}

// ----------------------------------------------------------------------------
// -------------- reverse rotation and scaling --------------------------------
// ----------------------------------------------------------------------------

Vector  Position::ReTransform( const Vector &Vec )
{
	Vector Result = Vec;
	Result.InvScale( Scale );

	if( Sequence[1] == 0 ) {
		Result = Rotation * Result;
	}
	return Result;
}

// ----------------------------------------------------------------------------
// ---------------- end of position.cc ----------------------------------------
// ----------------------------------------------------------------------------
