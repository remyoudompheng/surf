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





#ifndef HORNERXY_H
#define HORNERXY_H

class Polyx;
class Polyxy;
class Polyxyz;

// ----------------------------------------------------------------------------
// --------------- horner classes ------ implement horner sceme ---------------
// --------------- for multivariate polynoms ----------------------------------
// ----------------------------------------------------------------------------
// --------------- base class -- for bivariate polynoms -----------------------
// ----------------------------------------------------------------------------

class Hornerxy
{
private:
	void operator = (const Hornerxy &);

public:

        Hornerxy();
        Hornerxy( const Hornerxy &alt );
        ~Hornerxy();
        Hornerxy& operator=( Polyxy );   // transform polyxy to hornerpoly

        void    SetVar( const double );  // evaluates the pol2's at var
	                                 // and so builds up pol1
        int     Zero( const double,const double,double*,double*,const int );
                                         // calls up rootfinder for pol1
        int     OneZero( const double, const double, double* );
                                         // calls up maxrootfinder for pol1
        double  Horner(const double);        // returns val of pol1 at var
        double  HornerDerived(const double); // returns val of pol1' at var
        void    Print();                     // printout bivariate hornerpoly


protected:

        int     NumberofP2;
        Polyx   *Polynom2;
        int     *Pol2exp1;
        Polyx   *Polynom1;     // the polynom that is actually searched for zeroes
};

// ----------------------------------------------------------------------------
// -------------- derived class for trivariate polynoms -----------------------
// ----------------------------------------------------------------------------

class Hornerxyz : public Hornerxy
{
private:
	void operator=(const Hornerxyz &);

public:
	Hornerxyz();
	Hornerxyz( const Hornerxyz& alt );
	~Hornerxyz();

	Hornerxyz& operator=( Polyxyz );      // transform polyxyz to hornerpoly
	void       SetRow( const double var );// evaluates the pol3's at val
	                                      // and so builds up the pol2's

	void       Print();                   // printout trivariate hornerpoly

private:
	int   NumberofP3;
	Polyx *Polynom3;
	int   *Pol3expPol2;
	int   *Pol3exp2;

};

#endif

