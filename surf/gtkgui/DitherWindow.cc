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


#include <iostream.h>
#include "DitherWindow.h"
#include "RadioButtonBuilder.h"
#include "TableBuilder.h"

void DitherWindow::toggled_dither_method (GtkWidget *w, gpointer data)
{
        if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w))) {
		return;
	}

	DitherWindow *dw = (DitherWindow*)data;
	dw->ditherMethodChanged (w);
}

void DitherWindow::toggled_weight_method (GtkWidget *w, gpointer data)
{
	DitherWindow *dw = (DitherWindow *)data;
	dw->randomWeightsChanged (w);
}

void DitherWindow::toggled_enhance_method (GtkWidget *w, gpointer data)
{
	DitherWindow *dw = (DitherWindow *)data;
	dw->enhanceEdgesChanged (w);
}

void DitherWindow::toggled_gamma_method (GtkWidget *w, gpointer data)
{
	DitherWindow *dw = (DitherWindow *)data;
	dw->gammaCorrectionChanged (w);
}

void DitherWindow::ditherMethodChanged(GtkWidget *w)
{
	if ( w == dither_method.m1 || w == dither_method.m2 || w == dither_method.m3) {
		gtk_widget_set_sensitive (dither_method.con1, TRUE);
		gtk_widget_set_sensitive (dither_method.con2, FALSE);
		gtk_widget_set_sensitive (dither_method.con3, FALSE);
	} else if (w == dither_method.m4 || w == dither_method.m5) {
		gtk_widget_set_sensitive (dither_method.con1, FALSE);
		gtk_widget_set_sensitive (dither_method.con2, TRUE);
		gtk_widget_set_sensitive (dither_method.con3, FALSE);
	} else if (w == dither_method.m6 || w == dither_method.m7) {
		gtk_widget_set_sensitive (dither_method.con1, FALSE);
		gtk_widget_set_sensitive (dither_method.con2, FALSE);
		gtk_widget_set_sensitive (dither_method.con3, TRUE);
	}
}

void DitherWindow::randomWeightsChanged(GtkWidget *w)
{
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
                gtk_widget_set_sensitive(weight,FALSE);
	} else{
		gtk_widget_set_sensitive(weight,TRUE);
	}
}

void DitherWindow::enhanceEdgesChanged(GtkWidget *w)
{
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
                gtk_widget_set_sensitive(enhanceEdges.alpha,FALSE);
	} else{
		gtk_widget_set_sensitive(enhanceEdges.alpha,TRUE);
	}
}

void DitherWindow::gammaCorrectionChanged(GtkWidget *w)
{
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
                gtk_widget_set_sensitive(gamma,FALSE);
	} else{
		gtk_widget_set_sensitive(gamma,TRUE);
	}
}

void DitherWindow::initEnhanceEdges()
{
	enhanceEdges.enhanceEdges = gtk_check_button_new_with_label ("enhance edges:");
        controlWidget (enhanceEdges.enhanceEdges, "enhance_edges" );
	enhanceEdges.alpha = mygtk_new_spin (0.9, 0, 1, 0.01, 0.05, 0.1, 2);
        controlWidget (enhanceEdges.alpha, "alpha" );
        gtk_signal_connect( GTK_OBJECT(enhanceEdges.enhanceEdges),"pressed",(GtkSignalFunc)toggled_enhance_method,this );

}

DitherWindow::DitherWindow()
{
	tone_scale_adjustment = gtk_check_button_new_with_label ("tone scale adjustment");
	controlWidget (tone_scale_adjustment, "tone_scale_adjustment");
	gamma_correction = gtk_check_button_new_with_label ("gamma correction:");
	controlWidget (gamma_correction, "gamma_correction");
	gamma = mygtk_new_spin (0.0, 0.1, 5.0, 0.1, 0.2, 0.1, 1);
	controlWidget (gamma, "gamma");
        gtk_signal_connect( GTK_OBJECT(gamma_correction),"pressed",(GtkSignalFunc)toggled_gamma_method,this );


	serpentine_raster = gtk_check_button_new_with_label ("serpentine raster");
        controlWidget (serpentine_raster, "serpentine_raster" );
	random_weights = gtk_check_button_new_with_label ("random weights:");
        controlWidget (random_weights, "random_weights" );
        gtk_signal_connect( GTK_OBJECT(random_weights),"pressed",(GtkSignalFunc)toggled_weight_method,this );
	weight = mygtk_new_spin (0.4, 0, 1, 0.01, 0.05, 0.1, 2);
        controlWidget (weight, "weight" );


	GtkWidget *tmpbox = gtk_hbox_new (FALSE, 0);
//	gtk_box_pack_start (GTK_BOX(tmpbox), gtk_label_new ("weight:"), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(tmpbox), random_weights, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(tmpbox), weight, FALSE, FALSE, 0);

	GtkWidget *param1box = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(param1box), serpentine_raster, FALSE, FALSE, 0);
//	gtk_box_pack_start (GTK_BOX(param1box), random_weights, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(param1box), tmpbox, FALSE, FALSE, 0);

// 	gtk_widget_set_sensitive (param1box, FALSE);
	dither_method.con1 = param1box;

	initEnhanceEdges( );
	background = mygtk_new_spin (0.0, 0, 1, 0.01, 0.05, 0.1, 2);
        controlWidget (background, "background" );


	GtkWidget *box1 = gtk_vbox_new (FALSE, 0);
	GtkWidget *box2 = gtk_vbox_new (FALSE, 0);
	GtkWidget *box3 = gtk_vbox_new (FALSE, 0);

	RadioButtonBuilder rbb (false);
	GtkWidget *patternSizeBox = gtk_hbox_new (FALSE, 0);
	dither_method.con2 = patternSizeBox;
	gtk_box_pack_start (GTK_BOX(patternSizeBox), gtk_label_new("pattern size:"), FALSE, FALSE, 0);
	rbb.setBox (patternSizeBox);
	rbb.addButton ("4x4", "pattern_4x4");
	rbb.addButton ("8x8", "pattern_8x8");
	rbb.addButton ("16x16", "pattern_16x16");
	controlWidget (rbb.getWidget(), "pattern_size");


	TableBuilder tb2;
	tb2.setSize(5);
	tb2.newLine (box1, param1box);
	tb2.separator();
	tb2.newLine (box2, rbb.getContainer());
	tb2.separator();
	rbb.init(false);
	GtkWidget *baronBox = gtk_hbox_new (FALSE, 0);
	rbb.setBox (baronBox);
	gtk_box_pack_start (GTK_BOX(baronBox), gtk_label_new("barons:"), FALSE, FALSE, 0);
	rbb.addButton ("one", "one_baron");
	rbb.addButton ("two", "two_baron");
	controlWidget(rbb.getWidget(), "barons");
	
	tb2.newLine (box3, rbb.getContainer());
	dither_method.con3 = rbb.getContainer();

	TableBuilder tb;
	tb.setSize (3);
	
	rbb.init (false);
	rbb.setBox (box1);
	dither_method.m1 = rbb.addButton ("Floyd-Steinberg filter",		"floyd_steinberg_filter");
	dither_method.m2 = rbb.addButton ("Jarvis, Judis and Ninke filter",	"jarvis_judis_ninke_filter");
	dither_method.m3 = rbb.addButton ("Stucki filter",			"stucki_filter");

	rbb.setBox (box2);
	dither_method.m4 = rbb.addButton ("clustered dot ordered dither",	"clustered_dot_ordered_dither");
	dither_method.m5 = rbb.addButton ("dispersed dot ordered dither",	"dispersed_dot_ordered_dither");

	rbb.setBox (box3);
	dither_method.m6 = rbb.addButton ("dot diffusion",			"dot_diffusion");
	dither_method.m7 = rbb.addButton ("smooth dot diffusion",		"smooth_dot_diffusion");
	controlWidget (rbb.getWidget(), "dithering_method");

	rbb.connect_all ((GtkSignalFunc) toggled_dither_method, this);

	tmpbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(tmpbox), enhanceEdges.enhanceEdges, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(tmpbox), enhanceEdges.alpha, FALSE, FALSE, 0);
	tb.newLine (tmpbox, gtk_label_new("  "));

	//	tb.newLine ("background:", background);
	tb.newLine (tone_scale_adjustment,gtk_label_new(" "));

	tmpbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(tmpbox),gamma_correction, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(tmpbox),gamma, FALSE, FALSE, 0);
	tb.newLine (tmpbox, gtk_label_new("  "));


	GtkWidget *frame  = gtk_frame_new ("dithering method");
	GtkWidget *frame2 = gtk_frame_new ("image filters");
	GtkWidget *tmptable;
	GtkWidget *tmptable2;
	gtk_container_add (GTK_CONTAINER(frame), tmptable=tb2.getTable());
	gtk_container_add (GTK_CONTAINER(frame2) ,tmptable2=tb.getTable());
	gtk_table_set_col_spacing (GTK_TABLE(tmptable), 0, 20);
	gtk_table_set_col_spacing (GTK_TABLE(tmptable2),0, 0);
	container = gtk_vbox_new (FALSE, 10);
	gtk_box_pack_start (GTK_BOX(container), frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(container), frame2,FALSE, FALSE, 0);
}
