/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#include <Misc.h>

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include<iostream>

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
