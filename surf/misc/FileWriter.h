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


#ifndef FILEWRITER_H
#define FILEWRITER_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include<assert.h>
#include<cstdio>

class FileWriter
{
private:
	FileWriter (const FileWriter &);
	void operator=(const FileWriter &);

public:
	FileWriter(const char* str = 0) : name(str), f(0) {}
	~FileWriter() {
		if(!f) {
			return;
		}
		if(name[0]=='|') {
			std::pclose(f);
		} else {
			std::fclose(f);
		}
	}

	FILE* openFile(const char* str = 0) {
		if(f) {
			return f;
		}
		if(str) {
			name = str;
		}
		assert(name);
		if (name[0] == '|') {
			f = std::popen(name + 1, "w");
		} else {
			f = std::fopen(name, "w");
		}
		return f;
	}

	const char* getName() {
		assert(name);
		return name[0]=='|' ? "surf_picture" : name; 				
	}

	bool isWritingToPipe() {
		assert(name);
		return name[0]=='|';
	}

private:
	const char* name;
	FILE* f;
};

#endif
