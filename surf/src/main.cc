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
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "Script.h"
#include "welcomeMessage.h"
#include "compfn.h"
#include "Options.h"

extern void start_gtk_main();

Options* options;

static void init_all()
{
	init_pattern( );
	init_dot_diffusion( );
	// init_light();
	Script::init();
}

static void deinit_all()
{
	deinit_dot_diffusion();
	Script::deinit();
}

int main(int argc, char** argv)
{
	signal (SIGPIPE, SIG_IGN);
#ifdef DEBUG
	yydebug = 1;
#endif

	cout << welcomeMessage;

	options = new Options(argc, argv);

	init_all();

	if(options->get_no_gui()) {
		for(int i = options->getFirstFileIdx(); i < argc; ++i) {
			cerr << "************** executing " << argv[i] << endl;
			Script::executeScriptFromFile(argv[i]);
		}
	} else {
		start_gtk_main();
	}
	
	deinit_all();
	return 0;
}
