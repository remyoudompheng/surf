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

#include "float_buffer.h"
#include "bit_buffer.h"
#include "stop.h"

// ----------------------------------------------------------------------------
//  Perform the stucki algorithm from intensity to pixel
// ----------------------------------------------------------------------------

void dither_stucki (float_buffer &intensity, bit_buffer &pixel,
		    bool print_random_weights_data, double print_weight_data, bool print_serpentine_raster_data)

{
	float   value;
	float   err;
	float   err1;
	float   err2;
	float   err4;
	float   err8;
	float   weight_8;
	float   weight_4;
	float   weight_2;
	float   weight_1;
	float   weight_rand_8 = 0.0;
	float   weight_rand_4 = 0.0;
	float   weight_rand_2 = 0.0;
	float   weight_rand_1 = 0.0;
	
	bool even = true;
	int     px_m2,px_m1,px,px_p1,px_p2,px_c,px_a;
	int                 py,py_p1,py_p2;
	
	if( print_random_weights_data == 0 ) {
		weight_rand_8 = print_weight_data*8.0;
		weight_rand_4 = print_weight_data*4.0;
		weight_rand_2 = print_weight_data*2.0;
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
			// value = float_buffer_x_y_get( intensity,px,py );
			
			if( value > 0.5 ) {
				pixel.setPixel(px,py,PIXEL_WHITE );
				err = value - 1.0;
			} else {   
				pixel.setPixel(px,py,PIXEL_BLACK );
				err = value;
			}

			if( print_random_weights_data != 0 ) {
				err1 = err/42;
				err2 = err1*2;
				err4 = err1*4;
				err8 = err1*8;
				
				intensity.Add( px_p1,py   ,err8 );
				intensity.Add( px_p2,py   ,err4 );

				intensity.Add( px_m2,py_p1,err2 );
				intensity.Add( px_m1,py_p1,err4 );
				intensity.Add( px   ,py_p1,err8 );
				intensity.Add( px_p1,py_p1,err4 );
				intensity.Add( px_p2,py_p1,err2 );
				
				intensity.Add( px_m2,py_p2,err1 );
				intensity.Add( px_m1,py_p2,err2 );
				intensity.Add( px   ,py_p2,err4 );
				intensity.Add( px_p1,py_p2,err2 );
				intensity.Add( px_p2,py_p2,err1 );
			} else {   
				err /= 42;
				
				weight_8 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_8;
				weight_4 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_4;
				weight_2 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_2;
				weight_1 = ( rand( )/4294967295.0 - 1.0 )*weight_rand_1;
				
				intensity.Add( px_p1,py   , err*( 8 + weight_8 ) );
				intensity.Add( px_p2,py   , err*( 4 + weight_4 ) );
				
				intensity.Add( px_m2,py_p1, err*( 2 + weight_2 ) );
				intensity.Add( px_m1,py_p1, err*( 4 - weight_4 ) );
				intensity.Add( px   ,py_p1, err*( 8 - weight_8 ) );
				intensity.Add( px_p1,py_p1, err*( 4 + weight_4 ) );
				intensity.Add( px_p2,py_p1, err*( 2 - weight_2 ) );
				
				intensity.Add( px_m2,py_p2, err*( 1 + weight_1 ) );
				intensity.Add( px_m1,py_p2, err*( 2 + weight_2 ) );
				intensity.Add( px   ,py_p2, err*( 4 - weight_4 ) );
				intensity.Add( px_p1,py_p2, err*( 2 - weight_2 ) );
				intensity.Add( px_p2,py_p2, err*( 1 - weight_1 ) );
				
			}
		}
	}
}
