/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <NavigationWindow.h>
#include <Misc.h>

#include <gtkgl/gtkglarea.h>
#include <stdio.h>

#include<iostream>
#include<cmath>

NavigationWindow::NavigationWindow(Glade& _glade, Kernel& _kernel)
	: glade(_glade), kernel(_kernel), glarea(glade, kernel, this),
	  origx(0.0), origy(0.0), origz(0.0),
	  scalex(1.0), scaley(1.0), scalez(1.0), keep_scaleratio(true),
	  rotx(0.0), roty(0.0), rotz(0.0), updatingRot(false)
{
	pi = std::acos(-1);
	
	window = glade.get_widget("window_navigation");
	gtk_box_pack_start_defaults(GTK_BOX(glade.get_widget("vbox_glarea")),
				    glarea.getWidget());
	glade.sig_connect(window, "delete_event", _on_delete_event, this);

	sp_origx = glade.spin_connect("spinbutton_origx", _on_origx_changed, this);
	sp_origy = glade.spin_connect("spinbutton_origy", _on_origy_changed, this);
	sp_origz = glade.spin_connect("spinbutton_origz", _on_origz_changed, this);
	sp_scalex = glade.spin_connect("spinbutton_scalex", _on_scalex_changed, this);
	sp_scaley = glade.spin_connect("spinbutton_scaley", _on_scaley_changed, this);
	sp_scalez = glade.spin_connect("spinbutton_scalez", _on_scalez_changed, this);
	sp_rotx = glade.spin_connect("spinbutton_rotx", _on_rotx_changed, this);
	sp_roty = glade.spin_connect("spinbutton_roty", _on_roty_changed, this);
	sp_rotz = glade.spin_connect("spinbutton_rotz", _on_rotz_changed, this);

	GtkToggleButton* w = reinterpret_cast<GtkToggleButton*>(glade.get_widget("togglebutton_scaleratio"));
	glade.toggle_connect(w, _on_scaleratio_toggled, this);
	gtk_toggle_button_set_active(w, true); // libglade bug?

	glade.sig_connect("button_reset", "clicked", _on_reset_clicked, this);
}

void NavigationWindow::show()
{
	gtk_widget_show(window);
	gdk_window_raise(window->window);
	glarea.show();
//	kernel.send("triangulate_surface;\n");
}

void NavigationWindow::hide()
{
	glarea.hide();
	gtk_widget_hide(window);
}

void NavigationWindow::set_orig(gfloat x, gfloat y, gfloat z)
{
	gtk_spin_button_set_value(sp_origx, x);
	gtk_spin_button_set_value(sp_origy, y);
	gtk_spin_button_set_value(sp_origz, z);
}

void NavigationWindow::set_scale(gfloat x, gfloat y, gfloat z)
{
	gtk_spin_button_set_value(sp_scalex, x);
	gtk_spin_button_set_value(sp_scaley, y);
	gtk_spin_button_set_value(sp_scalez, z);
}

void NavigationWindow::set_rot(gfloat x, gfloat y, gfloat z)
{
	updatingRot = true;
	gtk_spin_button_set_value(sp_rotx, x);
	gtk_spin_button_set_value(sp_roty, y);
	gtk_spin_button_set_value(sp_rotz, z);
	updatingRot = false;
}


// Gtk callbacks:
// ======================================================================

void NavigationWindow::on_button_press_event(GdkEventButton* event)
{
	if(event->button == 1) {
		std::cerr << "button 1 pressed\n";
	}
}

void NavigationWindow::on_origx_changed(gfloat val)
{
	origx = val;
	glarea.set_orig(origx, origy, origz);
}
void NavigationWindow::on_origy_changed(gfloat val)
{
	origy = val;
	glarea.set_orig(origx, origy, origz);
}
void NavigationWindow::on_origz_changed(gfloat val)
{
	origz = val;
	glarea.set_orig(origx, origy, origz);
}

void NavigationWindow::on_scalex_changed(gfloat val)
{
	if(!keep_scaleratio) {
		scalex = val;
	} else {
		gfloat ratio = val/scalex;
		scalex = val;
		scaley *= ratio;
		scalez *= ratio;
		set_scale(scalex, scaley, scalez);
	}
	glarea.set_scale(scalex, scaley, scalez);
}
void NavigationWindow::on_scaley_changed(gfloat val)
{
	if(!keep_scaleratio) {
		scaley = val;
	} else {
		gfloat ratio = val/scaley;
		scaley = val;
		scalex *= ratio;
		scalez *= ratio;
		set_scale(scalex, scaley, scalez);
	}
	glarea.set_scale(scalex, scaley, scalez);
}
void NavigationWindow::on_scalez_changed(gfloat val)
{
	if(!keep_scaleratio) {
		scalez = val;
	} else {
		gfloat ratio = val/scalez;
		scalez = val;
		scalex *= ratio;
		scaley *= ratio;
		set_scale(scalex, scaley, scalez);
	}
	glarea.set_scale(scalex, scaley, scalez);
}

void NavigationWindow::on_rotx_changed(gfloat val)
{
	rotx = val;
	if(updatingRot) {
		return;
	}
	glarea.set_rot(rotx, roty, rotz);
}
void NavigationWindow::on_roty_changed(gfloat val)
{
	roty = val;
	if(updatingRot) {
		return;
	}
	glarea.set_rot(rotx, roty, rotz);
}
void NavigationWindow::on_rotz_changed(gfloat val)
{
	rotz = val;
	if(updatingRot) {
		return;
	}
	glarea.set_rot(rotx, roty, rotz);
}


void NavigationWindow::on_reset_clicked()
{
	kernel.update_position();

	double x, y, z;

	kernel.get_orig(x, y, z);
	set_orig(x, y, z);

	kernel.get_rotation(x, y, z);
	x = rad_to_deg(x);
	y = rad_to_deg(y);
	z = rad_to_deg(z);
	set_rot(x, y, z);
	glarea.set_rot(x, y, z); // because of the updatingRot stuff

	kernel.get_scale(x, y, z);
	set_scale(x, y, z);
}
