/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */

#ifndef HAVE_GDK_DRAW_BITMAP

#include <gdk_draw_bitmap.h>

// This function has been sent to the
// gtk-app-devel@lists.gnome.org mailing list by Andy Tai:

/* temporary implementation of the missing GDK function */

#include <gdk/gdkprivate.h>

void gdk_draw_bitmap     (GdkDrawable  *drawable,
                          GdkGC        *gc,
                          GdkDrawable  *src,
                          gint         xsrc,
                          gint         ysrc,
                          gint         xdest,
                          gint         ydest,
                          gint         width,
                          gint         height)
{
    GdkWindowPrivate *drawable_private;
    GdkWindowPrivate *src_private;
    GdkGCPrivate *gc_private;

    g_return_if_fail (drawable != 0);
    g_return_if_fail (src != 0);
    g_return_if_fail (gc != 0);

    drawable_private = (GdkWindowPrivate*) drawable;
    src_private = (GdkWindowPrivate*) src;
    if (drawable_private->destroyed || src_private->destroyed)
        return;

    gc_private = (GdkGCPrivate*) gc;

    if (width == -1) width = src_private->width;
    if (height == -1) height = src_private->height;

    XCopyPlane( drawable_private->xdisplay,
                src_private->xwindow,
                drawable_private->xwindow,
                gc_private->xgc,
                xsrc, ysrc,
                width, height,
                xdest, ydest,
                1 );
}

#endif
