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
#include <ScriptWindow.h>
#include <Misc.h>

#include <gtkgl/gtkglarea.h>
#include <stdio.h>

#include<iostream>
#include<cmath>

#ifdef HAVE_STRINGSTREAM
#  include<sstream>
#else
#  include<strstream>
#endif

NavigationWindow::NavigationWindow(ScriptWindow* _scriptwin)
	: scriptwin(_scriptwin),
	  glarea(this, scriptwin),
	  origx(0.0), origy(0.0), origz(0.0),
	  scalex(1.0), scaley(1.0), scalez(1.0), keep_scaleratio(true),
	  rotx(0.0), roty(0.0), rotz(0.0), updatingRot(false)
{
	pi = std::acos(-1.0);
	
	window = Glade::get_widget("window_navigation");
	gtk_box_pack_start_defaults(GTK_BOX(Glade::get_widget("vbox_glarea")),
				    glarea.getWidget());
	Glade::sig_connect(window, "delete_event", _on_delete_event, this);

	gtk_widget_set_events(window, GDK_BUTTON_PRESS_MASK);
	gtk_signal_connect(GTK_OBJECT(window), "button_press_event",
			   GTK_SIGNAL_FUNC(_on_button_press_event), (gpointer)this);
	
	
	sp_origx = Glade::spin_connect("spinbutton_origx", _on_origx_changed, this);
	sp_origy = Glade::spin_connect("spinbutton_origy", _on_origy_changed, this);
	sp_origz = Glade::spin_connect("spinbutton_origz", _on_origz_changed, this);
	sp_scalex = Glade::spin_connect("spinbutton_scalex", _on_scalex_changed, this);
	sp_scaley = Glade::spin_connect("spinbutton_scaley", _on_scaley_changed, this);
	sp_scalez = Glade::spin_connect("spinbutton_scalez", _on_scalez_changed, this);
	sp_rotx = Glade::spin_connect("spinbutton_rotx", _on_rotx_changed, this);
	sp_roty = Glade::spin_connect("spinbutton_roty", _on_roty_changed, this);
	sp_rotz = Glade::spin_connect("spinbutton_rotz", _on_rotz_changed, this);

	GtkToggleButton* w = reinterpret_cast<GtkToggleButton*>(Glade::get_widget("togglebutton_scaleratio"));
	Glade::toggle_connect(w, _on_scaleratio_toggled, this);
	gtk_toggle_button_set_active(w, true); // libglade bug?

	Glade::sig_connect("button_updatescript", "clicked", _on_update_clicked, this);
	Glade::sig_connect("button_reset", "clicked", _on_reset_clicked, this);

	Glade::sig_connect("togglecross", "activate", _on_togglecross_activate, this);
	Glade::sig_connect("togglewireframe", "activate", _on_togglewireframe_activate, this);
	Glade::sig_connect("central_perspective", "activate", _on_perspective_activate, this);
	Glade::sig_connect("parallel_perspective", "activate", _on_perspective_activate, this);
	Glade::sig_connect("save_3d_file", "activate", _on_save_activate, this);
	Glade::sig_connect("save_3d_file_as", "activate", _on_save_as_activate, this);
	Glade::sig_connect("close_navigation", "activate", _on_close_activate, this);
	popupmenu = Glade::get_widget("menu_navigation");
}

void NavigationWindow::show()
{
	gtk_widget_show(window);
	gdk_window_raise(window->window);
	glarea.show();
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

void NavigationWindow::on_update_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "// the following lines were inserted by gtksurf:\n"
	   << "origin_x = " << origx << ";\n"
	   << "origin_y = " << origy << ";\n"
	   << "origin_z = " << origz << ";\n"
	   << "rot_y = " << deg_to_rad(roty) << ";\n"
	   << "rot_x = " << deg_to_rad(rotx) << ";\n"
	   << "rot_z = " << deg_to_rad(rotz) << ";\n"
	   << "scale_x = " << scalex << ";\n"
	   << "scale_y = " << scaley << ";\n"
	   << "scale_z = " << scalez << ";\n"
	   << "// end of inserted text\n\n";
	scriptwin->insert(os.str());
}

void NavigationWindow::on_reset_clicked()
{
	Kernel::update_position();

	double x, y, z;

	Kernel::get_orig(x, y, z);
	set_orig(x, y, z);

	Kernel::get_rotation(x, y, z);
	x = rad_to_deg(x);
	y = rad_to_deg(y);
	z = rad_to_deg(z);
	set_rot(x, y, z);
	glarea.set_rot(x, y, z); // because of the updatingRot stuff

	Kernel::get_scale(x, y, z);
	GtkToggleButton* w = reinterpret_cast<GtkToggleButton*>(Glade::get_widget("togglebutton_scaleratio"));
	if(x == y && y == z) {
		gtk_toggle_button_set_active(w, true);
	} else {
		gtk_toggle_button_set_active(w, false);
	}
	set_scale(x, y, z);

	Kernel::Sequence a, b, c;
	Kernel::get_sequence(a, b, c);
	glarea.set_sequence(a, b, c);
}

// popup-menu callbacks:

void NavigationWindow::on_button_press_event(GdkEventButton* event = 0)
{
	if(event == 0) {
		gtk_menu_popup(GTK_MENU(popupmenu), 0, 0, 0, 0, 3, 0);
	} else if(event->button == 3) {
		gtk_menu_popup(GTK_MENU(popupmenu), 0, 0, 0, 0, 3, event->time);
	}
}

void NavigationWindow::on_togglecross_activate(GtkWidget*)
{
	glarea.toggle_cross();
}

void NavigationWindow::on_togglewireframe_activate(GtkWidget*)
{
	glarea.toggle_wireframe();
}

void NavigationWindow::on_perspective_activate(GtkWidget*)
{
	GtkCheckMenuItem* item = GTK_CHECK_MENU_ITEM(Glade::get_widget("central_perspective"));
	glarea.set_perspective(item->active);
}

void NavigationWindow::on_save_activate(GtkWidget*)
{
	if(filename.length() == 0) {
		on_save_as_activate();
		return;
	}

	std::string s = "filename = \"";
	s += filename + "\";\n";
	if(filetype.length() > 0) {
		s += "three_d_file_format = ";
		s += filetype + ";\n";
	}
	s += "save_three_d_image;\n";
	Kernel::send(s);
}

void NavigationWindow::on_save_as_activate(GtkWidget*)
{
	// construct "Save Options" frame w/file type menu:
	
	GtkWidget* menu = gtk_menu_new();

	std::list<std::string> fmts = Kernel::get_three_d_image_formats();
	std::list<std::string>::iterator i;
	for(i = fmts.begin(); i != fmts.end(); ) {
		GtkWidget* item = gtk_menu_item_new_with_label((*i).c_str());
		i++;
		gtk_object_set_user_data(GTK_OBJECT(item), const_cast<char*>((*i).c_str()));
		i++;
		gtk_menu_append(GTK_MENU(menu), item);
		Glade::sig_connect(item, "activate", _on_filetype_activate, this);
	}
	filetype.assign("auto");
	gtk_widget_show_all(menu);

	if(Glade::fileselect("Save 3D Image As", menu)) {
		filename = Glade::get_filename();
		on_save_activate();
	}
	
	gtk_widget_destroy(menu);
}

void NavigationWindow::on_filetype_activate(GtkWidget* wid)
{
	char* str = static_cast<char*>(gtk_object_get_user_data(GTK_OBJECT(wid)));
	
	if(str != 0) {
		filetype.assign(str);
	}
}

void NavigationWindow::on_close_activate(GtkWidget*)
{
	hide();
}
