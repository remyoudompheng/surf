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

#include "MainWindowController.h"
#include "NavigationWindow.h"
#include "gui_config.h"

#define GTK_NONE GtkAttachOptions(0)

// M_PI isn't standard C/C++ but gcc's math.h defines it..
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

NavigationWindow::NavigationWindow(MainWindowController* winctrl)
	: mwc(winctrl), shown(false), updating(false)
{
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
	EVENTCONNECT(window, "delete_event", handle_delete);
	gtk_window_set_title(GTK_WINDOW (window), PACKAGE " Navigation Window");
	gtk_container_border_width(GTK_CONTAINER (window), 5);
	gtk_widget_set_usize(window, 320, 0);

	GtkWidget* mainbox;
	mainbox = gtk_vbox_new(false, 5);
	gtk_container_add(GTK_CONTAINER(window), mainbox);

	GtkWidget* notebook;
	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(mainbox), notebook, false, false, 0);


	// Notebook:
	// ---------

	// "Rotation" tab:

	GtkWidget* vbox;
	vbox = gtk_vbox_new(false, 5);
	gtk_container_border_width(GTK_CONTAINER(vbox), 4);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Rotation"));

	GtkWidget* frame;
	frame = gtk_frame_new("relative:");
	gtk_box_pack_start(GTK_BOX(vbox), frame, false, false, 0);
	gtk_container_border_width(GTK_CONTAINER(frame), 4);

	GtkWidget* table;
	table = gtk_table_new(3, 2, false);
	gtk_container_add(GTK_CONTAINER(frame), table);
	gtk_container_border_width(GTK_CONTAINER(table), 4);

	GtkWidget* widget;
	widget = gtk_label_new("X:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 0, 1, GTK_NONE, GTK_NONE, 0, 0);
	xRotScaleAdj = (GtkAdjustment*)(gtk_adjustment_new(0, -M_PI, M_PI, M_PI/50, 1, 0));
	VOIDCONNECT(xRotScaleAdj, "value_changed", xRotScale);
	widget = gtk_hscale_new(xRotScaleAdj);
	EVENTCONNECT(widget, "button-release-event", xRotDone);
	gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
	gtk_scale_set_digits(GTK_SCALE(widget), 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 0, 1);

	widget = gtk_label_new("Y:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 1, 2, GTK_NONE, GTK_NONE, 0, 0);
	yRotScaleAdj = (GtkAdjustment*)(gtk_adjustment_new(0, -M_PI, M_PI, M_PI/50, 1, 0));
	VOIDCONNECT(yRotScaleAdj, "value_changed", yRotScale);
	widget = gtk_hscale_new(yRotScaleAdj);
	EVENTCONNECT(widget, "button-release-event", yRotDone);
	gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
	gtk_scale_set_digits(GTK_SCALE(widget), 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 1, 2);

	widget = gtk_label_new("Z:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 2, 3, GTK_NONE, GTK_NONE, 0, 0);
	zRotScaleAdj = (GtkAdjustment*)(gtk_adjustment_new(0, -M_PI, M_PI, M_PI/50, 1, 0));
	VOIDCONNECT(zRotScaleAdj, "value_changed", zRotScale);
	widget = gtk_hscale_new(zRotScaleAdj);
	EVENTCONNECT(widget, "button-release-event", zRotDone);
	gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
	gtk_scale_set_digits(GTK_SCALE(widget), 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 2, 3);


	frame = gtk_frame_new("absolute:");
	gtk_box_pack_start(GTK_BOX(vbox), frame, false, false, 0);
	gtk_container_border_width(GTK_CONTAINER(frame), 4);

	GtkWidget* hbox;
	hbox = gtk_hbox_new(false, 5);
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_border_width(GTK_CONTAINER(hbox), 4);

	widget = gtk_label_new("X:");
	gtk_box_pack_start(GTK_BOX(hbox), widget, false, false, 0);
	xRotSpinAdj = (GtkAdjustment*)(gtk_adjustment_new(0, minRot, maxRot, M_PI/50, 1, 0));
	VOIDCONNECT(xRotSpinAdj, "value_changed", xRotSpin);
	widget = gtk_spin_button_new(xRotSpinAdj, 0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), widget, true, true, 0);
	wrw.addWidget(widget, "rot_x");

	widget = gtk_label_new("Y:");
	gtk_box_pack_start(GTK_BOX(hbox), widget, false, false, 0);
	yRotSpinAdj = (GtkAdjustment*)(gtk_adjustment_new(0, minRot, maxRot, M_PI/50, 1, 0));
	VOIDCONNECT(yRotSpinAdj, "value_changed", yRotSpin);
	widget = gtk_spin_button_new(yRotSpinAdj, 0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), widget, true, true, 0);
	wrw.addWidget(widget, "rot_y");

	widget = gtk_label_new("Z:");
	gtk_box_pack_start(GTK_BOX(hbox), widget, false, false, 0);
	zRotSpinAdj = (GtkAdjustment*)(gtk_adjustment_new(0, minRot, maxRot, M_PI/50, 1, 0));
	VOIDCONNECT(zRotSpinAdj, "value_changed", zRotSpin);
	widget = gtk_spin_button_new(zRotSpinAdj, 0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), widget, true, true, 0);
	wrw.addWidget(widget, "rot_z");


	// "Scale" tab

	vbox = gtk_vbox_new(false, 5);
	gtk_container_border_width(GTK_CONTAINER(vbox), 4);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Scale"));
	
	table = gtk_table_new(3, 2, false);
	gtk_box_pack_start(GTK_BOX(vbox), table, false, false, 0);

	widget = gtk_label_new("X:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 0, 1, GTK_NONE, GTK_NONE, 0, 0);
	xScaleAdj = (GtkAdjustment*)(gtk_adjustment_new(1, minScale, maxScale, 0.1, 1, 0));
	VOIDCONNECT(xScaleAdj, "value_changed", xScale);
	widget = gtk_spin_button_new(xScaleAdj, 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 0, 1);
	wrw.addWidget(widget, "scale_x");

	widget = gtk_label_new("Y:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 1, 2, GTK_NONE, GTK_NONE, 0, 0);
	yScaleAdj = (GtkAdjustment*)(gtk_adjustment_new(1, minScale, maxScale, 0.1, 1, 0));
	VOIDCONNECT(yScaleAdj, "value_changed", yScale);
	widget = gtk_spin_button_new(yScaleAdj, 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 1, 2);
	wrw.addWidget(widget, "scale_y");

	widget = gtk_label_new("Z:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 2, 3, GTK_NONE, GTK_NONE, 0, 0);
	zScaleAdj = (GtkAdjustment*)(gtk_adjustment_new(1, minScale, maxScale, 0.1, 1, 0));
	VOIDCONNECT(zScaleAdj, "value_changed", zScale);
	widget = gtk_spin_button_new(zScaleAdj, 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 2, 3);
	wrw.addWidget(widget, "scale_z");

	maintAspectRatio = (GtkToggleButton*)(gtk_check_button_new_with_label("maintain aspect ratio"));
	gtk_toggle_button_set_active(maintAspectRatio, true);
	gtk_box_pack_start(GTK_BOX(vbox), (GtkWidget*)(maintAspectRatio), false, false, 0);


	// "Origin" tab

	vbox = gtk_vbox_new(false, 5);
	gtk_container_border_width(GTK_CONTAINER(vbox), 4);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Origin"));
	
	table = gtk_table_new(3, 2, false);
	gtk_box_pack_start(GTK_BOX(vbox), table, false, false, 0);

	widget = gtk_label_new("X:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 0, 1, GTK_NONE, GTK_NONE, 0, 0);
	xOriginAdj = (GtkAdjustment*)(gtk_adjustment_new(0, minOrig, maxOrig, 1, 10, 0));
	VOIDCONNECT(xOriginAdj, "value_changed", xOrigin);
	widget = gtk_spin_button_new(xOriginAdj, 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 0, 1);
	wrw.addWidget(widget, "origin_x");

	widget = gtk_label_new("Y:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 1, 2, GTK_NONE, GTK_NONE, 0, 0);
	yOriginAdj = (GtkAdjustment*)(gtk_adjustment_new(0, minOrig, maxOrig, 1, 10, 0));
	VOIDCONNECT(yOriginAdj, "value_changed", yOrigin);
	widget = gtk_spin_button_new(yOriginAdj, 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 1, 2);
	wrw.addWidget(widget, "origin_y");

	widget = gtk_label_new("Z:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 2, 3, GTK_NONE, GTK_NONE, 0, 0);
	zOriginAdj = (GtkAdjustment*)(gtk_adjustment_new(0, minOrig, maxOrig, 1, 10, 0));
	VOIDCONNECT(zOriginAdj, "value_changed", zOrigin);
	widget = gtk_spin_button_new(zOriginAdj, 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 2, 3);
	wrw.addWidget(widget, "origin_z");


	// get script values button:

	hbox = gtk_hbox_new(false, 5);
	gtk_box_pack_start(GTK_BOX(mainbox), hbox, false, false, 0);
	widget = gtk_button_new_with_label("get script values");
	gtk_box_pack_end(GTK_BOX(hbox), widget, false, false, 0);
	VOIDCONNECT(widget, "clicked", getScriptValues);
}


void NavigationWindow::show()
{
	gtk_widget_show_all(window);
	gdk_window_raise(window->window);
	shown = true;
}

gint NavigationWindow::handle_delete()
{
	gtk_widget_hide(window);
	shown = false;
	return true;
}

void NavigationWindow::xRotScale()
{
	if(shown && !updating) {
		double offset = xRotScaleAdj->value;
		double sinx = std::sin(offset), cosx = std::cos(offset);
		double rotXArr[] = {
			1,   0,    0,
			0,  cosx, sinx,
			0, -sinx, cosx
		};
		rotate(Matrix33(rotXArr));
	}
}

void NavigationWindow::yRotScale()
{
	if(shown && !updating) {
		double offset = yRotScaleAdj->value;
		double siny = std::sin(offset), cosy = std::cos(offset);
		double rotYArr[] = {
			cosy, 0, -siny,
			  0,  1,   0,
			siny, 0,  cosy
		};
		rotate(Matrix33(rotYArr));
	}
}

void NavigationWindow::zRotScale()
{
	if(shown && !updating) {
		double offset = zRotScaleAdj->value;
		double sinz = std::sin(offset), cosz = std::cos(offset);
		double rotZArr[] = {
			 cosz, sinz, 0,
			-sinz, cosz, 0,
			   0,    0,  1
		};
		rotate(Matrix33(rotZArr));
	}
}

gint NavigationWindow::xRotDone()
{
	updating = true;
	gtk_adjustment_set_value(xRotScaleAdj, 0);
	updating = false;
	rotX = tmpRotX;
	updateAngles();
	return 0;
}

gint NavigationWindow::yRotDone()
{
	updating = true;
	gtk_adjustment_set_value(yRotScaleAdj, 0);
	updating = false;
	rotY = tmpRotY;
	updateAngles();
	return 0;
}

gint NavigationWindow::zRotDone()
{
	updating = true;
	gtk_adjustment_set_value(zRotScaleAdj, 0);
	updating = false;
	rotZ = tmpRotZ;
	updateAngles();
	return 0;
}

void NavigationWindow::xRotSpin()
{
	if(shown && !updating) {
		tmpRotX = rotX = xRotSpinAdj->value;
		mwc->drawSurfaceWithParams();
	}
}

void NavigationWindow::yRotSpin()
{
	if(shown && !updating) {
		tmpRotY = rotY = yRotSpinAdj->value;
		mwc->drawSurfaceWithParams();
	}
}

void NavigationWindow::zRotSpin()
{
	if(shown && !updating) {
		tmpRotZ = rotZ = zRotSpinAdj->value;
		mwc->drawSurfaceWithParams();
	}
}

inline const Matrix33 NavigationWindow::getRotMat()
{
	double cx = std::cos(rotX);
	double sx = std::sin(rotX);
	double cy = std::cos(rotY);
	double sy = std::sin(rotY);
	double cz = std::cos(rotZ);
	double sz = std::sin(rotZ);
	double origRotArr[] = {
		 cy*cz + sx*sy*sz,   cx*sz,  -cz*sy + cy*sx*sz,
		-cy*sz + cz*sx*sy,   cx*cz,   sy*sz + cy*cz*sx,
		      cx*sy,          -sx,          cx*cy
	};
	return Matrix33(origRotArr);
}


void NavigationWindow::rotate(Matrix33 add)
{
	double sin_x, cos_x, sin_y, cos_y, sin_z, cos_z;
	double rad_x;
	Matrix33 rotd;

	rotd = add * getRotMat();

	sin_x = -rotd[2*3 + 1];
	rad_x = std::asin(sin_x);
	cos_x = std::cos(rad_x);
	cos_y = rotd[2*3 + 2] / cos_x;
	sin_y = rotd[2*3 + 0] / cos_x;
	sin_z = rotd[0*3 + 1] / cos_x;
	cos_z = rotd[1*3 + 1] / cos_x;

	if(sin_y > 1) sin_y = 1;
	else if(sin_y < -1) sin_y = -1;

	if(cos_y > 1) cos_y = 1;
	else if(cos_y < -1) cos_y = -1;

	if(sin_z > 1) sin_z = 1;
	else if(sin_z < -1) sin_z = -1;

	if(cos_z > 1) cos_z = 1;
	else if(cos_z < -1) cos_z = -1;

	tmpRotX = trimAngle(std::atan2(sin_x, cos_x));
	tmpRotY = trimAngle(std::atan2(sin_y, cos_y));
	tmpRotZ = trimAngle(std::atan2(sin_z, cos_z));

	updateAngles();

	mwc->drawSurfaceWithParams();
}

double NavigationWindow::trimAngle(double rad)
{
	while(rad < 0) {
		rad += 2*M_PI;
	}
	while(rad > 2*M_PI) {
		rad -= 2*M_PI;
	}
	return rad;
}

double NavigationWindow::trimScale(double scl)
{
	if(scl < minScale) {
		scl = minScale;
	} else if(scl > maxScale) {
		scl = maxScale;
	}
	return scl;
}

double NavigationWindow::trimOrig(double orig)
{
	if(orig < minOrig) {
		orig = minOrig;
	} else if(orig > maxOrig) {
		orig = maxOrig;
	}
	return orig;
}

void NavigationWindow::updateAngles()
{
	updating = true;
	gtk_adjustment_set_value(xRotSpinAdj, tmpRotX);
	gtk_adjustment_set_value(yRotSpinAdj, tmpRotY);
	gtk_adjustment_set_value(zRotSpinAdj, tmpRotZ);
	updating = false;
}


void NavigationWindow::xScale()
{
	if(!shown || updating) {
		return;
	}
	
	double value = ((GtkAdjustment*)xScaleAdj)->value;
	
	if(maintAspectRatio->active) {
		double ratioyx = scaleY / scaleX;
		double ratiozx = scaleZ / scaleX;
		
		scaleY = ratioyx * value;
		scaleZ = ratiozx * value;
		scaleX = value;
		updateScales();
	} else {
		scaleX = value;
	}
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::yScale()
{
	if(!shown || updating) {
		return;
	}
	
	double value = yScaleAdj->value;
	
	if(maintAspectRatio->active) {
		double ratioxy = scaleX / scaleY;
		double ratiozy = scaleZ / scaleY;
		
		scaleX = ratioxy * value;
		scaleZ = ratiozy * value;
		scaleY = value;
		updateScales();
	} else {
		scaleY = value;
	}
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::zScale()
{
	if(!shown || updating) {
		return;
	}
	
	double value = zScaleAdj->value;
	
	if(maintAspectRatio->active) {
		double ratioxz = scaleX / scaleZ;
		double ratioyz = scaleY / scaleZ;
		
		scaleX = ratioxz * value;
		scaleY = ratioyz * value;
		scaleZ = value;
		updateScales();
	} else {
		scaleZ = value;
	}
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::updateScales()
{
	updating = true;
	gtk_adjustment_set_value(xScaleAdj, scaleX);
	gtk_adjustment_set_value(yScaleAdj, scaleY);
	gtk_adjustment_set_value(zScaleAdj, scaleZ);
	updating = false;
}

void NavigationWindow::xOrigin()
{
	if(!shown || updating) {
		return;
	}
	origX = xOriginAdj->value;
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::yOrigin()
{
	if(!shown || updating) {
		return;
	}
	origY = yOriginAdj->value;
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::zOrigin()
{
	if(!shown || updating) {
		return;
	}
	origZ = zOriginAdj->value;
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::updateOrigin()
{
	updating = true;
	gtk_adjustment_set_value(xOriginAdj, origX);
	gtk_adjustment_set_value(yOriginAdj, origY);
	gtk_adjustment_set_value(zOriginAdj, origZ);
	updating = false;
}

void NavigationWindow::getScriptValues()
{
	mwc->evaluateScript();

	// here we would actually need something like:
	//
	// rotX = Script::lookUp("rot_x");
	//
	// position_numeric is a highly deprecated and obsolete
        // global variable!

	tmpRotX = rotX = trimAngle(ScriptVar::position_numeric.rot_x);
	tmpRotY = rotY = trimAngle(ScriptVar::position_numeric.rot_y);
	tmpRotZ = rotZ = trimAngle(ScriptVar::position_numeric.rot_z);
	updateAngles();
	scaleX = trimScale(ScriptVar::position_numeric.scale_x);
	scaleY = trimScale(ScriptVar::position_numeric.scale_y);
	scaleZ = trimScale(ScriptVar::position_numeric.scale_z);
	updateScales();
	origX = trimOrig(ScriptVar::position_numeric.orig_x);
	origY = trimOrig(ScriptVar::position_numeric.orig_y);
	origY =	trimOrig(ScriptVar::position_numeric.orig_z);
	updateOrigin();

	mwc->drawSurfaceWithParams();
}
