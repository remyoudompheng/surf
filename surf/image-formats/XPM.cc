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


#include "Misc.h"
#include "FileWriter.h"
#include "RgbBuffer.h"
#include "Script.h"

#include "XPM.h"

namespace ImageFormats {

	XPM imgFmt_XPM;

	bool XPM::saveColorImage(const char* fname, RgbBuffer& data, bool fromDlg)
	{
#ifndef NO_GUI
		if (shown) {
			return false;
		}
		shown = true;
#endif
		
		filename = std::strdup(fname); // preserve data
		buffer = &data;
		
#ifndef NO_GUI
		if (fromDlg) {
			ditherDialog();
		} else
#endif
		       {
			optimized = true;
			dither = true;
			ditherval = 20.0;
			reallySave();
			std::free(filename);
		}
		return true;
	}

#ifndef NO_GUI
	void XPM::ditherDialog()
	{
		dialog = gtk_dialog_new();
		gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
				   GTK_SIGNAL_FUNC(&XPM_handle_delete), this);
		gtk_window_set_title(GTK_WINDOW(dialog), "Save XPM image");

		// vbox:

		gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), 4);
		GtkWidget* frame;
		frame = gtk_frame_new("Options:");
		gtk_container_set_border_width(GTK_CONTAINER(frame), 8);
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
				   frame, true, true, 0);
		GtkWidget* vbox;
		vbox = gtk_vbox_new(true, 0);
		gtk_container_set_border_width(GTK_CONTAINER(vbox), 4);
		gtk_container_add(GTK_CONTAINER(frame), vbox);
		GtkWidget* rbt;
		rbt = gtk_radio_button_new_with_label(0, "Netscape Pallette");
		gtk_box_pack_start(GTK_BOX(vbox), rbt, false, false, 0);
		GSList *grp;
		grp = gtk_radio_button_group(GTK_RADIO_BUTTON(rbt));
		rbt = gtk_radio_button_new_with_label(grp, "Optimized Pallette");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbt), true);
		optimizedRB = rbt;
		gtk_box_pack_start(GTK_BOX(vbox), rbt, false, false, 0);
		ditherHBox = gtk_hbox_new(false, 0);
		gtk_box_pack_start(GTK_BOX(vbox), ditherHBox, false, false, 0);
		ditherCB = gtk_check_button_new_with_label("Dither Image:");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ditherCB), true);
		gtk_box_pack_start(GTK_BOX(ditherHBox), ditherCB, false, false, true);
		ditherAdj = gtk_adjustment_new(20, 0, 100, 1, 10, 0);
		ditherSpin = gtk_spin_button_new(GTK_ADJUSTMENT(ditherAdj), 0, 2);
		gtk_box_pack_start(GTK_BOX(ditherHBox), ditherSpin, true, true, 0);

		VOIDCONNECT(optimizedRB, "toggled", handle_optimized);
		VOIDCONNECT(ditherCB, "toggled", handle_dither);

		// action area:

		GtkWidget* bt;
		bt = gtk_button_new_with_label("Okay");
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
				   bt, true, true, 0);
		VOIDCONNECT(bt, "clicked", handle_ok);
		GTK_WIDGET_SET_FLAGS(bt, GTK_CAN_DEFAULT);
		gtk_widget_grab_default(bt);
		bt = gtk_button_new_with_label("Abort");
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
				   bt, true, true, 0);
		VOIDCONNECT(bt, "clicked", handle_cancel);


		gtk_widget_show_all(dialog);
	}

	void XPM::handle_ok()
	{
		optimized = GTK_TOGGLE_BUTTON(optimizedRB)->active;
		dither = GTK_TOGGLE_BUTTON(ditherCB)->active;
		ditherval = GTK_ADJUSTMENT(ditherAdj)->value;
		
		destroyDialog();
		reallySave();
	}

	gint XPM_handle_delete(GtkWidget*, GdkEvent*, gpointer data)
	{
		XPM* This = (XPM*)data;
		This->destroyDialog();
		std::free(This->filename);
		return true;
	}
#endif

	void XPM::reallySave()
	{
		FileWriter fw(filename);
		FILE *file;
		
		if((file = fw.openFile()) == 0) {
			Misc::alert ("Could not open file for writing...");
			return;
		}

		if (optimized) {
			buffer->OptimizedColor(dither, ditherval);
		} else {
			buffer->NetscapeColor();
		}

		// write header

		fputs("/* XPM */\n"
		      "static char* surf_image[] = {\n\n", file);

		int width = buffer->getWidth();
		int height = buffer->getHeight();
		int nmap = buffer->getNumCols();

		fprintf(file, "\"%d %d %d 2\",\n\n", width, height, nmap);
		
		// write colors
		
		const guint8* rmap = buffer->getRMap();
		const guint8* gmap = buffer->getGMap();
		const guint8* bmap = buffer->getBMap();

		for(int i = 0; i < nmap; i++) {
			fprintf(file, "\"%.2x c #%.2x%.2x%.2x\",\n", i, rmap[i], gmap[i], bmap[i]);
		}

		// write pixels

		const guint8* ptr = buffer->getMap();

		for(int i = 0; i < height; i++) {
			fputc('\"', file);
			for(int j = 0; j < width; j++) {
				fprintf(file, "%.2x", *ptr++);
			}
			fputs("\",\n", file);
		}

		fputs("\n};\n", file);

		std::free(filename);
	}

} // namespace ImageFormats
