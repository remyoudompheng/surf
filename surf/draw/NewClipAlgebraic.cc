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



#include "stdio.h"
#include "NewClipAlgebraic.h"

#include "gui_config.h"

int NewClipAlgebraic::clip_user_y( double uy )
{
    for( int i=0; i<MAIN_CLIP_AMOUNT_NUM; i++ )
    {
        if( p[i].n > 0 )
	{
	    hp[i]->setRow (uy);
	}
    }
    return  TRUE;
}

int NewClipAlgebraic::clip_user_xy( double ux,double uy,double &zmin,double &zmax )
{
    zmin = ScriptVar::clip_numeric.clip_back;
    zmax = ScriptVar::clip_numeric.clip_front;

    for( int i=0; i<MAIN_CLIP_AMOUNT_NUM; i++ )
    {
        if( p[i].n > 0 )
	{
	    hp[i]->setColumn( ux );
	}
    }
    return  TRUE;
}

int NewClipAlgebraic::clip_user_xyz( double uz )
{
    for( int i=0; i<MAIN_CLIP_AMOUNT_NUM; i++ )
    {
        if( p[i].n > 0 )
	{
            if( hp[i]->pZ.horner( uz ) < 0.0 )
	    {
                return  FALSE;
	    }
	}
    }
    return  TRUE;
}

extern double Y_AXIS_LR_ROTATE;

void    NewClipAlgebraic::polyxyz_adjust( polyxyz *q )
{
    for( int ind = 0; ind < POSITION_SEQUENCE_NUM; ind++ )
    {
        switch( ScriptVar::position_sequence_data[ind] ) {
	    case 0:
	        polyxyz_shift_self ( q,	
		        ScriptVar::position_numeric.orig_x,
		        ScriptVar::position_numeric.orig_y,
		        ScriptVar::position_numeric.orig_z);
		break;
			
	    case 1:
		polyxyz_rotate_self ( q,	   		
			ScriptVar::position_numeric.rot_x,
			ScriptVar::position_numeric.rot_y,
			ScriptVar::position_numeric.rot_z);
		break;
			
	    case 2:
		polyxyz_scale_self ( q,
			ScriptVar::position_numeric.scale_x,
		        ScriptVar::position_numeric.scale_y,
			ScriptVar::position_numeric.scale_z);
		break;
			
	    default:
		break;
	    }
	}

  	// --------------------------------------- 
	//  rotate/shift surface for 3D - picture
	// --------------------------------------- 
	
	if( ScriptVar::display_numeric.stereo_eye ) {
	    polyxyz_rotate_self( q,0.0, Y_AXIS_LR_ROTATE, 0.0 );
	}
	
  	// -------------------------------
	//  adjust surface to perspective
	// -------------------------------

	if( ScriptVar::position_perspective_data == ScriptVar::position_perspective_central_data ) {
	    polyxyz_perspective_self( q,ScriptVar::position_numeric.spectator_z);
	}
	
	polyxyz_norm_self( q );			
	polyxyz_sort     ( q );				
}
