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

#include "symtab.h"
#include "gui_config.h"
#include "RgbBuffer.h"

// 
// --- "cutsurface"
//

// polyxyz sym_cutsurface = NULLPOLYXYZ;

polyxyz sym_cutsurfaces[9];

//
// --- "color_file_format" color_ouput_data
//

int     color_output_data                = 1;

const int     color_output_xwd_data             = 0;
const int     color_output_sun_data             = 1;
const int     color_output_ppm_data		= 2;
const int     color_output_jpeg_data            = 3;





//
// --- "color_file_colormap" colormap_output_data
//
int     colormap_output_data                = colormap_output_true_color_data;


const int     colormap_output_netscape_data	= 0;	
const int     colormap_output_optimized_data	= 1;	
const int     colormap_output_true_color_data	= 2;	



//
//  --- "dither_file_format"
//

int     print_output_data;

const int     print_output_ps_data             = 0;
const int     print_output_eps_data            = 1;
const int     print_output_bitmap_data         = 2;
const int     print_output_tiff_data           = 3;
const int     print_output_bm2font_data        = 4;
const int     print_output_pgm_data            = 5;
const int     print_output_pbm_data            = 6;
