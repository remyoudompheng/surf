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

#include "Options.h"
#include<iostream>
#include <string.h>

#ifndef NO_GUI

#include <gtk/gtk.h>

namespace {
char usage_text[] = "Usage: surf -n | --no-gui FILE...\n"
                    "       surf [GTK-OPTIONS] [-x | --exec] [--progress-dialog]\n"
                    "            [--auto-resize] [FILE]...\n"
                    "       surf --help\n"
                    "\n"
		    "-n, --no-gui           disable GUI, execute all scripts passed as FILEs\n"
		    "-x, --exec             when running with GUI, execute first script immediately\n"
                    "    --progress-dialog  pop up a dialog instead of using a status bar\n"
                    "    --auto-resize      resize image windows automatically to image size\n"
                    "    --help             display this help and exit\n\n";
};

#endif

Options::Options(int _argc, char* _argv[])
	: argc(_argc), argv(_argv),
	  fileopts(1), no_gui(false), execute(false),
	  progress_dialog(false), auto_resize(false)
{
	if(argc > 1 &&
	   (strcmp(argv[1], "-n") == 0 || strcmp(argv[1], "--no-gui") == 0)) {
		no_gui = true;
		fileopts = 2;
	}
#ifndef NO_GUI
          else {
		gtk_init(&argc, &argv);
		
		int i;
		for(i = 1; i < argc; ++i) {
			if(argv[i][0] != '-') {
				break;
			}
			
			++fileopts;
			
			if(strcmp(argv[i], "-x") == 0 ||
			    strcmp(argv[i], "--exec") == 0) {
				execute = true;
			} else if(strcmp(argv[i], "--help") == 0) {
				cerr << usage_text;
				exit(0);
			} else if(strcmp(argv[i], "--progress-dialog") == 0) {
				progress_dialog = true;
			} else if(strcmp(argv[i], "--auto-resize") == 0) {
				auto_resize = true;
			} else if(strcmp(argv[i], "-n") == 0 ||
				  strcmp(argv[i], "--no-gui") == 0) {
				cerr << "Error: \'" << argv[i] << "\' must be the only option\n\n"
					  << usage_text;
				exit(1);
			} else {
				cerr << "Error: unknown option \'" << argv[i] << "\'\n\n"
					  << usage_text;
				exit(1);
			}
		}
		fileopts = i;
	}
#else
        no_gui = true;
#endif
}
