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


#include <assert.h>
#include <iostream.h>
#include <strstream.h>

#include <gtk/gtk.h>

#include "GuiThread.h"
#include "Requester.h"


static GtkWidget *alertWidget = 0;
static int choice;
static bool allowClose;

static gint deleteAlert (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	if (widget != alertWidget) {
		gdk_window_raise (alertWidget->window);
		return TRUE;
	}

	if (allowClose) {
		gtk_main_quit();  // this one effectively closes the window when gtk_main_quit returns
		choice = 0;
	}

	return TRUE;  // No, don't close the window...
}

static void closeAlert (GtkWidget *widget, gpointer data)
{
	choice = (int) data;
	gtk_main_quit();
}


static void addChoice (const char *choice, int nr)
{
	GtkWidget *button = gtk_button_new_with_label (choice);

	gtk_box_pack_start (GTK_BOX(GTK_DIALOG(alertWidget)->action_area), button, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(closeAlert), (gpointer) nr);

	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	if (nr == 0) {
		gtk_widget_grab_default (button);
	}
}


bool haveRequester()
{
	return alertWidget != 0;
}

void raiseRequester()
{
	assert(alertWidget);
	gdk_window_raise (alertWidget->window);
}

int showRequester (const char *message, 
		   const char *def, 
		   const char *alt1, 
		   const char *alt2)
{
	assert(GuiThread::isGuiThread());
	GtkWidget *old = alertWidget;
	bool oldAllowClose = allowClose;

	alertWidget = gtk_dialog_new ();


	gtk_window_set_modal (GTK_WINDOW(alertWidget),TRUE);
	gtk_signal_connect (GTK_OBJECT(alertWidget), "delete_event", (GtkSignalFunc) deleteAlert, 0);

	gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG (alertWidget)->vbox), 10);

	GtkWidget *label = gtk_label_new (message);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (alertWidget)->vbox),
			    label, TRUE, TRUE, 0);



	if (alt1) {
		addChoice (alt1,1);
		allowClose = false;
		if (alt2) {
			addChoice (alt2,2);
		}
	} else {
		allowClose = true;
	}


	if (def)
		addChoice (def, 0);
	else 
		addChoice ("Okay", 0);
		

	gtk_widget_show_all(alertWidget);
	
	gtk_grab_add (alertWidget);

	gtk_main();
	
	gtk_widget_destroy (alertWidget);	

	alertWidget = old;
	allowClose = oldAllowClose;

	return choice;
}



static void _showAlert (void *txt)
{
	showRequester ((const char *)txt, 0);
}

void showAlert (const char *alertText)
{
	if (GuiThread::isGuiThread()) {
		showRequester (alertText, 0);
	} else {
		GuiThread::addCommand (_showAlert, (void*)alertText);
		GuiThread::executeCommands();
	}
}
