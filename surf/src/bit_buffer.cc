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




#include <limits.h>
#include <iostream.h>

#include "bit_buffer.h"

void bit_buffer::write_image(const char* filename, ImageFormats::Format* fmt,
			     int paper_width, int paper_height, int resolution, bool fromDlg)
{
	fmt->saveDitheredImage(filename, *this, paper_width, paper_height, resolution, fromDlg);
}


void bit_buffer::setSize (int w, int h)
{
	width         = w;
	height        = h;

	bytesPerRow = width / CHAR_BIT + (width % CHAR_BIT ? 1 : 0);

	n_bytes       = height*bytesPerRow;

	delete []b;

	if (n_bytes > 0)
		b = new guint8[n_bytes];
	else {
		b = 0;
	}
}

bit_buffer::bit_buffer()
{
	width=0;
	height=0;
	n_bytes=0;
	b = 0;
}

bit_buffer::~bit_buffer()
{
	delete []b;
}



void bit_buffer::setBlack()
{
	int i;
 
	for( i=0; i<n_bytes; i++ ) {
		b[i] = 0;
	}
}


bool bit_buffer::getPixel (int x, int y)
{
	if( x < 0 || x >= width || y < 0 || y >= height ) {
		return  PIXEL_WHITE;
	} else {
		guint8 c = b[x / CHAR_BIT + y*bytesPerRow];
		return c & ( (1 << (CHAR_BIT-1)) >> x % CHAR_BIT) ? PIXEL_BLACK : PIXEL_WHITE;
	}
}

void bit_buffer::setPixel (int x, int y, int value)
{
	if( x >= 0 && x < width && y >= 0 && y < height ) {
		guint8 &c = b[x / CHAR_BIT + y*bytesPerRow];

		if (value == PIXEL_BLACK)
			c |= ( (1 << (CHAR_BIT-1)) >> x % CHAR_BIT);
		else
			c &= ~( (1 << (CHAR_BIT-1)) >> x % CHAR_BIT);
	}

}

