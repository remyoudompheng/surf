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





//-----------------------------------------------------------------------------
//--------------------- UnivariatePolynom.cc ----------------------------------
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <ScriptVar.h>
#include <simple.h>
#include <UniVariatePolynom.h>
#include <Sturmchain.h>
#include <Bezier.h>
#include <monomarith.h>
#include <polyroot.h>

// ----------------------------------------------------------------------------
// --------- definition of class statics --------------------------------------
// ----------------------------------------------------------------------------

double Polyx::Epsilon = 1e-4;           // used for polyx and bezier
int    Polyx::MaxIterations = 25;       // number of iterations for sturmchain
int    Polyx::Algorithm = NUMERIC_ROOT_BEZIER;
                                        // selected rootfinder algorithm
bool   Polyx::Multiflag = true;         // find largest or all roots

// ----------------------------------------------------------------------------
// --------- static function to set static elements (quasi globals) -----------
// ----------------------------------------------------------------------------

void Polyx::SetStatics( double val, int num, int alg, bool mfl )
{
	Epsilon = val;
	MaxIterations = num;
	Algorithm = alg;
	Multiflag = mfl;

	assert(Epsilon > 0);
}

// ----------------------------------------------------------------------------
// ------- polynom assignment -------------------------------------------------
// ----------------------------------------------------------------------------


Polyx& Polyx::operator=(const Polyx &Pol)
{
	delete [] Coeff;
	Coeff = new double[Number = Pol.Number];
	CopyCoeffs( Pol );
	return *this;
}

//-----------------------------------------------------------------------------
//---------polynom modulo division --------------------------------------------
//-----------------------------------------------------------------------------

Polyx Polyx::operator%(const Polyx& Q) const
{
	int j,k, qn = Q.Number - 1;
	Polyx Result = *this;
	for( int i = Number - 1; i >= qn; i-- )
		if( Result.Coeff[i] != 0.0 ) {
			double quot = Result.Coeff[i] / Q.Coeff[qn];
			Result.Coeff[i] = 0.0;
			for( k = i - 1, j = qn - 1; j >= 0; k--, j-- )
				Result.Coeff[k] -= quot * Q.Coeff[j];
		}
	for( Result.Number = qn;
	     Result.Number > 0 && Result.Coeff[Result.Number - 1] == 0.0;
	     Result.Number-- )
		;      // clean up polynom

	return Result;
}


//-----------------------------------------------------------------------------
//------------------------ print univariate polynom ---------------------------
//-----------------------------------------------------------------------------

void Polyx::Print() const
{
	for( int i = 0; i< Number; i++ ) {
		if( i > 0 ) {
			if( Coeff[i] >= 0.0 ) 
				fprintf( stderr, " +" );
			else 
				fprintf( stderr, " " );
		}
		fprintf( stderr, "%.4f*x^%d", Coeff[i], i );
	}
	fprintf( stderr, "\n" );
}

//-----------------------------------------------------------------------------
//---------return derivate of polynom -----------------------------------------
//-----------------------------------------------------------------------------

Polyx Polyx::Derive() const
{
	Polyx Result(Number-1);
	for( int i = 1; i < Number; i++ )
		Result.Coeff[i-1] = Coeff[i] * i;
	return Result;
}

// ----------------------------------------------------------------------------
// --------- set function pointers for zero function --------------------------
// --------- called by the constructors at creation time ----------------------
// --------- decides which rootfinding algorithm is used ----------------------
// --------- and if one or all zeroes in range are needed ---------------------
// ----------------------------------------------------------------------------

void Polyx::SetRootFunctionPointer()
{
	switch( Algorithm ) {
	case NUMERIC_ROOT_D_BISECTION :
		if( Multiflag ) {
			ZeroFunction = &Polyx::ZeroSturmMulti;
		} else {
			ZeroFunction = &Polyx::ZeroSturmSingle;
		}
		OneZeroFunction = &Polyx::ZeroSturmSingle;
		ZeroSturmFunction = &Polyx::Bisect;
		break;

	case NUMERIC_ROOT_D_REGULA :
		if( Multiflag )
			ZeroFunction = &Polyx::ZeroSturmMulti;
		else
			ZeroFunction = &Polyx::ZeroSturmSingle;
		OneZeroFunction = &Polyx::ZeroSturmSingle;
		ZeroSturmFunction = &Polyx::Pegasus;
		break;

	case NUMERIC_ROOT_D_PEGASUS :
		if( Multiflag )
			ZeroFunction = &Polyx::ZeroSturmMulti;
		else
			ZeroFunction = &Polyx::ZeroSturmSingle;
		OneZeroFunction = &Polyx::ZeroSturmSingle;
		ZeroSturmFunction = &Polyx::Pegasus;
		break;

	case NUMERIC_ROOT_D_ANDERSON :
		if( Multiflag )
			ZeroFunction = &Polyx::ZeroSturmMulti;
		else
			ZeroFunction = &Polyx::ZeroSturmSingle;
		OneZeroFunction = &Polyx::ZeroSturmSingle;
		ZeroSturmFunction = &Polyx::Anderson;
		break;

	case NUMERIC_ROOT_D_NEWTON :
		if( Multiflag )
			ZeroFunction = &Polyx::ZeroSturmMulti;
		else
			ZeroFunction = &Polyx::ZeroSturmSingle;
		OneZeroFunction = &Polyx::ZeroSturmSingle;
		ZeroSturmFunction = &Polyx::Anderson;
		break;

	case NUMERIC_ROOT_BEZIER :
		if( Multiflag )
			ZeroFunction = &Polyx::ZeroBezierMulti;
		else
			ZeroFunction = &Polyx::ZeroBezierSingle;
		OneZeroFunction = &Polyx::ZeroBezierSingle;
		ZeroSturmFunction = &Polyx::Bisect; // not used -- dummy
		break;

	default :
		fprintf( stderr, "got strange algorithm number %d\n",Algorithm );
		exit(1);
	}
}

// ----------------------------------------------------------------------------
// ------------------- polyx --- zero --- finders -----------------------------
// ----------------------------------------------------------------------------
// ------------------- zero finder dispatch function --------------------------
// ----------------------------------------------------------------------------
// -------- arguments : maximum and minimun of range to search ----------------
// -------------------- pointer to list of zeros ------------------------------
// -------------------- pointer to list of estimates (or NULL) ----------------
// -------------------- number of estimates -----------------------------------
// ---------- returns : number of zeroes found --------------------------------
// ----------------------------------------------------------------------------

int Polyx::Zero( double zmax, double zmin, double *Root,
		 double *RootLastPixel, int NumberOfRootsLastPixel ) const
{
	if( zmax <= zmin ) 
		return 0;        // return no if zero interval width
	int Degree = Number-1;

	while( Degree >= 0 && Coeff[Degree] == 0.0 )
		Degree--;                          // determine degree

	if( Degree < 3 )                     // use root formulas
		return ZeroSimplePoly( zmin, zmax, Root, Degree );

	return (this->*ZeroFunction)( zmin, zmax, Root,
				      RootLastPixel, NumberOfRootsLastPixel );
}    

int Polyx::OneZero( double zmax, double zmin, double *Root,
		    double *RootLastPixel, int NumberOfRootsLastPixel ) const
{
	if( zmax <= zmin ) 
		return 0;        // return no if zero interval width

	int Degree = Number-1;
	while( Degree >= 0 && Coeff[Degree] == 0.0 )
		Degree--;                          // determine degree

	if( Degree < 3 )                     // use root formulas
		return OneZeroSimplePoly( zmin, zmax, Root, Degree );

	return (this->*OneZeroFunction)( zmin, zmax, Root,
					 RootLastPixel, NumberOfRootsLastPixel );
}    

// ----------------------------------------------------------------------------
//-------------- zero finder for degree less than three -----------------------
// ----------------------------------------------------------------------------

int Polyx::ZeroSimplePoly( const double zmin, const double zmax,
			   double *Root, const int Degree ) const
{
	if( Degree < 0 ) {                             // empty polynom
		Root[0] = zmax; 
		return 1;
	}

	if( Degree == 0 ) 
		return 0;                  // const poly != 0

	if( Degree == 1 || fabs(Coeff[2]) < 1.0e-8 ) {       // zero of line
		
		Root[0] = -Coeff[0]/Coeff[1];
		return ( zmax > Root[0] && zmin < Root[0] ? 1 : 0 );
	}
  
	// now degree = 2, so get zeros of quadratic function
	// if lead coeff is very small, then formula fails...

	double discr = Coeff[1]*Coeff[1] - 4*Coeff[2]*Coeff[0];

	if( discr < 0 ) 
		return 0;

	double t1 = - Coeff[1] / (2.0 * Coeff[2]);
	if( discr == 0 ) {
		Root[0] = t1;
		return ( zmax > Root[0] && zmin < Root[0] ? 1 : 0 );
	}

	// now discriminant > 0
	int result  = 2;
	double t2 = sqrt( discr )/ fabs(2.0*Coeff[2]);
	Root[0] = t1 + t2;
	Root[1] = t1 - t2;
	if( zmax <= Root[1] || zmin >= Root[1] ) 
		result--;
	if( zmax <= Root[0] || zmin >= Root[0] ) { 
		result--; 
		Root[0] = Root[1]; 
	}
	
	if( !Multiflag ) {
		result = MIN( 1 , result);
	}

	return result;
}

int Polyx::OneZeroSimplePoly(const double zmin, const double zmax,
			     double *Root, const int Degree ) const
{
	if( Degree < 0 )                             // empty polynom
		Root[0] = zmax; 
	if( Degree == 0 ) 
		return 0;                  // const poly != 0
	if( Degree == 1 || fabs(Coeff[2]) < 1.0e-8 )       // zero of line
		Root[0] = -Coeff[0]/Coeff[1];
	else {
		// now degree = 2, so get zeros of quadratic function
		double discr = Coeff[1]*Coeff[1] - 4.0*Coeff[2]*Coeff[0];
		
		if( discr < 0.0 ) return 0;
		Root[0] = - Coeff[1] / (2.0 * Coeff[2]);
		if( discr > 0.0 ) {
			// now discriminant > 0, so get larger one
			double t2 = sqrt( discr )/ fabs(2.0*Coeff[2]);
			Root[0] += t2;
			if( zmax <= Root[0] || zmin >= Root[0] )
				Root[0] -= (t2 + t2);
		}
	}
	return ( zmax > Root[0] && zmin < Root[0] ? 1 : 0 );
}

// ----------------------------------------------------------------------------
// --- f u n c t i o n s - t o u c h i n g - b e z i e r - p o l y n o m s ----
// ----------------------------------------------------------------------------
// ------------ get all zeroes in a certain range -----------------------------
// ------------ on one side of zero -------------------------------------------
// ------------ use estimates at oldroot(i) if exist --------------------------
// ----------------------------------------------------------------------------

void Polyx::GetAllZeroesInRange( double Left, double Right,
				 double *Root, int &NumberOfZeroes,
				 double *OldRoot, int NumOld ) const
{
	Bezier Bez( *this, Left, Right ), Sub[2];  // generate bezier poly
	for( int i = 0; i < NumOld; i++ ) {        // go through all estimates

		if( OldRoot[i] >= Left && OldRoot[i] <= Right ) { // if estimate in range
			
			Bez.DivideBezier( Sub, OldRoot[i] ); // subdivide at estimate
			
			// look for zeros above oldroot(i) ( and below oldroot(i-1) )
			// while another root is found, divide poly by (x-root) and search on
			while( Sub[1].LargestRoot( Root, NumberOfZeroes ) )
				Sub[1].DivideDeflateLeft( Root[NumberOfZeroes-1] );

			// search rest ( le..oldroot(i))
			Bez = Sub[0];
		}
	}
	
	while( Bez.LargestRoot( Root, NumberOfZeroes ) )    // do the rest
		Bez.DivideDeflateLeft( Root[NumberOfZeroes-1] );
}

// ----------------------------------------------------------------------------
// ------------ get the largest zero in a certain range -----------------------
// ------------ on one side of zero -------------------------------------------
// ------------ use estimate at oldroot(0) if exists --------------------------
// ----------------------------------------------------------------------------

void Polyx::GetOneZeroInRange( double Left, double Right,
			       double *Root, int &NumberOfZeroes,
			       double *OldRoot, int NumOld ) const
{
	Bezier Bez( *this, Left, Right ), Sub[2];    // generate bezier poly
	if( NumOld == 0
	    || OldRoot[0] <= Left
	    || OldRoot[0] >= Right )
		Bez.LargestRoot( Root, NumberOfZeroes );
	else {
		Bez.DivideBezier( Sub, OldRoot[0] );
		if( !Sub[1].LargestRoot( Root, NumberOfZeroes ) )
			Sub[0].LargestRoot( Root, NumberOfZeroes );
	}
}

// ----------------------------------------------------------------------------
// ------------- zero methods bezier new type ---------------------------------
// ----------------------------------------------------------------------------
// -------------- get all zeros up to degree about 34 -------------------------
// ----- the trick is to split the polynom at 0, get all roots above zero -----
// ----------- swap projective x and w and get the roots below zero -----------
// ------------------- and this way avoid shifting ----------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// --------- get all zeroes in range -- new bezier method ---------------------
// ----------------------------------------------------------------------------

int Polyx::ZeroBezierMulti( double Left, double Right, double *Root,
			    double *OldRoot, int NumOld ) const
{

	int NumberOfZeroes = 0;
	if( Right > 0.0 ) {
		GetAllZeroesInRange( MAX( 0.0, Left ), Right,
				     Root, NumberOfZeroes,
				     OldRoot, NumOld );
	}
	if( Left < 0.0 ) {
		GetAllZeroesInRange( Left, MIN( 0.0, Right ),
				     Root, NumberOfZeroes,
				     OldRoot, NumOld );
	}
	return NumberOfZeroes;
}

// ----------------------------------------------------------------------------
// ---------- get largest zero in new bezier algorithm ------------------------
// ----------------------------------------------------------------------------

int Polyx::ZeroBezierSingle( double Left, double Right, double *Root,
			     double *OldRoot, int NumOld ) const
{
	int NumberOfZeroes = 0;
	if( Right > 0.0 ) {
		GetOneZeroInRange( MAX( 0.0, Left ) , Right,
				   Root, NumberOfZeroes,
				   OldRoot, NumOld );
	}
	if( Left < 0.0 && !NumberOfZeroes ) {
		GetOneZeroInRange( Left, MIN( 0.0, Right ),
				   Root, NumberOfZeroes,
				   OldRoot, NumOld );
	}
	return NumberOfZeroes;
}

// ----------------------------------------------------------------------------
// --------------Zero Methods S t u r m Versions ------------------------------
// ----------------------------------------------------------------------------
// ---------- get all zeroes in range with estimates at rootsold(i) -----------
// ----------------------------------------------------------------------------

int Polyx::ZeroSturmMulti( double Left, double Right, double *Roots,
			   double *RootsOld, int NumberOld ) const
{
  /*
  Sturmchain STC( *this );                      // generate sturmchain
  double b = Right, a = 0.0;
  double ValB = Horner( b ), ValA = 0.0;
  int    SgnChB = STC.SignChange( b, ValB ), SgnChA = 0;
  // signchain returns the number of signchainges of the sturmchain in b
  // second argument (f(b)) is given to save computation
  // first range is oldroot(0)...b

  int NumberOfZeroes = 0;
  for( int i = 0; i < NumberOld; i++ )
    {
      if( RootsOld[i] >= Left && RootsOld[i] <= Right )
	{
	  a = RootsOld[i];
	  ValA = Horner( a );
	  SgnChA = STC.SignChange( a, ValA );
	  STC.DivideIntervalMulti( a, ValA, SgnChA, b, ValB, SgnChB,
				   Roots, NumberOfZeroes, MaxIterations );
	  // search in range oldroot(i)...oldroot(i-1)
	  b = a;
	  ValB = ValA;
	  SgnChB = SgnChA;
	}
    }

  a = Left;
  ValA = Horner( a );
  SgnChA = STC.SignChange( a, ValA );
  STC.DivideIntervalMulti( a, ValA, SgnChA, b, ValB, SgnChB,
			   Roots, NumberOfZeroes, MaxIterations );
  // last range is a...oldroot(n)
  */
  // ----------------------------------
  //  Ok, better use my new method ... 
  // ----------------------------------

	int             NumberOfZeroes;
	polyx           p;
	static  int     Mults[99];  // FIXME: not thread safe...

	p.a = Coeff;		// start of HACK...See ChangeLog entry 1999-02-16
	p.n = Number - 1;
	
	NumberOfZeroes = polyx_all_roots( &p,Left,Right,Roots,Mults );
	p.a = 0;		// end of HACK !!!  

	return  NumberOfZeroes;
}

// ----------------------------------------------------------------------------
// ---------- get largest zero in range with estimate at rootsold(0) ----------
// ----------------------------------------------------------------------------

int Polyx::ZeroSturmSingle( double Left, double Right, double *Roots,
			    double *RootsOld, int NumberOld ) const
{
	Sturmchain STC( *this );
	double a = Left,
		b = Right;
	double ValA = Horner( a ),
		ValB = Horner( b );
	int    SgnChA = STC.SignChange( a, ValA ),
		SgnChB = STC.SignChange( b, ValB );
	int NumberOfZeroes = 0;
	if( NumberOld == 0 ||
	    RootsOld[0] <= Left ||
	    RootsOld[0] >= Right )   // estimate exists and is in range
		STC.DivideIntervalSingle( a, ValA, SgnChA, b, ValB, SgnChB,
					  Roots, NumberOfZeroes, MaxIterations );
	else {
		double m = RootsOld[0];
		double ValM = Horner( m );
		int    SgnChM = STC.SignChange( m, ValM );
		
		// search largest zero in range oldroot(0)...b and if none found
		// search in range a...oldroot(0)
		if( !STC.DivideIntervalSingle( m, ValM, SgnChM, b, ValB, SgnChB,
					       Roots, NumberOfZeroes, MaxIterations ) )
			STC.DivideIntervalSingle( a, ValA, SgnChA, m, ValM, SgnChM,
						  Roots, NumberOfZeroes, MaxIterations );
	}
	return NumberOfZeroes;
}

// ----------------------------------------------------------------------------
// ------root finder primitives for sturm version -----------------------------
// ----------------------------------------------------------------------------

int Polyx::Bisect(double a, double b, double fa, double fb,
		   double *Root, int &NumberOfRoots ) const
{
	double ab = 0.0, fab = 0.0;
	int i = 0;
	// subdivide interval at ab = (middle of interval) until f(ab) close zero
	// or until number of iterations exceeded
	while( fabs( fab = Horner( ab = (a+b)/2.0 ) ) > Epsilon
	       && i++ < MaxIterations ) {
		if( fa * fab < 0.0 ) {
			b = ab;
			fb = fab;
		} else {
			a = ab;
			fa = fab;
		}
	}
	if( i < MaxIterations ) {
		Root[NumberOfRoots++] = ab;
		return true;
	}
	return false;
}

int Polyx::Pegasus(double a, double b, double fa, double fb,
		    double *Root, int &NumberOfRoots ) const
{
	double ab = 0.0, fab = 0.0;
	int i = 0;
	// subdivide interval at ab = zero of line (a,f(a)) to (b,(f(b))
	// until f(ab) close zero or number of iterations exceeded
	while( fabs( fab = Horner( ab = a - fa * ( a - b ) / ( fa - fb ) ) )
	       > Epsilon && i++ < MaxIterations ) {
		if( fa * fab < 0.0 ) { // zero crossing between a and ab
			
			b = a;
			fb = fa;
		}
		else {// zero crossing between ab and b
			
			double g = 1.0 - fab / fa;
			fb *= ( g > 0.0 ? g : 0.5 );
		}
		a = ab;
		fa = fab;
	}

	if( i < MaxIterations ) {
		Root[NumberOfRoots++] = ab;
		return true;
	}
	return false;
}

int Polyx::Anderson(double a, double b, double fa, double fb,
		     double *Root, int &NumberOfRoots ) const
{
	double s = 0.0, ab = 0.0, fab = 0.0;
	int i = 0;
	// see above
	while( fabs( fab = Horner( ab = a - fa / ( s = ( fa - fb ) / ( a - b ) ) ) ) > Epsilon 
	       && i++ < MaxIterations ) {
		if( fa * fab < 0.0 ) {
			b = a;
			fb = fa;
		} else {
			double g = ( fab - fa ) / ( ab - a ) / s;
			fb *= ( g > 0.0 ? g : 0.5 );
		}
		a = ab;
		fa = fab;
	}

	if( i < MaxIterations ) {
		Root[NumberOfRoots++] = ( fabs( fa ) < fabs( fb ) ? a : b  );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
//---------------- end of file UnivariatePolynom.cc ---------------------------
//-----------------------------------------------------------------------------

