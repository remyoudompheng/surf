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

#include "XWD.h"

namespace ImageFormats {

	XWD imgFmt_XWD;


	bool XWD::saveColorImage(const char* filename,
				 guint8* rdata, guint8* gdata, guint8* bdata,
				 int width, int height, bool fromDlg)
	{
		FileWriter fw(filename);
		FILE *file;
		
		if((file = fw.openFile()) == 0) {
			Misc::alert ("Could not open file for writing...");
			return false;
		}
		
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
			return false;
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
		
		return true;
	}

} // namespace ImageFormats
