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

#ifndef SURFACECALC_H
#define SURFACECALC_H

#include "def.h"
#include "Vector.h" 
#include "color.h"
#include "NewClip.h"
#include "degree.h"
#include "TSDrawingArea.h"
#include "SurfaceDataStruct.h"
#include "WindowGeometry.h"
#include "Preview.h"

class SurfaceCalc
{
private:
	SurfaceCalc (const SurfaceCalc &);
	void operator=(const SurfaceCalc &);

public:
	SurfaceCalc();
	~SurfaceCalc();


	void setDisplay (TSDrawingArea *display)	{vdisplay=display;};
	void setPreview (const Preview &p)
		{ main_mosaic_choice_data = p;};

	void surface_calculate (int xmin,int ymin,int xmax,int ymax,
				class RgbBuffer &intensity);

	void CalculateCurveOnSurface( int xmin, int ymin, int xmax, int ymax,
				      RgbBuffer &intensity,
				      class float_buffer &z_buffer);


protected:
	void color_of_single_point (Vector &P, Vector &N, colorrgb &I, int obj_color);
	void color_of_single_point_old (Vector &P, Vector &N, colorrgb &I, int obj_color);

	void surface_refine_pixel( int px_refine,int py_refine,
				   colorrgb& pixel_color_refine );    	

	
	void illumination_of_surface( double wx,double wy,           
				      double (*root)[MAX_DEGREE],
				      double (*root_old)[MAX_DEGREE],    
				      double stepwidth,
				      colorrgb& color_intens );

	void initVars();


public:
	SurfaceDataStruct sf_ds;

protected:
 	NewClip *clipper;
	TSDrawingArea *vdisplay;

	void countLights();
	void setMainFactor();


	inline int needsRefining (const colorrgb &i1, const colorrgb &i2, const colorrgb &i3,
				  const colorrgb &i4, const colorrgb &i5, const colorrgb &i6,
				  const colorrgb &i7, const colorrgb &i8, const colorrgb &i9);

	void surface_set_pixel (int x, int y, const colorrgb &color)
		{
			if (vdisplay)
				vdisplay->drawSquare (x-(jump-1)/2, y-(jump-1)/2, jump, color.red, color.green, color.blue);
		};
	void surface_set_r_pixel (int x, int y, const colorrgb &color)
		{
			if (vdisplay)
				vdisplay->drawRSquare (x-(jump-1)/2, y-(jump-1)/2, jump, color.red, color.green, color.blue);
		};
	void surface_set_gb_pixel (int x, int y, const colorrgb &color)
		{
			if (vdisplay)
				vdisplay->drawGBSquare (x-(jump-1)/2, y-(jump-1)/2, jump, color.red, color.green, color.blue);
		};


	double pixel_to_user_x (double x) {return wingeom.pixelToUserX(x);};
	double pixel_to_user_y (double y) {return wingeom.pixelToUserY(y);};
	void packLights();
	
  	void main_factor_set() {wingeom.setSize(main_width_data, main_height_data);};



protected:
	WindowGeometry wingeom;

	float cb,cf;

        double   pixel_max_i; // = 0.0;
	int      jump_memory;  			// sk :variables for ...
	int      halfjump;			//     preview calculation run

	Vector   POS;                           // Spectator position

	Vector   LPOS[LIGHT_SOURCE_MAX_VALUE];  // Light position
	colorrgb licht[LIGHT_SOURCE_MAX_VALUE]; // sk :Light color	
	int numLights;
	colorrgb backgr;			// skbackgr

	int      flag_ambient;
	int      flag_diffuse;
	int      flag_reflected;
	int      flag_transmitted;

	float    ambient[3][3];   	// sk :1'ten 3 definiert RGB, ...
	float    diffuse[3][3];   	//     2'ten 3 surfs,inside,balls
	float    tocolor[3];            // sk :Farbe des Farbverlaufs

	double   reflected;       			
	double   transmitted;
	int      thickness;
	double   transmission;

	float    lay2; 				// sk :for mixing curve with surface color
	int      smoothness;
	double   transparence;
   	float    part_A;
        float    part_D;
        colorrgb tmpcolor;               	// sk :Ausweichvariable for color

	int jump;
	Preview main_mosaic_choice_data;
};
#endif
