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



#include "RGBToStandard.h"

unsigned long RGBToStandard (float r, float g, float b, XStandardColormap *m)
{
	if( r>1.0 ) 
		r=1.0;
	if( g>1.0 ) 
		g=1.0;
	if( b>1.0 ) 
		b=1.0;
	
	return m->base_pixel
		+ ((unsigned long)(0.5+r*m->red_max  ))*m->red_mult
		+ ((unsigned long)(0.5+g*m->green_max))*m->green_mult
		+ ((unsigned long)(0.5+b*m->blue_max ))*m->blue_mult;
}
