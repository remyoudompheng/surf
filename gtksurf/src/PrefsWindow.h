/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef PREFS_WINDOW_H
#define PREFS_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>

#include<string>

class ScriptWindow;

class PrefsWindow : public GladeWindow {
public:
	PrefsWindow(Glade& glade, ScriptWindow* scriptwin);
	virtual ~PrefsWindow() {}

	void show();
	void hide() {
		shown = false;
		gtk_widget_hide(window);
	}

private:
	Glade& glade;
	ScriptWindow* scriptwin;

	bool shown;

	GtkWidget* window;
	GtkWidget* radiobutton_default_font;
	GtkWidget* radiobutton_icons;
	GtkWidget* radiobutton_text;
	GtkWidget* hbox_font;

	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_cancel_clicked();
		return true;
	}

	void on_okay_clicked();
	static void _on_okay_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_okay_clicked();
	}
	void on_apply_clicked();
	static void _on_apply_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_apply_clicked();
	}
	void on_cancel_clicked();
	static void _on_cancel_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_cancel_clicked();
	}

	void on_font_toggled();
	static void _on_font_toggled(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_font_toggled();
	}
};

#endif //!PREFS_WINDOW_H
