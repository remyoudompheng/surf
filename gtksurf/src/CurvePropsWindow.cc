/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <CurvePropsWindow.h>
#include <ScriptWindow.h>
#include <Kernel.h>
#include <Misc.h>

#ifdef HAVE_STRINGSTREAM
#  include<sstream>
#else
#  include<strstream>
#endif

#include<cmath>

CurvePropsWindow::CurvePropsWindow(ScriptWindow* _scriptwin)
	: scriptwin(_scriptwin),
	  origx(0.0), origy(0.0), origz(0.0),
	  scalex(1.0), scaley(1.0), scalez(1.0),
	  rotz(0.0),
	  clip(false), radius(10.0)
{
	pi = std::acos(-1.0);

	for(int i = 0; i <= 2; i++) {
		bgcol[i] = 0;
		curvecol[i] = 255;
	}
	
	window = Glade::get_widget("window_curveprops");
	Glade::sig_connect(window, "delete_event", _on_delete_event, this);

	cs_background = Glade::colorsel_connect("colorselection_curvebackground", _on_backgroundcolor_changed, this);
	Glade::sig_connect("button_update_curvebgcol", "clicked", _on_update_bgcol_clicked, this);
	Glade::sig_connect("button_reset_curvebgcol", "clicked", _on_reset_bgcol_clicked, this);
	cs_curve = Glade::colorsel_connect("colorselection_curve", _on_curvecolor_changed, this);
	Glade::sig_connect("button_update_curvecol", "clicked", _on_update_curvecol_clicked, this);
	Glade::sig_connect("button_reset_curvecol", "clicked", _on_reset_curvecol_clicked, this);

	sp_origx = Glade::spin_connect("spinbutton_curve_origx", _on_origx_changed, this);
	sp_origy = Glade::spin_connect("spinbutton_curve_origy", _on_origy_changed, this);
	sp_origz = Glade::spin_connect("spinbutton_curve_origz", _on_origz_changed, this);
	Glade::sig_connect("button_update_curveorig", "clicked", _on_update_curveorig_clicked, this);
	Glade::sig_connect("button_reset_curveorig", "clicked", _on_reset_curveorig_clicked, this);
	sp_scalex = Glade::spin_connect("spinbutton_curve_scalex", _on_scalex_changed, this);
	sp_scaley = Glade::spin_connect("spinbutton_curve_scaley", _on_scaley_changed, this);
	sp_scalez = Glade::spin_connect("spinbutton_curve_scalez", _on_scalez_changed, this);
	Glade::sig_connect("button_update_curvescale", "clicked", _on_update_curvescale_clicked, this);
	Glade::sig_connect("button_reset_curvescale", "clicked", _on_reset_curvescale_clicked, this);
	sp_rotz = Glade::spin_connect("spinbutton_curve_rotz", _on_rotz_changed, this);
	Glade::sig_connect("button_update_curverot", "clicked", _on_update_curverot_clicked, this);
	Glade::sig_connect("button_reset_curverot", "clicked", _on_reset_curverot_clicked, this);
	tb_clip = reinterpret_cast<GtkToggleButton*>(Glade::get_widget("checkbutton_curve_clip"));
	Glade::toggle_connect(tb_clip, _on_clip_toggled, this);
	sp_radius = Glade::spin_connect("spinbutton_curve_radius", _on_radius_changed, this);
	Glade::sig_connect("button_update_curveclip", "clicked", _on_update_clip_clicked, this);
	Glade::sig_connect("button_reset_curveclip", "clicked", _on_reset_clip_clicked, this);
	sp_width = Glade::spin_connect("spinbutton_curve_width", _on_width_changed, this);
	sp_gamma = Glade::spin_connect("spinbutton_curve_gamma", _on_gamma_changed, this);
	Glade::sig_connect("button_update_curveapp", "clicked", _on_update_app_clicked, this);
	Glade::sig_connect("button_reset_curveapp", "clicked", _on_reset_app_clicked, this);

	Glade::sig_connect("button_update_curve", "clicked", _on_update_curve_clicked, this);
	Glade::sig_connect("button_reset_curve", "clicked", _on_reset_curve_clicked, this);
}

void CurvePropsWindow::show()
{
	gtk_widget_show(window);
	gdk_window_raise(window->window);
}

void CurvePropsWindow::hide()
{
	gtk_widget_hide(window);
}

void CurvePropsWindow::set_bgcol(int r, int g, int b)
{
	gdouble col[4];
	col[0] = r/255.0;
	col[1] = g/255.0;
	col[2] = b/255.0;
	col[3] = 1.0;
	gtk_color_selection_set_color(cs_background, col);
}
void CurvePropsWindow::set_curvecol(int r, int g, int b)
{
	gdouble col[4];
	col[0] = r/255.0;
	col[1] = g/255.0;
	col[2] = b/255.0;
	col[3] = 1.0;
	gtk_color_selection_set_color(cs_curve, col);
}

void CurvePropsWindow::set_orig(gfloat x, gfloat y, gfloat z)
{
	gtk_spin_button_set_value(sp_origx, x);
	gtk_spin_button_set_value(sp_origy, y);
	gtk_spin_button_set_value(sp_origz, z);
}

void CurvePropsWindow::set_scale(gfloat x, gfloat y, gfloat z)
{
	gtk_spin_button_set_value(sp_scalex, x);
	gtk_spin_button_set_value(sp_scaley, y);
	gtk_spin_button_set_value(sp_scalez, z);
}

void CurvePropsWindow::set_rot(gfloat z)
{
	gtk_spin_button_set_value(sp_rotz, z);
}

void CurvePropsWindow::set_clip(bool clip, gfloat radius)
{
	gtk_toggle_button_set_active(tb_clip, clip);
	gtk_spin_button_set_value(sp_radius, radius);
}

void CurvePropsWindow::set_app(double w, double g)
{
	gtk_spin_button_set_value(sp_width, w);
	gtk_spin_button_set_value(sp_gamma, g);
}


// Gtk callbacks:
// ======================================================================

void CurvePropsWindow::on_backgroundcolor_changed(gdouble col[4])
{
	for(int i = 0; i <= 2; i++) {
		bgcol[i] = (int)(col[i]*255.0);
	}
}
void CurvePropsWindow::on_update_bgcol_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "background_red = " << bgcol[0] << ";\n"
	   << "background_green = " << bgcol[1] << ";\n"
	   << "background_blue = " << bgcol[2] << ";\n";
	scriptwin->insert(os.str());
}
void CurvePropsWindow::on_reset_bgcol_clicked()
{
	int r, g, b;
	Kernel::get_background(r, g, b);
	set_bgcol(r, g, b);
}

void CurvePropsWindow::on_curvecolor_changed(gdouble col[4])
{
	for(int i = 0; i <= 2; i++) {
		curvecol[i] = (int)(col[i]*255.0);
	}
}
void CurvePropsWindow::on_update_curvecol_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "curve_red = " << curvecol[0] << ";\n"
	   << "curve_green = " << curvecol[1] << ";\n"
	   << "curve_blue = " << curvecol[2] << ";\n";
	scriptwin->insert(os.str());
}
void CurvePropsWindow::on_reset_curvecol_clicked()
{
	int r, g, b;
	Kernel::get_curve_color(r, g, b);
	set_curvecol(r, g, b);
}


void CurvePropsWindow::on_origx_changed(gfloat val)
{
	origx = val;
}
void CurvePropsWindow::on_origy_changed(gfloat val)
{
	origy = val;
}
void CurvePropsWindow::on_origz_changed(gfloat val)
{
	origz = val;
}
void CurvePropsWindow::on_update_curveorig_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "origin_x = " << origx << ";\n"
	   << "origin_y = " << origy << ";\n"
	   << "origin_z = " << origz << ";\n";
	scriptwin->insert(os.str());
}
void CurvePropsWindow::on_reset_curveorig_clicked()
{
	double x, y, z;
	Kernel::get_orig(x, y, z);
	set_orig(x, y, z);
}

void CurvePropsWindow::on_scalex_changed(gfloat val)
{
	scalex = val;
}
void CurvePropsWindow::on_scaley_changed(gfloat val)
{
	scaley = val;
}
void CurvePropsWindow::on_scalez_changed(gfloat val)
{
	scalez = val;
}
void CurvePropsWindow::on_update_curvescale_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "scale_x = " << scalex << ";\n"
	   << "scale_y = " << scaley << ";\n"
	   << "scale_z = " << scalez << ";\n";
	scriptwin->insert(os.str());
}
void CurvePropsWindow::on_reset_curvescale_clicked()
{
	double x, y, z;
	Kernel::get_scale(x, y, z);
	set_scale(x, y, z);
}

void CurvePropsWindow::on_rotz_changed(gfloat val)
{
	rotz = val;
}
void CurvePropsWindow::on_update_curverot_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "rot_z = " << deg_to_rad(rotz) << ";\n";
	scriptwin->insert(os.str());
}
void CurvePropsWindow::on_reset_curverot_clicked()
{
	double x, y, z;
	Kernel::get_rotation(x, y, z);
	z = rad_to_deg(z);
	set_rot(z);
}

void CurvePropsWindow::on_clip_toggled(bool value) {
	gtk_widget_set_sensitive(GTK_WIDGET(sp_radius), clip = value);
}
void CurvePropsWindow::on_radius_changed(gfloat val)
{
	radius = val;
}
void CurvePropsWindow::on_update_clip_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "clip = " << (clip ? "sphere" : "none") << ";\n"
	   << "clip_radius = " << radius << ";\n";
	scriptwin->insert(os.str());
}
void CurvePropsWindow::on_reset_clip_clicked()
{
	int c;
	double r;
	Kernel::get_clip(c, r);
	set_clip(c == 0, r);
}

void CurvePropsWindow::on_width_changed(gfloat val)
{
	width = val;
}
void CurvePropsWindow::on_gamma_changed(gfloat val)
{
	gamma = val;
}
void CurvePropsWindow::on_update_app_clicked()
{
#ifdef HAVE_STRINGSTREAM
	std::ostringstream os;
#else
	std::ostrstream os;
#endif
	os << "curve_width = " << width << ";\n"
	   << "curve_gamma = " << gamma << ";\n";
	scriptwin->insert(os.str());
}
void CurvePropsWindow::on_reset_app_clicked()
{
	double w, g;
	Kernel::get_curve_width(w);
	Kernel::get_curve_gamma(g);
	set_app(w, g);
}

void CurvePropsWindow::on_update_curve_clicked()
{
	on_update_app_clicked();
	on_update_clip_clicked();
	on_update_curverot_clicked();
	on_update_curvescale_clicked();
	on_update_curveorig_clicked();
	on_update_curvecol_clicked();
	on_update_bgcol_clicked();
}

void CurvePropsWindow::on_reset_curve_clicked()
{
	on_reset_bgcol_clicked();
	on_reset_curvecol_clicked();
	on_reset_curveorig_clicked();
	on_reset_curvescale_clicked();
	on_reset_curverot_clicked();
	on_reset_clip_clicked();
	on_reset_app_clicked();
}
