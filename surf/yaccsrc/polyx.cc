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



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mymemory.h"
#include "polyx.h"

/*****************************************************************************/
/* POLYNOMIALS IN X                                                          */
/*****************************************************************************/


/* ------------------------------------------------------------------------- */
/* allocate memory for  n  doubles in p                                      */
/* ------------------------------------------------------------------------- */

void polyx::allocCoeff (int size)
{
	n = size;
	delete a;
	a = new double [size+1];

	int i;	
	for( i = 0; i <= size; i++ ) {
		a[i] = 0.0;
	}
}

/* ------------------------------------------------------------------------- */
/* polynomial evaluation with horners method                                 */
/* ------------------------------------------------------------------------- */

double  polyx::horner (double x) const
{
	register    double  result;
	register    int     i;

	result = a[n];

	for( i = n - 1; i >= 0; i-- ) {
		result *= x;
                result += a[i];
	}

	return result;
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative                                                     */
/* ------------------------------------------------------------------------- */

void polyx::fastDerive (polyx &dp) const
{
	int     i,j;

	for( j = 0,i = 1; i <= n; j++,i++ ) {
		dp.a[j] = i*a[i];
	}
	
	dp.n = n - 1;
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative evaluation with horners method                      */
/* ------------------------------------------------------------------------- */

double  polyx::dx_horner (double x ) const
{
	register    double  result;
	register    int     i;

	result = a[n]*n;

	for( i = n - 1; i > 0; i-- ) {
		result = result*x + a[i]*i;
	}

	return result;
}

/* ------------------------------------------------------------------------- */
/* polynomial output		                                             */
/* ------------------------------------------------------------------------- */

ostream & operator << (ostream &os, const polyx &p)
{
	int     i;

	for( i = 0; i <= p.n; i++ ) {
		if( i > 0 ) {
			os << (p.a[i] >= 0.0 ? " +" : " ");
		}
		os << p.a[i] << "x^" << i;
	}
	os << endl;
	return os;
}
