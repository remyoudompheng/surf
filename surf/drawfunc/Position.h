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





#ifndef POSITION_H
#define POSITION_H

#include "Vector.h"
#include "MappingMatrix.h"
#include "gui_config.h"

class MappingMatrix;
class Polyxy;
class Polyxyz;

class Position
{
private:
	Position (const Position &);
	void operator=(const Position &);
	
public:

	Position (position_numeric_t Data,int*,int );
	~Position();

	int     Central() const { return Perspective; }
	Polyxy  Adjust( const Polyxy& );
	Polyxyz Adjust( const Polyxyz& );
	Vector  Adjust( const Vector &Vec );
	Vector  Rotate( const Vector &Vec );
	Vector  ReTransform( const Vector &Vec );
	Vector  Spectator() { Vector R(0.0,0.0,SpecZ); return R; }

private:
	Vector Turn;
	Vector Scale;
	Vector Shift;
	double SpecZ;
	int    *Sequence;
	int    Perspective;

	MappingMatrix PosXY;
	MappingMatrix PosXYZ;
	MappingMatrix Rotation;
};

#endif //POSITION_H
