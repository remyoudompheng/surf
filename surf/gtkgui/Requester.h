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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */

#ifndef REQUESTER_H
#define REQUESTER_H



extern void showAlert (const char *alert);
// it's safe to call showAlert from non gui threads



extern int showRequester (const char *message, 
			  const char *def=0, 
			  const char *alt1=0, 
			  const char *alt2=0);
// Pops up a modal dialog showing the message
// User can choose between the 3 alternatives
// return value is 0 for default choice, 1 for first alternative, ...


// Consider these two functions to be private...
extern bool haveRequester ();
extern void raiseRequester();


// Use this macro in the delete_event callback to prevent windows from closing 
// when a requester is active
// it's a bit ugly that we have to do such things. I consider this a bug in gtk.

#define DELETE_EVENT_CHECK_REQUESTER if (haveRequester()) {raiseRequester(); return TRUE;}

#endif
