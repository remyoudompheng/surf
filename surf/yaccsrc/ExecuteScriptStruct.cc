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


#include <assert.h>

#include "ExecuteScriptStruct.h"
#include "TSDrawingArea.h"
#include "mystrdup.h"

void ExecuteScriptStruct::setPreview (const bool arr[4])
{
	int active=0;
	int i;
	for (i=0; i<4; i++) {
		preview[i] = arr[i];
		if (preview[i])
			active++;
	}
	assert(active != 0);
}

ExecuteScriptStruct::ExecuteScriptStruct()
{
	firstPart = secondPart = thirdPart = 0;
	doneCallback = 0;
	userData = 0;
	userData2 = 0;
	drawingArea = 0;
	errorString=0;
	executeUserScriptCommands = true;
	int i;
	for (i=0; i<4; i++)
		preview[i] = (i==0);
}

ExecuteScriptStruct::~ExecuteScriptStruct()
{
	delete firstPart;
	delete secondPart;
	delete thirdPart;
	delete drawingArea;
}

void ExecuteScriptStruct::executeBeforeUserScript(const char *script)
{
	delete firstPart;
	firstPart = mystrdup (script);
}

void ExecuteScriptStruct::setUserScript(const char *script, bool executeCommands)
{
	delete secondPart;
	secondPart = mystrdup(script);
	executeUserScriptCommands = executeCommands;
}

void ExecuteScriptStruct::executeAfterUserScript (const char *script)
{
	delete thirdPart;
	thirdPart = mystrdup(script);
}
