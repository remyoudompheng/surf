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



#include <iostream.h>

#include <gdk/gdkx.h>

#include "mycolor.h"
#include "color.h"
#include "DrawingArea.h"
#include "RGBToStandard.h"
#include "RgbBuffer.h"

// #define DEBUG
#include "debug.h"
#include "defs.h"
#include "bit_buffer.h"
#include "RGBToNetscape.h"
#include "ColorWindow.h"


extern XStandardColormap *xstandardColormap;


DrawingArea::DrawingArea(bool colored)
{
	width = 200;
	height = 200;

	locks = 1;
	isColored = colored;
	gtkdrawingArea = gtk_drawing_area_new();

	gtk_widget_set_usize(gtkdrawingArea, width, height);

	gtk_signal_connect (GTK_OBJECT(gtkdrawingArea), "configure_event", GTK_SIGNAL_FUNC(handle_configure), this);
	gtk_signal_connect (GTK_OBJECT(gtkdrawingArea), "expose_event",    GTK_SIGNAL_FUNC(handle_expose),    this);
	gtk_signal_connect (GTK_OBJECT(gtkdrawingArea), "realize",         GTK_SIGNAL_FUNC(handle_realize),   this);
}

DrawingArea::~DrawingArea()
{
}

void DrawingArea::show()
{
	if (windowToShow) {
		windowToShow->show();
	}
}

void DrawingArea::setSize (int w, int h)
{
	if (width == w && height == h)
		return;

	width=w;
	height=h;

	gtk_widget_set_usize(gtkdrawingArea, width, height);
	if (gdkpixmap) {
		gdk_pixmap_unref(gdkpixmap);

		gdkpixmap = gdk_pixmap_new(gtkdrawingArea->window, width, height, isColored ? -1 : 1);
		xpixmap = ((GdkPixmapPrivate *)gdkpixmap)->xwindow;
	}
	
}



void DrawingArea::drawRectangle (int x, int y, int w, int h, float r, float g, float b)
{

	switch (myColorMode) {

	case COLOR_NETSCAPE:
		XSetForeground (xdisplay, xgc, pixels[RGBToNetscape(r,g, b, x, y)]);
		break;
		
	case COLOR_TRUE:
		XSetForeground (xdisplay, xgc, RGBToStandard(r,g,b, xstandardColormap));
		break;
	}
	
	XFillRectangle (xdisplay, xpixmap, xgc, x, y, w, h); 
}

void DrawingArea::drawPoint (int x, int y, float r, float g, float b)
{
	switch (myColorMode) {

	case COLOR_NETSCAPE:
		XSetForeground (xdisplay, xgc, pixels[RGBToNetscape(r,g, b, x, y)]);
		break;

	case COLOR_TRUE:
		XSetForeground (xdisplay, xgc, RGBToStandard(r,g,b, xstandardColormap));
		break;
	}

	XDrawPoint (xdisplay, xpixmap, xgc, x,y);
}

void DrawingArea::draw (int x, int y, int w, int h)
{
	GdkRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.width = w;
	rect.height = h;
	gtk_widget_draw (gtkdrawingArea, &rect);
// 	if (isColored) {
// 		XCopyArea (xdisplay, xpixmap, ((GdkWindowPrivate *)GTK_WIDGET(gtkdrawingArea)->window)->xwindow, xgc, x, y, w, h, x, y);
// 		XFlush(xdisplay);
// 	} else {
// 		XCopyPlane (xdisplay, xpixmap, GDK_WINDOW_XWINDOW(gtkdrawingArea->window), xgc,
// 			    x, y, w, h,
// 			    x,y,1);
// 	}
}

void DrawingArea::drawPoint (int x, int y, bool set)
{
	gdk_draw_point (gdkpixmap, set ? bitmap_set_gc : bitmap_unset_gc, x, y);
}

void DrawingArea::drawBitbuffer (bit_buffer &bb)
{
	setSize (bb.getWidth(), bb.getHeight());
	// XClearArea (xdisplay, xpixmap, 0,0,width,height, FALSE);
	int x;
	for (x=0; x<width; x++) {
		int y;
		for (y=0; y<height; y++) {
			drawPoint (x,y, !bb.getPixel(x, y));
		}
	}
	draw (0, 0, width, height);
}

void DrawingArea::drawRgbBuffer (RgbBuffer &buffer)
{
	setSize (buffer.getWidth(), buffer.getHeight());
	int x,y;
	int i=0;
	for (y=0; y<buffer.height; y++) {
		for (x=0; x<buffer.width; x++,i++) {
			drawPoint(x,y,  
				  ((float)buffer.r[i])/255.0, 
				  ((float)buffer.g[i])/255.0,
				  ((float)buffer.b[i])/255.0);
		}
	}	
	draw(0,0,width,height);
}

void DrawingArea::drawCurve (RgbBuffer &rgbbuffer, int curve_r, int curve_g, int curve_b)
{
	int    r_tmp,Layer1r;
	int    g_tmp,Layer1g;
	int    b_tmp,Layer1b;
	float  rr,gg,bb;
	double Layer2;
	int    num = 0;
    

	for( int y = 0; y < rgbbuffer.height; y++ ) {    
		for( int x = 0; x < rgbbuffer.width; x++, num++ ) {
			if( rgbbuffer.GetTag(x,y,CURVEBIT) ) {            
				Layer1r = rgbbuffer.r[num] ; 
				Layer1g = rgbbuffer.g[num] ; 
				Layer1b = rgbbuffer.b[num] ;
				// get intensity from curve at this pixel
				Layer2  = (double)rgbbuffer.curv[num]/255.0;
            
				r_tmp=(int)( (Layer1r*(1-Layer2)) + ( curve_r*Layer2));	
				g_tmp=(int)( (Layer1g*(1-Layer2)) + ( curve_g*Layer2));	
				b_tmp=(int)( (Layer1b*(1-Layer2)) + ( curve_b*Layer2));	
                            
				rgbbuffer.r[num]=Layer1r = (r_tmp >255) ? 255 : r_tmp;
				rgbbuffer.g[num]=Layer1g = (g_tmp >255) ? 255 : g_tmp;
				rgbbuffer.b[num]=Layer1b = (b_tmp >255) ? 255 : b_tmp;
                        
				rgbbuffer.DelTag(x,y,CURVEBIT);
				rgbbuffer.SetTag(x,y,DATABIT);
              
				rr = Layer1r/255.0;
				gg = Layer1g/255.0;
				bb = Layer1b/255.0;
				drawPoint(x, y, rr, gg, bb);
			}
		}
	}    
	draw(0,0,width,height);
}

//
// --- GTK Callbacks
// 

void DrawingArea::handle_realize (GtkWidget *widget, gpointer data)
{
	DrawingArea *This = (DrawingArea *)data;
	
	This->gdkpixmap = gdk_pixmap_new(widget->window, This->width, This->height, This->isColored ? -1 : 1);
	This->gdkgc = gdk_gc_new (widget->window);
	gdk_gc_copy (This->gdkgc, widget->style->white_gc);
	
	// we need the next two lines to make the dither window work on 8 bit displays
  	gdk_gc_set_foreground(This->gdkgc, &widget->style->white);
  	gdk_gc_set_background(This->gdkgc, &widget->style->black);
	

	This->xdisplay = GDK_GC_XDISPLAY(This->gdkgc);
 	This->xpixmap = ((GdkPixmapPrivate *)This->gdkpixmap)->xwindow;
 	This->xgc = GDK_GC_XGC(This->gdkgc);

	if (This->isColored) {
		This->drawRectangle (0, 0, This->width, This->height, 0,0,0);
#if 0
		int i,j;
		for (i=0; i<200; i++)
			for (j=0; j<200; j++) {
				This->drawRectangle (i,j,1,1, (i % 100) / 200.0 * 2, j / 200.0, i*j/40000.0);
			}
#endif
	} else {
		GdkGCValues gcval;

		This->bitmap_set_gc = gdk_gc_new (This->gdkpixmap);
		gdk_gc_get_values (This->bitmap_set_gc, &gcval);
		This->bitmap_unset_gc = gdk_gc_new (This->gdkpixmap);
		gdk_gc_set_foreground (This->bitmap_unset_gc, &gcval.background);
		

		gdk_draw_rectangle (This->gdkpixmap, This->bitmap_unset_gc, TRUE, 0,0, This->width, This->height);
#if 0
		int i,j;
		for (i=0; i<200; i++) 
			for (j=0; j<200; j++)
				This->drawPoint(i,j, i==j || (i-100)*(i-100)+(j-100)*(j-100) < 100 );
#endif
	}
	
	// gdk_window_set_back_pixmap (This->gtkdrawingArea->window, This->gdkpixmap, 0);
}

void DrawingArea::handle_configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
}

void DrawingArea::handle_expose (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	DrawingArea *This = (DrawingArea *) data;
	



	if (This->isColored) {
		XCopyArea (This->xdisplay, This->xpixmap, GDK_WINDOW_XWINDOW(widget->window), This->xgc, 
			   event->area.x, event->area.y, event->area.width, event->area.height,
			   event->area.x, event->area.y);
	} else {
		XCopyPlane (This->xdisplay, This->xpixmap, GDK_WINDOW_XWINDOW(widget->window), This->xgc,
			    event->area.x, event->area.y, event->area.width, event->area.height,
			    event->area.x, event->area.y, 1);

// 		gdk_draw_bitmap (widget->window, This->gdkgc, This->gdkpixmap,
// 				 event->area.x, event->area.y,
// 				 event->area.x, event->area.y,
// 				 event->area.width, event->area.height);
	}

}
