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



#ifndef DEBUG_H
#define DEBUG_H

#include <iostream.h>

class FunctionCall
{
public:
	FunctionCall(const char *bla);
	~FunctionCall();
    
	FunctionCall();
    
protected:
	char str[256];
	int off;
};


extern void spaces();
extern class ostream &dos;
extern int offlevel;
#endif

#undef BEGIN
#undef DMESS
#undef TRACE
#undef FAIL
#undef OFF

// #undef DEBUG
#ifdef DEBUG
#define BEGIN(x) FunctionCall dummyFunctionCall(x)
#define DMESS(x) {if (offlevel == 0) {spaces(); dos << x << endl;}}
#define TRACE(x) {if (offlevel == 0) {spaces(); dos << "TRACE:" << #x << ":=" << x << ";" <<endl;}}
#define FAIL     {spaces(); dos << "Bye...\n" << endl; exit(10);};
#define OFF       FunctionCall dummyFunctionCallOff 
#else
#define BEGIN(x) {}
#define DMESS(x) {}
#define TRACE(x) {}
#define FAIL
#define OFF
#endif



