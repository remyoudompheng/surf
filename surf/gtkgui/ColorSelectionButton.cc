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

#include <assert.h>
#include <stdio.h>
#include <iostream.h>

#include "ColorSelectionButton.h"

// #define DEBUG
#include "debug.h"

void ColorSelectionButton::disconnect ()
{
	gtk_signal_disconnect (GTK_OBJECT(colorSelectionDialog->ok_button), handler_id);
}

void ColorSelectionButton::selected (GtkWidget *widget, gpointer data)
{
	ColorSelectionButton *csb = (ColorSelectionButton *) data;

	gdouble color[4];
	gtk_color_selection_get_color (GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csb->colorSelectionDialog)->colorsel), 
				       color);
	csb->setColor (color[0], color[1], color[2]);
}

void ColorSelectionButton::selectOn (GtkColorSelectionDialog *csd)
{
	ColorSelectionButton *old =(ColorSelectionButton *)gtk_object_get_data (GTK_OBJECT(csd), "ColorSelectionButton");
	if (old) {
		old->disconnect();
	}

	gdouble color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = 0.0;
	gtk_color_selection_set_color (GTK_COLOR_SELECTION(csd->colorsel), color);
	char str [64];
	sprintf (str, "Choose %s", name);
	gtk_window_set_title (GTK_WINDOW(csd), str);
	handler_id = gtk_signal_connect (GTK_OBJECT(csd->ok_button), 
					 "clicked", 
					 (GtkSignalFunc) selected, this);
	colorSelectionDialog = csd;
	gtk_object_set_data (GTK_OBJECT(csd), "ColorSelectionButton", this);
	gtk_widget_show (GTK_WIDGET(csd));
	gdk_window_raise (GTK_WIDGET(csd)->window);
}

ColorSelectionButton::ColorSelectionButton ()
{
	name [0] = 0;
	button = gtk_button_new ();
	gtk_widget_set_usize (button, 20, 20);
	setColor (0,0,0);
	gtk_object_set_data (GTK_OBJECT(button), "button", this);
}

void ColorSelectionButton::setColor (double r, double g, double b)
{
	GtkStyle *style = gtk_style_copy (gtk_widget_get_default_style ());

	for (int i=0; i<5; i++) {
		
		style->bg[i].red   = (int) (r*0xffff);
		style->bg[i].green = (int) (g*0xffff);
		style->bg[i].blue  = (int) (b*0xffff);


		style->fg[i].red   = (int) (r*0xffff);
		style->fg[i].green = (int) (g*0xffff);
		style->fg[i].blue  = (int) (b*0xffff);
	}
	gtk_widget_set_style (button, style);
	
	red = r;
	green = g;
	blue = b;
}
