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
	Glade(int argc, char* argv[], const std::string& gladefile);
	virtual ~Glade();

	void main_loop() {
		gtk_main();
	}

	GtkWidget* get_widget(char* name) const {
		return glade_xml_get_widget(gladexml, name);
	}

	void sig_connect(char* name, char* sig, void (*func)(GtkWidget*, GladeWindow*), GladeWindow* This) const {
		sig_connect(get_widget(name), sig, func, This);
	}
	void sig_connect(GtkWidget* wid, char* sig, void (*func)(GtkWidget*, GladeWindow*), GladeWindow* This) const {
		gtk_signal_connect(GTK_OBJECT(wid), sig, GTK_SIGNAL_FUNC(func), gpointer(This));
	}
	void sig_connect(char* name, char* sig, int (*func)(GtkWidget*, GdkEvent*, GladeWindow*), GladeWindow* This) const {
		sig_connect(get_widget(name), sig, func, This);
	}
	void sig_connect(GtkWidget* wid, char* sig, int (*func)(GtkWidget*, GdkEvent*, GladeWindow*), GladeWindow* This) const {
		gtk_signal_connect(GTK_OBJECT(wid), sig, GTK_SIGNAL_FUNC(func), gpointer(This));
	}
	GtkSpinButton* spin_connect(char* name, void (*func)(GtkAdjustment*, GladeWindow*), GladeWindow* This) const {
		GtkSpinButton* sp = (GtkSpinButton*)get_widget(name);
		GtkWidget* adj= (GtkWidget*)gtk_spin_button_get_adjustment(sp);
		gtk_signal_connect(GTK_OBJECT(adj), "value_changed", GTK_SIGNAL_FUNC(func), gpointer(This));
		return sp;
	}
	GtkToggleButton* toggle_connect(GtkToggleButton* tb, void (*func)(GtkToggleButton*, GladeWindow*), GladeWindow* This) const {
		gtk_signal_connect(GTK_OBJECT(tb), "toggled", GTK_SIGNAL_FUNC(func), gpointer(This));
		return tb;
	}
	GtkToggleButton* toggle_connect(char* name, void (*func)(GtkToggleButton*, GladeWindow*), GladeWindow* This) const {
		GtkToggleButton* tb = (GtkToggleButton*)get_widget(name);
		return toggle_connect(tb, func, This);
	}
	std::string get_chars(GtkWidget* txtwid) {
		gchar* content = gtk_editable_get_chars(GTK_EDITABLE(txtwid), 0, -1);
		std::string res = content;
		g_free(content);
		return res;
	}

	bool ask_user(const char* txt) const;
        void show_message(const std::string& txt) const;

	bool fileselect(const std::string& title, GtkWidget* menu = 0);
	std::string get_filename() const {
		return filename;
	}
	std::string get_filetype() const {
		return filetype;
	}

	bool fontselect();
	std::string get_fontname() const {
		return fontname;
	}

private:
	GladeXML* gladexml;

	GMainLoop* gmainloop;

	// Ok-dialog stuff:
	GtkWidget* okdlg;
	void on_ok_clicked() {
		g_main_quit(gmainloop);
	}
	static void _on_ok_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<Glade*>(This)->on_ok_clicked();
	}
	static int _on_ok_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* This) {
		static_cast<Glade*>(This)->on_ok_clicked();
		return true;
	}

	// Yes/No-dialog stuff:
	GtkWidget* yesnodlg;
	bool yes_clicked;
	void on_yes_clicked() {
		yes_clicked = true;
		g_main_quit(gmainloop);
	}
	static void _on_yes_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<Glade*>(This)->on_yes_clicked();
	}
	void on_no_clicked() {
		yes_clicked = false;
		g_main_quit(gmainloop);
	}
	static void _on_no_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<Glade*>(This)->on_no_clicked();
	}
	static int _on_yesno_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* This) {
		static_cast<Glade*>(This)->on_no_clicked();
		return true;
	}

	// FileSelection-dialog stuff:
	GtkFileSelection* fsel;
	GtkWidget* saveopts_frame;
	GtkWidget* optionmenu;
	bool fselok_clicked;
	std::string filename;
	std::string filetype;
	void on_fselok_clicked() {
		fselok_clicked = true;
		g_main_quit(gmainloop);
	}
	static void _on_fselok_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<Glade*>(This)->on_fselok_clicked();
	}
	void on_fselcancel_clicked() {
		fselok_clicked = false;
		g_main_quit(gmainloop);
	}
	static void _on_fselcancel_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<Glade*>(This)->on_fselcancel_clicked();
	}
	static int _on_fsel_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* This) {
		static_cast<Glade*>(This)->on_fselcancel_clicked();
		return true;
	}

	// FontSelectionDialog stuff:
	GtkFontSelectionDialog* fontsel;
	bool fontselok_clicked;
	std::string fontname;
	void on_fontselok_clicked() {
		fontselok_clicked = true;
		g_main_quit(gmainloop);
	}
	static void _on_fontselok_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<Glade*>(This)->on_fontselok_clicked();
	}
	void on_fontselcancel_clicked() {
		fontselok_clicked = false;
		g_main_quit(gmainloop);
	}
	static void _on_fontselcancel_clicked(GtkWidget* w, GladeWindow* This) {
		static_cast<Glade*>(This)->on_fontselcancel_clicked();
	}
	static int _on_fontsel_delete_event(GtkWidget* w, GdkEvent* e, GladeWindow* This) {
		static_cast<Glade*>(This)->on_fontselcancel_clicked();
		return true;
	}
};

#endif //!MYGLADE_H
