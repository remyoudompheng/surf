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




#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/XWDFile.h>

#ifdef SUN
int fread ( char*,int,int,FILE* );
int fwrite ( char*,int,int,FILE* );
int fclose ( FILE* );
#endif /* SUN */

#include "Misc.h"
#include "FileWriter.h"
#include "RgbBuffer.h"

#include "XWD.h"

namespace ImageFormats {

	XWD imgFmt_XWD;


	bool XWD::saveColorImage(const char* fname, RgbBuffer& data, bool fromDlg)
	{
#ifndef NO_GUI
		if (shown) {
			return false;
		}

		if (fromDlg) {
			shown = true;

			dialog = gtk_dialog_new();
			EVENTCONNECT(dialog, "delete_event", handle_delete_event);
			gtk_window_set_title(GTK_WINDOW(dialog), "Save XWD image");

			// vbox:

			gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), 4);
			GtkWidget* frame = gtk_frame_new("Color Mode:");
			gtk_container_set_border_width(GTK_CONTAINER(frame), 8);
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
					   frame, true, true, 0);
			GtkWidget* vbox = gtk_vbox_new(true, 0);
			gtk_container_set_border_width(GTK_CONTAINER(vbox), 4);
			gtk_container_add(GTK_CONTAINER(frame), vbox);
			GtkWidget* rbt = gtk_radio_button_new_with_label(0, "True Color (24bpp)");
			gtk_box_pack_start(GTK_BOX(vbox), rbt, false, false, 0);
			GSList* grp = gtk_radio_button_group(GTK_RADIO_BUTTON(rbt));
			rbt = gtk_radio_button_new_with_label(grp, "Indexed (8bpp)");
			indexedRB = rbt;
			gtk_box_pack_start(GTK_BOX(vbox), rbt, false, false, 0);
			
			indexedFrame = gtk_frame_new("Indexed Options:");
			gtk_container_set_border_width(GTK_CONTAINER(indexedFrame), 8);
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
					   indexedFrame, true, true, 0);
			vbox = gtk_vbox_new(true, 0);
			gtk_container_set_border_width(GTK_CONTAINER(vbox), 4);
			gtk_container_add(GTK_CONTAINER(indexedFrame), vbox);
			rbt = gtk_radio_button_new_with_label(0, "Netscape Pallette");
			gtk_box_pack_start(GTK_BOX(vbox), rbt, false, false, 0);
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
			gtk_widget_set_sensitive(indexedFrame, false);
			
			VOIDCONNECT(indexedRB, "toggled", handle_indexed);
			VOIDCONNECT(optimizedRB, "toggled", handle_optimized);
			VOIDCONNECT(ditherCB, "toggled", handle_dither);
			
			// action area:
			
			GtkWidget* bt = gtk_button_new_with_label("Okay");
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
					   bt, true, true, 0);
			VOIDCONNECT(bt, "clicked", handle_ok);
			GTK_WIDGET_SET_FLAGS(bt, GTK_CAN_DEFAULT);
			gtk_widget_grab_default(bt);
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
			bool indexed = false;
			reallySave(fname, data);
		}
		return true;
	}

#ifndef NO_GUI
	void XWD::handle_ok()
	{
		indexed = GTK_TOGGLE_BUTTON(indexedRB)->active;
		optimized = GTK_TOGGLE_BUTTON(optimizedRB)->active;
		dither = GTK_TOGGLE_BUTTON(ditherCB)->active;
		ditherval = GTK_ADJUSTMENT(ditherAdj)->value;
		save = true;
		g_main_quit(gmainloop);
	}
	
	gint XWD::handle_delete_event()
	{
		save = false;
		g_main_quit(gmainloop);
		return true;
	}

	void XWD::handle_cancel()
	{
		save = false;
		g_main_quit(gmainloop);
	}
	
#endif

	void XWD::reallySave(const char* filename, RgbBuffer& buffer)
	{
		FileWriter fw(filename);
		FILE* file;
		
		if((file = fw.openFile()) == 0) {
			Misc::alert("Could not open file for writing...");
			return;
		}

		if (indexed) {
			saveAsIndexed(file, buffer);
		} else {
			saveAsTrueColor(file, buffer);
		}
	}

	void XWD::saveAsTrueColor(FILE* file, RgbBuffer& buffer)
	{
		int width = buffer.getWidth();
		int height = buffer.getHeight();

		char win_name[] = "surf_xwd";
		int win_name_size = strlen(win_name) + 1;
		
		/* Initialize xwd header */
		
		XWDFileHeader header;
		int header_size = sizeof(header) + win_name_size;
		
		header.header_size      = (CARD32)header_size;
		header.file_version     = (CARD32)XWD_FILE_VERSION;
		header.pixmap_format    = (CARD32)ZPixmap;
		header.pixmap_depth     = (CARD32)24;
		header.pixmap_width     = (CARD32)width;
		header.pixmap_height    = (CARD32)height;
		header.xoffset          = (CARD32)0;
		header.byte_order       = (CARD32)MSBFirst;
		header.bitmap_unit      = (CARD32)32;
		header.bitmap_bit_order = (CARD32)MSBFirst;
		header.bitmap_pad       = (CARD32)32;
		header.bits_per_pixel   = (CARD32)24;
		header.bytes_per_line   = (CARD32)((int)((24*width+31)/32))*4;
		header.visual_class     = (CARD32)DirectColor;
		header.red_mask         = (CARD32)(0x00ff0000);
		header.green_mask       = (CARD32)(0x0000ff00);
		header.blue_mask        = (CARD32)(0x000000ff);
		header.bits_per_rgb     = (CARD32)24;
		header.colormap_entries = (CARD32)256;
		header.ncolors          = (CARD32)0;
		header.window_width     = (CARD32)width;
		header.window_height    = (CARD32)height;
		header.window_x         = 0;
		header.window_y         = 0;
		header.window_bdrwidth  = (CARD32)0;
		
		/* Calculate before swapping !! */
		
		int scanline_pad = header.bytes_per_line - width*3;
		
		if (scanline_pad > 3) {
			std::cout << "scanline_pad too big..\n";
			return;
		}
		
		/* swap if necessary */
		
		unsigned long swaptest = 1;
		if (*(char*)&swaptest) {
			swaplong((char*)&header, sizeof(header));
		}
		
		
		/* Write xwd header */
		
		fwrite(&header, sizeof(header), 1, file);
		fwrite(win_name, win_name_size, 1, file);
		
		/* Write the image data */
		
		const guint8* rdata = buffer.getRData();
		const guint8* gdata = buffer.getGData();
		const guint8* bdata = buffer.getBData();

		int k = 0;
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				fputc(rdata[k], file);
				fputc(gdata[k], file);
				fputc(bdata[k], file);
				k++;
			}
			for(int j = 0; j < scanline_pad; j++) {
				fputc(0, file);
			}
		}
	}

	void XWD::saveAsIndexed(FILE* file, RgbBuffer& buffer)
	{
		if (optimized) {
			buffer.OptimizedColor(dither, ditherval);
		} else {
			buffer.NetscapeColor();
		}
		
		int width = buffer.getWidth();
		int height = buffer.getHeight();
		int nmap = buffer.getNumCols();

		XColor colors[256];
		char win_name[] = "surf_xwd";
		int win_name_size = strlen(win_name) + sizeof(char);
		
		/* Initialize xwd header */
		
		XWDFileHeader header;
		int header_size = sizeof(header) + win_name_size;
		
		header.header_size      = (CARD32)header_size;
		header.file_version     = (CARD32)XWD_FILE_VERSION;
		header.pixmap_format    = (CARD32)ZPixmap;
		header.pixmap_depth     = (CARD32)8;
		header.pixmap_width     = (CARD32)width;
		header.pixmap_height    = (CARD32)height;
		header.xoffset          = (CARD32)0;
		header.byte_order       = (CARD32)MSBFirst;
		header.bitmap_unit      = (CARD32)8;
		header.bitmap_bit_order = (CARD32)MSBFirst;
		header.bitmap_pad       = (CARD32)8;
		header.bits_per_pixel   = (CARD32)8;
		header.bytes_per_line   = (CARD32)width;
		header.visual_class     = (CARD32)PseudoColor;
		header.red_mask         = (CARD32)0;
		header.green_mask       = (CARD32)0;
		header.blue_mask        = (CARD32)0;
		header.bits_per_rgb     = (CARD32)8;
		header.colormap_entries = (CARD32)nmap;
		header.ncolors          = (CARD32)nmap;
		header.window_width     = (CARD32)width;
		header.window_height    = (CARD32)height;
		header.window_x         = 0;
		header.window_y         = 0;
		header.window_bdrwidth  = (CARD32)0;
		
		/* Get colors */
		
		const guint8* rmap = buffer.getRMap();
		const guint8* gmap = buffer.getGMap();
		const guint8* bmap = buffer.getBMap();

		for(int i = 0; i < nmap; i++) {
			colors[i].pixel = i;
			colors[i].red   = rmap[i] << 8;
			colors[i].green = gmap[i] << 8;
			colors[i].blue  = bmap[i] << 8;
			colors[i].flags = DoRed | DoGreen | DoBlue;
			colors[i].pad  =0;
		}
		
		/* swap if necessary */
		
		unsigned long swaptest = 1;
		
		if (*(char*)&swaptest) {
			swaplong((char*)&header, sizeof(header));
			
			for (int i = 0; i < nmap; i++) {
				swaplong((char*)&colors[i].pixel, sizeof(long));
				swapshort((char*)&colors[i].red, 3*sizeof(short));
			}
		}
		
		
		/* Write xwd header */
		
		fwrite(&header, sizeof(header), 1, file);
		fwrite(win_name, win_name_size, 1, file);
		
		/* Write colormap */
		
		fwrite((char*)colors, sizeof(XColor), nmap, file);
		
		/* Write image data */
		
		fwrite(buffer.getMap(), width*height, 1, file);
	}

} // namespace ImageFormats
