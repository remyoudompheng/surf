/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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



#include "antialiasing_factor.h"


// ------------------------------------------------------------------------sk
// sk :Bartlett - FILTER  for antialising in REFINE_PIXEL (surface.cc) ...
//     for different sizes
// ------------------------------------------------------------------------sk
static  int     antialias_filter_2x2[3][3] =
{
	{  1,  1 },
	{  1,  1 }
};

static  int     antialias_filter_3x3[3][3] =
{
	{  1,  2,  1 },
	{  2,  4,  2 },
	{  1,  2,  1 }
};

static  int     antialias_filter_4x4[4][4] =
{
	{  1,  2,  2,  1 },
	{  2,  4,  4,  2 },
	{  2,  4,  4,  2 },
	{  1,  2,  2,  1 }
};

static  int     antialias_filter_5x5[5][5] =
{
	{  1,  2,  3,  2,  1 },
	{  2,  4,  6,  4,  2 },
	{  3,  6,  9,  6,  3 },
	{  2,  4,  6,  4,  2 },
	{  1,  2,  3,  2,  1 }
};

static  int     antialias_filter_6x6[6][6] =
{
	{  1,  2,  3,  3,  2,  1 },
	{  2,  4,  6,  6,  4,  2 },
	{  3,  6,  9,  9,  6,  3 },
	{  3,  6,  9,  9,  6,  3 },
	{  2,  4,  6,  6,  4,  2 },
	{  1,  2,  3,  3,  2,  1 }
};

static  int     antialias_filter_7x7[7][7] =
{
	{  1,  2,  3,  4,  3,  2,  1 },
	{  2,  4,  6,  8,  6,  4,  2 },
	{  3,  6,  9, 12,  9,  6,  3 },
	{  4,  8, 12, 16, 12,  8,  4 },
	{  3,  6,  9, 12,  9,  6,  3 },
	{  2,  4,  6,  8,  6,  4,  2 },
	{  1,  2,  3,  4,  3,  2,  1 }
};

static  int     antialias_filter_8x8[8][8] =
{
	{  1,  2,  3,  4,  4,  3,  2,  1 },
	{  2,  4,  6,  8,  8,  6,  4,  2 },
	{  3,  6,  9, 12, 12,  9,  7,  3 },
	{  4,  8, 12, 16, 16, 12,  8,  4 },
	{  4,  8, 12, 16, 16, 12,  8,  4 },
	{  3,  6,  9, 12, 12,  9,  7,  3 },
	{  2,  4,  6,  8,  8,  6,  4,  2 },
	{  1,  2,  3,  4,  4,  3,  2,  1 }
};

// ------------------------------------------------------------------------sk
// sk :Funktion, die bei angebener size die entsprechende Filtermatrix 
//     auswählt, um die Gewichtung an der angegeben Position zurückzugeben 
// ------------------------------------------------------------------------sk
float antialiasing_factor (int x, int y, int size)
{
	switch (size) {
	case 1: return 1.0;
	case 2: return antialias_filter_2x2[x][y]/4.0;
	case 3: return antialias_filter_3x3[x][y]/16.0;
	case 4: return antialias_filter_4x4[x][y]/36.0;
	case 5: return antialias_filter_5x5[x][y]/81.0;
	case 6: return antialias_filter_6x6[x][y]/144.0;
	case 7: return antialias_filter_7x7[x][y]/264.0;
	case 8: return antialias_filter_8x8[x][y]/408.0;

	default:
		// FIXME: this should never happen...
		return 0.0;
		break;
	}
}
