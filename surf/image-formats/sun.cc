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
/* sun.c                                                                     */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mathematik.uni-mainz.de                                 */
/* Date:     6.8.98                                                          */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* I copied and modified the sun rasterfile pixmap dumper                    */
/* from the fbm package                                                      */
/* ------------------------------------------------------------------------- */
/*****************************************************************
 * flsun.c: FBM Release 1.0 25-Feb-90 Michael Mauldin
 *
 * Copyright (C) 1989,1990 by Michael Mauldin.  Permission is granted
 * to use this file in whole or in part for any purpose, educational,
 * recreational or commercial, provided that this copyright notice
 * is retained unchanged.  This software is available to all free of
 * charge by anonymous FTP and in the UUNET archives.
 *
 * flsun.c: 
 *
 * CONTENTS
 *	read_sun (image, rfile, mstr, mlen)
 *	write_sun (image, wfile)
 *
 * EDITLOG
 *	LastEditDate = Mon Jun 25 00:18:04 1990 - Michael Mauldin
 *	LastFileName = /usr2/mlm/src/misc/fbm/flsun.c
 *
 * HISTORY
 * 25-Jun-90  Michael Mauldin (mlm@cs.cmu.edu) Carnegie Mellon
 *	Package for Release 1.0
 *
 * 20-May-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Fixed problem with odd length rows on reading
 *
 * 26-Aug-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.96) mlm@cs.cmu.edu
 *
 * 12-Nov-88  Michael Mauldin (mlm) at Carnegie-Mellon University
 *	Created.
 *****************************************************************/

#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include <errno.h>
#include <string.h>

#include "sun.h"
#include "Misc.h"


#ifdef SUN
int fread ( char*,int,int,FILE* );
int fwrite ( char*,int,int,FILE* );
int fclose ( FILE* );
#endif /* SUN */

typedef struct rasterfile
{
	long ras_magic;
	long ras_width;
	long ras_height;
	long ras_depth;
	long ras_length;
	long ras_type;
	long ras_maptype;
	long ras_maplength;
} RASHDR;

#define BIG              1
#define SUN_MAGIC        0x59a66a95
#define RT_OLD	         0
#define RT_STANDARD      1
#define RT_BYTE_ENCODED  2
#define RT_RGB_FORMAT    3
#define RT_TIFF_FORMAT   4
#define RT_IFF_FORMAT    5

#define RMT_NONE	  0
#define RMT_EQUAL_RGB	  1
#define RMT_RAW	  2

/* ------------------------------------------------------------------------- */
/*  Write a long word to a file                                              */
/* ------------------------------------------------------------------------- */

static inline void put_long (register long word, FILE *file, int order)
{
	if (order == BIG) {
		fputc ((word >> 24) & 0x0ff, file); /* Put out biggest byte */
		fputc ((word >> 16) & 0x0ff, file); /* Put out 2nd byte */
		fputc ((word >> 8) & 0x0ff, file);  /* Put out 3rd byte */
		fputc (word & 0x0ff, file);         /* Put out littlest byte */
	} else {
		fputc (word & 0x0ff, file);         /* Put out littlest byte */
		fputc ((word >> 8) & 0x0ff, file);  /* Put out 3rd byte */
		fputc ((word >> 16) & 0x0ff, file); /* Put out 2nd byte */
		fputc ((word >> 24) & 0x0ff, file); /* Put out biggest byte */
	}
}

/* ------------------------------------------------------------------------- */
/* Save the contents of a pixmap in Sun rasterfile format                    */
/* ------------------------------------------------------------------------- */

bool write_sun8_file (byte *data, int width, int height,
		      byte *rmap, byte *gmap, byte *bmap,
		      int nmap, FILE *sunfile)
{
	RASHDR  rhdr;
	int     length = width*height;


	/* Initialize Sun raster header */

	rhdr.ras_magic     = SUN_MAGIC;
	rhdr.ras_width     = width;
	rhdr.ras_height    = height;
	rhdr.ras_depth     = 8;
	rhdr.ras_length    = length;
	rhdr.ras_type      = RT_STANDARD;
	rhdr.ras_maptype   = RMT_EQUAL_RGB;
	rhdr.ras_maplength = nmap*3;

	/* Write rasterfile header - note: use Sun byte order */

	put_long( rhdr.ras_magic,    sunfile,BIG );
	put_long( rhdr.ras_width,    sunfile,BIG );
	put_long( rhdr.ras_height,   sunfile,BIG );
	put_long( rhdr.ras_depth,    sunfile,BIG );
	put_long( rhdr.ras_length,   sunfile,BIG );
	put_long( rhdr.ras_type,     sunfile,BIG );
	put_long( rhdr.ras_maptype,  sunfile,BIG );
	put_long( rhdr.ras_maplength,sunfile,BIG );
  
	/* Dump colormap */

	fwrite (rmap, sizeof(byte), nmap, sunfile );
	fwrite (gmap, sizeof(byte), nmap, sunfile );
	fwrite (bmap, sizeof(byte), nmap, sunfile );
  
	/* Write bytes */

	fwrite (data, sizeof(byte)*length, 1, sunfile);


	return true;
}

bool write_sun24_file (byte *rdata, byte *gdata, byte *bdata,
		       int width, int height, FILE *sunfile)
{
	RASHDR  rhdr;
	int     length;
	int     i;
	
	length = width*height;

	/* Initialize Sun raster header */

	rhdr.ras_magic     = SUN_MAGIC;
	rhdr.ras_width     = width;
	rhdr.ras_height    = height;
	rhdr.ras_depth     = 24;
	rhdr.ras_length    = length*3;
	rhdr.ras_type      = RT_STANDARD;
	rhdr.ras_maptype   = RMT_NONE;
	rhdr.ras_maplength = 0;

	/* Write rasterfile header - note: use Sun byte order */

	put_long( rhdr.ras_magic,    sunfile,BIG );
	put_long( rhdr.ras_width,    sunfile,BIG );
	put_long( rhdr.ras_height,   sunfile,BIG );
	put_long( rhdr.ras_depth,    sunfile,BIG );
	put_long( rhdr.ras_length,   sunfile,BIG );
	put_long( rhdr.ras_type,     sunfile,BIG );
	put_long( rhdr.ras_maptype,  sunfile,BIG );
	put_long( rhdr.ras_maplength,sunfile,BIG );
  
	for( i=0; i<length; i++ ) {
		fputc( bdata[i],sunfile );
		fputc( gdata[i],sunfile );
		fputc( rdata[i],sunfile );
	}

	return true;
}
