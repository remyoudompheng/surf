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

#include <ScriptWindow.h>
#include <Kernel.h>
#include <About.h>
#include <Misc.h>

#include<iostream>
#include<fstream>
#include<strstream>

namespace {
	char modified_txt[] = "The current script has been modified.\n"
                              "Do you want to save it?";
};

ScriptWindow::ScriptWindow(Glade& _glade, Kernel& _kernel)
	: glade(_glade),
	  prefswin(glade, this),
	  imagewin(glade, _kernel, this),
	  navigationwin(glade, _kernel, this),
	  kernel(_kernel),
	  dirty(false)
{
	kernel.set_scriptwin(this);
	
	window = glade.get_widget("window_script");
	text_script = glade.get_widget("text_script");
	
	glade.sig_connect(text_script, "changed", _on_text_script_changed, this);
	glade.sig_connect(window, "delete_event", _on_delete_event, this);
	glade.sig_connect("new", "activate", _on_new_activate, this);
	glade.sig_connect("button_new", "clicked", _on_new_activate, this);
	glade.sig_connect("open", "activate", _on_open_activate, this);
	glade.sig_connect("button_open", "clicked", _on_open_activate, this);
	glade.sig_connect("save", "activate", _on_save_activate, this);
	glade.sig_connect("button_save", "clicked", _on_save_activate, this);
	glade.sig_connect("save_as", "activate", _on_save_as_activate, this);
	glade.sig_connect("preferences", "activate", _on_prefs_activate, this);
	glade.sig_connect("quit", "activate", _on_quit_activate, this);
	glade.sig_connect("cut", "activate", _on_cut_activate, this);
	glade.sig_connect("copy", "activate", _on_copy_activate, this);
	glade.sig_connect("paste", "activate", _on_paste_activate, this);
	glade.sig_connect("clear", "activate", _on_clear_activate, this);
	glade.sig_connect("navigation", "activate", _on_navigation_activate, this);
	glade.sig_connect("button_navigation", "clicked", _on_navigation_activate, this);
	glade.sig_connect("render_curve", "activate", _on_render_curve_activate, this);
	glade.sig_connect("button_render_curve", "clicked", _on_render_curve_activate, this);
	glade.sig_connect("render_surface", "activate", _on_render_surface_activate, this);
	glade.sig_connect("button_render_surface", "clicked", _on_render_surface_activate, this);
	glade.sig_connect("execute", "activate", _on_execute_activate, this);
	glade.sig_connect("button_execute", "clicked", _on_execute_activate, this);
	glade.sig_connect("about", "activate", _on_about_activate, this);
	glade.sig_connect("button_stop", "clicked", _on_stop_clicked, this);

	sbar = GTK_STATUSBAR(glade.get_widget("statusbar_script"));
	sbar_context = gtk_statusbar_get_context_id(sbar, "default context");
	
	pbar = glade.get_widget("progressbar_script");

	gtk_widget_show(window);

	on_new_activate();
}

void ScriptWindow::get_geometry(int& xpos, int& ypos, int& width, int& height)
{
	GtkWidget* window = glade.get_widget("window_script");
	int depth;
	gdk_window_get_geometry(window->window, &xpos, &ypos, &width, &height, &depth);

}

void ScriptWindow::set_geometry(int xpos, int ypos, int width, int height)
{
	GtkWidget* window = glade.get_widget("window_script");
	gtk_widget_set_uposition(window, xpos, ypos);
	gtk_window_set_default_size(GTK_WINDOW(window), width, height);
}

void ScriptWindow::set_font(const std::string& font)
{
	GtkStyle* style = gtk_style_copy(gtk_widget_get_default_style());
	if(font.length() > 0) {
		style->font = gdk_font_load(font.c_str());
	}
	gtk_widget_set_style(glade.get_widget("text_script"), style);
}

void ScriptWindow::load_file(const std::string& fname)
{
	std::ifstream file(fname.c_str());
	if(!file) {
		set_status("Open failed: Couldn't open file!");
		gdk_beep();
		return;
	}

	gtk_text_freeze(GTK_TEXT(text_script));
	gtk_editable_delete_text(GTK_EDITABLE(text_script), 0, -1);
	
	char buf[256];
	gint pos = 0;
	while(file.getline(buf, sizeof(buf) - 1, '\n')) {
		size_t len = strlen(buf);
		buf[len] = '\n';
		buf[len + 1] = 0;
		gtk_editable_insert_text(GTK_EDITABLE(text_script),
					 buf, len + 1, &pos);
	}
	gtk_text_set_point(GTK_TEXT(text_script), 0);
	gtk_text_thaw(GTK_TEXT(text_script));
	
	dirty = false;
	filename.assign(fname);
	set_title();

	set_status("Script loaded.");
}

void ScriptWindow::set_status(const std::string& txt)
{
	gtk_statusbar_pop(sbar, sbar_context);
	gtk_statusbar_push(sbar, sbar_context, txt.c_str());
	gtk_widget_draw(GTK_WIDGET(sbar), 0);
}

void ScriptWindow::set_progress(gfloat percentage)
{
	gtk_progress_set_percentage(GTK_PROGRESS(pbar), percentage);
	gtk_widget_draw(pbar, 0);
}

void ScriptWindow::progress_mode(bool mode) const
{
	gtk_progress_set_show_text(GTK_PROGRESS(pbar), mode);
	if(!mode) {
		gtk_progress_set_percentage(GTK_PROGRESS(pbar), 0);
	}
}

void ScriptWindow::set_title()
{
	std::string title = PACKAGE;
	title += " - ";
	if(filename.length() == 0) {
		title += "Untitled";
	} else {
		title += filename;
	}
	gtk_window_set_title(GTK_WINDOW(window), title.c_str());
}

void ScriptWindow::select_region(int from, int to)
{
	from -= prelude_length;
	to -= prelude_length;

	// catch the error case
        // (otherwise I get a segfault!? Hey Gtk, this shouldn't happen!)
	if(from < 0 || to < 0 || from > to
	   || to > int(glade.get_chars(text_script).length()) - 1) {
		Misc::print_warning("Something funny happenend in ScriptWindow::select_region. Write a bug report!");
		return;
	}

	gtk_editable_set_position(GTK_EDITABLE(text_script), from);
	gtk_editable_select_region(GTK_EDITABLE(text_script), from, to);
}

void ScriptWindow::insert(const std::string& str)
{
	gint pos = 0;
	gtk_editable_insert_text(GTK_EDITABLE(text_script), str.c_str(),
				 str.length(), &pos);
}


// Gtk callbacks:
// ======================================================================

void ScriptWindow::on_new_activate()
{
	if(dirty) {
		if(glade.ask_user(modified_txt)) {
			on_save_activate();
		}
	}
	
	filename.erase();
	set_title();
	gtk_text_freeze(GTK_TEXT(text_script));
	gtk_editable_delete_text(GTK_EDITABLE(text_script), 0, -1);
	gtk_text_thaw(GTK_TEXT(text_script));

	dirty = false;
}

void ScriptWindow::on_open_activate()
{
	if(dirty) {
		if(glade.ask_user(modified_txt)) {
			on_save_activate();
		}
	}
	
	set_status("Open Script...");
	
	if(glade.fileselect("Open Script")) {
		load_file(glade.get_filename());
	}
}

void ScriptWindow::on_save_activate()
{
	if(filename.length() == 0) {
		on_save_as_activate();
		return;
	}
	
	set_status("Save Script...");
	
	gchar* scr = gtk_editable_get_chars(GTK_EDITABLE(text_script), 0, -1);
	{
		std::ofstream file(filename.c_str());
		if(!file) {
			set_status("Saving failed: Couldn't open file!\n");
			gdk_beep();
			return;
		}
		file << scr;
	}
	g_free(scr);
		
	dirty = false;

	set_status("Script saved.");
}

void ScriptWindow::on_save_as_activate()
{
	set_status("Save Script As...");

	if(glade.fileselect("Save Script As")) {
		filename = glade.get_filename();
		set_title();
		on_save_activate();
	}
}

void ScriptWindow::on_prefs_activate()
{
	prefswin.show();
}

void ScriptWindow::on_quit_activate()
{
	if(dirty) {
		if(glade.ask_user(modified_txt)) {
			on_save_activate();
		}
	}

	kernel.disconnect_handler();
	gtk_main_quit();
}

void ScriptWindow::on_cut_activate()
{
	gtk_editable_cut_clipboard(GTK_EDITABLE(text_script));
}

void ScriptWindow::on_copy_activate()
{
	gtk_editable_copy_clipboard(GTK_EDITABLE(text_script));
}

void ScriptWindow::on_paste_activate()
{
	gtk_editable_paste_clipboard(GTK_EDITABLE(text_script));
}

void ScriptWindow::on_clear_activate()
{
	gtk_editable_delete_selection(GTK_EDITABLE(text_script));
}

void ScriptWindow::on_navigation_activate()
{
	kernel.reset();
	std::string script = "surface_run_commands = 0;\n";
	prelude_length = script.length();
	script += glade.get_chars(text_script);
	script += "surface_run_commands = 1;\n"
		  "triangulate_surface;\n";
	kernel.send(script);
	navigationwin.show();
}

void ScriptWindow::on_render_curve_activate()
{
	kernel.reset();
	std::string script = "surface_run_commands = 0;\n";
	prelude_length = script.length();
	script += glade.get_chars(text_script);
	script += "surface_run_commands = 1;\n"
		  "clear_screen;\n"
		  "draw_curve;\n";
	kernel.send(script);
}

void ScriptWindow::on_render_surface_activate()
{
	kernel.reset();
	std::string script = "surface_run_commands = 0;\n";
	prelude_length = script.length();
	script += glade.get_chars(text_script);
	script += "surface_run_commands = 1;\n"
		  "draw_surface;\n";
	kernel.send(script);
}

void ScriptWindow::on_execute_activate()
{
	kernel.reset();
	std::string script = "surface_run_commands = 1;\n";
	prelude_length = script.length();
	script += glade.get_chars(text_script);
	kernel.send(script);
}

void ScriptWindow::on_about_activate()
{
	About about(glade);
	about.show();
}

void ScriptWindow::on_stop_clicked()
{
	kernel.stop();
}
