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


#define USE_OPTION_MENU

#include "TableBuilder.h"
#include "PositionWindow.h"


#ifdef USE_OPTION_MENU
#include "OptionMenuBuilder.h"
#else
#include "RadioButtonBuilder.h"
#endif

// some static functions
static GtkWidget *spin_button();



PositionWindow::PositionWindow()
{
	origin_x = spin_button ();
	origin_y = spin_button ();
	origin_z = spin_button ();

	spectator_z = spin_button();

	rotate_x = spin_button();
	rotate_y = spin_button();
	rotate_z = spin_button();


	scale_x = spin_button();
	scale_y = spin_button();
	scale_z = spin_button();

	TableBuilder tb;
	tb.setSize (15);
	tb.newLine ("origin x:", origin_x);
	tb.newLine ("origin y:", origin_y);
	tb.newLine ("origin z:", origin_z);

	tb.separator();

	tb.newLine ("spectator z:", spectator_z);

	tb.separator();

	tb.newLine ("rotate x:", rotate_x);
	tb.newLine ("rotate y:", rotate_y);
	tb.newLine ("rotate z:", rotate_z);
	tb.separator();

	tb.newLine ("scale factor x:", scale_x);
	tb.newLine ("scale factor y:", scale_y);
	tb.newLine ("scale factor z:", scale_z);
	tb.separator();




	GtkWidget *box=gtk_hbox_new (FALSE, 0);
#ifdef USE_OPTION_MENU
	OptionMenuBuilder omb;
	omb.addOption ("parallel", "parallel");
	omb.addOption ("central", "central");
	GtkWidget *om = omb.getOptionMenu();
	controlWidget (om, "perspective");
	tb.newLine ("perspective:", om);
	
#else
	RadioButtonBuilder rbb (false);
	rbb.setBox (box);
	rbb.addButton ("parallel", "parallel");
	rbb.addButton ("central", "central");
	controlWidget (rbb.getWidget(), "perspective");
	tb.newLine ("perspective:", rbb.getContainer());

#endif

	table = tb.getTable();


	controlWidget (origin_x, "origin_x");
	controlWidget (origin_y, "origin_y");
	controlWidget (origin_z, "origin_z");

	controlWidget (scale_x, "scale_x");
	controlWidget (scale_y, "scale_y");
	controlWidget (scale_z, "scale_z");
	
	controlWidget (spectator_z, "spec_z");

	controlWidget (rotate_x, "rot_x");
	controlWidget (rotate_y, "rot_y");
	controlWidget (rotate_z, "rot_z");

}


static GtkWidget *spin_button()
{
	GtkObject *adj = gtk_adjustment_new (0.0, -10000.0, 10000.0, 0.5, 5.0, 0.0);
	GtkWidget *spin = gtk_spin_button_new(GTK_ADJUSTMENT(adj),1.0,2);
	return spin;
}
