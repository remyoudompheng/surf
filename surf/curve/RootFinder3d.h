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



#ifndef ROOTFINDER3D_H
#define ROOTFINDER3D_H

#include <assert.h>

#include "RootFinder2d.h"
#include "WindowGeometry.h"

// RootFound3dAction is used as a kind of callback class by Rootfinder3d
class RootFound3dAction
{
public:
	virtual ~RootFound3dAction() {};
	virtual bool foundRoot (double x, double y, double z)=0;
	// return false, if only biggest root is needed for a pair of values for x and y
	// otherwise return true, and you'll get all roots
};


// RootFinder3d finds common roots of 2 polynomials in 3 variables
class RootFinder3d : private RootFoundAction
{
private:
	RootFinder3d (const RootFinder3d &);
	void operator=(const RootFinder3d &);

public:
	RootFinder3d();
	~RootFinder3d();
	
	// find the common roots in the given area and call act->foundRoot for those found
	void findRoots (int x, int y, int width, int height, double zmin, double zmax, RootFound3dAction *act);

	// set the geometry
	void setGeometry (const WindowGeometry &wm)	{wingeo = wm;};

	// set the two polys
	void setPolys (const Polyxyz &_p1, const Polyxyz &_p2);
	
	// set the z distance...if the z values differ by less than zDistance 
	// for two roots of p1 and p2 (with same x and y value) then we consider
	// these to be common roots
	void setZDistance (double val)
		{
			assert(val > 0);
			zDistance = val;
		}
private:
	WindowGeometry wingeo;
	Polyxyz p1,p2;
	Polyxy res;
	Hornerxyz h1,h2;
	
	RootFound3dAction *action;

	double zDistance;

private:
	// this method gets called by RootFinder2d
	virtual void foundRoot (double x, double y);
};
#endif
