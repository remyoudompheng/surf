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



#include <assert.h>
#include <iostream.h>
#include <math.h>

#include "degree.h"
#include "RootFinder3d.h"
#include "RootFinder2d.h"
#include "resultant.h"


RootFinder3d::RootFinder3d()
{
	zDistance = 0.001;
}

RootFinder3d::~RootFinder3d()
{
}

void RootFinder3d::findRoots (int x, int y, int width, int height, double zmin, double zmax, RootFound3dAction *act)
{
	assert(act);
	action = act;

	res = resultant(p1,p2);
	RootFinder2d rf;
	rf.setGeometry (wingeo);
	rf.setPoly (res);
	rf.findRoots (x,y,width,height, this);
}

void RootFinder3d::setPolys (const Polyxyz &_p1, const Polyxyz &_p2)
{
	p1 = _p1;
	p2 = _p2;
	h1 = p1;
	h2 = p2;
}

void RootFinder3d::foundRoot (double x, double y)
{
	h1.SetRow (y);
	h1.SetVar (x);

	double Roots[MAX_DEGREE+1];
	int nr = h1.Zero(10.0,-10.0, Roots,0,0);
	if (nr > 0) {
		double Roots2[MAX_DEGREE+1];
		h2.SetRow(y);
		h2.SetVar(x);
		int nr2 = h2.Zero(10.0,-10.0, Roots2,0,0);
		if (nr2 > 0) {
			for (int i1=0;i1<nr;i1++) {
				for (int i2=0; i2<nr2; i2++) {
					double dist = Roots[i1]-Roots2[i2];
					if (fabs(dist)<zDistance) {
						if (!action->foundRoot(x,y,Roots[i1]))
							return;
					}
				}
			}
		}
	}
}
