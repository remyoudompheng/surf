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
/* monomarith.h: monomial                                                    */
/* ------------------------------------------------------------------------- */

#ifndef MONOMARITH_H
#define MONOMARITH_H

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Define struct for:                                                        */
/*                                                                           */
/*                         kx   ky                                           */
/*                      a*x   *y                                             */
/*                                                                           */
/* ------------------------------------------------------------------------- */

typedef struct
{
    double  a;                          /* Coefficient                       */
    int     kx;                         /* Exponent of x                     */
    int     ky;                         /* Exponent of y                     */

} monxy;

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Define struct for:                                                        */
/*                                                                           */
/*                         kx   ky    kz                                     */
/*                      a*x   *y    *z                                       */
/*                                                                           */
/* ------------------------------------------------------------------------- */

typedef struct
{
    double  a;                          /* Coefficient                       */ 
    int     kx;                         /* Exponent of x                     */
    int     ky;                         /* Exponent of y                     */
    int     kz;                         /* Exponent of z                     */

} monxyz;

/* ------------------------------------------------------------------------- */
/* global data                                                               */
/* ------------------------------------------------------------------------- */


extern  monxy   NULLMONXY;
extern  monxyz  NULLMONXYZ;
extern  monxy   ONEMONXY;
extern  monxyz  ONEMONXYZ;

/* ------------------------------------------------------------------------- */
/* function headers                                                          */
/* ------------------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ------------------------------------------------------------------------- */
/* constructors and destructors and copy                                     */
/* ------------------------------------------------------------------------- */

monxyz  *new_monxyz   ( int );
monxyz  *renew_monxyz ( monxyz*,int );
monxyz  *delete_monxyz( monxyz* );
void    copy_monxyz   ( monxyz*,monxyz*,int );

/* ------------------------------------------------------------------------- */
/* compare functions                                                         */
/* ------------------------------------------------------------------------- */

int     monxyz_power_equal( monxyz*,monxyz* );

/* ------------------------------------------------------------------------- */
/* arithmetics                                                               */
/* ------------------------------------------------------------------------- */

void    monxyz_add_self        ( monxyz*,monxyz* );
monxyz  monxyz_mult            ( monxyz,monxyz* );
void    monxyz_mult_self       ( monxyz*,monxyz* );
monxyz  monxyz_mult_double     ( monxyz,double );
void    monxyz_mult_double_self( monxyz*,double );
monxyz  monxyz_div_double      ( monxyz,double );
void    monxyz_div_double_self ( monxyz*,double );
monxyz  monxyz_power           ( monxyz,int );
monxyz  monxyz_neg             ( monxyz );
void    monxyz_neg_self        ( monxyz* );
monxyz  monxyz_dx              ( monxyz );
void    monxyz_dx_self         ( monxyz* );
monxyz  monxyz_dx_n            ( monxyz,int );
void    monxyz_dx_n_self       ( monxyz*,int );
monxyz  monxyz_dy              ( monxyz );
void    monxyz_dy_self         ( monxyz* );
monxyz  monxyz_dy_n            ( monxyz,int );
void    monxyz_dy_n_self       ( monxyz*,int );
monxyz  monxyz_dz              ( monxyz );
void    monxyz_dz_self         ( monxyz* );
monxyz  monxyz_dz_n            ( monxyz,int );
void    monxyz_dz_n_self       ( monxyz*,int );
monxyz  monxyz_dw              ( monxyz,int );
void    monxyz_dw_self         ( monxyz*,int );
monxyz  monxyz_dw_n            ( monxyz,int,int );
void    monxyz_dw_n_self       ( monxyz*,int,int );
monxyz  monxyz_dvector         ( monxyz,int,int,int,int,int );
void    monxyz_dvector_self    ( monxyz*,int,int,int,int,int );
monxyz  monxyz_dmonxyz         ( monxyz,int,monxyz*,int );
void    monxyz_dmonxyz_self    ( monxyz*,int,monxyz*,int );

/* ------------------------------------------------------------------------- */
/* conversions                                                               */
/* ------------------------------------------------------------------------- */

monxyz  int2monxyz   ( int );
monxyz  double2monxyz( double );
monxyz  atom         ( char* );

/* ------------------------------------------------------------------------- */
/* output                                                                    */
/* ------------------------------------------------------------------------- */

void    monxyz_print( monxyz* );

/* ------------------------------------------------------------------------- */
/* end of function headers                                                   */
/* ------------------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif  /* __cplusplus */


#endif  /* MONOMARITH_H */

/* ------------------------------------------------------------------------- */
/* end of file: monomarith.h                                                 */
/* ------------------------------------------------------------------------- */
