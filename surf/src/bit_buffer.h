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


#ifndef BITBUFFER_H
#define BITBUFFER_H

#include <assert.h>
#include <limits.h>
#include <stdio.h>


#define PIXEL_BLACK     0
#define PIXEL_WHITE     1

class bit_buffer
{
private:
	bit_buffer (const bit_buffer &);
	void operator=(const bit_buffer &);

public:
	bit_buffer();
	~bit_buffer();
	void setSize(int w, int h);
	void setBlack();

	int getWidth()  {return width;};
	int getHeight() {return height;};
	
	bool getPixel (int x, int y);
	void setPixel (int x, int y, int val);

	unsigned char getByte (int x, int y)
		{
			assert (x % CHAR_BIT == 0);
			if( x < 0 || x >= width || y < 0 || y >= height ) {
				return  0;
			} else {
				return b[x / CHAR_BIT + y*bytesPerRow];
			}
		}


	void write_as_pgm (FILE *f);
	void write_as_pbm (FILE *f);

private:
	int     width;                      // Width of image
	int     height;                     // Height of image

	int     n_bytes;                    // Number of bytes

	int bytesPerRow;

	unsigned char *b;             // Image buffer

};

#endif
