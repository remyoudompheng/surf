/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef GDK_DRAW_BITMAP_H
#define GDK_DRAW_BITMAP_H

#ifndef HAVE_GDK_DRAW_BITMAP

#include <gdk/gdk.h>

// (temporary?) substitute for the lacking gdk_draw_bitmap() function

void gdk_draw_bitmap(GdkDrawable* drawable,
		     GdkGC* gc,
		     GdkDrawable* src,
		     gint xsrc,
		     gint ysrc,
		     gint xdest,
		     gint ydest,
		     gint width,
		     gint height);

#endif

#endif //!GDK_DRAW_BITMAP_H
