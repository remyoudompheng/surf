/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef NO_GUI  // this whole thing makes only sense with GUI-support


#include "Misc.h"
#include "FileWriter.h"

#include "Pipe.h"


namespace ImageFormats {

	Pipe imgFmt_Pipe;

	bool Pipe::saveColorImage(const char* fname,
				  guint8* rdata, guint8* gdata, guint8* bdata,
				  int w, int h, bool fromDlg)
	{
		type = color;
		filename = std::strdup(fname); // preserve data
		red = rdata;
		green = gdata;
		blue = bdata;
		width = w;
		height = h;
		fromDialog = fromDlg;

		showDialog();

		return true;
	}

	bool Pipe::saveDitheredImage(const char* fname,
				     bit_buffer& buf,
				     int w, int h, int res, bool fromDlg)
	{
		type = dithered;
		filename = std::strdup(fname); // preserve data
		pixel = &buf;
		paper_width = w;
		paper_height = h;
		resolution = res;
		fromDialog = fromDlg;
		
		showDialog();
		
		return true;
	}

	void Pipe::showDialog()
	{
		dialog = gtk_dialog_new();
		gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
				   GTK_SIGNAL_FUNC(&Pipe_handle_delete), this);
		gtk_window_set_title(GTK_WINDOW(dialog), "Send through Pipe");

		// vbox:

		gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), 4);
		GtkWidget* hbox = gtk_hbox_new(true, 8);
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, true, true, 0);
		gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("choose image format:"), false, false, 0);
		GtkWidget* optMenu = gtk_option_menu_new();
		menu = makeFormatMenu(type);
		gtk_option_menu_set_menu(GTK_OPTION_MENU(optMenu), menu);
		gtk_box_pack_start(GTK_BOX(hbox), optMenu, true, true, 0);
		
		hbox = gtk_hbox_new(true, 8);
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, true, true, 0);
		gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("command:"), false, false, 0);
		entry = gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(hbox), entry, true, true, 0);

		// action area:

		GtkWidget* bt;
		bt = gtk_button_new_with_label("Okay");
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
				   bt, true, true, 0);
		VOIDCONNECT(bt, "clicked", handle_ok);
		bt = gtk_button_new_with_label("Abort");
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
				   bt, true, true, 0);
		VOIDCONNECT(bt, "clicked", handle_cancel);

		gtk_widget_show_all(dialog);
	}

	void Pipe::handle_ok()
	{
		format = getSelection(menu);
		command = std::strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
		
		destroyDialog();
		
		reallySave();
	}

	gint Pipe_handle_delete(GtkWidget*, GdkEvent*, gpointer data)
	{
		Pipe* This = (Pipe*)data;
		This->destroyDialog();
		return true;
	}

	void Pipe::reallySave()
	{
		int len = strlen(command);
		char* new_filename = new char[len + 1];
		new_filename[0] = '|';
		std::strcpy(new_filename + 1, command);

		switch (type) {
		case color:
			format->saveColorImage(new_filename, red, green, blue, width, height, fromDialog);
			break;
		case dithered:
			format->saveDitheredImage(new_filename, *pixel, paper_width, paper_height, resolution, fromDialog);
			break;
		default: ;
		}
		
		delete [] new_filename;
		std::free(command);
	}


} // namespace ImageFormats

#endif //!NO_GUI
