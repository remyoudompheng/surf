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

#ifndef CLMULTIPOLY_H
#define CLMULTIPOLY_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define  VARIABLE_X         0
#define  VARIABLE_Y         1
#define  VARIABLE_Z         2

#include <monomarith.h>
#include <polyarith.h>
#include <Monomial.h>
#include <UniVariatePolynom.h>

#include<iostream>



template<class Mon>
class MultiPoly
{
protected:
	int Number;
	int deg;
	Mon *Monomial;

public:
	MultiPoly();                      // empty polynom
	MultiPoly( const int );           // allocate number of monoms
	MultiPoly( const double );        // const poly
	MultiPoly( const Mon& );          // poly of one monom
	MultiPoly( const polyxyz alt );   // copy polyxyz
	MultiPoly( const MultiPoly& );    // copy Poly-class
	~MultiPoly();

	// ------------- operators --------------------------------------

	MultiPoly& operator*=(const double Mult);
	MultiPoly& operator*=(const Mon &mon);
	MultiPoly& operator*=(const MultiPoly& Pol2);
	MultiPoly& operator=( const double var );
	MultiPoly& operator=(const MultiPoly& Pol);
	MultiPoly& operator+=(const Mon &);
	MultiPoly& operator+=(const MultiPoly& );
	MultiPoly  operator*(const MultiPoly& Pol2) const;
	MultiPoly  operator*(const Mon &m) const;
	bool operator==( const MultiPoly& ) const;
	bool operator!=( const MultiPoly& ) const;
	
	// ---------------- element functions ----------------------------
	
	int     GetDeg() const
		{ return deg; }
	
	int     GetNumber() const
		{ return Number; }

	double  MaxabsCoeff() const
		{ 
			double MaxabsCoeff=0; 
			for( int i=0; i< Number; i++)
				if( Monomial[i].AbsCoeff() > MaxabsCoeff )
					MaxabsCoeff = Monomial[i].AbsCoeff();
			return MaxabsCoeff; 
		}
	void    Norm()
		{ 
			double a = MaxabsCoeff(); 
			if(!(fabs(a) < 1e-5)) {
				*this *= (10.0/a) ; 
			}
		}

	Mon     Monom(const int num) const
		{ 
			if ( num >= 0 && num < Number )
				return Monomial[num];
			return 0.0;
		}
	
	void    Renew(int num)
		{
			Mon *store = 0;
			if ( num <= 0 ) { 
				Number = 1; 
				store = new Mon [1]; 
				store[0] = 0.0; 
				deg = 0;
			} else { 
				store = new Mon [num];
				for( int i=0; i < Number; i++)
					store[i] = Monomial[i];
				Number = num;
			}
			delete [] Monomial; 
			Monomial = store;
		}

	void    SetDegree()
		{
			if( Number <= 0 )
				deg = DEG_UNSPEC;
			else {
				int iDeg = 0; int maxDeg = -1;
				for( int i = 0; i < Number; i++ ) { 
					iDeg = Monomial[i].Degree(); 
					if( maxDeg < iDeg ) 
						maxDeg = iDeg; 
				} 
				deg = maxDeg;
			}
		}

	int     GetNumberOfDifferentExponents( int Var ) const
		{
			int Counter = 1;
			for( int i = 1; i < Number; i++ ) 
				if(!Monomial[i].CompareDeg( Monomial[i-1], Var ) )
					Counter++;
			return Counter;
		}

	int     GetNumberOfDifferentExponents( int Var1, int Var2 ) const
		{
			int Counter = 1;
			for( int i = 1; i < Number; i++ )
				if( !Monomial[i].CompareDeg( Monomial[i-1], Var2 )
				    || !Monomial[i].CompareDeg( Monomial[i-1], Var1 ) )
					Counter++;
			return Counter;
		}
	int     MaxDegreeOfVariable( int Var ) const
		{
			int result = 0;
			for( int i = 0; i < Number; i++ ) {
				if( result < Monomial[i].Exponent( Var ) )
					result = Monomial[i].Exponent( Var );
			}
			return result;
		}

	MultiPoly& Derive(const int Var)
		{
			for(int i = 0; i < Number; i++ )
				Monomial[i].Derive( Var );
			Collect();
			return *this;
		}
	      
	void      Sort( const int );
	void      Shift(double,double,double);
	void      Print (ostream &os) const;

	int       Check() const;
	void      Collect();
	void      Subst( const MultiPoly* );
	MultiPoly Subst( const MultiPoly&, const int ) const;
	void      SwapXY();
	MultiPoly Solve( const int );
	MultiPoly CounterOfSolve( int ) const;
	MultiPoly NominatorOfSolve( int ) const;
	MultiPoly Subst( const MultiPoly&, const MultiPoly&, const int ) const;
	
	// friend MultiPoly pow( const MultiPoly& Poly, const int s );
	MultiPoly computePower (const int s) const;
	friend class MappingMatrix;
};

template<class Mon>
inline ostream &operator<< (ostream &os, const MultiPoly<Mon> &m)
{
	m.Print(os);
	return os;
}

template<class Mon>
inline MultiPoly<Mon> pow( const MultiPoly<Mon>& Poly, const int s )
{
	return Poly.computePower(s);
}


class Polyxy : public MultiPoly<MonomXY>
{
public:
	Polyxy()                   : MultiPoly<MonomXY>()    {}
	Polyxy(const int num)      : MultiPoly<MonomXY>(num) {}
	Polyxy(const double val)   : MultiPoly<MonomXY>(val) {}
	Polyxy(const polyxyz alt)  : MultiPoly<MonomXY>(alt) {}
	Polyxy(const Polyxy& alt)  : MultiPoly<MonomXY>(alt) {}
	Polyxy(const MultiPoly<MonomXY>& alt)  : MultiPoly<MonomXY>(alt) {}
	Polyxy(const MultiPoly<MonomXYZ>& alt) : MultiPoly<MonomXY>(alt.GetNumber()) { 
		for(int i = 0; i < Number; i++) {
			Monomial[i] = MonomXY(alt.Monom(i));
		}
		Collect();
	}

	void Transform(Polyx*, int*, Polyx*, int) const;
};

class Polyxyz : public MultiPoly<MonomXYZ>
{
public:
	Polyxyz()                   : MultiPoly<MonomXYZ>()    {}
	Polyxyz(const int num)      : MultiPoly<MonomXYZ>(num) {}
	Polyxyz(const polyxyz alt)  : MultiPoly<MonomXYZ>(alt) {}
	Polyxyz(const Polyxyz& alt) : MultiPoly<MonomXYZ>(alt) {}
	Polyxyz(const MultiPoly<MonomXYZ>& alt) : MultiPoly<MonomXYZ>(alt) {}
	
	void    Transform( Polyx*, int*, int*, Polyx*, int*, Polyx* );
	void    Perspective( const double z );

	double evaluate (double x, double y, double z);
};

#endif
