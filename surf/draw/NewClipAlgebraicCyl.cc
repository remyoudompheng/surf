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



#include <stdio.h>

#include <NewClipAlgebraic.h>
#include <NewClipAlgebraicCyl.h>
#include <ScriptVar.h>


void NewClipAlgebraicCyl::init( void )
{
    // ----------------------------
    //  two polynomials:
    //  p[0] = r^2-(x-cx)^2-(z-cz)^2
    //  p[1] = r^2-(y-cy)^2
    //
    //  r  = clip_numeric.radius
    //  cx = clip_numeric.center_x
    //  cy = clip_numeric.center_y
    //  cz = clip_numeric.center_z
    // ----------------------------

	int i;

    new_coeff_polyxyz( &p[0],7 );
    new_coeff_polyxyz( &p[1],4 );

    p[0].m[0].a = p[1].m[0].a = ScriptVar::clip_numeric.radius*ScriptVar::clip_numeric.radius;
    p[0].m[1].a = p[0].m[4].a = p[1].m[1].a = -1.0;

    p[0].m[1].kx = p[0].m[4].kz = p[1].m[1].ky = 2;
    p[0].m[2].kx = p[0].m[5].kz = p[1].m[2].ky = 1;

    p[0].m[2].a = 2.0*ScriptVar::clip_numeric.center_x;
    p[0].m[5].a = 2.0*ScriptVar::clip_numeric.center_z;
    p[1].m[2].a = 2.0*ScriptVar::clip_numeric.center_y;

    p[0].m[3].a = -ScriptVar::clip_numeric.center_x*ScriptVar::clip_numeric.center_x;
    p[0].m[6].a = -ScriptVar::clip_numeric.center_z*ScriptVar::clip_numeric.center_z;
    p[1].m[3].a = -ScriptVar::clip_numeric.center_y*ScriptVar::clip_numeric.center_y;

    for( i=0; i<2; i++ )
    {
        polyxyz_collect( &p[i] );
        polyxyz_set_degree( &p[i] );
        polyxyz_adjust( &p[i] );
	hp[i]  = new hornerpolyxyz ( p[i] );
    }

    for( i=2; i<MAIN_CLIP_AMOUNT_NUM; i++ )
    {
        p[i].n = 0;
    }
}

