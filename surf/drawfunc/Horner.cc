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



#include <math.h>
#include <stdio.h>

#include "def.h"
#include "simple.h"
#include "monomarith.h"
#include "polyarith.h"
#include "MultiVariatePolynom.h"
#include "Horner.h"

// ----------------------------------------------------------------------------
// -------------- Constructors ------------------------------------------------
// ----------------------------------------------------------------------------

Hornerxy::Hornerxy()                               // generate zero hornerpoly
	: NumberofP2(0),
	  Polynom2(NULL),
	  Pol2exp1(NULL),
	  Polynom1 (new Polyx(0.0))
{
}


Hornerxy::Hornerxy( const Hornerxy &alt )          // copy constructor
	: NumberofP2(alt.NumberofP2),
	  Polynom2(new Polyx[alt.NumberofP2]),
	  Pol2exp1(new int[alt.NumberofP2]),
	  Polynom1(new Polyx(*(alt.Polynom1)))
{ 
	for( int i = 0; i < NumberofP2; i++ ) { 
		Polynom2[i] = alt.Polynom2[i];
		Pol2exp1[i] = alt.Pol2exp1[i]; 
	}
}
 
Hornerxy::~Hornerxy()                              // clean up
{ 
	NumberofP2 = 0; 
	delete [] Polynom2; 
	delete [] Pol2exp1; 
	delete Polynom1;
}

// ----------------------------------------------------------------------------
// -------------- transformation from polyxy to hornerxy ----------------------
// ----------------------------------------------------------------------------
// Polyxy is not passed as reference since Sort is not const
Hornerxy& Hornerxy::operator=( Polyxy Funktion )
{
	Funktion.Sort( 1 );       // sort in order xy
	NumberofP2 = Funktion.GetNumberOfDifferentExponents( 1 ); 
	Polynom2 = new Polyx[NumberofP2]; 
	Pol2exp1 = new int[NumberofP2];
	Funktion.Transform( Polynom2, Pol2exp1, Polynom1, 1 );
	return *this;
}

// ----------------------------------------------------------------------------
// ------------- set second to last variable -- i.e. build pol1 ---------------
// ----------------------------------------------------------------------------

void Hornerxy::SetVar( const double var )
{
	Polynom1->NullCoeffs();
	for( int i = 0; i < NumberofP2; i++ )
		Polynom1->SetHornered( Pol2exp1[i], Polynom2[i], var );
}

// ----------------------------------------------------------------------------
// ------------- call up rootfinder -------------------------------------------
// ----------------------------------------------------------------------------


int Hornerxy::Zero( const double min,          // right of range
		    const double max,          // left of range
		    double* Roots,             // array for zeroes to find
		    double* RootsOld,          // array of estimates
		    const int NumberOld )      // number of estimates
{
	return Polynom1->Zero( min,
			       max,
			       Roots,
			       RootsOld,
			       NumberOld );
}

int Hornerxy::OneZero( const double min,          // right of range
		       const double max,          // left of range
		       double* Roots)             // array for zeroes to find
{
	return Polynom1->OneZero( min,
				  max,
				  Roots,
				  NULL,
				  0 );
}

// ----------------------------------------------------------------------------
// ------------- return value of pol1 at var ----------------------------------
// ----------------------------------------------------------------------------

double Hornerxy::Horner( const double var )
{
	return Polynom1->Horner( var );
}

// ----------------------------------------------------------------------------
// ------------- return value of derivative of pol1 at var --------------------
// ----------------------------------------------------------------------------

double Hornerxy::HornerDerived(const double var)
{
	return Polynom1->HornerDerived( var );
}

// ----------------------------------------------------------------------------
// ----------------------------- printout routine -----------------------------
// ----------------------------------------------------------------------------
	    
void Hornerxy::Print()
{
	int i = 0;
	fprintf( stderr,"p1 = " );
	Polynom1->Print();
	fprintf( stderr,"p2e1 =\n");
	for( i = 0; i < NumberofP2; i++ )
		fprintf( stderr,"%d ",Pol2exp1[i]);
	fprintf( stderr,"\n p2 =\n" );
	for( i = 0; i < NumberofP2; i++ ) {
		fprintf( stderr, "pYkY= %d      ", Pol2exp1[i]);
		Polynom2[i].Print();
	}
}


// ----------------------------------------------------------------------------
// ------------------ hornerxyz constructors -- base class hornerxy -----------
// ----------------------------------------------------------------------------

Hornerxyz::Hornerxyz()                        // create zero hornerxyz
	: Hornerxy(),
	  NumberofP3(0),
	  Polynom3(NULL),
	  Pol3expPol2(NULL),
	  Pol3exp2(NULL)
{
}

Hornerxyz::Hornerxyz( const Hornerxyz& alt ) // copy constructor
	: Hornerxy(alt),
	  NumberofP3(alt.NumberofP3),
	  Polynom3(new Polyx[alt.NumberofP3]), 
	  Pol3expPol2(new int[alt.NumberofP3]),
	  Pol3exp2(new int[alt.NumberofP3])
{ 
	for( int i = 0; i < NumberofP3; i++ ) {
		Polynom3[i] = alt.Polynom3[i];
		Pol3expPol2[i] = alt.Pol3expPol2[i];
		Pol3exp2[i] = alt.Pol3exp2[i];
	}
}

Hornerxyz::~Hornerxyz()                      // clean up
{ 
	NumberofP3 = 0;
	delete [] Polynom3;
	delete [] Pol3expPol2;
	delete [] Pol3exp2;
}

// ----------------------------------------------------------------------------
// ----------------- transformation from polyxyz to hornerxyz -----------------
// ----------------------------------------------------------------------------
// Polyxyz is not passed as reference since Sort is not const
Hornerxyz& Hornerxyz::operator=( Polyxyz Funktion )
{
	Funktion.Sort(0);
	NumberofP3  = Funktion.GetNumberOfDifferentExponents( 0, 2 );
	NumberofP2  = Funktion.GetNumberOfDifferentExponents( 2 ); 
	
	Polynom3    = new Polyx[NumberofP3];
	Pol3expPol2 = new int[NumberofP3];
	Pol3exp2    = new int[NumberofP3];
	Polynom2    = new Polyx[NumberofP2]; 
	Pol2exp1    = new int[NumberofP2];
	
	Funktion.Transform( Polynom3,
			    Pol3expPol2,
			    Pol3exp2,
			    Polynom2,
			    Pol2exp1,
			    Polynom1 );
	return *this;
}

// ----------------------------------------------------------------------------
// ------------- set variable in pol3-array to build up pol2's ----------------
// ----------------------------------------------------------------------------

void Hornerxyz::SetRow( const double var )
{
	int i;
	for( i = 0; i < NumberofP2; i++ )
		Polynom2[i].NullCoeffs();
	for( i = 0; i < NumberofP3; i++ )
		Polynom2[Pol3expPol2[i]].SetHornered( Pol3exp2[i], Polynom3[i], var );
}

// ----------------------------------------------------------------------------
// ---------------------- printout routine ------------------------------------
// ----------------------------------------------------------------------------

void Hornerxyz::Print()
{
	Hornerxy::Print();

	int i;
	
	fprintf( stderr,"p3e2 =\n");
	for( i = 0; i < NumberofP3; i++ )
		fprintf( stderr,"%d ",Pol3exp2[i]);

	fprintf( stderr,"\n p3ep2 =\n");
	for( i = 0; i < NumberofP3; i++ )
		fprintf( stderr,"%d ",Pol3expPol2[i]);

	fprintf( stderr,"\n p3 =\n" );
	for( i = 0; i < NumberofP3; i++ ) {
		fprintf( stderr, "    ");
		Polynom3[i].Print();
	}
}

// ----------------------------------------------------------------------------
// ------------------ end of file Horner.cc -----------------------------------
// ----------------------------------------------------------------------------
