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




#include <assert.h>

#include "SaveImageDialog.h"

#include "Script.h"
#include "RgbBuffer.h"
#include "bit_buffer.h"
#include "gui_config.h"
#include "Misc.h"

SaveImageDialog::SaveImageDialog ()
{
	fileselectiondialog = gtk_file_selection_new ("Save image...");
	gtk_signal_connect (GTK_OBJECT(fileselectiondialog), "delete_event", GTK_SIGNAL_FUNC(handle_delete), this);

	VOIDCONNECT(GTK_FILE_SELECTION(fileselectiondialog)->ok_button, "clicked", handle_okay);
	VOIDCONNECT(GTK_FILE_SELECTION(fileselectiondialog)->cancel_button, "clicked", handle_cancel);
	
	GtkWidget* frame = gtk_frame_new("Save Options:");
	GtkWidget* hbox = gtk_hbox_new(false, 8);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Determine File Type:"), false, false, 0);
	optionMenu = gtk_option_menu_new();
	gtk_box_pack_start(GTK_BOX(hbox), optionMenu, true, true, 0);
	gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(fileselectiondialog)->main_vbox), frame, false, false, 0);
}

void SaveImageDialog::show(ImageFormats::ColorType t)
{
	GtkWidget* menu = 0;

	switch (type = t) {
	case ImageFormats::color:
		gtk_window_set_title (GTK_WINDOW(fileselectiondialog), "Save color image as...");
		menu = ImageFormats::makeFormatMenu(ImageFormats::color);
		break;
	case ImageFormats::dithered:
		gtk_window_set_title (GTK_WINDOW(fileselectiondialog), "Save dithered image as...");
		menu = ImageFormats::makeFormatMenu(ImageFormats::dithered);
		break;
	default:
		Misc::alert("Strange error: ColorType out of range.");
		return;
	}
	gtk_option_menu_set_menu(GTK_OPTION_MENU(optionMenu), menu);
	gtk_widget_show_all (fileselectiondialog);
	gdk_window_raise(fileselectiondialog->window);
}

gint SaveImageDialog::handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	SaveImageDialog* This = (SaveImageDialog*)data;
	This->hide();
	return TRUE;
}

void SaveImageDialog::handle_okay()
{
	ImageFormats::Format* fmt = ImageFormats::getSelection(GTK_OPTION_MENU(optionMenu)->menu);
	assert(fmt != 0);
	
	hide(); // don't hide earlier (fmt would be 0, because menu would have been removed..)

	const char* filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fileselectiondialog));
	assert(filename != 0);

	switch (type) {
	case ImageFormats::color: {
		fmt->saveColorImage(filename, *Script::getBuffer(), true);
		break;
	}
	case ImageFormats::dithered: {
		fmt->saveDitheredImage(filename, *Script::getBitBuffer(),
				       main_width_data, main_height_data,
				       print_resolution_array_data[print_resolution_data],
				       true);
		break;
	}
	default:
		; // without the "default:" we'd get a warning..
	}
}
