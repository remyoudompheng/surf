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

#include <Glade.h>

Glade::Glade(int argc, char* argv[], const std::string& gladefile)
{
	gtk_init(&argc, &argv);
	glade_init();
	gladexml = glade_xml_new(gladefile.c_str(), 0);
	if(gladexml == 0) {
		g_error("Couldn't load Glade XML file.\n");
	}

	okdlg = get_widget("dialog_ok");
	sig_connect(okdlg, "delete_event", _on_ok_delete_event, this);
	sig_connect("button_ok", "clicked", _on_ok_clicked, this);
	
	yesnodlg = get_widget("dialog_yesno");
	sig_connect(yesnodlg, "delete_event", _on_yesno_delete_event, this);
	sig_connect("button_yes", "clicked", _on_yes_clicked, this);
	sig_connect("button_no", "clicked", _on_no_clicked, this);

	fsel = reinterpret_cast<GtkFileSelection*>(get_widget("fileselection"));
	sig_connect(fsel->ok_button, "clicked", _on_fselok_clicked, this);
	sig_connect(fsel->cancel_button, "clicked", _on_fselcancel_clicked, this);
	sig_connect(GTK_WIDGET(fsel), "delete_event", _on_fsel_delete_event, this);
	saveopts_frame = gtk_frame_new("Save Options");
	GtkWidget* hbox = gtk_hbox_new(false, 4);
	gtk_container_add(GTK_CONTAINER(saveopts_frame), hbox);
	gtk_container_border_width(GTK_CONTAINER(hbox), 4);
	GtkWidget* label = gtk_label_new("Determine File Type:");
	gtk_box_pack_start(GTK_BOX(hbox), label, false, false, 0);
	optionmenu = gtk_option_menu_new();
	gtk_box_pack_start(GTK_BOX(hbox), optionmenu, true, true, 0);
	gtk_box_pack_start(GTK_BOX(fsel->main_vbox), saveopts_frame, false, false, 0);
	gtk_widget_show_all(saveopts_frame);
	
	fontsel = reinterpret_cast<GtkFontSelectionDialog*>(get_widget("fontselectiondialog"));
	sig_connect(fontsel->ok_button, "clicked", _on_fontselok_clicked, this);
	sig_connect(fontsel->cancel_button, "clicked", _on_fontselcancel_clicked, this);
	sig_connect(GTK_WIDGET(fontsel), "delete_event", _on_fontsel_delete_event, this);

	gmainloop = g_main_new(false);
}

Glade::~Glade()
{
	g_main_destroy(gmainloop);
	
	gtk_object_unref(GTK_OBJECT(gladexml));
}

bool Glade::ask_user(const std::string& txt) const
{
	gtk_label_set_text(GTK_LABEL(get_widget("label_yesno")), txt.c_str());
	gtk_widget_grab_focus(get_widget("button_yes"));
	gtk_widget_show(yesnodlg);
	gtk_grab_add(yesnodlg);
	g_main_run(gmainloop);
	gtk_grab_remove(yesnodlg);
	gtk_widget_hide(yesnodlg);
	return yes_clicked;
}

void Glade::show_message(const std::string& txt) const
{
	gtk_label_set_text(GTK_LABEL(get_widget("label_ok")), txt.c_str());
	gtk_widget_grab_focus(get_widget("button_ok"));
	gtk_widget_show(okdlg);
	gtk_grab_add(okdlg);
	g_main_run(gmainloop);
	gtk_grab_remove(okdlg);
	gtk_widget_hide(okdlg);
}

bool Glade::fileselect(const std::string& title, GtkWidget* menu)
{
	if(menu == 0) {
		gtk_widget_hide(saveopts_frame);
	} else {
		gtk_option_menu_set_menu(GTK_OPTION_MENU(optionmenu), menu);
		gtk_widget_show(saveopts_frame);
	}
	
	gtk_window_set_title(GTK_WINDOW(fsel), title.c_str());
	gtk_widget_show(GTK_WIDGET(fsel));
	gtk_grab_add(GTK_WIDGET(fsel));
	g_main_run(gmainloop);
	gtk_grab_remove(GTK_WIDGET(fsel));
	gtk_widget_hide(GTK_WIDGET(fsel));
	filename.assign(gtk_file_selection_get_filename(fsel));
	return fselok_clicked;
}

bool Glade::fontselect()
{
	gtk_widget_show(GTK_WIDGET(fontsel));
	gtk_grab_add(GTK_WIDGET(fontsel));
	g_main_run(gmainloop);
	gtk_grab_remove(GTK_WIDGET(fontsel));
	gtk_widget_hide(GTK_WIDGET(fontsel));
	fontname.assign(gtk_font_selection_dialog_get_font_name(fontsel));
	return fontselok_clicked;
}
