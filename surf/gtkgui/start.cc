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

#include "mygtk.h"
#include "GuiThread.h"
#include "mycolor.h"
#include "MainWindowController.h"
#include "Document.h"
#include "Options.h"

void new_input(void *data, int source, GdkInputCondition cond)
{
	GuiThread::readAll();
	struct Command comm;
	int count=0;
	while (GuiThread::getNextCommand(comm)) {
		comm.comm (comm.arg);
		count++;
	}
}

gint after_gui_creation(gpointer data)
{
	MainWindowController* mwc = (MainWindowController*)data;
	bool doc_loaded = false;
	int argc = options->get_argc();
	char** argv = options->get_argv();
	
	for(int i = options->getFirstFileIdx(); i < argc; i++) {
		Document* doc = Document::loadDocument(argv[i]);
		if(doc) {
			if(!doc_loaded) {
				mwc->editDocument(doc);	
				doc_loaded = true;
			}
		}
	}
	
	if (doc_loaded && options->get_exec()) {
		mwc->executeScript();
	} else {
		mwc->show();
	}

	return false;
}

void start_gtk_main()
{
	GuiThread::init();
	gdk_input_add(GuiThread::getFileDescriptor(), GDK_INPUT_READ, new_input, 0);
	initColors();
	MainWindowController *mainWindowController = new MainWindowController();
	gtk_idle_add(after_gui_creation, mainWindowController);

	gtk_main();

	delete mainWindowController;
	Document::freeAllDocuments();
	GuiThread::deinit();
}
