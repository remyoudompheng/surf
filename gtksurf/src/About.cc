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

#include <About.h>

About::About(const Glade& _glade)
	: glade(_glade)
{
	win = glade.get_widget("window_about");
	gtk_widget_add_events(win, GDK_BUTTON_PRESS_MASK);
	glade.sig_connect(win, "button_press_event", _on_button_press_event, this);
	glade.sig_connect(win, "delete_event", _on_delete_event, this);

	gtk_widget_realize(win); // we have to do this do get an win->window
	gdk_window_set_decorations(win->window, GdkWMDecoration(0));

	GtkWidget* label = glade.get_widget("label_about");
	gtk_label_set_text((GtkLabel*)label,
                           PACKAGE " " VERSION "\n"
			   "a GTK+ based graphical frontend to surf\n"
			   "\n"
			   "Author: Johannes Beigel <jojo@beigel.de>\n"
			   "Copyright (C) 2001 Johannes Beigel\n"
			   "License: GPL v2 or later\n"
			   "\n"
			   "http://surf.sourceforge.net");
	
	// set style:
	style = gtk_style_copy(gtk_widget_get_default_style());
	GdkColormap* cmap = gdk_colormap_get_system();
	gdk_color_white(cmap, &style->fg[0]);
	gdk_color_black(cmap, &style->bg[0]);
	gtk_widget_set_style(label, style);
	gtk_widget_set_style(win, style);

	gmainloop = g_main_new(false);
}

About::~About()
{
	g_main_destroy(gmainloop);
	gtk_widget_hide(win);
	gtk_style_unref(style);
}

void About::show()
{
	gtk_widget_show_all(win);
	g_main_run(gmainloop); // wait for user's answer
	gtk_widget_hide(win);
}
