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



// ===========================================================================
// File      roots.cc
// Author:   Stephan Endrass
// Address:  endrass@mi.uni-erlangen.de
// Date:     3.2.95
// ===========================================================================

#include <stdio.h>
#include <math.h>
#include <memory.h>

#include "degree.h"
#include "def.h"
#include "monomarith.h"
#include "polyarith.h"
#include "polyroot.h"
#include "bezierarith.h"
#include "hornerarith.h"
#include "polylexyacc.h"
#include "gui_config.h"
#include "NewClip.h"

//  using namespace globals;
int numeric_multi_root = 0;

// ----------------------------------------------------------------------------
//  compute roots of f in [zmin,zmax] where g is positive
//      f:           polynomial
//      [zmin,zmax]: interval
//      *root:       roots of f
//      *root_old:   values near the roots
//      old_n:       # of values in *root_old
//  depends on:
//      numeric_multi_root: TRUE  => compute all roots
//                          FALSE => compute biggest root
//      numeric_root_finder_data: chooses method
// ----------------------------------------------------------------------------

int     zero_of_polyx( polyx *f,
                double zmax,double zmin,
                double *root,
                double *root_old,int old_n,
                NewClip *clipper )
{


	// ------------
	//  get degree
	// ------------
    
	int n = f->n;
        int num = 0;
 
	while( n >= 0 && f->a[n] == 0.0 ) {
		n--;
	}

	switch( n ) {
		// ---------------------------
		//  zero polynomial: one root
		// ---------------------------
        case -1 :
		root[0] = zmax;
		return  TRUE;
 
		// ------------------------------
		//  constant polynomial: no root
		// ------------------------------
        case  0 :
		return  0;
 
		// -------------------------------------
		//  linear polynomial: at most one root
		// ------------------------------------
        case  1 :
		root[0] = -f->a[0]/f->a[1];
		return  ( zmin < root[0] && root[0] < zmax && 
                          clipper->clip_user_xyz( root[0] ) ? 1 : 0 );
 
        // -------------------------------------------------------
        //  polynomial of degree two or higher:  several methods
        // -------------------------------------------------------
        default :

		if( numeric_root_finder_data == NUMERIC_ROOT_BEZIER ) {
			// -------------------------------
			//  Rockwoods all roots algorithm
			// -------------------------------
                        if( numeric_multi_root ) {
				if( old_n == 0 ) {
					num = zero_of_bezier_multi_root_no_help(
						f,zmin,zmax,
						root );
				} else {
					num = zero_of_bezier_multi_root_no_help(
						f,zmin,zmax,
						root );
				}
			} else {   
				if( old_n == 0 ) {
				        if( zero_of_bezier_single_root_no_help( f,zmin,zmax,root ) > 0 )
					{
                                            while( !clipper->clip_user_xyz( root[0] ) && zmin<root[0] )
					    {
                                                if( zero_of_bezier_single_root_no_help_open( f,zmin,root[0],root ) == 0 )
						{
                                                    return  0;
						}
					    }
                                            return  1;
					}
				} else {   
				        if( zero_of_bezier_single_root_help( f,zmin,zmax,root,root_old[0] ) > 0 )
					{
                                            while( !clipper->clip_user_xyz( root[0] ) && root[0]>zmin )
					    {
                                                if( zero_of_bezier_single_root_help_open( f,zmin,root[0],root,root_old[0] )==0 )
						{
                                                    return  0;
						}
					    }
                                            return  1;
					}
				}
			}    
		} else {

			// -------------------
			//  D-chain algorithm
			// -------------------

			int     mults[MAX_DEGREE];
			num = polyx_all_roots( f,zmin,zmax,root,mults );

                        if( !numeric_multi_root )
			{
                            for( int j=0; j<num; j++ )
		            {
                                if( clipper->clip_user_xyz( root[j] ) )
			        {
                                    root[0] = root[j];
                                    return  1;
			        }
	                    }
                            return  0;
			}
		}
	}
        int num_new = num;

        for( int j=0, k=0; j<num; j++ )
	{
                if( clipper->clip_user_xyz( root[j] ) )
		{
                        if( k<j ) root[k] = root[j];
                        k++;
	   	}
                else
		{
                        num_new--;
       		}
        }
        return  num_new;
}

// ============================================================================
// End of roots.cc
// ============================================================================
