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

#include "Misc.h"

#ifndef NO_GUI
#include "Requester.h"
#include "GuiThread.h"
#endif



bool Misc::haveGUI ()
{
#ifdef NO_GUI
	return false;
#else
	return GuiThread::haveGUI();
#endif
}


void Misc::alert (const char * str)
{
#ifdef NO_GUI
	cerr << "ALERT: " << str << endl;
#else
	if (haveGUI()) {
		showAlert (str);
	} else {
		cerr << "ALERT: " << str << endl;
	}
#endif
}

void Misc::alert (ostrstream &ostr)
{
	ostr << ends;
	alert(ostr.str());
	//	ostr.freeze(0);
}

void Misc::setDoing (const char *str, double val)
{
	
}

void Misc::setDone (double done)
{
	cout << done << "       " << "\r";
}
