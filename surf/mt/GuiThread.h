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


#ifndef GUITHREAD_H
#define GUITHREAD_H

#ifdef NO_GUI
class GuiThread
{
public:
	static bool haveGUI()
		{return false;};
	
//  	static void wakeup()
//  		{}
//  	static void addCommand (void (*comm)(void*), void *arg)
//  		{}
  	static void executeCommands()
  		{}
	
//  	static bool isGuiThread ()
//  		{
//  			return false;
//  		}
};
#else

#include <pthread.h>

struct Command
{
	void (*comm) (void *);
	void *arg;
};


class GuiThread
{
public:
	static bool haveGUI();
	static void wakeup();
	static void addCommand (void (*comm)(void*), void *arg);	
	static void executeCommands();
	
	static bool isGuiThread ();

	/* These are used by the thread who controls the gui */
	static void init();
	static void deinit();

	static bool getNextCommand (Command &invoc);
	static int getFileDescriptor();
	static void readAll();
	static void dmess();
};
#endif
#endif
