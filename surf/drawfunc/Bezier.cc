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

#include <Bezier.h>
#include <def.h>
#include <simple.h>
#include <UniVariatePolynom.h>

#include<cstdio>
#include<cmath>

//-----------------------------------------------------------------------------
//-------------------- bezier constructor -------------------------------------
//-------------------- rockwood : scale and shift interval to 0...1 -----------
//------------------------------- and do bernstein transformation -------------
//-------------------- new : scale coefficients and subdivide at outer border -
//-----------------------------------------------------------------------------


Bezier::Bezier( const Polyx &Polynom, double le, double ri ) 
	: Polyx(Polynom.GetNumber()), Left(le), Right(ri)  // set interval borders
{
	int num = Number - 1, l;
	double *cf = Coeff, *cf2 = Polynom.GetCoeff();
	if( le >= 0.0 ) {
		for( l = 0; l < Number; l++, cf++, cf2++ )
			*cf = *cf2 / binom_coeff( num, l );   // scale coeffs
      
		DivideLeft(1.0, ri);   // subdivide at r and keep left
		if(le > 0.0) {                      // subdivide at l and keep right
			DivideRight((ri - le)/ri);
		}
	} else {
		double factor = ( (Number % 2) ? 1.0 : -1.0);
		for( l = 0, cf2 += num;
		     l < Number;
		     cf++, l++, cf2--, factor = -factor )
			*cf = *cf2 * factor / binom_coeff( num, l ); 
		// scale coeffs
		// store in reverse order and multiply 
		// with (-1)^(n-i) to swap x and w
		
		DivideRight(-le, 1.0); // subdivide at l and keep right
		if(ri < 0.0) {                    // subdivide at r and keep left
			DivideLeft((le - ri)/le);
		}
		if(ri > 0.0) {
			DivideLeft((le -ri)/le);
		}
		
	}
}

//-----------------------------------------------------------------------------
//--------------- bezier element functions ------------------------------------
//--------------- get largest zero of bezier ----------------------------------
//--------------- algorithm : get zero of polygon and subdivide if ------------
//--------------------------- distance to interval borders > epsilon ----------
//-----------------------------------------------------------------------------

int Bezier::LargestRoot( double* Roots, int& NumberOfRoots) const
{
	double width = Right - Left, sadd = 0.0, tt = 0.0, r = 0.0;
	if( width <= 0.0 || !PolygonCrossing( sadd ) ) {
		return false;
	}

	// return no if zero interval width or no crossing
  
	tt = sadd;
	r = (1 - tt)*Left + tt*Right;

	if( r - Left < Epsilon || Right - r < Epsilon ) {
		// if close enough to interval borders
		if( NumberOfRoots == 0 || fabs( Roots[NumberOfRoots-1] - r ) > 1.0e-7 ) {
			Roots[NumberOfRoots++] = r;
		}
		return true;
	}

	Bezier Sub[2];                    // subdivide interval at polygon crossing
	DivideBezier( Sub, sadd, r );     // de casteljau
                                    // search upper and then lower subinterval

	return ( Sub[1].LargestRoot( Roots, NumberOfRoots ) ? true :
		 Sub[0].LargestRoot( Roots, NumberOfRoots ) );
}

// ----------------------------------------------------------------------------
// ------------ subdivision of a bezier polynom -------------------------------
// ------------ puts lower part of interval in field(0) -----------------------
// ------------ and upper part in field(1) ------------------------------------
// ----------------------------------------------------------------------------

void Bezier::DivideBezier( Bezier *Field, const double s ) const
{
	if(Right > Left) {
		DivideBezier(Field, (s - Left)/(Right - Left), s);
	}	
}

// ----------------------------------------------------------------------------
// -------------- the new bezier algorithm generates its own t ----------------
// ----------------------------------------------------------------------------

void Bezier::DivideBezier( Bezier *Field, const double t, const double x )const
{
	int n = Number - 1;
	Bezier *Field1 = Field + 1;
	*Field1 = *Field = *this;
	if( Right != Left ) {
		double emt = 1.0 - t;
		double *cll = Field->Coeff + n, 
			*l1c = Field1->Coeff + n - 1,
			*fc, *cf, *cfm1;
		for( fc = Field->Coeff + 1; fc <= cll; fc++, l1c-- ) {
			for( cf = cll, cfm1 = cf - 1; cf >= fc; cf--, cfm1-- )
				*cf = *cf * t + *cfm1 * emt;
			*l1c = *cll;                          // throw out knot of right part
		}
		Field->Right = Field1->Left = x;          // adjust borders
	}
}

// ----------------------------------------------------------------------------
// ---------- bezier subdivision with integrated deflation --------------------
// ---------- after the subdivision keep the left part ------------------------
// ---------- which then has a zero at t = 1 ----------------------------------
// ----------------------------------------------------------------------------
// ---------- version for rockwood method -------------------------------------
// ----------------------------------------------------------------------------

void Bezier::DivideDeflateLeft( const double s )
{
	if ( Right > Left ) {
		double t = (Right - s)/(Right - Left);
		
		DivideLeft( t );       // subdivide at t and keep left
		int i = --Number;      // decrease number and do deflation at t = 1
		double *cll = Coeff + i;
		for( double *cf = Coeff; cf < cll; cf++, i-- )
			*cf *= ((double)Number/(double)i);
		Right = s;
	}
}

// ----------------------------------------------------------------------------
// ------ de casteljau algorithms ---------------------------------------------
// ----------------------------------------------------------------------------
// ------ subdivide and keep left part of interval ----------------------------
// ----------------------------------------------------------------------------

void Bezier::DivideLeft( const double t )
{
	double emt = 1.0 - t;
	double *cll = Coeff + Number - 1, *cl, *cf, *cf2;
	for( cl = Coeff + 1; cl <= cll; cl++ )
		for( cf = cll, cf2 = cf - 1; cf >= cl; cf--, cf2-- )
			*cf = *cf * emt + *cf2 * t;
}

void Bezier::DivideLeft(const double t1, const double t2)
{
	double* cll = Coeff + Number - 1;
	for(double* cl = Coeff + 1; cl <= cll; cl++) {
		for(double* cf = cll, *cf2 = cf - 1; cf >= cl; cf--, cf2--) {
			*cf = *cf2 * t1 + *cf * t2;
		}
	}
}

// ----------------------------------------------------------------------------
// ------ subdivide and keep right part of interval ---------------------------
// ----------------------------------------------------------------------------

void Bezier::DivideRight( const double t )
{
	double emt = 1.0 - t;
	double *cll = Coeff + Number - 1, *cl, *cf, *cf2;
	for( cl = cll - 1; cl >= Coeff; cl-- )
		for( cf = Coeff, cf2 = cf + 1; cf <= cl; cf++, cf2++ )
			*cf = *cf * t + *cf2 * emt;
}

void Bezier::DivideRight(const double t1, const double t2)
{
	double* cll = Coeff + Number - 1;
	for(double* cl = cll - 1; cl >= Coeff; cl--) {
		for(double* cf = Coeff, *cf2 = cf + 1; cf <= cl; cf++, cf2++) {
			*cf = *cf * t1 + *cf2 * t2;
		}
	}
}

// ----------------------------------------------------------------------------
// -------- get crossing of bezier polygon ------------------------------------
// ----------------------------------------------------------------------------


int Bezier::PolygonCrossing(double &s) const
{
	int found = Number - 1;
	double *cf = Coeff + found, *cf2 = cf - 1;

	while(found && *cf * (*cf2) >= 0.0) {
		found--, cf--, cf2--;                   // find crossing descending
	}
	
	if( !found ) {                             // no crossing in interval
		return false;
	}

	s = (double)(found-1);                    // crossing at left knot
	double diff  = ( *cf - *cf2 );            // knot difference
	if( diff != 0.0 )
		s += fabs( *cf2 / diff );               // crossing at crossing of polygon

	s /= (double)(Number - 1);                // rescale in interval
	
	return true;
}


// ----------------------------------------------------------------------------
// ---------------- end of file bezier.cc -------------------------------------
// ----------------------------------------------------------------------------
