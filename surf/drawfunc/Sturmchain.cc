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
//---------------------------- Sturmchain.cc ----------------------------------
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include "def.h"
#include "simple.h"
#include "UniVariatePolynom.h"
#include "Sturmchain.h"


//-----------------------------------------------------------------------------
//------------------ Sturmchain constructor -----------------------------------
//-----------------------------------------------------------------------------

Sturmchain::Sturmchain( const Polyx &Pol )
  : Chain(new Polyx[Pol.GetNumber()+1])           // generate sturmchain
{
  int i = 0;
  Chain[0] = Pol;
  Chain[1] = Pol.Derive();
  for( i = 2;
       Chain[i-1].ShorterThan(Chain[i-2]) && !Chain[i-1].IsConstant();
       i++ )
    {
      Chain[i] = Chain[i-2] % Chain[i-1];
      Chain[i].NegateCoeffs();
    }
  if( Chain[i-1].IsEmpty() ) i--;
  Length = i;
}

Sturmchain::~Sturmchain() // cleanup
{
  delete [] Chain;
}

// ----------------------------------------------------------------------------
// ------------ return number of signchanges of sturmchain in a ---------------
// ------------ f(a) is given to save computation -----------------------------
// ----------------------------------------------------------------------------

int Sturmchain::SignChange( double x, double Val ) const
{
  int result = 0;
  for( int i = 1; i < Length; i++ )
    {
      double ValN = Chain[i].Horner( x );
      if( Val*ValN < 0.0 )
	result ++;
      Val = ValN;
    }
  return result;
}

// ----------------------------------------------------------------------------
// ------ divides intervall until there is only one root inside ---------------
// ------ is first called from class polyx with last arg maxiterations --------
// ------ and then counts down to zero ----------------------------------------
// ----------------------------------------------------------------------------


int Sturmchain::DivideIntervalMulti( double x1, double fx1, int sx1,
				     double x2, double fx2, int sx2,
				     double *Root, int &NumberOfRoots,
				     int Iterations )
{
  int Total = sx1 - sx2; // difference of signchanges between a and b
                         // is the number of zeroes between a and b
  if( Total > 1 )
    {
	    if( --Iterations == 0 ) {
		    return false;
	    }
	    
      double m = ( x1 + x2 ) / 2.0;
      double fm = Chain[0].Horner( m );
      int    sm = SignChange( m, fm );
      // check range m...b (upper first to get roots descending)
      int Flag = DivideIntervalMulti( m, fm ,sm, x2, fx2, sx2,
				      Root, NumberOfRoots, Iterations );
      // check range a...m
      Flag += DivideIntervalMulti( x1, fx1, sx1, m, fm, sm,
				   Root, NumberOfRoots, Iterations );
      return Flag; // >0 if root in one of both ranges
    }
  if( Total == 1 )
    return Chain[0].RootFunction( x2, x1, fx2, fx1, Root, NumberOfRoots );
  // root function calls the selected algorithm via function pointer
  return false;
}

// ----------------------------------------------------------------------------
// ------- same but get only largest root -------------------------------------
// ----------------------------------------------------------------------------


int Sturmchain::DivideIntervalSingle( double x1, double fx1, int sx1,
				      double x2, double fx2, int sx2,
				      double *Root, int &NumberOfRoots,
				      int Iterations )
{
  int Total = sx1 - sx2;
  if( Total > 1 )
    {
	    if( --Iterations == 0 ) {
		    return false;
	    }
	    
      double m = ( x1 + x2 ) / 2.0;
      double fm = Chain[0].Horner( m );
      int    sm = SignChange( m, fm );
      int Flag = 0;
      // if not in m...b
      if( !( Flag = DivideIntervalSingle( m, fm ,sm, x2, fx2, sx2,
					  Root, NumberOfRoots, Iterations ) ) )
	// search a...m
	Flag = DivideIntervalSingle( x1, fx1, sx1, m, fm, sm, Root,
				     NumberOfRoots, Iterations );
      return Flag;
    }
  if( Total == 1 ) {
	  return Chain[0].RootFunction( x2, x1, fx2, fx1, Root, NumberOfRoots );
  }
  
  return false;
}


// ----------------------------------------------------------------------------
// ------------ end of file Sturmchain.cc -------------------------------------
// ----------------------------------------------------------------------------











