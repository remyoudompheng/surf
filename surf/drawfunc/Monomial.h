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




#ifndef CLMONOM_H
#define CLMONOM_H

#include <math.h>
#include <iostream.h>

#include "simple.h"
#include "monomarith.h"

// ----------------------------------------------------------------------------
// ------------- monomial class template --------------------------------------
// ----------------------------------------------------------------------------

template<int dimen>
class Monom
{
protected:
	double  a;                           // Coefficient
	int        k[dimen];                 // Exponent Field

public:
	// default zero monomial
	Monom() : a(0.0)
		{
			for( int i = 0; i < dimen; i++ )
				k[i] = 0;
		}

	// convert double to monom
	Monom (const double Coef) : a(Coef)
		{
			for( int i = 0; i < dimen; i++ )
				k[i] = 0;
		}

	// copy monom
	Monom (const Monom& m) : a(m.a)
		{
			for( int i = 0; i < dimen; i++ )
				k[i] = m.k[i];
		}

	// copy monom from pointer
	Monom (const Monom* m) : a(m->a)
		{
			for( int i = 0; i < dimen; i++ )
				k[i] = m->k[i];
		}

	// copy monom from monomstruct
	Monom (const monxyz& m) : a(m.a)
		{
			k[0] = m.kx;
			k[1] = m.ky;
		}

	// --------------------------------------------------------------------------
	// check equality of exponents
	int    operator>=( const Monom &m ) const 
		{
			int result = TRUE;
			for( int i = 0; (i < dimen) &&  (result == TRUE); i++ ) 
				if( k[i] != m.k[i] ) 
					result = FALSE; 
			return result;
		}

	// check equality of monoms
	int    operator==( const Monom &m ) const 
		{ return ( *this >= m ) && ( fabs( a - m.a) < epsilon ) ;   }


	// assignment, copy monomial
	Monom& operator=( const Monom &m ) 
		{
			a = m.a;
			for( int i = 0; i < dimen; i++ )
				k[i] = m.k[i];
			return *this;
		}

	// addition 
	Monom& operator+=( const Monom &m )
		{
			if( *this >= m )
				a += m.a;
			return *this;
		}

	// monom multiplication with double
	Monom& operator*=( const double Mult )
		{ a *= Mult; return *this; }
	Monom  operator*( const double Mult ) const 
		{ Monom Mneu ( *this ); Mneu.a *= Mult; return Mneu; }

	// multiplication with monom
	Monom& operator*=( const Monom &m )
		{
			a *= m.a;
			for( int i = 0; i < dimen; i++ )
				k[i] += m.k[i];
			return *this;
		}
	Monom  operator*( const Monom &m ) const 
		{ Monom Mneu ( *this ); Mneu *= m; return Mneu; }

	//---------------------------------------------------------------------------
	// get degree of monomial
	int    Degree() const
		{
			int result = 0;
			for( int i = 0; i < dimen; i++ )
				result += k[i];
			return result;
		}


	void Print(ostream &os) const
		{
			os << a;
			for( int i = 0; i < dimen; i++ )
				os << "*" << (char) (i+'x') << "^" << k[i];
			
		}


	// get absolute of coefficient
	double AbsCoeff() const
		{ return fabs( a ); }


	// compare exponents of variable nr. var
	int    CompareDeg( const Monom &m, const int Var ) const
		{ 
			if( Var >= dimen ) 
				return FALSE;
			return (k[Var] == m.k[Var]); 
		}

	// provide a comparation for sorting monomials
	// priority set to variable nr. var
	int    Compare( const Monom &m, const int Var )
		{
			if ( Var < 0 || Var > 1 ) 
				return 0;

			if ( k[Var] == m.k[Var] )
				return ( m.k[1-Var] - k[1-Var] );

			return (m.k[Var] - k[Var]);
		}

	// derive a monomial in variable nr. var
	void   Derive(const int Var)
		{
			if( Var < dimen ) {
				if( k[Var] > 0 ) {
					a *= k[Var]; 
					k[Var]--;
				}
				else 
					a = 0.0;
			}
		}
	// return exonent of variable nr. var
	int    Exponent( int Var ) const
		{
			if( Var < dimen ) 
				return k[Var];
			return 0;
		}
	// set exponent
	void   Exponent( int Var, int Expo )
		{ 
			if( Var < dimen ) 
				k[Var] = Expo;
		}
	// return and clear exponent
	int    ExtractExponent( int Var )
		{
			int result = 0;
			if( Var < dimen ) {
				result = k[Var];
				k[Var] = 0;
			}
			return result;
		}

	// return coefficient
	double Coeff() const 
		{ return a; }

	// set coefficient
	void   SetCoeff(const double cof) 
		{ a = cof; }
	
	void   SwapXY()
		{
			int tmp = k[0];
			k[0] = k[1];
			k[1] = tmp;
		}
	
	friend class Polyxyz;
	
	// power function for monomials
	Monom computePower (int Exp) const 
		{
			Monom Mneu;
#ifdef SUN
			if ( a == 0.0 && Exp == 0 ) 
				Mneu.a = 1.0;
			else
#endif //SUN
				Mneu.a = pow( a, (double) Exp );
			for( int i = 0; i < dimen; i++ ) 
				Mneu.k[i] = k[i] * Exp;
			return Mneu;
			
		}
};


template<int dimen>
inline ostream &operator << (ostream &os, const Monom<dimen> &m)
{
	m.Print(os);
	return os;
}

template<int dimen>
inline Monom<dimen> pow (const Monom<dimen> &monom, int s)
{
	return monom.computePower(s);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

typedef Monom<2> MonomXY;

// ----------------------------------------------------------------------------
// ---------- derive trivariate monomial class --------------------------------
// ---------- and provide casting mon3 to mon2 --------------------------------
// ----------------------------------------------------------------------------

class MonomXYZ : public Monom<3> {
public:
	MonomXYZ()                    : Monom<3>()    {}
	MonomXYZ( const Monom<3> &m ) : Monom<3>( m ) {}
	MonomXYZ( const MonomXYZ *m ) : Monom<3>( m ) {}
	MonomXYZ( const monxyz &m )   : Monom<3>( m ) { k[2] = m.kz;}
	MonomXYZ( const double d )    : Monom<3>( d ) {}
	MonomXYZ( const MonomXY &m ) : Monom<3>(m.Coeff())
		{ k[0] = m.Exponent( 0 ); k[1] = m.Exponent( 1 ); k[2] = 0; }
	
	// casting operator
	operator MonomXY() 
		{
			MonomXY Mneu;
			Mneu.SetCoeff( a );
			Mneu.Exponent( 0, k[0] );
			Mneu.Exponent( 1, k[1] );
			return Mneu;
		}
	
	// provide comparation for sorting of trivariate monomials
	int Compare( const MonomXYZ &m, const int Var )
		{
			if ( k[2] == m.k[2] )
				return Monom<3>::Compare( m, Var );
			return (m.k[2] - k[2]);
		}
	
};

inline MonomXYZ pow (const MonomXYZ &m, int s)
{
	return m.computePower(s);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#endif  /* MONOMARITH_H */
