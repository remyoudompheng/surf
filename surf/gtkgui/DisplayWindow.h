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

#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H

#include <gtk/gtk.h>
#include "mygtk.h"

#include "ParameterWindow.h"
#include "TableBuilder.h"
#include "WidgetReadWriter.h"

class DisplayWindow : public ParameterWindow
{
public:
	DisplayWindow();
	const char *getName ()
		{ return "display";};
	
	GtkWidget *getContainer()
		{
			return mainBox;
		}

private:
	GtkWidget *mainBox;
	
	void depthCueingChanged   (GtkWidget *w);
	void normalizeChanged   (GtkWidget *w);

	struct {
		GtkWidget *frame;
		GtkWidget *table;
		GtkWidget *level;
		GtkWidget *threshold;
		GtkWidget *radius;
	} antialias;
	
	struct {
		GtkWidget *frame;
		GtkWidget *box;
		GtkWidget *depth_cue;
		GtkWidget *depth_value;
	} depth_cue;

	struct {
		GtkWidget *frame;
		GtkWidget *box;
		GtkWidget *normalize_brightness;
		GtkWidget *normalize_factor;
	} normalize;
	
	struct {
		GtkWidget *frame;
		GtkWidget *eye_distance;
		GtkWidget *dist_to_screen;
		GtkWidget *left_eye_red;
		GtkWidget *right_eye_blue;
		GtkWidget *right_eye_green;
	} stereo_params;


	void init_stereo_params();
	void init_antialias();
	void init_depth_cue();
	void init_normalize();

	static void toggled_depth_method  (GtkWidget *w, gpointer data);
	static void toggled_normalize_method  (GtkWidget *w, gpointer data);

};
#endif
