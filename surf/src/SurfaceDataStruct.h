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


#ifndef SURFACEDATASTRUCT_H
#define SURFACEDATASTRUCT_H

#include <CanvasDataStruct.h>
#include <def.h>

struct surf_property_struct
{
	double red;
	double green;
	double blue;
	double ins_red;
	double ins_green;
	double ins_blue;

	double transmit;
	double diffuse;
	double reflect;
	double transp;
	int smooth;
	
	int root_n;
    
};

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

	CanvasDataStruct *getFormula (int nr) {
		return &formulas[new_index[nr-1]];
	}

public:
	CanvasDataStruct     formulas[MAIN_SURFACE_AMOUNT_NUM];	
	surf_property_struct surfaces[MAIN_SURFACE_AMOUNT_NUM]; 
	surf_property_struct curve;
	
	/* sk beinhaltet den Index der Fl�chen nach der Initialisierung   */
	int    new_index[MAIN_SURFACE_AMOUNT_NUM];	
	
	double prev_root;   /* sk letzte gezeichneteoder bearbeitete NUS  */       	
	int    surf_n;      /* sk Anzahl der zu berechnenden Fl�chen      */
	int    root_n_all;  /* sk Anzahl aller vorhandenen NUS aller Fl�chen*/        
	bool   init_all;    /* sk wurde �berhaupt eine Fl�che initialisiert*/
};

#endif //!SURFACE_DATA_STRUCT_H
