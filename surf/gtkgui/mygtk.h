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



#ifndef MYGTK_H
#define MYGTK_H

#include <gtk/gtk.h>
#include <iostream.h>

#define RECALL(method,klass)							\
static void _ ## method (klass *k)						\
{										\
	k->method();								\
}										\
void method()

#define VOIDCALL(method,klass)							\
static void _ ## method (GtkWidget *widget, gpointer data)			\
{										\
	((klass*)data)->method();						\
}										\
void method ()


#define MENUCALLBACK(method,klass)							\
static void m_ ## method (gpointer data, guint action, GtkWidget *widget)	\
{										\
	((klass*)data)->method();						\
}

#define MENUCALL(method) (GtkItemFactoryCallback) m_ ## method

#define VOIDCONNECT(obj, signal, method) \
 gtk_signal_connect(GTK_OBJECT(obj), signal, (GtkSignalFunc) & _ ## method, this)


inline GtkWidget * mygtk_new_spin (double value, double lower, double upper, double inc1, double inc2, double accel, int digits)
{
	GtkObject *adj = gtk_adjustment_new (value, lower, upper, inc1, inc2, 0);
	return gtk_spin_button_new (GTK_ADJUSTMENT(adj), accel, digits);
}

extern void mygtk_set_tip (GtkWidget *widget, const char *str);
#endif
