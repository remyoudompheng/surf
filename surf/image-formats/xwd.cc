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




/* ------------------------------------------------------------------------- */
/* xwd.c                                                                     */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     2.1.95                                                          */
/* ------------------------------------------------------------------------- */

/* $XConsortium: xwd.c,v 1.56 91/07/25 18:00:15 rws Exp $ */
/* Copyright 1987 Massachusetts Institute of Technology   */

/*
 * xwd.c MIT Project Athena, X Window system window raster image dumper.
 */

/*
 * This copy of xwd.c contains no main, it was hacked out
 * from the file FElt-2.0/lib/Widgets/xwd.c for inclusion
 * of this functionality into surf ... I just adapted
 * the functionality that I needed to form a simple pixmap
 * dumper.
 */

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/XWDFile.h>

#include "xwd.h"
#include "Misc.h"

#ifdef SUN
int fread ( char*,int,int,FILE* );
int fwrite ( char*,int,int,FILE* );
int fclose ( FILE* );
#endif /* SUN */

#define BIG 1

/* ------------------------------------------------------------------------- */
/* Who had the SWAP idea ???                                                 */
/* ------------------------------------------------------------------------- */

static  void    _swapshort( register char *bp,register unsigned n )
{
	register char c;
	register char *ep = bp + n;

	while (bp < ep) {
		c = *bp;
		*bp = *(bp + 1);
		bp++;
		*bp++ = c;
	}
}

/* ------------------------------------------------------------------------- */
/* Make easy things complicated                                              */
/* ------------------------------------------------------------------------- */

static  void    _swaplong(register char *bp,register unsigned n)
{
	register char c;
	register char *ep = bp + n;
	register char *sp;

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

/* ------------------------------------------------------------------------- */
/* Save the contents of a pixmap in XWD format                               */
/* ------------------------------------------------------------------------- */

bool write_xwd8_file (byte *data, int width, int height,
		      byte *rmap, byte *gmap, byte *bmap,
		      int nmap, FILE *xwdfile)
{
	int                 length;
	int                 i;
	XColor              colors[256];
	int                 win_name_size;
	int                 header_size;
	const char                *win_name;
	XWDFileHeader       header;
	unsigned    long    swaptest = 1;

	length=width*height;
    
	win_name = "surf_xwd";
	win_name_size = strlen(win_name) + sizeof(char);
	
	/* Initialize xwd header */

	header_size = sizeof(header) + win_name_size;

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

	for( i=0; i<nmap;i++ ) {
		colors[i].pixel=i;
		colors[i].red  =rmap[i] << 8;
		colors[i].green=gmap[i] << 8;
		colors[i].blue =bmap[i] << 8;
		colors[i].flags=DoRed | DoGreen | DoBlue;
		colors[i].pad  =0;
	}

	/* swap if necessary */
	
	if( *(char*)&swaptest ) {
		_swaplong( (char*)&header,sizeof(header) );
		
		for( i = 0; i < nmap; i++ ) {
			_swaplong ( (char*)&colors[i].pixel,sizeof(long) );
			_swapshort( (char*)&colors[i].red  ,3*sizeof(short) );
		}
	}


	/* Write xwd header */
	
	fwrite( (char*)&header,sizeof(header),1,xwdfile );
	fwrite( win_name,win_name_size,1,xwdfile );
	
	/* Write colormap */

	fwrite((char*)colors,sizeof(XColor),nmap,xwdfile);

	/* Write image data */
	
	fwrite( data,(int)length,1,xwdfile );

	return true;
}

bool write_xwd24_file (byte *rdata, byte *gdata, byte *bdata,
		       int width, int height, FILE *xwdfile)
{
	int                 length;
	int                 i,j,k;
	int                 win_name_size;
	int                 header_size;
	const char          *win_name;
	XWDFileHeader       header;
	int                 scanline_pad;
	unsigned    long    swaptest = 1;
	
	length=width*height;
	
	win_name = "surf_xwd";
	win_name_size = strlen(win_name) + sizeof(char);
	
	/* Initialize xwd header */
	
	header_size = sizeof(header) + win_name_size;

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
	
	scanline_pad = header.bytes_per_line - width*3;
	
	if( scanline_pad > 3 ) {
		printf( "scanline_pad too big..\n" );
		exit( 1 );
	}

	/* swap if necessary */

	if( *(char*)&swaptest ) {
		_swaplong( (char*)&header,sizeof(header) );
	}


	/* Write xwd header */

	fwrite( (char*)&header,sizeof(header),1,xwdfile );
	fwrite( win_name,win_name_size,1,xwdfile );

	/* Write the image data */

	for( i=0, k=0; i<height; i++ ) {
		for( j=0; j<width; j++ ) {
			fputc( rdata[k],xwdfile );
			fputc( gdata[k],xwdfile );
			fputc( bdata[k],xwdfile );
			k++;
		}
		for( j=0; j<scanline_pad; j++ ) {
			fputc( 0,xwdfile );
		}
	}

	return true;
}
