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

#ifndef POSITIONWINDOW_H
#define POSITIONWINDOW_H

#include <gtk/gtk.h>
#include "mygtk.h"

#include "ParameterWindow.h"
#include "WidgetReadWriter.h"

class PositionWindow : public ParameterWindow
{
private:
	PositionWindow (const PositionWindow &);
	void operator=(const PositionWindow &);

public:
	const char * getName ()
		{ return  "position"; }
	
	GtkWidget *getContainer ()
		{ return table; };

	
	PositionWindow();
	
private:
	GtkWidget *table;

	GtkWidget *origin_x;
	GtkWidget *origin_y;
	GtkWidget *origin_z;


	GtkWidget *spectator_z;

	GtkWidget *rotate_x;
	GtkWidget *rotate_y;
	GtkWidget *rotate_z;


	GtkWidget *scale_x;
	GtkWidget *scale_y;
	GtkWidget *scale_z;

};

#endif
