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

#ifndef SYMTAB_H
#define SYMTAB_H


#include "polyarith.h"


// 
// --- "cutsurface"
//
// extern polyxyz sym_cutsurface;
extern polyxyz sym_cutsurfaces[9];

//
// --- "color_file_format" color_ouput_data
//

extern  int     color_output_data;
extern  const int     color_output_xwd_data;  // "xwd"
extern  const int     color_output_sun_data;  // "sun"
extern  const int     color_output_ppm_data;  // "ppm"




//
// --- "color_file_colormap" colormap_output_data
//

extern  int     colormap_output_data;

extern  const int     colormap_output_netscape_data;	// "Netscape"
extern  const int     colormap_output_optimized_data;	// "Optimized"
extern  const int     colormap_output_true_color_data;	// "TrueColor"


//
//  --- "dither_file_format"
//
extern  int     print_output_data;

extern  const int     print_output_ps_data;
extern  const int     print_output_eps_data;
extern  const int     print_output_bitmap_data;
extern  const int     print_output_tiff_data;
extern  const int     print_output_bm2font_data;
extern  const int     print_output_pgm_data;
extern  const int     print_output_pbm_data;
#endif
