/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001, 2002 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef SURFPROPS_WINDOW_H
#define SURFPROPS_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>

#include<string>

#undef ADJCB
#define ADJCB(name)\
	void on_##name##_changed(gfloat val);\
	static void _on_##name##_changed(GtkAdjustment* adj, GladeWindow* This) {\
		static_cast<SurfPropsWindow*>(This)->on_##name##_changed(adj->value);\
	}
#undef BUTCB
#define BUTCB(name)\
	void on_##name##_clicked();\
	static void _on_##name##_clicked(GtkWidget*, GladeWindow* This) {\
		static_cast<SurfPropsWindow*>(This)->on_##name##_clicked();\
	}

class ScriptWindow;

class SurfPropsWindow : public GladeWindow {
public:
	SurfPropsWindow(ScriptWindow* scriptwin);
	virtual ~SurfPropsWindow() {}

	void show();
	void hide();

private:
	ScriptWindow* scriptwin;

	GtkWidget* window;

	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<SurfPropsWindow*>(This)->hide();
		return true;
	}

	void update_material(int i);
	void reset_material(int i);
	BUTCB(update_material);
	BUTCB(reset_material);
	
	void update_trafo(int i);
	void reset_trafo(int i);
	BUTCB(update_trafo);
	BUTCB(reset_trafo);
	
	void update_light(int i);
	void reset_light(int i);
	BUTCB(update_light);
	BUTCB(reset_light);
	
	BUTCB(update_illumination);
	BUTCB(reset_illumination);
	
	BUTCB(update_perspective);
	BUTCB(reset_perspective);

	BUTCB(update_all);
	BUTCB(reset_all);

	double pi;
	double rad_to_deg(double rad) {
		return 180.0*rad/pi;
	}
	double deg_to_rad(double deg) {
		return pi*deg/180.0;
	}
};

#endif //!SURFPROPS_WINDOW_H
