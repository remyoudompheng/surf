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

#include <gtk/gtk.h>

#include<string>
#include<fstream>

class ScriptWindow;

class PrefsWindow : public GladeWindow {
public:
	PrefsWindow(ScriptWindow* scriptwin);
	virtual ~PrefsWindow();

	void show();
	void hide() {
		shown = false;
		gtk_widget_hide(window);
	}

	GtkToolbarStyle getToolbarStyle() const {
		return toolbar;
	}
	const std::string& getFont() const {
		return font;
	}

private:
	std::string filename;

	ScriptWindow* scriptwin;

	GtkToolbarStyle toolbar;
	std::string font;
	bool remember_pos;

	bool shown;

	void read_prefs();
	void save_prefs();

	GtkWidget* window;
	GtkToggleButton* radiobutton_default_font;
	GtkToggleButton* radiobutton_custom_font;
	GtkToggleButton* radiobutton_icons;
	GtkToggleButton* radiobutton_text;
	GtkToggleButton* radiobutton_both;
	GtkWidget* hbox_font;
	GtkEntry* entry_font;
	GtkToggleButton* checkbutton_pos;

	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_cancel_clicked();
		return true;
	}

	void on_okay_clicked();
	static void _on_okay_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_okay_clicked();
	}
	void on_cancel_clicked();
	static void _on_cancel_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_cancel_clicked();
	}

	void on_font_toggled();
	static void _on_font_toggled(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_font_toggled();
	}
	void on_font_clicked();
	static void _on_font_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<PrefsWindow*>(This)->on_font_clicked();
	}
};

#endif //!PREFS_WINDOW_H
