/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef IMAGE_WINDOW_H
#define IMAGE_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>
#include <DitherWindow.h>

#ifdef HAVE_LIBGDK_PIXBUF
#  include <gdk-pixbuf/gdk-pixbuf.h>
#endif

#include<string>
#include<fstream>

#undef MENUCB
#define MENUCB(name) \
        void on_##name##_activate(GtkWidget* w = 0);\
        static void _on_##name##_activate(GtkWidget* w, GladeWindow* This) {\
                static_cast<ImageWindow*>(This)->on_##name##_activate(w);\
        }

class ScriptWindow;

class ImageWindow : public GladeWindow {
public:
	ImageWindow(ScriptWindow* sw);
	virtual ~ImageWindow() {}

	void show() {
		gtk_widget_show(window);
		gdk_window_raise(window->window);
	}
	void hide() {
		gtk_widget_hide(window);
	}
		

	enum ImageMode {
		SURFACE,
		CURVE
	};
	void set_mode(ImageMode _mode) {
		mode = _mode;
	}
	
	void clear();

	void read_data();
	
private:
	ScriptWindow* scriptwin;
	DitherWindow ditherwin;

	int width;
	int height;
	int rowstride;
	char* pixdata;
#ifdef HAVE_LIBGDK_PIXBUF
	GdkPixbuf* pixbuf;
#endif

	ImageMode mode;
	
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
	MENUCB(dither);

	MENUCB(filetype);

	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<ImageWindow*>(This)->on_close_activate();
		return true;
	}
	void on_key_press_event(GdkEventKey* e);
	static int _on_key_press_event(GtkWidget*, GdkEvent* e, GladeWindow* This) {
		static_cast<ImageWindow*>(This)->on_key_press_event(reinterpret_cast<GdkEventKey*>(e));
		return true;
	}
	void on_expose_event(GdkEventExpose* event);
	static int _on_expose_event(GtkWidget*, GdkEvent*e, GladeWindow* This) {
		static_cast<ImageWindow*>(This)->on_expose_event(reinterpret_cast<GdkEventExpose*>(e));
		return false;
	}
	void on_button_press_event(GdkEventButton* event);
	static int _on_button_press_event(GtkWidget*, GdkEvent*e, GladeWindow* This) {
		static_cast<ImageWindow*>(This)->on_button_press_event(reinterpret_cast<GdkEventButton*>(e));
		return false;
	}

	static void delete_array_fn(guchar pixels[], gpointer) {
		delete [] pixels;
	}
};

#endif //!IMAGE_WINDOW_H
