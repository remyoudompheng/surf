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
#include <NewClipAlgebraicUser.h>
#include <ScriptVar.h>


void NewClipAlgebraicUser::init( void )
{
    for( int i=0; i<MAIN_CLIP_AMOUNT_NUM; i++ )
    {
        if( ScriptVar::main_clip_pxyz_data[i].n > 0 )
	{
	    p[i] = polyxyz_copy( &ScriptVar::main_clip_pxyz_data[i] );

            polyxyz_adjust( &p[i] );

            //fprintf( stderr,"%d\n",i );
            //polyxyz_print( &p[i] );

	    hp[i]  = new hornerpolyxyz ( p[i] );
	}
        else 
        {
	    p[i].n = 0;
	}
    }
}

