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



#ifndef VIRTUALDISPLAY_H
#define VIRTUALDISPLAY_H


#include "Thread.h"
#include "RgbBuffer.h"
#include "gui_config.h"

#ifdef NO_GUI
class bit_buffer;
class TSDrawingArea 
{
private:
	TSDrawingArea (const TSDrawingArea &);
	void operator=(const TSDrawingArea &);
	
public:
	TSDrawingArea ()	{};
	~TSDrawingArea()	{};

	void drawSquare (int x, int y, int size, float r, float g, float b)
		{}
	void drawRSquare (int x, int y, int size, float r, float g, float b)
		{}
	void drawGBSquare (int x, int y, int size, float r, float g, float b)
		{}
	
	void displayRectangle (int x, int y, int width, int height)
		{}
	void setSize(int width, int height)
		{}

	void drawBitbuffer (bit_buffer &bb)
		{}
	void drawRgbBuffer (RgbBuffer &rgbbuffer, bool onlyCurve=false, int curve_red=0, int curve_green=0, int curve_blue=0)
		{}
	void showColorAreaWindow()
	        {}
	void showDitherAreaWindow()
		{}
	
protected:
};

#else

#include "DrawingArea.h"


#define MAXSQUARES 200*200


class TSDrawingArea 
{
private:
	TSDrawingArea (const TSDrawingArea &);
	void operator=(const TSDrawingArea &);
	
public:
	TSDrawingArea () {numSquares=0;sda=0;};
	~TSDrawingArea();

	void drawSquare (int x, int y, int size, float r, float g, float b);
	void drawRSquare (int x, int y, int size, float r, float g, float b);
	void drawGBSquare (int x, int y, int size, float r, float g, float b);
	
	void displayRectangle (int x, int y, int width, int height);
	void setSize(int width, int height);

	void drawBitbuffer (bit_buffer &bb);
	void drawRgbBuffer (RgbBuffer &rgbbuffer, bool onlyCurve=false, int curve_red=0, int curve_green=0, int curve_blue=0);


	void setColorArea (DrawingArea *da) 
		{
			da->lock();
			sda = da;
		}

	void setDitherArea (DrawingArea *da)
		{
			da->lock();
			ditherArea = da;
		}
	
	void showColorAreaWindow();
	void showDitherAreaWindow();

	class DrawingArea *sda;
	class DrawingArea *ditherArea;

protected:
	Mutex mutex;
	struct {
		int x;
		int y;
		int size;
		float r;
		float g;
		float b; 
	} squares[MAXSQUARES];
	int numSquares;


protected:

	static void _gui_drawSquares(void *);
	void gui_drawSquares();
	
	static void _gui_displayRectangle (void *ptr);
	void gui_displayRectangle (struct DisplayRectStruct *rect);


	static void _gui_setSize (void *ptr);

	static void _gui_drawBitbuffer (void *ptr);
	static void _gui_drawRgbBuffer (void *ptr);
	
	static void _gui_showAreaWindow(void *ptr);
};

#endif
#endif
