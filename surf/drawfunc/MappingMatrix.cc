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



#include <stdio.h>
#include <math.h>



// ----------------------------------------------------------------------------
// ------------------------- matrix assingments -------------------------------
// ----------------------------------------------------------------------------

#include "MappingMatrix.h"

MappingMatrix& MappingMatrix::operator=(const MappingMatrix &alt)
{
	CopyAll( alt );
	return *this;
}
  
MappingMatrix& MappingMatrix::operator=(const double val)
{ 
	for(int i = 0; i < 4; i++ ) 
		for(int j = 0; j < 4; j++ )
			Entry[i][j] = ( i == j ? val : 0 ); 
	return *this;
}

// ----------------------------------------------------------------------------
// ------------------------- matrix multiplication ----------------------------
// ----------------------------------------------------------------------------

MappingMatrix MappingMatrix::operator*( const MappingMatrix &m ) const
{
	MappingMatrix Result;
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ ) {
			Result.Entry[i][j] = 0;
			for( int k = 0; k < 4; k++ )
				Result.Entry[i][j] += Entry[i][k] * m.Entry[k][j];
		}
	return Result;
}

MappingMatrix& MappingMatrix::operator*=( const MappingMatrix &m )
{
	(*this) = (*this) * m;
	return *this;
}

// ----------------------------------------------------------------------------
// ----------------- turn positioning matrix ----------------------------------
// ----------------------------------------------------------------------------

void MappingMatrix::Turn( double Alpha, int num )
{
	MappingMatrix Mapping = 1.0;

	if( num != 1 )  
		Alpha = -Alpha;       // y turns the other direction

	if( num < 3 ) {
		Mapping.Entry[(num+1)%3][(num+1)%3] = cos( Alpha );
		Mapping.Entry[(num+2)%3][(num+2)%3] = cos( Alpha );
		Mapping.Entry[(num+1)%3][(num+2)%3] = -sin( Alpha );
		Mapping.Entry[(num+2)%3][(num+1)%3] = sin( Alpha );
	}
	*this *= Mapping;
}

// ----------------------------------------------------------------------------
// ----------- shift positioning matrix -- not used ---------------------------
// ----------------------------------------------------------------------------

void MappingMatrix::Translate( double x, double y, double z )
{
	MappingMatrix Mapping(1.0);
	Mapping.Entry[0][3] = -x;
	Mapping.Entry[1][3] = -y;
	Mapping.Entry[2][3] = -z;
	*this *= Mapping;
}

// ----------------------------------------------------------------------------
// ------------ scale positioning matrix --------------------------------------
// ----------------------------------------------------------------------------

void MappingMatrix::Scale( double val, int var )
{
	MappingMatrix Mapping(1.0);
	Mapping.Entry[var][var] = val;
	*this *= Mapping;
}

// ----------------------------------------------------------------------------
// ------------ printout matrix for debug purposes ----------------------------
// ----------------------------------------------------------------------------

void MappingMatrix::Print() const
{
	fprintf(stderr,"Matrix:\n");
	for( int i = 0; i < 4; i++ ) {
		for( int j = 0; j < 4; j++ )
			fprintf(stderr,"%f ",Entry[i][j]);
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");
}

// ----------------------------------------------------------------------------
// -------------------- copy matrix -------------------------------------------
// ----------------------------------------------------------------------------

void MappingMatrix::CopyAll(const MappingMatrix &alt)
{
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ )
			Entry[i][j] = alt.Entry[i][j];
}


// ----------------------------------------------------------------------------
// ----------- create trivariate substitution polynom fo positioning ----------
// ----------------------------------------------------------------------------

Polyxyz MappingMatrix::CreateSubstPolyxyz( int Var ) const
{
	Polyxyz Result(4);
	if( Var >= 0 && Var < 4 ) {
		for( int i = 0; i < 4; i++ )  {
			Result.Monomial[i].SetCoeff( Entry[Var][i] );
			Result.Monomial[i].Exponent( i, 1 );
		}
	}
	Result.Collect();
	return Result;
}

// ----------------------------------------------------------------------------
// ----------- create bivariate substitution polynom fo positioning -----------
// ----------------------------------------------------------------------------

Polyxy MappingMatrix::CreateSubstPolyxy( int Var ) const
{
	Polyxy Result(4);
	if( Var >= 0 && Var < 4 ) {
		for( int i = 0; i < 4; i++ ) { 
			Result.Monomial[i].SetCoeff( Entry[Var][i] ); 
			Result.Monomial[i].Exponent( i, 1 ); 
		}
	}
	Result.Collect();
	return Result;
}

// ----------------------------------------------------------------------------
// ------------ multiplikation matrix times vector ----------------------------
// ----------------------------------------------------------------------------

Vector operator*(const MappingMatrix &M, const Vector &V)
{
	Vector Result;
	Result.x = M.Entry[0][0] * V.x + M.Entry[0][1] * V.y + M.Entry[0][2] * V.z;
	Result.y = M.Entry[1][0] * V.x + M.Entry[1][1] * V.y + M.Entry[1][2] * V.z;
	Result.z = M.Entry[2][0] * V.x + M.Entry[2][1] * V.y + M.Entry[2][2] * V.z;
	return Result;
}

// ----------------------------------------------------------------------------
// ------------ multiplikation vector times matrix ----------------------------
// ----------------------------------------------------------------------------

Vector operator*( const Vector &V, const MappingMatrix &M)
{
	Vector Result;
	Result.x = M.Entry[0][0] * V.x + M.Entry[1][0] * V.y + M.Entry[2][0] * V.z;
	Result.y = M.Entry[0][1] * V.x + M.Entry[1][1] * V.y + M.Entry[2][1] * V.z;
	Result.z = M.Entry[0][2] * V.x + M.Entry[1][2] * V.y + M.Entry[2][2] * V.z;
	return Result;
}

// ----------------------------------------------------------------------------
// ---------------- end of mappingmatrix.cc -----------------------------------
// ----------------------------------------------------------------------------
