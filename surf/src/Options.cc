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

#ifndef NO_GUI

#include <unistd.h>

#ifdef HAVE_GETOPT_LONG
# define _GNU_SOURCE_H
# include <getopt.h>
#endif

#include <gtk/gtk.h>

namespace {
#ifdef HAVE_GETOPT_LONG
char usage_text[] = "Usage: surf -n | --no-gui FILE...\n"
                    "       surf [GTK-OPTIONS] [-x | --exec] [-p | --progress-dialog]\n"
                    "            [-a | --auto-resize] [FILE]...\n"
                    "       surf -h | --help\n"
                    "\n"
		    "-n, --no-gui           disable GUI, execute all scripts passed as FILEs\n"
		    "-x, --exec             when running with GUI, execute first script immediately\n"
                    "-p, --progress-dialog  pop up a dialog instead of using a status bar\n"
                    "-a, --auto-resize      resize image windows automatically to image size\n"
                    "-h, --help             display this help and exit\n\n";
#else
char usage_text[] = "Usage: surf -n FILE...\n"
                    "       surf [GTK-OPTIONS] -xpa [FILE]...\n"
                    "       surf -h\n"
                    "\n"
		    "-n  disable GUI, execute all scripts passed as FILEs\n"
		    "-x  when running with GUI, execute first script immediately\n"
                    "-p  pop up a dialog instead of using a status bar\n"
                    "-a  resize image windows automatically to image size\n"
                    "-h  display this help and exit\n\n";
#endif
};

#endif // !NO_GUI

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
		int c;

#ifdef HAVE_GETOPT_LONG

		option longopts[] = {
                        { "help", no_argument, 0, 'h' },
			{ "exec", no_argument, 0, 'x' },
                        { "progress-dialog", no_argument, 0, 'p' },
                        { "auto-resize", no_argument, 0, 'a' },
			{ "no-gui", no_argument, 0, 'n' }
		};
		while((c = getopt_long(argc, argv, "hxpan", longopts, 0)) != -1) {

#else

		while((c = getopt(argc, argv, "hxpan")) != -1) {

#endif // HAVE_GETOPT_LONG

			switch(c) {
			case 'h':
				std::cerr << usage_text;
				exit(0);
				break;
			case 'x':
				execute = true;
				break;
			case 'p':
				progress_dialog = true;
				break;
			case 'a':
				auto_resize = true;
				break;
			case 'n':
#ifdef HAVE_GETOPT_LONG
				std::cerr << "Error: -n|--no-gui must be the only option\n\n"
					  << usage_text;
#else
				std::cerr << "Error: -n must be the only option\n\n"
					  << usage_text;
#endif
				exit(1);
				break;
			}
		}
		fileopts = optind;
	}
#else
        no_gui = true;
#endif // !NO_GUI
}
