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





#ifndef MAPMAT_H
#define MAPMAT_H

// ----------------------------------------------------------------------------
// --------------- class for positioning matrix -------------------------------
// ----------------------------------------------------------------------------

#include "MultiVariatePolynom.h"
#include "Vector.h"

class MappingMatrix
{
public:
	MappingMatrix()                   {}         
	MappingMatrix( const MappingMatrix &alt )  { CopyAll( alt ); }
	MappingMatrix( const double diagval )			// generate diagonal matrix of dval
		{
			for(int i = 0; i < 4; i++ ) 
				for(int j = 0; j < 4; j++ )
					Entry[i][j] = ( i == j ? diagval : 0 ); 
		}
	
	// --------------- operators ------------------------------------------------
	
	MappingMatrix& operator=(const MappingMatrix &alt);		// copy matrix
	MappingMatrix& operator=(const double val);		// double to diagonal matrix of dval
	
	MappingMatrix  operator*(const MappingMatrix &alt) const;	// matrix multiplication
	MappingMatrix& operator*=( const MappingMatrix &m );     
	
	// ---------------- methods -------------------------------------------------
	
	void Turn( double, int );			// turn positioning matrix
	void Translate( double, double, double );	// shift positioning matrix
	void Scale( double, int );			// scale positioning matrix
	void CopyAll(const MappingMatrix &alt);		// copy entries
	
	Polyxyz CreateSubstPolyxyz( int Var ) const;	// generate substitution poly
	Polyxy CreateSubstPolyxy( int Var ) const;	// generate substitution poly
	void Print() const;				// printout for debugging
	

	friend Vector operator*(const MappingMatrix&, const Vector& );		// mat times vec
	friend Vector operator*(const Vector&, const MappingMatrix& );		// vec times mat

private:
	double Entry[4][4];

};

// ----------------------------------------------------------------------------

#endif // MAPMAT_H
