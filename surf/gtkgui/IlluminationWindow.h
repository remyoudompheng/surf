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


#ifndef ILLUMINATIONWINDOW_H
#define ILLUMINATIONWINDOW_H

#include "mygtk.h"
#include "ParameterWindow.h"
#include "ColorSelectionButton.h"
#include "WidgetReadWriter.h"

class IlluminationWindow : public ParameterWindow
{
private:
	IlluminationWindow (const IlluminationWindow &);
	void operator=(const IlluminationWindow &);

public:
	IlluminationWindow();
	~IlluminationWindow();

	const char *getName () 
		{ return "illumination";};
	GtkWidget *getContainer()
		{ return container; }

	virtual void writeSymbols (SymbolTable &st);
	virtual void readSymbols (SymbolTable &st);
	
private:

	GtkWidget *container;
	GtkWidget *colorseldialog;
	ColorSelectionButton * buttons [2][MAIN_SURFACE_AMOUNT_NUM];
        GtkWidget *diffuse[MAIN_SURFACE_AMOUNT_NUM];
        GtkWidget *reflected[MAIN_SURFACE_AMOUNT_NUM];
        GtkWidget *transmitted[MAIN_SURFACE_AMOUNT_NUM];
        GtkWidget *smoothness[MAIN_SURFACE_AMOUNT_NUM];
        GtkWidget *transparence[MAIN_SURFACE_AMOUNT_NUM];
        GtkWidget *thickness[MAIN_SURFACE_AMOUNT_NUM];
        ColorSelectionButton * color_buttons[LIGHT_SOURCE_MAX_VALUE];
	GtkWidget *light_x[LIGHT_SOURCE_MAX_VALUE];
	GtkWidget *light_y[LIGHT_SOURCE_MAX_VALUE];
	GtkWidget *light_z[LIGHT_SOURCE_MAX_VALUE];
	GtkWidget *volume[LIGHT_SOURCE_MAX_VALUE];

	struct {
		GtkWidget *ambient;
		GtkWidget *diffuse;
		GtkWidget *reflected;
		GtkWidget *transmitted;
	} illumination;
	
	struct {
		GtkWidget *container;
		ColorSelectionButton button;
	} gradient;

	struct {
		GtkWidget *container;
		ColorSelectionButton color;
		GtkWidget *width;
		GtkWidget *gamma;
	} curve;
	
	ColorSelectionButton backgroundColor;
	

	void init_gradient();
	void init_curve();
	
	//
	// --- GTK CALLBACKS
	// 
	
	static gint handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data);	
	static void button_clicked (GtkWidget *widget, gpointer data);
	
	VOIDCALL(colorSelected, IlluminationWindow);
	VOIDCALL(closeColorSelection, IlluminationWindow);
};


#endif
