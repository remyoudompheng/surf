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


#ifndef HORNERGROUPXY_H
#define HORNERGROUPXY_H


#include "polyarith.h"
#include "Horner.h"

class Position;
class HornergroupXYZ;
class RationalHornerXY;

class HornergroupXY
{
private:
	HornergroupXY (const HornergroupXY &);
	void operator=(const HornergroupXY &);
public:
	HornergroupXY( HornergroupXYZ*, RationalHornerXY* ); 
	HornergroupXY( polyxyz*, Position* );
	HornergroupXY (Polyxy Funct, Position *pos=0);

	~HornergroupXY();
	int getState() {return state;}
	void SetVar( int, double );
	int Zero( int, double, double, double*, double*, int );
	double EstimateDelta( int, double );

private:
	Hornerxy Orig [2];
	Hornerxy Derivate [2];

	int state;
};

#endif
