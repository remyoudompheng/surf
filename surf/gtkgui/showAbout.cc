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



#include <stdio.h>
#include <iostream.h>

#include <gtk/gtk.h>

#include "showAbout.h"
#include "welcomeMessage.h"

static GtkWidget *about=0;
static GdkPixmap *pixmap=0;
static GdkBitmap *mask=0;

static void aboutDestroyed()
{
	about=0;
}

static void okayClicked()
{
	gtk_object_destroy (GTK_OBJECT(about));
}

static void unrealize (GtkWidget *window, gpointer data)
{
//  	cerr << "showAbout: unrealize called." << endl;
	if (pixmap) {
		gdk_pixmap_unref(pixmap);
		pixmap = 0;
	}
	if (mask) {
		gdk_bitmap_unref(mask);
		mask = 0;
	}
}

static void realize (GtkWidget *window, gpointer data)
{
//  	cerr << "showAbout: realize called." << endl;

	GtkStyle *style;
           
	style = gtk_style_copy (gtk_widget_get_style( window ));

	
//  	pixmap = gdk_pixmap_create_from_xpm_d( window->window,  &mask,
//  					       &style->fg[GTK_STATE_NORMAL],
//  					       (gchar **)xpm_surf_icon );

	pixmap = gdk_pixmap_create_from_xpm (window->window, &mask, &style->bg[GTK_STATE_NORMAL], DATADIR "/surf/surf.xpm");
	if (pixmap) {
//  		cerr << "showAbout: got a pixmap...." << endl;
		int i;
		for (i=0; i<5; i++) {
			style->bg_pixmap[i] = pixmap;
		}

		// gdk_window_set_back_pixmap (window->window, pixmap, 0);
	}


	gtk_widget_set_style (window, style);	

}


void showAbout ()
{
	if (about != 0) {
		gdk_window_raise(about->window);
		// gtk_widget_grab_focus (about);
		// gtk_window_activate_focus(GTK_WINDOW(about));
		// gtk_window_activate_default(GTK_WINDOW(about));
		return;
	}


	char str[64];
	about = gtk_dialog_new();
	gtk_signal_connect (GTK_OBJECT(about), "destroy", (GtkSignalFunc) aboutDestroyed,0);
	GtkWidget *label;
	gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG (about)->vbox), 10);

	label = gtk_label_new (welcomeMessage);
	GtkStyle *style = gtk_style_copy (gtk_widget_get_style( label));
	int i;
	for (i=0; i<5; i++) {
		style->fg[i].red = 0xffff;
		style->fg[i].green = 0xffff;
		style->fg[i].blue = 0xffff;
	}
	gtk_widget_set_style(label, style);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (about)->vbox),
			    label, TRUE, TRUE, 0);

	sprintf (str, "GTK-Version:%d.%d.%d\nCompiled:" __DATE__ , GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
	label = gtk_label_new (str);
	gtk_widget_set_style(label, style);

	// gtk_label_set_justify (GTK_LABEL(label), GTK_JUSTIFY_LEFT);

	gtk_box_pack_start (GTK_BOX(GTK_DIALOG(about)->vbox),
			    label, TRUE, TRUE, 0);

	GtkWidget *button = gtk_button_new_with_label ("Close");
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	gtk_box_pack_start (GTK_BOX(GTK_DIALOG(about)->action_area), button, TRUE, TRUE, 0);
	gtk_widget_grab_default (button);


	gtk_signal_connect (GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(okayClicked), 0);

	gtk_signal_connect (GTK_OBJECT(about), "realize", GTK_SIGNAL_FUNC(realize), 0);
	gtk_signal_connect (GTK_OBJECT(about), "unrealize", GTK_SIGNAL_FUNC(unrealize), 0);

	gtk_widget_show_all(about);
}
