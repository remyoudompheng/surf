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



#ifndef SCRIPT_H
#define SCRIPT_H


class TSDrawingArea;
class RgbBuffer;
class bit_buffer;
class float_buffer;
class SymbolTable;

#include "TSDrawingArea.h"
#include "ExecuteScriptStruct.h"
#include "Preview.h"
#include "float_buffer.h"

class Script
{
public:
	// returns true if Script can be executed (that means no other thread is
	// executing an script), false otherwise
	static bool startScriptExecution(ExecuteScriptStruct *ess);

	// thread execution starts here
	static void *startThread (void *data);

	static void init();
	static void deinit();

	static char *readFile (const char *name);
	static void executeScriptFromFile (const char *name);

	static TSDrawingArea *getDisplay();
	static void setDisplay (TSDrawingArea *display);
	
	static RgbBuffer *getBuffer() {return buffer;};
	static void setBuffer (RgbBuffer *_buffer) {buffer = _buffer;};

	static bit_buffer *getBitBuffer() {return bitbuffer;};

	static float_buffer *getZBuffer() {return zbuffer;};
	static float_buffer *getZBuffer3d() 
		{
			if (!zbuffer3d) {
				zbuffer3d = new float_buffer();
			}
			return zbuffer3d;
		}

	static void setRootFinderStatics ();
	static void checkVariables();
	// this one checks if the variables are out of range (e.g. epsilon <= 0)
	// 

	static /*const*/ SymbolTable & getDefaultValues();

	static const Preview &getPreview()
		{ return preview;}

protected:
	static TSDrawingArea *display;
	static RgbBuffer *buffer;
	static bit_buffer *bitbuffer;
	static float_buffer *zbuffer;
	static float_buffer *zbuffer3d;

	static SymbolTable *defaultValues;
	static Preview preview;

protected:
	static void addNewCommands();


private:
	static void beforeScriptExecution();
	static int internalExecuteScript (const char *str, bool runCommands=true);

protected:
	// Commands....
	static void setSize();
	static void drawSurface();
	static void saveColorImage();

	static void cutWithSurface();
	static void computeResultant();
	
	static void clearScreen();
	static void saveDitheredImage();
	static void ditherSurface();
	static void ditherCurve();
	static void clearPixmap();
};

#endif
