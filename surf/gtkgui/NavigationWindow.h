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

#ifndef NAVIGATIONWINDOW_H
#define NAVIGATIONWINDOW_H

#include <iostream.h>
#include "mygtk.h"
#include "Matrix33.h"
#include "WidgetReadWriter.h"

class MainWindowController;

class NavigationWindow
{
public:
	NavigationWindow(MainWindowController* winctrl);

	void show(); 

	void writeSymbols(SymbolTable& st) {
		wrw.writeSymbols(st);
	}
	
	void readSymbols (SymbolTable& st) {
		wrw.readSymbols(st);
	}
	
private:
	MainWindowController* mwc;

	WidgetReadWriter wrw;

	bool shown;
	bool rotationUpdating;
	bool scaleUpdating;
	bool originUpdating;

	GtkWidget* window;
	GtkObject* xRotSpinAdj;
	GtkObject* yRotSpinAdj;
	GtkObject* zRotSpinAdj;
	GtkObject* xScaleAdj;
	GtkObject* yScaleAdj;
	GtkObject* zScaleAdj;
	GtkWidget* maintAspectRatio;
	GtkObject* xOriginAdj;
	GtkObject* yOriginAdj;
	GtkObject* zOriginAdj;

	const Matrix33 getRotMat();
	void rotate(Matrix33 add);
	void rotateX(double rad);
	void rotateY(double rad);
	void rotateZ(double rad);
	void updateAngles();
	void updateScales();
	void updateOrigins();

	// Gtk callbacks:
	static gint handle_delete(GtkWidget* w, GdkEvent* e, gpointer d);
	VOIDCALL(xDec, NavigationWindow);
	VOIDCALL(xInc, NavigationWindow);
	VOIDCALL(yDec, NavigationWindow);
	VOIDCALL(yInc, NavigationWindow);
	VOIDCALL(zDec, NavigationWindow);
	VOIDCALL(zInc, NavigationWindow);
	VOIDCALL(xRotSpin, NavigationWindow);
	VOIDCALL(yRotSpin, NavigationWindow);
	VOIDCALL(zRotSpin, NavigationWindow);
	VOIDCALL(rotationReset, NavigationWindow);
	VOIDCALL(xScale, NavigationWindow);
	VOIDCALL(yScale, NavigationWindow);
	VOIDCALL(zScale, NavigationWindow);
	VOIDCALL(scaleReset, NavigationWindow);
	VOIDCALL(xOrigin, NavigationWindow);
	VOIDCALL(yOrigin, NavigationWindow);
	VOIDCALL(zOrigin, NavigationWindow);
	VOIDCALL(originReset, NavigationWindow);
};


#endif // !NAVIGATIONWINDOW_H
