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

#ifndef COLORSELECTIONBUTTON_H
#define COLORSELECTIONBUTTON_H

#include <gtk/gtk.h>
#include <string.h>

#include "mygtk.h"

class ColorSelectionButton
{
public:
	ColorSelectionButton ();

	void setColor (double r, double g, double b);
	
	void setName (const char *aName)
		{
			strcpy (name, aName);
			char str[256];
			sprintf (str, "Press this button to select %s", aName);
			mygtk_set_tip(button, str);
		}

	void selectOn (GtkColorSelectionDialog *csd);

	GtkWidget *getWidget()
		{ return button; };

	double red,green,blue;

private:
	GtkWidget *button;
	char name [32];
	guint handler_id;

	static void selected (GtkWidget *widget, gpointer data);
	void disconnect ();
	GtkColorSelectionDialog *colorSelectionDialog;

};

#endif
