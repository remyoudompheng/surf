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



/* ------------------------------------------------------------------------- */
/* hornerarith.c: fast multivariate polynomial evalation                     */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     14.8.94                                                         */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <assert.h>

#include "simple.h"
#include "monomarith.h"
#include "polyarith.h"
#include "hornerarith.h"

static void polyxyz_to_horner( hornerpolyxyz *hp, const polyxyz *p );

void hornerpolyxyz::free()
{
	delete []pY;
	delete []pX;

	delete pYpX;
	delete pYkx;
	delete pXkz;
}

void hornerpolyxyz::setNull()
{
	pY=0;

	pYpX=0;
	pYkx=0;
	pYn=0;
              
	pX=0; 

	pXkz=0;
	pXn=0;
}

hornerpolyxyz::hornerpolyxyz()
{
	setNull();
}

hornerpolyxyz::hornerpolyxyz (const polyxyz &p)
{
	setNull();
	polyxyz_to_horner (this, &p);
}

hornerpolyxyz::~hornerpolyxyz()
{
	free();
}

void hornerpolyxyz::setRow (double y)
{
	int     i;

	for( i = 0; i < pXn; i++ ) {
		memset( (void*)pX[i].a,0,( pX[i].n + 1 )*sizeof(double) );
	}

	for( i = 0; i < pYn; i++ ) {
		pX [ pYpX[i] ].a [ pYkx[i] ] = pY[i].horner (y);
	}

}

void hornerpolyxyz::setColumn (double x)
{
	int     i;

	memset( (void*)pZ.a,0,( pZ.n + 1 )*sizeof(double) );

	for( i = 0; i < pXn; i++ ) {
		pZ.a[pXkz[i]] = pX[i].horner (x);
	}

}





/*****************************************************************************/
/* Polynome in z erzeugen, um das Polynom in x, y und z : p schnell mit ******/
/* dem Hornerschema auszuwerten                                         ******/
/*****************************************************************************/





/* ------------------------------------------------------------------------- */
/* convert a polynomial to a  hornerpolyxyz                                     */
/* ------------------------------------------------------------------------- */

void hornerpolyxyz::polyxyz_to_horner( hornerpolyxyz *hp, const polyxyz *p )
{

	int     c_pYn;
	int     c_pXn;
	int     i;


	hp->pXn = hp->pYn = 1;

	/* ----------------------------- */
	/* compute number of polynomials */
	/* ----------------------------- */

	for( i = 1; i < p->n; i++ ) {
		if( p->m[i].kz != p->m[i-1].kz ) {
			hp->pXn++;
		}
		if( p->m[i].kz != p->m[i-1].kz || p->m[i].kx != p->m[i-1].kx ) {
			hp->pYn++;
		}
	}

	/* ------------ */
	/* allocate 'em */
	/* ------------ */

	hp->pY = new polyx [hp->pYn];
	hp->pX = new polyx [hp->pXn];

	hp->pYpX = new int [hp->pYn];
	hp->pYkx = new int [hp->pYn];
	hp->pXkz = new int [hp->pXn];

	hp->pX[0].allocCoeff (p->m[0].kx);
	hp->pY[0].allocCoeff (p->m[0].ky );

	/* -------------- */
	/* initialize 'em */
	/* -------------- */

	hp->pY[0].a[p->m[0].ky] = p->m[0].a;
	hp->pYpX[0]             = 0;
	hp->pYkx[0]             = p->m[0].kx;
	hp->pXkz[0]             = p->m[0].kz;

	c_pXn = c_pYn = 0;

	for( i = 1; i < p->n; i++ ) {
		if( p->m[i].kz != p->m[i-1].kz ) {
			c_pXn++;
			hp->pX[c_pXn].allocCoeff(p->m[i].kx );
			hp->pXkz[c_pXn] = p->m[i].kz;
		}
		if( p->m[i].kz != p->m[i-1].kz || p->m[i].kx != p->m[i-1].kx ) {
			c_pYn++;
			hp->pY[c_pYn].allocCoeff(p->m[i].ky );
			hp->pYpX[c_pYn] = c_pXn;
			hp->pYkx[c_pYn] = p->m[i].kx;
		}
		hp->pY[c_pYn].a[p->m[i].ky] = p->m[i].a;
	}
	hp->pZ.allocCoeff(p->m[0].kz );
}


/* ------------------------------------------------------------------------- */
/* compute the coefficients of a hornerpolyxyz for  y = const                   */
/* ------------------------------------------------------------------------- */

// void    set_row_of_hornerpolyxyz( hornerpolyxyz *hp,double y )
// {
//         int     i;

// 	for( i = 0; i < hp->pXn; i++ ) {
// 		memset( (void*)hp->pX[i].a,0,( hp->pX[i].n + 1 )*sizeof(double) );
// 	}

// 	for( i = 0; i < hp->pYn; i++ ) {
// 		hp->pX[hp->pYpX[i]].a[hp->pYkx[i]] = polyx_horner( hp->pY[i],y );
// 	}
// }

/* ------------------------------------------------------------------------- */
/* compute the coefficients of a hornerpolyxyz for  x = const                   */
/* ------------------------------------------------------------------------- */

// void    set_colummn_of_hornerpolyxyz( hornerpolyxyz *hp,double x )
// {
//         int     i;

// 	memset( (void*)hp->pZ.a,0,( hp->pZ.n + 1 )*sizeof(double) );

// 	for( i = 0; i < hp->pXn; i++ ) {
// 		hp->pZ.a[hp->pXkz[i]] = polyx_horner( hp->pX[i],x );
// 	}
// }

/* ------------------------------------------------------------------------- */
/* output of hornerpolyxyz                                                      */
/* ------------------------------------------------------------------------- */

// void    hornerpolyxyz_i_print( hornerpolyxyz *h )
// {
// 	int     i;

// 	fprintf( stderr,"pZ = " );

// 	polyx_print( h->pZ );

// 	fprintf( stderr,"pX =\n" );

// 	for( i = 0; i < h->pXn; i++ ) {
// 		fprintf( stderr,"    " );
// 		polyx_print( h->pX[i] );
// 	}

// 	fprintf( stderr,"pY =\n" );

// 	for( i = 0; i < h->pYn; i++ ) {
// 		fprintf( stderr,"    " );
// 		polyx_print( h->pY[i] );
// 	}
// }

/* ------------------------------------------------------------------------- */
/* end of file: hornerarith.c                                                */
/* ------------------------------------------------------------------------- */
