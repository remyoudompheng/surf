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


#ifndef OPTIONMENUBUILDER_H
#define OPTIONMENUBUILDER_H

#include "mygtk.h"

class OptionMenuBuilder
{
private:
	OptionMenuBuilder(const OptionMenuBuilder &);
	void operator=(const OptionMenuBuilder &);

public:
	OptionMenuBuilder()
		{
			optionMenu = 0;
		}

	GtkWidget *addOption (const char *name, const char *value)
		{
			init();
			GtkWidget *menuitem = gtk_radio_menu_item_new_with_label (group, name);
			gtk_object_set_data (GTK_OBJECT(menuitem), "value", (void *)value);
			group=gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM(menuitem));			

			gtk_menu_append (GTK_MENU(menu),menuitem);
			return menuitem;
		}
	
	GtkWidget *getOptionMenu ()
		{
			gtk_option_menu_set_menu (GTK_OPTION_MENU(optionMenu), menu);
			GtkWidget *retval = optionMenu;
			menu = optionMenu = 0;
			group = 0;
			return retval;
		}
	void connect_all (GtkSignalFunc func, gpointer data)
		{
		        GSList *grp = group;
			while (grp) {
				GtkWidget *w = (GtkWidget *) grp->data;
				gtk_signal_connect (GTK_OBJECT(w), "activate", func, data);
				grp = grp->next;
			}
		}

private:
	void init()
		{
			if (!optionMenu) {
				optionMenu = gtk_option_menu_new ();
				menu = gtk_menu_new();
				group=0;
			}
		}

	GtkWidget *optionMenu;
	GtkWidget *menu;
	GSList *group;
};
#endif
