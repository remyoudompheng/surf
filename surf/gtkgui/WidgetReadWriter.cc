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

#include "WidgetReadWriter.h"
#include "ColorSelectionButton.h"


// #define DEBUG
#include "debug.h"

// ------------------------------------------
//  write symbol to symtab from GUI
// ------------------------------------------

static void write_symbol (gpointer data, gpointer user_data)
{
	SymbolTable *st = (SymbolTable *) user_data;
	GtkWidget *widget = (GtkWidget *)data;
	const char *name = (const char *)gtk_object_get_data (GTK_OBJECT(widget), "variable_name");

	if (GTK_IS_OPTION_MENU(widget)) {
                DMESS("option_menu: " << name);
		GtkWidget  *active = gtk_menu_get_active (GTK_MENU(gtk_option_menu_get_menu(GTK_OPTION_MENU(widget))));
		const char *val    = (const char *)gtk_object_get_data(GTK_OBJECT(active), "value");
 		st->insert(name, val);
		return;
	} else if (GTK_IS_RADIO_BUTTON(widget)) {
                DMESS("radio_button: " << name);
		GSList *group = gtk_radio_button_group(GTK_RADIO_BUTTON(widget));

		GtkWidget *w;
		for(;;) {
			w = (GtkWidget *)group->data;
			if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w))) {
				break;
			}
			group=group->next;
			assert(group);
		}
		const char *val = (const char *)gtk_object_get_data(GTK_OBJECT(w), "rbvalue");
		st->insert(name, val);
  		return;
	}
	
	if (GTK_IS_SPIN_BUTTON(widget)) {
                DMESS("spin_button: " << name);
		// gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), st->lookupDouble(name));
		if (GTK_SPIN_BUTTON(widget)->digits==0)
			st->insertInt (name, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget)));
		else
			st->insertDouble (name, gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON (widget)));

		return;
	}
        if (GTK_IS_HSCALE(widget)) {
	        GtkAdjustment *adj = gtk_range_get_adjustment( GTK_RANGE( widget) );
                st->insertInt (name, adj->value );
                return;
	}
	if (GTK_IS_TOGGLE_BUTTON(widget)) {
                DMESS("toggle_button: " << name);
		st->insert (name,( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) ? "yes" : "no" ) );
		return;
	}
	if (GTK_IS_CHECK_BUTTON(widget)) {
                DMESS("check_button: " << name );
		st->insert (name,( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) ? "yes" : "no" ) );
		return;
	}
	if (GTK_IS_BUTTON(widget)) {
                DMESS("button: " << name );
		ColorSelectionButton *csb = (ColorSelectionButton *) gtk_object_get_data (GTK_OBJECT(widget), "button");
		if (csb) {
			char str[256];
			sprintf (str, "%s_red", name);
			st->insertInt (str, (int) (csb->red *0xff));

			sprintf (str, "%s_green", name);
			st->insertInt (str, (int) (csb->green *0xff));

			sprintf (str, "%s_blue", name);
			st->insertInt (str, (int) (csb->blue *0xff));
			return;
		}
	}
	assert(0);
}

// ------------------------------------------
//  read symbol from symtab and write to GUI
// ------------------------------------------

static void read_symbol (gpointer data, gpointer user_data)
{
	SymbolTable *st = (SymbolTable *) user_data;
	GtkWidget *widget = (GtkWidget *)data;
	const char *name = (const char *)gtk_object_get_data (GTK_OBJECT(widget), "variable_name");

	if (GTK_IS_OPTION_MENU(widget)) {
                DMESS("option_menu: " << name );
		GSList *group = GTK_RADIO_MENU_ITEM(GTK_OPTION_MENU(widget)->menu_item)->group;
		assert(group);
		const char *value = st->lookup(name);
		GtkWidget *w;
		for (;;) {
			w = (GtkWidget *)group->data;
			const char *this_value = (const char *)gtk_object_get_data(GTK_OBJECT(w), "value");
			assert(value);

			group=group->next;

			if (strcmp (value, this_value)==0) {

				// gtk_option_menu_set_history (GTK_OPTION_MENU(widget), g_slist_length(group)-1);

				// this seems really really strange...but actually works. Understand that
				// the first option added to the option menu, is the last in the list !
				gtk_option_menu_set_history (GTK_OPTION_MENU(widget), g_slist_length(group));
				break;
			}

			if (group == 0) {
			        cerr << __FILE__ << " " << __LINE__ << ": WARNING: value " << value << " for variable " << name << " unknown" << endl;
				break;
			}
			
		}
		return;
	} else if (GTK_IS_RADIO_BUTTON(widget)) {
                DMESS("radio_button: " << name );
		const char *value = st->lookup(name);
		
		GSList *group = gtk_radio_button_group(GTK_RADIO_BUTTON(widget));
		
		GtkWidget *w;
		for(;;) {
			w = (GtkWidget *)group->data;
			const char *w_val = (const char *)gtk_object_get_data(GTK_OBJECT(w),"rbvalue");
			if (strcmp(w_val, value)==0) {
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), TRUE);
				break;
			}
			group=group->next;
			if (group == 0) {
				cerr << __FILE__ << " " << __LINE__ << ": WARNING: value " << value << " for variable " << name << " unknown" << endl;
				break;
			}
		}
  		return;
	}

	if (GTK_IS_SPIN_BUTTON(widget)) {
                DMESS("spin_button: " << name );
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), st->lookupDouble(name));
		// st->insertDouble (name, gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON (widget)));
		return;
	}

        if (GTK_IS_HSCALE(widget)) {
	        GtkAdjustment *adj = gtk_range_get_adjustment( GTK_RANGE( widget) );
                gtk_adjustment_set_value( adj,st->lookupInt(name) );
                return;
	}

	if (GTK_IS_TOGGLE_BUTTON(widget)) {
                DMESS("check_button: " << name );
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), !st->lookupInt(name));
		return;
	}

	if (GTK_IS_CHECK_BUTTON(widget)) {
                DMESS("toggle_button: " << name );
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), !st->lookupInt(name));
		return;
	}

	if (GTK_IS_BUTTON(widget)) {
                DMESS("button: " << name );
		ColorSelectionButton *csb = (ColorSelectionButton *) gtk_object_get_data (GTK_OBJECT(widget), "button");
		if (csb) {
			char str[256];
			sprintf (str, "%s_red", name);
			double r = st->lookupInt (str) / 255.0;

			sprintf (str, "%s_green", name);
			double g = st->lookupInt (str) / 255.0;

			sprintf (str, "%s_blue", name);
			double b = st->lookupInt (str) / 255.0;
			csb->setColor (r, g, b);
			return;
		}
	}


	assert(0);
}


WidgetReadWriter::WidgetReadWriter()
{
	list = 0;
}

void WidgetReadWriter::addWidget (GtkWidget *widget, const char *variable_name)
{
	list = g_slist_prepend (list, widget);
	gtk_object_set_data (GTK_OBJECT(widget), "variable_name", (void*)variable_name);
}

void WidgetReadWriter::writeSymbols (SymbolTable &st)
{
	g_slist_foreach (list, write_symbol, &st);
}

void WidgetReadWriter::readSymbols (SymbolTable &st)
{
	g_slist_foreach (list, read_symbol, &st);
}
