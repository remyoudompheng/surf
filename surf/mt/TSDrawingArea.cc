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



#ifndef NO_GUI

#include <assert.h>

#include "TSDrawingArea.h"
#include "GuiThread.h"
#include "DrawingArea.h"

// #define DEBUG
#include "debug.h"


struct DrawRgbBufferStruct
{
	class TSDrawingArea *tsda;
	class RgbBuffer *rgbbuffer;
	bool onlyCurve;
	int curve_red;
	int curve_green;
	int curve_blue;
};

struct DrawBitbufferStruct
{
	class TSDrawingArea *tsda;
	bit_buffer *bb;
};

struct NewSize
{
	class TSDrawingArea *tsda;
	int width;
	int height;
};

struct DisplayRectStruct {
	TSDrawingArea *tsda;
	int x;
	int y;
	int width;
	int height;
};


TSDrawingArea::~TSDrawingArea()
{
	if (ditherArea)
		ditherArea->unlock();
	if (sda)
		sda->unlock();
}

void TSDrawingArea::_gui_setSize (void *ptr)
{
	BEGIN("TSDrawingArea::_gui_setSize");
	NewSize *ns = (NewSize *) ptr;
	ns->tsda->sda->setSize(ns->width, ns->height);
	delete ns;
}

void TSDrawingArea::drawRgbBuffer (RgbBuffer &rgbbuffer, bool onlyCurve, int curve_red, int curve_green, int curve_blue)
{
	DrawRgbBufferStruct drbs;
	drbs.tsda = this;
	drbs.rgbbuffer = &rgbbuffer;
	drbs.onlyCurve=onlyCurve;
	drbs.curve_red = curve_red;
	drbs.curve_green = curve_green;
	drbs.curve_blue = curve_blue;
	GuiThread::addCommand (_gui_drawRgbBuffer, &drbs);
	GuiThread::executeCommands();
}

void TSDrawingArea::_gui_showAreaWindow(void *ptr)
{
	DrawingArea *da = (DrawingArea *) ptr;
	da->show();
}

void TSDrawingArea::showColorAreaWindow()
{
	if (sda) {
		GuiThread::addCommand (_gui_showAreaWindow, sda);
		GuiThread::executeCommands();
	}
}

void TSDrawingArea::showDitherAreaWindow()
{
	if (ditherArea) {
		GuiThread::addCommand (_gui_showAreaWindow, ditherArea);
		GuiThread::executeCommands();
	}
}

void TSDrawingArea::_gui_drawRgbBuffer (void *ptr)
{
	DrawRgbBufferStruct *drbs = (DrawRgbBufferStruct *) ptr;
	if (drbs->onlyCurve) {
		drbs->tsda->gui_drawSquares();
		drbs->tsda->sda->drawCurve (*drbs->rgbbuffer, drbs->curve_red, drbs->curve_green, drbs->curve_blue);
	} else {
		drbs->tsda->gui_drawSquares();
		drbs->tsda->sda->drawRgbBuffer (*drbs->rgbbuffer);
	}
}

void TSDrawingArea::drawBitbuffer (bit_buffer &bb)
{
	DrawBitbufferStruct dbs;
	dbs.tsda = this;
	dbs.bb = &bb;
	GuiThread::addCommand (_gui_drawBitbuffer, &dbs);
	GuiThread::executeCommands();
}

void TSDrawingArea::_gui_drawBitbuffer (void * ptr)
{
	DrawBitbufferStruct *dbs = (DrawBitbufferStruct *) ptr;
	dbs->tsda->ditherArea->drawBitbuffer (*dbs->bb);
}

void TSDrawingArea::setSize (int width, int height)
{
	BEGIN("TSDrawingArea::setSize");
	NewSize *ns = new NewSize();
	ns->tsda = this;
	ns->width = width;
	ns->height = height;
	GuiThread::addCommand(_gui_setSize, ns);
	GuiThread::wakeup();
}

void TSDrawingArea::_gui_drawSquares (void *ptr)
{
	TSDrawingArea *tsda = (TSDrawingArea *) ptr;
	tsda->gui_drawSquares();
}


void TSDrawingArea::gui_drawSquares()
{
	mutex.lock();
	int i;
	for (i=0; i<numSquares; i++) {
		sda->drawRectangle (squares[i].x, squares[i].y, squares[i].size, squares[i].size,
				    squares[i].r, squares[i].g, squares[i].b);
	}
	numSquares=0;
	mutex.unlock();
}

void TSDrawingArea::drawSquare (int x, int y, int size, float r, float g, float b)
{
	mutex.lock();
	if (numSquares == MAXSQUARES-1) {
// 		cout << "squares full..." << endl;
		// here we have to unlock, because the gui thread will 
		// try to lock this mutex when it calls "draw(this)"
		mutex.unlock();


		GuiThread::addCommand (_gui_drawSquares, this);
		GuiThread::executeCommands();
		mutex.lock();
	}
	
	assert(numSquares < MAXSQUARES-1);
	
	squares[numSquares].x = x;
	squares[numSquares].y = y;
	squares[numSquares].size = size;
	squares[numSquares].r = r;
	squares[numSquares].g = g;
	squares[numSquares].b = b;

	numSquares++;

	mutex.unlock();
}

void TSDrawingArea::drawRSquare (int x, int y, int size, float r, float g, float b)
{
	mutex.lock();
	if (numSquares == MAXSQUARES-1) {
// 		cout << "squares full..." << endl;
		// here we have to unlock, because the gui thread will 
		// try to lock this mutex when it calls "draw(this)"
		mutex.unlock();


		GuiThread::addCommand (_gui_drawSquares, this);
		GuiThread::executeCommands();
		mutex.lock();
	}
	
	assert(numSquares < MAXSQUARES-1);
	
	squares[numSquares].x = x;
	squares[numSquares].y = y;
	squares[numSquares].size = size;
	squares[numSquares].r = r;

	numSquares++;

	mutex.unlock();
}

void TSDrawingArea::drawGBSquare (int x, int y, int size, float r, float g, float b)
{
	mutex.lock();
	if (numSquares == MAXSQUARES-1) {
// 		cout << "squares full..." << endl;
		// here we have to unlock, because the gui thread will 
		// try to lock this mutex when it calls "draw(this)"
		mutex.unlock();


		GuiThread::addCommand (_gui_drawSquares, this);
		GuiThread::executeCommands();
		mutex.lock();
	}
	
	assert(numSquares < MAXSQUARES-1);
	
	squares[numSquares].x = x;
	squares[numSquares].y = y;
	squares[numSquares].size = size;
	squares[numSquares].g = g;
	squares[numSquares].b = b;

	numSquares++;

	mutex.unlock();
}

void TSDrawingArea::displayRectangle (int x, int y, int width, int height)
{
	// First flush buffer
	DisplayRectStruct *drs = new DisplayRectStruct();
	drs->x=x;
	drs->y=y;
	drs->width=width;
	drs->height=height;
	drs->tsda=this;
	GuiThread::addCommand (_gui_drawSquares, this);
	GuiThread::addCommand (_gui_displayRectangle, drs);
	GuiThread::wakeup();
	// GuiThread::executeCommands();
}

void TSDrawingArea::_gui_displayRectangle (void *ptr)
{
	DisplayRectStruct *drs = (DisplayRectStruct *) ptr;
	drs->tsda->sda->draw(drs->x, drs->y, drs->width, drs->height);
	delete drs;
}
#endif
