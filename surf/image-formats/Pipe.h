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


#ifndef IMAGEFORMAT_PIPE_H
#define IMAGEFORMAT_PIPE_H

#ifndef NO_GUI  // this whole thing makes only sense with GUI-support

#include <gtk/gtk.h>
#include "mygtk.h"

#include <cstring>
#include <cstdlib>

#include "ImageFormats.h"

namespace ImageFormats {

	class Pipe : public Format {
	public:
		const char* getName() {
			return "Send through Pipe";
		}
		
		ColorType getColorType() {
			return both;
		}

		bool isExtension(const char* ext) {
			return false;
		}

		bool saveColorImage(const char* filename, RgbBuffer& data, bool fromDlg);
		
		bool saveDitheredImage(const char* filename, bit_buffer& data, int paper_width, int paper_height, int resolution, bool fromDlg);

		void showDialog();
		void destroyDialog() {
			gtk_widget_destroy(dialog);
		}

	private:
		ColorType type;
		char* filename;
		RgbBuffer* buffer;
		bit_buffer* pixel;
		int paper_height, paper_width;
		int resolution;
		bool fromDialog;
		
		GtkWidget* dialog;
		GtkWidget* menu;
		GtkWidget* entry;

		Format* format;
		char* command;
		
		VOIDCALL(handle_ok, Pipe);
		VOIDCALL(handle_cancel, Pipe) {
			destroyDialog();
			std::free(filename);
		}

		void reallySave();
	};

	gint Pipe_handle_delete(GtkWidget*, GdkEvent*, gpointer data);

	extern Pipe imgFmt_Pipe;
}


#endif //!NO_GUI

#endif //!IMAGEFORMAT_PIPE_H
