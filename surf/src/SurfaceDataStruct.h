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

#ifndef SURFACEDATASTRUCT_H
#define SURFACEDATASTRUCT_H

#include "CanvasDataStruct.h"
#include "def.h"

// ----------------------------------------------------------------------------
// sk :Neuer struct indem die Eigenschaften aller Flächen, ...
//     die Formeln aller Flächen und Curve-Eigenschaften festhält
// ----------------------------------------------------------------------------


typedef struct					/* sk neuer surface_propery-Struct */
{
	double red;				/* surface red,green,blue */ 
	double green;
	double blue;
	double ins_red; 			/* inside red,green,blue */
	double ins_green;
	double ins_blue;

	double transmit;			/* transmitted  */
	double diffuse; 			/* diffuse      */
	double reflect; 			/* reflected    */
	double transp;				/* transparence */
	int    smooth;  			/* smoothness   */
	
	int    root_n;				/* Anzahl der NUS */
    
} surf_property_struct;	

class SurfaceDataStruct
{
private:
	SurfaceDataStruct (const SurfaceDataStruct &);
	void operator= (const SurfaceDataStruct &);
	
public:
	SurfaceDataStruct();
	~SurfaceDataStruct();
	int init ();
	void initCurveProperty();

	CanvasDataStruct *getFormula (int nr) 
		{
			return &formulas[new_index[nr-1]];
		}

public:
	CanvasDataStruct     formulas[MAIN_SURFACE_AMOUNT_NUM];	
	surf_property_struct surfaces[MAIN_SURFACE_AMOUNT_NUM]; 
	surf_property_struct curve;
	
	/* sk beinhaltet den Index der Flächen nach der Initialisierung   */
	int    new_index[MAIN_SURFACE_AMOUNT_NUM];	
	
	double prev_root;   /* sk letzte gezeichneteoder bearbeitete NUS  */       	
	int    surf_n;      /* sk Anzahl der zu berechnenden Flächen      */
	int    root_n_all;  /* sk Anzahl aller vorhandenen NUS aller Flächen*/        
	bool   init_all;    /* sk wurde überhaupt eine Fläche initialisiert*/


};

#endif
