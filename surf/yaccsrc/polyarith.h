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
/* polyarith.h: include polynomial arithmetics                               */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     14.8.94                                                         */
/* ------------------------------------------------------------------------- */

#ifndef POLYARITH_H
#define POLYARITH_H

#include "monomarith.h"

#define DEG_UNSPEC    -2
/* #define DEG_NULL      -1  */


/*****************************************************************************/
/* POLYNIMIALS IN X,Y AND Z                                                  */
/*****************************************************************************/

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Define some struct for:                                                   */
/*                 n-1                                                       */
/*                 ---                                                       */
/*                 \          kx[k]   ky[k]   kz[k]                          */
/*                  >   a[k]*x      *y      *z                               */
/*                 /                                                         */
/*                 ---                                                       */
/*                 k=0                                                       */
/*                                                                           */
/* ------------------------------------------------------------------------- */

typedef struct
{
	int     n;                          /* number of monomials               */
	int     deg;                        /* degree of polnomial               */

	monxyz  *m;                         /* array of monomials                */

} polyxyz;

/* ------------------------------------------------------------------------- */
/*  Global data                                                              */
/* ------------------------------------------------------------------------- */

extern  polyxyz NULLPOLYXYZ;

/* ------------------------------------------------------------------------- */
/*  Prototypes                                                               */
/* ------------------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ------------------------------------------------------------------------- */
/*  Constructors and destructors and copy                                    */
/* ------------------------------------------------------------------------- */

void    new_coeff_polyxyz   ( polyxyz*,int );
void    renew_coeff_polyxyz ( polyxyz*,int );
void    delete_coeff_polyxyz( polyxyz* );

void    delete_coeff_polyxyz_control( polyxyz* );

polyxyz polyxyz_copy      ( polyxyz* );

/* ------------------------------------------------------------------------- */
/*  Sort and collect                                                         */
/* ------------------------------------------------------------------------- */

void    polyxyz_collect   ( polyxyz* );
void    polyxyz_sort      ( polyxyz* );
void    polyxyz_set_degree( polyxyz* );

/* ------------------------------------------------------------------------- */
/*  Arithmetics                                                              */
/* ------------------------------------------------------------------------- */

double  polyxyz_maxabs_coeff    ( polyxyz* );
void    polyxyz_norm_self       ( polyxyz* );
polyxyz polyxyz_add             ( polyxyz*,polyxyz* );
void    polyxyz_add_self        ( polyxyz*,polyxyz* );
polyxyz polyxyz_sub             ( polyxyz*,polyxyz* );
polyxyz polyxyz_mult            ( polyxyz*,polyxyz* );
void    polyxyz_mult_self       ( polyxyz*,polyxyz* );
void    polyxyz_mult_monxyz_self( polyxyz*,monxyz* );
void    polyxyz_mult_double_self( polyxyz*,double );
void    polyxyz_div_double_self ( polyxyz*,double );
polyxyz polyxyz_power           ( polyxyz*,int );
void    polyxyz_neg_self        ( polyxyz* );
void    polyxyz_scale_self      ( polyxyz*,double,double,double );
int     polyxyz_max_coeff       ( int );
polyxyz polyxyz_shift_x         ( polyxyz*,double );
void    polyxyz_shift_x_self    ( polyxyz*,double );
polyxyz polyxyz_shift_y         ( polyxyz*,double );
void    polyxyz_shift_y_self    ( polyxyz*,double );
polyxyz polyxyz_shift_z         ( polyxyz*,double );
void    polyxyz_shift_z_self    ( polyxyz*,double );
void    polyxyz_shift_self      ( polyxyz*,double,double,double );
polyxyz polyxyz_rotate_x        ( polyxyz*,double );
void    polyxyz_rotate_x_self   ( polyxyz*,double );
polyxyz polyxyz_rotate_y        ( polyxyz*,double );
void    polyxyz_rotate_y_self   ( polyxyz*,double );
polyxyz polyxyz_rotate_z        ( polyxyz*,double );
void    polyxyz_rotate_z_self   ( polyxyz*,double );
void    polyxyz_rotate_self     ( polyxyz*,double,double,double );
polyxyz polyxyz_perspective     ( polyxyz*,double );
void    polyxyz_perspective_self( polyxyz*,double );
polyxyz polyxyz_hessian_surface     ( polyxyz* );
void    polyxyz_hessian_surface_self( polyxyz* );
polyxyz polyxyz_hessian_curve       ( polyxyz* );
void    polyxyz_hessian_curve_self  ( polyxyz* );
polyxyz polyxyz_hesse           ( polyxyz* );
void    polyxyz_hesse_self      ( polyxyz* );
polyxyz polyxyz_dx              ( polyxyz* );
void    polyxyz_dx_self         ( polyxyz* );
polyxyz polyxyz_dx_n            ( polyxyz*,int );
void    polyxyz_dx_n_self       ( polyxyz*,int );
polyxyz polyxyz_dy              ( polyxyz* );
void    polyxyz_dy_self         ( polyxyz* );
polyxyz polyxyz_dy_n            ( polyxyz*,int );
void    polyxyz_dy_n_self       ( polyxyz*,int );
polyxyz polyxyz_dz              ( polyxyz* );
void    polyxyz_dz_self         ( polyxyz* );
polyxyz polyxyz_dz_n            ( polyxyz*,int );
void    polyxyz_dz_n_self       ( polyxyz*,int );
polyxyz polyxyz_dw              ( polyxyz* );
void    polyxyz_dw_self         ( polyxyz* );
polyxyz polyxyz_dw_n            ( polyxyz*,int );
void    polyxyz_dw_n_self       ( polyxyz*,int );
polyxyz polyxyz_dvector         ( polyxyz*,int,int,int,int );
void    polyxyz_dvector_self    ( polyxyz*,int,int,int,int );
polyxyz polyxyz_dmonxyz         ( polyxyz*,monxyz*,int );
void    polyxyz_dmonxyz_self    ( polyxyz*,monxyz*,int );


/* ------------------------------------------------------------------------- */
/*  Conversions                                                              */
/* ------------------------------------------------------------------------- */

polyxyz int2polyxyz      ( int i );
polyxyz double2polyxyz   ( double d );
polyxyz monxyz2polyxyz( monxyz *m );

/* ------------------------------------------------------------------------- */
/*  Output                                                                   */
/* ------------------------------------------------------------------------- */

void    polyxyz_print( polyxyz* );

#ifdef  __cplusplus
}
#endif  /* __cplusplus */


#endif  /* POLYARITH_H */

/* ------------------------------------------------------------------------- */
/* end of file: polyarith.h                                                  */
/* ------------------------------------------------------------------------- */
