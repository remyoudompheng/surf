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

#include "DisplayWindow.h"

#include "TableBuilder.h"

void DisplayWindow::toggled_depth_method (GtkWidget *w, gpointer data)
{
	DisplayWindow *dw = (DisplayWindow *)data;
	dw->depthCueingChanged (w);
}

void DisplayWindow::depthCueingChanged(GtkWidget *w)
{
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
                gtk_widget_set_sensitive(depth_cue.depth_value,FALSE);
	} else{
		gtk_widget_set_sensitive(depth_cue.depth_value,TRUE);
	}
}

void DisplayWindow::toggled_normalize_method (GtkWidget *w, gpointer data)
{
	DisplayWindow *dw = (DisplayWindow *)data;
	dw->normalizeChanged (w);
}

void DisplayWindow::normalizeChanged(GtkWidget *w)
{
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
                gtk_widget_set_sensitive(normalize.normalize_factor,FALSE);
	} else{
		gtk_widget_set_sensitive(normalize.normalize_factor,TRUE);
	}
}

static GtkWidget *get_color_spin()
{
	GtkObject *adj;
	GtkWidget *spin;
	adj = gtk_adjustment_new (0.0, 0.0, 1.0, 0.001, 0.01, 0.0);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT(adj), 0.01, 3);
	return spin;
}

void DisplayWindow::init_stereo_params()
{
	stereo_params.frame = gtk_frame_new ("stereo image");
	gtk_container_border_width (GTK_CONTAINER(stereo_params.frame), 5);
		
	TableBuilder tb;
	tb.setSize (5);
	
	stereo_params.eye_distance = mygtk_new_spin (0.0, -10.0, 10.0, 0.1, 1.0, 0.1, 1);
	tb.newLine ("eye distance:", stereo_params.eye_distance);
	

	stereo_params.dist_to_screen = mygtk_new_spin (0.0, -30.0, 30.0, 0.1, 1.0, 0.1, 1);
	
	tb.newLine ("distance from screen:", stereo_params.dist_to_screen);

	tb.newLine ("left eye red value:",	stereo_params.left_eye_red    = get_color_spin());
	tb.newLine ("right eye blue value:",	stereo_params.right_eye_blue  = get_color_spin());
	tb.newLine ("right eye green value:",	stereo_params.right_eye_green = get_color_spin());
	GtkWidget *table = tb.getTable();
	
	gtk_container_add (GTK_CONTAINER(stereo_params.frame), table);

	controlWidget (stereo_params.eye_distance, "stereo_eye");
	controlWidget (stereo_params.dist_to_screen, "stereo_z");
	controlWidget (stereo_params.left_eye_red, "stereo_red");
	controlWidget (stereo_params.right_eye_blue, "stereo_blue");
	controlWidget (stereo_params.right_eye_green, "stereo_green");
}

void DisplayWindow::init_antialias()
{
	//
	// --- initialize antialias struct
	// 
	antialias.frame = gtk_frame_new ("antialiasing");
	gtk_container_border_width (GTK_CONTAINER(antialias.frame), 5);

	TableBuilder tb;
	tb.setSize(3);

	antialias.level = mygtk_new_spin(1.0, 1.0, 8.0, 1,1,0,0);
	tb.newLine ("level:", antialias.level);
	

	antialias.threshold = mygtk_new_spin (0.0001, 0.0001, 0.9999, 0.0001, 0.0001, 0.4, 4);
	tb.newLine ("threshold:", antialias.threshold);


	antialias.radius = mygtk_new_spin (0.5, 0.5, 5.0, 0.1, 0.2, 0.1, 1);
	tb.newLine ("radius:", antialias.radius);

	antialias.table = tb.getTable();
  	gtk_container_border_width (GTK_CONTAINER(antialias.table), 5);
  	gtk_container_add (GTK_CONTAINER(antialias.frame), antialias.table);

	controlWidget (antialias.level, "antialiasing");
	controlWidget (antialias.threshold, "antialiasing_threshold");
	controlWidget (antialias.radius, "antialiasing_radius");
}


void DisplayWindow::init_depth_cue()
{
	//
	// --- initialize depth_cue struct
	//
	depth_cue.box = gtk_hbox_new (FALSE, 0);
	depth_cue.depth_cue = gtk_check_button_new_with_label ("depth:");
	gtk_box_pack_start (GTK_BOX(depth_cue.box), depth_cue.depth_cue, FALSE, FALSE,0);
	depth_cue.depth_value = mygtk_new_spin (14.0, -1000.0, 10.0, 0.1, 1.0, 0.1, 1);
	gtk_box_pack_start (GTK_BOX(depth_cue.box), depth_cue.depth_value, TRUE, TRUE, 0);

	depth_cue.frame = gtk_frame_new ("depth cueing");
	gtk_container_border_width (GTK_CONTAINER(depth_cue.frame),5);
	gtk_container_add (GTK_CONTAINER(depth_cue.frame), depth_cue.box);

        gtk_signal_connect( GTK_OBJECT(depth_cue.depth_cue),"pressed",(GtkSignalFunc)toggled_depth_method,this );

	controlWidget (depth_cue.depth_cue, "depth_cueing");
	controlWidget (depth_cue.depth_value, "depth");
}

void DisplayWindow::init_normalize()
{
	//
	// --- initialize normalize struct
	//
	normalize.box = gtk_hbox_new (FALSE, 0);
	normalize.normalize_brightness = gtk_check_button_new_with_label ("factor:");
	gtk_box_pack_start (GTK_BOX(normalize.box), normalize.normalize_brightness, FALSE, FALSE,0);
	normalize.normalize_factor = mygtk_new_spin (1.0, 0.0, 100.0, 0.01, 1.0, 0.1, 1);
	gtk_box_pack_start (GTK_BOX(normalize.box), normalize.normalize_factor, TRUE, TRUE, 0);

	normalize.frame = gtk_frame_new ("normalize brightness");
	gtk_container_border_width (GTK_CONTAINER(normalize.frame),5);
	gtk_container_add (GTK_CONTAINER(normalize.frame), normalize.box);

        gtk_signal_connect( GTK_OBJECT(normalize.normalize_brightness),"pressed",(GtkSignalFunc)toggled_normalize_method,this );

	controlWidget (normalize.normalize_brightness, "normalize_brightness");
	controlWidget (normalize.normalize_factor, "normalize_factor");
}

DisplayWindow::DisplayWindow ()
{
	init_stereo_params( );
	init_normalize( );
	init_antialias( );
	init_depth_cue( );

	//
	// --- initialize remaining parts
	// 

	mainBox = gtk_vbox_new (FALSE, 10);
	gtk_container_border_width (GTK_CONTAINER(mainBox), 5);


#if 0
	GtkWidget *optimizeColors = gtk_check_button_new_with_label ("optimize colors");
	gtk_box_pack_start (GTK_BOX(mainBox), optimizeColors, FALSE, FALSE ,0);

	GtkWidget *ditherColor = gtk_check_button_new_with_label ("dither colors");
	gtk_box_pack_start (GTK_BOX(mainBox), ditherColor, FALSE, FALSE ,0);

	GtkWidget *normBrightness = gtk_check_button_new_with_label ("normalize brightness");
	gtk_box_pack_start (GTK_BOX(mainBox), normBrightness, FALSE, FALSE ,0);
#endif


	gtk_box_pack_start (GTK_BOX(mainBox), antialias.frame, FALSE, FALSE ,0);
	gtk_box_pack_start (GTK_BOX(mainBox), normalize.frame, FALSE, FALSE ,0);
	gtk_box_pack_start (GTK_BOX(mainBox), depth_cue.frame, FALSE, FALSE ,0);
	gtk_box_pack_start (GTK_BOX(mainBox), stereo_params.frame, FALSE, FALSE ,0);
}

