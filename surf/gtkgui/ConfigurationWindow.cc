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


#include "def.h"
#include "ConfigurationWindow.h"
#include "PositionWindow.h"
#include "NumericWindow.h"
#include "IlluminationWindow.h"
#include "DisplayWindow.h"
#include "DitherWindow.h"
#include "ClipWindow.h"

static GtkWidget * makeFrame (GtkWidget *widget, const char *name)
{
	GtkWidget *frame = gtk_frame_new (name);
	gtk_container_border_width(GTK_CONTAINER(frame), 5);
	gtk_container_add (GTK_CONTAINER(frame), widget);
	return frame;
}

void ConfigurationWindow::addWindow (ParameterWindow *pw)
{
	gtk_container_border_width(GTK_CONTAINER(pw->getContainer()), 5);

	GtkWidget *frame = makeFrame (pw->getContainer(), pw->getName());
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), 
				  frame,
				  gtk_label_new (pw->getName()));
	gtk_object_set_data (GTK_OBJECT(frame), "parameterwindow", pw);
	assert(childrenCount < sizeof(children)/sizeof(ParameterWindow *));
	children[childrenCount++] = pw;
}

ConfigurationWindow::ConfigurationWindow()
{
	childrenCount=0;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);	
	gtk_signal_connect (GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC (handle_delete), this);
	gtk_window_set_title (GTK_WINDOW(window), PACKAGE " Configuration Window");

	notebook = gtk_notebook_new ();

	addWindow (new PositionWindow ());
	addWindow (new IlluminationWindow ());
        addWindow (new ClipWindow ());
	addWindow (new DisplayWindow ());
	addWindow (new DitherWindow ());
	addWindow (new NumericWindow ());

	GtkWidget *vbox = gtk_vbox_new (FALSE, 5);
	
	gtk_container_add (GTK_CONTAINER(window), vbox);
	GtkWidget *button = gtk_button_new_with_label ("print script");

	VOIDCONNECT (button, "clicked", printScript);

	gtk_box_pack_start (GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

	gtk_container_border_width(GTK_CONTAINER(window), 5);
}

ConfigurationWindow::~ConfigurationWindow()
{
	int i;
	for (i=0; i<childrenCount; i++) {
		delete children[i];
	}
}


void ConfigurationWindow::show ()
{
	gtk_widget_show_all(window);
	gdk_window_raise(window->window);
}

void ConfigurationWindow::writeSymbols (SymbolTable &st)
{
	int i;
	for (i=0; i<childrenCount; i++) {
		children[i]->wrw_writeSymbols(st);
		children[i]->writeSymbols(st);
	}
}

void ConfigurationWindow::readSymbols (SymbolTable &st)
{
	int i;
	for (i=0; i<childrenCount; i++) {
		children[i]->wrw_readSymbols(st);
		children[i]->readSymbols(st);
	}
}


//
// --- GTK CALLBACKS
// 

gint ConfigurationWindow::handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	ConfigurationWindow *cw = (ConfigurationWindow *)data;
	gtk_widget_hide (cw->window);
	return TRUE;
}

void ConfigurationWindow::printScript ()
{
	SymbolTable st;
	GtkWidget *widget = gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), 
						       gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook)));
	ParameterWindow *pw = (ParameterWindow *)gtk_object_get_data (GTK_OBJECT(widget), "parameterwindow");
	pw->writeSymbols (st);
	pw->wrw_writeSymbols(st);
	cout << st << endl;
}
