/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef MISC_H
#define MISC_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include<string>

namespace Misc {
	void syscall_failed(const std::string& txt, bool fatal = true);
	void print_error(const std::string& txt, bool fatal = true);
	void print_warning(const std::string& txt);
};

#endif //!MISC_H
