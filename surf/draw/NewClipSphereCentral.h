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



#ifndef CLIPSPHERECENTRAL_H
#define CLIPSPHERECENTRAL_H

#include <NewSimpleClip.h>

class NewClipSphereCentral : public NewSimpleClip
{
public:
	void init();
	bool clip_user_y(double uy);
	bool clip_user_xy(double ux, double uy, double& zmin, double& zmax);
	bool clip_user_xyz(double uz);

protected:
	// initialized by init
	double DISCR;
	double A0,A1,A2;
	double DISCR1;
	double B0,B1;
	
	double a0,a1,a2,a3,a4,a5,a6;
	double b0,b1,b2,b3,b4,b5;

	// initialized by clip_user_y
	double b6,b7,b8;
	double discr1,discr2;
	double a7,a8,a9;
};

#endif
