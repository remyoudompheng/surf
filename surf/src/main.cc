/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Script.h>
#include <IO.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_GETOPT_LONG
#  define _GNU_SOURCE_H
#  include <getopt.h>
#else
#define getopt_long(argc, argv, optstring, longopts, longindex) \
        getopt(argc, argv, optstring)
#endif

#include <signal.h>
#include <stdlib.h>

#include<iostream>
#include<cstring>


namespace {
char usage_text[] =
"\n"
"surf is a script driven visualization tool for real algebraic geometry.\n"
"\n"
"Usage: surf [OPTION]... [FILE]...\n"
"\n"
#ifdef HAVE_GETOPT_LONG
"  -q, --quiet      be quiet\n"
"  -h, --help       display this help and exit\n"
"  -V, --version    output version information and exit\n"
#else
"  -q    don't be chatty\n"
"  -h    display this help and exit\n"
"  -V    output version information and exit\n"
#endif
"\n"
"Report bugs to the SourceForge bug tracking system at\n"
"http://sourceforge.net/tracker/?group_id=3275\n";
}


void sig_handler(int sig)
{
	if(sig == SIGHUP || sig == SIGINT) {
		Script::stop();
	}
}

int main(int argc, char* argv[])
{
	// ignore SIGPIPE signals:
	struct sigaction sa;
	std::memset(static_cast<void*>(&sa), 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);

	// stop calculations on SIGHUP:
// ok, what's wrong w/these two lines!??
//	sa.sa_handler = sighup_handler;
//	sigaction(SIGTERM, &sa, 0);
	signal(SIGHUP, sig_handler);

	// do the same on C-C if we have a terminal as input
	if(isatty(STDIN_FILENO)) {
		signal(SIGINT, sig_handler);
	}

#ifdef DEBUG
	yydebug = 1;
#endif

	// parse options:
#ifdef HAVE_GETOPT_LONG
	option longopts[] = {
                { "quiet", no_argument, 0, 'q' },
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'V' },
		{ "kernel", no_argument, 0, 'k' },
                { 0, 0, 0, 0 }
	};
#endif

	bool quiet = false;
	bool kernel_mode = false;
	
	int c;
	while((c = getopt_long(argc, argv, "qhVk", longopts, 0)) != -1) {

		switch(c) {
                case 'q':
			quiet = true;
			break;
		case 'h':
			std::cout << usage_text;
			exit(0);
			break;
		case 'V':
			std::cout <<
PACKAGE " " VERSION "\n"
"\n"
"Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet Erlangen-Nuernberg,\n"
"1997-2002 Johannes Gutenberg-Universitaet Mainz.\n"
"\n"
"This is free software; you can redistribute it and/or modify it under the\n"
"terms of the GNU General Public License; either version 2 of the License,\n"
"or any later version.\n"
"\n"
"Written by Stephan Endrass, Hans Huelf, Ruediger Oertel, Ralf Schmitt,\n"
"Kai Schneider and Johannes Beigel.\n";

			exit(0);
			break;
		case 'k':
			kernel_mode = true;
			break;
		default:
			std::cerr << "Unknown option!\n";
			std::cerr << usage_text;
			exit(1);
		}
	}

        IO::init(quiet, kernel_mode);
	Script::init();

	if(kernel_mode) {
		Script::kernel();
	} else {
		if(optind == argc) {
			Script::executeScriptFromStdin();
		} else {
			for(int i = optind; i < argc; ++i) {
				if(!quiet) {
					std::cerr << "Processing file " << argv[i] << "\n";
				}
				Script::executeScriptFromFile(argv[i]);
			}
		}
	}
	
	Script::deinit();
	
	return 0;
}
