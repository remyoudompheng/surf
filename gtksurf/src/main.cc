/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_GETOPT_LONG
# define _GNU_SOURCE_H
# include <getopt.h>
#else
#define getopt_long(argc, argv, optstring, longopts, longindex) \
        getopt(argc, argv, optstring)
#endif

#include<iostream>

#include <ScriptWindow.h>
#include <Kernel.h>
#include <Misc.h>

#define KERNEL_BINARY BINDIR "/surf"

#define GLADEFILE PKGDATADIR "/gtksurf.glade"

namespace {
char usage_text[] =
#ifdef HAVE_GETOPT_LONG
"Usage: gtksurf [OPTION]... [FILE]\n"
"\n"
"  -k, --kernel=PATHTOKERNEL   load PATHTOKERNEL as surf kernel instead of\n"
"                               the default " KERNEL_BINARY "\n"
"  -g, --glade-file=GLADEFILE  use GLADEFILE as Glade GUI description file\n"
"                               instead of " GLADEFILE "\n"
"  -h, --help                  display this help and exit\n"
"  -V, --version               output version information and exit\n"
"\n";
#else
"Usage: gtksurf [OPTION]... [FILE]\n"
"\n"
"  -k   load PATHTOKERNEL as surf kernel instead of the\n"
"         default " KERNEL_BINARY "\n"
"  -g   use PATHTOGLADEFILE as GUI description file\n"
"         instead of " GLADEFILE "\n"
"  -h   display this help and exit\n"
"  -V   output version information and exit\n"
"\n";
#endif // HAVE_GETOPT_LONG
}

int main(int argc, char* argv[])
{
	// parse options:
	option longopts[] = {
		{ "kernel", required_argument, 0, 'k' },
		{ "glade-file", required_argument, 0, 'g' },
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'V' },
                { 0, 0, 0, 0 }
	};

	// default values:
	std::string kernel_path = KERNEL_BINARY;
	std::string gladefile = GLADEFILE;

	int c;
	while((c = getopt_long(argc, argv, "k:g:hV", longopts, 0)) != -1) {

		switch(c) {
		case 'k':
			std::cout << "Using " << optarg << " as kernel.\n";
			kernel_path.assign(optarg);
			break;
		case 'g':
			std::cout << "Using " << optarg
				  << " as GUI description file.\n";
			gladefile.assign(optarg);
			break;
		case 'h':
			std::cout << usage_text;
			exit(0);
			break;
		case 'V':
			std::cout <<
"\n"
PACKAGE " version " VERSION "\n"
"\n"
"Visualizing algebraic curves and algebraic surfaces\n"
"\n"
"Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet Erlangen-Nuernberg,\n"
"              1997-2001 Johannes Gutenberg-Universitaet Mainz.\n"
"\n"
"Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel, Ralf Schmitt,\n"
"Kai Schneider and Johannes Beigel.\n"
"\n"
"This program is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation; either version 2 of the License, or\n"
"(at your option) any later version.\n"
"\n"
"For reporting bugs or getting news about latest developments,\n"
"please visit our homepage at http://surf.sourceforge.net/\n"
"\n";

			exit(0);
			break;
		default:
			std::cerr << "\nUnknown option!\n\n";
			std::cerr << usage_text;
			exit(1);
		}
	}

	Kernel kernel(kernel_path);

	Glade glade(argc, argv, gladefile);

	ScriptWindow script(glade, kernel);

	if(optind < argc) {
		script.load_file(argv[optind]);
	}
	
	glade.main_loop();
	
	return 0;
}
