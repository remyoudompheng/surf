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



#include "AvailableImageFormats.h"
#include "Misc.h"

#include <iostream>
#include <endian.h>

#include "ImageFormats.h"

namespace ImageFormats {

#ifndef NO_GUI
	GtkWidget* makeFormatMenu(ColorType type)
	{
		GtkWidget* menu = gtk_menu_new();
		GSList* group = 0;

		for (int i = 0; i != numAvailableFormats; ++i) {
			ColorType fmtType = availableFormats[i]->getColorType();
			if (fmtType != both && fmtType != type) {
				continue;
			}
			
			GtkWidget* menuitem = gtk_radio_menu_item_new_with_label(group, availableFormats[i]->getName());
			gtk_object_set_user_data(GTK_OBJECT(menuitem), availableFormats[i]);
			group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(menuitem));
			gtk_menu_append(GTK_MENU(menu), menuitem);
		}
		return menu;
	}

	Format* getSelection(GtkWidget* menu)
	{
		GtkWidget* actItem = gtk_menu_get_active(GTK_MENU(menu));
		return (Format*)gtk_object_get_user_data(GTK_OBJECT(actItem));
	}
#endif // !NO_GUI
	
	Format* guessFormat(const char* filename, ColorType type)
	{
		if (filename[0] == '|') { // saving to pipe
			switch (type) {
			case color:
				return &imgFmt_PPM;
			case dithered:
				return &imgFmt_PBM;
			default: ; // we don't like warnings..
			}
		}

		char* ext = strrchr(filename, '.');

		if (ext == 0) {
			return 0;
		}

		ext += 1;
		
		for (int i = 0; i != numAvailableFormats; ++i) {
			if (availableFormats[i]->isExtension(ext)) {
				ColorType fmtType = availableFormats[i]->getColorType();
				if (fmtType == both || fmtType == type) {
					return availableFormats[i];
				} else {
					Misc::alert("You chose the wrong extension.");
					return 0;
				}
			}
		}
		return 0;
	}
	
	
	// some file format implementations could use this utility function:
	// Write a long word to a file respecting endianess:
	void put_long(long word, FILE* file, int order)
	{
#if __BYTE_ORDER == LITTLE_ENDIAN
		if (order == BIG_ENDIAN) {
#else
	        if (order == LITTLE_ENDIAN) {
#endif
			fputc((word >> 24) & 0x0ff, file);
			fputc((word >> 16) & 0x0ff, file);
			fputc((word >> 8) & 0x0ff, file);
			fputc(word & 0x0ff, file);
		} else {
			fputc(word & 0x0ff, file);
			fputc((word >> 8) & 0x0ff, file);
			fputc((word >> 16) & 0x0ff, file);
			fputc((word >> 24) & 0x0ff, file);
		}
	}
	
	
	void swapshort(char* bp, unsigned int n)
	{
		char c;
		char* ep = bp + n;
		
		while (bp < ep) {
			c = *bp;
			*bp = *(bp + 1);
			bp++;
			*bp++ = c;
		}
	}

	void swaplong(char* bp, unsigned int n)
	{
		char c;
		char *ep = bp + n;
		char *sp;
		
		while (bp < ep) {
			sp = bp + 3;
			c = *sp;
			*sp = *bp;
			*bp++ = c;
			sp = bp + 1;
			c = *sp;
			*sp = *bp;
			*bp++ = c;
			bp += 2;
		}
	}



} // namespace ImageFormats
