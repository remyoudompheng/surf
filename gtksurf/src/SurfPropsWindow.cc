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

#include <SurfPropsWindow.h>
#include <ScriptWindow.h>
#include <Kernel.h>
#include <Misc.h>

#ifdef HAVE_STRINGSTREAM
#  include<sstream>
#else
#  include<strstream>
#endif

#include<cmath>

SurfPropsWindow::SurfPropsWindow(ScriptWindow* _scriptwin)
	: scriptwin(_scriptwin)
{
	pi = std::acos(-1.0);

	window = Glade::get_widget("window_surfprops");
	Glade::sig_connect(window, "delete_event", _on_delete_event, this);

	Glade::sig_connect("button_surfprops_material_update", "clicked", _on_update_material_clicked, this);
	Glade::sig_connect("button_surfprops_material_reset", "clicked", _on_reset_material_clicked, this);

	Glade::sig_connect("button_surfprops_trafo_update", "clicked", _on_update_trafo_clicked, this);
	Glade::sig_connect("button_surfprops_trafo_reset", "clicked", _on_reset_trafo_clicked, this);

	Glade::sig_connect("button_surfprops_light_update", "clicked", _on_update_light_clicked, this);
	Glade::sig_connect("button_surfprops_light_reset", "clicked", _on_reset_light_clicked, this);

	Glade::sig_connect("button_surfprops_illu_update", "clicked", _on_update_illumination_clicked, this);
	Glade::sig_connect("button_surfprops_illu_reset", "clicked", _on_reset_illumination_clicked, this);

	Glade::sig_connect("button_surfprops_persp_update", "clicked", _on_update_perspective_clicked, this);
	Glade::sig_connect("button_surfprops_persp_reset", "clicked", _on_reset_perspective_clicked, this);

	Glade::sig_connect("button_surfprops_update", "clicked", _on_update_all_clicked, this);
	Glade::sig_connect("button_surfprops_reset", "clicked", _on_reset_all_clicked, this);
}

void SurfPropsWindow::show()
{
	gtk_widget_show(window);
	gdk_window_raise(window->window);
}

void SurfPropsWindow::hide()
{
	gtk_widget_hide(window);
}


void SurfPropsWindow::update_material(int i)
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_update_material_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::reset_material(int i)
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_reset_material_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::update_trafo(int i)
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_update_trafo_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::reset_trafo(int i)
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_reset_trafo_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::update_light(int i)
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_update_light_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::reset_light(int i)
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_reset_light_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_update_illumination_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_reset_illumination_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_update_perspective_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_reset_perspective_clicked()
{
	std::cout << "Ok\n";
}

void SurfPropsWindow::on_update_all_clicked()
{
	for(int i = 1; i <= 9; i++) {
		update_material(i);
		update_trafo(i);
		update_light(i);
	}
	on_update_illumination_clicked();
	on_update_perspective_clicked();
}

void SurfPropsWindow::on_reset_all_clicked()
{
	for(int i = 1; i <= 9; i++) {
		reset_material(i);
		reset_trafo(i);
	}
	for(int i = 1; i <= 9; i++) {
		reset_light(i);
	}
	on_reset_illumination_clicked();
	on_reset_perspective_clicked();
}
