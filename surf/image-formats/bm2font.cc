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
// File      bm2font.cc
// Author:   Stephan Endrass
// Address:  endrass@mi.uni-erlangen.de
// Date:     21.5.96
// ============================================================================


#include <stdlib.h>
#include <stdio.h>

#include "bit_buffer.h"
#include "Misc.h"

// ----------------------------------------------------------------------------
//  Produce bm2font output
// ----------------------------------------------------------------------------

bool bm2fontprint (class bit_buffer &pixel, const char *bm2fontfilename,
		   int paper_width, int paper_height,
		   int res)
{
	FILE *fp_bm2font = fopen( ".surf.b2f","w" );
	if (fp_bm2font==0) {
		Misc::alert ("Could not open file");
		return false;
	}

	int px,py;
	unsigned char byte;

	for( py = 0; py < paper_height; py++ ) {
		for( px = 0; px < paper_width; px += 8 ) {
			byte = 255-pixel.getByte(px,py);
			fputc( byte,fp_bm2font );
		}
	}
	fclose( fp_bm2font );

	//  execute bm2font on the result
	// -------------------------------

	char command[400];
	int  rest  = paper_width%8;
	int  bytes = paper_width/8 + ( rest == 0 ? 0 : 1 );

	sprintf( command,"bm2font .surf.b2f -jn -f%s -l%d -gn -h%d -v%d \n",
		 bm2fontfilename,bytes,res,res );
	system( command );
	system( "/bin/rm .surf.b2f" );

	return  true;
}
