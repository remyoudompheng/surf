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

#include <math.h>
#include <stdlib.h>
#include "dither_pixel_radius_adjust.h"
#include "float_buffer.h"
#include "stop.h"
// ----------------------------------------------------------------------------
//  Perform pixel-radius-correction (printer) on intensity
// ----------------------------------------------------------------------------

void dither_pixel_radius_adjust (float_buffer &intensity, float rel_radius)
{
	int     i;

	float   alpha1, alpha2, alpha3;
	float   b, c, e, h, l;
	float   beta, gamma, delta;
	
	float   d = 2.54/300;
	float   r = rel_radius*d;
	float   a = d/M_SQRT2;


	if( rel_radius < 1.0/M_SQRT2 ) {
		c = sqrt( r*r - d*d/4 );
		
		gamma = acos( d/2/r );

		alpha1 = ( r*r*( 3*M_PI - 8*gamma ) + 4*c*d )/4/d/d;

		alpha2 = r*r*M_PI/2/d/d;

		alpha3 = r*r*M_PI/4/d/d;
	} else {
		b = sqrt( r*r - a*a );
		e = a - b;
		h = e/M_SQRT2;
		l = sqrt( r*r - h*h );
		
		beta  = acos( a/r );
		delta = acos( l/r );
		
		alpha1 = ( r*r*(2*M_PI - 8*beta) +8*a*b )/4/d/d +
			( h*h + l*h -r*r*delta )/d/d;
		
		alpha2 = ( r*r*(2*M_PI - 8*beta) + 8*a*b )/4/d/d;
		
		alpha3 = r*r*M_PI/4/d/d;
	}
	
	float   b1 = 1.0 - alpha1;
	float   b2 = 1.0 - alpha2;
	float   b3 = 1.0 - alpha3;
	
	float   m1 = 1.0/4/( alpha1 - 1.0 );
	float   m2 = 1.0/4/( alpha2 - alpha1 );
	float   m3 = 1.0/4/( alpha3 - alpha2 );
	float   m4 = 1.0/4/( -alpha3 );
	
	for( i = 0; i < intensity.getHeight() && !stop; i++ ) {
		for( int j = 0; j < intensity.getWidth(); j++ ) {
			float value = intensity.Get( j, i );
			if( value < b1 )
				value = m1 * value + 1.0;
			else if( value < b2 )
				value = m2 * ( value - b1 ) + 0.75;
			else if( value < b3 )
				value = m3 * ( value - b2 ) + 0.5;
			else
				value = m4 * ( value - b3 ) + 0.25;
			value = 1.0 - value;
			intensity.Set( j, i, value );
		}
	}
}
