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



#ifndef BEZIER_H
#define BEZIER_H

#include "UniVariatePolynom.h"

//---------------- bezier polynom class derived from polyx -------------------

class Bezier : public Polyx 
{
	double Left;
	double Right;
	// ---------------- internal used functions --------------------------------

	void    DivideBezier( Bezier*, const double, const double ) const;
	// de casteljau

	int     PolygonCrossing(double&) const;
	// return crossing of bezier polygon in double

	void    DivideLeft( const double );  // divide interval and keep left part
	void    DivideRight( const double ); // divide interval and keep right part

public:
	Bezier()                    : Polyx(), Left(0), Right(0)
		{}
	Bezier( const Bezier &Bez ) : Polyx(Bez), Left(Bez.Left), Right(Bez.Right)
		{}

	Bezier( const Polyx&, double, double );
  
	Bezier& operator=( const Bezier &Bez )
		{
			delete [] Coeff; Coeff = new double[Number=Bez.Number]; 
			CopyCoeffs( Bez ); Left = Bez.Left; Right = Bez.Right; return *this;
		}

	double  GetLeft() const { return Left; }
	
	int     LargestRoot( double* Roots, int& NumberOfRoots)    const;

	void    DivideBezier( Bezier*, const double  ) const;         // de casteljau
	void    DivideDeflateLeft( const double );
	// de casteljau with integrated deflation
};

#endif // BEZIER_H
