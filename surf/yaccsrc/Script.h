/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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

#ifdef HAVE_CONIFG_H
#  include <config.h>
#endif

class RgbBuffer;
class bit_buffer;
class Triangulator;
class SymbolTable;

#include <float_buffer.h>

class Script
{
public:
	static void init(bool quiet);
	static void deinit();

	static char* readFile(const char* name);
	static void executeScriptFromStdin();
	static void executeScriptFromFile(const char* name);

	static bool isQuiet() { return quiet; }
	static bool stdout_is_tty() { return stdout_is_a_tty; }

	static RgbBuffer* getBuffer() { return buffer; }
	static void setBuffer(RgbBuffer* _buffer) { buffer = _buffer; }

	static bit_buffer* getBitBuffer() { return bitbuffer; }

//#ifdef HAVE_LIBGTS
//	static Triangulator* getTriangulator() { return tritor; }
//#endif

	static float_buffer* getZBuffer() { return zbuffer; }
	static float_buffer* getZBuffer3d() {
		if(!zbuffer3d) {
			zbuffer3d = new float_buffer();
		}
		return zbuffer3d;
	}

	static void setRootFinderStatics();
	static void checkVariables();

	static SymbolTable& getDefaultValues();

	static void stop() {
		stop_flag = true;
	}
	static bool isStopped() {
		return stop_flag;
	}
	static bool isKernelMode() {
		return kernel_mode;
	}

private:
	static bool quiet;
	static bool stdin_is_a_tty;
	static bool stdout_is_a_tty;
	
	static bool stop_flag;
	static bool kernel_mode;

	static RgbBuffer* buffer;
	static bit_buffer* bitbuffer;
	static float_buffer* zbuffer;
	static float_buffer* zbuffer3d;
//#ifdef HAVE_LIBGTS
//	static Triangulator* tritor;
//#endif

	static SymbolTable* defaultValues;

	static void addNewCommands();

	static void beforeScriptExecution();
	static void internalExecuteScript(const char* str);

	// Commands....
	static void setKernelMode();

	static void setSize();
	static void drawCurve();
	static void drawSurface();
	static void saveColorImage();

	static void cutWithPlane();
	static void cutWithSurface();
	static void computeResultant();
	
	static void clearScreen();
	static void saveDitheredImage();
//	static void save3DImage();
	static void ditherSurface();
	static void ditherCurve();
	static void reset();
//	static void triangulateSurface();
	static void kernelModeSet();
	static void printDefaults();
	static void printColorImageFormats();
	static void printDitherImageFormats();
//	static void print3DImageFormats();
	static void printPosition();
};

#endif //!SCRIPT_H
