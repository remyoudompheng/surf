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
/* simple.h                                                                  */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     14.8.94                                                         */
/* ------------------------------------------------------------------------- */

#ifndef SIMPLE_H
#define SIMPLE_H

extern  double  epsilon;



/* ------------------------------------------------------------------------- */
/*  The sun's power function behaves somewhat ugly ...                       */
/* ------------------------------------------------------------------------- */

#ifdef SUN
#define sunpow( x,y )     ( (y) == 0.0 && (x) == 0.0 ? 1.0 : pow( (x),(y) ) )
#endif  /*  SUN   */

/* ------------------------------------------------------------------------- */
/*  Prototypes                                                               */
/* ------------------------------------------------------------------------- */

int     double_equal( double,double );

int     binom_coeff( int,int );
int     multinom_coeff( int,int*,int );
// int     itotexascii( int,char*,int );


#endif  // SIMPLE_H

/* ------------------------------------------------------------------------- */
/* end of file: simple.h                                                     */
/* ------------------------------------------------------------------------- */
