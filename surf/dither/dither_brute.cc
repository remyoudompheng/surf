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
//  Perform brute force dithering, just for curves ...
// ----------------------------------------------------------------------------

void dither_brute (const float_buffer &intensity, bit_buffer &pixel) 
{ 
	const int width = intensity.getWidth();
	const int height = intensity.getHeight();	
	pixel.setSize(width, height);

	int     px;
	int     py;

	for( py = 0; py < height && !stop; py++ ) {
		for( px = 0; px < width; px++ ) {
			pixel.setPixel (px,py,
					intensity.Get( px,py ) > 0.5 ? PIXEL_WHITE : PIXEL_BLACK  );
		}

	}
}
