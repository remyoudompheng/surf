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

#include<iostream>

#include <RootFinder2d.h>
#include <degree.h>

// #define DEBUG
#include <surfdebug.h>



RootFinder2d::RootFinder2d() : poly(0)
{}

RootFinder2d::~RootFinder2d()
{
	delete poly;
}

void RootFinder2d::setPoly (const Polyxy &p)
{
	delete poly;
	poly = new HornergroupXY (p, 0);
}

void RootFinder2d::findRootsInDirection (int direction, int min, int max, double minimal, double maximal, RootFoundAction *action)
{
	assert(direction==0 || direction == 1);

	double Root	[MAX_DEGREE+1];			// array to store roots
	double Estimate	[MAX_DEGREE+1];			// array to store estimates

	int    NumberOfEstimates = 0;     // counter
	double Coord[2];

	// go through all lines resp. columns
	for (int Pixel=min; Pixel<max; Pixel++) {

		// transform pixel y to y coordinate
		Coord[direction] = toUser (direction, Pixel);

		// set value in function
		poly->SetVar( direction, Coord[direction] );
	  
		// find all roots of function in min...max
		// with estimates at estimate(0)...estimate(NumberOfEstimates-1)

		int NumberOfRoots = poly->Zero (direction, maximal, minimal, Root, Estimate, NumberOfEstimates);
		
		if (NumberOfRoots) {
			// reset number of estimates
			NumberOfEstimates = 0;
			// go through all roots found
			for (int RootNumber = 0; RootNumber<NumberOfRoots; RootNumber++) {
				Coord[1-direction] = Root[RootNumber];
				action->foundRoot (Coord[0], Coord[1]);

				// set estimate at old root
				Estimate[NumberOfEstimates] = Root[RootNumber]
					+ Delta[direction] * poly->EstimateDelta( direction, Root[RootNumber] );
				
				// only use estimate if smaller than previous
				if( NumberOfEstimates == 0 || Estimate[NumberOfEstimates] < Estimate[NumberOfEstimates-1] )
					NumberOfEstimates++;
			}
		}
	}
}


void RootFinder2d::findRoots (int x, int y, int width, int height, RootFoundAction *action)
{
	Delta[0] = getDelta(0);
	Delta[1] = getDelta(1);

  	findRootsInDirection (0, x, x+width,  toUser(1, y+height-1), toUser(1, y),  action);
	findRootsInDirection (1, y, y+height, toUser(0, x), toUser(0, x+width-1),  action);
}
