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




#ifndef IMAGEFORMAT_XPM_H
#define IMAGEFORMAT_XPM_H

#include "ImageFormats.h"

#ifndef NO_GUI
#include <gtk/gtk.h>
#include "mygtk.h"
#endif

#include <cstring>
#include <cstdlib>

namespace ImageFormats {

	class XPM : public Format {
	public:
		XPM() : shown(false) { }
		
		const char* getName() {
			return "XPM (8 bit)";
		}
		
		ColorType getColorType() {
			return color;
		}

		bool isExtension(const char* ext) {
			if (std::strcasecmp(ext, "xpm") == 0) {
				return true;
			}
			return false;
		}

		bool saveColorImage(const char* filename, RgbBuffer& data, bool fromDlg);
		
		bool saveDitheredImage(const char* filename, bit_buffer& data, int paper_width, int paper_height, int resolution, bool fromDlg) {
			return false;
		}

#ifndef NO_GUI
		void destroyDialog() {
			gtk_widget_destroy(dialog);
			shown = false;
		}
#endif
		
		char* filename;

	private:
		bool shown;

		RgbBuffer* buffer;
		
		bool optimized;
		bool dither;
		double ditherval;

#ifndef NO_GUI		
		void ditherDialog();

		GtkWidget* dialog;
		GtkWidget* optimizedRB;
		GtkWidget* ditherHBox;
		GtkWidget* ditherCB;
		GtkObject* ditherAdj;
		GtkWidget* ditherSpin;
		
		VOIDCALL(handle_ok, XPM);
		VOIDCALL(handle_cancel, XPM) {
			destroyDialog();
		}
		VOIDCALL(handle_optimized, XPM) {
			gtk_widget_set_sensitive(ditherHBox,
						 GTK_TOGGLE_BUTTON(optimizedRB)->active);
		}
		VOIDCALL(handle_dither, XPM) {
			gtk_widget_set_sensitive(ditherSpin,
						 GTK_TOGGLE_BUTTON(ditherCB)->active);
		}

#endif

		void reallySave();
	};

#ifndef NO_GUI
	gint XPM_handle_delete(GtkWidget*, GdkEvent*, gpointer data);
#endif

	extern XPM imgFmt_XPM;
}

#endif //!IMAGEFORMAT_XPM_H
