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



#ifndef EXECUTESCRIPTSTRUCT
#define EXECUTESCRIPTSTRUCT

#include "Preview.h"
class TSDrawingArea;
class Thread;

struct ExecuteScriptStruct
{
	ExecuteScriptStruct();
	~ExecuteScriptStruct ();

	void setPreview (const bool arr[4]);

	void executeBeforeUserScript (const char *script);
	void setUserScript (const char *script, bool executeCommands=true);
	void executeAfterUserScript(const char *script);

	void callWhenDone (void (*cb) (ExecuteScriptStruct *))
		{ doneCallback = cb; }
	void setDrawingArea (TSDrawingArea *area)
		{ drawingArea = area; };

	
	Thread *getThread()		{ return thread;	}
	const char *getErrorString()	{ return errorString;	}
	int getErrorBegin()		{ return error_begin-1;	}
	int getErrorEnd()		{ return error_end-1;	}
	bool errorOccured()		{ return parse_result!=0;	}

	friend class Script;

	void *userData;
	void *userData2;

protected:
	bool executeUserScriptCommands;
	Thread *thread;
	TSDrawingArea *drawingArea;

	const char *firstPart;
	const char *secondPart;
	const char *thirdPart;
	
	int parse_result;
	int error_begin;
	int error_end;
	
	const char *errorString;

	void (*doneCallback) (ExecuteScriptStruct *);
	Preview preview;
};

#endif
