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
#include <cmath>

class MainWindowController;

class NavigationWindow
{
public:
	NavigationWindow(MainWindowController* winctrl);

	void show(); 

	void writeSymbols(SymbolTable& st) {
		wrw.writeSymbols(st);
	}
	
	void readSymbols(SymbolTable& st) {
		wrw.readSymbols(st);
	}
	
private:
	MainWindowController* mwc;

	WidgetReadWriter wrw;

	bool shown;
	bool updating;

	GtkWidget* window;
	GtkAdjustment* xRotScaleAdj;
	GtkAdjustment* yRotScaleAdj;
	GtkAdjustment* zRotScaleAdj;
	GtkAdjustment* xRotSpinAdj;
	GtkAdjustment* yRotSpinAdj;
	GtkAdjustment* zRotSpinAdj;
	GtkAdjustment* xScaleAdj;
	GtkAdjustment* yScaleAdj;
	GtkAdjustment* zScaleAdj;
	GtkToggleButton* maintAspectRatio;
	GtkAdjustment* xOriginAdj;
	GtkAdjustment* yOriginAdj;
	GtkAdjustment* zOriginAdj;

	const Matrix33 getRotMat();
	void rotate(Matrix33 add);
	void updateAngles();
	void updateScales();
	void updateOrigin();
	double trimAngle(double rad);
	double trimScale(double scl);
	double trimOrig(double orig);

	// Gtk callbacks:
	EVENTCALL(handle_delete, NavigationWindow);
	VOIDCALL(xRotScale, NavigationWindow);
	VOIDCALL(yRotScale, NavigationWindow);
	VOIDCALL(zRotScale, NavigationWindow);
	EVENTCALL(xRotDone, NavigationWindow);
	EVENTCALL(yRotDone, NavigationWindow);
	EVENTCALL(zRotDone, NavigationWindow);
	VOIDCALL(xRotSpin, NavigationWindow);
	VOIDCALL(yRotSpin, NavigationWindow);
	VOIDCALL(zRotSpin, NavigationWindow);
	VOIDCALL(xScale, NavigationWindow);
	VOIDCALL(yScale, NavigationWindow);
	VOIDCALL(zScale, NavigationWindow);
	VOIDCALL(xOrigin, NavigationWindow);
	VOIDCALL(yOrigin, NavigationWindow);
	VOIDCALL(zOrigin, NavigationWindow);
	VOIDCALL(getScriptValues, NavigationWindow);

	double rotX, rotY, rotZ;
	double tmpRotX, tmpRotY, tmpRotZ;
	double scaleX, scaleY, scaleZ;
	double origX, origY, origZ;

	static const double minRot = 0;
	static const double maxRot = 2*M_PI;
	static const double minScale = 0.001;
	static const double maxScale = 100;
	static const double minOrig = -100;
	static const double maxOrig = 200;
};


#endif // !NAVIGATIONWINDOW_H
