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



#ifndef IMAGEFORMAT_JPEG_H
#define IMAGEFORMAT_JPEG_H

#include "ImageFormats.h"

#ifndef NO_GUI
#include <gtk/gtk.h>
#include "mygtk.h"
#endif

#include <cstring>
#include <cstdlib>

namespace ImageFormats {

	class JPEG : public Format {
	public:
		const char* getName() {
			return "JPEG";
		}
		
		ColorType getColorType() {
			return color;
		}

		bool isExtension(const char* ext) {
			if (std::strcasecmp(ext, "jpg") == 0 ||
			    std::strcasecmp(ext, "jpeg") == 0) {
				return true;
			}
			return false;
		}

		bool saveColorImage(const char* filename, guint8* rdata, guint8* gdata, guint8* bdata, int width, int height, bool fromDlg);
		
		bool saveDitheredImage(const char* filename, bit_buffer& data, int paper_width, int paper_height, int resolution, bool fromDlg) {
			return false;
		}

#ifndef NO_GUI
		void qualityDialog();
		void destroyDialog() {
			gtk_widget_destroy(dialog);
		}
#endif

	private:
		char* filename;
		guint8* red;
		guint8* green;
		guint8* blue;
		int width, height;
		
		int quality;

#ifndef NO_GUI		
		GtkWidget* dialog;
		GtkObject* qualityAdj;

		VOIDCALL(handle_ok, JPEG);
		VOIDCALL(handle_cancel, JPEG) {
			destroyDialog();
			std::free(filename);
		}
#endif

		void reallySave();
	};

#ifndef NO_GUI
	gint JPEG_handle_delete(GtkWidget*, GdkEvent*, gpointer data);
#endif
}

#endif //!IMAGEFORMAT_JPEG_H
