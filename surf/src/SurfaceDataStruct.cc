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



#include <string.h> 

#include <SurfaceDataStruct.h>
#include <ScriptVar.h>
#include <cube.h>

SurfaceDataStruct::SurfaceDataStruct()
{
	memset (surfaces,0, sizeof(surfaces));
	memset (&curve, 0, sizeof(curve));
}

SurfaceDataStruct::~SurfaceDataStruct()
{
}


int SurfaceDataStruct::init ()
{
	int i,k;
	for (k=i=0; i< MAIN_SURFACE_AMOUNT_NUM; i++) {
		if (formulas[k].initWithSurface(i)) {

			surfaces[k].red  = ScriptVar::color_slider[i].red / 255.0;
			surfaces[k].green=  ScriptVar::color_slider[i].green/255.0; 
			surfaces[k].blue =  ScriptVar::color_slider[i].blue/255.0;

			surfaces[k].ins_red  = ScriptVar::color_slider[i].inside_red  /255.0;
			surfaces[k].ins_green= ScriptVar::color_slider[i].inside_green/255.0;
			surfaces[k].ins_blue = ScriptVar::color_slider[i].inside_blue /255.0;
			
			surfaces[k].diffuse= ScriptVar::light_settings[i].diffuse/100.0;
			surfaces[k].reflect= ScriptVar::light_settings[i].reflected/100.0; 
			surfaces[k].transmit=ScriptVar::light_settings[i].transmitted/100.0; 
			
			surfaces[k].smooth = ScriptVar::light_settings[i].smoothness;
			surfaces[k].transp = ScriptVar::light_settings[i].transparence/100.0;

			/*sk :merke Index der Fläche, mit dem sie im Skript definiert wurde */ 
			new_index[i]=k;	     

			k++;
		}
	}

	// --------------------
	//  init clipping_cube
	// --------------------
	
	init_cube( &clip_cube );

	surf_n=k;
	init_all = k > 0;
	return k;
}

void SurfaceDataStruct::initCurveProperty()
{
	/* sk:curve NUS-Anzahl auf 1 setzen, damit sie        */
	/*    in illumintion_of_surface() berücksichtigt wird */

	curve.root_n = 1;

	// sk :Setze die Kurvenfarbe in den neuen Struct    	
	curve.red  =ScriptVar::curve_color_slider_data[RED]  /255.0;
	curve.green=ScriptVar::curve_color_slider_data[GREEN]/255.0;
	curve.blue =ScriptVar::curve_color_slider_data[BLUE] /255.0;

	curve.transp =0/100.0;

	// möglichkeit offen gelassen für weitere curve Properties
	// f_d->curve.ambient=100/100.0;
	// f_d->curve.diffuse=100/100.0;
	// f_d->curve.reflect=100/100.0;
	// f_d->curve.smooth=24;


}
