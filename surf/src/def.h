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

#ifndef	DEF_H
#define	DEF_H

#include <mytypes.h>

// ----------------------------------------------------------------------------
//  sk : alle defines aus config.cc hierhergebracht
//  These guys cannot come as consts  
// ----------------------------------------------------------------------------


#define SURFS_COLOR_SLIDER_NUM          6
#define SU_IN_BA_COLOR_SLIDER_NUM       9   /* sk jeweils 3 Slider für die Farben */
                                	    /* SURFACE,INSIDE,BALLS    		  */
#define LIGHT_SLIDER_NUM                3   /* sk auf 3 reduziert */
#define COLOR_GRADIENT_END_NUM          3   /* sk drei Panels für GradientFarbe*/
#define COLOR_BACKGROUND_NUM            3   /* skbackgr  */

#define LIGHT_SETTINGS_FIRST_NUM        4
#define LIGHT_SETTINGS_SECOND_NUM       3   /* sk Erweitert auf 3*/
#define LIGHT_POSITION_NUM              3
#define LIGHT_SOURCE_MAX_VALUE          9


#define INTERACT_STEP_NUM               3
#define POSITION_NUMERIC_NUM           10    
#define POSITION_SEQUENCE_NUM           3
#define DISPLAY_NUMERIC_NUM             5   /* sk Für Panels im Display-Menü */
#define MAIN_MOSAIC_CHOICE_NUM          5   /* eigentlich 4 , 5-ter Wert für Speicherung des vohergehenden Wertes*/
#define MAIN_MEM_LAST_MOSAIC_INDEX      MAIN_MOSAIC_CHOICE_NUM-1  /* sk */
#define MAIN_SURFACE_AMOUNT_NUM         9
#define MAIN_CLIP_AMOUNT_NUM           12

#define CUT_NUMERIC_NUM                14

#define CLIP_NUM_CHOICE                 6
#define CLIP_NUMERIC_NUM                6   /* sk um DepthCueing-Einträge erweitert*/
 
#define CURVE_COLOR_SLIDER_NUM          3   /* sk ACHTUNG auch in general.h */




enum {red,green,blue}; 					/* sk */

#ifndef min
#define min(a,b)	( ((a)<(b))?(a):(b) )
#endif

#ifndef max
#define max(a,b)	( ((a)<(b))?(b):(a) )
#endif

//  Someday we'll possibly need them again 
//  #define COLORMAP_8_BIT_STATIC          0
//  #define COLORMAP_8_BIT_DYNAMIC         1
//  #define COLORMAP_STD_TRUE_COLOR        2
//  #define COLORMAP_STD_DIRECT_COLOR      3

#endif  //!DEF_H 
