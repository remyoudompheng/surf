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
#include "ColorWindow.h"
#include "MainWindowController.h"

#define DEBUG
#include "debug.h"

gboolean ColorWindow::color_window_key_pressed (GtkWidget *w, GdkEventKey *event, gpointer data)
{
        ColorWindow *cw = (ColorWindow *) data;
	if (event->string && strcmp(event->string, "q") == 0) {
	        cw->mainWindowController->quit();
	}
	return TRUE;
}

gboolean ColorWindow::color_window_clicked (GtkWidget *w, GdkEventButton *event, gpointer data)
{
	ColorWindow *cw = (ColorWindow *) data;
	if (event->button == 3) { // fixme
		if (cw->mainWindowController) {
			cw->mainWindowController->toggleDisplayedState();
		}
	}
	return TRUE;
}

ColorWindow::ColorWindow(MainWindowController *mwc, bool colored)
	: isColored(colored), mainWindowController(mwc), surfDrawingArea(colored)
{
	surfDrawingArea.setWindowToShow(this);
	scrolledWindow = gtk_scrolled_window_new(0,0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(scrolledWindow, 200,200);

	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(scrolledWindow), surfDrawingArea.getGtkDrawingArea());

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), colored ? PACKAGE " color window" : PACKAGE " dither window");
	gtk_container_add (GTK_CONTAINER(window), scrolledWindow);
	gtk_signal_connect (GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(handle_delete), this);

	gtk_signal_connect (GTK_OBJECT(window), "button_press_event", (GtkSignalFunc) color_window_clicked, this);
	gtk_signal_connect (GTK_OBJECT(window), "key_press_event", (GtkSignalFunc) color_window_key_pressed, this);
}

DrawingArea *ColorWindow::getSurfDrawingArea() 
{
	return &surfDrawingArea;
}

void ColorWindow::show()
{
	gtk_widget_show_all(window); 
	gdk_window_raise(window->window);

	mainWindowController->enableSaveButton(isColored ? Color : Dithered);
}

void ColorWindow::lock()
{
	surfDrawingArea.lock();
}

void ColorWindow::unlock()
{
	surfDrawingArea.unlock();
}


// Callback

gint ColorWindow::handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	ColorWindow *cw = (ColorWindow *) data;
	if (!cw->surfDrawingArea.isLocked()) {
		delete cw;
		return FALSE;
	} else 
		return TRUE;
}
