/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef MYGLADE_H
#define MYGLADE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include<string>

#include <gtk/gtk.h>
#include <glade/glade.h>

class GladeWindow {};

class Glade : public GladeWindow {
public:
	static void init(int argc, char* argv[], const std::string& gladefile);
	static void deinit();

	static void main_loop() {
		gtk_main();
	}

	static GtkWidget* get_widget(char* name) {
		return glade_xml_get_widget(gladexml, name);
	}

	static void sig_connect(char* name, char* sig, void (*func)(GtkWidget*, GladeWindow*), GladeWindow* This = 0) {
		sig_connect(get_widget(name), sig, func, This);
	}
	static void sig_connect(GtkWidget* wid, char* sig, void (*func)(GtkWidget*, GladeWindow*), GladeWindow* This = 0) {
		gtk_signal_connect(GTK_OBJECT(wid), sig, GTK_SIGNAL_FUNC(func), gpointer(This));
	}
	static void sig_connect(char* name, char* sig, int (*func)(GtkWidget*, GdkEvent*, GladeWindow*), GladeWindow* This = 0) {
		sig_connect(get_widget(name), sig, func, This);
	}
	static void sig_connect(GtkWidget* wid, char* sig, int (*func)(GtkWidget*, GdkEvent*, GladeWindow*), GladeWindow* This = 0) {
		gtk_signal_connect(GTK_OBJECT(wid), sig, GTK_SIGNAL_FUNC(func), gpointer(This));
	}
	static GtkSpinButton* spin_connect(char* name, void (*func)(GtkAdjustment*, GladeWindow*), GladeWindow* This = 0) {
		GtkSpinButton* sp = (GtkSpinButton*)get_widget(name);
		GtkWidget* adj= (GtkWidget*)gtk_spin_button_get_adjustment(sp);
		gtk_signal_connect(GTK_OBJECT(adj), "value_changed", GTK_SIGNAL_FUNC(func), gpointer(This));
		return sp;
	}
	static GtkToggleButton* toggle_connect(GtkToggleButton* tb, void (*func)(GtkToggleButton*, GladeWindow*), GladeWindow* This = 0) {
		gtk_signal_connect(GTK_OBJECT(tb), "toggled", GTK_SIGNAL_FUNC(func), gpointer(This));
		return tb;
	}
	static GtkToggleButton* toggle_connect(char* name, void (*func)(GtkToggleButton*, GladeWindow*), GladeWindow* This = 0) {
		GtkToggleButton* tb = (GtkToggleButton*)get_widget(name);
		return toggle_connect(tb, func, This);
	}
	static std::string get_chars(GtkWidget* txtwid) {
		gchar* content = gtk_editable_get_chars(GTK_EDITABLE(txtwid), 0, -1);
		std::string res = content;
		g_free(content);
		return res;
	}

	static bool ask_user(const char* txt);
        static void show_message(const std::string& txt);

	static bool fileselect(const std::string& title, GtkWidget* menu = 0);
	static std::string get_filename() {
		return filename;
	}
	static std::string get_filetype() {
		return filetype;
	}

	static bool fontselect();
	static std::string get_fontname() {
		return fontname;
	}

private:
	static GladeXML* gladexml;

	static GMainLoop* gmainloop;

	// Ok-dialog stuff:
	static GtkWidget* okdlg;
	static void on_ok_clicked(GtkWidget*, GladeWindow*) {
		g_main_quit(gmainloop);
	}
	static int on_ok_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* d) {
		on_ok_clicked(w, d);
		return true;
	}

	// Yes/No-dialog stuff:
	static GtkWidget* yesnodlg;
	static bool yes_clicked;
	static void on_yes_clicked(GtkWidget*, GladeWindow*) {
		yes_clicked = true;
		g_main_quit(gmainloop);
	}
	static void on_no_clicked(GtkWidget*, GladeWindow*) {
		yes_clicked = false;
		g_main_quit(gmainloop);
	}
	static int on_yesno_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* d) {
		on_no_clicked(w, d);
		return true;
	}

	// FileSelection-dialog stuff:
	static GtkFileSelection* fsel;
	static GtkWidget* saveopts_frame;
	static GtkWidget* optionmenu;
	static bool fselok_clicked;
	static std::string filename;
	static std::string filetype;
	static void on_fselok_clicked(GtkWidget*, GladeWindow*) {
		fselok_clicked = true;
		g_main_quit(gmainloop);
	}
	static void on_fselcancel_clicked(GtkWidget*, GladeWindow*) {
		fselok_clicked = false;
		g_main_quit(gmainloop);
	}
	static int on_fsel_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* d) {
		on_fselcancel_clicked(w, d);
		return true;
	}

	// FontSelectionDialog stuff:
	static GtkFontSelectionDialog* fontsel;
	static bool fontselok_clicked;
	static std::string fontname;
	static void on_fontselok_clicked(GtkWidget*, GladeWindow*) {
		fontselok_clicked = true;
		g_main_quit(gmainloop);
	}
	static void on_fontselcancel_clicked(GtkWidget*, GladeWindow*) {
		fontselok_clicked = false;
		g_main_quit(gmainloop);
	}
	static int on_fontsel_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* d) {
		on_fontselcancel_clicked(w, d);
		return true;
	}
};

#endif //!MYGLADE_H
