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
"\n"
"gtksurf is a graphical frontend to surf, the script driven visualization\n"
"tool for real algebraic geometry.\n"
"\n"
"Usage: gtksurf [OPTION]... [FILE]\n"
"\n"
#ifdef HAVE_GETOPT_LONG
"  -k, --kernel=PATHTOKERNEL   load PATHTOKERNEL as surf kernel instead of\n"
"                               the default " KERNEL_BINARY "\n"
"  -g, --glade-file=GLADEFILE  use GLADEFILE as Glade GUI description file\n"
"                               instead of " GLADEFILE "\n"
"  -h, --help                  display this help and exit\n"
"  -V, --version               output version information and exit\n"
#else
"  -k PATHTOKERNEL     load PATHTOKERNEL as surf kernel instead of the\n"
"                       default " KERNEL_BINARY "\n"
"  -g PATHTOGLADEFILE  use PATHTOGLADEFILE as GUI description file\n"
"                       instead of " GLADEFILE "\n"
"  -h                  display this help and exit\n"
"  -V                  output version information and exit\n"
#endif // HAVE_GETOPT_LONG
"\n"
"Report bugs to the SourceForge bug tracking system at\n"
"http://sourceforge.net/tracker/?group_id=3275\n";
}

int main(int argc, char* argv[])
{
	// parse options:
#ifdef HAVE_GETOPT_LONG
	option longopts[] = {
		{ "kernel", required_argument, 0, 'k' },
		{ "glade-file", required_argument, 0, 'g' },
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'V' },
                { 0, 0, 0, 0 }
	};
#endif

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
PACKAGE " " VERSION "\n"
"\n"
"Copyright (C) 2001 Johannes Beigel\n"
"\n"
"This is free software; you can redistribute it and/or modify it under the\n"
"terms of the GNU General Public License; either version 2 of the License,\n"
"or any later version.\n"
"\n"
"Written by Johannes Beigel.\n";

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
