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


#ifndef CLIPWINDOW_H
#define CLIPWINDOW_H

#include <gtk/gtk.h>
#include "mygtk.h"

#include "ParameterWindow.h"
#include "WidgetReadWriter.h"

class ClipWindow : public ParameterWindow
{
public:
	const char *getName( )
		{ return "clip";};
	GtkWidget  *getContainer( )
		{ return table; };


	ClipWindow( );
private:
        GtkWidget *table;

        GtkWidget *clip_front;
        GtkWidget *clip_back;

        GtkWidget *clip_method;

        GtkWidget *radius;
        GtkWidget *center_x;
        GtkWidget *center_y;
        GtkWidget *center_z;

	void clipMethodChanged    (GtkWidget *w);
	static void toggled_clip_method (GtkWidget *w, gpointer data);
};

#endif
