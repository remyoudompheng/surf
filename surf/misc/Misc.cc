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


#include <Misc.h>
#include <Script.h>

#include <errno.h>
#include<iostream>

namespace {
	std::string default_action = "Progress";
	std::string action = default_action;
}

void Misc::syscall_failed(const std::string& txt, bool fatal)
{
	std::string errstr = txt + " failed: " + strerror(errno);
	print_error(errstr, fatal);
}

void Misc::print_error(const std::string& txt, bool fatal)
{
	std::cerr << "ERROR: " << txt << "\n";
	if(fatal) {
		abort();
	}
}

void Misc::print_warning(const std::string& txt)
{
	std::cerr << "WARNING: " << txt << "\n";
}

void Misc::progress(const std::string& _action)
{
	action = _action;
	if(!Script::isQuiet()) {
		if(Script::stdout_is_tty()) {
			std::cout << action << ": ";
		} else {
			std::cout << "status " << action << '\n';
		}
		std::cout.flush();
	}
}

void Misc::progress(int percent)
{
	if(!Script::isQuiet()) {
		if(Script::stdout_is_tty()) {
			std::cout << '\r' << action << ": " << percent << '%';
		} else {
			std::cout << "progress " << percent << '\n';
		}
		std::cout.flush();
	}
}

void Misc::progress(SpecialProgressType type)
{
	if(Script::isQuiet()) {
		return;
	}

	if(Script::stdout_is_tty()) {
		std::cout << '\r' << action << ": ";
	} else {
		std::cout << "progress ";
	}
	
	switch(type) {
	case done:
		std::cout << "done\n";
		break;
	case aborted:
		std::cout << "aborted\n";
		break;
	}
	action = default_action;
	std::cout.flush();
}
