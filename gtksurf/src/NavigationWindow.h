/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef NAVIGATION_WINDOW_H
#define NAVIGATION_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>
#include <GLArea.h>

#include<string>

#undef ADJCBB
#define ADJCB(name)\
	void on_##name##_changed(gfloat val);\
	static void _on_##name##_changed(GtkAdjustment* adj, GladeWindow* This) {\
		static_cast<NavigationWindow*>(This)->on_##name##_changed(adj->value);\
	}

#undef MENUCB
#define MENUCB(name) \
        void on_##name##_activate(GtkWidget* w = 0);\
        static void _on_##name##_activate(GtkWidget* w, GladeWindow* This) {\
                static_cast<NavigationWindow*>(This)->on_##name##_activate(w);\
        }

class ScriptWindow;

class NavigationWindow : public GladeWindow {
public:
	NavigationWindow(ScriptWindow* scriptwin);
	virtual ~NavigationWindow() {}

	void show();
	void hide();

	void set_orig(gfloat x, gfloat y, gfloat z);
	void set_scale(gfloat x, gfloat y, gfloat z);
	void set_rot(gfloat x, gfloat y, gfloat z);

	void on_button_press_event(GdkEventButton* event = 0);
	
private:
	ScriptWindow* scriptwin;

	GLArea glarea;

	double pi;

	gfloat origx, origy, origz;
	gfloat scalex, scaley, scalez;
	bool keep_scaleratio;
	gfloat rotx, roty, rotz;
	bool updatingRot;

	std::string filename;
	std::string filetype;

	double rad_to_deg(double rad) {
		return 180.0*rad/pi;
	}
	double deg_to_rad(double deg) {
		return pi*deg/180.0;
	}
	
	// Gtk stuff follows:
	// ==================

	GtkWidget* window;
	GtkSpinButton* sp_origx;
	GtkSpinButton* sp_origy;
	GtkSpinButton* sp_origz;
	GtkSpinButton* sp_scalex;
	GtkSpinButton* sp_scaley;
	GtkSpinButton* sp_scalez;
	GtkSpinButton* sp_rotx;
	GtkSpinButton* sp_roty;
	GtkSpinButton* sp_rotz;
	GtkWidget* popupmenu;
	
	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<NavigationWindow*>(This)->hide();
		return true;
	}

	ADJCB(origx);
	ADJCB(origy);
	ADJCB(origz);
	ADJCB(scalex);
	ADJCB(scaley);
	ADJCB(scalez);
	ADJCB(rotx);
	ADJCB(roty);
	ADJCB(rotz);

	void on_scaleratio_toggled(bool value) {
		keep_scaleratio = value;
	}
	static void _on_scaleratio_toggled(GtkToggleButton* tb, GladeWindow* This) {
		static_cast<NavigationWindow*>(This)->on_scaleratio_toggled(gtk_toggle_button_get_active(tb));
	}

	void on_update_clicked();
	static void _on_update_clicked(GtkWidget*, GladeWindow* This) {
		static_cast<NavigationWindow*>(This)->on_update_clicked();
	}
	void on_reset_clicked();
	static void _on_reset_clicked(GtkWidget*, GladeWindow* This) {
		static_cast<NavigationWindow*>(This)->on_reset_clicked();
	}

	static gint _on_button_press_event(GtkWidget* widget, GdkEventButton* event, NavigationWindow* _this) {
		_this->on_button_press_event(event);
		return true;
	}

	MENUCB(togglecross);
	MENUCB(togglewireframe);
	MENUCB(perspective);
	MENUCB(save);
	MENUCB(save_as);
	MENUCB(close);
	
	MENUCB(filetype);
};

#endif //!NAVIGATION_WINDOW_H
