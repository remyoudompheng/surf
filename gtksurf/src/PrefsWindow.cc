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

#include <PrefsWindow.h>
#include <ScriptWindow.h>
#include <Misc.h>

#include<iostream>
#include<strstream>

PrefsWindow::PrefsWindow(Glade& _glade, ScriptWindow* _scriptwin)
	: glade(_glade),
	  scriptwin(_scriptwin),
	  toolbar(GTK_TOOLBAR_ICONS), remember_pos(false),
	  shown(false)
{
	filename.assign(getenv("HOME"));
	filename += "/.gtksurf";
	read_prefs();

	window = glade.get_widget("window_prefs");
	glade.sig_connect(window, "delete_event", _on_delete_event, this);
	glade.sig_connect("button_prefs_ok", "clicked", _on_okay_clicked, this);
	glade.sig_connect("button_prefs_cancel", "clicked", _on_cancel_clicked, this);
	radiobutton_default_font = GTK_TOGGLE_BUTTON(glade.get_widget("radiobutton_default_font"));
	radiobutton_custom_font = GTK_TOGGLE_BUTTON(glade.get_widget("radiobutton_custom_font"));
	glade.sig_connect(GTK_WIDGET(radiobutton_default_font), "toggled", _on_font_toggled, this);
	radiobutton_icons = GTK_TOGGLE_BUTTON(glade.get_widget("radiobutton_icons"));
	radiobutton_text = GTK_TOGGLE_BUTTON(glade.get_widget("radiobutton_text"));
	radiobutton_both = GTK_TOGGLE_BUTTON(glade.get_widget("radiobutton_both"));
	hbox_font = glade.get_widget("hbox_font");
	entry_font = GTK_ENTRY(glade.get_widget("entry_font"));
	glade.sig_connect("button_font", "clicked", _on_font_clicked, this);
	checkbutton_pos = GTK_TOGGLE_BUTTON(glade.get_widget("checkbutton_pos"));
}

PrefsWindow::~PrefsWindow()
{
	save_prefs();
}

void PrefsWindow::read_prefs()
{
	std::ifstream f(filename.c_str());
	if(!f) {
		return;
	}
	
	std::string l;
	while(std::getline(f, l)) {
		std::string fontstr = "font=";
		std::string toolbarstr = "toolbar=";
		std::string posstr = "geometry=";
		if(l.substr(0, fontstr.length()) == fontstr) {
			font.assign(l.substr(fontstr.length()));
			scriptwin->set_font(font);
		} else if(l.substr(0, toolbarstr.length()) == toolbarstr) {
			std::string value = l.substr(toolbarstr.length());
			if(value == "text") {
				toolbar = GTK_TOOLBAR_TEXT;
			} else if(value == "both") {
				toolbar = GTK_TOOLBAR_BOTH;
			} else {
				toolbar = GTK_TOOLBAR_ICONS;
			}
			scriptwin->set_toolbar_style(toolbar);
		} else if(l.substr(0, posstr.length()) == posstr) {
			remember_pos = true;
			std::string value = l.substr(posstr.length());
			std::istrstream iss(value.c_str());
			int xpos, ypos, width, height;
			iss >> xpos >> ypos >> width >> height;
			scriptwin->set_geometry(xpos, ypos, width, height);
		}
	}
}

void PrefsWindow::save_prefs()
{
	std::ofstream f(filename.c_str(), ios::out | ios::trunc);
	if(!f) {
		std::string s = "Couldn't open/create preferences file \'";
		s += filename + "\'.\n";
		Misc::print_warning(s);
		return;
	}

	if(font.length() > 0) {
		f << "font=" << font << '\n';
	}

	f << "toolbar=";
	switch(toolbar) {
	case GTK_TOOLBAR_ICONS:
		f << "icons\n";
		break;
	case GTK_TOOLBAR_TEXT:
		f << "text\n";
		break;
	case GTK_TOOLBAR_BOTH:
		f << "both\n";
		break;
	}

	if(remember_pos) {
		int xpos, ypos, width, height;
		scriptwin->get_geometry(xpos, ypos, width, height);
		f << "geometry="
		  << xpos << ' ' << ypos << ' '
		  << width << ' ' << height << '\n';
	}
}

void PrefsWindow::show()
{
	if(!shown) {
		shown = true;
		
		gtk_entry_set_text(entry_font, font.c_str());
		if(font.length() > 0) {
			gtk_toggle_button_set_active(radiobutton_custom_font, true);
		} else {
			gtk_toggle_button_set_active(radiobutton_default_font, true);
		}
		
		if(toolbar == GTK_TOOLBAR_ICONS) {
			gtk_toggle_button_set_active(radiobutton_icons, true);
		} else if(toolbar == GTK_TOOLBAR_TEXT) {
			gtk_toggle_button_set_active(radiobutton_text, true);
		} else {
			gtk_toggle_button_set_active(radiobutton_both, true);
		}

		gtk_toggle_button_set_active(checkbutton_pos, remember_pos);
	}
	gtk_widget_show(window);
	gdk_window_raise(window->window);
	
	gtk_widget_grab_focus(glade.get_widget("button_prefs_ok"));
}

void PrefsWindow::on_okay_clicked()
{
	if(gtk_toggle_button_get_active(radiobutton_icons)) {
		toolbar = GTK_TOOLBAR_ICONS;
	} else if(gtk_toggle_button_get_active(radiobutton_text)) {
		toolbar = GTK_TOOLBAR_TEXT;
	} else {
		toolbar = GTK_TOOLBAR_BOTH;
	}

	if(gtk_toggle_button_get_active(radiobutton_default_font)) {
		font.assign("");
	} else {
		gchar* f = gtk_editable_get_chars(GTK_EDITABLE(entry_font), 0, -1);
		font.assign(f);
		g_free(f);
	}

	remember_pos = gtk_toggle_button_get_active(checkbutton_pos);
	
	hide();

	scriptwin->set_font(font);
	scriptwin->set_toolbar_style(toolbar);
}

void PrefsWindow::on_cancel_clicked()
{
	hide();
}

void PrefsWindow::on_font_toggled()
{
	if(gtk_toggle_button_get_active(radiobutton_default_font)) {
		gtk_widget_set_sensitive(hbox_font, false);
	} else {
		gtk_widget_set_sensitive(hbox_font, true);
	}
		
}

void PrefsWindow::on_font_clicked()
{
	if(glade.fontselect()) {
		gtk_entry_set_text(entry_font, glade.get_fontname().c_str());
	}
}
