/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef DITHER_WINDOW_H
#define DITHER_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>
#include <Kernel.h>

#include<string>

#undef MENUCB
#define MENUCB(name) \
        void on_##name##_activate(GtkWidget* w = 0);\
        static void _on_##name##_activate(GtkWidget* w, GladeWindow* This) {\
                static_cast<DitherWindow*>(This)->on_##name##_activate(w);\
        }

class DitherWindow : public GladeWindow {
public:
	DitherWindow(Glade& glade, Kernel& kernel);
	virtual ~DitherWindow() {}

	void show() {
		gtk_widget_show(window);
		gdk_window_raise(window->window);
	}
	void hide() {
		gtk_widget_hide(window);
	}
		
	void set_image(guint8* pixdata, int width, int height);

private:
	Glade& glade;
	Kernel& kernel;

	GdkGCValues gcval;
	GdkBitmap* bitmap;

	std::string filename;
	std::string filetype;

	// Gtk stuff follows:
	// ==================

	GtkWidget* window;
	GtkWidget* drawingarea;
	GtkWidget* popupmenu;

	void set_title();

	MENUCB(save);
	MENUCB(save_as);
	MENUCB(close);

	MENUCB(filetype);

	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<DitherWindow*>(This)->on_close_activate();
		return true;
	}
	void on_key_press_event(GdkEventKey* e);
	static int _on_key_press_event(GtkWidget*, GdkEvent* e, GladeWindow* This) {
		static_cast<DitherWindow*>(This)->on_key_press_event(reinterpret_cast<GdkEventKey*>(e));
		return true;
	}
	void on_expose_event(GdkEventExpose* event);
	static int _on_expose_event(GtkWidget*, GdkEvent*e, GladeWindow* This) {
		static_cast<DitherWindow*>(This)->on_expose_event(reinterpret_cast<GdkEventExpose*>(e));
		return false;
	}
	void on_button_press_event(GdkEventButton* event);
	static int _on_button_press_event(GtkWidget*, GdkEvent*e, GladeWindow* This) {
		static_cast<DitherWindow*>(This)->on_button_press_event(reinterpret_cast<GdkEventButton*>(e));
		return false;
	}
};

#endif //!DITHER_WINDOW_H
