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




#ifdef HAVE_LIBJPEG

#include "Misc.h"
#include "FileWriter.h"
#include "RgbBuffer.h"

// these two includes and the 'extern "C" { }' are needed for jpeglib.h
// otherwise I get errors here :-/  (stdio.h is neede anyway..)
#include <ctype.h>
#include <stdio.h>
extern "C" {
#include <jpeglib.h>
}

#include "JPEG.h"

namespace ImageFormats {

	JPEG imgFmt_JPEG;

	bool JPEG::saveColorImage(const char* fname, RgbBuffer& data, bool fromDlg)
	{
#ifndef NO_GUI
		if(shown) {
			return false;
		}

		if (fromDlg) {
			shown = true;

			dialog = gtk_dialog_new();
			EVENTCONNECT(dialog, "delete_event", handle_delete_event);
			gtk_window_set_title(GTK_WINDOW(dialog), "Save JPEG image");
			
			// vbox:
			
			gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), 4);
			GtkWidget* frame = gtk_frame_new("adjust image quality:");
			gtk_container_set_border_width(GTK_CONTAINER(frame), 8);
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
					   frame, true, true, 0);
			qualityAdj = gtk_adjustment_new(90, 0, 100, 1, 10, 0);
			GtkWidget* scale = gtk_hscale_new(GTK_ADJUSTMENT(qualityAdj));
			gtk_container_add(GTK_CONTAINER(frame), scale);
			
			// action area:
			
			GtkWidget* bt = gtk_button_new_with_label("Okay");
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
					   bt, true, true, 0);
			GTK_WIDGET_SET_FLAGS(bt, GTK_CAN_DEFAULT);
			gtk_widget_grab_default(bt);
			VOIDCONNECT(bt, "clicked", handle_ok);
			bt = gtk_button_new_with_label("Abort");
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
					   bt, true, true, 0);
			VOIDCONNECT(bt, "clicked", handle_cancel);
			
			gmainloop = g_main_new(false);

			gtk_widget_show_all(dialog);
			g_main_run(gmainloop);
			
			// wait for ok or cancel..

			g_main_destroy(gmainloop);
			gtk_widget_destroy(dialog);

			if(save) {
				reallySave(fname, data);
			}

			shown = false;
		} else
#endif
		       {
			quality = 90;
			reallySave(fname, data);
		}
		return true;
	}

	void JPEG::reallySave(const char* filename, RgbBuffer& buffer)
	{
		FileWriter fw(filename);
		FILE* file;
		
		if((file = fw.openFile()) == 0) {
			Misc::alert("Could not open file for writing...");
			return;
		}

		int width = buffer.getWidth();
		int height = buffer.getHeight();

		// init cinfo struct:

		jpeg_compress_struct cinfo;
		jpeg_error_mgr jerr;
		
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, file);
		cinfo.image_width = width;
		cinfo.image_height = height;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, quality, true);
		jpeg_start_compress(&cinfo, true);
		
		
		// compress and write to file:
		
		int row_stride = width*3;
		JSAMPLE* row = new JSAMPLE[row_stride];
		JSAMPROW rowpointer[1] = { row };
		JSAMPLE* rptr = buffer.getRData();
		JSAMPLE* gptr = buffer.getGData();
		JSAMPLE* bptr = buffer.getBData();
		
		while(cinfo.next_scanline < unsigned(height)) {
			JSAMPLE* ptr = row;
			for(int i = 0; i < width; ++i) {
				*ptr++ = *rptr++;
				*ptr++ = *gptr++;
				*ptr++ = *bptr++;
			}
			jpeg_write_scanlines(&cinfo, rowpointer, 1);
		}
		
		delete [] row;
		
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
	}

#ifndef NO_GUI

	// Gtk callbacks:

	void JPEG::handle_ok()
	{
		quality = int(GTK_ADJUSTMENT(qualityAdj)->value);
		save = true;
		g_main_quit(gmainloop);
	}

	void JPEG::handle_cancel()
	{
		save = false;
		g_main_quit(gmainloop);
	}

	gint JPEG::handle_delete_event()
	{
		save = false;
		g_main_quit(gmainloop);
		return true;
	}
#endif


} // namespace ImageFormats

#endif // HAVE_LIBJPEG
