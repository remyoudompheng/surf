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

#include<iostream>

PrefsWindow::PrefsWindow(Glade& _glade, ScriptWindow* _scriptwin)
	: glade(_glade),
	  scriptwin(_scriptwin),
	  shown(false)
{
	window = glade.get_widget("window_prefs");
	glade.sig_connect(window, "delete_event", _on_delete_event, this);
	glade.sig_connect("button_prefs_ok", "clicked", _on_okay_clicked, this);
	glade.sig_connect("button_prefs_apply", "clicked", _on_apply_clicked, this);
	glade.sig_connect("button_prefs_cancel", "clicked", _on_cancel_clicked, this);
	radiobutton_default_font = glade.get_widget("radiobutton_default_font");
	glade.sig_connect(radiobutton_default_font, "toggled", _on_font_toggled, this);
	radiobutton_icons = glade.get_widget("radiobutton_icons");
	radiobutton_text = glade.get_widget("radiobutton_text");
	hbox_font = glade.get_widget("hbox_font");
}

void PrefsWindow::show()
{
	if(!shown) {
		shown = true;
	}
	gtk_widget_show(window);
	gdk_window_raise(window->window);
}

void PrefsWindow::on_okay_clicked()
{
}

void PrefsWindow::on_apply_clicked()
{
	
}

void PrefsWindow::on_cancel_clicked()
{
	hide();
}

void PrefsWindow::on_font_toggled()
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_default_font))) {
		gtk_widget_set_sensitive(hbox_font, false);
	} else {
		gtk_widget_set_sensitive(hbox_font, true);
	}
		
}
