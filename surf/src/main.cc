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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */


#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <gtk/gtk.h>

#include "Script.h"
#include "welcomeMessage.h"
#include "compfn.h"

#ifndef NO_GUI
char usage_text[] = "Usage: surf -n | --nogui FILE...\n"
                    "       surf [GTK-OPTIONS] [-x | --exec] [FILE]...\n"
                    "       surf --help\n"
                    "\n"
		    "-n, --no-gui  disable GUI, execute all scripts passed as FILEs\n"
		    "-x, --exec    when running with GUI, execute first script immediately\n"
                    "    --help    display this help and exit\n\n";
#endif

extern void start_gtk_main (int argc, char **argv, int fileopts, bool execute);

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

int main (int argc, char** argv)
{
	signal (SIGPIPE, SIG_IGN);
#ifdef DEBUG
	yydebug = 1;
#endif

	bool execute = false;
	int fileopts = 1;

#ifndef NO_GUI
	bool nogui = false;

	if (argc > 1 &&
	    (strcmp(argv[1], "-n") == 0 || strcmp(argv[1], "--no-gui") == 0)) {
		nogui = true;
		fileopts = 2;
	} else {
		nogui = false;
		
		gtk_init(&argc, &argv);
		
		int i;
		for (i = 1; i < argc; ++i) {
			if (argv[i][0] != '-') {
				break;
			}
			
			++fileopts;
			
			if (strcmp(argv[i], "-x") == 0 ||
			    strcmp(argv[i], "--exec") == 0) {
				execute = true;
			} else if (strcmp(argv[i], "--help") == 0) {
				std::cerr << usage_text;
				exit(0);
			} else if (strcmp(argv[i], "-n") == 0 ||
				   strcmp(argv[i], "--no-gui") == 0) {
				std::cerr << "Error: \'" << argv[i] << "\' must be the only option\n\n"
					  << usage_text;
				exit(1);
			} else {
				std::cerr << "Error: unknown option \'" << argv[i] << "\'\n\n"
					  << usage_text;
				exit(1);
			}
		}
		fileopts = i;
	}
#else
	bool nogui = true;
#endif

	std::cout << welcomeMessage;

	init_all();

	if (nogui) {
		for (int i = fileopts; i < argc; ++i) {
			std::cerr << "************** executing " << argv[i] << std::endl;
			Script::executeScriptFromFile(argv[i]);
		}
	} else {
		start_gtk_main (argc, argv, fileopts, execute);
	}
	
	deinit_all();
	return 0;
}
