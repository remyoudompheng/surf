/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001, 2002 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef CURVEPROPS_WINDOW_H
#define CURVEPROPS_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Glade.h>

#include<string>

#undef ADJCB
#define ADJCB(name)\
	void on_##name##_changed(gfloat val);\
	static void _on_##name##_changed(GtkAdjustment* adj, GladeWindow* This) {\
		static_cast<CurvePropsWindow*>(This)->on_##name##_changed(adj->value);\
	}
#undef BUTCB
#define BUTCB(name)\
	void on_##name##_clicked();\
	static void _on_##name##_clicked(GtkWidget*, GladeWindow* This) {\
		static_cast<CurvePropsWindow*>(This)->on_##name##_clicked();\
	}

class ScriptWindow;

class CurvePropsWindow : public GladeWindow {
public:
	CurvePropsWindow(ScriptWindow* scriptwin);
	virtual ~CurvePropsWindow() {}

	void show();
	void hide();

	void set_bgcol(int r, int g, int b);
	void set_curvecol(int r, int g, int b);
	void set_orig(gfloat x, gfloat y, gfloat z);
	void set_scale(gfloat x, gfloat y, gfloat z);
	void set_rot(gfloat z);
	void set_clip(bool clip, gfloat radius);
	void set_app(double w, double g);

private:
	ScriptWindow* scriptwin;

	// Gtk stuff follows:
	// ==================

	GtkWidget* window;

	GtkColorSelection* cs_background;
	GtkColorSelection* cs_curve;
	int bgcol[3], curvecol[3];

	GtkSpinButton* sp_origx;
	GtkSpinButton* sp_origy;
	GtkSpinButton* sp_origz;
	GtkSpinButton* sp_scalex;
	GtkSpinButton* sp_scaley;
	GtkSpinButton* sp_scalez;
	GtkSpinButton* sp_rotz;

	gfloat origx, origy, origz;
	gfloat scalex, scaley, scalez;
	gfloat rotz;

	GtkToggleButton* tb_clip;
	GtkSpinButton* sp_radius;
	bool clip;
	gfloat radius;

	GtkSpinButton* sp_width;
	GtkSpinButton* sp_gamma;
	gfloat width, gamma;
	
	static int _on_delete_event(GtkWidget*, GdkEvent*, GladeWindow* This) {
		static_cast<CurvePropsWindow*>(This)->hide();
		return true;
	}

	void on_backgroundcolor_changed(gdouble col[4]);
	static void _on_backgroundcolor_changed(GtkColorSelection* cs, GladeWindow* This) {
		gdouble col[4];
		gtk_color_selection_get_color(cs, col);
		static_cast<CurvePropsWindow*>(This)->on_backgroundcolor_changed(col);
	}
	BUTCB(update_bgcol);
	BUTCB(reset_bgcol);
	void on_curvecolor_changed(gdouble col[4]);
	static void _on_curvecolor_changed(GtkColorSelection* cs, GladeWindow* This) {
		gdouble col[4];
		gtk_color_selection_get_color(cs, col);
		static_cast<CurvePropsWindow*>(This)->on_curvecolor_changed(col);
	}
	BUTCB(update_curvecol);
	BUTCB(reset_curvecol);
	

	ADJCB(origx);
	ADJCB(origy);
	ADJCB(origz);
	BUTCB(update_curveorig);
	BUTCB(reset_curveorig);
	ADJCB(scalex);
	ADJCB(scaley);
	ADJCB(scalez);
	BUTCB(update_curvescale);
	BUTCB(reset_curvescale);
	ADJCB(rotz);
	BUTCB(update_curverot);
	BUTCB(reset_curverot);

	void on_clip_toggled(bool value);
	static void _on_clip_toggled(GtkToggleButton* tb, GladeWindow* This) {
		static_cast<CurvePropsWindow*>(This)->on_clip_toggled(gtk_toggle_button_get_active(tb));
	}
	ADJCB(radius);
	BUTCB(update_clip);
	BUTCB(reset_clip);
	ADJCB(width);
	ADJCB(gamma);
	BUTCB(update_app);
	BUTCB(reset_app);

	BUTCB(update_curve);
	BUTCB(reset_curve);

	double pi;
	double rad_to_deg(double rad) {
		return 180.0*rad/pi;
	}
	double deg_to_rad(double deg) {
		return pi*deg/180.0;
	}
	
};

#endif //!CURVEPROPS_WINDOW_H
