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



#ifndef IMAGEFORMATS_H
#define IMAGEFORMATS_H

#ifndef NO_GUI
#  include <gtk/gtk.h>
#else
typedef unsigned char guint8;   // this _could_ be dangerous..
typedef unsigned short guint16; // this *is* dangerous!
typedef unsigned long guint32;  // and this one, too.

// If we really want to go for sure, we had to configure this in
// configure (like Gtk's does for glibconfig.h)

#endif
#include <stdio.h>

class bit_buffer;
class RgbBuffer;

namespace ImageFormats {
	enum ColorType {
		color, dithered, both
	};
	
	class Format {
	public:
		virtual const char* getName() = 0;
		virtual bool isExtension(const char* ext) = 0;
		virtual ColorType getColorType() = 0;
		
		virtual bool saveColorImage(const char* filename, RgbBuffer& data, bool fromDlg) = 0;
		virtual bool saveDitheredImage(const char* filename, bit_buffer& data, int paper_width, int paper_height, int resolution, bool fromDlg) = 0;
	};

#ifndef NO_GUI
	GtkWidget* makeFormatMenu(ColorType type);
	Format* getSelection(GtkWidget* menu);
#endif
	Format* guessFormat(const char* filename, ColorType type);
	void put_long(long word, FILE* f, int order);
	void swapshort(char* bp, unsigned int n);
	void swaplong(char*bp, unsigned int n);
}

#endif //!IMAGEFORMATS_H
