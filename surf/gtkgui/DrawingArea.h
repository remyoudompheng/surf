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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */

#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <gtk/gtk.h>
#include <gdk/gdkprivate.h>
#include <X11/Xlib.h>


class bit_buffer;
class RgbBuffer;
class ColorWindow;

class DrawingArea
{
private:
	DrawingArea (const DrawingArea &);
	void operator = (const DrawingArea &);
public:
	DrawingArea(bool colored);
	~DrawingArea();

	GtkWidget * getGtkDrawingArea() {return gtkdrawingArea;}

	void setSize (int w, int h);
	void drawRectangle (int x, int y, int w, int h, float r, float g, float b);
	void drawPoint (int x, int y, float r, float g, float b);
	void draw (int x, int y, int w, int h); // on screen 

	void drawPoint (int x, int y, bool black=true);
	void drawBitbuffer (bit_buffer &bb);
	void drawRgbBuffer (RgbBuffer &rgbbuffer);
	void drawCurve (RgbBuffer &rgbbuffer, int curve_r, int curve_g, int curve_b);

	void show();

	void lock()
		{ locks++; }
	void unlock ()
		{locks--;};
	bool isLocked()
		{return locks > 0;};

	void setWindowToShow(ColorWindow *cw)
		{windowToShow = cw;}
protected:
	int locks;
	GtkWidget *gtkdrawingArea;
	GdkPixmap *gdkpixmap;
	GdkGC *gdkgc;

	GdkGC *bitmap_set_gc;
	GdkGC *bitmap_unset_gc;

	Display *xdisplay;
 	GC xgc;
 	Pixmap xpixmap;

	class ColorWindow *windowToShow;

	int width;
	int height;
	bool isColored;

	static void handle_configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
	static void handle_expose (GtkWidget *widget, GdkEventExpose *event, gpointer data);
	static void handle_realize (GtkWidget *widget, gpointer data);
};
#endif
