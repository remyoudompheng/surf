/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>
#include <PrefsWindow.h>
#include <ImageWindow.h>
#include <CurvePropsWindow.h>
#include <SurfPropsWindow.h>
//#include <NavigationWindow.h>

#include<string>

#undef MENUCB
#define MENUCB(name) \
        void on_##name##_activate();\
        static void _on_##name##_activate(GtkWidget*, GladeWindow* This) {\
                static_cast<ScriptWindow*>(This)->on_##name##_activate();\
        }

class ScriptWindow : public GladeWindow {
public:
	ScriptWindow();
	virtual ~ScriptWindow() {}

	void load_file(const std::string& fname);

	void set_status(const std::string& txt);
	void progress_mode(bool mode) const;
	void set_progress(gfloat percentage);

	void select_region(int from, int to);

	void insert(const std::string& str);

	void get_geometry(int& xpos, int& ypos, int& width, int& height);
	void set_geometry(int xpos, int ypos, int width, int height);
	void set_font(const std::string& font);
	void set_toolbar_style(GtkToolbarStyle style) {
		gtk_toolbar_set_style(GTK_TOOLBAR(Glade::get_widget("toolbar")),
				      prefswin.getToolbarStyle());
	}

private:
	PrefsWindow prefswin;
	ImageWindow imagewin;
	CurvePropsWindow curvepropswin;
	SurfPropsWindow surfpropswin;
//	NavigationWindow navigationwin;

	bool dirty; // was script changed since last save?
	std::string filename;

	size_t prelude_length;

	// Gtk stuff follows:
	// ==================

	GtkWidget* window;
	GtkWidget* text_script;

	GtkStatusbar* sbar; // statusbar widget
	guint sbar_context;

	GtkWidget* pbar;

	void set_title();

	MENUCB(new);
	MENUCB(open);
	MENUCB(save);
	MENUCB(save_as);
        MENUCB(prefs);
	MENUCB(quit);
	MENUCB(cut);
	MENUCB(copy);
	MENUCB(paste);
	MENUCB(clear);
	MENUCB(select_all);
//	MENUCB(navigation);
	MENUCB(render_curve);
	MENUCB(render_surface);
	MENUCB(execute);
	MENUCB(curve_properties);
        MENUCB(surface_properties);
	MENUCB(about);

	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<ScriptWindow*>(This)->on_quit_activate();
		return true;
	}

	static void _on_text_script_changed(GtkWidget*, GladeWindow* This) {
		static_cast<ScriptWindow*>(This)->dirty = true;
	}

	void on_stop_clicked();
	static void _on_stop_clicked(GtkWidget*, GladeWindow* This) {
		static_cast<ScriptWindow*>(This)->on_stop_clicked();
	}
};

#endif //!SCRIPTWINDOW_H
