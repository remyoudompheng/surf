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


#include <IO.h>

#include <errno.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include<iostream>

#ifdef HAVE_LIBREADLINE
#  if defined(HAVE_READLINE_READLINE_H)
#    include <readline/readline.h>
#  elif defined(HAVE_READLINE_H)
#    include <readline.h>
#  else
     extern char* readline();
#  endif
#endif
#ifdef HAVE_READLINE_HISTORY
#  if defined(HAVE_READLINE_HISTORY_H)
#    include <readline/history.h>
#  elif defined(HAVE_HISTORY_H)
#    include <history.h>
#  else
     extern void add_history();
     extern int write_history();
     extern int read_history();
#  endif
#endif

namespace {

const char *PROMPT = "-> ";

bool quiet;
bool kernel_mode;
bool stdin_is_a_tty;
bool stdout_is_a_tty;
bool interactive;

std::string default_action = "Progress";
std::string action = default_action;

}

namespace IO {

void init(bool _quiet, bool _kernel_mode)
{
	quiet = _quiet;
	kernel_mode = _kernel_mode;

	stdin_is_a_tty = isatty(STDIN_FILENO);
	stdout_is_a_tty = isatty(STDOUT_FILENO);
	interactive = stdin_is_a_tty && stdout_is_a_tty;
}

void print(const std::string& txt)
{
	std::cout << txt;
	std::cout.flush();
}

void print(int i)
{
	std::cout << i;
	std::cout.flush();
}

void print(double d)
{
	std::cout << d;
	std::cout.flush();
}

void print_banner()
{
	if(kernel_mode) {
		std::cout << PACKAGE " " VERSION "\n";
	} else if(stdout_is_a_tty && !quiet) {
		std::cout <<
"                      _____\n"
"  ________ __________/ ____\\\n"
" /  ___/  |  \\_  __ \\   __\\\n"
" \\___ \\|  |  /|  | \\/|  |\n"
"/____  >____/ |__|   |__| v" VERSION "\n"
"     \\/\n\n";
	}
	std::cout.flush();
}

bool getline(std::string& line)
{
	if(!kernel_mode && interactive) {
#ifdef HAVE_LIBREADLINE
		rl_bind_key('\t', rl_insert);
		rl_bind_key('^', rl_insert);
		char* l = readline(PROMPT);
		if(l) {
			if(*l) {
				add_history(l);
				line = l;
				free(l);
				return true;
			}
			free(l);
		}
#else
		std::cout << PROMPT;
		std::cout.flush();
		if(std::getline(std::cin, line)) {
			if(line.length() > 0) {
				return true;
			}
		}
#endif		
	} else {
		if(std::getline(std::cin, line)) {
			if(line.length() > 0) {
				return true;
			}
		}
	}

	return false;
}

void syscall_failed(const std::string& txt, bool fatal)
{
	std::string errstr = txt + " failed: " + strerror(errno);
	print_error(errstr, fatal);
}

void print_error(const std::string& txt, bool fatal)
{
	if(kernel_mode) {
		std::cout << "error " << txt << std::endl;
		std::cout.flush();
	} else {
		std::cerr << "ERROR: " << txt << std::endl;
	}
	if(fatal) {
		abort();
	}
}

void print_warning(const std::string& txt)
{
	if(quiet) {
		return;
	}
	if(kernel_mode) {
		std::cout << "warning " << txt << std::endl;
		std::cout.flush();
	} else {
		std::cerr << "WARNING: " << txt << std::endl;
	}
}

void kernel_msg(const std::string& txt)
{
	if(!kernel_mode) {
		return;
	}
	std::cout << txt;
	std::cout.flush();
}

void syntax_error(const std::string& txt, int line_number, int error_begin_char, int char_number)
{
	if(kernel_mode) {
		std::cout << "syntaxerror " << error_begin_char << ' ' << char_number << std::endl;
		std::cout.flush();
	} else if(stdin_is_a_tty) {
		std::cerr << "Syntax error!" << std::endl;
	} else {
		std::cerr << "Syntax error in line " << line_number << ": " << txt << std::endl;
	}
}

void IO::progress(const std::string& _action)
{
	action = _action;
	if(kernel_mode) {
		std::cout << "status " << action << std::endl;
	} else if(stdout_is_a_tty && !quiet) {
		std::cout << action << ": ";
	}
	std::cout.flush();
}

void IO::progress(int percent)
{
	if(kernel_mode) {
		std::cout << "progress " << percent << std::endl;
	} else if(stdout_is_a_tty && !quiet) {
		std::cout << '\r' << action << ": " << percent << '%';
	}
	std::cout.flush();
}

void IO::progress(SpecialProgressType type)
{
	if(kernel_mode) {
		std::cout << "progress ";
	} else if(stdout_is_a_tty) {
		if(quiet) {
			return;
		}
		std::cout << '\r' << action << ": ";
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

bool is_kernel_mode()
{
	return kernel_mode;
}

} // namespace IO
