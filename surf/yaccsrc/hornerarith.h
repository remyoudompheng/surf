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




/* ------------------------------------------------------------------------- */
/* hornerarith.h: include fast multivariate polynomial evaluation            */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     14.8.94                                                         */
/* ------------------------------------------------------------------------- */

#ifndef HORNERARITH_H
#define HORNERARITH_H

#include "polyx.h"
#include "polyarith.h"

/* ------------------------------------------------------------------------- */
/*  structure for fast trivariate polynomial evaluation                      */
/* ------------------------------------------------------------------------- */

class hornerpolyxyz
{
private:
	hornerpolyxyz (const hornerpolyxyz &);
	void operator=(const hornerpolyxyz &);
	

public:
	hornerpolyxyz();
	hornerpolyxyz(const polyxyz &p);
	~hornerpolyxyz();


	void setRow (double y);
	void setColumn (double x);

public:
	polyx   *pY;                        /* polynomials in y are coeffs of pX */

	int     *pYpX;                      /* relational index                  */
	int     *pYkx;                      /* relational index                  */
	int     pYn;                        /* number of pY                      */
                                        /*                                   */
	polyx   *pX;                        /* polynomials in x are coeffs of pZ */

	int     *pXkz;                      /* relational index                  */
	int     pXn;                        /* number of pX                      */

	polyx   pZ;                         /* polynomial in z                   */


private:
	void setNull();
	void free();


	static void polyxyz_to_horner( hornerpolyxyz *hp, const polyxyz *p );
};


#endif  /* HORNERARITH_H */


/* ------------------------------------------------------------------------- */
/* end of file: hornerarith.h                                                */
/* ------------------------------------------------------------------------- */
