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



#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <iostream.h>

#define TIMER(s) Timer aTimer(s)
class Timer
{
public:
	Timer(const char *str) {name=str; start=clock();};
	Timer(clock_t &ac) {aClock = &ac; name=0; start=clock();};

	~Timer()
		{
			clock_t end = clock();
			if (name) {
				cout << "Time spend in " << name << ":" << end-start << "=" 
				     << (float)(end-start) / CLOCKS_PER_SEC << " s" << endl;
			} else if (aClock) {
				*aClock += end-start;
			}
		};

protected:
	const char *name;
	clock_t start;

	clock_t *aClock;

};

#endif
