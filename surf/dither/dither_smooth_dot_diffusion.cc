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

#include <stdlib.h>
#include <assert.h>

#include "float_buffer.h"
#include "bit_buffer.h"
#include "compfn.h"
#include "stop.h"

// ----------------------------------------------------------------------------
//  Perform the smooth-dot-diffusion algorithm from intensity to pixel
// ----------------------------------------------------------------------------

void    dither_smooth_dot_diffusion( float_buffer &intensity, bit_buffer &pixel, int print_barons_data)
{
	assert (print_barons_data >= 0 && print_barons_data <= 1);

	int main_width_data = intensity.getWidth();
	int main_height_data = intensity.getHeight();

	int     j,k;
	int     x;
	int     y;
	
	float   value;
	float   compare;
	float   err;
	
	for( k = 0; k < 64 && !stop; k++ ) {
		compare = 0.5/( 64 - k );
		
		for( y = (*s_d_d_s[print_barons_data])[k].y;
		     y < main_height_data && !stop;
		     y += 8 ) {
			for( x = (*s_d_d_s[print_barons_data])[k].x;
			     x < main_width_data;
			     x += 8 ) {
				value = intensity.Get( x, y );
				
				if( value < compare ) {
					pixel.setPixel(x,y,PIXEL_BLACK );
					err = value;
				} else {   
					pixel.setPixel(x,y,PIXEL_WHITE );
					err = value - 1.0;
				}

				if( k < 63 ) {
					err /=  63 - k;
					
					for( j = 0;
					     j < (*s_d_d_s[print_barons_data])[k].n;
					     j++ ) {
						intensity.Add(
							x + (*s_d_d_s[print_barons_data])[k].err_x[j],
							y + (*s_d_d_s[print_barons_data])[k].err_y[j],
							err );
					}
				}    
			}    
		}
	}    
}
