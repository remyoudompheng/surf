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



#include <math.h>
#include <stdio.h>

#include <NewClipAlgebraic.h>
#include <NewClipAlgebraicDode.h>
#include <ScriptVar.h>


void NewClipAlgebraicDode::init( void )
{
    // ------------------------------------------------------------------------
    //  six polynomials:
    //
    //  p[0] = (2*r+0*(x-cx)+a*(y-cy)+b*(z-cz))*(r+0*(x-cx)-a*(y-cy)-b*(z-cz))
    //  p[1] = (2*r+0*(x-cx)+a*(y-cy)-b*(z-cz))*(r+0*(x-cx)-a*(y-cy)+b*(z-cz))
    //
    //  p[2] = (2*r+b*(x-cx)+0*(y-cy)+a*(z-cz))*(r-b*(x-cx)-0*(y-cy)+a*(z-cz))
    //  p[3] = (2*r+b*(x-cx)+0*(y-cy)-a*(z-cz))*(r+b*(x-cx)-0*(y-cy)+a*(z-cz))
    //
    //  p[4] = (2*r-a*(x-cx)+b*(y-cy)+0*(z-cz))*(r+a*(x-cx)+b*(y-cy)-0*(z-cz))
    //  p[5] = (2*r+a*(x-cx)-b*(y-cy)+0*(z-cz))*(r-a*(x-cx)+b*(y-cy)+0*(z-cz))
    //
    //  r  = clip_numeric.radius
    //  cx = clip_numeric.center_x
    //  cy = clip_numeric.center_y
    //  cz = clip_numeric.center_z
    //
    //  a  = sqrt(3)*(-3+sqrt(5))
    //  b  = sqrt(3)*(-1+sqrt(5))
    // ------------------------------------------------------------------------

    double  a = sqrt(3.0)*(-3.0+sqrt(5.0));
    double  b = sqrt(3.0)*(-1.0+sqrt(5.0));

    polyxyz h[12];

    int i;

    for( i=0; i<12; i++ )
    {
        new_coeff_polyxyz( &h[i],4 );
        h[i].m[1].kx = h[i].m[2].ky = h[i].m[3].kz = 1;
    }

    h[0].m[0].a = 2*ScriptVar::clip_numeric.radius + 0*ScriptVar::clip_numeric.center_x - a*ScriptVar::clip_numeric.center_y - b*ScriptVar::clip_numeric.center_z;
    h[1].m[0].a = 2*ScriptVar::clip_numeric.radius + 0*ScriptVar::clip_numeric.center_x - a*ScriptVar::clip_numeric.center_y + b*ScriptVar::clip_numeric.center_z;
    h[6].m[0].a = 2*ScriptVar::clip_numeric.radius + 0*ScriptVar::clip_numeric.center_x + a*ScriptVar::clip_numeric.center_y + b*ScriptVar::clip_numeric.center_z;
    h[7].m[0].a = 2*ScriptVar::clip_numeric.radius + 0*ScriptVar::clip_numeric.center_x + a*ScriptVar::clip_numeric.center_y - b*ScriptVar::clip_numeric.center_z;

    h[0].m[1].a =  0; h[0].m[2].a =  a; h[0].m[3].a =  b;
    h[1].m[1].a =  0; h[1].m[2].a =  a; h[1].m[3].a = -b;
    h[6].m[1].a =  0; h[6].m[2].a = -a; h[6].m[3].a = -b;
    h[7].m[1].a =  0; h[7].m[2].a = -a; h[7].m[3].a =  b;


    h[2].m[0].a = 2*ScriptVar::clip_numeric.radius - b*ScriptVar::clip_numeric.center_x + 0*ScriptVar::clip_numeric.center_y - a*ScriptVar::clip_numeric.center_z;
    h[3].m[0].a = 2*ScriptVar::clip_numeric.radius + b*ScriptVar::clip_numeric.center_x + 0*ScriptVar::clip_numeric.center_y - a*ScriptVar::clip_numeric.center_z;
    h[8].m[0].a = 2*ScriptVar::clip_numeric.radius + b*ScriptVar::clip_numeric.center_x + 0*ScriptVar::clip_numeric.center_y + a*ScriptVar::clip_numeric.center_z;
    h[9].m[0].a = 2*ScriptVar::clip_numeric.radius - b*ScriptVar::clip_numeric.center_x + 0*ScriptVar::clip_numeric.center_y + a*ScriptVar::clip_numeric.center_z;

    h[2].m[1].a =  b; h[2].m[2].a =  0; h[2].m[3].a =  a;
    h[3].m[1].a = -b; h[3].m[2].a =  0; h[3].m[3].a =  a;
    h[8].m[1].a = -b; h[8].m[2].a =  0; h[8].m[3].a = -a;
    h[9].m[1].a =  b; h[9].m[2].a =  0; h[9].m[3].a = -a;


    h[4].m[0].a  = 2*ScriptVar::clip_numeric.radius - a*ScriptVar::clip_numeric.center_x - b*ScriptVar::clip_numeric.center_y + 0*ScriptVar::clip_numeric.center_z;
    h[5].m[0].a  = 2*ScriptVar::clip_numeric.radius - a*ScriptVar::clip_numeric.center_x + b*ScriptVar::clip_numeric.center_y + 0*ScriptVar::clip_numeric.center_z;
    h[10].m[0].a = 2*ScriptVar::clip_numeric.radius + a*ScriptVar::clip_numeric.center_x + b*ScriptVar::clip_numeric.center_y + 0*ScriptVar::clip_numeric.center_z;
    h[11].m[0].a = 2*ScriptVar::clip_numeric.radius + a*ScriptVar::clip_numeric.center_x - b*ScriptVar::clip_numeric.center_y + 0*ScriptVar::clip_numeric.center_z;

    h[4].m[1].a  =  a; h[4].m[2].a  =  b; h[4].m[3].a  =  0;
    h[5].m[1].a  =  a; h[5].m[2].a  = -b; h[5].m[3].a  =  0;
    h[10].m[1].a = -a; h[10].m[2].a = -b; h[10].m[3].a =  0;
    h[11].m[1].a = -a; h[11].m[2].a =  b; h[11].m[3].a =  0;

    for( i=0; i<6; i++ )
    {
        p[i] = polyxyz_mult( &h[i],&h[i+6] );
        polyxyz_collect( &p[i] );
        polyxyz_set_degree( &p[i] );
        polyxyz_adjust( &p[i] );
	hp[i]  = new hornerpolyxyz ( p[i] );
    }

    for( i=6; i<MAIN_CLIP_AMOUNT_NUM; i++ )
    {
        p[i].n = 0;
    }

    for( i=0; i<12; i++ )
    {
        delete_coeff_polyxyz( &h[i] );
    }
}

