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

#include <DitherWindow.h>
#include <ScriptWindow.h>
#include <Misc.h>
#include <gdk_draw_bitmap.h>

#include <gdk/gdkkeysyms.h>

#include<iostream>
#include<fstream>
#include<strstream>

namespace {
	guint8 change_bits(guint8 b)
	{
		return    (b >> 7) & 0x01
			| (b >> 5) & 0x02
			| (b >> 3) & 0x04
			| (b >> 1) & 0x08
			| (b << 1) & 0x10
			| (b << 3) & 0x20
			| (b << 5) & 0x40
			| (b << 7) & 0x80;
	}
}

DitherWindow::DitherWindow(Glade& _glade, Kernel& _kernel, ScriptWindow* sw)
	: glade(_glade), kernel(_kernel), scriptwin(sw),
	  bitmap(0)
{
	kernel.set_ditherwin(this);
	
	window = glade.get_widget("window_dither");
	drawingarea = glade.get_widget("drawingarea_dither");
	
	glade.sig_connect(window, "delete_event", _on_delete_event, this);
	glade.sig_connect(window, "key_press_event", _on_key_press_event, this);
	glade.sig_connect(drawingarea, "expose_event", _on_expose_event, this);
	glade.sig_connect(drawingarea, "button_press_event", _on_button_press_event, this);
	gtk_widget_add_events(drawingarea, GDK_BUTTON_PRESS_MASK);
	glade.sig_connect("save_dither", "activate", _on_save_activate, this);
	glade.sig_connect("save_dither_as", "activate", _on_save_as_activate, this);
	glade.sig_connect("close_dither", "activate", _on_close_activate, this);
	
	popupmenu = glade.get_widget("menu_dither");
	
	set_title();

	GdkColormap* cmap = gdk_colormap_get_system();
	gdk_color_white(cmap, &gcval.background);
	gdk_color_black(cmap, &gcval.foreground);
}

void DitherWindow::set_title()
{
	std::string title = PACKAGE;
	title += " - Dither Image: ";

	if(filename.length() == 0) {
		title += "Untitled";
	} else {
		title += filename;
	}
	gtk_window_set_title(GTK_WINDOW(window), title.c_str());
}

void DitherWindow::read_data()
{
	if(bitmap != 0) {
		gdk_bitmap_unref(bitmap);
	}

	std::string whstring = kernel.receive_line();
	std::istrstream is(whstring.c_str());
	int width;
	int height;
	is >> width >> height;
	size_t bytesPerRow = width/8 + (width%8 ? 1 : 0);
	size_t length = height*bytesPerRow;
	guint8* data = new guint8[length];
	for(size_t i = 0; i != length; i++) {
		data[i] = change_bits(kernel.receive_byte());
	}

	bitmap = gdk_bitmap_create_from_data(drawingarea->window,
					     reinterpret_cast<gchar*>(data),
					     width, height);
	delete [] data;

	gtk_widget_set_usize(drawingarea, width, height);
	gtk_widget_set_usize(window, width, height);

	show();
}


// Gtk callbacks:
// ======================================================================

void DitherWindow::on_key_press_event(GdkEventKey* event)
{
	if(event->state == GDK_CONTROL_MASK) {
		switch(event->keyval) {
		case GDK_s:
			on_save_activate();
			break;
		case GDK_w:
			on_close_activate();
			break;
		}
	}
}

void DitherWindow::on_expose_event(GdkEventExpose* event)
{
	GdkGC* gc = gdk_gc_new_with_values(drawingarea->window, &gcval,
					   GdkGCValuesMask(GDK_GC_FOREGROUND | GDK_GC_BACKGROUND));
	gdk_draw_bitmap(drawingarea->window, gc,
			bitmap, 
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);
}

void DitherWindow::on_button_press_event(GdkEventButton* event)
{
	if(event->button == 3) {
		gtk_menu_popup(GTK_MENU(popupmenu), 0, 0, 0, 0, 3, event->time);
	}
}

void DitherWindow::on_save_activate(GtkWidget*)
{
	if(filename.length() == 0) {
		on_save_as_activate();
		return;
	}

	std::string s = "filename = \"";
	s += filename + "\";\n";
	if(filetype.length() > 0) {
		s += "dither_file_format = ";
		s += filetype + ";\n";
	}
	s += "save_dithered_image;\n";
	kernel.send(s);
}

void DitherWindow::on_save_as_activate(GtkWidget*)
{
	// construct "Save Options" frame w/file type menu:
	
	GtkWidget* menu = gtk_menu_new();

	std::list<std::string> fmts = kernel.get_dither_image_formats();
	std::list<std::string>::iterator i;
	for(i = fmts.begin(); i != fmts.end(); ) {
		GtkWidget* item = gtk_menu_item_new_with_label((*i).c_str());
		i++;
		gtk_object_set_user_data(GTK_OBJECT(item), const_cast<char*>((*i).c_str()));
		i++;
		gtk_menu_append(GTK_MENU(menu), item);
		glade.sig_connect(item, "activate", _on_filetype_activate, this);
	}
	filetype.assign("auto");
	gtk_widget_show_all(menu);

	if(glade.fileselect("Save Dither Image As", menu)) {
		filename = glade.get_filename();
		set_title();
		on_save_activate();
	}
	
	gtk_widget_destroy(menu);
}

void DitherWindow::on_close_activate(GtkWidget*)
{
	hide();
}

void DitherWindow::on_filetype_activate(GtkWidget* wid)
{
	char* str = static_cast<char*>(gtk_object_get_user_data(GTK_OBJECT(wid)));
	if(str != 0) {
		filetype.assign(str);
	}
}
