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



#include "RGBToNetscape.h"
#include "compfn.h"

int RGBToNetscape (float r, float g, float b, int x, int y)
{
        if( r>=1.0 ) 
		r=0.9999;
        if( g>=1.0 ) 
		g=0.9999;
        if( b>=1.0 ) 
		b=0.9999;

        r *= 5;
        g *= 5;
        b *= 5;
	
        float  c = compare_function[1][2]( x,y );

        if( r-(int)r > c ) 
		r+=1.0;
        if( g-(int)g > c ) 
		g+=1.0;
        if( b-(int)b > c ) 
		b+=1.0;

        return ((int)(r))*36 + ((int)(g))*6 + ((int)(b));
}
