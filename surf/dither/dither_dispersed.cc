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
//  Perform the dispersed-dot-ordered-dither algorithm from intensity to pixel
// ----------------------------------------------------------------------------

void dither_dispersed (float_buffer &intensity, bit_buffer &pixel, int print_pattern_size_data)
{
	assert(print_pattern_size_data >= 0 && print_pattern_size_data <= 2);	
	float (*cmp) (int, int) = compare_function[1][print_pattern_size_data];

	int     px;
	int     py;

	for( py = 0; py < intensity.getHeight() && !stop; py++ ) {
		for( px = 0; px < intensity.getWidth(); px++ ) {
			pixel.setPixel (px,py,
					intensity.Get( px,py ) > cmp(px,py) ? PIXEL_WHITE : PIXEL_BLACK );
		}
	}
}
