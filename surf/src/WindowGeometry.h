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



#ifndef WINDOWGEOMETRY_H
#define WINDOWGEOMETRY_H

#include "def.h"

// 
// --- WindowGeometry is used for transformation of window (pixel) coordinates into
// --- user coordinates
//

class WindowGeometry
{
public:
	//
	// --- Note: this time default copy and assignment operators are okay (no pointers)
	//

	WindowGeometry(int w, int h);
	WindowGeometry() {width=height=0;}
	void setSize(int w, int h);
	
	double pixelToUserX (double x) const
		{
			return toUserXFactor*x-width_factor;
			// return 2*x*width_factor/(double)(width-1)-width_factor;
		}

	double pixelToUserY (double y) const
		{
			return toUserYFactor*y+height_factor;
			// return -2*y*height_factor / (double)(height-1) + height_factor;
		}
	
	double pixelToUser (int dir, double pix) const
		{
			if (dir==0)
				return pixelToUserX(pix);
			else
				return pixelToUserY(pix);
		}
	
	double userToPixelX (double ux) const
		{
			return (ux+width_factor)/(2*width_factor)*(width-1);
		}
	
	double userToPixelY (double uy) const 
		{
			return (-uy+height_factor)/(2*height_factor)*(height-1);
		}

	
	double userToPixel (int dir, double uv) const 
		{
			if (dir==0)
				return userToPixelX(uv);
			else
				return userToPixelY(uv);
		}

	double getDelta (int dir) const
		{
			return pixelToUser (dir,0)-pixelToUser(dir,1);
		}

	int getWidth() const
		{ return width;};
	int getHeight() const
		{ return height;};
	
	double WinSizeFactor() const 
		{
			return min(width,height)/20.0;
		}

private:
	
	void setFactor();

private:
	int width;
	int height;
	
	double width_factor;
	double height_factor;

	double toUserXFactor;
	double toUserYFactor;
};
#endif
