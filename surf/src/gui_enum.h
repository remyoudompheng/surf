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




#ifndef GUI_ENUM_H
#define GUI_ENUM_H



// sk :neuer enum für RED,GREEN,BLUE
typedef enum
{
	RED,
	GREEN,
	BLUE
    
} color_components_enum;

typedef enum
{
	SURFS,
	INSIDE,
	BALLS
} surfball_enum;


typedef enum 
{ 
	LIGHT_SMOOTHNESS, 
	LIGHT_TRANSPARENCE, 
	LIGHT_THICKNESS 		/* sk */

} light_settings_second_enum; 

typedef enum 
{ 
	LIGHT_POSITION_X, 
	LIGHT_POSITION_Y, 
	LIGHT_POSITION_Z 
 
} light_position_enum; 

typedef enum
{
	NUMERIC_ROOT_D_BISECTION,
	NUMERIC_ROOT_D_REGULA,
	NUMERIC_ROOT_D_PEGASUS,
	NUMERIC_ROOT_D_ANDERSON,
	NUMERIC_ROOT_D_NEWTON,
	NUMERIC_ROOT_BEZIER
} NUMERIC_ROOT_ENUM;    

#endif
