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
#include <cmath>

#define GTK_NONE ((GtkAttachOptions)0)

NavigationWindow::NavigationWindow(MainWindowController* winctrl)
	: mwc (winctrl), shown(false),
	  rotationUpdating(false), scaleUpdating(false), originUpdating(false)
{
	GtkWidget* notebook;
	GtkWidget* vbox;
	GtkWidget* widget;
	GtkWidget* table;
	GtkWidget* frame;
	GtkWidget* hbox;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
	gtk_signal_connect(GTK_OBJECT (window), "delete_event",
			   GTK_SIGNAL_FUNC (handle_delete), this);
	gtk_window_set_title(GTK_WINDOW (window), PACKAGE " Navigation Window");
	gtk_container_border_width(GTK_CONTAINER (window), 5);
	gtk_widget_set_usize(window, 320, 0);
	
	notebook = gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(window), notebook);


	// "Rotation" tab:
	// ---------------

	vbox = gtk_vbox_new(false, 5);
	gtk_container_border_width(GTK_CONTAINER(vbox), 4);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox,
				 gtk_label_new("Rotation"));

	frame = gtk_frame_new("relative:");
	gtk_box_pack_start(GTK_BOX(vbox), frame, false, false, 0);
	gtk_container_border_width(GTK_CONTAINER(frame), 4);

	table = gtk_table_new(3, 3, false);
	gtk_container_add(GTK_CONTAINER(frame), table);
	gtk_container_border_width(GTK_CONTAINER(table), 4);

	widget = gtk_label_new("X:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 0, 1, GTK_NONE, GTK_NONE, 0, 0);
	widget = gtk_button_new_with_label("-");
	VOIDCONNECT(widget, "clicked", xDec);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 0, 1);
	widget = gtk_button_new_with_label("+");
	VOIDCONNECT(widget, "clicked", xInc);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 2, 3, 0, 1);

	widget = gtk_label_new("Y:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 1, 2, GTK_NONE, GTK_NONE, 0, 0);
	widget = gtk_button_new_with_label ("-");
	VOIDCONNECT (widget, "clicked", yDec);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 1, 2);
	widget = gtk_button_new_with_label("+");
	VOIDCONNECT (widget, "clicked", yInc);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 2, 3, 1, 2);

	widget = gtk_label_new("Z:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 2, 3, GTK_NONE, GTK_NONE, 0, 0);
	widget = gtk_button_new_with_label("-");
	VOIDCONNECT(widget, "clicked", zDec);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 2, 3);
	widget = gtk_button_new_with_label("+");
	VOIDCONNECT(widget, "clicked", zInc);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 2, 3, 2, 3);

	frame = gtk_frame_new("absolute:");
	gtk_box_pack_start(GTK_BOX(vbox), frame, false, false, 0);
	gtk_container_border_width(GTK_CONTAINER(frame), 4);

	hbox = gtk_hbox_new(false, 5);
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_border_width(GTK_CONTAINER(hbox), 4);

	widget = gtk_label_new("X:");
	gtk_box_pack_start(GTK_BOX(hbox), widget, false, false, 0);
	xRotSpinAdj = gtk_adjustment_new(0, -180, 180, 1, 10, 0);
	VOIDCONNECT(xRotSpinAdj, "value_changed", xRotSpin);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(xRotSpinAdj), 0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), widget, true, true, 0);
	wrw.addWidget(widget, "rot_x");

	widget = gtk_label_new("Y:");
	gtk_box_pack_start(GTK_BOX(hbox), widget, false, false, 0);
	yRotSpinAdj = gtk_adjustment_new(0, -180, 180, 1, 10, 0);
	VOIDCONNECT(yRotSpinAdj, "value_changed", yRotSpin);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(yRotSpinAdj), 0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), widget, true, true, 0);
	wrw.addWidget(widget, "rot_y");

	widget = gtk_label_new("Z:");
	gtk_box_pack_start(GTK_BOX(hbox), widget, false, false, 0);
	zRotSpinAdj = gtk_adjustment_new(0, -180, 180, 1, 10, 0);
	VOIDCONNECT(zRotSpinAdj, "value_changed", zRotSpin);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(zRotSpinAdj), 0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), widget, true, true, 0);
	wrw.addWidget(widget, "rot_z");

	hbox = gtk_hbox_new(false, 5);
	gtk_box_pack_end(GTK_BOX(vbox), hbox, false, false, 0);
	widget = gtk_button_new_with_label("reset");
	VOIDCONNECT(widget, "clicked", rotationReset);
	gtk_box_pack_end(GTK_BOX(hbox), widget, false, false, 0);
	
	
	// "Scale" tab
	// -----------

	vbox = gtk_vbox_new(false, 5);
	gtk_container_border_width(GTK_CONTAINER(vbox), 4);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox,
				 gtk_label_new("Scale"));
	
	table = gtk_table_new(3, 2, false);
	gtk_box_pack_start(GTK_BOX(vbox), table, false, false, 0);

	widget = gtk_label_new("X:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 0, 1, GTK_NONE, GTK_NONE, 0, 0);
	xScaleAdj = gtk_adjustment_new(1, 0.001, 100, 0.1, 1, 0);
	VOIDCONNECT(xScaleAdj, "value_changed", xScale);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(xScaleAdj), 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 0, 1);
	wrw.addWidget(widget, "scale_x");

	widget = gtk_label_new("Y:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 1, 2, GTK_NONE, GTK_NONE, 0, 0);
	yScaleAdj = gtk_adjustment_new(1, 0.001, 100, 0.1, 1, 0);
	VOIDCONNECT(yScaleAdj, "value_changed", yScale);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(yScaleAdj), 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 1, 2);
	wrw.addWidget(widget, "scale_y");

	widget = gtk_label_new("Z:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 2, 3, GTK_NONE, GTK_NONE, 0, 0);
	zScaleAdj = gtk_adjustment_new(1, 0.001, 100, 0.1, 1, 0);
	VOIDCONNECT(zScaleAdj, "value_changed", zScale);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(zScaleAdj), 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 2, 3);
	wrw.addWidget(widget, "scale_z");

	maintAspectRatio = gtk_check_button_new_with_label("maintain aspect ratio");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(maintAspectRatio), true);
	gtk_box_pack_start(GTK_BOX(vbox), maintAspectRatio, false, false, 0);

	hbox = gtk_hbox_new(false, 5);
	gtk_box_pack_end(GTK_BOX(vbox), hbox, false, false, 0);
	widget = gtk_button_new_with_label("reset");
	VOIDCONNECT(widget, "clicked", scaleReset);
	gtk_box_pack_end(GTK_BOX(hbox), widget, false, false, 0);


	// "Origin" tab
	// ------------

	vbox = gtk_vbox_new(false, 5);
	gtk_container_border_width(GTK_CONTAINER(vbox), 4);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox,
				 gtk_label_new("Origin"));
	
	table = gtk_table_new(3, 2, false);
	gtk_box_pack_start(GTK_BOX(vbox), table, false, false, 0);

	widget = gtk_label_new("X:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 0, 1, GTK_NONE, GTK_NONE, 0, 0);
	xOriginAdj = gtk_adjustment_new(0, -100, 100, 1, 10, 0);
	VOIDCONNECT(xOriginAdj, "value_changed", xOrigin);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(xOriginAdj), 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 0, 1);
	wrw.addWidget(widget, "origin_x");

	widget = gtk_label_new("Y:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 1, 2, GTK_NONE, GTK_NONE, 0, 0);
	yOriginAdj = gtk_adjustment_new(0, -100, 100, 1, 10, 0);
	VOIDCONNECT(yOriginAdj, "value_changed", yOrigin);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(yOriginAdj), 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 1, 2);
	wrw.addWidget(widget, "origin_y");

	widget = gtk_label_new("Z:");
	gtk_table_attach(GTK_TABLE(table), widget, 0, 1, 2, 3, GTK_NONE, GTK_NONE, 0, 0);
	zOriginAdj = gtk_adjustment_new(0, -100, 100, 1, 10, 0);
	VOIDCONNECT(zOriginAdj, "value_changed", zOrigin);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(zOriginAdj), 0, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), widget, 1, 2, 2, 3);
	wrw.addWidget(widget, "origin_z");

	hbox = gtk_hbox_new(false, 5);
	gtk_box_pack_end(GTK_BOX(vbox), hbox, false, false, 0);
	widget = gtk_button_new_with_label("reset");
	VOIDCONNECT(widget, "clicked", originReset);
	gtk_box_pack_end(GTK_BOX(hbox), widget, false, false, 0);
}


void NavigationWindow::show()
{
	updateAngles();
	updateScales();
	updateOrigins();
	gtk_widget_show_all(window);
	gdk_window_raise(window->window);
	shown = true;
}


gint NavigationWindow::handle_delete(GtkWidget* w, GdkEvent* e, gpointer d)
{
	NavigationWindow *rw = (NavigationWindow*)d;
	gtk_widget_hide(rw->window);
	rw->shown = false;
	return true;
}

void NavigationWindow::xDec()
{
	rotateX(-0.1);
}

void NavigationWindow::xInc()
{
	rotateX(0.1);
}

void NavigationWindow::yDec()
{
	rotateY(-0.1);
}

void NavigationWindow::yInc()
{
	rotateY(0.1);
}

void NavigationWindow::zDec()
{
	rotateZ(-0.1);
}

void NavigationWindow::zInc()
{
	rotateZ(0.1);
}

void NavigationWindow::xRotSpin()
{
	if(shown && !rotationUpdating) {
		ScriptVar::position_numeric.rot_x = ((GtkAdjustment*)xRotSpinAdj)->value;
		mwc->drawSurfaceWithParams();
	}
}

void NavigationWindow::yRotSpin()
{
	if(shown && !rotationUpdating) {
		ScriptVar::position_numeric.rot_y = ((GtkAdjustment*)yRotSpinAdj)->value;
		mwc->drawSurfaceWithParams();
	}
}

void NavigationWindow::zRotSpin()
{
	if(shown && !rotationUpdating) {
		ScriptVar::position_numeric.rot_z = ((GtkAdjustment*)zRotSpinAdj)->value;
		mwc->drawSurfaceWithParams();
	}
}


void NavigationWindow::rotateX(double rad)
{
	double sinx = std::sin(rad), cosx = std::cos(rad);
	double rotXArr[] = {
		1, 0, 0, 0, cosx, sinx, 0, -sinx, cosx
	};
	Matrix33 mat(rotXArr);
	rotate(mat);
}

void NavigationWindow::rotateY(double rad)
{
	double siny = std::sin(rad), cosy = std::cos(rad);
	double rotYArr[] = {
		cosy, 0, -siny, 0, 1, 0, siny, 0, cosy
	};
	Matrix33 mat(rotYArr);
	rotate(mat);
}

void NavigationWindow::rotateZ(double rad)
{
	double sinz = std::sin(rad), cosz = std::cos(rad);
	double rotZArr[] = {
		cosz, sinz, 0, -sinz, cosz, 0, 0, 0, 1
	};
	Matrix33 mat(rotZArr);
	rotate(mat);
}

void NavigationWindow::rotationReset()
{
	ScriptVar::position_numeric.rot_x = ScriptVar::position_numeric.rot_y =
		ScriptVar::position_numeric.rot_z = 0;
	updateAngles();
	mwc->drawSurfaceWithParams();
}

inline const Matrix33 NavigationWindow::getRotMat()
{
	double cosx = std::cos(ScriptVar::position_numeric.rot_x);
	double sinx = std::sin(ScriptVar::position_numeric.rot_x);
	double cosy = std::cos(ScriptVar::position_numeric.rot_y);
	double siny = std::sin(ScriptVar::position_numeric.rot_y);
	double cosz = std::cos(ScriptVar::position_numeric.rot_z);
	double sinz = std::sin(ScriptVar::position_numeric.rot_z);
	double origRotArr[] = {
		cosy*cosz + sinx*siny*sinz, cosx*sinz, -cosz*siny+cosy*sinx*sinz,
		-cosy*sinz + cosz*sinx*siny, cosx*cosz, siny*sinz + cosy*cosz*sinx,
		cosx*siny, -sinx, cosx*cosy
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

	ScriptVar::position_numeric.rot_x = std::atan2(sin_x, cos_x);
	ScriptVar::position_numeric.rot_y = std::atan2(sin_y, cos_y);
	ScriptVar::position_numeric.rot_z = std::atan2(sin_z, cos_z);

	mwc->drawSurfaceWithParams();

	updateAngles();
}

void NavigationWindow::updateAngles()
{
	rotationUpdating = true;
	gtk_adjustment_set_value(GTK_ADJUSTMENT(xRotSpinAdj), ScriptVar::position_numeric.rot_x);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(yRotSpinAdj), ScriptVar::position_numeric.rot_y);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(zRotSpinAdj), ScriptVar::position_numeric.rot_z);
	rotationUpdating = false;
}


void NavigationWindow::xScale()
{
	if(!shown || scaleUpdating) {
		return;
	}
	
	double value = ((GtkAdjustment*)xScaleAdj)->value;
	
	if(GTK_TOGGLE_BUTTON(maintAspectRatio)->active) {
		double ratioyx = ScriptVar::position_numeric.scale_y / ScriptVar::position_numeric.scale_x;
		double ratiozx = ScriptVar::position_numeric.scale_z / ScriptVar::position_numeric.scale_x;
		
		ScriptVar::position_numeric.scale_y = ratioyx * value;
		ScriptVar::position_numeric.scale_z = ratiozx * value;
		ScriptVar::position_numeric.scale_x = value;
		updateScales();
	} else {
		ScriptVar::position_numeric.scale_x = value;
	}
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::yScale()
{
	if(!shown || scaleUpdating) {
		return;
	}
	
	double value = ((GtkAdjustment*)yScaleAdj)->value;
	
	if(GTK_TOGGLE_BUTTON(maintAspectRatio)->active) {
		double ratioxy = ScriptVar::position_numeric.scale_x / ScriptVar::position_numeric.scale_y;
		double ratiozy = ScriptVar::position_numeric.scale_z / ScriptVar::position_numeric.scale_y;
		
		ScriptVar::position_numeric.scale_x = ratioxy * value;
		ScriptVar::position_numeric.scale_z = ratiozy * value;
		ScriptVar::position_numeric.scale_y = value;
		updateScales();
	} else {
		ScriptVar::position_numeric.scale_y = value;
	}
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::zScale()
{
	if(!shown || scaleUpdating) {
		return;
	}
	
	double value = ((GtkAdjustment*)zScaleAdj)->value;
	
	if(GTK_TOGGLE_BUTTON(maintAspectRatio)->active) {
		double ratioxz = ScriptVar::position_numeric.scale_x / ScriptVar::position_numeric.scale_z;
		double ratioyz = ScriptVar::position_numeric.scale_y / ScriptVar::position_numeric.scale_z;
		
		ScriptVar::position_numeric.scale_x = ratioxz * value;
		ScriptVar::position_numeric.scale_y = ratioyz * value;
		ScriptVar::position_numeric.scale_z = value;
		updateScales();
	} else {
		ScriptVar::position_numeric.scale_z = value;
	}
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::scaleReset()
{
	ScriptVar::position_numeric.scale_x = ScriptVar::position_numeric.scale_y =
		ScriptVar::position_numeric.scale_z = 1;
	updateScales();
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::updateScales()
{
	scaleUpdating = true;
	gtk_adjustment_set_value(GTK_ADJUSTMENT(xScaleAdj), ScriptVar::position_numeric.scale_x);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(yScaleAdj), ScriptVar::position_numeric.scale_y);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(zScaleAdj), ScriptVar::position_numeric.scale_z);
	scaleUpdating = false;
}

void NavigationWindow::xOrigin()
{
	if(!shown || originUpdating) {
		return;
	}
	ScriptVar::position_numeric.orig_x = ((GtkAdjustment*)xOriginAdj)->value;
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::yOrigin()
{
	if(!shown || originUpdating) {
		return;
	}
	ScriptVar::position_numeric.orig_y = ((GtkAdjustment*)yOriginAdj)->value;
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::zOrigin()
{
	if(!shown || originUpdating) {
		return;
	}
	ScriptVar::position_numeric.orig_z = ((GtkAdjustment*)zOriginAdj)->value;
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::originReset()
{
	ScriptVar::position_numeric.orig_x = ScriptVar::position_numeric.orig_y =
		ScriptVar::position_numeric.orig_z = 0;
	updateOrigins();
	mwc->drawSurfaceWithParams();
}

void NavigationWindow::updateOrigins()
{
	originUpdating = true;
	gtk_adjustment_set_value(GTK_ADJUSTMENT(xOriginAdj), ScriptVar::position_numeric.orig_x);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(yOriginAdj), ScriptVar::position_numeric.orig_y);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(zOriginAdj), ScriptVar::position_numeric.orig_z);
	originUpdating = false;
}
