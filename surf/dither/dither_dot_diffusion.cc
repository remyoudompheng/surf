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
#include <assert.h>

#include "dither_dot_diffusion.h"
#include "float_buffer.h"
#include "bit_buffer.h"
#include "compfn.h"
#include "stop.h"

// ----------------------------------------------------------------------------
//  Perform the dot-diffusion algorithm from intensity to pixel
// ----------------------------------------------------------------------------

void    dither_dot_diffusion( float_buffer &intensity, bit_buffer &pixel, int print_barons_data)
{
	assert (print_barons_data >= 0 && print_barons_data <= 1);

	int main_width_data = intensity.getWidth();
	int main_height_data = intensity.getHeight();

	int     j,k;  
	int     x;   
	int     y;
	
	float   value;
	float   err; 
	
	for( k = 0; k < 64 && !stop; k++ ) {
		for( y = (*d_d_s[print_barons_data])[k].y;
		     y < main_height_data && !stop;
		     y += 8 ) {
			for( x = (*d_d_s[print_barons_data])[k].x;
			     x < main_width_data;
			     x += 8 ) {
				value = intensity.Get( x,y );
				
				if( value < 0.5 ) {
					pixel.setPixel (x,y,PIXEL_BLACK );
					err = value;
				} else {   
					pixel.setPixel (x,y,PIXEL_WHITE );
					err = value - 1.0;
				}

				for( j = 0; j < 8; j++ ) {
					intensity.Add( x + d_d_n_x[j],y + d_d_n_y[j],
						       err*(*d_d_s[print_barons_data])[k].err[j] );
				}
			}    
		}    
	}    
}
