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
#include <stdlib.h>

#include "def.h"
#include "degree.h"
#include "simple.h"
#include "MultiVariatePolynom.h"
#include "MultiIndex.h"     // for power function
#include "DrawFunc.h"       // for errnums in check
#include "Monomial.h"

// ----------------------------------------------------------------------------
// ---------------- construktors of base - template ---------------------------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon>::MultiPoly()
	: Number(1), deg(0), Monomial(new Mon [1])
{ 
	Monomial[0] = 0.0;
}


template<class Mon>
MultiPoly<Mon>::MultiPoly( const int num )
	: Number(num), deg(DEG_UNSPEC), Monomial(new Mon [num])
{
}


template<class Mon>
MultiPoly<Mon>::MultiPoly(const polyxyz alt)
  : Number(alt.n), deg(alt.deg), Monomial(new Mon [alt.n])
{ 
	for( int i =0; i < Number; i++)
		Monomial[i]=alt.m[i];
}

template<class Mon>
MultiPoly<Mon>::MultiPoly( const MultiPoly &alt )
  : Number(alt.Number), deg(alt.deg),    Monomial(new Mon [alt.Number])
{ 
	for( int i = 0; i < Number; i++ )
		Monomial[i] = alt.Monomial[i];
}


template<class Mon>
MultiPoly<Mon>::MultiPoly( const double value )
	: Number(1), deg(0), Monomial(new Mon [1])
{
	Monomial[0] = value;
}


template<class Mon>
MultiPoly<Mon>::MultiPoly( const Mon &m )
	: Number(1), deg(m.Degree()), Monomial(new Mon [1])
{
	Monomial[0] = m;
}


template<class Mon>
MultiPoly<Mon>::~MultiPoly() 
{
	delete [] Monomial;
}


// ----------------------------------------------------------------------------
// --------------------    operators of template ------------------------------
// --------------------        reflexive     ----------------------------------
// ----------------------------------------------------------------------------
// ----------------------- multiplications ------------------------------------

template<class Mon>
MultiPoly<Mon>& MultiPoly<Mon>::operator*=(const double Mult)
{ 
	for( int i =0; i< Number; i++) 
		Monomial[i] *= Mult; 
	return *this; 
} 


template<class Mon>
MultiPoly<Mon>& MultiPoly<Mon>::operator*=(const Mon &mon)
{
	for( int i = 0; i < Number; i++ )
		Monomial[i] *= mon;
	Collect();
	return *this;
}


template<class Mon>
MultiPoly<Mon>& MultiPoly<Mon>::operator*=(const MultiPoly& Pol2)
{
	Mon* Temp = new Mon [ Number * Pol2.Number ];
	int i = 0, j = 0, k = 0, l = 0;
	for( i = 0; i < Number; i++ )
		for( j = 0; j < Pol2.Number; j++ ) {
			// calculate next monomial
			Mon Monom = Monomial[i] * Pol2.Monomial[j];
			for( l = 0; l < k && !(Temp[l]>=Monom); l++ )
				;
			// see if monomial with same exponents exists
			// if not first monomial or found -- add up
			if ( k != l )
				Temp[l] += Monom;
			else
				Temp[k++] = Monom; // otherwise add to chain
		}
	delete [] Monomial;
	Monomial = Temp;
	Number = k;
	Renew( Number );
	Collect();
	return *this;
}

// ----------------------------------------------------------------------------
// ------------------- additions ----------------------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon>& MultiPoly<Mon>::operator+=( const MultiPoly& Pol )
{
	Mon* Temp = new Mon [ Number + Pol.Number ];
	int i = 0, j = 0;
	for( i = 0; i < Number; i++ )
		Temp[i] = Monomial[i];
	for( i = 0; i < Pol.Number; i++ ) {
		for( j = 0; j < Number && !(Pol.Monomial[i]>=Temp[j]); j++ );
		// suche ob schon ein monom mit gleichen exponenten existiert
		if ( j < Number )
			Temp[j] += Pol.Monomial[i];  // falls gefunden aufaddieren
		else
			Temp[Number++] = Pol.Monomial[i]; // falls nicht anhängen
	}
	delete [] Monomial;
	Monomial = Temp;
	Renew( Number );
	Collect();
	return *this;
}


template<class Mon>
MultiPoly<Mon>& MultiPoly<Mon>::operator+=( const Mon& m )
{
	int i = 0;
	for( i = 0; i < Number && !(Monomial[i]>=m); i++ );
	if ( i == Number ) {
		Mon *Temp = new Mon [Number+1];
		for( i = 0; i < Number; i++ )
			Temp[i] = Monomial[i];
		Temp[Number++] = m;
		delete [] Monomial;
		Monomial = Temp;
		if( m.Degree() > deg ) deg = m.Degree();
	} else
		Monomial[i] += m;
	return *this;
}


// ----------------------------------------------------------------------------
// ----------------------- assignments ----------------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon>& MultiPoly<Mon>::operator=( const double var )
{ 
	delete [] Monomial; 
	Monomial = new Mon [1]; 
	Monomial[0] = var; 
	deg = 0; 
	Number = 1;
	return *this;
}


template<class Mon>
MultiPoly<Mon>& MultiPoly<Mon>::operator=(const MultiPoly& Pol)
{ 
	Mon* Temp = new Mon [Pol.Number];
	deg = Pol.deg; Number = Pol.Number;
	for( int i = 0; i < Number; i++ ) 
		Temp[i] = Pol.Monomial[i];
	delete [] Monomial;
	Monomial = Temp;
	return *this; 
}

// ----------------------------------------------------------------------------
// ------------------- normal multiplication ----------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon>  MultiPoly<Mon>::operator*(const Mon &m) const
{
	MultiPoly Pol = *this; Pol *= m; return Pol;
}


template<class Mon>
MultiPoly<Mon> MultiPoly<Mon>::operator*(const MultiPoly& Pol2) const 
{
	MultiPoly Pol = *this; Pol *= Pol2; return Pol;
}

// ----------------------------------------------------------------------------
// ------------------- comparators --------------------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
int MultiPoly<Mon>::operator==( const MultiPoly& Pol ) const
{
	if( Number != Pol.Number  || deg != Pol.deg ) 
		return FALSE;

	int diff = 0;
	for( int i = 0; i < Number && !diff; i++ )
		diff = !( Monomial[i] == Pol.Monomial[i] );
	return diff;
}

template<class Mon>
int MultiPoly<Mon>::operator!=( const MultiPoly& Pol ) const
{
	return !( *this == Pol );
}

// ----------------------------------------------------------------------------
// ---------------- element functions of base template ------------------------
// ----------------------------------------------------------------------------


template<class Mon>
int MultiPoly<Mon>::Check(void) const
{
	if( Number == 0 )
		return CURVE_ZERO;

	if( deg < 1 )
		return CURVE_CONST;

	if( deg >= MAX_DEGREE )
		return CURVE_DEG_HIGH;

	return FALSE;
}

// ----------------------------------------------------------------------------
// ---------- printout for debugging ------------------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
void MultiPoly<Mon>::Print (ostream &os) const
{
	for( int i=0; i< Number; i++) { 
		if( Monomial[i].Coeff() >= 0.0 )
			os << " +";
		else
			os << " ";
		//os << Monomial[i];
		Monomial[i].Print(os);
		os << endl;
	}
}

// ----------------------------------------------------------------------------
// ------------ garbage collect -----------------------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
void MultiPoly<Mon>::Collect(void)
{
	int i,j;
	for( i = Number - 1; i > 0 ; i--) {
		for( j = i-1; j>=0 && !(Monomial[i]>=Monomial[j]); j-- )
			;
		if( j>=0 ) {
			Monomial[j]+=Monomial[i];
			if( (--Number) > i) 
				Monomial[i]=Monomial[Number];
		}
	}
	while( i < Number ) {
		if( Monomial[i].Coeff() ) 
			i++;
		else if( (--Number) > i ) 
			Monomial[i] = Monomial[Number];
	}
	Renew( Number );
	SetDegree();
}

// ----------------------------------------------------------------------------
// ------------ translate polynom -- part of positioning ----------------------
// ----------------------------------------------------------------------------

template<class Mon>
void MultiPoly<Mon>::Shift(double sx, double sy, double sz)
{
	MultiPoly *SubPol = new MultiPoly [3];
	SubPol[0] = -sx;
	SubPol[1] = -sy;
	SubPol[2] = -sz;
	for( int i = 0; i < 3;  i++ ) {
		// Substitutionspolynome ' x-shiftx', 'y-shifty', 'z-shiftz' erzeugen
		Mon Monom = 1.0; Monom.Exponent( i, 1 ); // monom ist aktuelle variable
		SubPol[i] += Monom;
	}
	Subst( SubPol ); // und substituieren
	delete [] SubPol;
	Collect();
}

// ----------------------------------------------------------------------------
// ------------- substitute polynom for variable nr. var ----------------------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon> MultiPoly<Mon>::Subst( const MultiPoly<Mon> &SubstPoly, const int Var ) const
{
	MultiPoly Result, Part; int i,j;
	for( i = 0; i < Number;  i++ ) {
		Part = Monomial[i];
		j = Part.Monomial[0].ExtractExponent( Var );
		Part *= pow( SubstPoly, j );
		Result += Part;
	}
	return Result;
}

// ----------------------------------------------------------------------------
// -------------- substitute polyfield for each variable ----------------------
// ----------------------------------------------------------------------------

template<class Mon>
void MultiPoly<Mon>::Subst( const MultiPoly<Mon> *SubstPoly )
{
	MultiPoly<Mon> Result, Part; int i,j;
	SetDegree();
	// precalculate all powers 0...deg for all substitution polynoms
	MultiPoly<Mon> **PowPoly = new MultiPoly<Mon>* [3];
	for( i = 0; i < 3; i++ ) {
		PowPoly[i] = new MultiPoly<Mon> [deg+1];
		for( j = 0; j <= deg; j++ ) 
			PowPoly[i][j] = pow( SubstPoly[i], j );
	}

	// place the adequately powered substitution polynoms in every monomial
	for( i = 0; i < Number ; i++ ) {
		Part = Monomial[i].Coeff();
		for( j = 0; j < 3; j++ )
			Part *= PowPoly[j][Monomial[i].Exponent(j)];
		Result += Part;
	}
	for( j = 0; j < 3; j++ ) 
		delete [] PowPoly[j];
	delete [] PowPoly;
	*this = Result;
}

// ----------------------------------------------------------------------------
// ------------ swap variables x and y ----------------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
void MultiPoly<Mon>::SwapXY()
{
	for( int i = 0; i < Number; i++ )
		Monomial[i].SwapXY();
}


// ----------------------------------------------------------------------------
// ------------ solve a polynomial for one variable ---------------------------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon> MultiPoly<Mon>::Solve(const int Var)
{
	int Location = -1;
	int error = 0;
	Collect();
	for( int i = 0; i < Number && !error; i++ ) {
		if( Monomial[i].Exponent( Var ) > 1 ) 
			error = 1;
		if( Monomial[i].Exponent( Var ) == 1 ) {
			if( Monomial[i].Degree() != 1 ) 
				error = 1;
			Location = i;
		}
	}

	// if anything is strange return zero
	if( error == 1
	    || Location == -1
	    || Number == 1
	    || Monomial[Location].Coeff() == 0.0 )
		return 0.0;

	double CoeffRez = - 1.0 / Monomial[Location].Coeff();
	MultiPoly Result = *this;
	Result.Monomial[Location] = 0.0;
	Result *= CoeffRez;
	Result.Collect();
	return Result;
}

// ----------------------------------------------------------------------------
// ------------ sort monomials ------------------------------------------------
// ----------------------------------------------------------------------------

template<class Mon>
void MultiPoly<Mon>::Sort( const int Var )
{
	for( int j = 1; j < Number; j++ ) {
		for( int i = 1; i < Number; i++ ) {
			if( Monomial[i-1].Compare( Monomial[i], Var ) > 0 ) {
				Mon Temp      = Monomial[i-1];
				Monomial[i-1] = Monomial[i];
				Monomial[i]   = Temp;
			}
		}
	}
}

// ----------------------------------------------------------------------------
// ------------------- power function for polynoms -- declared friend ---------
// ----------------------------------------------------------------------------
template<class Mon>
MultiPoly<Mon> MultiPoly<Mon>::computePower (const int s) const
{
	if( s < 0 ) {
		fprintf( stderr, "Trying to get %d-th power of a MultiPoly\n",s);
		exit(1);
	}

	if( s == 0 ) 
		return 1.0;

	if( s == 1 ) 
		return *this;

	if( Number == 0 ) 
		return 0.0;

	if( Number == 1 )
		return (Mon) pow( Monomial[0], s );

	MultiPoly<Mon> Pneu = 0.0;
	Index MultiIndex( Number, s );

	do {
		Mon Monom =  MultiIndex.Coeff();
		for( int i = 0; i < Number; i++ )
			Monom *= pow( Monomial[i], MultiIndex.Element(i) );
		Pneu += Monom;
	} while( ++MultiIndex );
	return Pneu;
	
}

// template<class Mon>
// MultiPoly<Mon> pow( const MultiPoly<Mon>& Poly, const int s )
// {
// 	return Poly.computePower(s);
// }

// template<class Mon>
// MultiPoly<Mon> pow( const MultiPoly<Mon>& Poly, const int s )
// {
//   if( s < 0 )
//     {
//       fprintf( stderr, "Trying to get %d-th power of a MultiPoly\n",s);
//       exit(1);
//     }
//   if( s == 0 ) return 1.0;
//   if( s == 1 ) return Poly;
//   if( Poly.Number == 0 ) return 0.0;
//   if( Poly.Number == 1 )
//   return (Mon) pow( Poly.Monomial[0], s );

//   MultiPoly<Mon> Pneu = 0.0;
//   Index MultiIndex( Poly.Number, s );

//   do{
//        Mon Monom =  MultiIndex.Coeff();
//        for( int i = 0; i < Poly.Number; i++ )
// 	 Monom *= pow( Poly.Monomial[i], MultiIndex.Element(i) );
//        Pneu += Monom;
//   }while( ++MultiIndex );
//   return Pneu;
// }

// ----------------------------------------------------------------------------
// --------------------- special functions for derived classes ----------------
// - transformation from polyxyz to hornerxyz called by hornerxyz::operator= --
// ----------------------------------------------------------------------------

void Polyxyz::Transform( Polyx *PolY, int *pYepX, int *pYeX,
			 Polyx *PolX, int *pXeZ, Polyx *PolZ )
{
	int pxn = 0, pyn = 0;
  
	PolX[0].NewCoeff( Monomial[0].Exponent(0)+1 );
	PolY[0].NewCoeff( Monomial[0].Exponent(1)+1 );
	PolY[0].WrtCoeff( Monomial[0].Exponent(1), Monomial[0].Coeff() );
	pYepX[0] = 0;
	pYeX[0] = Monomial[0].Exponent(0);
	pXeZ[0] = Monomial[0].Exponent(2);
	
	for( int i = 1; i < Number; i++ ) {
		if( !Monomial[i].CompareDeg( Monomial[i-1], 2 ) ) { 
			PolX[++pxn].NewCoeff( Monomial[i].Exponent(0)+1 ); 
			pXeZ[pxn] = Monomial[i].Exponent(2); 
		}
		if( !Monomial[i].CompareDeg( Monomial[i-1], 2 ) 
		    || !Monomial[i].CompareDeg( Monomial[i-1], 0 ) ) { 
			PolY[++pyn].NewCoeff( Monomial[i].Exponent(1)+1 ); 
			pYepX[pyn] = pxn;
			pYeX[pyn] = Monomial[i].Exponent(0);
		}
		PolY[pyn].WrtCoeff( Monomial[i].Exponent(1) , Monomial[i].Coeff() );
	}
	PolZ->NewCoeff( Monomial[0].Exponent(2)+1 );
}

// ----------------------------------------------------------------------------
// --------- perspective trivariate polynom -----------------------------------
// ----------------------------------------------------------------------------

void Polyxyz::Perspective( const double z )
{
	double z1 = -1.0/z;
	MultiPoly<MonomXYZ> *SubPol = new MultiPoly<MonomXYZ>[3];
	
	for( int i = 0; i < 2; i++ ) {
		MonomXYZ Monom1 = z1;
		Monom1.Exponent( i,1 );
		Monom1.Exponent( 2,1 );
		MonomXYZ Monom2 = 1.0;
		Monom2.Exponent( i,1 );
		SubPol[i] = Monom1; SubPol[i] += Monom2;
	}

	// substitute  x  by  z1*xz + x , and  y  by  z1*y z + y

	MonomXYZ Monom3 = 1.0;
	Monom3.Exponent( 2, 1 );
	SubPol[2] = Monom3;  // substitute  z  by  z
	Subst( SubPol );
	delete [] SubPol;
	Collect();
}

// ----------------------------------------------------------------------------
// - transformation from polyxy to hornerxy called by hornerxy::transform -----
// ------------------ two possible directions for ''hornering'' ---------------
// ----------------------------------------------------------------------------

void Polyxy::Transform( Polyx *Polynom, int *Exponent, Polyx* PolynomX,
			int dir ) const
{
	dir = dir & 1;
	int Counter = 0; Polynom[0].NewCoeff( Monomial[0].Exponent(1-dir)+1 );
	Polynom[0].WrtCoeff( Monomial[0].Exponent(1-dir) , Monomial[0].Coeff() );
	Exponent[0] = Monomial[0].Exponent(dir);
	for( int i = 1; i < Number; i++ ) {
		if( !Monomial[i].CompareDeg( Monomial[i-1], dir ) ) {
			Polynom[ ++Counter ].NewCoeff( Monomial[i].Exponent(1-dir)+1 );
			Exponent[ Counter ] = Monomial[i].Exponent(dir);
		}
		Polynom[ Counter ].WrtCoeff( Monomial[i].Exponent(1-dir),
					     Monomial[i].Coeff() );
	}
	PolynomX->NewCoeff( Monomial[0].Exponent(dir)+1 );
}

// ----------------------------------------------------------------------------
// solve polynom cut for variable z and subst result in this, return as polyxy
// ----------------------------------------------------------------------------
// -------- return denominatorof polynom solved for variable nr. var ----------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon> MultiPoly<Mon>::CounterOfSolve( int Var ) const
{
	MultiPoly Counter = 0.0;
	for( int i = 0; i < Number; i++ ) {
		if( Monomial[i].Exponent( Var ) == 0 ) {
			Mon Monom = Monomial[i];           // copy monom
			Monom.SetCoeff( -Monom.Coeff( ) );    // invert sign of coefficient
			Counter += Monom;                  // add to denomiantor
		}
	}
	Counter.Collect();
	return Counter;
}

// ----------------------------------------------------------------------------
// -------- return nominator of polynom solved for variable nr. var -----------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon> MultiPoly<Mon>::NominatorOfSolve( int Var ) const
{
	MultiPoly Nominator = 0.0;
	for( int i = 0; i < Number; i++ ) {
		if( Monomial[i].Exponent( Var ) == 1 ) {
			Mon Monom = Monomial[i];          // copy monom
			Monom.Exponent( Var, 0 );         // clear z exponent
			Nominator += Monom;               // add to nominator
		}
	}
	Nominator.Collect();
	return Nominator;
}

// ----------------------------------------------------------------------------
// ------------- substitute rational polynom for variable nr. var -------------
// ----------------------------------------------------------------------------

template<class Mon>
MultiPoly<Mon> MultiPoly<Mon>::Subst( const MultiPoly& Counter, 
				      const MultiPoly& Nominator,
				      const int Var ) const
{
	int i = 0;
	int ZmaxDeg = MaxDegreeOfVariable( 2 ); // calc max z degree of surface

	MultiPoly *PowPoly = new MultiPoly [ZmaxDeg+1];

	for( i = 0; i <= ZmaxDeg; i++ ) {
		PowPoly[i] = pow( Counter,i )*pow( Nominator,ZmaxDeg-i );
	}
	MultiPoly Result = 0.0, Part = 0.0;

	for( i = 0; i < Number; i++ ) {
		Mon Monom = Monomial[i]; // copy monom
		int zDeg = Monom.ExtractExponent( Var );
		Part = Monom;
		Part *= PowPoly[zDeg];
		Result += Part;
	}

	delete []PowPoly;

	return Result;
}

double Polyxyz::evaluate(double x, double y, double z)
{
	double result=0;
	int i;
	for (i=0; i<Number; i++) {
		result += Monomial[i].a*pow(x, (double)Monomial[i].k[0])*pow(y, (double)Monomial[i].k[1])*pow(z, (double)Monomial[i].k[2]);
	}
	return result;
}

// ----------------------------------------------------------------------------
// ------------ declare templates -- at end for gcc 2.6.3 ---------------------
// ----------------------------------------------------------------------------

#ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION
template class MultiPoly<MonomXY>;
template class MultiPoly<MonomXYZ>;
template class Monom<2>;
template class Monom<3>;
#endif

// ----------------------------------------------------------------------------
// ------------- end of file MultiVariatePolynom.cc ---------------------------
// ----------------------------------------------------------------------------
