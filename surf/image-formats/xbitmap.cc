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



// ============================================================================
// File      xbitmap.cc
// Author:   Stephan Endrass
// Address:  endrass@mi.uni-erlangen.de
// Date:     3.2.95
// ============================================================================

#include <stdio.h>
#include <limits.h>

#include "xbitmap.h"
#include "bit_buffer.h"


static inline unsigned char rotateByte (unsigned char c)
{
	unsigned char retval=0;
	int i;
	for (i=0; i< CHAR_BIT; i++) {
		retval <<= 1;
		retval |= c & 1;
		c >>= 1;
	}
	return retval;
}
// ----------------------------------------------------------------------------
//  Produce a X11 bitmap
// ----------------------------------------------------------------------------

bool bitmapprint (bit_buffer &pixel, FILE *fp_bitmap, const char *bitmapfilename,
		  int paper_width, int paper_height)
{
	int     px,py,byte;
	int     bytes_per_line = paper_width/8 + ( paper_width%8 == 0 ? 0 : 1 );
	int     bits_per_line = bytes_per_line*8;
	int     komma_end = paper_height*bytes_per_line - 1;
	int     count = 0;


	if ( fp_bitmap == 0 ) {
		return  false;
	}

	fprintf( fp_bitmap,"#define %s_width %d\n",bitmapfilename,bits_per_line );
	fprintf( fp_bitmap,"#define %s_height %d\n",bitmapfilename,paper_height );
	fprintf( fp_bitmap,"static char %s_bits[] = {\n",bitmapfilename );

	for( py = 0; py < paper_height; py++ ) {
		fprintf( fp_bitmap,"   " );		

		for( px = 0; px < paper_width; px += 8 ) {			
			byte = rotateByte(pixel.getByte(px,py));
			fprintf( fp_bitmap," 0x%.2x",byte );
			count++;

			if( count < komma_end ) {
				fprintf( fp_bitmap,"," );
			}
		}
	}
	fprintf( fp_bitmap,"};\n" );

	return  true;
}

// ============================================================================
// End of xbitmap.cc
// ============================================================================
