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

#ifndef ROOTFINDER2D_H
#define ROOTFINDER2D_H

#include "HornergroupXY.h"
#include "MultiVariatePolynom.h"
#include "WindowGeometry.h"
#include "TreePolynom.h"
#include "Monom.h"


// RootFoundAction is used as a kind of callback class by Rootfinder2d
class RootFoundAction
{
public:
	virtual ~RootFoundAction(){};
	virtual void foundRoot (double x, double y)=0;
};


// RootFinder2d finds roots of a polynomial in 2 variables
class RootFinder2d
{
private:
	RootFinder2d (const RootFinder2d &);
	void operator=(const RootFinder2d&);

public:
	RootFinder2d();
	~RootFinder2d();

	// set the geometry
	void setGeometry (const WindowGeometry &wm)	{wingeo = wm;};

	// set the polynomial
	void setPoly (const Polyxy &p);

	// find roots in the specified area and call action->foundRoot with roots found
	void findRoots (int x, int y, int width, int height, RootFoundAction *action);	
	
	
protected:
	HornergroupXY *poly;
	WindowGeometry wingeo;

	double Delta [2];

private:
	double toUser (int dir, double pix)
		{ return wingeo.pixelToUser(dir,pix);};
	
	double toPixel (int dir, double u)
		{return wingeo.userToPixel(dir,u);};

	double getDelta (int dir)
		{return wingeo.getDelta(dir);};

	void findRootsInDirection (int direction, int min, int max, double minimal, double maximal, RootFoundAction *action);
};

#endif
