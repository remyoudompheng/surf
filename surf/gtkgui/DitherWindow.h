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



#ifndef DITHERWINDOW_H
#define DITHERWINDOW_H

#include <gtk/gtk.h>
#include "mygtk.h"

#include "ParameterWindow.h"
#include "WidgetReadWriter.h"

class DitherWindow : public ParameterWindow
{
public:
	const char *getName ()
		{ return "dither";};

	DitherWindow( );
private:

	void ditherMethodChanged    (GtkWidget *w);
	void randomWeightsChanged   (GtkWidget *w);
	void gammaCorrectionChanged (GtkWidget *w);
	void enhanceEdgesChanged    (GtkWidget *w);

	struct {
		GtkWidget *con1;
		GtkWidget *m1;
		GtkWidget *m2;
		GtkWidget *m3;

		GtkWidget *con2;
		GtkWidget *m4;
		GtkWidget *m5;

		GtkWidget *con3;
		GtkWidget *m6;
		GtkWidget *m7;
	} dither_method;

	struct {
		GtkWidget *container;
		GtkWidget *enhanceEdges;
		GtkWidget *alpha;
	} enhanceEdges;
	
	GtkWidget *background;
	GtkWidget *tone_scale_adjustment;
	GtkWidget *gamma_correction;
	GtkWidget *gamma;


	GtkWidget *serpentine_raster;
	GtkWidget *random_weights;
	GtkWidget *weight;

	GtkWidget *barons;


	void initEnhanceEdges();
	static void toggled_dither_method  (GtkWidget *w, gpointer data);
	static void toggled_weight_method  (GtkWidget *w, gpointer data);
	static void toggled_enhance_method (GtkWidget *w, gpointer data);
	static void toggled_gamma_method   (GtkWidget *w, gpointer data);
};

#endif
