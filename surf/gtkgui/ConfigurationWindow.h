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


#ifndef CONFIGURATIONWINDOW_H
#define CONFIGURATIONWINDOW_H

#include <iostream.h>
#include "mygtk.h"

#include "ParameterWindow.h"

class ConfigurationWindow
{
private:
	ConfigurationWindow(const ConfigurationWindow&);
	void operator=(const ConfigurationWindow &);

public:
	ConfigurationWindow();
	~ConfigurationWindow();

	void show (); 
	void writeSymbols (SymbolTable &st);
	void readSymbols (SymbolTable &st);


private:
	GtkWidget *window;
	GtkWidget *notebook;

	ParameterWindow * children[16];
	int childrenCount;

	void addWindow (ParameterWindow *pw);


	//
	// --- GTK CALLBACKS
	// 

	VOIDCALL(printScript, ConfigurationWindow);
	static gint handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data);
};

#endif
