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





// ===========================================================================
// File      compfn.h
// Author:   Stephan Endrass
// Address:  endrass@mi.uni-erlangen.de
// Date:     3.2.95
// ============================================================================

#ifndef COMPFN_H
#define COMPFN_H


// -------------------------------
//  Information for dot diffusion
// -------------------------------

typedef struct
{
	int     x;
	int     y;
 
	double  err[8];
 
} dot_diffusion_struct;
 
// --------------------------------------
//  Information for smooth dot diffusion
// --------------------------------------

typedef struct
{
	int     x;
	int     y;
 
	int     n;
	int     *err_x;
	int     *err_y;
	double  *err;
 
} smooth_dot_diffusion_struct;

// ----------------------------------------------------------------------------
//  Function prototypes
// ----------------------------------------------------------------------------

void    init_pattern( void );
void    init_dot_diffusion( void );
void    deinit_dot_diffusion();

// ----------------------------------------------------------------------------
//  Global variables
// ----------------------------------------------------------------------------

extern  int     d_d_n_x[8];
extern  int     d_d_n_y[8];
extern  int     d_d_w[8];
extern  int     d_d_n_n_x[9];
extern  int     d_d_n_n_y[9];

extern  dot_diffusion_struct        (*d_d_s[2])[64];    
extern  smooth_dot_diffusion_struct (*s_d_d_s[2])[64];

extern  float   (*compare_function[2][3])( int,int );

#endif  /* COMPFN_H */

// ============================================================================
// End of compfn.h
// ============================================================================
