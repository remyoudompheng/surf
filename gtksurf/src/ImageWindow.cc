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

#include <ImageWindow.h>

#include<iostream>
#include<fstream>
#include<strstream>

ImageWindow::ImageWindow(Glade& _glade, Kernel& _kernel)
	: glade(_glade), kernel(_kernel),
	  pixbuf(0)
{
	window = glade.get_widget("window_image");
	drawingarea = glade.get_widget("drawingarea");
	
	glade.sig_connect(window, "delete_event", _on_delete_event, this);
	glade.sig_connect(drawingarea, "expose_event", _on_expose_event, this);
	glade.sig_connect(drawingarea, "button_press_event", _on_button_press_event, this);
	gtk_widget_add_events(drawingarea, GDK_BUTTON_PRESS_MASK);
	glade.sig_connect("save_image", "activate", _on_save_activate, this);
	glade.sig_connect("save_image_as", "activate", _on_save_as_activate, this);
	glade.sig_connect("close_image", "activate", _on_close_activate, this);
	glade.sig_connect("copy_image", "activate", _on_copy_activate, this);
	
	popupmenu = glade.get_widget("menu_image");
	
	set_title();
}

void ImageWindow::set_title()
{
	std::string title = PACKAGE;
	title += " - Color Image: ";
	if(filename.length() == 0) {
		title += "Untitled";
	} else {
		title += filename;
	}
	gtk_window_set_title(GTK_WINDOW(window), title.c_str());
}

void ImageWindow::show()
{
	gtk_widget_show(window);
	gdk_window_raise(window->window);
}

void ImageWindow::hide()
{
	gtk_widget_hide(window);
}

void ImageWindow::clear()
{
	if(pixbuf == 0) {
		return; // there's nothing we could clear..
	}

	int width = gdk_pixbuf_get_width(pixbuf);
	int height = gdk_pixbuf_get_height(pixbuf);
	gdk_pixbuf_unref(pixbuf);

	size_t length = 3*width*height;
	
	guchar* pixdata = new guchar[length];
	for(size_t i = 0; i != length; i++) {
		pixdata[i] = 0;
	}
	pixbuf = gdk_pixbuf_new_from_data(pixdata, GDK_COLORSPACE_RGB, false, 8,
					  width, height, 3*width, delete_array_fn, 0);

        // I think it's better if we do *not* refresh the display!
        // gtk_widget_queue_draw(drawingarea);
	show();
}

void ImageWindow::set_image(guchar* pixdata, int width, int height, size_t rowstride)
{
	if(pixbuf != 0) {
		gdk_pixbuf_unref(pixbuf);
	}

	pixbuf = gdk_pixbuf_new_from_data(pixdata, GDK_COLORSPACE_RGB, false, 8,
					  width, height, rowstride, delete_array_fn, 0);

	//width = gdk_pixbuf_get_width(pixbuf);
	//height = gdk_pixbuf_get_height(pixbuf);
	gtk_widget_set_usize(drawingarea, width, height);
	gtk_widget_set_usize(window, width, height);

	show();
}


// Gtk callbacks:
// ======================================================================

void ImageWindow::on_expose_event(GdkEventExpose* event)
{
	gdk_pixbuf_render_to_drawable(pixbuf, drawingarea->window, 
				      drawingarea->style->fg_gc[1],
				      event->area.x, event->area.y,
				      event->area.x, event->area.y,
				      event->area.width, event->area.height,
				      GdkRgbDither(GDK_RGB_DITHER_NONE), 0, 0);
}

void ImageWindow::on_button_press_event(GdkEventButton* event)
{
	if(event->button == 3) {
		gtk_menu_popup(GTK_MENU(popupmenu), 0, 0, 0, 0, 3, event->time);
	}
}

void ImageWindow::on_save_activate(GtkWidget*)
{
	if(filename.length() == 0) {
		on_save_as_activate();
		return;
	}

	std::string s = "filename = \"";
	s += filename + "\";\n";
	if(filetype.length() > 0) {
		s += "color_file_format = ";
		s += filetype + ";\n";
	}
	s += "save_color_image;\n";
	kernel.send(s);
}

void ImageWindow::on_save_as_activate(GtkWidget*)
{
	// construct "Save Options" frame w/file type menu:
	
	GtkWidget* menu = gtk_menu_new();

	std::list<std::string> fmts = kernel.get_color_image_formats();
	std::list<std::string>::iterator i;
	for(i = fmts.begin(); i != fmts.end(); ) {
		GtkWidget* item = gtk_menu_item_new_with_label(i->c_str());
		i++;
		gtk_object_set_user_data(GTK_OBJECT(item), const_cast<char*>(i->c_str()));
		i++;
		gtk_menu_append(GTK_MENU(menu), item);
		glade.sig_connect(item, "activate", _on_filetype_activate, this);
	}
	filetype.assign("auto");
	gtk_widget_show_all(menu);

	if(glade.fileselect("Save Image As", menu)) {
		filename = glade.get_filename();
		set_title();
		on_save_activate();
	}
	
	gtk_widget_destroy(menu);
}

void ImageWindow::on_close_activate(GtkWidget*)
{
	hide();
}

void ImageWindow::on_copy_activate(GtkWidget*)
{
	// TODO!!!

	// copy image
}

void ImageWindow::on_filetype_activate(GtkWidget* wid)
{
	char* str = static_cast<char*>(gtk_object_get_user_data(GTK_OBJECT(wid)));
	
	if(str != 0) {
		std::cerr << "filetype is now: " << str << "\n";
		filetype.assign(str);
	}
}
