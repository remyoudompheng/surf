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



#include <stdlib.h>

#include "dither_jarvis_judis_ninke.h"
#include "float_buffer.h"
#include "bit_buffer.h"
#include "stop.h"

void dither_jarvis_judis_ninke (float_buffer &intensity, bit_buffer &pixel,
				bool print_random_weights_data, double print_weight_data, bool print_serpentine_raster_data)

{
	float   value;
	float   err;
	float   err1;
	float   err3;
	float   err5;
	float   err7;
	float   weight_7;
	float   weight_5;
	float   weight_3;
	float   weight_1;
	float   weight_rand_7 = 0.0;
	float   weight_rand_5 = 0.0;
	float   weight_rand_3 = 0.0;
	float   weight_rand_1 = 0.0;
	
	bool     even = true;
	int     px_m2,px_m1,px,px_p1,px_p2,px_c,px_a;
	int                 py,py_p1,py_p2;
	
	if( print_random_weights_data == 0 ) {
		weight_rand_7 = print_weight_data*7.0;
		weight_rand_5 = print_weight_data*5.0;
		weight_rand_3 = print_weight_data*3.0;
		weight_rand_1 = print_weight_data*1.0;
	}

	for( py = 0, py_p1 = 1, py_p2 = 2;
	     py < intensity.getHeight() && !stop;
	     py++,   py_p1++,   py_p2++, even = !even ) {
		if( print_serpentine_raster_data == 0 && !even ) {
			px_m2 = intensity.getWidth() + 1;
			px_m1 = intensity.getWidth();
			px    = intensity.getWidth() - 1;
			px_p1 = intensity.getWidth() - 2;
			px_p2 = intensity.getWidth() - 3;
			px_a  = -1;
		} else {
			px_m2 = -2;
			px_m1 = -1;
			px    = 0;
			px_p1 = 1;
			px_p2 = 2;
			px_a  = 1;
		}
		
		for( px_c = 0; px_c < intensity.getWidth();
		     px_m2 += px_a,
			     px_m1 += px_a,
			     px    += px_a,
			     px_p1 += px_a,
			     px_p2 += px_a,
			     px_c++ ) {
			value = intensity.Get( px, py );
			
			
			if( value > 0.5 ) {
				pixel.setPixel(px,py,PIXEL_WHITE );
				err = value - 1.0;
			} else {   
				pixel.setPixel (px,py,PIXEL_BLACK );
				err = value;
			}
 
			if( print_random_weights_data != 0 ) {
				err1 = err/48;
				err3 = err1*3;
				err5 = err1*5;
				err7 = err1*7;
				
				intensity.Add( px_p1,py   ,err7 );
				intensity.Add( px_p2,py   ,err5 );
				
				intensity.Add( px_m2,py_p1,err3 );
				intensity.Add( px_m1,py_p1,err5 );
				intensity.Add( px   ,py_p1,err7 );
				intensity.Add( px_p1,py_p1,err5 );
				intensity.Add( px_p2,py_p1,err3 );
				
				intensity.Add( px_m2,py_p2,err1 );
				intensity.Add( px_m1,py_p2,err3 );
				intensity.Add( px   ,py_p2,err5 );
				intensity.Add( px_p1,py_p2,err3 );
				intensity.Add( px_p2,py_p2,err1 );
			} else {   
				err /= 48;
				
				weight_7 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_7;
				weight_5 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_5;
				weight_3 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_3;
				weight_1 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_1;
				
				intensity.Add( px_p1,py   , err*( 7 + weight_7 ) );
				intensity.Add( px_p2,py   , err*( 5 + weight_5 ) );
				
				intensity.Add( px_m2,py_p1, err*( 3 + weight_3 ) );
				intensity.Add( px_m1,py_p1, err*( 5 - weight_5 ) );
				intensity.Add( px   ,py_p1, err*( 7 - weight_7 ) );
				intensity.Add( px_p1,py_p1, err*( 5 + weight_5 ) );
				intensity.Add( px_p2,py_p1, err*( 3 - weight_3 ) );
				
				intensity.Add( px_m2,py_p2, err*( 1 + weight_1 ) );
				intensity.Add( px_m1,py_p2, err*( 3 + weight_3 ) );
				intensity.Add( px   ,py_p2, err*( 5 - weight_5 ) );
				intensity.Add( px_p1,py_p2, err*( 3 - weight_3 ) );
				intensity.Add( px_p2,py_p2, err*( 1 - weight_1 ) );
				
			}
		}
	}    
}
