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



#ifndef RADIOBUTTONBUILDER_H
#define RADIOBUTTONBUILDER_H

#include "mygtk.h"

class RadioButtonBuilder
{
public:
	void init(bool wantBox=true)
		{
			radioButton = 0;
			box = wantBox ? gtk_vbox_new (FALSE, 0) : 0;
		}

	RadioButtonBuilder(bool wantBox=true)
		{
			radioButton = 0;
			box = wantBox ? gtk_vbox_new (FALSE, 0) : 0;
		}
	GtkWidget *addButton (const char *name, const char *value) 
		{
			GSList *group = radioButton ? gtk_radio_button_group(GTK_RADIO_BUTTON(radioButton)) : 0;
			radioButton = gtk_radio_button_new_with_label (group, name);
			gtk_object_set_data (GTK_OBJECT(radioButton), "rbvalue", (void*)value);

			if (box)
				gtk_box_pack_start (GTK_BOX(box), radioButton, FALSE, FALSE, 0);
			return radioButton;
		}
	GtkWidget *getContainer ()
		{
			return box;
		}

	void setBox (GtkWidget *aBox)
		{
			box = aBox;
		}
	
	GtkWidget *getWidget()
		{
			return radioButton;
		}
	void connect_all (GtkSignalFunc func, gpointer data)
		{
			GSList *group = gtk_radio_button_group(GTK_RADIO_BUTTON(radioButton));
			while (group) {
				GtkWidget *w = (GtkWidget *) group->data;
				gtk_signal_connect (GTK_OBJECT(w), "toggled", func, data);
				group = group->next;
			}
		}
private:
	GtkWidget *box;
	GtkWidget *radioButton;
};
#endif
