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

#include "WindowGeometry.h"

WindowGeometry::WindowGeometry (int w, int h)
{
	setSize(w,h);
}

void WindowGeometry::setSize(int w, int h)
{
	width = w;
	height = h;
	setFactor();
}

void WindowGeometry::setFactor ()
{
	if( width == height ) {
		width_factor  = 10.0;
		height_factor = 10.0;
	} else if( width < height ) {
		width_factor  = 10.0;
		height_factor = 10.0*height/width;
	} else {   
		width_factor  = 10.0*width/height;
		height_factor = 10.0;
	}
	
	toUserXFactor = 2*width_factor/(double) (width-1);
	toUserYFactor = -2*height_factor/(double)(height-1);
}
