/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef ABOUT_H
#define ABOUT_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>

class About : public GladeWindow {
public:
	About(const Glade& _glade);
	virtual ~About();

	void show();

private:
	const Glade& glade;
	GtkWidget* win;
	GtkStyle* style;
	GMainLoop* gmainloop;

	void hide() {
		g_main_quit(gmainloop);
	}
	static int _on_button_press_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		((About*)(This))->hide();
		return true;
	}
	static int _on_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* This) {
		((About*)(This))->hide();
		return true;
	}
};

#endif //!ABOUT_H
