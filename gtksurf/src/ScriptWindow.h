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
#include <ImageWindow.h>
#include <NavigationWindow.h>
#include <Kernel.h>

#include<string>

#undef MENUCB
#define MENUCB(name) \
        void on_##name##_activate();\
        static void _on_##name##_activate(GtkWidget*, GladeWindow* This) {\
                static_cast<ScriptWindow*>(This)->on_##name##_activate();\
        }

class ScriptWindow : public GladeWindow {
public:
	ScriptWindow(Glade& glade, Kernel& kernel);
	virtual ~ScriptWindow() {}

	void load_file(const std::string& fname);

	void set_status(const std::string& txt);
	void progress_mode(bool mode) const;
	void set_progress(gfloat percentage);
	void select_region(int from, int to);

private:
	Glade& glade;

	ImageWindow imagewin;
	NavigationWindow navigationwin;

	Kernel& kernel;

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
	MENUCB(quit);
	MENUCB(cut);
	MENUCB(copy);
	MENUCB(paste);
	MENUCB(clear);
	MENUCB(navigation);
	MENUCB(render_curve);
	MENUCB(render_surface);
	MENUCB(execute);
	MENUCB(about);

	static int _on_script_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
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