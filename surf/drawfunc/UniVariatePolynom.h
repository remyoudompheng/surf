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





//-----------------------------------------------------------------------------
//------------------- UnivariatePolynom.H -------------------------------------
//-----------------------------------------------------------------------------

#ifndef _POLYX_H
#define _POLYX_H

#include <stdio.h>

class Polyx
{
private:
	int (Polyx::*ZeroFunction)( double, double, double*, double*, int ) const;
        int (Polyx::*ZeroSturmFunction)(double, double, double, double,
					  double*, int& ) const;
	int (Polyx::*OneZeroFunction)( double, double, double*,
					 double*, int ) const;
	// --------------- rootfinders ----------------------------------------------
	int   ZeroSimplePoly( const double, const double, double*, const int ) const;
	int   OneZeroSimplePoly( const double, const double,
				 double*, const int ) const;

	// ------------- group one ---- sturmchain version --------------------------
        int ZeroSturmMulti( double, double, double*, double*, int ) const;
	int  ZeroSturmSingle( double, double, double*, double*, int ) const;

	// ------------- used by group two ------------------------------------------
	void  GetAllZeroesInRange( double,double, double*,int&, double*, int ) const;
	void  GetOneZeroInRange( double, double, double*, int&, double*, int ) const;

	// ------------- group two ---- new bezier version --------------------------
	int   ZeroBezierMulti( double, double, double*, double*, int ) const;
	int   ZeroBezierSingle( double, double, double*, double*, int ) const;

	// ------------- sturmchain root finders ------------------------------------
	int Bisect(double,double,double,double,double*,int&) const;
	int Pegasus(double,double,double,double,double*,int&) const;
	int Anderson(double,double,double,double,double*,int&) const;
	
	// ------------- choose root finding algorithm ------------------------------
	void  SetRootFunctionPointer();
	// sets the root function at creation time of poly

protected:
	static  double Epsilon;
	static  int    MaxIterations;
	static  int    Algorithm;
	static  bool   Multiflag;

	int     Number;           // number of coeffs
	double  *Coeff;           // coeff list, in bezier used as knot list
	// n.b.: a poly like ax^n needs n+1 coeffs i.e. 0...n

public:
	Polyx () : Number(0), Coeff(NULL)				// empty polynom
		{ SetRootFunctionPointer(); }

	Polyx ( const double val ): Number(1), Coeff(new double[1])	// convert double to polynom
		{
			Coeff[0] = val;
			SetRootFunctionPointer();
		}

	Polyx ( const int num ) : Number(num), Coeff(new double[num])	// polynom with num coeffs all eq zero
		{
			NullCoeffs();
			SetRootFunctionPointer();
		}

	Polyx ( const Polyx &Pol ) : Number(Pol.Number), Coeff(new double[Pol.Number])	// copy polynom
		{
			CopyCoeffs( Pol );
			SetRootFunctionPointer();
		}

	~Polyx() { delete [] Coeff; }    // clean up

	// ----------- static function to set general properties --------------------

	static void SetStatics( double epsilon, int maxIterations, int algorithm, bool multiroot );

	// ------------------ operators needed --------------------------------------
  
	Polyx&  operator=(const Polyx& );       // copy polyx
	Polyx   operator%(const Polyx& ) const; // polyx modulo division
	Polyx   operator-() const               // unary minus -- mirror at zero
		{
			Polyx Pol(*this);
			for( int i = 0; i < Number; i++ )
				if( i&1 )
					Pol.Coeff[i] = -Pol.Coeff[i];
			return Pol;
		}

	// ------------  simple element functions (inline) --------------------------
	
	void    CopyCoeffs( const Polyx &Pol )
		{ for( int i = 0; i < Number; i++ ) Coeff[i] = Pol.Coeff[i]; }
	double  Horner( const double var ) const // returns f(var)
		{
			double result = 0.0;
			for( int i = Number-1; i >= 0; i-- )
				result = result * var + Coeff[i];
			return result;
		}
	double  HornerDerived(const double var) const // returns f'(var)
		{
			double result = 0.0;
			for( int i = Number-1; i > 0; i--)
				result = result * var + Coeff[i]*i;
			return result;
		}
	void    NegateCoeffs()                        // used to generate sturmchain
		{ for( int i = 0; i < Number; i++ ) Coeff[i] = -Coeff[i]; }
	void    NewCoeff( const int num )             // realloc coeffs
		{
			delete [] Coeff; Number = num; Coeff = new double [Number];
			NullCoeffs();
		}
	void    NullCoeffs()                          // set all coeffs eq 0
		{ for( int i = 0; i < Number; i++ ) Coeff[i] = 0.0; }
	
	// integrated : sets coeff number num with the value of arg2 at var
	void    SetHornered( const int num, const Polyx &pol, double var )
		{ if( num >= 0 && num < Number ) Coeff[num] = pol.Horner( var ); }
	

	// used in transformation mulpol to horner
	void    WrtCoeff( const int idx, const double val )
		{ if ( (idx >= 0) && (idx < Number) ) Coeff[idx] = val; }

	double* GetCoeff() const                 { return Coeff; }
	int     GetNumber() const                { return Number; }
	int     IsConstant() const               { return (Number <= 1); }
	int     IsEmpty() const                  { return ( Coeff == NULL ); }
	int     ShorterThan( Polyx &Pol2 ) const { return (Number < Pol2.Number); }

	// --------------- rootfinders ----------------------------------------------

	int     Zero( double, double, double*, double*, int ) const; // main entry
	int     OneZero( double, double, double*, double*, int ) const;
	// get largest root only

	int     RootFunction(double a,double fa,double b,double fb,
			     double *R,int &N ) const
		{ return (this->*ZeroSturmFunction) ( a, fa, b, fb, R, N ); }
	// called from sturmchain::divide
	// bugfix to call fn ptr from outside class

	void    Print() const;  // printout polyx for debugs
	Polyx   Derive() const; // returns derivate of polyx
};

//-----------------------------------------------------------------------------

#endif // POLYX_H
