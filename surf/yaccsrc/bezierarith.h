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





/* ------------------------------------------------------------------------- */
/* bezierarith.h: include bezier arithmetic                                  */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     8 marcc 1996                                                    */
/* ------------------------------------------------------------------------- */

#ifndef BEZIERARITH_H
#define BEZIERARITH_H


#include "polyx.h"

/* ------------------------------------------------------------------------- */
/*  This is a bezier-function                                                */
/* ------------------------------------------------------------------------- */

typedef struct
{
    double  *p;                         /* knots                             */
                                        /*                                   */
    int     n;                          /* number of knots                   */
    double  a;                          /* left interval border              */
    double  b;                          /* right interval border             */

} bezier;

/* ------------------------------------------------------------------------- */
/*  Prototypes                                                               */
/* ------------------------------------------------------------------------- */


int     zero_of_bezier_single_root_no_help( polyx*,double,double,double* );
int     zero_of_bezier_single_root_no_help_open( polyx*,double,double,double* );
int     zero_of_bezier_multi_root_no_help ( polyx*,double,double,double* );
int     zero_of_bezier_single_root_help   ( polyx*,double,double,
					    double*,double );
int     zero_of_bezier_single_root_help_open   ( polyx*,double,double,
					    double*,double );
    

#endif  /* BEZIERARITH_H */

/* ------------------------------------------------------------------------- */
/* end of file: bezierarith.h                                                */
/* ------------------------------------------------------------------------- */
