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

#ifndef COLORWINDOW_H
#define COLORWINDOW_H

#include <assert.h>

#include <gtk/gtk.h>

#include "DrawingArea.h"

class ColorWindow
{
private:
	ColorWindow (const ColorWindow &);
	void operator=(const ColorWindow &);

public:
	ColorWindow(class MainWindowController *mwc, bool colored=true);
	DrawingArea *getSurfDrawingArea();

	void show(); 
	
	void lock();
	void unlock();

protected:
	GtkWidget *window;
	GtkWidget *scrolledWindow;
	class MainWindowController *mainWindowController;
	DrawingArea surfDrawingArea;
protected:
	// callbacks
	static gint handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data);
	static gboolean color_window_clicked (GtkWidget *w, GdkEventButton *event, gpointer data);
	static gboolean color_window_key_pressed (GtkWidget *w, GdkEventKey *event, gpointer data);
};
#endif
