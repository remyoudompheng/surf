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




// ===========================================================================
// File      cube.h
// Author:   Stephan Endrass
// Address:  endrass@mi.uni-erlangen.de
// Date:     2 march 96
// ============================================================================

#ifndef CUBE_H
#define CUBE_H

// -----------------------------------------
//  define the struct for the clipping cube
// -----------------------------------------

typedef struct
{
	double    center_x;
	double    center_y;
	double    center_z;

	double    radius;

	double    equations[6][4];
} cube;

extern  cube    clip_cube;

void    init_cube( cube* );

#endif /* CUBE_H */

// ============================================================================
// End of cube.h
// ============================================================================
